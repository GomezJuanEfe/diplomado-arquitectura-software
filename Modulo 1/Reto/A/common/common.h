/*
 * common.h
 * --------
 * Utilidades COMPARTIDAS por las dos variantes (UDP y memoria compartida).
 * Aquí vive todo lo que no es específico de una técnica de comunicación:
 * medir tiempo, calcular estadísticas y escribir los resultados a disco.
 *
 * Comentado línea por línea en español porque este código se explicará
 * en una sustentación oral y el autor no tiene experiencia previa en C.
 */

#ifndef COMMON_H
#define COMMON_H

/*
 * NOTA: este código usa CPU_ZERO/CPU_SET/sched_setaffinity, que son
 * extensiones de glibc (no del estándar C ni de POSIX puro). Por eso el
 * Makefile compila con -D_GNU_SOURCE: ese flag debe llegar ANTES de que
 * se incluya el primer header del sistema en cada archivo .c, así que no
 * se puede resolver con un #define aquí adentro (llegaría demasiado
 * tarde si el .c ya incluyó <stdio.h> antes de este header).
 */

#include <stdio.h>      /* printf, fprintf, FILE, fopen                    */
#include <stdint.h>     /* uint64_t: entero sin signo de 64 bits            */
#include <stdlib.h>     /* qsort, malloc, exit                              */
#include <string.h>     /* memset                                           */
#include <time.h>       /* struct timespec, clock_gettime, CLOCK_MONOTONIC  */
#include <sched.h>      /* sched_setaffinity, sched_setscheduler            */
#include <errno.h>      /* errno                                            */

/* ------------------------------------------------------------------------ *
 * PARÁMETROS DEL EXPERIMENTO
 * ------------------------------------------------------------------------ */

/* Número total de intercambios estímulo->respuesta que hace cada cliente.  */
#define TOTAL_ITERACIONES 100000

/*
 * Cuántas de esas iteraciones iniciales se descartan y NO entran en las
 * estadísticas. Se descartan porque al principio de cualquier programa hay
 * "arranque en frío": las páginas de memoria del arreglo aún no están
 * mapeadas físicamente, la caché del procesador está vacía, el sistema
 * operativo puede estar re-priorizando el proceso, etc. Ese ruido inicial
 * no representa el comportamiento real en régimen permanente (steady state).
 */
#define ITERACIONES_CALENTAMIENTO 10000

/* Cuántas muestras quedan realmente disponibles para las estadísticas.     */
#define MUESTRAS_VALIDAS (TOTAL_ITERACIONES - ITERACIONES_CALENTAMIENTO)

/* ------------------------------------------------------------------------ *
 * MEDICIÓN DE TIEMPO
 * ------------------------------------------------------------------------ */

/*
 * ¿Por qué CLOCK_MONOTONIC y no time() ni la hora del reloj de pared?
 * - time() solo tiene resolución de 1 segundo: inútil para medir microsegundos.
 * - El reloj de pared (CLOCK_REALTIME / datetime) puede saltar hacia
 *   adelante o hacia atrás (ajustes de NTP, cambios de hora), lo que
 *   arruinaría una resta "fin - inicio".
 * - CLOCK_MONOTONIC siempre avanza hacia adelante, a un ritmo constante,
 *   y no le importa la hora real del sistema: es exactamente lo que se
 *   necesita para medir duraciones (RTT = round-trip time).
 */

/* Devuelve la diferencia (fin - inicio) en NANOSEGUNDOS como double. */
static inline double diferencia_ns(struct timespec inicio, struct timespec fin) {
    /* Diferencia de segundos, ya convertida a nanosegundos (1 s = 1e9 ns). */
    double segundos_en_ns = (double)(fin.tv_sec - inicio.tv_sec) * 1e9;
    /* Diferencia de la parte de nanosegundos dentro del segundo. */
    double nanos = (double)(fin.tv_nsec - inicio.tv_nsec);
    /* La suma de ambas partes da la duración total transcurrida. */
    return segundos_en_ns + nanos;
}

/* ------------------------------------------------------------------------ *
 * ESTADÍSTICAS
 * ------------------------------------------------------------------------ */

