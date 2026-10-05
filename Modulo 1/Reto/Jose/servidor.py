import socket

# Crear socket UDP en la interfaz local (loopback)
servidor = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
servidor.bind(('127.0.0.1', 5000))

print("Servidor escuchando estímulos en localhost:5000...")

while True:
    # Espera a recibir datos (bloqueante)
    datos, direccion = servidor.recvfrom(1024)
    # Responde inmediatamente
    servidor.sendto(b"respuesta", direccion)