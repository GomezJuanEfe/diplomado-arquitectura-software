# Workshop Parte 1 - Introducción

> Resumen del video [Workshop Parte 1 Introducción](https://youtu.be/VGu92qtteJ4) (7:17), basado en la transcripción y las diapositivas.

## ¿Por qué hablar de observabilidad?

- Ha sido un tema **subestimado** en los últimos años, pero hoy es clave para **sostener y escalar** los sistemas tecnológicos y permitir que crezcan con el tiempo.
- Preguntas que motivan el tema:
  - ¿Qué pasa cuando un sistema en producción empieza a fallar?
  - ¿Cómo sabemos qué parte del sistema está afectada?
  - ¿Cómo identificamos rápido la **causa raíz**?
- Aplica a sistemas distribuidos, microservicios, arquitecturas orientadas a eventos e incluso monolitos complejos.
- **No basta con que el sistema funcione: debe ser comprensible** de punta a punta. Hay que entender qué pasa cuando termina una transacción, cuando se ejecuta un comando o cuando corre un proceso de negocio completo.

## ¿Qué es la observabilidad?

**La capacidad de un sistema para permitir que entendamos qué está ocurriendo internamente**, apoyándose en distintas herramientas.

| Monitoreo | Observabilidad |
|---|---|
| Responde **qué** está fallando. | Responde **por qué** está fallando. |

Son **complementarios** y juntos dan una visión global del sistema.

### Por qué es clave en la práctica

- Con cientos de microservicios, múltiples bases de datos y colas de eventos, no tener visibilidad es como **volar un avión a ciegas**.
- Las empresas con servicios **24x7** (Netflix, Amazon, Google, Meta) no pueden permitirse caídas largas ni servicios lentos.

## La observabilidad como facilitador de la "buena arquitectura"

La diapositiva del video la presenta en cinco principios:

| Principio | Qué implica |
|---|---|
| **Norte definido y camino documentado** | Hoja de ruta documentada, posibles evoluciones y mantenimiento, limitaciones conocidas. |
| **Excelencia operativa** | Despliegues constantes y pequeños que acotan las causas de fallo; aprender de los errores y anticiparlos. |
| **Evitar martillos de oro** | Ninguna arquitectura resuelve todo: cada una responde a una necesidad de negocio o técnica concreta y solo a ciertos atributos de calidad. |
| **Eficiencia y costos como parte de su esencia** | Buscar siempre mejor desempeño a menor costo; refinar y optimizar. |
| **Observar y aprender** | Los sistemas deben poder **medirse y cuantificarse**, es decir, ser *observables*. |

La observabilidad es lo que permite saber **dónde mejorar, optimizar o reducir costos**, y hacer crecer el sistema de forma orgánica.

## ¿Qué se usará? Herramientas de observabilidad en AWS

| Pilar | Herramienta | Qué es | Para qué sirve |
|---|---|---|---|
| **Logs** | CloudWatch Logs | Registro inmutable y con marca de tiempo de eventos discretos. | Descubrir comportamientos emergentes e impredecibles. |
| **Métricas** | CloudWatch Metrics | Representación numérica de datos medidos en intervalos de tiempo. | Identificar tendencias, modelado matemático y predicción. |
| **Trazas** | AWS X-Ray | Serie de eventos distribuidos relacionados que codifican el flujo completo de una petición. | Ver el camino que recorre una petición y la estructura de la solicitud. |

- El workshop usa **AWS**, pero los conceptos son transversales: cualquier nube, entorno on-premise o aplicación.
- Objetivo: entender **para qué sirve cada pilar**, desde la práctica, para saber qué implementar al diseñar una solución como arquitecto.

## Instalaciones requeridas

| Herramienta | Para qué | Enlace |
|---|---|---|
| Visual Studio Code | Editor sugerido | [code.visualstudio.com](https://code.visualstudio.com/) |
| AWS CLI | Simplificar interacciones con AWS | [Guía de instalación](https://docs.aws.amazon.com/es_es/cli/latest/userguide/getting-started-install.html) |
| Git (consola o GitHub Desktop) | Clonar el repositorio del workshop | [git-scm.com](https://git-scm.com/book/es/v2/Inicio---Sobre-el-Control-de-Versiones-Instalaci%C3%B3n-de-Git) / [GitHub Desktop](https://docs.github.com/en/desktop/installing-and-authenticating-to-github-desktop/installing-github-desktop) |
| Cuenta de AWS | La misma usada en el curso | [Registro](https://signin.aws.amazon.com/signup?request_type=register) |
| Docker | Los despliegues usan contenedores | [Docker Desktop](https://www.docker.com/products/docker-desktop/) |
| Postman o terminal con curl | Probar los endpoints | [Postman](https://www.postman.com/downloads/) |
| AWS Toolkit | Integraciones y despliegue *serverless* | [AWS Toolkit para VS Code](https://docs.aws.amazon.com/toolkit-for-vscode/latest/userguide/connect.html) |
| Warp o terminal de preferencia | Ejecutar comandos (también PowerShell) | [warp.dev](https://www.warp.dev/) |

### Consideraciones del entorno

- El video se desarrolla sobre **Windows**, por ser el sistema más usado, así que hay que seguir las instrucciones al pie de la letra.
- El workshop original está diseñado para **Linux**. En Linux o macOS será más sencillo ejecutar ciertos pasos.
