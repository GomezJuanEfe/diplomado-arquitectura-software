/*
 * client_udp.c
 * ------------
 * Cliente de medición de latencia para la variante UDP (baseline).
 *
 * Protocolo:
 *   1) Por cada iteración, envía un datagrama de 8 bytes con el número de
 *      iteración (el "estímulo").
 *   2) Espera a que el servidor lo devuelva igual (la "respuesta").
 *   3) Mide cuánto tiempo pasó entre el envío y la llegada de la
 *      respuesta: eso es el RTT (round-trip time, tiempo de ida y vuelta).
 *
 * Al terminar, envía un datagrama especial ("centinela") para avisarle al
 * servidor que cierre, y escribe las estadísticas a disco.
 *
 * Comentado línea por línea en español para sustentación oral.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../common/common.h"

#define PUERTO_DEFECTO 9001
#define CENTINELA_PARADA UINT64_MAX

/* Núcleo de CPU al que ancla el cliente. Distinto al del servidor (0)
 * para que cada proceso tenga su propio núcleo dedicado y no se estorben
 * ni compitan por la misma caché/tiempo de CPU. */
#define NUCLEO_CLIENTE 1

int main(int argc, char *argv[]) {
    /* Argumentos opcionales: IP del servidor y puerto. Por defecto
     * apuntamos a 127.0.0.1 (loopback), tal como exige el reto. */
    const char *ip_servidor = (argc > 1) ? argv[1] : "127.0.0.1";
    int puerto = (argc > 2) ? atoi(argv[2]) : PUERTO_DEFECTO;

    fijar_cpu(NUCLEO_CLIENTE);
    intentar_prioridad_tiempo_real();

    int fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in direccion_servidor;
    memset(&direccion_servidor, 0, sizeof(direccion_servidor));
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons((uint16_t)puerto);
    inet_pton(AF_INET, ip_servidor, &direccion_servidor.sin_addr);

    /*
     * connect() en un socket UDP NO abre una conexión de verdad (UDP no
     * tiene conexiones); lo que hace es "fijar" el destino por defecto
     * de este socket. Ventajas:
     *   - Podemos usar send()/recv() en vez de sendto()/recvfrom(), un
     *     poco más simples porque no repetimos la dirección en cada
     *     llamada.
     *   - El kernel filtra automáticamente paquetes que no vengan de esa
     *     IP:puerto exactos, evitando que otro proceso "hable" con
     *     nuestro socket por error durante la medición.
     */
    if (connect(fd_socket, (struct sockaddr *)&direccion_servidor,
                sizeof(direccion_servidor)) < 0) {
        perror("connect");
        close(fd_socket);
        exit(EXIT_FAILURE);
    }

    printf("[client_udp] conectado a %s:%d (nucleo %d)\n",
           ip_servidor, puerto, NUCLEO_CLIENTE);
    printf("[client_udp] ejecutando %d iteraciones (%d de calentamiento)...\n",
           TOTAL_ITERACIONES, ITERACIONES_CALENTAMIENTO);

    /*
     * Arreglo PREASIGNADO para guardar cada muestra de latencia (en
     * nanosegundos). Se reserva UNA vez, antes del bucle, precisamente
     * para que dentro del bucle no haya ninguna llamada a malloc() ni a
     * I/O (ambas son operaciones lentas e impredecibles que ensuciarían
     * la medición). "calloc" además inicializa la memoria en cero, lo
     * que fuerza a que el sistema operativo ya le asigne páginas físicas
     * reales ahora, y no la primera vez que se escribe dentro del bucle.
     */
    double *latencias_ns = calloc(MUESTRAS_VALIDAS, sizeof(double));
    if (latencias_ns == NULL) {
        fprintf(stderr, "No se pudo reservar memoria para las muestras\n");
        exit(EXIT_FAILURE);
    }

    /* Buffers alineados a 64 bytes (línea de caché), enviados y
     * recibidos tal cual. Igual que en el servidor, en un cliente de un
     * solo hilo esto no evita false sharing (no hay otro hilo con quien
     * compartir línea de caché), pero se deja alineado por consistencia
     * con la variante de memoria compartida, donde SÍ es crítico. */
    _Alignas(64) uint8_t buffer_envio[64];
    _Alignas(64) uint8_t buffer_recepcion[64];

    for (int i = 0; i < TOTAL_ITERACIONES; i++) {
        /* Preparamos el estímulo: los primeros 8 bytes son el contador. */
        uint64_t contador = (uint64_t)i;
        memcpy(buffer_envio, &contador, sizeof(contador));

        struct timespec t_inicio, t_fin;

        /* Marca de tiempo justo ANTES de enviar el estímulo. */
        clock_gettime(CLOCK_MONOTONIC, &t_inicio);

        /* send(): como ya hicimos connect(), no hace falta indicar la
         * dirección destino en cada llamada. */
        if (send(fd_socket, buffer_envio, sizeof(buffer_envio), 0) < 0) {
            perror("send");
            break;
        }

        /* recv() bloquea hasta que llegue la respuesta del servidor. */
        if (recv(fd_socket, buffer_recepcion, sizeof(buffer_recepcion), 0) < 0) {
            perror("recv");
            break;
        }

        /* Marca de tiempo justo DESPUÉS de recibir la respuesta. */
        clock_gettime(CLOCK_MONOTONIC, &t_fin);

        /* Solo guardamos la muestra si ya pasamos el calentamiento. */
        if (i >= ITERACIONES_CALENTAMIENTO) {
            latencias_ns[i - ITERACIONES_CALENTAMIENTO] =
                diferencia_ns(t_inicio, t_fin);
        }
    }

    /* Avisamos al servidor que ya terminamos, para que cierre su bucle. */
    uint64_t centinela = CENTINELA_PARADA;
    memcpy(buffer_envio, &centinela, sizeof(centinela));
    send(fd_socket, buffer_envio, sizeof(buffer_envio), 0);

    close(fd_socket);

    /* Ordenamos las muestras: calcular_estadisticas() necesita el
     * arreglo ordenado para leer los percentiles por posición. */
    qsort(latencias_ns, MUESTRAS_VALIDAS, sizeof(double), comparar_double);

    estadisticas_t stats = calcular_estadisticas(latencias_ns, MUESTRAS_VALIDAS);
    imprimir_resultados("udp_loopback", stats);
    escribir_resultados("udp_loopback", stats,
                         "results/latencia.log",
                         "results/resultados_udp.json");

    free(latencias_ns);
    return 0;
}
