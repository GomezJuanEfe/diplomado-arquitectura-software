# Fundamentos de la arquitectura de Software

## Definiciones

>The software architecture of a system is the set of structures needed to reason about the system. These structures comprise software elements, relations among them and properties of both.

Architecture in practice 4th Ed.

>Architecture is about the important stuff…whatever that is...

Ralph Johnson

>An architecture is the set of significant decisions about the organization of a software system, the selection of structural elements and their interfaces by which the system is composed, together with their behavior as specified in the collaborations among those elements, the composition of these elements into progressively larger subsystems, and the architectural style that guides this organization -- these elements and their interfaces, their collaborations, and their composition.

Philippe Kruchten [RUP 2023]

>Architecture is the fundamental organization of a system embodied in its components, their relationships to each other, and to the environment, and the principles guiding its design and evolution.

IEEE  1471 - 2000


## Implicaciones de la definición

**La arquitectura es un conjunto de estructuras de software**

Una estructura en set de elementos unidos por una relación. Los sistemas de software están compuestos por muchas estructuras y no hay una estructura única que pueda reclamar se la arquitectura.

**La arquitectura es una abstracción**

Dado que la arquitectura consta de estructuras, y las estructuras se componen de elementos y relaciones; en consecuencia, tenemos que una arquitectura comprende elementos de software y cómo estos elementos se relacionan entre sí. Esto significa que la arquitectura, específica e intencionalmente, omite cierta información sobre los elementos que no son útiles para el razonamiento del sistema. De modo que la arquitectura es, en esencia, la abstracción de un sistema que selecciona ciertos detalles y omite otros.

**Arquitectura vs. Diseño**

La arquitectura es diseño, pero no todo lo que es diseño es arquitectura. Es decir que no todas las decisiones de diseño terminan por estar vinculadas a la arquitectura dado que es, después de todo, una abstracción y dependen entonces de la discreción y buen juicio de los diseñadores posteriores, e incluso, de quienes la implementarán.

**Todo sistema tiene una arquitectura**

Todo sistema tiene una arquitectura, porque todos los sistemas tienen elementos y relaciones. Esto demuestra que existe una diferencia entre la arquitectura de un sistema y la representación de ella. Dado que una arquitectura puede existir independientemente de su descripción o especificación, esto muestra la importancia de la documentación de la arquitectura. 

**No todas las arquitecturas son buenas arquitecturas**

Nuestra definición es indiferente sobre si la arquitectura de un sistema es buena o mala. Una determinada arquitectura puede favorecer o dificultar la consecución de necesidades importantes para el sistema.

**La arquitectura incluye el comportamiento**

El comportamiento de cada elemento es parte de la arquitectura en la medida en que el comportamiento puede ayudarnos a pensar acerca del sistema. El comportamiento de los elementos refleja cómo interactúan entre ellos y con el ambiente.

# Leyes y alcance de la arquitectura de Software

## Leyes de la arquitectura de Software

*Primera ley:* Todo en la arquitectura de Software es un intercambio (Trade-off)

*Segunda ley:* El porqué es más importante que el cómo

## Alcance de la arquitectura de Software

- Las responsabilidades de un arquitecto de software abarcan habilidades técnicas, habilidades blandas, conciencia operativa y muchas otras.

- La arquitectura consiste en la estructura combinada con las características de la arquitectura ("-ilidades"), las decisiones de arquitectura y los principios de diseño.

- La estructura se refiere al tipo de estilos de arquitectura utilizados en el sistema.

- Las características de la arquitectura se refieren a las  "-ilidades" que el sistema debe soportar.

- Las decisiones de arquitectura son reglas para construir sistemas.

- Los principios de diseño son pautas para construir sistemas. 

![software architecture](image.png)

Credits: Fundamentas of Software Architecture

# La importancia de la Arquitectura de Software 

En la arquitectura de software, las decisiones clave definen la estructura y el comportamiento del sistema, mientras que las restricciones actúan como límites que guían el diseño y la implementación. Comprender las restricciones del negocio, las limitaciones tecnológicas y los recursos disponibles es esencial para tomar decisiones arquitectónicas efectivas. Estas decisiones no solo configuran el sistema desde su creación, sino que también impactan su evolución a lo largo del tiempo, influyendo en aspectos cruciales como la performance, la seguridad y la facilidad de mantenimiento del software. 

# Restricciones y decisiones de la arquitectura

## Restricciones de arquitectura
En la arquitectura de software, las restricciones son cierto tipo de regalas o limitaciones que dictan ciertos aspectos del diseño y la implementación del sistema.

