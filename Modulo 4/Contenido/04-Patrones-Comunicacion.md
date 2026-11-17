# Cloud Native - Patrones de comunicación

> Resumen de la sección **Cierre** y de la descripción de la *Guía PDF. Cloud native - Patrones de comunicación* (27 páginas).
>
> ⚠️ Este archivo resume solo lo que dice la página del curso sobre la guía. El texto del PDF no se extrajo.

## Contexto

Una aplicación nativa de la nube es una colección de microservicios conectados entre sí y con sistemas externos. Con la proliferación de microservicios y requisitos de negocio cada vez mayores, **una comunicación sólida entre servicios es uno de los retos más difíciles** de estas arquitecturas.

## Temas que cubre la guía

- **Mensajería síncrona**
  - Request-Response
  - Remote Procedure Calls (RPC)
- **Mensajería asíncrona** basada en *brokers* de mensajería
- **Patrones de definición** de servicios (contratos e interfaces)
- **Tecnologías de implementación**
- **Testing** de la comunicación entre servicios
- **Seguridad**
- **Observabilidad**
- **DevOps**

## Ideas clave relacionadas (de las otras presentaciones)

- Los microservicios son *smart endpoints* unidos por *dumb pipes* (comunicación ligera, sin ESB).
- La comunicación debe ser **resiliente**: circuit breakers, reintentos con backoff y timeouts.
- Cada enlace de comunicación puede apoyarse en patrones de fiabilidad, seguridad y enrutamiento.
- La composición de servicios se implementa con **orquestación** o **coreografía**.
- Las arquitecturas basadas en eventos (EDA) necesitan un **broker de eventos**.