/* Estructura donde guardamos el resumen final de todas las mediciones. */
typedef struct {
    double min_ns;       /* La latencia más baja observada.                */
    double max_ns;       /* La latencia más alta observada (peor caso).    */
    double p50_ns;       /* Mediana: la mitad de las muestras está debajo. */
    double p99_ns;       /* El 99% de las muestras está por debajo de esto.*/
    double p999_ns;      /* El 99.9% de las muestras está por debajo.      */
    double promedio_ns;  /* Promedio aritmético simple.                    */
    long   muestras;     /* Cuántas muestras se usaron para este cálculo.  */
} estadisticas_t;

/*
 * Estas cuatro funciones (comparar_double, calcular_estadisticas,
 * escribir_resultados, imprimir_resultados) solo las usan los CLIENTES
 * (son quienes miden el RTT); los servidores incluyen este mismo header
 * pero nunca las llaman. __attribute__((unused)) le dice a gcc que no
 * avise con -Wall si en algún .c concreto quedan sin usar.
 */

/* Función de comparación que exige qsort() para ordenar doubles ascendente.*/
static int __attribute__((unused)) comparar_double(const void *a, const void *b) {
    double da = *(const double *)a; /* Reinterpreta el puntero genérico... */
    double db = *(const double *)b; /* ...como puntero a double, y lo lee. */
    if (da < db) return -1; /* a va antes que b                            */
    if (da > db) return 1;  /* a va después que b                          */
    return 0;                /* son iguales                                */
}

/*
 * Calcula min, max, percentiles y promedio.
 * IMPORTANTE: requiere que "muestras" YA esté ordenado ascendentemente,
 * porque los percentiles se leen directamente por posición en el arreglo.
 */
static estadisticas_t __attribute__((unused)) calcular_estadisticas(double *muestras, long n) {
    estadisticas_t s;
    s.muestras = n;
    s.min_ns = muestras[0];       /* El primer elemento es el más chico.   */
    s.max_ns = muestras[n - 1];   /* El último elemento es el más grande.  */

    /*
     * Percentil p: el valor por debajo del cual cae el p% de las muestras.
     * Con el arreglo ordenado, basta tomar el elemento en la posición
     * floor(p * n). Se acota (clamp) a n-1 para nunca salirse del arreglo.
     */
    long idx_p50  = (long)(0.500 * (double)n);
    long idx_p99  = (long)(0.990 * (double)n);
    long idx_p999 = (long)(0.999 * (double)n);
    if (idx_p50  >= n) idx_p50  = n - 1;
    if (idx_p99  >= n) idx_p99  = n - 1;
    if (idx_p999 >= n) idx_p999 = n - 1;

    s.p50_ns  = muestras[idx_p50];
    s.p99_ns  = muestras[idx_p99];
    s.p999_ns = muestras[idx_p999];

    /* Promedio = suma de todo, dividido entre la cantidad de elementos. */
    double suma = 0.0;
    for (long i = 0; i < n; i++) {
        suma += muestras[i];
    }
    s.promedio_ns = suma / (double)n;

    return s;
}

/*
 * Escribe el resultado en dos archivos:
 *  - ruta_log:  texto plano, legible por humanos, en modo "append" (agrega
 *               al final sin borrar lo que ya había, para poder acumular
 *               los resultados de varias corridas o variantes).
 *  - ruta_json: JSON simple, en modo "escritura" (sobreescribe), pensado
 *               para que lo lea el servidor web de comparación.
 *
 * Se llama UNA sola vez, después de terminar el bucle de medición: por eso
 * esta función NO cuenta como "I/O dentro del bucle caliente".
 */
static void __attribute__((unused)) escribir_resultados(const char *nombre_variante, estadisticas_t s,
                                 const char *ruta_log, const char *ruta_json) {
    FILE *flog = fopen(ruta_log, "a");
    if (flog != NULL) {
        fprintf(flog, "==== Variante: %s ====\n", nombre_variante);
        fprintf(flog, "Muestras validas : %ld\n", s.muestras);
        fprintf(flog, "Minimo   (ns)    : %.2f\n", s.min_ns);
        fprintf(flog, "p50      (ns)    : %.2f\n", s.p50_ns);
        fprintf(flog, "p99      (ns)    : %.2f\n", s.p99_ns);
        fprintf(flog, "p99.9    (ns)    : %.2f\n", s.p999_ns);
        fprintf(flog, "Maximo   (ns)    : %.2f\n", s.max_ns);
        fprintf(flog, "Promedio (ns)    : %.2f\n\n", s.promedio_ns);
        fclose(flog);
    } else {
        perror("No se pudo abrir el archivo de log");
    }

    FILE *fjson = fopen(ruta_json, "w");
    if (fjson != NULL) {
        fprintf(fjson,
            "{\n"
            "  \"variante\": \"%s\",\n"
            "  \"muestras\": %ld,\n"
            "  \"min_ns\": %.2f,\n"
            "  \"p50_ns\": %.2f,\n"
            "  \"p99_ns\": %.2f,\n"
            "  \"p999_ns\": %.2f,\n"
            "  \"max_ns\": %.2f,\n"
            "  \"promedio_ns\": %.2f\n"
            "}\n",
            nombre_variante, s.muestras, s.min_ns, s.p50_ns,
            s.p99_ns, s.p999_ns, s.max_ns, s.promedio_ns);
        fclose(fjson);
    } else {
        perror("No se pudo abrir el archivo JSON");
    }
}

