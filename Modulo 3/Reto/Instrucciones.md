# Instrucciones
## Actividad: Módulo 3. Reto - Diseñando Arquitecturas Resilientes 

Estimado estudiante, a continuación, se presentan los pasos para desarrollar la actividad del Módulo 3. Lee con atención y desarrolla todos los pasos, entregando las evidencias solicitadas: 

### Caso: contexto del Negocio 

Eres parte del equipo de arquitectura de Sistemas UltraSeguros S.A., una empresa líder que ofrece servicios críticos para múltiples clientes alrededor del mundo. Entre estos servicios, se encuentran la gestión de transacciones financieras, análisis de datos en tiempo real y monitoreo de infraestructuras críticas. 

El éxito de la compañía depende de un sistema capaz de mantenerse operativo incluso en las peores circunstancias. En un mundo donde los picos de tráfico, errores inesperados y fallos en componentes individuales son inevitables, tus clientes confían en que la plataforma pueda adaptarse, degradarse progresivamente y recuperarse automáticamente sin interrupciones significativas. 

La gerencia ha identificado un problema urgente: durante los picos de tráfico, el sistema tiende a fallar bajo carga, lo que afecta la experiencia del cliente y genera pérdidas millonarias. ¿Puedes diseñar una arquitectura que haga que el sistema reaccione a los fallos automáticamente y se recupere con inteligencia? 

## El Reto 

Tu misión, como arquitecto, es diseñar y construir un sistema que cumpla con las siguientes características: 

1. Degradación progresiva y recuperación automática: 

- Si el sistema detecta fallos recurrentes o altos niveles de errores, debe degradar automáticamente sus capacidades para mantener los servicios críticos en funcionamiento. 

- Cuando los indicadores de salud regresen a niveles aceptables, el sistema debe recuperar gradualmente todas sus funcionalidades. 

2. Resiliencia dinámica basada en la salud de los servicios: 

- Los cambios en los niveles de servicio deben registrarse y ser visibles en logs o métricas. 

3. Pruebas bajo carga: 

- Se proporcionará un script de pruebas que simula picos de carga, incluyendo solicitudes que generan fallos intencionales. 

- Tu sistema debe pasar por los 3 niveles de servicio (Full, Degradado, y Operación Mínima) y recuperarse exitosamente al nivel inicial y transicionar entre los mismos. 

4. Tolerancia a fallos: 

- Durante el estado de Operación Mínima, el sistema debe seguir respondiendo cuando la petición falle con un mensaje de Error claro para los usuarios: "Nivel 3: Sistema bajo mantenimiento, intente más tarde" 

- Durante el estado de Operación Mínima, el sistema debe seguir respondiendo cuando la petición se logre resolver con un mensaje de Error claro para los usuarios: "Nivel 3: Operación al mínimo" 

5. Requerimientos Técnicos 

Ten en cuenta los requerimientos técnicos necesarios: 

- **API Gateway**: actuará como punto de entrada para todas las solicitudes y debe enrutar dinámicamente las solicitudes a diferentes niveles de servicio. 

- **3 servicios diferentes por nivel de servicio:** 

  - **Nivel 1 (full)**: el sistema responde con todas sus capacidades activas. 

  - **Nivel 2 (degradado)**: funciona con un subconjunto de capacidades, priorizando los servicios esenciales. 

  - **Nivel 3 (mínimo)**: responde con un mensaje de mantenimiento. 

- **Logs y métricas**: el sistema debe registrar todos los cambios de nivel y las métricas clave. 

6. Pruebas

Se proporcionará un script de pruebas K6 que simula 6 minutos de solicitudes, distribuyendo las cargas y fallos intencionales. Tu solución debe: 

- Pasar por los 3 niveles de servicio durante la ejecución. 

- Mostrar recuperación completa al finalizar las pruebas. 

- Demostrar degradación y recuperación de forma dinámica. 

7. Entregables 

Como evidencia de la actividad, debes entregar los siguientes productos: 

- **Código fuente**:

  - Archivo comprimido en formato ZIP con toda la implementación necesaria. 

- **Documentación técnica**:

Documento en formato PDF que incluya lo siguiente:   

  - Decisiones de arquitectura: justificar por qué se eligieron los componentes y servicios utilizados. 

  - Atributo de calidad más importante: explicar cuál es y por qué fue priorizado. 

  - Diagrama de la arquitectura: representación visual clara de la solución. 

  - Tácticas de arquitectura: explicar las tácticas utilizadas para cumplir con los objetivos. 

- **Demostración en vivo o video (2.5 Puntos)**:

Formato: video en MP4 (máx. 5 minutos) o enlace a YouTube según corresponda.   

  - Ejecución funcional de la arquitectura bajo las condiciones simuladas por el script de pruebas. 

  - Explicación clara de cómo tu solución detecta fallos, degrada el sistema y se recupera. 

  - Logs y métricas que validen el comportamiento esperado. 

8. Restricciones 

- Solo se permite usar servicios de AWS. 

- No se proporcionará plantilla inicial. Debes investigar, diseñar y construir la solución desde cero. 

- No puedes modificar el script de pruebas proporcionado. 

9. Objetivo final 

Demuestra que tu solución puede mantener el sistema en funcionamiento incluso en las peores circunstancias. Solo las arquitecturas más resilientes e inteligentes superarán este reto. 

10. Detalles de las transiciones esperadas: 

  - **Inicio**: El sistema comienza en Nivel 1 (Full Service). 

  - **Degradación**:

    - Si se detectan 5 errores o más, el sistema transiciona al Nivel 2 (Degradado). 

    - Si se detectan 10 errores o más, el sistema transiciona al Nivel 3 (Mantenimiento). 

  - **Recuperación**:

    - Desde Nivel 3, cuando los indicadores vuelven a estar en un estado saludable, el sistema transiciona a Nivel 2. 

    - Desde Nivel 2, cuando los indicadores están completamente estables, el sistema regresa a Nivel 1. 