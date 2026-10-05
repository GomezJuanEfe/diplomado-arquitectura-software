# Reto Módulo 1 — Latencia mínima estímulo → respuesta

Sistema operativo objetivo: Ubuntu/Linux (POSIX nativo). Probado en
`gcc 13.3.0` sobre `Linux x86_64`.

## Arquitectura en dos planos

- **Plano de datos (crítico):** los binarios en C bajo `udp/` y `shm/`.
  Corren en la misma máquina, sin HTTP, sin JSON, sin ninguna capa que no
  sea estrictamente necesaria. Es lo único que se mide.
- **Plano de control (no crítico):** el servidor web en `web/`, escrito en
  Python con la librería estándar. Sirve para *visualizar* los resultados
  después de que la medición ya terminó. Nunca participa del RTT medido.

Esta separación es intencional: mezclar HTTP/JSON dentro del bucle de
medición habría hecho imposible ver la diferencia real entre técnicas de
comunicación, que es justamente el punto del reto.

## Estructura de carpetas

```
reto/
├── Makefile
├── common/common.h         # timing, estadísticas, afinidad de CPU (compartido)
├── udp/                    # variante baseline: eco UDP sobre loopback
├── shm/                    # variante optimizada: memoria compartida + busy-wait
├── python_backup/          # respaldo en Python puro de la variante UDP
├── web/                    # plano de control: servidor + página comparativa
└── results/                # generado en tiempo de ejecución (logs y JSON)
```

## Compilar y correr

```bash
make all        # compila las 4 binarios (server/client × udp/shm)
make run-udp    # levanta servidor UDP, corre el cliente, guarda resultados
make run-shm    # levanta servidor de memoria compartida, corre el cliente
make run-web    # sirve la comparación visual en http://127.0.0.1:8080
make clean      # borra binarios, resultados y el objeto de memoria compartida
```

Cada `run-*` ejecuta 100 000 iteraciones estímulo→respuesta, descarta las
primeras 10 000 como calentamiento, y escribe:

- `results/latencia.log` — texto plano, se va acumulando entre corridas.
- `results/resultados_udp.json` / `resultados_shm.json` — un archivo por
  variante, sobrescrito en cada corrida.
- `results/resultados.json` — combinado, generado por el servidor web al
  responder `/api/resultados` (también es un entregable en sí mismo).

Respaldo en Python puro (por si `gcc` no está disponible o algo falla):

```bash
python3 python_backup/server_udp.py 9002 &
python3 python_backup/client_udp.py 127.0.0.1 9002
```

## Resultados de referencia (16 núcleos, gcc -O2, sin privilegios de root)

| Variante                  | p50       | p99      | p99.9    | min      | max       |
|----------------------------|-----------|----------|----------|----------|-----------|
| Memoria compartida (C)      | 249 ns    | 355 ns   | 4.16 µs  | 197 ns   | 16.84 µs  |
| UDP loopback (C)            | 12.37 µs  | 45.01 µs | 66.19 µs | 8.72 µs  | 515.43 µs |
| UDP loopback (Python)       | 13.84 µs  | 33.88 µs | 57.15 µs | 12.29 µs | 1.84 ms   |
| fetch() navegador (control) | 4.30 ms   | —        | —        | 2.90 ms  | 7.70 ms   |

La memoria compartida es ~50× más rápida que UDP crudo, y UDP crudo es a
su vez ~350× más rápido que un `fetch()` del navegador al mismo
`localhost`. Sin privilegios de root no se pudo activar `SCHED_FIFO`
(tiempo real); con `sudo` los números de cola (p99, p99.9, max) deberían
mejorar porque el planificador dejaría de interrumpir el proceso.

## Por qué cada optimización, explicado para la sustentación

- **`CLOCK_MONOTONIC`** (`common.h`): es el único reloj que garantiza
  avanzar siempre hacia adelante a ritmo constante. `time()` solo tiene
  resolución de 1 segundo; el reloj de pared puede saltar por ajustes de
  NTP y arruinar una resta de tiempos.
- **`sched_setaffinity` (afinidad de CPU):** clava cada proceso a un
  núcleo fijo para que el planificador no lo mueva de núcleo, lo que
  invalidaría su caché y metería ruido (jitter) en cada medición.
- **`SCHED_FIFO` (prioridad de tiempo real):** evita que el planificador
  interrumpa al proceso para dejar correr a otros. Requiere privilegios
  de root; si no los hay, el programa avisa por `stderr` y sigue en modo
  normal (es "best-effort", no crítico para que el programa funcione).
- **Buffers/estructuras alineadas a 64 bytes:** 64 bytes es el tamaño de
  una línea de caché en x86_64. En la variante de memoria compartida esto
  es crítico: separa la variable que escribe el cliente de la que escribe
  el servidor en líneas de caché distintas, evitando *false sharing*
  (que una escritura de un núcleo invalide innecesariamente la caché del
  otro núcleo). En la variante UDP se aplica por consistencia, pero al
  ser de un solo hilo no hay false sharing que evitar — se documenta esa
  diferencia directamente en los comentarios de `udp/*.c`.
- **`TCP_NODELAY`:** no aplica a ninguna de las dos variantes, porque
  ninguna usa TCP. Se documenta la decisión en `udp/server_udp.c` en vez
  de forzar una opción sin sentido.
- **Espera activa (busy-wait) vs. bloqueo:** UDP usa `recvfrom()`, que
  bloquea el proceso y deja que el kernel lo despierte cuando llega un
  paquete (ese despertar cuesta tiempo). La variante de memoria compartida
  usa un bucle que pregunta todo el tiempo "¿ya cambió el valor?", sin
  ceder el CPU — gasta 100% de un núcleo, pero elimina el costo de
  despertar al proceso. Es el trade-off central de esta variante.

## Notas

- Ambos servidores fuerzan explícitamente `127.0.0.1` (loopback): el
  plano de datos vive únicamente dentro de la misma máquina, tal como
  exige el enunciado.
- `make clean` también borra `/dev/shm/reto_latencia_shm` por si un
  cliente/servidor terminó de forma anómala y dejó el objeto de memoria
  compartida sin liberar.
