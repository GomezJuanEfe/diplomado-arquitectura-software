# Técnicas de Diagramación

> Anexo del [Módulo 3](./Modulo-3.md) — Presentación.

## Introducción

Al describir visualmente una arquitectura, el arquitecto a menudo debe mostrar diferentes **vistas**. Por ejemplo, es probable que muestre primero una descripción general de toda la topología de la arquitectura y luego profundice en las partes individuales para entrar en los detalles del diseño. Sin embargo, si muestra una parte sin indicar dónde se ubica dentro de la arquitectura general, confunde a quienes la ven.

> ***Consistencia representacional:*** la práctica de mostrar siempre la relación entre las partes de una arquitectura, ya sea en diagramas o presentaciones, **antes** de cambiar de vista.

Esta presentación cubre tres estándares de diagramación:

| Estándar | Enfoque | Uso típico |
|---|---|---|
| **UML** | Estructura y comportamiento a nivel de software. | Diagramas de secuencia y de clases. |
| **C4** | Vistas del sistema en cuatro niveles de zoom. | Documentar la arquitectura de un sistema de software. |
| **ArchiMate** | Arquitectura empresarial en capas. | Relacionar negocio, aplicaciones y tecnología. |

# UML

## Contexto

El **lenguaje unificado de modelado (UML)** fue un estándar que unificó tres filosofías de diseño que competían en la década de 1980. Se suponía que iba a ser lo mejor de todos los mundos, pero, como muchas cosas diseñadas por comité, no logró mucho impacto fuera de las organizaciones que exigían su uso. Los arquitectos y desarrolladores todavía usan los **diagramas de secuencia** y **de clases** de UML para comunicar estructura y flujo de trabajo, pero la mayoría de los otros tipos de diagramas UML han caído en desuso.

## Documentar el comportamiento

Documentar una arquitectura requiere una **documentación del comportamiento** que complemente las vistas estructurales, describiendo cómo interactúan los elementos de la arquitectura entre sí. Razonar sobre características como el potencial de un sistema para bloquearse (*deadlock*), su capacidad para completar una tarea en el tiempo deseado o su consumo máximo de memoria requiere que la descripción de la arquitectura proporcione información sobre:

- Las características de los elementos individuales y su consumo de recursos.
- Los patrones de interacción entre ellos, es decir, cómo se comportan entre sí.

## Diagrama de secuencia

Un diagrama de secuencia UML muestra una secuencia de interacciones entre instancias de elementos tomados de la documentación estructural. Es útil, al diseñar un sistema, para **identificar dónde se deben definir las interfaces**. Muestra solo las instancias que participan en el escenario que se documenta.

Tiene dos dimensiones:

- **Vertical:** representa el tiempo. Las interacciones se organizan de arriba hacia abajo.
- **Horizontal:** representa las distintas instancias.

**Ejemplo: inicio de sesión**

```mermaid
sequenceDiagram
    actor U as :User
    participant LP as :Login Page
    participant LC as :Login Controller
    participant DAO as :User Direct Access Object
    participant US as :User Session
    participant L as :Logger

    U->>LP: login
    LP->>LC: login(...)
    LC->>DAO: checkPwd(...)
    DAO-->>LC: 
    LC->>US: new
    US-->>LC: 
    LC-)L: register User Login(...)
    LC-->>LP: 
    LP-->>U: 
```

| Notación | Significado |
|---|---|
| Flecha sólida con punta llena | Mensaje síncrono |
| Flecha sólida con punta abierta | Mensaje asíncrono |
| Flecha punteada | Mensaje de retorno |
| Línea vertical punteada | Línea de vida (*lifeline*) |
| Rectángulo sobre la línea de vida | Ocurrencia de ejecución |

## Diagrama de actividad

Los diagramas de actividad UML son similares a los **diagramas de flujo**. Muestran un proceso de negocio como una secuencia de pasos (llamados **acciones**) e incluyen notación para expresar **ramificación condicional** y **concurrencia**, así como para mostrar eventos de envío y recepción. Las flechas entre acciones indican el flujo de control. Opcionalmente, pueden indicar qué elemento de la arquitectura o qué actor realiza cada acción (mediante carriles o *swimlanes*).

