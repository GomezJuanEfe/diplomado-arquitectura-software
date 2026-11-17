# Cloud Native - Metodología

> Resumen de la *Presentación 3 - Cloud Native Metodología*.

Construir aplicaciones nativas de la nube requiere una metodología distinta a la convencional. La metodología de los **Doce Factores** tiene lagunas y no cubre todo el ciclo de vida, por eso se propone un proceso en fases: **diseño → desarrollo → ejecución → gestión → observabilidad**.

## 1. Diseño de la aplicación

- No se empieza a programar de inmediato: primero se identifican las **capacidades de negocio** y las **dependencias externas**.
- Se usa **Domain-Driven Design (DDD)**:
  - Se analiza el dominio (retail, salud…) y se definen **contextos delimitados** (ventas, RRHH, soporte…).
  - Cada contexto se divide en **agregados** (grupos de objetos de dominio tratados como una unidad).
  - Se suele empezar con un servicio por contexto delimitado y luego dividirlo por agregados.
- Al cerrar el DDD se definen las interfaces de servicio y los estilos de comunicación.

## 2. Desarrollo de la aplicación

| Práctica | Idea clave |
|---|---|
| **Código base independiente** | Cada microservicio tiene su repositorio; un solo código base con múltiples despliegues (dev, staging, prod). |
| **Configuraciones desacopladas** | El código es agnóstico al entorno; la configuración cambia por despliegue. |
| **Pruebas independientes** | Pruebas unitarias por microservicio + **pruebas de integración** del sistema completo. |
| **Contenerización** | No es obligatoria pero sí muy útil. Hace los servicios inmutables (se descartan las instancias defectuosas en vez de repararlas) y exige arranque rápido y apagado elegante. Kubernetes aporta *readiness* y *liveness checks*. |
| **Conectividad, composiciones y APIs** | Ver abajo. |
| **Automatización** | CI, CD, IaC y entrega continua para pruebas, build, release y despliegue. |

### Conectividad, composiciones y APIs

- **Interacciones basadas en servicios**: todo se expone y consume como servicio (incluidos los *servicios de respaldo*, como bases de datos o brokers). La abstracción de servicio permite descubrimiento dinámico y balanceo de carga (p. ej. un Service de Kubernetes).
- **Comunicación entre servicios y composiciones**: a través de la red con distintos patrones y protocolos (composiciones, productores y consumidores de eventos). Hay que incluir capacidades transversales: seguridad, comunicación resiliente (**circuit breakers, reintentos con backoff, timeouts**), enrutamiento, métricas y trazas. Algunas pueden delegarse a la plataforma.
- **Capacidades como APIs gestionadas**: un API Gateway y un plano de gestión aportan seguridad, limitación, caché, versionado, monetización y portal de desarrolladores. Kubernetes no las incluye; se necesitan tecnologías de gestión de APIs.

## 3. Ejecución en un entorno dinámico

- La aplicación debe correr en cualquier entorno sin cambiar código.
- La orquestación (Kubernetes) asume aprovisionamiento, gestión de recursos, inmutabilidad y autoescalado, incluso en escenarios multicloud.
- Las aplicaciones corren **sin estado**; si necesitan estado, se guarda en un almacén externo, desacoplado del ciclo de vida del contenedor.

## 4. Plano de control para la gestión dinámica

Capa central de gestión (interfaz web + API REST o RPC) para controlar el comportamiento de los entornos. Es el punto de interacción de DevOps y desarrolladores.

## 5. Observabilidad y monitoreo

- **Observabilidad**: capacidad de comprender y explicar el estado de un sistema **sin desplegar código nuevo**.
- Sirve para resolver problemas, registrar transacciones de negocio, detectar anomalías e identificar patrones.
- Sus pilares: **registros (logs), métricas, rastreo (trazas) y visualización de servicios**.
- Los proveedores cloud los ofrecen como servicios gestionados; normalmente basta con habilitar agentes o bibliotecas cliente.

## Arquitectura de referencia

Varios microservicios con distintos patrones de comunicación, cada uno con su propio almacén de datos, y un broker de eventos compartido o privado. Las capacidades de negocio se exponen por un **API Gateway**. Los servicios basados en eventos (EDA) necesitan un broker de eventos. El procesamiento de flujos (*streaming*) usa patrones y tecnologías propios.
