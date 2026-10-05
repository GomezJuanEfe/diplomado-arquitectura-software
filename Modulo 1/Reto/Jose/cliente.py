import socket
import time
import statistics

cliente = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
destino = ('127.0.0.1', 5000)

N = 2000
latencias_ms = []

for _ in range(N):
    # 1. Medir tiempo justo antes de enviar
    inicio = time.perf_counter()

    # 2. Enviar estímulo y recibir respuesta
    cliente.sendto(b"estimulo", destino)
    respuesta, _ = cliente.recvfrom(1024)

    # 3. Medir tiempo inmediatamente al recibir
    fin = time.perf_counter()

    latencias_ms.append((fin - inicio) * 1000)

cliente.close()

minimo = min(latencias_ms)
maximo = max(latencias_ms)
p50 = statistics.median(latencias_ms)
p99 = statistics.quantiles(latencias_ms, n=100)[98]

print(f"Respuesta del servidor: {respuesta.decode()}")
print(f"Muestras: {N}")
print(f"Latencia mínima: {minimo:.4f} ms")
print(f"Latencia p50: {p50:.4f} ms")
print(f"Latencia p99: {p99:.4f} ms")
print(f"Latencia máxima: {maximo:.4f} ms")