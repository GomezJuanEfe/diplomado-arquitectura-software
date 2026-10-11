# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Qué es este repo

Solución al "Reto 2" del Diplomado de Cloud Computing: un sistema de alerta temprana para una flota
vehicular. El enunciado completo está en [Instrucciones.md](./Instrucciones.md) — léelo antes de
proponer arquitectura, ya que define la rúbrica de evaluación y los formatos exactos de payload/correo.

**Estado actual:** reto completo y verificado de punta a punta en AWS (`sa-east-1`, cuenta
`270278216472`) vía AWS CLI — API Gateway REST (`reto2-fleet-api`) → Lambda
(`reto2-emergency-processor`) → Gmail API. La prueba de carga con `k6-script.js` (1000 peticiones en
27.8s, 100% status 200) dio un tiempo último-envío-k6 → `EMAIL_SENT` de **~1.2s**, dentro del rango de
puntaje máximo de la rúbrica (<15s). No hay IaC ni pipeline de CI/CD — el despliegue es una serie de
comandos `aws` ejecutados manualmente y documentados abajo; no asumas que existe un script de
despliegue automatizado salvo que se agregue explícitamente.

## El problema a resolver (resumen operativo)

- Los vehículos envían eventos `"Position"` o `"Emergency"` a un endpoint HTTP (payload de ejemplo en
  Instrucciones.md).
- Un script de k6 (no incluido en este repo) manda 1000 peticiones en 30s.
- Cuando llega un evento `"Emergency"`, el sistema debe enviar un correo a una cuenta Gmail personal y
  loguear, con hora exacta: (1) la recepción del evento Emergency y (2) el envío exitoso del correo.
- El tiempo medido para la nota es: último envío de k6 → envío/recepción del correo. < 15s = puntaje
  máximo (2.5 pts); 15–45s = 1.5 pts; > 45s = 0.5 pts.

## Restricciones no negociables

Estas son impuestas por el enunciado/rúbrica, no son elección de diseño:

- API Gateway (si se usa): rate 15 req/s, burst 2000, stage `prod`.
- Máximo 10 instancias simultáneas de procesadores (Lambda, Docker, ECS, EC2, ALB — lo que se use).
- Notificación por correo a una cuenta Gmail real del estudiante.
- Logs explícitos con timestamp de: recepción del Emergency y envío del correo.

Cualquier diseño propuesto debe caber dentro de estos límites; si una opción los viola, descártala
antes de profundizar en ella.

## Flujo de decisiones de arquitectura

Las decisiones arquitectónicas se documentan como ADRs en `docs/decisiones/`, usando
`0000-plantilla.md` como formato (Contexto / Opciones consideradas / Decisión / Consecuencias). Cada
ADR nueva debe:

1. Crearse como `docs/decisiones/NNNN-titulo-corto.md` (numeración secuencial).
2. Enlazarse desde la sección "Decisiones tomadas" de este archivo (actualmente vacía).

No dupliques la justificación de una decisión en el código o en otro documento — el ADR es la fuente
única de verdad sobre el "por qué".

### Decisiones tomadas
- [0001 – Arquitectura de ingesta y notificación de emergencias](./docs/decisiones/0001-arquitectura-ingesta-y-notificacion.md):
  API Gateway REST → Lambda Python → Gmail API (OAuth2), despliegue manual vía AWS CLI, sin
  dependencias externas en el código de la Lambda. Nota de implementación dentro del ADR: la
  concurrencia quedó **sin reservar** porque el límite de cuenta en `sa-east-1` ya es 10 (no se puede
  reservar 10 sin bajar el "unreserved" de la cuenta por debajo del mínimo exigido por AWS).

## Permisos de Claude Code en este proyecto

`.claude/settings.json` ya restringe el entorno para este reto:

- Deniega lectura de `.env` / `.env.*` (credenciales de AWS/Gmail probablemente vivirán ahí).
- Pide confirmación antes de cualquier `aws ...` o `git push ...`. Esto es intencional: los comandos
  de AWS pueden crear/modificar recursos reales (API Gateway, Lambda, etc.) contra las cuentas del
  estudiante, así que no asumas aprobación implícita más allá del alcance pedido.

## Comandos

### Recursos AWS desplegados (`sa-east-1`, cuenta `270278216472`)

- Rol IAM: `reto2-lambda-role` (trust `lambda.amazonaws.com` + policy administrada
  `AWSLambdaBasicExecutionRole`).
- Lambda: `reto2-emergency-processor` (runtime `python3.13`, handler `handler.lambda_handler`, sin
  concurrencia reservada — ver nota en ADR 0001).
- API Gateway REST: `reto2-fleet-api`, recurso `POST /events`, integración `AWS_PROXY`, stage `prod`
  con throttling `rate=15` / `burst=2000` aplicado vía `update-stage` (path `/*/*/throttling/...`).
- URL de ingesta: `https://{API_ID}.execute-api.sa-east-1.amazonaws.com/prod/events` (el `API_ID` es
  el que devuelve `aws apigateway get-rest-apis --region sa-east-1` filtrando por `reto2-fleet-api`;
  `k6-script.js` sí trae esta URL hardcodeada por simplicidad — si se recrea el API Gateway y cambia
  el `API_ID`, hay que actualizarla ahí a mano).

### Generar el `refresh_token` de Gmail (una sola vez, local, nunca en Lambda)

`python scripts/generate_gmail_refresh_token.py` — abre el navegador para el consentimiento OAuth2 y
deja el refresh token para copiar como variable de entorno `GMAIL_REFRESH_TOKEN` en la Lambda.

### Actualizar las variables de entorno de la Lambda (hoy son placeholders `PENDIENTE`)

```bash
aws lambda update-function-configuration \
  --region sa-east-1 \
  --function-name reto2-emergency-processor \
  --environment "Variables={GMAIL_CLIENT_ID=...,GMAIL_CLIENT_SECRET=...,GMAIL_REFRESH_TOKEN=...,GMAIL_TO=gomezjuanefe@gmail.com}"
```

### Redesplegar código tras editar `lambda/handler.py`

```bash
cd lambda && zip -j /tmp/lambda.zip handler.py && cd ..
aws lambda update-function-code \
  --region sa-east-1 \
  --function-name reto2-emergency-processor \
  --zip-file fileb:///tmp/lambda.zip
```

### Probar manualmente (sin k6)

```bash
URL="https://<API_ID>.execute-api.sa-east-1.amazonaws.com/prod/events"
curl -X POST "$URL" -H "Content-Type: application/json" \
  -d '{"type":"Emergency","vehicle_plate":"XYZ999","status":"accidente"}'
```

### Ver logs (con timestamp de `EMERGENCY_RECEIVED` / `EMAIL_SENT` / `EMAIL_SEND_FAILED`)

```bash
aws logs tail /aws/lambda/reto2-emergency-processor --region sa-east-1 --since 10m
```

### Correr la prueba de carga con k6

`k6` no viene preinstalado en este entorno; se instaló como binario de usuario en `~/.local/bin/k6`
(descargado de los releases de GitHub de `grafana/k6`, sin tocar paquetes del sistema). Para correrlo:

```bash
export PATH="$HOME/.local/bin:$PATH"
k6 run k6-script.js
```

El script (`k6-script.js`, raíz del proyecto) manda 1000 peticiones con 10 VUs en ~28s; solo la última
(`globalIndex=999`) es `"Emergency"`, el resto `"Position"`. Último resultado medido: 1000/1000 con
status 200, `EMERGENCY_RECEIVED` → `EMAIL_SENT` en ~1.2s (ver timestamps reales con el comando de logs
de arriba, filtrando justo después de correr k6).
