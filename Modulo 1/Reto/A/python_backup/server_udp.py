#!/usr/bin/env python3
"""
server_udp.py
-------------
Respaldo en Python puro (solo librería estándar) de la variante UDP,
por si algo impide compilar/correr la versión en C durante la
sustentación. Implementa el mismo protocolo de eco sobre loopback.

No se espera que iguale la latencia de la versión en C: el intérprete de
Python agrega su propia sobrecarga (gestión del GIL, boxing de objetos,
etc.). Sirve como demostración conceptual y como plan B funcional.
"""

import socket   # sockets UDP, igual concepto que en C
import struct   # para empaquetar/desempaquetar el contador como 8 bytes
import sys      # leer argumentos de línea de comandos

PUERTO_DEFECTO = 9001
# Mismo valor centinela que en la versión en C (2**64 - 1) para pedirle
# al servidor que cierre su bucle.
CENTINELA_PARADA = 2**64 - 1


def main():
    # Si se pasó un puerto como argumento, se usa ese; si no, el de defecto.
    puerto = int(sys.argv[1]) if len(sys.argv) > 1 else PUERTO_DEFECTO

    # socket.AF_INET = IPv4, socket.SOCK_DGRAM = UDP (sin conexión).
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Igual que en C: forzamos loopback (127.0.0.1), nunca una IP externa.
    sock.bind(("127.0.0.1", puerto))

    print(f"[server_udp.py] escuchando en 127.0.0.1:{puerto}")

    while True:
        # recvfrom bloquea hasta que llega un datagrama; devuelve los
        # bytes recibidos y la dirección (ip, puerto) del remitente.
        datos, direccion_cliente = sock.recvfrom(64)

        # Interpretamos los primeros 8 bytes como un entero sin signo de
        # 64 bits en formato little-endian ("<Q"), igual que en C.
        (valor,) = struct.unpack("<Q", datos[:8])

        if valor == CENTINELA_PARADA:
            print("[server_udp.py] senal de parada recibida, cerrando.")
            break

        # Eco: se devuelve exactamente lo mismo que llegó.
        sock.sendto(datos, direccion_cliente)

    sock.close()


if __name__ == "__main__":
    main()
