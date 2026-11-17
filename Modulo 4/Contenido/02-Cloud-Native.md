# Cloud Native

> Resumen de la *Presentación 2 - Cloud Native* (sección **Profundización**).

## Definición

Las tecnologías nativas de la nube permiten crear y ejecutar aplicaciones escalables en entornos dinámicos (nubes públicas, privadas, híbridas y multicloud). Contenedores, mallas de servicio, microservicios, infraestructura inmutable y APIs declarativas son ejemplos de este enfoque.

Producen sistemas **poco acoplados, resilientes, gestionables y observables** que, junto con automatización robusta, permiten hacer cambios de alto impacto de forma frecuente y predecible.

## Características

### 1. Diseñada como una colección de microservicios

Servicios independientes y poco acoplados, cada uno orientado a una capacidad de negocio bien definida.

- **Monolito**: complejo, difícil de escalar, caro de mantener, base de datos compartida.
- **SOA**: aplicación como colección de servicios de grano grueso (servicios web) integrados por un **ESB** central. Sus limitaciones: servicios que no escalan de forma independiente, dependencias entre aplicaciones, problemas de fiabilidad por la centralización y poca diversidad tecnológica.
- **Microservicios**: servicios de grano fino, orientados al negocio, sin componentes centrales como el ESB. Son *smart endpoints* conectados por *dumb pipes* (comunicación ligera). No comparten base de datos. La granularidad se define con el **contexto acotado** de DDD.

### 2. Uso de contenedores y orquestación

- **Contenedor**: proceso aislado del sistema operativo anfitrión, con su propio sistema de archivos dado por una **imagen inmutable** (código + dependencias + runtime), guardada en un registro de contenedores.
- **Contenerización**: empaquetar microservicios en contenedores. Docker es la plataforma de facto. Da portabilidad, coherencia entre entornos y menor peso que las máquinas virtuales.
- **Orquestación**: gestionar el ciclo de vida de los contenedores; es imposible hacerlo a mano en producción.

#### Kubernetes

- Capa de abstracción que automatiza despliegue, escalado, tolerancia a fallos y redes.
- Portabilidad: corre en cualquier nube o centro de datos que lo soporte; todos los grandes proveedores lo ofrecen como servicio gestionado.
- **Clúster**: nodos del **plano de control** (gestionan y programan cargas) y nodos **worker**.
- **Servidor API**: comunica el plano de control con los nodos; el **kubelet** de cada nodo mantiene su estado.
- **Pod**: unidad básica de despliegue; uno o más contenedores con una IP única en el clúster.
- **Service** (agrupación lógica de pods con balanceo), **ReplicaSet** (número de réplicas), **Deployment** (cómo se despliegan los cambios).
- **Bucles de reconciliación**: mantienen el estado deseado sin intervención (p. ej. siempre 3 réplicas).
- Los objetos se definen en YAML o JSON.

### 3. Funciones sin servidor (Serverless)

Un microservicio puede modelarse como función. La plataforma (AWS Lambda, Azure Functions, Google Cloud Functions) aporta escalado automático, resiliencia, seguridad y observabilidad. Es ideal para cargas en ráfagas, trabajos por lotes y servicios basados en eventos.

### 4. Máquinas virtuales

Se pueden usar sin contenedores, pero con más complejidad y sobrecarga. En la práctica predominan los contenedores, la orquestación o serverless.

### 5. Automatización del ciclo de vida de desarrollo

- Código → push a Git → **Integración Continua** (build, pruebas unitarias, empaquetado).
- **Despliegue / Entrega Continua**: aplica la configuración de cada entorno y despliega. La diferencia: en *entrega continua* el paso a producción requiere aprobación manual; en *despliegue continuo* es automático.
- **Infraestructura como Código (IaC)**: redes, VMs, balanceadores y topología definidos de forma declarativa, sin intervención manual.

### 6. Gestión dinámica

Capacidades necesarias para operar en producción (ofrecidas por AWS, Azure y GCP):

- Compatibilidad con múltiples nubes
- Plano de control central
- Aprovisionamiento de recursos (CPU, memoria, almacenamiento, red)
- Optimización de recursos
- **Observabilidad**: logs, métricas y trazas con control centralizado
- Alta disponibilidad
- Calidad de servicio (QoS): seguridad extremo a extremo, regulación, cumplimiento, versionado
- Autoescalado según tráfico o carga

Kubernetes evita acoplarse a un proveedor específico.
