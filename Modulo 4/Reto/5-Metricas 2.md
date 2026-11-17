# Workshop Parte 5 - Métricas 2

> Resumen del video [Workshop Parte 5 Métricas 2](https://youtu.be/KUOe0UP2Sf4) (17:25), basado en la transcripción y lo que se muestra en pantalla.

## 1. Validar las métricas asíncronas (EMF)

Continúa desde la parte 4, con `get-all-items` y `get-by-id` ya desplegados usando *Embedded Metric Format*.

1. En Postman, ejecutar **Get All Items** y **Get Item by ID** (ID 1).
2. Ir a **CloudWatch → Logs → Log groups** y abrir los grupos de `monitoring-app-getAllItemsFunction-*` y `monitoring-app-getByIdFunction-*`.
3. En el *log stream* del día aparece un **log estructurado en JSON** con:
   - Contexto: `LogGroup`, `ServiceName` (`item_service`), `ServiceType` (`AWS::Lambda::Function`), `function_name`, `traceId`, etc.
   - Un bloque `_aws.CloudWatchMetrics` con el `Namespace` (`MonitoringApp`), las `Dimensions` y las `Metrics` (nombre y unidad).
   - El valor de la métrica, p. ej. `"ColdStart": 1` o `SuccessfulGetAllItems: 1`.
4. En **CloudWatch → Métricas → `MonitoringApp`** aparecen las nuevas métricas (`ColdStart`, `SuccessfulGetAllItems`, `SuccessfulGetItem`). Para verlas: estadística **Sum** y vista **Number**. ⏱️ De nuevo hay que esperar la ventana de 5 minutos.

**Conclusión**: las métricas se pueden **enviar como logs, de forma asíncrona**, sin que la funcionalidad de negocio se vea afectada si falla el envío. Esto abre la puerta a alertas y métricas de negocio, no solo de infraestructura.

## 2. Alarmas

Las alarmas permiten **actuar de forma proactiva**: detectar cambios en el sistema, dar una alerta temprana y saber cuándo algo va por mal camino.

### Crear la alarma

**CloudWatch → Alarmas → Crear alarma**:

| Paso | Configuración |
|---|---|
| Métrica | `MonitoringApp` → `SuccessfulGetAllItems` (operación `get-all-items`) |
| Estadística / periodo | **Sum** en **5 minutos** |
| Condición | Umbral **estático**, **Greater than `1`** |
| Datapoints to alarm | **1 de 1**. Por defecto aparecía 5 de 5; se cambia para no esperar cinco periodos. |
| Datos faltantes | *Treat missing data as missing* (por defecto) |
| Acción | En estado **In alarm**, notificar a un **nuevo tópico SNS** (`topico_alarmas_email`) con el **correo** propio como endpoint |
| Nombre | `My Observability Alarm` |

> **Datos faltantes**: si la función no se invoca, no se genera la métrica. Hay que decidir si la alarma mantiene su estado, ignora esos datos o los trata como faltantes.

### Confirmar la suscripción SNS

- Llega un correo de AWS para **confirmar la suscripción** al tópico; hay que dar clic en el enlace.
- En **SNS → Tópicos** la suscripción debe aparecer como **Confirmada**.
- ⚠️ En el video el correo tenía un error de tipeo y la confirmación no llegó. **Revisar bien el correo**.

### Probar

- Inicialmente la alarma muestra **Datos insuficientes**.
- Se hacen dos o más llamadas a *Get All Items* (con una basta para superar el umbral) y, tras la ventana de 5 minutos, la alarma pasa a **En alarma** y llega el **correo de notificación**.

### Uso en el negocio

- Ejemplos: detectar cuando fallan las compras, aumentan los rechazos o hay problemas al **tokenizar o enrolar una tarjeta**.
- Lo clave es **definir los umbrales relevantes para el negocio**.
- La acción no tiene que ser un correo: puede ser un evento, una notificación a otro proceso o disparar un proceso operativo (p. ej. escalar una instancia).

## 3. Dashboards

### Intento con CDK (falló)

El workshop crea los dashboards automáticamente con **AWS CDK**:

```bash
cd serverless-observability-workshop/code/cloudwatch-cdk
npm outdated
npm update --force
npm install --force
npm install -g typescript aws-cdk
cdk deploy -c stack_name=monitoring-app
```

❌ Falla con un error de que no encuentra `cloudwatch-cdk.ts`. Es un problema de **librerías desactualizadas**: el workshop ya no se mantiene, TypeScript está en la versión 5 y CDK pasó de la v1 a la v2.

### Crear el dashboard manualmente

1. **CloudWatch → Dashboards → Crear dashboard** (`dashboard-metricas`).
2. **Widget de números** con métricas de negocio de `MonitoringApp`: `ColdStart`, `SuccessfulGetAllItems` y `SuccessfulPutItem`.
3. **Widget de alarma** con `My Observability Alarm`.
4. **Widget de métricas de infraestructura**: métricas por defecto de AWS (no del namespace personalizado), en **Lambda → By Function Name → getAllItems**:
   - `Duration`, `Invocations`, `ConcurrentExecutions`, `Errors` y `Throttles`.
5. Guardar el dashboard.

Los dashboards admiten gráficos de barras y de tendencia, indicadores numéricos, tablas de logs y más, y se pueden ajustar a gusto.

## Recapitulación

- Las métricas deben responder a **lo que le importa al negocio**: qué queremos medir.
- Sobre ellas se montan **dashboards** visibles para negocio y operaciones.
- Y **alarmas** que permitan al sistema **actuar** ante esos eventos.
