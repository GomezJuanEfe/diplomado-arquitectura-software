/*
 * client_shm.c
 * ------------
 * Cliente de medición de latencia para la variante de memoria compartida
 * (optimizada). Mismo protocolo de medición que client_udp.c (100000
 * iteraciones, 10000 de calentamiento, percentiles al final), pero el
 * mecanismo de comunicación es totalmente distinto: en vez de mandar
 * datagramas por la red, escribe y lee directamente en memoria compartida
 * con el servidor, usando espera activa (busy-wait) en vez de bloquear.
 *
 * Comentado línea por línea en español para sustentación oral.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <time.h>       /* nanosleep, para el reintento de apertura        */

#include "../common/common.h"
#include "shm_common.h"

/* Núcleo de CPU al que ancla el cliente. Distinto al del servidor para
 * que cada proceso tenga su propio núcleo físico dedicado. */
#define NUCLEO_CLIENTE 1

/* Cuántas veces reintentar abrir la memoria compartida si el servidor
 * todavía no la creó (esto pasa fuera del bucle medido, no afecta el
 * RTT). */
#define REINTENTOS_APERTURA 50

int main(int argc, char *argv[]) {
    const char *nombre_shm = (argc > 1) ? argv[1] : NOMBRE_SHM_DEFECTO;

    fijar_cpu(NUCLEO_CLIENTE);
    intentar_prioridad_tiempo_real();

    /*
     * A diferencia del servidor, el cliente NO usa O_CREAT: solo debe
     * ABRIR un objeto que el servidor ya haya creado. Si el servidor
     * todavía no arrancó, shm_open() falla con ENOENT; reintentamos unas
     * cuantas veces con una pequeña espera (esto es apenas al arrancar,
     * fuera del bucle de medición, así que no afecta el RTT medido).
     */
    int fd = -1;
    for (int intento = 0; intento < REINTENTOS_APERTURA; intento++) {
        fd = shm_open(nombre_shm, O_RDWR, 0666);
        if (fd >= 0) break;
        struct timespec espera = { .tv_sec = 0, .tv_nsec = 20 * 1000 * 1000 }; /* 20 ms */
        nanosleep(&espera, NULL);
    }
    if (fd < 0) {
        fprintf(stderr, "No se pudo abrir la memoria compartida '%s'. "
                        "¿Esta corriendo server_shm?\n", nombre_shm);
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    void *region = mmap(NULL, sizeof(canal_compartido_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    canal_compartido_t *canal = (canal_compartido_t *)region;

    printf("[client_shm] conectado al canal '%s' (nucleo %d)\n",
           nombre_shm, NUCLEO_CLIENTE);
    printf("[client_shm] ejecutando %d iteraciones (%d de calentamiento)...\n",
           TOTAL_ITERACIONES, ITERACIONES_CALENTAMIENTO);

    /* Arreglo preasignado para las muestras: igual razón que en
     * client_udp.c, cero asignaciones de memoria dentro del bucle. */
    double *latencias_ns = calloc(MUESTRAS_VALIDAS, sizeof(double));
    if (latencias_ns == NULL) {
        fprintf(stderr, "No se pudo reservar memoria para las muestras\n");
        exit(EXIT_FAILURE);
    }

    /*
     * Empezamos la secuencia en 1 (no en 0) a propósito: el servidor
     * inicializa el canal completo en cero, así que el valor 0 significa
     * "todavía no hay ningún mensaje". Si el primer estímulo real fuera
     * 0, el servidor no podría distinguirlo del estado inicial.
     */
    for (int i = 0; i < TOTAL_ITERACIONES; i++) {
        uint64_t secuencia = (uint64_t)(i + 1);

        struct timespec t_inicio, t_fin;
        clock_gettime(CLOCK_MONOTONIC, &t_inicio);

        /* Publicamos el estímulo. memory_order_release: todo lo que
         * escribimos antes de esta línea queda visible para el servidor
         * en cuanto él vea este nuevo valor (aquí no hay más datos, pero
         * es la forma correcta de sincronizar). */
        atomic_store_explicit(&canal->secuencia_cliente, secuencia,
                               memory_order_release);

        /* Espera activa hasta que el servidor haga eco de ESTA misma
         * secuencia (evita confundir una respuesta vieja con la nueva). */
        uint64_t respuesta;
        do {
            respuesta = atomic_load_explicit(&canal->secuencia_servidor,
                                              memory_order_acquire);
        } while (respuesta != secuencia);

        clock_gettime(CLOCK_MONOTONIC, &t_fin);

        if (i >= ITERACIONES_CALENTAMIENTO) {
            latencias_ns[i - ITERACIONES_CALENTAMIENTO] =
                diferencia_ns(t_inicio, t_fin);
        }
    }

    /* Avisamos al servidor que ya terminamos. */
    atomic_store_explicit(&canal->secuencia_cliente, CENTINELA_PARADA_SHM,
                           memory_order_release);

    /* El cliente desmapea pero NO borra (shm_unlink) el objeto: eso lo
     * hace el servidor, que fue quien lo creó y es quien sabe cuándo ya
     * nadie más lo va a necesitar. */
    munmap(region, sizeof(canal_compartido_t));

    qsort(latencias_ns, MUESTRAS_VALIDAS, sizeof(double), comparar_double);

    estadisticas_t stats = calcular_estadisticas(latencias_ns, MUESTRAS_VALIDAS);
    imprimir_resultados("shm_busywait", stats);
    escribir_resultados("shm_busywait", stats,
                         "results/latencia.log",
                         "results/resultados_shm.json");

    free(latencias_ns);
    return 0;
}
