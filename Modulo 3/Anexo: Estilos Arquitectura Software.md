# Estilos de arquitectura de software

> Notas de la presentación interactiva del curso (Contenidos obligatorios). Tres secciones: **Fundamentos**, **Patrones** y **Arquitecturas monolíticas vs distribuidas**.

---

## 1. Fundamentos

### Estilo vs patrón de arquitectura

| Concepto | Qué describe | Nivel |
|---|---|---|
| **Estilo de arquitectura** | La estructura general de cómo se organizan la interfaz de usuario y el código del backend (p. ej. en capas dentro de un monolito, o en servicios desplegados por separado) y cómo ese código interactúa con el almacén de datos. | Macroestructura del sistema |
| **Patrón de arquitectura** | Estructuras de diseño de nivel inferior que resuelven problemas concretos *dentro* de un estilo (p. ej. lograr alta escalabilidad o rendimiento en un conjunto de operaciones o entre servicios). | Bloques de construcción reutilizables |

**Jerarquía (ejemplo):**

```
Architecture style (Microservices)
├── Architecture pattern (CQRS)
├── Architecture pattern (Event forward)
└── Design patterns (Builder, Factory, Template, ...)
```

### Estilos híbridos
- Los estilos pueden **combinarse** para formar una solución completa; los híbridos son comunes porque ningún estilo resuelve todos los problemas de negocio.
- Híbridos frecuentes:
  - **Microservicios basados en eventos** — eventos entre microservicios.
  - **Microservicios basados en espacios** — unidades de procesamiento implementadas como microservicios.
  - **Micronúcleo basado en eventos** — eventos entre el sistema central y los componentes remotos enchufables (plug-ins).
- Antes de combinar, hay que entender cada estilo individual y sus puntos fuertes y débiles.

### Por qué importan los estilos
- Entenderlos ocupa buena parte del esfuerzo de un arquitecto nuevo.
- Cada estilo encarna un **conjunto conocido de trade-offs**; conocerlos permite tomar la decisión correcta para un problema de negocio concreto.

### Qué es un estilo (resumen)
- A veces llamados *patrones de arquitectura*: describen una **relación nombrada de componentes** que cubre varias características de arquitectura.
- El **nombre** del estilo funciona como abreviatura compartida entre arquitectos experimentados (igual que los patrones de diseño): captura muchos detalles entendidos.
- Por eso los arquitectos deben conocer los nombres de los estilos genéricos fundamentales.
- Un estilo describe la **topología** y las **características de arquitectura asumidas/por defecto**, tanto beneficiosas como perjudiciales.

---

## 2. Patrones fundamentales

### 2.1 Gran Bola de Barro (*Big Ball of Mud*)
- Ausencia de cualquier estructura arquitectónica perceptible. Nombre tomado del **anti-patrón** definido por **Brian Foote y Joseph Yoder (1997)**.
- Código espagueti estructurado al azar, que crece sin control y se repara con parches rápidos; la información se comparte entre partes distantes hasta volverse casi global o duplicada.
- Versión moderna: una app de scripts simple con manejadores de eventos conectados **directamente** a llamadas a la base de datos, sin estructura interna real.
- Muchas apps triviales empiezan así y se vuelven inmanejables al crecer.
- **Problemas:** cambio cada vez más difícil, despliegue, testeabilidad, escalabilidad y rendimiento. Se debe evitar a toda costa.
- **Caso ejemplo:** app web Java construida "lo más rápido posible" durante años. Visualización de acoplamiento en círculo (cada punto = clase, cada línea = dependencia; más gruesa = más fuerte). Resultado: cambiar una clase tiene efectos impredecibles en otras → cambiar da miedo.

### 2.2 Arquitectura Unitaria (*Unitary Architecture*)
Evolución histórica:
1. Al inicio solo existía la computadora y el software corría en ella.
2. Hardware y software empezaron como una sola entidad y se separaron al crecer la necesidad de capacidades más sofisticadas.
3. Con los PC, gran parte del desarrollo comercial se centró en máquinas individuales.
4. Con los PC en red aparecieron los sistemas distribuidos (cliente/servidor). Hoy hay pocas arquitecturas unitarias fuera de **sistemas embebidos** y entornos muy restringidos.

- Los sistemas tienden a crecer en funcionalidad, lo que exige **separación de responsabilidades** para mantener características operativas como rendimiento y escala.

### 2.3 Cliente/Servidor (*Client/Server*)
- Con el tiempo hubo que **particionar** un único sistema; cómo hacerlo es la base de muchos estilos.
- Estilo fundamental: separar la funcionalidad técnica entre **frontend y backend** → arquitectura de **dos niveles** o cliente/servidor. Tiene varias variantes según la época:

