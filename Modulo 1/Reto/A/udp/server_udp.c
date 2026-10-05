/*
 * server_udp.c
 * ------------
 * Servidor de ECO por UDP sobre loopback (127.0.0.1).
 *
 * "Eco" significa: recibe un paquete y devuelve exactamente el mismo
 * contenido al remitente. Es la variante BASELINE (de referencia) contra
 * la que se compara la versión optimizada con memoria compartida.
 *
 * NOTA SOBRE TCP_NODELAY: esa opción existe para desactivar el algoritmo
 * de Nagle en sockets TCP (que agrupa paquetes pequeños para enviar menos
 * paquetes más grandes, a costa de latencia). Aquí NO aplica: estamos
 * usando UDP, que nunca agrupa datagramas ni tiene algoritmo de Nagle.
 * Cada sendto() se convierte en un datagrama independiente de inmediato.
 * Se documenta la decisión en vez de aplicar una opción que no existe
 * para este tipo de socket.
 *
 * Comentado línea por línea en español para sustentación oral.
 */

#include <stdio.h>      /* printf, perror                                  */
#include <stdlib.h>     /* exit, atoi                                      */
#include <string.h>     /* memset                                          */
#include <unistd.h>     /* close                                           */
#include <stdint.h>     /* uint64_t                                        */
#include <arpa/inet.h>  /* htons, inet_pton, struct sockaddr_in            */
#include <sys/socket.h> /* socket, bind, recvfrom, sendto                  */

#include "../common/common.h"

/* Puerto UDP por defecto si el usuario no pasa uno por línea de comandos. */
#define PUERTO_DEFECTO 9001

/*
 * Valor especial de "estímulo" que el cliente envía al final para avisarle
 * al servidor que ya terminó y debe cerrar su bucle (en vez de quedarse
 * escuchando para siempre). UINT64_MAX nunca aparece como número de
 * iteración real (las iteraciones van de 0 a 99999).
 */
#define CENTINELA_PARADA UINT64_MAX

/* Núcleo de CPU al que ancla el servidor (ver common.h: fijar_cpu). */
#define NUCLEO_SERVIDOR 0

int main(int argc, char *argv[]) {
    /* Si el usuario pasó un puerto como argumento, se usa ese; si no, el
     * puerto por defecto definido arriba. atoi() convierte texto a int. */
    int puerto = (argc > 1) ? atoi(argv[1]) : PUERTO_DEFECTO;

    /* Optimizaciones de sistema operativo: ver comentarios en common.h. */
    fijar_cpu(NUCLEO_SERVIDOR);
    intentar_prioridad_tiempo_real();

    /*
     * socket(AF_INET, SOCK_DGRAM, 0):
     *  - AF_INET   -> familia de direcciones IPv4.
     *  - SOCK_DGRAM -> socket de DATAGRAMAS, es decir, UDP (a diferencia
     *                  de SOCK_STREAM que sería TCP).
     *  - 0         -> protocolo por defecto para ese tipo de socket (UDP).
     * Devuelve un "descriptor de archivo" (un número entero) que se usa
     * después para referirse a este socket.
     */
    int fd_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* struct sockaddr_in describe una dirección IPv4 + puerto. */
    struct sockaddr_in direccion_local;
    memset(&direccion_local, 0, sizeof(direccion_local)); /* la limpia a cero */
    direccion_local.sin_family = AF_INET;
    /* htons = "host to network short": convierte el puerto al orden de
     * bytes que espera la red (big-endian), sin importar el orden nativo
     * de la CPU donde corre este programa. */
    direccion_local.sin_port = htons((uint16_t)puerto);
    /* INADDR_ANY escucharía en todas las interfaces; aquí forzamos
     * explícitamente 127.0.0.1 (loopback) porque el reto exige que el
     * plano de datos viva SOLO dentro de la misma máquina. */
    inet_pton(AF_INET, "127.0.0.1", &direccion_local.sin_addr);

    /* bind() asocia el socket a la dirección/puerto que armamos arriba. */
    if (bind(fd_socket, (struct sockaddr *)&direccion_local,
             sizeof(direccion_local)) < 0) {
        perror("bind");
        close(fd_socket);
        exit(EXIT_FAILURE);
    }

    printf("[server_udp] escuchando en 127.0.0.1:%d (nucleo %d)\n",
           puerto, NUCLEO_SERVIDOR);

    /* Buffer alineado a 64 bytes (tamaño típico de línea de caché). Para
     * un servidor de un solo hilo esto no evita "false sharing" (eso
     * importa cuando dos hilos/núcleos escriben variables distintas que
     * comparten línea de caché, como en la variante de memoria
     * compartida). Aun así se alinea aquí por prolijidad y para que el
     * tamaño del buffer sea múltiplo exacto de una línea de caché. */
    _Alignas(64) uint8_t buffer[64];

    /* Dirección del remitente: recvfrom() la llena con quién nos escribió,
     * para poder responderle exactamente a esa misma dirección. */
    struct sockaddr_in direccion_cliente;
    socklen_t tam_direccion_cliente = sizeof(direccion_cliente);

    for (;;) { /* bucle infinito hasta recibir la señal de parada */
        /*
         * recvfrom() bloquea (duerme) al proceso hasta que llega un
         * datagrama. Lo copia a "buffer" y guarda en "direccion_cliente"
         * quién lo envió. Devuelve cuántos bytes llegaron.
         */
        ssize_t bytes_recibidos = recvfrom(
            fd_socket, buffer, sizeof(buffer), 0,
            (struct sockaddr *)&direccion_cliente, &tam_direccion_cliente);

        if (bytes_recibidos < 0) {
            perror("recvfrom");
            continue; /* si hubo un error puntual, seguimos escuchando */
        }

        /* Interpretamos los primeros 8 bytes del buffer como el contador
         * de iteración que mandó el cliente (uint64_t). */
        uint64_t valor;
        memcpy(&valor, buffer, sizeof(valor));

        if (valor == CENTINELA_PARADA) {
            printf("[server_udp] senal de parada recibida, cerrando.\n");
            break; /* sale del bucle infinito y termina el programa */
        }

        /*
         * sendto() devuelve EL MISMO buffer, tal cual llegó, a la
         * dirección del cliente que lo mandó: esto es literalmente "hacer
         * eco". No se toca ni se interpreta el contenido más que para
         * revisar el centinela de arriba.
         */
        sendto(fd_socket, buffer, (size_t)bytes_recibidos, 0,
               (struct sockaddr *)&direccion_cliente, tam_direccion_cliente);
    }

    close(fd_socket); /* libera el descriptor del socket */
    return 0;
}
