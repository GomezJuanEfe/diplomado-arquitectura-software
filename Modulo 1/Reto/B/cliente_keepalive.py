import time, http.client

N = 2_000
muestras = [0] * N

conn = http.client.HTTPConnection("127.0.0.1", 8080)
conn.request("GET", "/"); conn.getresponse().read()   # una vez, fuera del cronómetro

for i in range(N):
    t0 = time.perf_counter_ns()
    conn.request("GET", "/")
    conn.getresponse().read()
    t1 = time.perf_counter_ns()
    muestras[i] = t1 - t0

with open("keepalive.log", "w") as f:
    for m in muestras:
        f.write(f"{m}\n")

muestras.sort()
us = lambda x: round(x / 1000, 1)
print("mínimo:", us(muestras[0]), "us")
print("p50:   ", us(muestras[N // 2]), "us")
print("p99:   ", us(muestras[int(N * 0.99)]), "us")
print("máximo:", us(muestras[-1]), "us")