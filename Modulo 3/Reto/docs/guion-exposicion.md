# Guion de la exposición en vivo

La prueba dura unos 7,5 minutos. Lánzala al empezar a explicar la arquitectura: así la degradación
ocurre mientras hablas. k6 espera hasta el siguiente minuto exacto antes de empezar.

## Antes de exponer

- No ejecutes `destroy.sh`: la API debe seguir desplegada.
- Confirma que `reto3-recupera-l2` y `reto3-recupera-l3` no estén en ALARM.
- Confirma que el estado está en nivel 1 y 0 errores:

```bash
aws dynamodb get-item --region us-east-1 --table-name reto3-estado \
  --key '{"id":{"S":"estado"}}' --query '[Item.nivel.N,Item.errores.N]' --output text
```

- Haz un `curl` a la API unos minutos antes, para calentar las Lambdas. Debe responder Nivel 1.
- Abre: terminal con k6, dashboard `reto3-resiliencia`, máquina `reto3-enrutador` y este comando:

```bash
aws logs tail /aws/lambda/reto3-auditor --region us-east-1 --follow --format short \
  --filter-pattern NIVEL_CAMBIADO
```

- Alternativa más visual: `python3 panel/servidor.py`, abre `http://127.0.0.1:8091/slides` y lanza
  la prueba desde la diapositiva 8; el panel completo está en `http://127.0.0.1:8091`. Si recargas la página, la corrida se conserva; si reinicias el servidor, no.
- Ten a mano el PDF y `docs/evidencia/`: son tu respaldo.

## Secuencia

Los minutos de la prueba son aproximados y cuentan desde que k6 envía la primera petición.

| Prueba | Pantalla | Qué decir |
|---|---|---|
| Antes de lanzar k6 | Diagrama del PDF | El problema: el sistema fallaba bajo carga. Priorizo disponibilidad: se degrada de a poco para seguir funcionando en todo lo que puede. |
| 0:00 | Terminal k6 y logs | Primer bloque: 5 errores. Pasa a Nivel 2 sin llegar a Nivel 3. **Detecta:** cada error suma 1 en un contador atómico. |
| 0:30–2:00 | Máquina de estados | API Gateway no enruta según el estado, así que Step Functions lee el nivel y elige el servicio. Así gasto 1 Lambda por petición: la cuenta tiene un límite de 10. |
| 2:05–2:50 | Terminal k6 | **Degrada:** 15 errores, pasa a 2 y luego a 3. Muestra los dos mensajes de Nivel 3. La petición que cruza el umbral aún la atiende el nivel anterior. |
| 2:50–4:10 | Dashboard y alarmas | **Se recupera:** subir es más lento que bajar a propósito. Alarma OK más calma: unos 45 s a Nivel 2 y 75 s a Nivel 1. Si un aviso se pierde, el reconciliador corrige cada minuto. |
| 4:10–5:00 | Terminal k6 | Tercer bloque: lo mismo, sin intervención humana. |
| 6:30 en adelante | Logs y dashboard | **Evidencia:** secuencia completa 1, 2, 1, 2, 3, 2, 1, 2, 3, 2, 1, sin ningún 502 ni 504. Cierre: gané disponibilidad y pagué latencia y complejidad. |

## Si algo sale distinto

- **El bloque 3 empieza en Nivel 2:** es válido. El margen entre bloques es de unos 9 s y la
  recuperación puede no alcanzar. Se cumplen igual los 3 niveles y la recuperación final.
- **Falla el Wi-Fi o AWS:** usa los resultados del PDF y `docs/evidencia/k6-transiciones.jsonl`.
- **Una corrida anterior acaba de terminar:** espera a que el estado vuelva a 1 y 0.
- **Se acaba el tiempo:** deja k6 corriendo y muestra la evidencia guardada.

## Si preguntan

- **¿Aguanta un pico real?** No está medido: el script envía 0,31 peticiones por segundo. El límite
  de 10 Lambdas es el principal riesgo pendiente.
- **¿Por qué no solo alarmas?** Tardan demasiado para ser exactas con 5 y 10 errores.
- **¿Por qué dos alarmas?** Cada una emite un solo evento al volver a OK.
- **¿Modificaste el script?** Solo el ID de la API, en la línea 47.
