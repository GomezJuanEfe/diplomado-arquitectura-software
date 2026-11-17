# Dashboard - CTO Application Overview

> Resumen del video [Dashboard Stories: Monitoring third party cloud services with IsDown](https://youtu.be/gIAgVDvHkLA) (10:29, en inglés) de **SquaredUp**, basado en la transcripción y lo que se muestra en pantalla. Es el dashboard que se analiza en el foro: [CTO Application Overview](https://squaredup.com/dashboard-gallery/cto-application-overview/).

## Contexto

Adam (SquaredUp) entrevista a **Richard Jones, CTO de SquaredUp**, que construyó el dashboard **"CTO Overview"** con el propio producto para monitorear su aplicación cloud.

**Propósito**: es lo **primero que revisa cada mañana**. Le da el **estado general de la aplicación** de un vistazo, *"tomarle el pulso"*: qué funciona, qué no y qué hay que revisar en detalle.

## Paneles del dashboard

| Panel | Tipo | Qué muestra |
|---|---|---|
| **App Architecture with Health** | Diagrama SVG con estado de salud | Diagrama de arquitectura (CloudFront, S3, API Gateway, Lambda, DynamoDB en AWS; B2C, Cosmos DB, Relay en Azure) con el estado de salud de cada componente |
| **API Error Rate** (KPI) | Indicador (*gauge*) | Tasa de error promedio de las Lambdas en los últimos 15 min: **0.14 %** |
| **API Latency** (KPI) | Indicador (*gauge*) | Latencia promedio de API Gateway en los últimos 15 min: **~1212 ms** |
| **Azure Service Availability** | Estado de salud | Azure Relay, Azure B2C (Identity) y Azure Cosmos DB |
| **Azure Relay Connections** | Barras | Conexiones totales por día (últimos 7 días) |
| **Request Duration for On-Demand (API) and Scheduled Requests** | Línea de tendencia | Duración promedio de las Lambdas por hora, últimos 7 días |
| **AWS and Azure Cloud Service Status** | Tabla | Estado de AWS y Azure (vía **IsDown**) con enlaces a sus páginas oficiales de estado |
| **Cloud Costs ($)** | Dona | Costo del último mes: **$2,524.91** (AWS ≈ $1,921 + Azure ≈ $603) |
| **Incidents impacting our AWS or Azure Cloud Regions** | Tabla | Incidentes recientes del proveedor en las regiones usadas (p. ej. fallas de clúster AKS, más errores y latencia en US-EAST-1), su severidad y si ya se resolvieron |

## Funcionalidades destacadas

### Diagrama de arquitectura "vivo"

- Se dibujó en **diagrams.net (draw.io)**, aunque sirve cualquier herramienta que exporte **SVG** (también Visio).
- Los elementos del diagrama tienen **hipervínculos** a objetos de SquaredUp (otros dashboards, workspaces o recursos). Si el objeto tiene **estado de salud** (monitoreo), el diagrama **lo hereda y lo muestra**.
- **Ejemplo**: el dashboard estaba en *warning* porque el componente **B2C Auth** tenía una **latencia de login de 2.3 s**, por encima del umbral de **2 s**. Al hacer clic se llega al dashboard de *Session Metrics* con el detalle.
- Al hacer clic en **API Gateway** (en verde) se ven sus métricas: errores y latencia con tendencia a la baja.

### Multicloud en un solo lugar

- La arquitectura está en **AWS y Azure**. Sin SquaredUp tendría que revisar dashboards separados en AWS, en Azure, de costos y del estado de los proveedores.
- Usa **plugins** como fuentes de datos: **AWS**, **Azure** e **IsDown**.

### Estado de los proveedores (IsDown)

- Muestra si **AWS o Azure tienen incidentes** en las regiones que usan, sin entrar a IsDown, y permite ver si los afectan o ya se resolvieron.
- Tiene enlaces directos a `status.aws.amazon.com`, a la página de estado de Azure y a IsDown.

### KPIs a la vista

- Las métricas más importantes (tasa de error y latencia) están **directamente en el dashboard**, sin hacer clic, con **umbrales basados en su línea base** (*baseline*).
- Combina **resumen** (salud en el diagrama) y **detalle** (métricas y gráficas), y todo permite **profundizar** (*drill-down*) cuando hace falta.

### Costos con SQL Analytics

- El panel de costos une **una consulta a AWS y otra a Azure** con un **`UNION`** de SQL.
- Ambas vienen en **dólares**. Si no, habría que convertir monedas, un error común al combinar datos de costos.

### Notificaciones y compartir

- Todo panel con monitoreo puede enviar **notificaciones** a Slack, Teams, correo o **incidentes en ServiceNow**. El CTO recibe un correo con enlace al dashboard si algo cambia durante el día.
- Con la **API de notificaciones**, un dashboard o panel se puede **enviar automáticamente** (p. ej. a un canal de Slack cada mañana).
- Se puede **compartir** con un enlace solo para la organización o **público**, y proyectar en pantallas de la oficina.

## Relación con las preguntas del foro

**¿Qué métricas, logs y trazas se observan?**

- **Métricas**: tasa de error de Lambda, latencia de API Gateway, duración de las peticiones, conexiones de Azure Relay, latencia de login (B2C) y **costos** por proveedor.
- **Logs y eventos**: no se ven logs de la aplicación como tal. Sí hay **registros de eventos e incidentes** de los proveedores (IsDown) y estados de salud derivados del monitoreo.
- **Trazas**: no hay trazas distribuidas explícitas. El **diagrama de arquitectura** muestra el **flujo entre componentes** (cliente → CloudFront → API Gateway → Lambda → DynamoDB / servicios de Azure) con su salud, de forma parecida a un mapa de servicios.

**¿Qué temáticas se abordan?**

- **Salud general de la aplicación** y su **arquitectura**.
- **Rendimiento y confiabilidad** (KPIs de error y latencia con umbrales).
- **Disponibilidad de terceros** (estado de AWS y Azure).
- **Costos** cloud (FinOps).
- **Multicloud** en una sola vista.
- **Alertamiento proactivo** y **comunicación** del estado al equipo.
