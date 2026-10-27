# Principios y técnicas esenciales para diseñar arquitecturas de software de alto rendimiento y calidad

El propósito central de este módulo es dominar las técnicas fundamentales para el diseño, representación y evaluación de arquitectura de software, incluyendo:

- **Técnicas de de diagramación** para visualizar y comunicar efectivamente la estructura de sistemas complejos.
- **Diseño Basado en Atributos (ADD)** para priorizar y satisfacer de calidad clave.
- **Estilos Arquitectónicos** (monolítico, microservicios, basado en eventos)y sus aplicaciones prácticas
- **Análisis de riesgos** mediante matrices y técnicas colaborativas.

## Introducción

Este módulo se enfoca en cómo **representar**, **diseñar** y **evaluar** una arquitectura de software. Se organiza en tres partes:

| Sección | Tema | Pregunta que responde |
|---|---|---|
| Presentación | Técnicas de diagramación | ¿Cómo comunico visualmente una arquitectura? |
| Profundización | Diseño Basado en Atributos (ADD) y estilos de arquitectura | ¿Cómo diseño una arquitectura guiada por atributos de calidad? |
| Cierre | Análisis de riesgos | ¿Cómo anticipo y mitigo lo que puede salir mal? |

```mermaid
flowchart LR
    A["<b>Representar</b><br/>Técnicas de diagramación"] --> B["<b>Diseñar</b><br/>ADD + Estilos de arquitectura"] --> C["<b>Evaluar</b><br/>Análisis de riesgos"]
```

# 1. Presentación: Técnicas de diagramación

Técnicas esenciales para representar visualmente la arquitectura de un sistema, sus aplicaciones y beneficios en el diseño y la documentación.

**Temas:**

- **UML**
- **Modelo C4**
- **ArchiMate**

> 📎 Anexo: [Técnicas de diagramación](./Anexo:%20Tecnica%20Diagramacion.md)

# 2. Profundización

## 2.1 Diseño Basado en Atributos (ADD)

El **ADD** (*Attribute-Driven Design*) es un método sistemático para diseñar arquitecturas de software de forma **estructurada y repetible**, tomando como base los **atributos de calidad** (rendimiento, seguridad, mantenibilidad, escalabilidad, etc.).

Permite a los arquitectos tomar decisiones de diseño informadas para que el sistema cumpla no solo los requisitos funcionales, sino también los de calidad.

**Temas:**

- Identificar y priorizar los atributos de calidad.
- **Drivers arquitectónicos.**
- Pasos del proceso: desde la revisión de las entradas hasta la iteración y validación del diseño.
- Refinamiento de elementos.
- Toma de decisiones basada en atributos de calidad.

> 📎 Anexo: [Diseño Basado en Atributos (ADD)](./Anexo:%20ADD.md)

## 2.2 Estilos de arquitectura de software

Los **estilos de arquitectura** son los principios y estructuras fundamentales que guían la organización y el diseño de un sistema de software. Cada estilo define cómo se disponen la interfaz de usuario, el código del backend y la interacción con los datos.

**Temas:**

- Principales estilos de arquitectura, desde los tradicionales hasta los más innovadores.
- Características, ventajas y desventajas de cada uno.
- Análisis comparativo para elegir el estilo más adecuado según el contexto.

> 📎 Anexo: [Estilos de arquitectura de software](./Anexo:%20Estilos%20Arquitectura%20Software.md)

# 3. Cierre: Análisis de riesgos

El **análisis de riesgos** es una práctica esencial para anticipar y mitigar problemas que puedan afectar el éxito del proyecto y fortalecer la resiliencia del diseño.

**Temas:**

| Herramienta / técnica | Para qué sirve |
|---|---|
| **Matriz de riesgos** | Identificar y priorizar los riesgos según su **probabilidad** e **impacto**. |
| **Evaluación de riesgos** | Analizar y valorar los riesgos identificados, determinar su relevancia y las acciones para gestionarlos. |
| **Lluvia de riesgos** | Técnica colaborativa para descubrir, consensuar y mitigar riesgos con la participación del equipo. |

> 📎 Anexo: [Análisis de riesgos](./Anexo:%20Analisis%20Riesgos.md)