Estas restricciones pueden originarse a partir de requerimientos específicos del negocio, limitaciones tecnológicas, o el capital humano disponible, entre otros factores. Comprender y gestionar estas restricciones es crucial para el éxito de un proyecto de software, ya que influyen significativamente en las decisiones arquitectónicas (Bass, Clements, y Kazman, 2015).

### Restricciones de negocio
Las restricciones de negocio se derivan de las necesidades, estrategias, y objetivos específicos de la organización que implementa el sistema. Estas incluyen:

**Requisitos de mercado:** como la necesidad de lanzar un producto antes de una fecha específica para captar una oportunidad de mercado.

**Presupuesto:** limitaciones en los recursos financieros disponibles que pueden afectar la elección de tecnologías o la capacidad de implementar ciertas características.

**Regulaciones y cumplimiento legal:** normativas que el software debe cumplir, lo que puede limitar las opciones de diseño o requerir ciertas funcionalidades.

### Restricciones de tecnología

Las restricciones tecnológicas están relacionadas con las herramientas, plataformas, y estándares existentes que deben ser utilizados en el desarrollo del sistema. Estas incluyen:

**Compatibilidad con sistemas existentes:** requisitos para que el nuevo sistema funcione con sistemas legados o tecnologías específicas preexistentes.

**Infraestructura disponible:** las capacidades de la infraestructura actual que pueden limitar el rendimiento o la escalabilidad del sistema.

**Tecnologías obligatorias:** Decisiones corporativas o de proyecto que requieren el uso de ciertas herramientas o tecnologías, a veces por razones de seguridad, soporte o acuerdos preexistentes con proveedores.

### Restricciones de personas / conocimiento

Estas restricciones están ligadas a las habilidades, experiencia y número de personal disponible para trabajar en el proyecto; estas abarcan lo siguiente:

**Experiencia del equipo:** las tecnologías o metodologías que el equipo domina pueden influir en la arquitectura elegida.

**Disponibilidad de especialistas:** la falta de expertos en un área tecnológica específica puede limitar las opciones de diseño o requerir capacitación adicional, impactando los plazos y costos.

**Cultura organizacional:** las preferencias o aversiones de la organización hacia ciertas prácticas o tecnologías también pueden imponer restricciones en el diseño del sistema.

## Decisiones de arquitectura

Las decisiones de arquitectura son elecciones fundamentales que definen la estructura y el comportamiento de un sistema de software. Estas decisiones abordan problemas críticos del diseño del sistema y establecen las bases sobre las cuales el software sera construido y evolucionará a lo largo del tiempo. (Bass, Clements, y Kazman, 2012).

Estas, son cruciales porque configuran el marco dentro del cual se tomarán todas las demás decisiones técnicas. Afectan la selección de tecnologías, la formulación de políticas de desarrollo y mantenimiento y las estrategias de implementación. Al definir cómo se organizan y se interconectan los componentes del software, estas decisiones impactan directamente en la capacidad del sistema para cumplir con los requisitos funcionales y no funcionales, influenciando atributos de calidad como la performance, la seguridad, y la facilidad de mantenimiento

### Consecuencias de malas decisiones de arquitectura

**Rigidez del Sistema:** una arquitectura mal diseñada puede ser difícil de modificar y expandir, lo que puede obstaculizar la adaptación del sistema a nuevos requisitos o tecnologías.

**Costos incrementados:** decisiones equivocadas pueden resultar en retrabajos costosos y aumentos en el tiempo de desarrollo y mantenimiento.

**Degradación del rendimiento:** una arquitectura que no considera adecuadamente los atributos de rendimiento necesarios puede llevar a un sistema que no cumple con las expectativas de los usuarios o los estándares de la industria.

### Consecuencias de buenas decisiones de arquitectura

**Consistencia y cohesión:** facilita la integración de diferentes componentes y promueve un enfoque uniforme en el desarrollo.

**Claridad y comunicabilidad:** hace que la arquitectura sea más fácil de entender y comunicar a todas las partes interesadas, reduciendo así los riesgos de malentendidos y errores en las fases de desarrollo.

**Flexibilidad y escalabilidad:** permite que el sistema se adapte más fácilmente a cambios en los requisitos o en el entorno tecnológico.

# Atributos de calidad

![Atributos de calidad](./atributos%20de%20calidad.png)

Las cuatro flechas son las cuatro dimensiones que, juntas, forman la arquitectura:

**Estructura (abajo):** qué estilo usas — monolito, microservicios, capas.

