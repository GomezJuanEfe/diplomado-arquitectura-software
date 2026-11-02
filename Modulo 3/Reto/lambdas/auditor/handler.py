"""Registra cada cambio de nivel a partir del stream de DynamoDB."""

import json
from datetime import datetime, timezone

from metricas import emitir


def lambda_handler(event, context):
    for registro in event["Records"]:
        if registro["eventName"] != "MODIFY":
            continue

        imagen_anterior = registro["dynamodb"]["OldImage"]
        imagen_nueva = registro["dynamodb"]["NewImage"]
        desde = int(imagen_anterior["nivel"]["N"])
        hacia = int(imagen_nueva["nivel"]["N"])
        if desde == hacia:
            continue

        print(json.dumps({
            "evento": "NIVEL_CAMBIADO",
            "desde": desde,
            "hacia": hacia,
            "errores": int(imagen_nueva["errores"]["N"]),
            "timestamp": datetime.now(timezone.utc).isoformat(timespec="milliseconds"),
        }))
        emitir(ServiceLevel=hacia)
