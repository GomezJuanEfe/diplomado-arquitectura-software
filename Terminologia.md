# Dominio

Es el área del negocio o del mundo real que tu software resuelve. Es el problema, no la solución. Si trabajas en un banco, el dominio incluye cuentas, transferencias, saldos, intereses, mora. Si trabajas en un hospital: pacientes, citas, historias clínicas, prescripciones. En una tienda online: catálogo, carrito, órdenes, envíos.

Una forma práctica de detectarlo: es el vocabulario que usaría alguien del negocio que no sabe programar. Un contador te habla de "conciliación bancaria" y "asiento contable" — eso es dominio. Nunca te va a hablar de "latencia p99" o "cobertura de pruebas".

### Por qué importa para lo que estábamos viendo

El filtro decía: una característica de arquitectura no es del dominio. La razón es que son dos preguntas separadas.

Toma "transferir dinero entre dos cuentas". Eso es dominio puro: qué pasa, qué reglas aplican, qué saldos cambian. Ahora pregúntate cosas como si debe completarse en menos de un segundo, si debe seguir funcionando cuando un servidor se cae, si debe quedar auditable. Ninguna de esas respuestas cambia qué es una transferencia. Pero todas cambian cómo construyes el sistema.

Y al revés: "disponibilidad del 99.99%" significa exactamente lo mismo en el banco, en el hospital y en la tienda. Por eso se dice que las características de arquitectura son agnósticas al dominio — se pueden nombrar sin saber de qué va el negocio.

El matiz que no quiero que se te pierda

Eso no significa que el dominio sea irrelevante para elegirlas. Es al revés: el dominio es de dónde salen. Es el negocio el que te dice que en trading la latencia se mide en milisegundos, mientras que en un sistema de nómina que corre una vez al mes eso da igual — pero ahí importa muchísimo la trazabilidad, porque hay auditorías.

O sea: el dominio no forma parte de la lista de características, pero es quien decide cuáles de esas doce son críticas para tu sistema.