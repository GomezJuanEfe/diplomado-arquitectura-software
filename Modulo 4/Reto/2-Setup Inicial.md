# Workshop Parte 2 - Setup Inicial

> Resumen del video [Workshop Parte 2 Setup Inicial](https://youtu.be/QjxxFIYFU6c) (5:29).

## Contexto del workshop

- Se trabaja con el **AWS Serverless Observability Workshop**, una aplicación sencilla enfocada en el *backend* que usa:
  - **Amazon CloudWatch**
  - **AWS X-Ray**
  - **AWS Lambda**
  - **Amazon DynamoDB**
- El objetivo es entender **para qué sirven las trazas, los logs y las métricas**, con ejercicios y algo de código en Visual Studio Code.
- Se recomienda leer con detalle la introducción del workshop. En el video solo se repasan las definiciones clave.

## Los tres pilares

### Trazas (tracing distribuido)

- Muestran **cómo se propaga una petición por los componentes del sistema**.
- Ejemplo: petición → API Gateway → microservicio 1 → microservicio 2 → servidor de colas → microservicio 3 → base de datos.
- Permiten ver **por qué componentes pasa una transacción** e identificar bajo rendimiento, problemas y **cuellos de botella**.
- Herramienta: **AWS X-Ray**.

### Logs

- Centralizan en **un solo lugar y en un formato estándar** el comportamiento de la aplicación.
- Permiten ver qué tareas se ejecutan, **correlacionar tareas y transacciones**, seguir el paso de un componente a otro, qué se respondió, qué se procesó y qué errores hubo.
- Unos buenos logs permiten entender el comportamiento de uno o varios componentes.

### Métricas

- Asociadas al **rendimiento del sistema**, principalmente de infraestructura.
- También se pueden crear **métricas con contexto de negocio**, algo que se trabajará en el workshop.

## Setup de la cuenta AWS

El workshop recomienda **Cloud9**, pero AWS lo descontinuó hace un año, así que se usa la **cuenta propia** con un usuario IAM.

### 1. Crear un usuario IAM

1. En la consola de AWS, buscar **IAM** en el menú principal.
2. Crear un **usuario** para la aplicación de monitoreo, con cualquier nombre.
3. **No** darle acceso a la consola (no se necesita).
4. Adjuntar estas **políticas administradas**:

   | Política | Motivo |
   |---|---|
   | `AmazonDynamoDBFullAccess` | Tabla de la aplicación |
   | `AWSLambda_FullAccess` | Funciones serverless |
   | `CloudWatchFullAccess` | Logs y métricas |
   | `AWSCloudFormationFullAccess` | Despliegue con `sam deploy` |
   | `AmazonS3FullAccess` | Artefactos del despliegue SAM |

   > ⚠️ La buena práctica es aplicar **mínimo privilegio** y dar solo los permisos necesarios. En el video se usan políticas *full access* para simplificar.

5. Las etiquetas (tags) son opcionales; no se usan.

### 2. Crear las claves de acceso

1. Entrar al usuario recién creado → pestaña **Credenciales de seguridad**.
2. En **Claves de acceso (Access keys)**, crear una nueva clave.
3. La clave servirá para **conectar Visual Studio Code y el entorno de desarrollo local** con la cuenta de AWS.
