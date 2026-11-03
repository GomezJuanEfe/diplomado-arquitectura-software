#!/usr/bin/env python3
"""Panel local para lanzar la prueba k6 y seguir el sistema en tiempo real.

Es una herramienta de demostracion, no parte de la solucion desplegada en AWS.
Usa solo la libreria estandar y la CLI `aws`, con las credenciales de quien lo
ejecuta. Escucha unicamente en 127.0.0.1.
"""

import json
import os
import re
import shutil
import subprocess
import tempfile
import threading
import time
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

REGION = os.environ.get("AWS_REGION", "us-east-1")
PUERTO = int(os.environ.get("PANEL_PUERTO", "8091"))
DIR_PANEL = Path(__file__).resolve().parent
DIR_RETO = DIR_PANEL.parent
SCRIPT_K6 = DIR_RETO / "K6 script - reto3.js"
HOSTS_PERMITIDOS = {f"127.0.0.1:{PUERTO}", f"localhost:{PUERTO}"}
PAGINAS = {"/": "index.html", "/slides": "slides.html"}

TABLA = "reto3-estado"
ALARMA_L3 = "reto3-recupera-l3"
ALARMA_L2 = "reto3-recupera-l2"
LOG_AUDITOR = "/aws/lambda/reto3-auditor"

SEGUNDOS_ESTADO = 2
SEGUNDOS_ALARMAS = 3
SEGUNDOS_TRANSICIONES = 5
TIMEOUT_AWS = 8
SEGUNDOS_CIERRE_K6 = 3

LINEA_K6 = re.compile(
    r"Minute: (\d+), Iteration: (\d+), ErrorParam: (true|false), Status: (\d+), Body: (.*)",
    re.DOTALL,
)
ESPERA_K6 = re.compile(r"Esperando (\d+) segundos")


class Panel:
    """Estado compartido entre el proceso k6, los lectores de AWS y los navegadores."""

    def __init__(self):
        self.condicion = threading.Condition()
        self.generacion = 0
        self.eventos = []
        self.estado = {"nivel": None, "errores": None, "alarmas": {}, "error_aws": None}
        self.version_estado = 0
        self.proceso = None
        self.activa = False
        self.inicio_ms = None
        self.clientes = 0
        self.transiciones_vistas = set()

    def emitir(self, tipo, **datos):
        with self.condicion:
            self.eventos.append({"tipo": tipo, **datos})
            self.condicion.notify_all()

    def actualizar_estado(self, **campos):
        with self.condicion:
            self.estado.update(campos)
            self.version_estado += 1
            self.condicion.notify_all()

    def corriendo(self):
        return self.activa

    def motivo_bloqueo(self):
        """Devuelve por que no se puede iniciar, o None si se puede."""
        if self.corriendo():
            return "Ya hay una prueba en curso"
        if self.estado.get("nivel") is None:
            return "Aún no se puede leer el estado en AWS"
        if self.estado["nivel"] != 1 or self.estado["errores"] != 0:
            return "El sistema aún no vuelve a nivel 1 con 0 errores"
        return None

    def foto_estado(self):
        return {
            **self.estado,
            "corriendo": self.corriendo(),
            "bloqueo": self.motivo_bloqueo(),
        }


panel = Panel()


def aws_json(*argumentos):
    resultado = subprocess.run(
        ["aws", *argumentos, "--region", REGION, "--output", "json"],
        capture_output=True, text=True, timeout=TIMEOUT_AWS, check=True,
    )
    return json.loads(resultado.stdout)


def leer_estado():
    item = aws_json(
        "dynamodb", "get-item", "--table-name", TABLA,
        "--key", '{"id":{"S":"estado"}}', "--consistent-read",
    )["Item"]
    panel.actualizar_estado(
        nivel=int(item["nivel"]["N"]),
        errores=int(item["errores"]["N"]),
        ultimo_error=item.get("ultimo_error", {}).get("S"),
        leido_en=datetime.now(timezone.utc).isoformat(),
        error_aws=None,
    )


