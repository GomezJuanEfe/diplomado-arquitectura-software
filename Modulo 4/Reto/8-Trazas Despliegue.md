# Workshop Parte 8 - Trazas Despliegue

> Resumen del video [Workshop Parte 8 Trazas Despliegue](https://youtu.be/ffr2p-Lf0us) (3:50), basado en la transcripción y lo que se muestra en pantalla.

## Cierre del workshop

### Secciones para leer (no implementar)

El workshop incluye otros temas. Se recomienda **estudiarlos y leerlos, no implementarlos**, porque algunos **generan costos altos** y se pueden analizar a partir de lo que muestra el workshop:

- **Testing your application**:
  - **Synthetic Canaries** (API Canaries): pruebas sintéticas para validar el API.
  - **Performance Bottlenecks** con **Lambda Insights**: cómo activarlo, generar carga y explorar métricas de la aplicación (*Single Function Visualization*, integrado con X-Ray). Ayuda a entender qué se puede mejorar.
- **Accelerating Observability Development with Powertools**: **Powertools for AWS Lambda** es una librería de AWS que facilita implementar **métricas, logs y trazas** de distintas formas.

## Conclusiones

- La **observabilidad es clave**. Se puede aprovechar para:
  - Explotar **métricas de negocio**.
  - Usar los **logs** de la aplicación como base para métricas de negocio.
  - Usar las **trazas** para **optimizar**.
- **Optimizar es ahorrar**: en serverless (y otros modelos), si una función pasa de 10 s a 5 s, el **costo de operación baja un 50 %**.
- **Buscar un balance**: la observabilidad también **cuesta**. Hay que decidir qué observar e implementar, sobre todo en los **procesos críticos de negocio**, donde optimizar un 20 % del tiempo da mucho más beneficio que lo que cuesta la observabilidad.
- Los atributos de calidad (**desempeño, confiabilidad, seguridad, disponibilidad**) deben ir acompañados de observabilidad: métricas, validaciones y puntos de comparación para ser más efectivos y eficientes y lograr una aplicación **más robusta**.

## Limpieza de recursos (*Cleanup*)

Al terminar, **eliminar todo lo desplegado** para no generar costos. Se hace para las **dos aplicaciones**:

```bash
# App de métricas y logs
cd serverless-observability-workshop/code/sample-app
sam delete --profile develop          # stack monitoring-app

# App de trazas
cd ../sample-app-tracing
sam delete --profile develop          # stack monitoring-app-tracing
```

- Confirmar la eliminación del stack y de los artefactos en S3 cuando se pregunte.
- En **CloudFormation** se ve cómo se destruyen los stacks `monitoring-app` y `monitoring-app-tracing`.

Dudas o inquietudes: comentarios, foros habilitados o canales de comunicación del curso.
