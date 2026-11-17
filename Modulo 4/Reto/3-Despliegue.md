# Workshop Parte 3 - Despliegue

> Resumen del video [Workshop Parte 3 Despliegue](https://youtu.be/JYixvTjWCQA) (13:59), basado en la transcripción y lo que se muestra en pantalla.

## 1. Conectar las credenciales con VS Code

1. En IAM, dentro del usuario creado en la parte 2, crear una **Access Key** para uso local (*Local code*). La etiqueta es opcional.
2. **Descargar el `.csv`**: la *secret key* solo se ve en ese momento. ⚠️ No compartir la access key ni la secret key con nadie.
3. En VS Code, en la extensión **AWS Toolkit**, conectar un perfil e ingresar la access key y la secret key. En el video el perfil se llama `develop`.

## 2. Preparar el entorno

- Tener **Docker abierto y corriendo**; SAM lo usa para el *build*.
- Crear una carpeta de trabajo (p. ej. `workshop`), abrirla en VS Code y ubicarse en ella desde la terminal (Warp en el video).
- Clonar el repositorio del workshop (requiere Git). El paso de la carpeta `environment` del workshop no es necesario:

  ```bash
  git clone https://github.com/aws-samples/serverless-observability-workshop.git
  cd serverless-observability-workshop/code/sample-app
  ```

## 3. La aplicación de ejemplo (`template.yaml`)

El template SAM de `code/sample-app` define una arquitectura sencilla pero suficiente para entender cada pilar:

```
Clientes → API Gateway → Lambda: Get Item by ID  ┐
                       → Lambda: Get All Items   ├→ DynamoDB (SampleTable)
                       → Lambda: Put Item        ┘
```

- **API Gateway** como punto de entrada.
- **3 funciones Lambda**: `getByIdFunction`, `getAllItemsFunction` y `putItemFunction`.
- **Tabla DynamoDB** donde se guardan los elementos.
- **Grupos de logs de CloudWatch** con días de retención para el API y cada función.
- **Salidas** (*outputs*): la URL del API Gateway y el nombre de la tabla.

**Único cambio pedido**: actualizar el runtime de `nodejs18.x` a **`nodejs20.x`**.

## 4. Build y deploy con SAM

Se necesita **Node.js** instalado.

```bash
npm install                       # tarda un par de minutos; las vulnerabilidades reportadas se pueden ignorar
sam build -u                      # build dentro de un contenedor (Docker)
sam deploy -g --profile develop   # usar el nombre de TU perfil
```

> Si no se pasa `--profile`, SAM toma el perfil por defecto y falla con un error de *security token* inválido.

Respuestas del asistente `sam deploy -g`:

| Pregunta | Respuesta |
|---|---|
| Stack Name | `monitoring-app` |
| AWS Region | La región elegida (`us-east-2` en el video; puede ser `us-east-1`) |
| Confirm changes before deploy | `N` |
| Allow SAM CLI IAM role creation | `Y` |
| Disable rollback | `Y` |
| Funciones sin autorización definida (*has no authentication*) | `Y` (aceptar) |
| Save arguments to configuration file | `Y` (Enter por defecto) |

El progreso del despliegue se puede seguir en **CloudFormation**, donde aparece el nuevo stack.

### Problema encontrado: permisos insuficientes

- El primer deploy **falló** porque el usuario no tenía permisos para crear recursos de **API Gateway** (ni roles IAM).
- **Solución**: agregar al usuario `IAMFullAccess` y `AmazonAPIGatewayAdministrator`. `AmazonAPIGatewayInvokeFullAccess` **no basta**, porque se necesita poder hacer `POST` sobre la API de gestión.
- **Políticas finales del usuario (7)**:
  - `AmazonS3FullAccess`
  - `AmazonDynamoDBFullAccess`
  - `AmazonAPIGatewayAdministrator`
  - `IAMFullAccess`
  - `AWSLambda_FullAccess`
  - `CloudWatchLogsFullAccess`
  - `AWSCloudFormationFullAccess`
- Al reintentar, SAM indicó un *deployment en progreso*: el stack había quedado en **`ROLLBACK_COMPLETE`**. Hay que **eliminar el stack** en CloudFormation y volver a ejecutar `sam deploy -g` con los mismos valores.

Al terminar, el stack `monitoring-app` aparece en CloudFormation y las funciones `monitoring-app-*` en la consola de **Lambda**.

## 5. Probar los endpoints

- El workshop usa variables de entorno y `curl` (pensado para Linux). En Windows, copiar la **URL del API** (output `ApiUrl` del deploy) y usar **Postman**.
- Peticiones creadas:

| Operación | Método y ruta | Resultado esperado |
|---|---|---|
| Put Item | `POST {ApiUrl}/items/` con body `{"id": "1", "name": "Sample test item"}` (y otro con `id` 2) | `200` con respuesta vacía |
| Get All Items | `GET {ApiUrl}/items/` | Lista de todos los elementos |
| Get Item by ID | `GET {ApiUrl}/items/{id}` | Un solo elemento |

## 6. El código de las funciones (`src/handlers`)

Cada handler crea un **cliente de DynamoDB** y devuelve `200` al ejecutar la operación:

- **put-item**: guarda un elemento en la tabla.
- **get-all-items**: hace un **scan** de toda la tabla.
- **get-by-id**: consulta por la **llave** específica y devuelve un solo elemento.

## Siguiente paso

Con la arquitectura base desplegada y probada, en la siguiente parte se empieza a implementar **métricas** para ver cómo se reflejan en el sistema.
