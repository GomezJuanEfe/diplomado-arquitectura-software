# Participación en el foro - Módulo 4: Observabilidad en Arquitecturas Cloud

¡Hola a todos!

Comparto mi análisis del workshop *AWS Serverless Observability*. El workshop parte de una aplicación sencilla (API Gateway + 3 funciones Lambda + DynamoDB), desplegada con AWS SAM, y sobre ella va incorporando los tres pilares de la observabilidad: **métricas, logs y trazas**.

## El camino recorrido: descubrimientos, retos y soluciones

**Descubrimientos.** Lo que más me llamó la atención es la diferencia entre **monitoreo** y **observabilidad**: el monitoreo responde *qué* está fallando, la observabilidad ayuda a entender *por qué*. También me pareció muy valioso ver que las métricas no tienen que ser solo de infraestructura. Con pocas líneas de código el workshop publica métricas de negocio (`SuccessfulPutItem`, `FailedPutItem`, `ColdStart`), las visualiza en CloudWatch, crea una **alarma que notifica por correo vía SNS** y arma un **dashboard** que combina métricas de negocio con métricas propias de Lambda (duración, invocaciones, errores, concurrencia).

**Retos y cómo se resuelven.** El workshop plantea varios obstáculos prácticos que vale la pena tener en cuenta:

- **Permisos IAM**: el despliegue falla si el usuario no tiene permisos para crear recursos de API Gateway, IAM o SNS. Se resuelve agregando las políticas necesarias. En un entorno real lo correcto es aplicar **mínimo privilegio** y no políticas *full access*.
- **Stacks en `ROLLBACK_COMPLETE`**: después de un despliegue fallido, hay que **eliminar el stack** en CloudFormation antes de volver a desplegar.
- **Perfil de AWS**: si no se indica `--profile`, SAM toma el perfil por defecto y falla con un error de token.
- **Partes desactualizadas**: Cloud9 ya no está disponible y el proyecto CDK que crea los dashboards no compila con las versiones actuales de TypeScript y CDK. La alternativa es trabajar desde la cuenta propia con VS Code y **crear el dashboard manualmente**.
- **Ventanas de tiempo**: las métricas y alarmas se agregan en periodos de 5 minutos, así que los resultados no se ven al instante.

## ¿Qué conceptos aprendidos o nuevos me llevo?

- **Observabilidad como atributo de calidad**: un sistema no solo debe funcionar, debe poder **medirse y explicarse** desde dentro.
- **Cold start** en Lambda y cómo medirlo con una métrica propia.
- **Métricas síncronas vs. asíncronas**: publicar una métrica dentro del flujo de negocio (`putMetric`) puede hacer que un fallo del servicio de métricas afecte la operación. El **Embedded Metric Format (EMF)** lo evita enviando las métricas como **logs estructurados**.
- **Alarmas** con umbrales, *datapoints* y manejo de datos faltantes, como base para actuar de forma proactiva.
- **CloudWatch Logs Insights** para consultar y agregar logs y llevar esas consultas a un dashboard. Las métricas dan el total y los logs permiten desglosarlo.
- **Tracing distribuido con X-Ray**: el **mapa de servicios** (API Gateway → Lambda → DynamoDB / SNS → Lambda) y la **línea de tiempo** de cada petición. Por ejemplo, en una petición de 1.8 s, unos 790 ms se iban en la inicialización de la Lambda y unos 600 ms en la consulta a DynamoDB.
- **Subsegmentos, anotaciones y metadatos** para enriquecer las trazas y poder filtrarlas.
- **Optimizar es ahorrar**: en serverless, bajar una función de 10 s a 5 s reduce su costo de ejecución a la mitad. Pero la observabilidad también cuesta, así que hay que buscar un **balance** y enfocarse en los procesos críticos.

## En mi organización o experiencia, ¿había visto un esquema de observabilidad similar?

No algo como lo del workshop. Trabajo como desarrollador *full stack* y me encargo de crear y mantener los sitios web de la empresa, alojados en **WP Engine** y **SiteGround**. Cuando algo falla, lo que hacemos es **revisar los logs directamente en el servidor**. Los proveedores de hosting ofrecen algunas métricas en sus paneles, pero son métricas generales del servicio, no métricas propias de nuestras aplicaciones ni de negocio.

