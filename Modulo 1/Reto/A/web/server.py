#!/usr/bin/env python3
"""
server.py
---------
Servidor web MÍNIMO del PLANO DE CONTROL (no crítico, fuera de la
medición de latencia). Su único trabajo es:
  1) Servir la página index.html con el navegador.
  2) Exponer /api/resultados: lee los JSON que generaron los clientes en
     C (results/resultados_udp.json y results/resultados_shm.json) y los
     combina en una sola respuesta JSON.
  3) Exponer /api/ping: responde lo antes posible, para que el propio
     navegador pueda medir, con fetch(), un round-trip HTTP real como
     "grupo de control" y así evidenciar la diferencia de órdenes de
     magnitud frente a UDP crudo y memoria compartida.

Usa SOLO la librería estándar de Python (http.server, json, pathlib):
nada de Flask, FastAPI ni ningún paquete externo, tal como pide el reto.
"""

import json
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

# Directorio donde vive este script (reto/web/), y el directorio del
# reto completo (un nivel arriba), donde están results/ e index.html.
DIR_WEB = Path(__file__).resolve().parent
DIR_RETO = DIR_WEB.parent
DIR_RESULTS = DIR_RETO / "results"

PUERTO_DEFECTO = 8080


def leer_json_si_existe(ruta: Path):
    """Devuelve el contenido de un archivo JSON como dict, o None si el
    archivo todavía no existe (por ejemplo, si esa variante no se corrió
    todavía con `make run-udp` / `make run-shm`)."""
    if not ruta.exists():
        return None
    try:
        with open(ruta, "r") as f:
            return json.load(f)
    except (json.JSONDecodeError, OSError):
        return None


class ManejadorReto(BaseHTTPRequestHandler):
    # Silencia el log por defecto de línea por request para no ensuciar
    # la terminal durante la demo (se puede comentar esta línea si se
    # quiere ver el detalle de cada request).
    def log_message(self, formato, *args):
        pass

    def _responder_json(self, cuerpo: dict, codigo: int = 200):
        datos = json.dumps(cuerpo).encode("utf-8")
        self.send_response(codigo)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(datos)))
        # No cachear nunca las respuestas de la API: cada carga de página
        # debe reflejar la corrida más reciente de los benchmarks en C.
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(datos)

    def do_GET(self):
        if self.path == "/api/ping":
            # Ruta mínima: responder ya, sin tocar disco ni hacer nada
            # más. Este es el "trabajo" que el navegador va a cronometrar
            # con fetch() + performance.now() como grupo de control.
            self._responder_json({"ok": True})
            return

        if self.path == "/api/resultados":
            udp = leer_json_si_existe(DIR_RESULTS / "resultados_udp.json")
            shm = leer_json_si_existe(DIR_RESULTS / "resultados_shm.json")
            udp_py = leer_json_si_existe(
                DIR_RESULTS / "resultados_udp_python.json")

            combinado = {"udp": udp, "shm": shm, "udp_python": udp_py}

            # Además de responder, dejamos un resultados.json combinado
            # en disco: es uno de los entregables pedidos explícitamente.
            try:
                with open(DIR_RESULTS / "resultados.json", "w") as f:
                    json.dump(combinado, f, indent=2)
                    f.write("\n")
            except OSError:
                pass

            self._responder_json(combinado)
            return

        # Cualquier otra ruta GET: servir archivos estáticos desde web/
        # (en la práctica, solo index.html). Ruta simple y segura: no se
        # admite navegar a subcarpetas ni salir de DIR_WEB.
        ruta_pedida = "index.html" if self.path in ("/", "") else self.path.lstrip("/")
        archivo = (DIR_WEB / ruta_pedida).resolve()

        if DIR_WEB not in archivo.parents and archivo != DIR_WEB:
            self.send_error(403, "Prohibido")
            return
        if not archivo.is_file():
            self.send_error(404, "No encontrado")
            return

        tipo = "text/html; charset=utf-8" if archivo.suffix == ".html" else "application/octet-stream"
        cuerpo = archivo.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", tipo)
        self.send_header("Content-Length", str(len(cuerpo)))
        self.end_headers()
        self.wfile.write(cuerpo)


def main():
    import sys
    puerto = int(sys.argv[1]) if len(sys.argv) > 1 else PUERTO_DEFECTO
    servidor = ThreadingHTTPServer(("127.0.0.1", puerto), ManejadorReto)
    print(f"[web] plano de control (no critico) sirviendo en "
          f"http://127.0.0.1:{puerto}")
    print("[web] presiona Ctrl+C para detener")
    try:
        servidor.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        servidor.server_close()


if __name__ == "__main__":
    main()
