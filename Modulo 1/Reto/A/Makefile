# Makefile del reto de latencia mínima (Módulo 1).
#
# Targets:
#   make all       -> compila las dos variantes (UDP y memoria compartida).
#   make run-udp   -> corre servidor + cliente UDP y guarda resultados.
#   make run-shm   -> corre servidor + cliente de memoria compartida.
#   make clean     -> borra binarios y resultados generados.
#
# Flags pedidos por el enunciado:
#   -O2 -Wall     : optimización y todos los warnings del compilador.
#   -lrt -pthread : -lrt para shm_open/clock_gettime en sistemas donde no
#                   están en libc directamente, -pthread porque los
#                   atomics de <stdatomic.h> en la variante shm se apoyan
#                   en primitivas de threading del sistema.

CC      = gcc
# -D_GNU_SOURCE: habilita CPU_ZERO/CPU_SET/sched_setaffinity (extensiones
# de glibc, no del estándar C). Debe ir como flag de compilación y no
# como #define dentro de un header, porque los feature-test-macros de
# glibc solo tienen efecto si están definidos ANTES del primer header del
# sistema que se incluya en cada archivo .c.
CFLAGS  = -O2 -Wall -D_GNU_SOURCE
LDFLAGS = -lrt -pthread

BIN_DIR     = bin
RESULTS_DIR = results

.PHONY: all run-udp run-shm clean

all: $(BIN_DIR)/server_udp $(BIN_DIR)/client_udp \
     $(BIN_DIR)/server_shm $(BIN_DIR)/client_shm

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# ---- Variante UDP (baseline) ----------------------------------------------

$(BIN_DIR)/server_udp: udp/server_udp.c common/common.h | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ udp/server_udp.c $(LDFLAGS)

$(BIN_DIR)/client_udp: udp/client_udp.c common/common.h | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ udp/client_udp.c $(LDFLAGS)

run-udp: $(BIN_DIR)/server_udp $(BIN_DIR)/client_udp | $(RESULTS_DIR)
	@echo ">> Levantando servidor UDP en segundo plano..."
	./$(BIN_DIR)/server_udp 9001 & \
	SERVER_PID=$$!; \
	sleep 0.3; \
	echo ">> Corriendo cliente UDP (100000 iteraciones)..."; \
	./$(BIN_DIR)/client_udp 127.0.0.1 9001; \
	wait $$SERVER_PID

# ---- Variante memoria compartida (optimizada) ------------------------------

$(BIN_DIR)/server_shm: shm/server_shm.c shm/shm_common.h common/common.h | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ shm/server_shm.c $(LDFLAGS)

$(BIN_DIR)/client_shm: shm/client_shm.c shm/shm_common.h common/common.h | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ shm/client_shm.c $(LDFLAGS)

run-shm: $(BIN_DIR)/server_shm $(BIN_DIR)/client_shm | $(RESULTS_DIR)
	@echo ">> Levantando servidor de memoria compartida en segundo plano..."
	./$(BIN_DIR)/server_shm & \
	SERVER_PID=$$!; \
	sleep 0.3; \
	echo ">> Corriendo cliente de memoria compartida (100000 iteraciones)..."; \
	./$(BIN_DIR)/client_shm; \
	wait $$SERVER_PID

# ---- Servidor web de comparación (no crítico, fuera de la medición) -------

run-web:
	python3 web/server.py

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(RESULTS_DIR)
	rm -f /dev/shm/reto_latencia_shm
