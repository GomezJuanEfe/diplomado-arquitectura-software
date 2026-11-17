# Cloud Native - Patrones de gestión de APIs

> Resumen de la *Presentación 4 - Cloud Native: patrones de gestión de APIs*.

## Conceptos básicos

- Las capacidades de negocio se exponen a consumidores **externos** (clientes, socios) e **internos** (otros equipos) mediante APIs.
- **Gestión de APIs**: crear, gestionar, proteger, analizar y ampliar APIs sobre microservicios existentes.
- Una **API / proxy de API** expone una capacidad de forma gestionada, puede tener uno o varios microservicios detrás y corre en un runtime de gateway. **La lógica de negocio va en los microservicios**; la API solo aporta seguridad, regulación, versionado, etc.
- Beneficios: mayor compromiso de los consumidores, integración con otros servicios, monetización, analítica de negocio y exposición segura y resiliente.

## Roles

| Rol | Responsabilidad |
|---|---|
| Creador / desarrollador de API | Aprovisiona APIs y su documentación; no gestiona el ciclo de vida. |
| Editor (publisher) de API | Controla el ciclo de vida, las suscripciones, la monetización y las estadísticas. A menudo se fusiona con el creador. |
| Desarrollador de aplicaciones | Descubre APIs en el portal, se suscribe, obtiene tokens y las invoca. |
| Administrador del plano de control | Opera la plataforma: roles, bases de datos y seguridad. |

**Ciclo de vida típico**: el creador construye la API en el gateway → el editor la publica en el portal → el desarrollador se suscribe → las aplicaciones consumen por el gateway → editores y creadores observan y gestionan.

## Componentes

- **API Gateway**: puerta de entrada; acepta las llamadas y valida tokens, certificados y credenciales.
- **Plano de gestión de API**: define APIs y productos, controla el ciclo de vida y aplica políticas (throttling, caché, seguridad, versionado), usuarios y visibilidad.
- **Portal de desarrolladores**: "tienda" de APIs para descubrir, probar, suscribirse, valorar y obtener claves. También da analítica de uso.

## Patrones de API Management

### 1. API Gateway

- Capa separada delante de los microservicios; actúa como **fachada**. No todo microservicio debe exponerse.
- Puede ser **ascendente** (de microservicios a API) o **descendente** (primero se diseña la API).
- Ejemplo: una API de pedidos sobre servicios compuestos con orquestación o coreografía.
- **Consideraciones**:
  - No poner lógica de negocio ni orquestaciones en la capa de API.
  - Al inicio puede bastar el gateway, sin plano de control ni portal.
  - Puede haber varias capas para consumidores internos y externos.
  - Un gateway monolítico puede volverse pesado (de ahí el siguiente patrón).
  - Las APIs suelen exponerse por REST/OpenAPI o GraphQL.

### 2. API Microgateway

- Gateway **distribuido**: cada API tiene su propio runtime ligero en un contenedor.
- Se escala de forma independiente (p. ej. la API de búsqueda sin escalar la de pedidos).
- Todo se gestiona desde un plano central y un portal único.
- Requiere soporte de la plataforma y orquestación (Kubernetes). En la nube suele ser transparente.

### 3. Service Mesh Sidecar como API Gateway

- Si ya hay una **malla de servicios**, las funciones del gateway se delegan a los **proxies sidecar** de cada microservicio.
- Los sidecars se controlan desde el plano de gestión de API y el plano de control de la malla.
- Los usuarios difieren: DevOps controla la malla y los creadores/editores controlan las APIs.
- Es una de las arquitecturas distribuidas **más complejas**; conviene usar un servicio cloud que la gestione.

## Patrones de consumo de APIs (frontend ↔ backend)

| Patrón | Cuándo usarlo | Consideraciones |
|---|---|---|
| **Comunicación directa** | Apps pequeñas con pocos servicios y sin presupuesto para gestión de APIs. Los microservicios quedan públicos detrás de un balanceador. | Acoplamiento estrecho, riesgos de seguridad y ningún punto central para capacidades transversales. Migrar a API Gateway al crecer. |
| **Consumo a través del API Gateway** | La opción más adoptada. El frontend consume APIs gestionadas con un contrato definido. | El gateway es la fachada: el backend cambia sin romper el frontend. Los frontends cumplen las políticas de seguridad y throttling. |
| **Backend for Frontends (BFF)** | Experiencias distintas por tipo de cliente (móvil, web, escritorio) con necesidades muy diferentes. | API a medida por frontend, gestionada por su equipo y con ciclo de vida propio. Añade sobrecarga y proliferación de APIs; usarlo solo si mejora mucho la experiencia. |

## Tecnologías

- **On‑premise**: Kong, MuleSoft, Red Hat 3scale, WSO2 API Manager.
- **Cloud**: Google Apigee, Azure API Management, **Amazon API Gateway**, MuleSoft Anypoint Platform.
