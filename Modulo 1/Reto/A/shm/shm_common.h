/*
 * shm_common.h
 * ------------
 * Estructura del "canal" de comunicación en memoria compartida entre el
 * servidor y el cliente de la variante optimizada.
 *
 * Idea general: en vez de mandar datos por la red (con todo el costo de
 * pasar por el kernel, armar paquetes, etc.), servidor y cliente comparten
 * literalmente el mismo bloque de memoria RAM. Uno escribe una variable,
 * el otro la lee directamente, sin llamadas al sistema de por medio. Eso
 * es lo que puede bajar la latencia de decenas de microsegundos a
 * fracciones de microsegundo.
 *
 * Comentado línea por línea en español para sustentación oral.
 */

#ifndef SHM_COMMON_H
#define SHM_COMMON_H

#include <stdatomic.h> /* _Atomic, atomic_store_explicit, atomic_load_explicit */
#include <stdint.h>    /* uint64_t                                             */

/*
 * Tamaño típico de una "línea de caché" en procesadores x86_64 modernos:
 * 64 bytes. El procesador no mueve datos entre memoria RAM y caché de a
 * un byte; los mueve en bloques de este tamaño. Esto importa para el
 * "false sharing" explicado abajo.
 */
#define TAMANO_LINEA_CACHE 64

/*
 * "False sharing" (compartición falsa): ocurre cuando dos variables que
 * NO tienen relación entre sí (cada una la usa un núcleo/hilo distinto)
 * quedan, por casualidad, dentro de la MISMA línea de caché de 64 bytes.
 * Cuando el núcleo A escribe su variable, invalida toda la línea de
 * caché en el núcleo B (aunque B no le importe esa variable), obligando
 * a B a releer la línea entera desde memoria principal. Si esto pasa en
 * un bucle de espera activa que se ejecuta millones de veces por
 * segundo, el costo se vuelve enorme y arruina justamente la ganancia de
 * latencia que buscamos con memoria compartida.
 *
 * Solución: separar cada variable "caliente" (la que escribe cada lado)
 * en su propia línea de caché completa, usando relleno (padding) para
 * ocupar el resto de los 64 bytes.
 */
typedef struct {
    /* El CLIENTE escribe aquí el número de la iteración actual (el
     * "estímulo"). El SERVIDOR solo lo lee. */
    _Atomic uint64_t secuencia_cliente;
    /* Relleno para que "secuencia_cliente" ocupe toda su línea de caché
     * ella sola: 64 bytes totales - los bytes que ya ocupa el uint64_t. */
    char relleno_1[TAMANO_LINEA_CACHE - sizeof(_Atomic uint64_t)];

    /* El SERVIDOR escribe aquí el eco de esa misma secuencia (la
     * "respuesta"). El CLIENTE solo lo lee. Vive en su PROPIA línea de
     * caché, separada de secuencia_cliente, para que las escrituras de
     * un lado nunca invaliden la caché relevante del otro lado. */
    _Atomic uint64_t secuencia_servidor;
    char relleno_2[TAMANO_LINEA_CACHE - sizeof(_Atomic uint64_t)];
} __attribute__((aligned(TAMANO_LINEA_CACHE))) canal_compartido_t;
/*
 * aligned(64) en el propio tipo garantiza que, sin importar dónde se
 * ubique una variable de este tipo, arranque justo al inicio de una
 * línea de caché física de 64 bytes (nunca a la mitad de una). En nuestro
 * caso esto ya lo garantiza también mmap() -- devuelve memoria alineada a
 * página (4096 bytes, múltiplo de 64) -- pero se deja explícito en el
 * tipo para que la garantía no dependa de cómo se reserve la memoria.
 */

/* Nombre del objeto de memoria compartida POSIX (shm_open). Empieza con
 * "/" por convención de la API, aunque en Linux termina viéndose como un
 * archivo dentro de /dev/shm/. */
#define NOMBRE_SHM_DEFECTO "/reto_latencia_shm"

/* Igual que en la variante UDP: valor especial que el cliente escribe en
 * secuencia_cliente al final para decirle al servidor "ya terminé, cierra". */
#define CENTINELA_PARADA_SHM UINT64_MAX

#endif /* SHM_COMMON_H */