### ¿Encuentro alguna diferencia?

Sí, bastantes:

- **Reactivo vs. proactivo**: nosotros revisamos los logs cuando ya hay un problema reportado. En el workshop, las alarmas avisan cuando se supera un umbral, incluso antes de que el usuario lo note.
- **Métricas del proveedor vs. métricas propias**: las del hosting son genéricas. En el workshop la aplicación publica sus **propias métricas**, incluidas las de negocio, y eso permite medir lo que realmente importa.
- **Logs dispersos vs. centralizados**: revisar logs en el servidor es manual y difícil de correlacionar. En el workshop los logs están **centralizados y estructurados**, se pueden consultar con queries y llevar a un dashboard.
- **Sin trazas**: no tenemos forma de seguir una petición de punta a punta. Si un sitio está lento, encontrar si el problema es un plugin, la base de datos o un servicio externo se hace a prueba y error.

### ¿Creo que faltan puntos por mejorar?

Sí. A partir de lo visto, aplicaría en mi contexto:

- **Centralizar los logs** en una sola herramienta en lugar de revisarlos en cada servidor, para poder buscar y correlacionar eventos.
- **Alertas básicas** de disponibilidad y errores para enterarnos antes que los usuarios.
- **Métricas de negocio** de los sitios (formularios enviados, conversiones, errores en procesos clave) junto a las métricas técnicas.
- **Monitoreo de rendimiento (APM)** o algo de tracing en los flujos críticos, para encontrar cuellos de botella con datos y no por intuición.
- Definir **qué vale la pena observar** según el impacto y el costo, empezando por lo crítico.

## Análisis del dashboard *CTO Application Overview* (SquaredUp)

### ¿Qué métricas, logs y trazas observo?

- **Métricas**:
  - KPI de **tasa de error** de las Lambdas (0.14 % promedio en 15 min).
  - KPI de **latencia** de API Gateway (~1.2 s).
  - **Duración de las peticiones** a lo largo de 7 días.
  - **Conexiones** de Azure Relay por día.
  - **Latencia de login** (B2C), que dejaba el dashboard en *warning* al superar el umbral de 2 s.
  - **Costos cloud** del último mes por proveedor (AWS y Azure).
- **Logs / eventos**: no se ven logs de la aplicación como tal. Sí hay un registro de **eventos e incidentes de los proveedores** (vía IsDown) en las regiones que usan, con su severidad y si ya se resolvieron.
- **Trazas**: no hay trazas distribuidas explícitas. Sin embargo, el **diagrama de arquitectura con estado de salud** cumple un rol parecido al *Trace Map* de X-Ray: muestra el flujo entre componentes (CloudFront → API Gateway → Lambda → DynamoDB y servicios de Azure) y cuál está afectado. Desde cada componente se puede profundizar al detalle.

### ¿Qué temáticas se abordan?

- **Salud general de la aplicación** a partir de su arquitectura.
- **Rendimiento y confiabilidad**: errores y latencia con umbrales definidos según su línea base.
- **Disponibilidad de terceros**: si AWS o Azure tienen incidentes que los afecten.
- **Costos (FinOps)**: gasto mensual consolidado de ambos proveedores.
- **Multicloud** en una sola vista, sin saltar entre consolas.
- **Alertamiento y comunicación**: notificaciones a correo, Slack, Teams o ServiceNow, y la opción de compartir el dashboard.

Me pareció un gran ejemplo de lo visto en el módulo: un dashboard pensado para **tomar decisiones rápido**, que mezcla **resumen y detalle** y conecta lo técnico (errores, latencia) con el negocio (costos, impacto de los proveedores). Su principal oportunidad de mejora sería agregar **trazas y logs de la aplicación** para pasar del *qué* falla al *por qué*. Y, comparado con mi día a día, deja claro cuánto se gana al pasar de revisar logs en el servidor a tener una vista como esta.

¡Quedo atento a sus comentarios y experiencias!