def leer_alarmas():
    respuesta = aws_json(
        "cloudwatch", "describe-alarms", "--alarm-names", ALARMA_L3, ALARMA_L2,
    )
    alarmas = {a["AlarmName"]: a["StateValue"] for a in respuesta["MetricAlarms"]}
    panel.actualizar_estado(alarmas=alarmas)


def leer_transiciones():
    if panel.inicio_ms is None:
        return
    respuesta = aws_json(
        "logs", "filter-log-events", "--log-group-name", LOG_AUDITOR,
        "--start-time", str(panel.inicio_ms), "--filter-pattern", '"NIVEL_CAMBIADO"',
    )
    for evento in respuesta["events"]:
        if evento["eventId"] in panel.transiciones_vistas:
            continue
        panel.transiciones_vistas.add(evento["eventId"])
        panel.emitir("transicion", **json.loads(evento["message"]))


def lector_periodico(funcion, segundos):
    """Consulta AWS mientras alguien mira el panel o corre una prueba."""
    while True:
        if panel.clientes > 0 or panel.corriendo():
            try:
                funcion()
            except (subprocess.SubprocessError, ValueError, KeyError) as error:
                panel.actualizar_estado(error_aws=f"{funcion.__name__}: {type(error).__name__}")
        time.sleep(segundos)


def procesar_log_k6(linea):
    try:
        mensaje = json.loads(linea)["msg"]
    except (ValueError, KeyError, TypeError):
        return

    espera = ESPERA_K6.search(mensaje)
    if espera:
        panel.emitir("espera", segundos=int(espera.group(1)))
        return

    peticion = LINEA_K6.search(mensaje)
    if not peticion:
        return
    minuto, iteracion, error, status, cuerpo = peticion.groups()
    try:
        respuesta = json.loads(cuerpo)
    except ValueError:
        respuesta = {"mensaje": cuerpo.strip()}
    panel.emitir(
        "peticion",
        minuto=int(minuto),
        iteracion=int(iteracion),
        error=error == "true",
        status=int(status),
        nivel=respuesta.get("nivel"),
        mensaje=respuesta.get("mensaje", ""),
        contador=panel.estado.get("errores"),
    )


def seguir_metricas(ruta, proceso):
    """Lee las latencias que k6 va escribiendo en su salida JSON."""
    numero = 0
    with open(ruta, encoding="utf-8") as archivo:
        while True:
            linea = archivo.readline()
            if not linea:
                if proceso.poll() is not None:
                    return
                time.sleep(0.2)
                continue
            if not linea.endswith("\n"):
                archivo.seek(archivo.tell() - len(linea.encode("utf-8")))
                time.sleep(0.1)
                continue
            try:
                dato = json.loads(linea)
            except ValueError:
                continue
            if dato.get("metric") == "http_req_duration" and dato.get("type") == "Point":
                numero += 1
                panel.emitir("latencia", numero=numero, ms=round(dato["data"]["value"], 1))


def ejecutar_k6():
    k6 = shutil.which("k6") or str(Path.home() / ".local/bin/k6")
    directorio = tempfile.mkdtemp(prefix="reto3-panel-")
    ruta_metricas = Path(directorio) / "metricas.json"
    ruta_metricas.touch()
    try:
        proceso = subprocess.Popen(
            [k6, "--log-format=json", "--quiet", "--no-color", "run",
             "--out", f"json={ruta_metricas}", SCRIPT_K6.name],
            cwd=DIR_RETO, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE, text=True,
        )
    except OSError as error:
        shutil.rmtree(directorio, ignore_errors=True)
        terminar_corrida(-1, f"No se pudo ejecutar k6: {error}")
        return
    panel.proceso = proceso
    lector = threading.Thread(target=seguir_metricas, args=(ruta_metricas, proceso), daemon=True)
    lector.start()

    for linea in proceso.stderr:
        procesar_log_k6(linea)

    codigo = proceso.wait()
    lector.join(timeout=5)
    shutil.rmtree(directorio, ignore_errors=True)
    terminar_corrida(codigo)


def terminar_corrida(codigo, detalle=""):
    with panel.condicion:
        panel.activa = False
        panel.proceso = None
        panel.version_estado += 1
    panel.emitir("fin", codigo=codigo, detalle=detalle)


def forzar_cierre(proceso):
    if proceso.poll() is None:
        proceso.kill()


