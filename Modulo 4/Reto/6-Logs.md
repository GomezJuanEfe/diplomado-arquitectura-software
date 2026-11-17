# Workshop Parte 6 - Logs

> Resumen del video [Workshop Parte 6 Logs](https://youtu.be/znIQ3C24wH0) (5:34), basado en la transcripción.

## Enfoque del video

La sección de **logging centralizado** del workshop es muy práctica y el tema de logs es muy conocido, así que **no se ejecuta paso a paso en el video**: cada estudiante debe hacerla por su cuenta. El video se detiene solo en los puntos clave del **análisis de logs**.

## Lo que pide el workshop (a ejecutar por cuenta propia)

1. **Agregar un permiso** para que **API Gateway** pueda escribir logs directamente en **CloudWatch**.
2. Entender **cómo funciona la librería de logging** del proyecto: qué hace y qué información se puede capturar.
3. **Modificar los handlers** para registrar los eventos. Igual que en las partes anteriores, el workshop trae el *handler* completo para copiarlo.

## Idea central

> Los logs no solo sirven para **entender cómo se comporta** la aplicación, también sirven para **analizarla**.

## CloudWatch Logs Insights

**Logs Insights** permite hacer **consultas (queries)** sobre los logs para buscar y agregar información.

1. Seleccionar el grupo de logs de la Lambda y ejecutar una consulta básica: se ven los mismos logs de la aplicación.
2. **Filtrar** buscando un texto dentro del mensaje (`@message`, donde se guarda todo por defecto). En el video se busca `operation`, una de las **dimensiones** que se registran en los logs, por ejemplo con los logs EMF de la parte 5.
3. **Agregar con `stats`**: contar peticiones y agrupar (`by`) por un campo o dimensión específica.

Consulta ilustrativa del tipo que se construye en el video:

```sql
fields @timestamp, @message
| filter @message like /operation/
| stats count(*) by operation
```

Resultado en el video: **3 operaciones exitosas** registradas en los logs de esa Lambda.

## ¿Métricas o logs?

Ambos permiten determinar con exactitud el comportamiento del sistema. Según lo que se registre y se necesite, se pueden construir distintos **dashboards de negocio**:

- **Métrica** → el **total** (p. ej. cuántas compras se hicieron).
- **Logs** → el **detalle** para desglosar ese total (p. ej. a qué **categoría, área o subproducto** pertenece cada compra).

Combinarlos **enriquece el dashboard**.

## Llevar la consulta a un dashboard

1. En el dashboard creado en la parte 5, **agregar un widget de tipo Logs** con visualización de **tabla**.
2. Seleccionar el grupo de logs de la Lambda de monitoreo y **pegar la consulta**.
3. **Ejecutarla** para validar y **crear el widget**.

Así el dashboard ofrece un análisis más diferenciado y desglosado.

## Conclusión

Lo más importante de esta parte es que **los logs también deben aprovecharse para temas de negocio**, no solo para depurar. Hay que completar por cuenta propia todo el módulo de logs del workshop; las dudas se resuelven en la sesión, el foro o los canales del curso.
