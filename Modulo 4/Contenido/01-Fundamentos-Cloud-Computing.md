# Fundamentos de Cloud Computing

> Resumen de la *Presentación 1 - Fundamentos de Cloud Computing* (sección **Introducción** de los contenidos obligatorios).

## Definición

Gartner define cloud computing como un estilo de computación en el que capacidades de TI **escalables y elásticas** se entregan **como servicio** usando tecnologías de Internet. Los tres aspectos clave son: escalabilidad, elasticidad y prestación como servicio a través de Internet.

- **Escalabilidad**: aumentar cómputo y almacenamiento es fácil; no hay que comprar, desempacar ni conectar máquinas.
- **Elasticidad**: la escalabilidad funciona en ambos sentidos; la capacidad también se reduce cuando ya no se necesita (como una banda elástica). No todos los servicios la traen automáticamente; muchas veces la debe construir el cliente.

## Características esenciales (NIST)

| Característica | Idea principal |
|---|---|
| Autoservicio bajo demanda | El usuario aprovisiona recursos por sí mismo, sin depender de un proveedor de hosting o de procesos lentos. |
| Amplio acceso a la red | Los servicios están disponibles a través de Internet (la mayoría es nube pública). |
| Agrupación de recursos | Varios usuarios comparten el mismo pool de infraestructura; es lo que permite el ahorro (aunque existen instancias dedicadas / *bare metal*). |
| Elasticidad | Escalar hacia arriba y hacia abajo rápidamente según la demanda (p. ej. una venta flash). |
| Servicio medido | Se paga solo por lo que se usa (tiempo, almacenamiento, usuarios, instrucciones…), de forma transparente. |

## Modelos de servicio

| Modelo | Quién gestiona qué | Ejemplos |
|---|---|---|
| **IaaS** – Infraestructura como Servicio | El proveedor da recursos básicos; el consumidor instala y gestiona el software. Más control, más trabajo. | Máquinas virtuales |
| **PaaS** – Plataforma como Servicio | El consumidor programa y configura; el proveedor gestiona la infraestructura subyacente. Low Code / No Code entran aquí y se ven impactados por la IA. | Plataformas de desarrollo |
| **SaaS** – Software como Servicio | Todo lo gestiona el proveedor; el usuario solo configura y usa desde el navegador. | Google Docs, Office 365, SAP SuccessFactors, ServiceNow, Zendesk |

Modelos intermedios:

- **FaaS / Serverless**: código que se ejecuta ante *triggers*; entre PaaS y SaaS.
- **Contenedores**: un nivel por encima del sistema operativo; entre IaaS y PaaS.
- Algunos productos (p. ej. bases de datos) existen en las tres variantes.

## Modelos de despliegue

| Modelo | Descripción | Relevancia hoy |
|---|---|---|
| **Pública** | Acceso a través de Internet pública; hoy con cifrado en tránsito y en reposo. | Prácticamente toda la nube actual. |
| **Privada** | Servicios cloud sobre infraestructura privada. | Poco extendida; desaprovecha la mayoría de ventajas de la nube. |
| **Comunitaria** | Un grupo de consumidores construye una nube compartida. | Casi teórica; las *GovClouds* se le parecen. |
| **Híbrida** | Combinación de dos o más modelos. | Muy común en sentido amplio: varios proveedores cloud + centros de datos propios. |

## Impulsores de la adopción de la nube

- **Economía**: beneficia tanto al proveedor como al consumidor.
  - **De CAPEX a OPEX**: de grandes inversiones iniciales a costos operativos corrientes.
  - **Costo total de propiedad**: el soporte anual on‑premise cuesta 10–20 % del precio de compra; hay casos con reducciones de costos operativos del 80–90 %.
  - **Economías de escala**: el costo de añadir una unidad de infraestructura baja a medida que crece el total gestionado.
- **Enfoque**: la empresa se concentra en sus competencias básicas (su ventaja competitiva), no en operar infraestructura.
- **Escalabilidad**: crecer rápido y en las ubicaciones adecuadas, especialmente a nivel global.
- **Agilidad**: reaccionar rápido al mercado (evidente durante la pandemia de COVID‑19).
- **Resiliencia**: evita construir y probar costosos sitios de respaldo con *failover* propio.
- **Seguridad**: el aislamiento on‑premise es una ilusión en un mundo conectado; la nube ha avanzado mucho en seguridad.
- **Sostenibilidad**: mejor utilización de recursos compartidos; un servidor tradicional 24/7 consume ~3 veces la energía necesaria.
