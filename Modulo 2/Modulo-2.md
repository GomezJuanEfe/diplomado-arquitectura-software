# Requerimientos y Tácticas de Arquitectura de Software

A lo largo de este módulo se va a estudiar:

 - **El desempeño, la disponibilidad, la seguridad y la interoperabilidad**, y cómo estos atributos, al convertirse en Requerimientos Arquitecturalmente Significativos (ASR), impactoan directamente las decisiones de diseño en la arquitectura de software.

 - Vamos a aprender a documentar escenarios de calidad, aplicar tácticas arquitectónicas específicas y justificar decisiones técnicas con base en contextos reales de uso

 ## Introducción

 En este primer tema del módulo nos vamos a sumergir en los **requerimientos de calidad** que acompañan a los **requerimientos funcionales**, como la latencia, la disponibilidad y la seguridad.

 Vamos a aprender a identificar los (ASR) Requerimientos Arquitecturalemente Significativos. Vamos a entender cómo estos atributos de calidad influyen en el diseño y funcionamiento de un sistema a través de ejemplos prácticos.

 ### Requerimiento de Calidad

 En el desarrollo de software, no basta con que una funcionalidad exista. También es clave cómo se comporta esa funcionalidad bajo diferentes condiciones. Aquí es donde entran los **requerimientos de calidad**, también conocidos como **requerimientos no funcionales**.

 Estos requerimientos acompañan a los funcionales, pero les agregan condiciones específicas relacionadas:
 - Tiempo de respuesta
 - Disponibilidad
 - Confiabilidad
 - Seguridad
 - Entre otros factores

 Son fundamentales para garantizar una experiencia eficiente y segura para los usuarios, y para que el sistema cumpla su propósito en contextos reales de operación.

 **Ejemplo:** Cuando se pulse el botón *Confirmar pago*, se debe procesar la transacción financiera con la entidad correspondiente. Pero, además, se espera que este proceso:

<ul style="list-style:none">
  <li>✅ Sea rápido<li>
  <li>✅ Ocurra con alta disponibilidad</li>
  <li>✅ Solo esté habilitado para usuarios autorizados</li>
  <li>✅ Proteja los datos del usuario</li>
  <li>✅ Sea compatible con diversas entidades financieras</li>
<ul>

## Profundización

Exploraremos los atributos de **disponibilidad, desempeño y seguridad**, y cómo cada uno de ellos impacta el diseño y funcionamiento de un sistema.

### Disponibilidad (AVAILABILITY)

#### Definición
La **disponibilidad** se refiere a la propiedad del software de estar ahí listo para realizar su tarea cuando se necesita. También abarca la capacidad de un sistema para **enmascarar o reparar defectos** de modo que no se conviertan en fallas, asegurando así que el periodo de interrupción del servicio acumulativo no exceda un valor requerido durante un intervalo de tiempo específico.

Una **falla** es la **desviación de un sistema** de su especificación, donde esa desviación es visible externamente. Determinar que ha ocurrido una falla requiere algún observador externo en el entorno.

La causa de una **falla (failure)** se llama **defecto (fault)**. Un defecto puede ser interno o externo al sistema bajo consideración. Los estados intermedios entre la ocurrencia de un defecto y la ocurrencia de una falla se denominan errores. Los defectos se pueden prevenir, tolerar, eliminar o pronosticar.

#### Relación con otros atributos de calidad

La **disponibilidad** está estrechamente relacionada con la **seguridad**, pero claramente dista de ella. Un ataque de denegación de servicio (DoS) está diseñado explícitamente para hacer que un sistema falle, es decir, para que no esté disponible.

La disponibilidad también está estrechamente relacionada con el **Rendimiento**, ya que puede ser difícil saber cuándo un sistema ha fallado y cuándo simplemente responde de manera extremadamente lenta.

#### Escenario

General:

![alt text](./Escenario%20general.png)

Escenario concreto - Un servidor en una granja de servidores falla durante el funcionamiento normal y el sistema informa al operador y continúa funcionando sin tiempo de inactividad.

![alt text](./Escenario%20concreto.png)

#### Tácticas

Las **tácticas de disponibilidad** están diseñadas para permitir que un sistema prevenga o soporte fallas del sistema para que el servicio que entrega siga cumpliendo con su especificación.

Las tácticas de disponibilidad tienen 1 de 3 propósitos:

1. Detección
2. Recuperación
3. Prevención de fallas

Ver anexo para explorar las tácticas de disponibilidad.


### Desempeño (PERFORMANCE)

#### Definición

"It's about time" - se trata de eso, del **tiempo**, y de la capacidad del sistema para cumplir con los requisitos de tiempo. El hecho es que las operaciones en las computadoras toman tiempo:

- Los cálculos toman un tiempo del orden de miles de nanosegundos.
- El acceso al disco (ya sea de estado sólido o giratorio) toma un tiempo del orden de decenas de milisegundos.
- El acceso a la red toma un tiempo que va desde cientos de microsegundos dentro del mismo centro de datos hasta más de 100 milisegundos para mensajes intercontinentales.
- El tiempo debe tenerse en cuenta al diseñar su sistema para el rendimiento.

##### Concurrencia

La **Concurrencia** es uno de los conceptos más importantes de un arquitecto debe comprender. La concurrencia se refiere al número de operaciones que ocurren en paralelo.

La concurrencia ocurre cada vez que su sistema **crea un nuevo subproceso**, porque los subprocesos, por definición, son secuencias de control independientes.

**La multitarea en su sistema es compatible** con subprocesos independientes. Múltiples usuarios son compatibles simultáneamente en su sistema mediante el uso de subprocesos.

**La simultaneidad** también ocurre cada vez que su sistema se ejecuta en más de un procesador, ya sea que esos procesadores estén empaquetados por separado o como procesadores de múltiples núcleos.

#### Escenario

500 usuarios inician 2.000 solicitudes en un intervalo de 30 segundos, en condiciones normales de funcionamiento. El sistema procesa todas las solicitudes con una latencia promedio de dos segundos.

#### Tácticas de Desempeño

**Control Resource Demand**
- Manage Work Requests
- Limit Event Response
- Prioritize Events
- Reduce Computational Overhead
- Bound Execution Times
- Increase Efficiency

**Manage Resources**
- Increase Resources
- Introduce Concurrency
- Maintain Multiple Copies of Computationes
- Maintain Multiple Copies of Data
- Bound Queue Sizes
- Schedule Resources

