# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Qué es este repo

Solución al "Reto 2" del Diplomado de Cloud Computing: un sistema de alerta temprana para una flota
vehicular. El enunciado completo está en [Instrucciones.md](./Instrucciones.md) — léelo antes de
proponer arquitectura, ya que define la rúbrica de evaluación y los formatos exactos de payload/correo.

**Estado actual: no hay código fuente todavía.** El repo solo contiene el enunciado, una plantilla de
ADR y la configuración de permisos de Claude Code. No existen build/lint/test commands porque no se ha
elegido ni implementado la arquitectura. No inventes comandos; cuando se agregue código (IaC, funciones
Lambda, scripts de despliegue, k6), documenta aquí cómo desplegar, probar y ver logs.

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
(Vacío. Cada decisión vive en docs/decisiones/ y se enlaza aquí a medida que se tomen.)

## Permisos de Claude Code en este proyecto

`.claude/settings.json` ya restringe el entorno para este reto:

- Deniega lectura de `.env` / `.env.*` (credenciales de AWS/Gmail probablemente vivirán ahí).
- Pide confirmación antes de cualquier `aws ...` o `git push ...`. Esto es intencional: los comandos
  de AWS pueden crear/modificar recursos reales (API Gateway, Lambda, etc.) contra las cuentas del
  estudiante, así que no asumas aprobación implícita más allá del alcance pedido.

## Comandos
(Se llenan cuando existan: desplegar, probar con k6, ver logs.)
