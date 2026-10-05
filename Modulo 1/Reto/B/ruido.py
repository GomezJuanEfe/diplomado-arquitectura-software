import time

N = 100_000
muestras = [0] * N  # reservamos la lista antes, para no medir el costo de hacerla crecer

for i in range(N):
  t0 = time.perf_counter_ns()
  t1 = time.perf_counter_ns()
  muestras[i] = t1 - t0 # ¿cuánto tiempo pasó entre dos preguntas seguidas?

muestras.sort()
print("mínimo: ", muestras[0], "ns")
print("p50:    ", muestras[N // 2], "ns")
print("p99:    ", muestras[int(N * 0.99)], "ns")
print("máximo: ", muestras[-1], "ns")