En particular, los diagramas de actividad pueden expresar concurrencia:

- Un **nodo fork** (una barra gruesa perpendicular a las flechas) divide el flujo en dos o más flujos concurrentes.
- Un **nodo join** (también una barra perpendicular) sincroniza esos flujos en uno solo. El join espera a que todos los flujos entrantes terminen antes de continuar.

**Ejemplo: rastreador de buceo (Depth Meter / Dive Tracker / Thermometer)**

```mermaid
flowchart TB
    S((inicio)) --> RP1[read pressure]
    RP1 --> D1{¿bajo el agua?}
    D1 -- "no" --> RP1
    D1 -- "sí" --> EDM[enter dive mode]
    EDM --> F1[["fork"]]

    F1 --> CD[check depth]
    CD --> RP2[read pressure]
    RP2 --> D2{¿asciende muy rápido?}
    D2 -- "sí" --> BA[beep alarm]
    D2 -- "no" --> D3
    BA --> D3{¿bajo el agua?}
    D3 -- "sí" --> SL1[sleep 0.5 sec]
    SL1 --> CD
    D3 -- "no" --> EX1>exit dive mode]

    F1 --> CWT[check water temperature]
    CWT --> RWT[read water temperature]
    RWT --> SL2[sleep 30 sec]
    SL2 --> CWT
    CWT -. "recibe señal" .-> EX2>exit dive mode]
    EX2 --> J1

    EX1 --> J1[["join"]]
    J1 --> FDM[finish dive mode]
    FDM --> E((fin))
```

> En el diagrama original, los pasos están organizados en tres carriles: **Depth Meter**, **Dive Tracker** y **Thermometer**. El monitoreo de profundidad y el de temperatura corren en paralelo entre el *fork* y el *join*.

# C4

## Definición

**C4** es una técnica de diagramación desarrollada por **Simon Brown** para abordar las deficiencias de UML y modernizar su enfoque. Las cuatro C de C4 son:

| Nivel | Vista | Descripción |
|---|---|---|
| 1 | **Contexto (*Context*)** | Representa todo el contexto del sistema, incluidos los roles de los usuarios y las dependencias externas. |
| 2 | **Contenedores (*Containers*)** | Los límites y contenedores de despliegue físicos (y a menudo lógicos) dentro de la arquitectura. Es un buen punto de encuentro entre operaciones y arquitectos. |
| 3 | **Componentes (*Components*)** | La vista de componentes del sistema; es la que mejor se alinea con la visión que el arquitecto tiene del sistema. |
| 4 | **Código / Clases (*Code*)** | C4 usa el mismo estilo de diagramas de clases de UML, que son efectivos, por lo que no es necesario reemplazarlos. |

**Notas adicionales (del orador)**

- Si una empresa busca estandarizar una técnica de diagramación, C4 es una buena alternativa.
- Sin embargo, como todas las herramientas de diagramación técnica, no puede expresar todos los tipos de diseño que una arquitectura puede abordar.
- C4 es más adecuado para **arquitecturas monolíticas**, donde las relaciones entre contenedores y componentes pueden diferir, y menos adecuado para **arquitecturas distribuidas**, como los microservicios.

## Jerarquía y niveles de zoom

> Un **sistema de software** está compuesto por uno o más **contenedores** (aplicaciones y almacenes de datos), cada uno de los cuales contiene uno o más **componentes**, que a su vez se implementan mediante uno o más elementos de **código** (clases, interfaces, objetos, funciones, etc.).

```mermaid
flowchart TB
    SS["<b>Software System</b>"] --> C1["Container"]
    SS --> C2["<b>Container</b><br/>(p. ej., web app, app móvil,<br/>esquema de BD, file system, object store)"]
    SS --> C3["Container"]
    C2 --> CO1["Component"]
    C2 --> CO2["<b>Component</b>"]
    C2 --> CO3["Component"]
    CO2 --> K1["Code"]
    CO2 --> K2["<b>Code</b>"]
    CO2 --> K3["Code"]
```

