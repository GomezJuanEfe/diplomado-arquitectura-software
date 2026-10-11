# Evidencia de logs de ejecución

`logs-emergencia-raw.txt` es la salida textual de CloudWatch Logs (`aws logs tail
/aws/lambda/reto2-emergency-processor --region sa-east-1`) para una invocación real generada por la
corrida de `k6-script.js` del 2026-10-11, placa `TNJ-388`, `RequestId=d2ebf20d-7fd6-4a46-89d2-fae6f20732bc`.

Muestra, con hora exacta (UTC), los dos puntos que exige la rúbrica:

- `[EMERGENCY_RECEIVED] 2026-10-11T03:21:24.535+00:00` — recepción del evento `Emergency`.
- `[EMAIL_SENT] 2026-10-11T03:21:25.566+00:00` — envío exitoso del correo a Gmail.

**Delta recepción → envío del correo: ~1.03s** (muy por debajo del umbral de 15s de la rúbrica).

Para reproducir esta evidencia después de otra corrida de k6:

```bash
aws logs tail /aws/lambda/reto2-emergency-processor --region sa-east-1 --since 10m \
  | grep -E "EMERGENCY_RECEIVED|EMAIL_SENT|EMAIL_SEND_FAILED"
```