**Decisiones de arquitectura (izquierda):** reglas duras. "La capa de presentación nunca habla directo con la base de datos."

**Principios de diseño (derecha):** guías flexibles. "Preferimos mensajería asíncrona entre servicios."
Características de la arquitectura (arriba): las "-ilidades".

Lo que está en el centro es la lista de características, y ahí está la idea importante: las otras tres dimensiones existen para sostener esas características. Eliges microservicios porque necesitas escalabilidad y agilidad, no al revés.

## Características de Arquitectura

Muchas organizaciones definen estas características del software con una variedad de términos, incluidos requisitos no funcionales y atributos de calidad.

Una característica de arquitectura cumple 3 criterios:

- Especifica una consideración de diseño sin dominio
- Influye en algún aspecto estructural del diseño
- Es crítico o importante para el éxito de la aplicación

>### Los tres criterios en un caso concreto
>
>Imagina una plataforma de venta de entradas para conciertos. Casi todo el tiempo tiene poco tráfico, pero el día que salen a la venta las entradas de un artista grande, en cinco minutos entran cien mil personas.
>
>Tomemos **elasticidad** (capacidad de absorber picos bruscos de carga) y pasémosla por los tres filtros:
>
>- ¿Es una consideración sin dominio? Sí. Puedes decir "el sistema debe ser elástico" sin mencionar entradas, conciertos ni artistas.
>- ¿Influye en la estructura? Muchísimo. Te obliga a servicios sin estado, a una cola para serializar las compras, a no guardar la sesión en memoria del servidor, a separar el servicio de compra del resto para escalarlo solo a él.
>- ¿Es crítica para el éxito? Totalmente. Si el sitio se cae en esos cinco minutos, el negocio no existe.
>
>Pasa los tres. Es una característica de arquitectura.
>
>### Ahora los casos que fallan
>
>Esto es lo que hace útil el modelo, porque los tres filtros existen para dejar cosas fuera.
>
>**Falla el primero:** "calcular el IVA del 19% sobre el valor de la entrada". Es dominio puro, un requisito funcional. Cambia el código, no la arquitectura.
>
>**Falla el segundo:** "el equipo debe usar nombres de variables en camelCase". No es dominio, pero no mueve una sola pieza de la estructura. Es una convención, no una característica de arquitectura.
>
>**Falla el tercero, y este es el interesante:** toma la misma elasticidad y llévala a un sistema de nómina interno que usan doce personas de RR.HH. una vez al mes. Sigue siendo sin dominio. Sigue influyendo en la estructura si decides soportarla. Pero no es crítica para nada, porque ese pico de carga nunca va a ocurrir.
>
>Ahí está el valor del tercer criterio. Soportar una característica siempre cuesta: más complejidad, más costo, y casi siempre sacrificas otra cosa. Diseñar esa nómina para elasticidad sería pagar un precio alto a cambio de nada. Es la primera ley de tus notas, aplicada: todo es un trade-off.
>

![Características de arquitectura](./Caracteristicas%20de%20arquitectura.png)

El triángulo son los tres criterios, uno por lado, y el mensaje de la figura es que los tres deben cumplirse a la vez. Un triángulo se sostiene solo si tiene sus tres lados; quita uno y no hay figura. Eso es todo lo que dice la forma.

Richards y Ford distinguen entre características explícitas, las que sí aparecen escritas en el documento de requisitos, y características implícitas, las que nadie escribe pero el sistema necesita igual. Nadie pone en un requisito "el sistema debe estar disponible" ni "los datos de la tarjeta no deben filtrarse". Se dan por supuestas. Y sin embargo son las que hunden proyectos.

## Características de la Arquitectura: Operacionales

Cómo se comporta el sistema corriendo, en producción. La pregunta de fondo: ¿responde bien, sigue vivo, aguanta la carga? Es lo que le importa al equipo de operaciones a las 3 de la mañana. Otra pregunta que me puedo hacer es ¿puedo medir esto con un cronómetro o un monitor, con el sistema corriendo en producción? Si la respuesta es sí, es operacional.

### Disponibilidad

Tiempo que el sistema debe estar **operativo** (si es 24/7, se requieren medidas para garantizar su rápida recuperación ante fallos).

### Continuidad

Capacidad de **recuperación** ante desastres.

### Rendimiento

Incluye pruebas de estrés, análisis de picos, frecuencia de uso de funciones, capacidad requerida y tiempos de respuesta.

### Recuperabilidad