/* Imprime la tabla de resultados por consola, para verlos sin abrir archivos.*/
static void __attribute__((unused)) imprimir_resultados(const char *nombre_variante, estadisticas_t s) {
    printf("\n==== Resultados: %s ====\n", nombre_variante);
    printf("Muestras validas : %ld\n", s.muestras);
    printf("Minimo   : %10.2f ns\n", s.min_ns);
    printf("p50      : %10.2f ns\n", s.p50_ns);
    printf("p99      : %10.2f ns\n", s.p99_ns);
    printf("p99.9    : %10.2f ns\n", s.p999_ns);
    printf("Maximo   : %10.2f ns\n", s.max_ns);
    printf("Promedio : %10.2f ns\n\n", s.promedio_ns);
}

/* ------------------------------------------------------------------------ *
 * OPTIMIZACIONES DEL SISTEMA OPERATIVO (best-effort, no críticas)
 * ------------------------------------------------------------------------ */

/*
 * Fija (ancla) el proceso actual a un único núcleo de CPU.
 *
 * ¿Para qué sirve? Si el planificador de Linux mueve el proceso de un
 * núcleo a otro mientras corre, el proceso pierde todo lo que tenía en la
 * caché L1/L2 de ese núcleo (tiene que "recalentarla" en el núcleo nuevo),
 * y eso añade latencia extra e impredecible (jitter) a cada medición.
 * Clavar el proceso a un núcleo fijo elimina esa fuente de ruido.
 *
 * Es "best-effort": si falla (por ejemplo, la máquina tiene menos núcleos
 * de los que pedimos), se avisa por stderr pero el programa sigue
 * funcionando igual, solo que sin esta optimización.
 */
static void fijar_cpu(int nucleo) {
    cpu_set_t conjunto;         /* Tipo especial: un "conjunto de CPUs".    */
    CPU_ZERO(&conjunto);        /* Lo vacía (ningún CPU seleccionado aún).  */
    CPU_SET(nucleo, &conjunto); /* Agrega el núcleo pedido al conjunto.     */
    /* Le pide al kernel que el proceso (pid 0 = "yo mismo") solo pueda
     * ejecutarse en los núcleos del conjunto que acabamos de armar. */
    if (sched_setaffinity(0, sizeof(conjunto), &conjunto) != 0) {
        fprintf(stderr, "[aviso] no se pudo fijar CPU %d (%s); "
                        "se continua sin afinidad de CPU.\n",
                nucleo, strerror(errno));
    }
}

/*
 * Intenta subir la prioridad del proceso a "tiempo real" (SCHED_FIFO).
 *
 * Un proceso normal puede ser interrumpido por el planificador en
 * cualquier momento para dejar correr a otros procesos ("time slicing").
 * SCHED_FIFO le dice al kernel: "no interrumpas a este proceso por otros
 * de prioridad normal", lo que reduce aún más el jitter en el bucle de
 * medición. Requiere privilegios de root (o la capacidad CAP_SYS_NICE);
 * si no los tenemos, sched_setscheduler() falla con EPERM y simplemente
 * seguimos en modo normal (best-effort, igual que fijar_cpu).
 */
static void intentar_prioridad_tiempo_real(void) {
    struct sched_param parametro;
    /* Usamos la prioridad máxima permitida para la política SCHED_FIFO. */
    parametro.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if (sched_setscheduler(0, SCHED_FIFO, &parametro) != 0) {
        fprintf(stderr, "[aviso] no se pudo obtener prioridad de tiempo "
                        "real (%s); probablemente falten privilegios de "
                        "root. Se continua con prioridad normal.\n",
                strerror(errno));
    }
}

#endif /* COMMON_H */