| Variante | Descripción |
|---|---|
| **Escritorio + servidor de BD** | Apps de escritorio ricas (p. ej. Windows) con los datos en un servidor de base de datos independiente, conectado por protocolos de red estándar. Presentación en el escritorio; el cómputo pesado en el servidor de BD. |
| **Navegador + servidor web** | Con la web moderna: navegador ↔ servidor web ↔ servidor de BD. Clientes aún más ligeros, mayor distribución dentro y fuera de firewalls. Se sigue considerando de **dos niveles**, porque servidor web y BD corren en máquinas del centro de operaciones y la UI en el navegador. |
| **Tres niveles** | Popular a fines de los 90: nivel de **BD** (servidor industrial), nivel de **aplicación** (servidor de aplicaciones, Java/.NET) y **frontend** (HTML generado y cada vez más JavaScript). Ligada a protocolos de red como **CORBA** y **DCOM**, que facilitaban arquitecturas distribuidas. |

---

## 3. Arquitecturas monolíticas vs distribuidas

Clasificación en dos tipos principales:
- **Monolíticas:** una única unidad de despliegue para todo el código.
- **Distribuidas:** múltiples unidades de despliegue conectadas mediante protocolos de acceso remoto.

> Ningún esquema es perfecto, pero todas las distribuidas comparten desafíos que no aparecen en las monolíticas, lo que hace útil esta separación.

### 3.1 Arquitectura monolítica

**Ventajas**
- Más simples de diseñar e implementar.
- Una sola unidad de despliegue → **más económicas** en coste global.
- Se desarrollan y despliegan **más rápido**.

**Desventajas** (escalabilidad, tolerancia a fallos y elasticidad)
- Un error fatal (p. ej. falta de memoria) **tumba toda la aplicación**.
- **MTTR** (tiempo medio de recuperación) y **MTTS** (tiempo medio de arranque) suelen medirse en **minutos** → recuperación lenta, que también afecta escalabilidad y elasticidad.
- Escalar exige escalar **toda** la aplicación aunque solo una parte lo necesite (balanceo de carga de varias instancias) → ineficaz y costoso.

**Ejemplos de estilos monolíticos**
- Arquitectura en capas / monolito modular
- Arquitectura de canalización (*pipeline*)
- Arquitectura de micronúcleo (*microkernel*)

### 3.2 Arquitecturas distribuidas

**Ventajas ("superpoderes")**
- **Escalabilidad y elasticidad** a nivel de servicio individual.
- **Tolerancia a fallos:** si falla un servicio, los demás suelen seguir atendiendo; los servicios caídos se recuperan rápido (a veces el usuario ni lo nota).
- **MTTS/MTTR** en **segundos** (o milisegundos), no minutos.
- **Rendimiento** (en algunos casos).
- **Agilidad:** al estar la funcionalidad en unidades desplegadas por separado, es más fácil localizar y aplicar cambios; el alcance de pruebas se limita al servicio afectado y se reduce el riesgo de despliegue.

**Desventajas**
- Sujetas a las **8 falacias de la computación distribuida** (supuestos que creemos ciertos pero son falsos). Hacen que estos sistemas sean difíciles de mantener deterministas y fiables; siguen vigentes desde que se acuñaron a finales de los 90.
- Otras dificultades:
  - Transacciones distribuidas
  - Consistencia eventual
  - Gestión del flujo de trabajo
  - Gestión de errores
  - Sincronización de datos
  - Gestión de contratos
  - …y muchas otras complejidades
- **Coste** mucho mayor, tanto de implementación inicial como de mantenimiento continuo.

**Ejemplos de estilos distribuidos**
- Arquitectura basada en eventos
- Arquitectura de microservicios
- Arquitectura basada en servicios
- Arquitectura orientada a servicios (SOA)
- Arquitectura basada en espacios

### 3.3 Las 8 falacias de la computación distribuida

1. La red es confiable
2. La latencia es cero
3. El ancho de banda es infinito
4. La red es segura
5. La topología nunca cambia
6. *Hay un solo administrador* ⚠️ (en la presentación aparece repetido "La topología nunca cambia"; la falacia clásica #6 es esta)
7. El costo de transporte es cero
8. La red es homogénea

---

## Resumen rápido

| | Monolítica | Distribuida |
|---|---|---|
| Unidades de despliegue | 1 | Varias (protocolos remotos) |
| Simplicidad / coste | ✅ Alta / bajo | ❌ Baja / alto |
| Velocidad de desarrollo inicial | ✅ | ❌ |
| Escalabilidad / elasticidad | ❌ Toda la app | ✅ Por servicio |
| Tolerancia a fallos | ❌ Un fallo tumba todo | ✅ Fallos aislados |
| MTTR / MTTS | Minutos | Segundos |
| Agilidad (cambios, pruebas, despliegue) | ❌ | ✅ |
| Complejidad operativa | Baja | Alta (falacias, transacciones, consistencia…) |
