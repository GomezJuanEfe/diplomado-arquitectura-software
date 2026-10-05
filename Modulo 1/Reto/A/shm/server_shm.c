/*
 * server_shm.c
 * ------------
 * Servidor de ECO por memoria compartida (variante OPTIMIZADA).
 *
 * A diferencia del servidor UDP, este NO usa sockets ni el stack de red
 * del kernel: crea una región de memoria que el cliente también puede
 * mapear en su propio espacio de direcciones, y se comunican escribiendo
 * y leyendo directamente esas variables (con espera activa / busy-wait).
 *
 * Comentado línea por línea en español para sustentación oral.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* ftruncate, close                                */
#include <fcntl.h>      /* O_CREAT, O_RDWR (flags para shm_open)           */
#include <sys/mman.h>   /* shm_open, mmap, munmap, shm_unlink              */
#include <sys/stat.h>   /* modo de permisos (0666)                         */
#include <stdatomic.h>  /* atomic_load_explicit, atomic_store_explicit     */

#include "../common/common.h"
#include "shm_common.h"

/* Núcleo de CPU al que ancla el servidor (distinto al del cliente). */
#define NUCLEO_SERVIDOR 0

int main(int argc, char *argv[]) {
    /* Se puede pasar un nombre de memoria compartida distinto al
     * por defecto como primer argumento, si se necesitara correr varias
     * instancias en paralelo sin que se pisen. */
    const char *nombre_shm = (argc > 1) ? argv[1] : NOMBRE_SHM_DEFECTO;

    fijar_cpu(NUCLEO_SERVIDOR);
    intentar_prioridad_tiempo_real();

    /*
     * shm_open crea (O_CREAT) un objeto de memoria compartida con el
     * nombre indicado, o lo abre si ya existe. O_RDWR pide acceso de
     * lectura y escritura. 0666 son los permisos estilo Unix
     * (lectura/escritura para todos) del objeto recién creado.
     * Devuelve un descriptor de archivo, igual que open() normal.
     */
    int fd = shm_open(nombre_shm, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /*
     * Recién creado, el objeto de memoria compartida tiene tamaño 0.
     * ftruncate() lo agranda (o lo achica) hasta el tamaño que
     * necesitamos: exactamente el tamaño de nuestra estructura del canal.
     */
    if (ftruncate(fd, sizeof(canal_compartido_t)) != 0) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    /*
     * mmap() "mapea" ese objeto de memoria compartida dentro del espacio
     * de direcciones de ESTE proceso, como si fuera un arreglo normal.
     *   - NULL         -> dejamos que el kernel elija en qué dirección.
     *   - sizeof(...)  -> cuántos bytes mapear.
     *   - PROT_READ|PROT_WRITE -> el proceso puede leer y escribir ahí.
     *   - MAP_SHARED   -> los cambios son visibles para otros procesos
     *                     que mapeen el MISMO objeto (justo lo que
     *                     necesitamos: que el cliente vea lo que
     *                     escribimos, y viceversa).
     *   - fd, 0        -> el objeto ya abierto, desde el byte 0.
     */
    void *region = mmap(NULL, sizeof(canal_compartido_t),
                         PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (region == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(nombre_shm);
        exit(EXIT_FAILURE);
    }

    /* Ya podemos cerrar el descriptor de archivo: el mapeo en memoria
     * (region) sigue siendo válido aunque cerremos "fd", porque mmap ya
     * hizo su trabajo de vincular la memoria virtual con el objeto. */
    close(fd);

    /* Reinterpretamos el bloque de memoria mapeado como nuestra
     * estructura del canal, y lo inicializamos en cero (ambas
     * secuencias en 0 significa "todavía no hay ningún mensaje"). */
    canal_compartido_t *canal = (canal_compartido_t *)region;
    memset(canal, 0, sizeof(canal_compartido_t));

    printf("[server_shm] canal '%s' listo (%zu bytes), nucleo %d\n",
           nombre_shm, sizeof(canal_compartido_t), NUCLEO_SERVIDOR);
    printf("[server_shm] esperando mensajes (espera activa)...\n");

    /* "ultima_vista" guarda el último valor de secuencia_cliente que ya
     * procesamos, para saber cuándo llegó uno NUEVO. */
    uint64_t ultima_vista = 0;

    for (;;) {
        uint64_t actual;

        /*
         * ESPERA ACTIVA (busy-wait): en vez de "dormir" el proceso y que
         * el sistema operativo lo despierte cuando haya novedades (como
         * hace recvfrom() en UDP, que bloquea), aquí el procesador se
         * queda preguntando en un bucle muy ajustado "¿ya cambió el
         * valor?, ¿ya cambió?, ¿ya cambió?" sin ceder el CPU a nadie más.
         *
         * Esto consume 100% de un núcleo de CPU todo el tiempo (por eso
         * NO es apto para la mayoría de programas normales), pero evita
         * por completo la latencia de que el kernel reprograme y
         * despierte al proceso, que es justamente el costo que domina
         * el RTT en la variante UDP. Es el intercambio (trade-off)
         * central de esta variante: se gasta CPU para ganar latencia.
         *
         * atomic_load_explicit con memory_order_acquire garantiza que,
         * una vez que vemos el nuevo valor de secuencia_cliente, también
         * veamos cualquier otra escritura que el cliente haya hecho
         * ANTES de publicar ese valor (aquí no hay otro dato, pero es la
         * forma correcta y portable de sincronizar dos hilos/procesos
         * mediante una variable atómica).
         */
        do {
            actual = atomic_load_explicit(&canal->secuencia_cliente,
                                           memory_order_acquire);
        } while (actual == ultima_vista);

        ultima_vista = actual;

        if (actual == CENTINELA_PARADA_SHM) {
            printf("[server_shm] senal de parada recibida, cerrando.\n");
            break;
        }

        /*
         * "Hacer eco": publicamos el mismo valor recibido en
         * secuencia_servidor. memory_order_release es la contraparte de
         * memory_order_acquire: asegura que el cliente, cuando vea este
         * nuevo valor, vea un estado consistente.
         */
        atomic_store_explicit(&canal->secuencia_servidor, actual,
                               memory_order_release);
    }

    /* Limpieza: desmapea la memoria y borra el objeto de memoria
     * compartida del sistema (si no lo borráramos, quedaría "vivo" en
     * /dev/shm/ aunque ningún proceso lo esté usando). */
    munmap(region, sizeof(canal_compartido_t));
    shm_unlink(nombre_shm);

    return 0;
}