def detener_prueba():
    proceso = panel.proceso
    if proceso is not None and proceso.poll() is None:
        proceso.terminate()
        # k6 deja de enviar peticiones, pero el init del script lo retiene hasta el segundo 00.
        threading.Timer(SEGUNDOS_CIERRE_K6, forzar_cierre, args=(proceso,)).start()


def iniciar_prueba():
    with panel.condicion:
        motivo = panel.motivo_bloqueo()
        if motivo:
            return motivo
        panel.generacion += 1
        panel.eventos = []
        panel.transiciones_vistas = set()
        panel.inicio_ms = int(time.time() * 1000)
        panel.activa = True
        panel.eventos.append({"tipo": "inicio", "inicio_ms": panel.inicio_ms})
        panel.condicion.notify_all()
    threading.Thread(target=ejecutar_k6, daemon=True).start()
    return None


class Manejador(BaseHTTPRequestHandler):
    def log_message(self, formato, *args):
        pass

    def _host_valido(self):
        return self.headers.get("Host") in HOSTS_PERMITIDOS

    def _json(self, cuerpo, codigo=200):
        datos = json.dumps(cuerpo).encode("utf-8")
        self.send_response(codigo)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(datos)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(datos)

    def do_GET(self):
        if not self._host_valido():
            self.send_error(403)
        elif self.path in PAGINAS:
            datos = (DIR_PANEL / PAGINAS[self.path]).read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(datos)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(datos)
        elif self.path == "/api/eventos":
            self._transmitir()
        else:
            self.send_error(404)

    def do_POST(self):
        # El encabezado propio impide que otra pagina lance pruebas desde el navegador.
        if not self._host_valido() or self.headers.get("X-Panel") != "1":
            self.send_error(403)
        elif self.path == "/api/iniciar":
            motivo = iniciar_prueba()
            self._json({"error": motivo} if motivo else {"ok": True}, 409 if motivo else 200)
        elif self.path == "/api/detener":
            detener_prueba()
            self._json({"ok": True})
        else:
            self.send_error(404)

    def _enviar(self, tipo, datos):
        carga = json.dumps(datos, ensure_ascii=False)
        self.wfile.write(f"event: {tipo}\ndata: {carga}\n\n".encode("utf-8"))
        self.wfile.flush()

    def _transmitir(self):
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream")
        self.send_header("Cache-Control", "no-store")
        self.end_headers()

        generacion = None
        indice = 0
        version = -1
        with panel.condicion:
            panel.clientes += 1
        try:
            while True:
                with panel.condicion:
                    panel.condicion.wait_for(
                        lambda: generacion != panel.generacion
                        or indice < len(panel.eventos)
                        or version != panel.version_estado,
                        timeout=15,
                    )
                    reinicio = generacion != panel.generacion
                    if reinicio:
                        generacion = panel.generacion
                        indice = 0
                    nuevos = panel.eventos[indice:]
                    indice += len(nuevos)
                    cambio_estado = version != panel.version_estado
                    version = panel.version_estado
                    foto = panel.foto_estado()

                if reinicio:
                    self._enviar("reinicio", {})
                for evento in nuevos:
                    self._enviar(evento["tipo"], evento)
                if cambio_estado:
                    self._enviar("estado", foto)
                # Comentario SSE: detecta navegadores cerrados y mantiene viva la conexion.
                self.wfile.write(b": latido\n\n")
                self.wfile.flush()
        except (BrokenPipeError, ConnectionResetError):
            pass
        finally:
            with panel.condicion:
                panel.clientes -= 1


def main():
    for funcion, segundos in (
        (leer_estado, SEGUNDOS_ESTADO),
        (leer_alarmas, SEGUNDOS_ALARMAS),
        (leer_transiciones, SEGUNDOS_TRANSICIONES),
    ):
        threading.Thread(target=lector_periodico, args=(funcion, segundos), daemon=True).start()

    servidor = ThreadingHTTPServer(("127.0.0.1", PUERTO), Manejador)
    print(f"Panel en http://127.0.0.1:{PUERTO}")
    try:
        servidor.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        detener_prueba()


if __name__ == "__main__":
    main()
