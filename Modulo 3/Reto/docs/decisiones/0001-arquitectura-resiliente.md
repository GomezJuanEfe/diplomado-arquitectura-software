# 0001 – Arquitectura resiliente por niveles de servicio

## Contexto

UltraSeguros necesita que su plataforma siga operando bajo fallos y picos de carga. El reto exige
3 niveles de servicio, degradación automática con 5 y 10 errores, recuperación escalonada, registro
de cada cambio y uso exclusivo de servicios de AWS.

Restricción medida en la cuenta: el límite es de **10 ejecuciones concurrentes de Lambda** en
`us-east-1`, sin margen para reservar concurrencia. Cada Lambda adicional en el camino de una
petición reduce la capacidad disponible en un pico.

**Atributo de calidad priorizado: Disponibilidad.** El sistema se degrada de a poco para seguir
funcionando en todo lo que aún puede, en vez de pasar de "todo" a "nada".

## Opciones consideradas

**Enrutamiento dinámico**

1. **Step Functions Express (elegida):** 1 Lambda por petición y flujo visible; VTL más laborioso.
2. **Lambda router:** simple, pero consume 2 de concurrencia por petición (unas 5 simultáneas).
3. **Stage variable con `update-stage`:** enruta en el gateway, pero depende del plano de control.

**Detección y decisión**

1. **Contador + barrido:** reacción inmediata; recuperación con lógica propia.
2. **Solo alarmas de CloudWatch:** desacopladas, pero tardan demasiado para una prueba de minutos.
3. **Híbrido (elegida):** degradar con contador atómico y recuperar con alarmas.

**Despliegue**

1. **CloudFormation:** reproducible y sin instalaciones; plantilla verbosa.
2. **AWS SAM:** más compacto, pero requiere instalar su CLI.
3. **AWS CLI en script (elegida):** se ve cada recurso creado; `deploy.sh` es repetible.

## Decisión

- **Entrada:** API Gateway REST, stage `niveles`, `GET /`, con throttling `rate=50`, `burst=100`.
  REST es necesaria: sus plantillas VTL limpian la salida de Step Functions y fijan el código HTTP.
- **Enrutamiento:** máquina Express `reto3-enrutador`: lee el nivel y un `Choice` elige el servicio.
- **Servicios:** `reto3-svc-full`, `reto3-svc-degradado` y `reto3-svc-minimo`.
- **Estado:** un ítem en DynamoDB (`nivel`, `errores`, `ultimo_error`, `actualizado_en`).
- **Degradación:** `ADD` atómico desde la máquina; con 5 y 10 errores sube el nivel con una escritura
  condicional.
- **Recuperación:** dos alarmas sobre `Errors`, EventBridge y la Lambda `reto3-controlador`, que sube
  un solo nivel por ejecución.
- **Registro:** DynamoDB Streams y `reto3-auditor`, único punto que escribe `NIVEL_CAMBIADO`.
- **Contingencia:** `Catch` hacia un `Pass` que responde sin Lambda y registra el fallo.

## Decisiones surgidas durante la implementación

- **Reconciliador cada minuto:** una alarma emite un solo evento; si se pierde, o si el fallo no
  generó métrica, el sistema quedaría degradado. La prueba de fallo real lo confirmó necesario: la
  recuperación la hizo él, a los 94 s.
- **Mínimos de calma (20 y 50 s) contra `ultimo_error`:** las alarmas van con retraso. Sin esta
  guarda, una ejecución del reconciliador durante una ráfaga podría reiniciar el contador antes de
  llegar a 5 errores.
- **Sin ventanas de tiempo fijas:** una ventana por minuto podría partir 15 errores en 7 y 8 y no
  llegar nunca a 10. El contador se reinicia con cada recuperación.

## Consecuencias (trade-offs)

- **Gana:** disponibilidad. Se recorren los 3 niveles y se recupera sin intervención; ninguna
  respuesta fue 502 ni 504 en la prueba.
- **Pierde:** latencia, por la orquestación (mediana de 227 ms en k6) y unos 3,5 s en contingencia
  por los reintentos.
- **Pierde:** simplicidad. Hay dos mecanismos de detección y un controlador que mantener.
- **Riesgo aceptado:** DynamoDB es dependencia única del estado, en una sola región.
- **Riesgo aceptado:** los fallos reales que impiden ejecutar la Lambda no generan la métrica
  `Errors`; degradan igual, pero el gráfico de errores no los muestra.
- **Riesgo aceptado:** la API es pública porque el script k6 no usa autenticación.
