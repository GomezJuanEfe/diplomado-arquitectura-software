# Workshop Parte 4 - Métricas

> Resumen del video [Workshop Parte 4 Métricas](https://youtu.be/CLf1zhLzW1g) (14:54), basado en la transcripción y lo que se muestra en pantalla.

## Objetivo

Trabajar con **métricas personalizadas** de dos formas, **síncrona** y **asíncrona**, y entender las ventajas y desventajas de cada una. Antes de empezar se verifica que la aplicación desplegada en la parte 3 siga funcionando.

## Concepto clave: *Cold Start*

- Una Lambda no corre en un servidor aprovisionado 24x7. Ante una solicitud, AWS **inicializa un contenedor** para atenderla, y ese arranque es el ***cold start***.
- Si la Lambda se sigue llamando, el contenedor se mantiene vivo. Si deja de llamarse (≈ 5 minutos en el video), se apaga y la siguiente invocación vuelve a tener cold start.

## 1. Métricas síncronas (`putMetric`)

Se modifica `src/handlers/put-item.js` (sección *Pushing Metrics Synchronously* del workshop):

1. Importar las unidades de métricas y declarar `let _cold_start = true`.
2. **ColdStart**: si `_cold_start` es verdadero, publicar `ColdStart = 1` en el servicio `item_service` (nombre configurable) y ponerlo en `false`.
3. **UnsupportedHTTPMethod**: si el método no es `POST` (p. ej. `PUT`, `DELETE`, `PATCH`), publicar la métrica y lanzar un error, porque el cliente consume la API de forma incorrecta.
4. **Métricas de negocio**: `SuccessfulPutItem = 1` al final del `try` y `FailedPutItem = 1` al inicio del `catch`.

```js
await putMetric(name = 'SuccessfulPutItem', unit = MetricUnit.Count, value = 1, { service: 'item_service', ... })
```

> 💡 Para evitar errores de copiado, conviene copiar el **método completo** (*Full putItemHandler method*) que aparece al final de la página del workshop y reemplazar el `export` existente.

### Desplegar el cambio

- **Opción rápida**: pegar el código directamente en la consola de Lambda y hacer *Deploy*.
- **Opción recomendada** (como en un entorno productivo): `sam build` + `sam deploy --profile <perfil>`. La guía del workshop no incluye el perfil; **hay que agregarlo**.
- SAM detecta que solo cambió `put-item` y despliega únicamente eso (≈ 2 minutos).

### Probar

- Con Postman se crean los ítems **3 y 4** (`POST`) y se verifican con *Get All Items*. Un `500` en el video fue por un error de tipeo en la URL.
- Se intenta un `PATCH` para provocar la métrica `UnsupportedHTTPMethod`, pero **API Gateway responde `403 Forbidden`**: el API solo expone los métodos definidos, así que la petición nunca llega a la Lambda.

### Ver las métricas en CloudWatch

1. **CloudWatch → Métricas → Todas las métricas → Custom namespaces → `MonitoringApp`**.
2. Agregar a la gráfica `ColdStart` (de la función correspondiente) y, en *operaciones*, `SuccessfulPutItem` y `FailedPutItem`.
3. Usar la estadística **Sum** con periodo de **5 minutos** y la vista **Number**.
4. ⏱️ Las métricas se agregan en ventanas de 5 minutos, así que **hay que esperar** a que se cierre el intervalo en que se hicieron las peticiones.

**Resultado**: `ColdStart = 1`, `SuccessfulPutItem = 2` (ítems 3 y 4), `FailedPutItem = 1`.

### Métricas con sentido de negocio

Las métricas no son solo de infraestructura (latencia, errores). También sirven para **medir el negocio**: compras completadas, procesos terminados, ventas o conexiones. Se pueden llevar a un **dashboard** para negocio y operaciones. Por ejemplo: *"4000 transacciones, 3900 exitosas y 100 fallidas por estos motivos"*.

### ⚠️ Desventaja de las métricas síncronas

La publicación de la métrica ocurre **dentro del flujo de la lógica de negocio**. Si `putMetric` falla o el servicio de métricas no está disponible, **la operación de negocio se ve afectada**: se podría perder una venta porque falló la métrica. Por eso se introduce el enfoque asíncrono.

## 2. Métricas asíncronas con *Embedded Metric Format* (EMF)

- **EMF** (introducido por AWS en 2019) permite enviar **métricas personalizadas como logs**. Se escriben en CloudWatch Logs y CloudWatch extrae las métricas de forma asíncrona, sin llamadas a la API de métricas dentro del flujo.

### Pasos

1. **Modificar `template.yaml`**: en `Globals → Function → Environment → Variables`, agregar debajo de la línea 22:

   ```yaml
   AWS_EMF_NAMESPACE: MonitoringApp
   ```

2. **Modificar `get-all-items.js`**:
   - Reemplazar las primeras líneas de imports para usar `logMetricEMF` (de `aws-embedded-metrics`) en vez de `putMetric`.
   - Agregar las mismas validaciones: `ColdStart`, `UnsupportedHTTPMethod` (si no es `GET`), `SuccessfulGetAllItems` y `FailedGetAllItems`, todas con valor `1`.
   - Copiar el *Full getAllItemsHandler method* y reemplazar solo el `export`.
3. **Modificar `get-by-id.js`** de la misma forma: imports, cold start y el handler completo.
4. Desplegar en un solo comando:

   ```bash
   sam build -u; sam deploy --profile develop
   ```

La validación de estas métricas asíncronas continúa en la **parte 5**.

## Síncronas vs. asíncronas

| | Síncronas (`putMetric`) | Asíncronas (EMF, `logMetricEMF`) |
|---|---|---|
| Cómo se publican | Llamada directa a la API de CloudWatch Metrics | Se escriben como logs estructurados |
| Impacto en el negocio | Un fallo puede romper la operación | No bloquean el flujo de negocio |
| Latencia añadida | Sí, en cada invocación | Mínima |
