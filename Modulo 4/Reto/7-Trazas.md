# Workshop Parte 7 - Trazas

> Resumen del video [Workshop Parte 7 Trazas](https://youtu.be/pqG5L3HvXVw) (25:54), basado en la transcripción y lo que se muestra en pantalla.

## Objetivo

Cerrar el workshop con **tracing distribuido** usando **AWS X-Ray**. Las trazas permiten encontrar **cuellos de botella**, ver cómo se comporta la aplicación y comprobar si todos los componentes reciben y procesan bien las peticiones.

## 1. Desplegar la app de tracing (`sample-app-tracing`)

```bash
cd serverless-observability-workshop/code/sample-app-tracing
npm install
sam build
sam deploy -g --profile develop   # stack: monitoring-app-tracing
```

- Mismas respuestas del asistente que en la parte 3: misma región, sin confirmar cambios, deshabilitar rollback, aceptar el resto y guardar la configuración.
- ⚠️ Sin `--profile` el deploy falla. Hay que cancelarlo con `Ctrl+C` y relanzarlo con el perfil.

### Problema encontrado: permisos de SNS

- El deploy falló porque esta versión de la app crea un **tópico SNS** y el usuario no tenía permisos.
- **Solución**: agregar la política **`AmazonSNSFullAccess`** al usuario IAM.
- Antes de reintentar, **eliminar el stack** en CloudFormation, que queda en `ROLLBACK_COMPLETE`.

### Probar

- En Lambda aparecen las mismas funciones con el prefijo `monitoring-app-tracing-*`.
- En Postman, **duplicar las peticiones** (Put Item, Get All Items, Get Item by ID) con la **nueva URL** del API (cuidado con el doble `//`) y probarlas.

## 2. Activar el tracing (*Active Tracing*)

En `template.yaml` de `sample-app-tracing`, sección `Globals`:

```yaml
Globals:
  Function:
    Tracing: Active          # X-Ray para todas las Lambdas
  Api:
    TracingEnabled: true     # X-Ray para API Gateway
```

- Volver a hacer `sam build` + `sam deploy --profile develop`. La URL del API no cambia.
- Probar las APIs (crear el ítem 2 y consultar) e ir a **X-Ray traces** en CloudWatch.

### Activar X-Ray en el stage de API Gateway

Si el API Gateway no aparece en el mapa: **API Gateway → API de tracing → Stages → `Prod` → Logs y rastreo** y **activar X-Ray tracing**. Verificar que sea el stage que se está llamando (*Prod*).

## 3. Capturar las llamadas al SDK de AWS

Para que aparezcan **DynamoDB** y **SNS** en el mapa, hay que envolver el SDK con X-Ray en los **4 handlers** (`put-item.js`, `get-all-items.js`, `get-by-id.js` y `notify-new-item.js`):

```js
const AWSXRay = require('aws-xray-sdk-core')
const AWS = AWSXRay.captureAWS(require('aws-sdk'))
const docClient = new AWS.DynamoDB.DocumentClient()
const sns = new AWS.SNS()   // solo en put-item
```

- La captura con X-Ray debe declararse **antes** de crear el cliente de DynamoDB.
- En los handlers que no usan SNS solo se agregan las líneas de X-Ray y DynamoDB.

Desplegar de nuevo y consumir las APIs.

## 4. El mapa de trazas (*Trace Map*)

Ahora el mapa muestra la **correlación entre componentes**:

```
Cliente → API Gateway (stage Prod) → Lambda (contexto) → Lambda (función) → DynamoDB
                                     put-item → SNS topic → Lambda notify-new-item
```

- La Lambda `put-item` **publica en SNS** y la Lambda `notify-new-item` está **suscrita al tópico**: recibe el evento SNS y lo procesa.
- Cada nodo muestra métricas como **transacciones por minuto** (p. ej. 0.2/min) y **% de éxito** (100 %).
- Es muy valioso para **operaciones**: permite medir **ANS/SLA** y saber cuánto tiempo hubo caídas.

## 5. Análisis de una traza (*Segments Timeline*)

Ejemplo de una petición `GET` a Get All Items:

| Segmento | Duración |
|---|---|
| API Gateway (stage `Prod`) | 1.80 s |
| Lambda (total) | ~1.78 s |
| └ Initialization (*cold start*) | 791 ms |
| └ Invocation | 815 ms |
| &nbsp;&nbsp;&nbsp;└ DynamoDB (scan) | ~600 ms |
| └ Overhead | 18 ms |

La traza muestra **cómo se distribuye el tiempo**: dónde está el cuello de botella, qué servicio tarda más y qué optimizar. La traza también enlaza a sus **logs** en CloudWatch Logs Insights.

## 6. Subsegmentos, anotaciones y metadatos

Se agrega instrumentación manual en los 4 handlers (en `put-item` y los demás se copia el código completo del workshop):

- **Subsegmentos**: miden partes específicas del código, como guardar en DynamoDB o publicar en SNS.
- **Anotaciones** (indexadas y filtrables): `ItemID`, `Status` (`SUCCESS` en el camino feliz o una anotación de fallo en el `catch`) e `ItemsCount`.
- **Metadatos** (no indexados): el ítem guardado en DynamoDB y el mensaje publicado en SNS, útiles para saber **qué se envió**.

Desplegar de nuevo y probar (crear el ítem 5 y consultarlo).

### Filtrar trazas por anotaciones

En **X-Ray → Traces**, usar *User annotations* (`ItemID`, `Status`, `ItemsCount`) y refinar la consulta:

```
annotation.Status = "SUCCESS"
```

(con **comillas dobles**). Al abrir la traza del `POST` se ven los nuevos subsegmentos con sus tiempos, la **metadata** guardada en DynamoDB, el mensaje enviado a SNS y la ejecución de `notify-new-item` con estado *SUCCESS*.

## Conclusiones

- Subsegmentos, anotaciones y metadatos **enriquecen la información** del sistema: cuánto tarda escribir en la base de datos, notificar, consumir un servicio o hacer un cálculo.
- Ejemplo: si una Lambda de 30 s gasta 10 s en un paso, **ahí está el punto focal** para optimizar.
- Lo principal de las trazas es tener **toda la información necesaria para optimizar el código**.
- Como valor agregado, el workshop permite crear **grupos de X-Ray** para organizar trazas por latencia, prioridad, etc. Queda como ejercicio.
