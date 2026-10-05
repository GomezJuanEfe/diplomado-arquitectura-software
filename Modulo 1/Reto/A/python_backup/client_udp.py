#!/usr/bin/env python3
"""
client_udp.py
-------------
Respaldo en Python puro del cliente de medición UDP. Mismo protocolo y
misma metodología de medición que client_udp.c:
  - 100000 iteraciones, se descartan las primeras 10000 como calentamiento.
  - Reloj monotónico (time.monotonic_ns(), el equivalente en Python de
    clock_gettime(CLOCK_MONOTONIC) en C: nunca time.time() ni datetime,
    porque esos pueden saltar con ajustes del reloj del sistema).
  - Cero I/O dentro del bucle: las muestras se guardan en una lista
    preasignada y los archivos se escriben recién al final.
"""

import socket
import struct
import sys
import time
import json

PUERTO_DEFECTO = 9001
CENTINELA_PARADA = 2**64 - 1

TOTAL_ITERACIONES = 100_000
ITERACIONES_CALENTAMIENTO = 10_000
MUESTRAS_VALIDAS = TOTAL_ITERACIONES - ITERACIONES_CALENTAMIENTO


def calcular_estadisticas(muestras_ordenadas):
    """Recibe una lista YA ORDENADA de latencias en nanosegundos y
    devuelve un diccionario con min, max, percentiles y promedio."""
    n = len(muestras_ordenadas)

    def percentil(p):
        idx = int(p * n)
        if idx >= n:
            idx = n - 1
        return muestras_ordenadas[idx]

    return {
        "variante": "udp_loopback_python",
        "muestras": n,
        "min_ns": muestras_ordenadas[0],
        "p50_ns": percentil(0.50),
        "p99_ns": percentil(0.99),
        "p999_ns": percentil(0.999),
        "max_ns": muestras_ordenadas[-1],
        "promedio_ns": sum(muestras_ordenadas) / n,
    }


def main():
    ip_servidor = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
    puerto = int(sys.argv[2]) if len(sys.argv) > 2 else PUERTO_DEFECTO

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # connect() en UDP no abre conexión real: solo fija el destino por
    # defecto, permitiendo usar send()/recv() en vez de sendto()/recvfrom().
    sock.connect((ip_servidor, puerto))

    print(f"[client_udp.py] conectado a {ip_servidor}:{puerto}")
    print(f"[client_udp.py] ejecutando {TOTAL_ITERACIONES} iteraciones "
          f"({ITERACIONES_CALENTAMIENTO} de calentamiento)...")

    # Lista preasignada (llena de ceros) para no reasignar memoria dentro
    # del bucle de medición.
    latencias_ns = [0.0] * MUESTRAS_VALIDAS

    for i in range(TOTAL_ITERACIONES):
        # Empaquetamos el contador como 8 bytes little-endian, igual
        # formato que espera el servidor en C o en Python.
        estimulo = struct.pack("<Q", i)

        t_inicio = time.monotonic_ns()
        sock.send(estimulo)
        sock.recv(64)
        t_fin = time.monotonic_ns()

        if i >= ITERACIONES_CALENTAMIENTO:
            latencias_ns[i - ITERACIONES_CALENTAMIENTO] = float(t_fin - t_inicio)

    # Avisamos al servidor que ya terminamos.
    sock.send(struct.pack("<Q", CENTINELA_PARADA))
    sock.close()

    latencias_ns.sort()
    stats = calcular_estadisticas(latencias_ns)

    print(f"\n==== Resultados: {stats['variante']} ====")
    print(f"Muestras validas : {stats['muestras']}")
    print(f"Minimo   : {stats['min_ns']:10.2f} ns")
    print(f"p50      : {stats['p50_ns']:10.2f} ns")
    print(f"p99      : {stats['p99_ns']:10.2f} ns")
    print(f"p99.9    : {stats['p999_ns']:10.2f} ns")
    print(f"Maximo   : {stats['max_ns']:10.2f} ns")
    print(f"Promedio : {stats['promedio_ns']:10.2f} ns\n")

    with open("results/latencia.log", "a") as flog:
        flog.write(f"==== Variante: {stats['variante']} ====\n")
        flog.write(f"Muestras validas : {stats['muestras']}\n")
        flog.write(f"Minimo   (ns)    : {stats['min_ns']:.2f}\n")
        flog.write(f"p50      (ns)    : {stats['p50_ns']:.2f}\n")
        flog.write(f"p99      (ns)    : {stats['p99_ns']:.2f}\n")
        flog.write(f"p99.9    (ns)    : {stats['p999_ns']:.2f}\n")
        flog.write(f"Maximo   (ns)    : {stats['max_ns']:.2f}\n")
        flog.write(f"Promedio (ns)    : {stats['promedio_ns']:.2f}\n\n")

    with open("results/resultados_udp_python.json", "w") as fjson:
        json.dump(stats, fjson, indent=2)
        fjson.write("\n")


if __name__ == "__main__":
    main()
