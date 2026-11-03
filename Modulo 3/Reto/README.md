# Reto 3 – Arquitectura resiliente en AWS

Sistema de 3 niveles de servicio que se degrada ante errores y se recupera solo.

- Decisiones: [`docs/decisiones/0001-arquitectura-resiliente.md`](docs/decisiones/0001-arquitectura-resiliente.md)
- Evidencia de la prueba: [`docs/evidencia/`](docs/evidencia/)

## Requisitos

AWS CLI con credenciales, Python 3, `zip` y k6. La región por defecto es `us-east-1`.

## Desplegar

```bash
./scripts/deploy.sh
```

Se puede repetir sin duplicar recursos. Al final imprime la URL de la API. Tras el primer despliegue,
espera 2 minutos: el stream y las alarmas tardan en quedar activos.

## Probar

La URL de la línea 47 del script debe apuntar a tu despliegue. Ese ID es el único cambio hecho al
script entregado.

```bash
k6 run "K6 script - reto3.js"
```

Dura unos 7,5 minutos. Empieza con el estado en nivel 1 y 0 errores.

```bash
aws logs tail /aws/lambda/reto3-auditor --region us-east-1 --since 15m --filter-pattern NIVEL_CAMBIADO
```

El dashboard de CloudWatch se llama `reto3-resiliencia`.

## Panel local

Herramienta de demostración para lanzar k6 y seguir el sistema en tiempo real. No es parte de la
solución en AWS ni del ZIP de entrega. Usa solo la librería estándar de Python, la CLI `aws` y k6.

```bash
python3 panel/servidor.py
```

Abre `http://127.0.0.1:8091`; las diapositivas interactivas de la exposición están en
`http://127.0.0.1:8091/slides` y se navegan con las flechas (F: pantalla completa). El botón se habilita cuando el sistema está en nivel 1 con 0 errores.
El contador se lee cada 3 s, así que puede ir una petición por detrás; la secuencia exacta es la de
"Transiciones registradas", que llega con unos segundos de retraso. Tras la última petición, k6
puede tardar hasta un minuto en cerrar, porque el script repite su espera al segundo 00.

## Eliminar

```bash
./scripts/destroy.sh --confirmar
```

Es irreversible y también borra los logs de CloudWatch.