Cada nivel es un **"zoom in"** sobre un elemento del nivel anterior: Contexto → Contenedores → Componentes → Código. Esto aplica directamente el principio de **consistencia representacional**: siempre se sabe en qué parte del sistema general se está.

Fuente: [c4model.com](https://c4model.com)

## Ejemplo: Internet Banking System

### Nivel 1 — Contexto

```mermaid
flowchart TB
    P(["<b>Personal Banking Customer</b><br/>[Person]<br/>Cliente del banco con cuentas personales"])
    IBS["<b>Internet Banking System</b><br/>[Software System]<br/>Permite a los clientes ver información<br/>de sus cuentas y hacer pagos"]
    EMAIL["<b>E-mail System</b><br/>[Software System]<br/>Sistema interno de correo<br/>Microsoft Exchange"]
    MF["<b>Mainframe Banking System</b><br/>[Software System]<br/>Almacena toda la información bancaria:<br/>clientes, cuentas, transacciones"]

    P -- "Consulta saldos y hace pagos usando" --> IBS
    IBS -- "Envía e-mails usando" --> EMAIL
    EMAIL -- "Envía e-mails a" --> P
    IBS -- "Obtiene información de cuentas<br/>y hace pagos usando" --> MF

    classDef person fill:#08427b,color:#fff,stroke:#052e56
    classDef system fill:#1168bd,color:#fff,stroke:#0b4884
    classDef external fill:#999,color:#fff,stroke:#6b6b6b
    class P person
    class IBS system
    class EMAIL,MF external
```

### Nivel 2 — Contenedores

```mermaid
flowchart TB
    P(["<b>Personal Banking Customer</b><br/>[Person]"])

    subgraph IBS["Internet Banking System [Software System]"]
        WEB["<b>Web Application</b><br/>[Java and Spring MVC]<br/>Entrega el contenido estático<br/>y la SPA"]
        SPA["<b>Single-Page Application</b><br/>[JavaScript and Angular]<br/>Toda la funcionalidad de banca<br/>en el navegador"]
        MOB["<b>Mobile App</b><br/>[Xamarin]<br/>Subconjunto de la funcionalidad<br/>en el móvil"]
        API["<b>API Application</b><br/>[Java and Spring MVC]<br/>Funcionalidad vía API JSON/HTTPS"]
        DB[("<b>Database</b><br/>[Oracle Database Schema]<br/>Usuarios, credenciales con hash,<br/>logs de acceso")]
    end

    EMAIL["<b>E-mail System</b><br/>[Software System]"]
    MF["<b>Mainframe Banking System</b><br/>[Software System]"]

    P -- "Visita bigbank.com/ib [HTTPS]" --> WEB
    P -- "Consulta saldos y hace pagos" --> SPA
    P -- "Consulta saldos y hace pagos" --> MOB
    WEB -- "Entrega al navegador del cliente" --> SPA
    SPA -- "Llama a la API [JSON/HTTPS]" --> API
    MOB -- "Llama a la API [JSON/HTTPS]" --> API
    API -- "Lee y escribe [SQL/TCP]" --> DB
    API -- "Envía e-mails [SMTP]" --> EMAIL
    API -- "Llama a la API [XML/HTTPS]" --> MF
    EMAIL -- "Envía e-mails a" --> P
```

### Nivel 3 — Componentes (API Application)

```mermaid
flowchart TB
    SPA["<b>Single-Page Application</b><br/>[Container]"]
    MOB["<b>Mobile App</b><br/>[Container]"]

    subgraph API["API Application [Container]"]
        SIC["<b>Sign In Controller</b><br/>[Spring MVC Rest Controller]<br/>Permite iniciar sesión"]
        RPC["<b>Reset Password Controller</b><br/>[Spring MVC Rest Controller]<br/>Restablece la contraseña con URL de un solo uso"]
        ASC["<b>Accounts Summary Controller</b><br/>[Spring MVC Rest Controller]<br/>Resumen de las cuentas del cliente"]
        SEC["<b>Security Component</b><br/>[Spring Bean]<br/>Inicio de sesión, cambio de contraseña, etc."]
        EC["<b>E-mail Component</b><br/>[Spring Bean]<br/>Envía e-mails a los usuarios"]
        MBF["<b>Mainframe Banking System Facade</b><br/>[Spring Bean]<br/>Fachada hacia el mainframe"]
    end

    DB[("<b>Database</b><br/>[Container]")]
    EMAIL["<b>E-mail System</b><br/>[Software System]"]
    MF["<b>Mainframe Banking System</b><br/>[Software System]"]

    SPA & MOB -- "Llama a la API [JSON/HTTPS]" --> SIC & RPC & ASC
    SIC -- "Usa" --> SEC
    RPC -- "Usa" --> SEC
    RPC -- "Usa" --> EC
    ASC -- "Usa" --> MBF
    SEC -- "Lee y escribe [SQL/TCP]" --> DB
    EC -- "Envía e-mails usando" --> EMAIL
    MBF -- "Usa [XML/HTTPS]" --> MF
```

### Nivel 4 — Código (Mainframe Banking System Facade)

```mermaid
classDiagram
    namespace com_bigbankplc_internetbanking_component_mainframe {
        class MainframeBankingSystemFacade {
            <<interface>>
        }
        class MainframeBankingSystemFacadeImpl
        class MainframeBankingSystemException
        class GetBalanceRequest
        class GetBalanceResponse
        class BankingSystemConnection
        class AbstractRequest {
            <<abstract>>
        }
        class AbstractResponse
    }
    class InternetBankingSystemException

    MainframeBankingSystemFacade <|.. MainframeBankingSystemFacadeImpl
    MainframeBankingSystemFacadeImpl ..> MainframeBankingSystemException : throws
    MainframeBankingSystemFacadeImpl ..> GetBalanceRequest : creates
    MainframeBankingSystemFacadeImpl ..> BankingSystemConnection : uses
    MainframeBankingSystemFacadeImpl ..> GetBalanceResponse : parses
    InternetBankingSystemException <|-- MainframeBankingSystemException
    AbstractRequest <|-- GetBalanceRequest
    AbstractResponse <|-- GetBalanceResponse
    BankingSystemConnection ..> AbstractRequest : sends
    BankingSystemConnection ..> AbstractResponse : receives
```

# ArchiMate

## Definición

**ArchiMate** (una amalgama de *Archi*tecture-Ani*mate*) es un **lenguaje de modelado de arquitectura empresarial** de código abierto que apoya la descripción, el análisis y la visualización de la arquitectura dentro de los dominios de negocio y entre ellos. Es un estándar técnico de **The Open Group** y ofrece un lenguaje de modelado más liviano para ecosistemas empresariales.

El objetivo de ArchiMate es ser **"lo más pequeño posible"**, no cubrir todos los escenarios de casos extremos. Por eso se ha convertido en una opción popular entre muchos arquitectos.

## Relación con TOGAF

ArchiMate se usa junto con el ciclo **ADM** (*Architecture Development Method*) de **TOGAF**. Las capas centrales de ArchiMate (*core layers*) corresponden a las fases B, C y D del ciclo:

```mermaid
flowchart TB
    PRE((Preliminary)) --> A((A. Architecture Vision))
    A --> B((B. Business Architecture))
    B --> C((C. Information Systems Architecture))
    C --> D((D. Technology Architecture))
    D --> E((E. Opportunities and Solutions))
    E --> F((F. Migration Planning))
    F --> G((G. Implementation Governance))
    G --> H((H. Architecture Change Management))
    H --> A
    RM((Requirements Management)) <--> A & B & C & D & E & F & G & H

    B -.- BL["Business Layer"]
    C -.- AL["Application Layer"]
    D -.- TL["Technology Layer"]
```

| Zona del ciclo | Fases |
|---|---|
| **Strategy & Motivation** | Preliminary, A |
| **Core Layers** (ArchiMate) | B → Business Layer, C → Application Layer, D → Technology Layer |
| **Implementation & Migration** | E, F, G, H |

> **Nota:** en la imagen original, la fase D aparece etiquetada como "Business Layer". Es un error de la imagen: la fase D (*Technology Architecture*) corresponde a la **Technology Layer**.

## Capas

El lenguaje núcleo de ArchiMate define una estructura de elementos genéricos y sus relaciones, que pueden especializarse en diferentes capas. Define tres capas:

| Capa | Descripción |
|---|---|
| **Negocio (*Business*)** | Ofrece productos y servicios a clientes externos, que la organización realiza mediante procesos de negocio ejecutados por actores de negocio. |
| **Aplicación (*Application*)** | Soporta la capa de negocio con servicios de aplicación, que son realizados por aplicaciones (software). |
| **Tecnología (*Technology*)** | Ofrece los servicios de infraestructura (procesamiento, almacenamiento, comunicación) necesarios para ejecutar las aplicaciones, realizados por hardware de cómputo y comunicación y por software de sistema. |

> Estas capas son compartidas por otros marcos, el más importante de los cuales es **TOGAF**, aunque TOGAF las llama *Business*, *Information System* (*Application* y *Data*) e *Information Technology*.

## Capas y aspectos

Además de las capas, ArchiMate organiza sus elementos en tres **aspectos**. El resultado es una matriz de capas × aspectos:

| | Estructura pasiva | Comportamiento | Estructura activa |
|---|---|---|---|
| **Business** | | | |
| **Application** | | | |
| **Technology** | | | |

- **Estructura activa:** los elementos que *realizan* el comportamiento (actores, componentes, nodos).
- **Comportamiento:** lo que se *hace* (procesos, funciones, servicios).
- **Estructura pasiva:** los objetos *sobre los que* se actúa (objetos de negocio, objetos de datos, artefactos).

```mermaid
flowchart LR
    AS["<b>Active Structure</b><br/>(quién)"] -- "perform" --> B["<b>Behavior</b><br/>(qué hace)"] -- "on / with" --> PS["<b>Passive Structure</b><br/>(sobre qué)"]
```

Ejemplo en la capa de aplicación:

| Active Structures | Behavior | Passive Structures |
|---|---|---|
| Application component | Application service | Data object |
| Application interface | Application function | |
| Application collaboration | Application interaction | |

## Elementos por capa

### Elementos de estructuras activas

| Business | Application | Technology |
|---|---|---|
| Business actor | Application component | Node |
| Business role | Application interface | Device |
| Business collaboration | Application collaboration | System software |
| Business interface | | Infrastructure interface |
| Location | | Network |
| | | Communication path |

### Elementos de comportamiento

| Business | Application | Technology |
|---|---|---|
| Business process | Application service | Infrastructure service |
| Business function | Application function | Infrastructure function |
| Business interaction | Application interaction | |
| Business event | | |
| Business service | | |

### Elementos de estructuras pasivas

| Business | Application | Technology |
|---|---|---|
| Business object | Data object | Artifact |
| Representation | | |
| Meaning | | |
| Value | | |
| Product | | |
| Contract | | |

> **Colores convencionales:** amarillo para la capa de negocio, azul claro para la capa de aplicación y verde para la capa de tecnología.

## Ejemplo: manejo de reclamaciones de una aseguradora

```mermaid
flowchart TB
    subgraph BL["Business Layer"]
        direction TB
        subgraph ERA["External Roles and Actors"]
            CLIENT["Client<br/>«Business Actor»"] --> INSURANT["Insurant<br/>«Business Role»"]
        end
        subgraph EBS["External Business Services"]
            CR(["Claim Registration"])
            CI(["Customer Information"])
            CP(["Claim Payment"])
        end
        subgraph BP["Business Processes and Internal Actors / Roles"]
            subgraph HC["Handle Claim «Business Process»"]
                R1[Register] --> R2[Accept] --> R3[Valuate] --> R4[Pay]
            end
            ARCHI["ArchiSurance<br/>«Business Actor»"] --> INSURER["Insurer<br/>«Business Role»"]
        end
    end

    subgraph AL["Application Layer"]
        direction TB
        subgraph EAS["External Application Services"]
            IA(["Insurance Application"])
            CDM(["Customer Data Modification"])
            PP(["Premium Payment"])
        end
        subgraph ACS["Application Components and Services"]
            CRM["CRM System"]
            PDM["Policy Data Management"]
            FIN["Financial Application"]
        end
    end

    subgraph TL["Technology Layer"]
        direction TB
        subgraph EIS["External Infrastructure Services"]
            CFS(["Claim Files Service"])
            CUFS(["Customer File Service"])
        end
        subgraph INF["Infrastructure"]
            MAIN["Mainframe «Node»<br/>CICS · DBMS · Message Queuing<br/>«System Software»"]
            UNIX["Unix Server Farm<br/>UNIX Server · UNIX Server «Device»"]
            NAS["NAS File Server «Device»"]
        end
    end

    INSURANT -- "served by" --> CR & CI & CP
    CR -. "realized by" .-> HC
    CP -. "realized by" .-> HC
    INSURER -- "assigned to" --> HC
    HC -- "served by" --> IA & CDM & PP
    IA -. "realized by" .-> CRM
    CDM -. "realized by" .-> PDM
    PP -. "realized by" .-> FIN
    PDM -- "served by" --> CFS
    FIN -- "served by" --> CUFS
    CFS -. "realized by" .-> MAIN
    CUFS -. "realized by" .-> NAS
```

> Diagrama simplificado del original: se leen de arriba hacia abajo las relaciones "*served by*" (la capa inferior presta servicios a la superior) y "*realized by*" (un elemento concreto realiza un servicio).

Este ejemplo muestra los principales tipos de relación de ArchiMate:

| Relación | Significado |
|---|---|
| **Serving** | Un elemento presta un servicio a otro. |
| **Realization** | Un elemento concreto realiza uno más abstracto (p. ej., un proceso realiza un servicio). |
| **Assignment** | Asigna un actor o rol a un comportamiento. |
| **Triggering** | Un comportamiento dispara el siguiente (p. ej., Register → Accept). |

# Herramientas

[softwarearchitecture.tools](https://softwarearchitecture.tools/) reúne las mejores herramientas, gratuitas y de pago, para diagramar y diseñar arquitecturas de software.

**Modelling tools:** permiten reutilizar y sincronizar objetos entre múltiples vistas, a veces en diferentes niveles de abstracción y con validación estricta. Son ideales para documentación de larga duración.

| Herramienta | Descripción |
|---|---|
| **IcePanel** | Modelado C4 colaborativo, diseñado para aumentar la confianza del equipo. |
| **Enterprise Architect** | Diagramación y modelado empresarial. |
| **Archi** | Herramienta de código abierto para crear modelos y bocetos en ArchiMate. |
| **Structurizr** | Diagramas C4 y documentación. |
| **Carbide** | Modelado y diagramación visual en C4. |
| **StarUML** | Herramienta de modelado enfocada en UML. |
| **Aplas** | Visibilidad del panorama de software. |
| **GenMyModel** | Herramienta de modelado colaborativa. |
| **Gaphor** | Herramienta de modelado de código abierto. |
| **Hackolade** | Diseño visual de esquemas para almacenamiento e intercambio de datos. |
| **Visual Paradigm** | Herramienta de modelado colaborativa en línea. |
| **Archipeg** | Herramienta combinada de arquitectura empresarial y de soluciones. |

# Bibliografía

Mark Richards, Neal Ford. *Fundamentals of Software Architecture: An Engineering Approach*. O'Reilly, 2020.