**Requisitos de continuidad del negocio** (ej.: tiempo máximo para restaurar el sistema tras un desastre). Esto impacta la estrategia de backups y la necesidad de hardware redundante.

### Confiabilidad/Seguridad

Evalúa si el sistema debe ser a **prueba de fallos o es crítico**
(ej.: afecta vidas humanas o generaría pérdidas financieras significativas).

### Robustez

Capacidad del sistema para manejar errores y condiciones límite durante su ejecución, como caídas de conexión a Internet, cortes de energía o fallos de hardware.

### Escalabilidad

Capacidad del sistema para mantener su rendimiento y operatividad ante el aumento de usuarios o solicitudes.

## Características de la Arquitectura: Estructurales

Tiene que ver con la calidad interna del código y qué tan fácil es trabajar con él. La pregunta: ¿alguien que llega nuevo entiende esto? ¿Puedo cambiarlo sin romper cinco cosas? El usuario final nunca las ve; el equipo de desarrollo las sufre todos los días.

### Configurabilidad

Capacidad que tienen los usuarios finales para modificar fácilmente aspectos de la **configuración del software** mediante interfaces intuitivas.

### Extensibilidad

Grado de importancia para incorporar nuevas funcionalidades al **sistema de manera modular**.

### Capacidad de instalación

Facilidad para **desplegar el sistema** en todas las plataformas requeridas.

### Reutilización

Habilidad para aprovechar componentes comunes en **múltiples productos**.

### Localización

Soporte para múltiples idiomas en pantallas de **entrada/consulta**, informes, caracteres multibyte, así como unidades de medida y monedas locales.

### Mantenibilidad

Facilidad para **implementar cambios** y mejoras en el sistema a lo largo del tiempo.

### Portabilidad

¿Requiere el sistema ejecutarse en múltiples plataformas? (Ejemplo: ¿debe funcionar el frontend tanto con Oracle como con SAP DB?).

### Capacidad de Actualización

Facilidad para migrar rápidamente desde una versión anterior de la aplicación/solución a una versión más reciente, tanto en servidores como en clientes.

## Características del Arquitectura: Transversales (Cross-cutting)

No encajan limpio en las otras dos y atraviesan el sistema entero. No son "una parte" del sistema, son una exigencia que aplica en todas partes a la vez.

### Accesibilidad

Garantizar el acceso a todos los usuarios, incluyendo aquellos con discapacidades como daltonismo o pérdida auditiva.

### Capacidad de archivado

¿Los datos deberán archivarse o eliminarse después de un tiempo determinado? (Ejemplo: cuentas de clientes que deben eliminarse después de tres meses o marcarse como obsoletas y archivarse en una base de datos secundaria para acceso futuro).

### Autenticación

Requisitos de seguridad para verificar la identidad de los usuarios.

### Autorización

Requisitos de seguridad para controlar el acceso a funciones específicas dentro de la aplicación (por caso de uso, subsistema, página web, regla de negocio, nivel de campo, etc).

### Aspectos legales

¿Qué restricciones legislativas afectan al sistema (protección de datos, Sarbanes Oxley, GDPR, etc.)?

¿Qué derechos de reserva requiere la empresa? ¿Existen regulaciones sobre cómo debe construirse o implementarse la aplicación?

### Privacidad

Capacidad de proteger transacciones incluso del personal interno (mediante encriptación que impida el acceso a administradores de bases de datos y arquitectos de red).

### Seguridad

Requieren los datos:
- ¿Encriptación en la base de datos?
- ¿Encriptación en comunicaciones internas?
- ¿Qué protocolos de autenticación se necesitan para acceso remoto?

### Capacidad de soporte

Nivel de asistencia técnica requerido:
- Complejidad de logs necesarios
- Herramientas de diagnóstico para depuración
- Recursos para soluciones de incidencias

### Usabilidad

Consideraciones críticas:
- Curva de aprendizaje para usuarios
- Diseño intuitivo centrado en la experiencia del usuario
- Requisitos ergonómicos (Deben priorizarse como cualquier aspecto arquitectural)


## CARACTERÍSTICAS DE ARQUITECTURA: ISO/IEC 25021

###  Elementos de medida de calidad:

Define un conjunto de medidas base y derivadas recomendadas, que están destinadas a ser utilizadas durante todo el ciclo de vida del desarrollo de software. El documento describe un conjunto de medidas que se pueden utilizar como entrada para la medición de la calidad del producto de software o la calidad del software en uso.

![Software Quality Product](./Software%20Quality%20Product.png)

![Características de Arquitectura](./Caracteristicas%20de%20arquitectura_2.png)