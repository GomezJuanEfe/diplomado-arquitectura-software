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

## Eliminar

```bash
./scripts/destroy.sh --confirmar
```

Es irreversible y también borra los logs de CloudWatch.
