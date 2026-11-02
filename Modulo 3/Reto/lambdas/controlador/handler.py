"""Recupera el servicio un nivel a la vez segun las alarmas de CloudWatch."""

import json
from datetime import datetime, timezone

import boto3
from boto3.dynamodb.conditions import Attr

TABLA = "reto3-estado"
ALARMA_L3 = "reto3-recupera-l3"
ALARMA_L2 = "reto3-recupera-l2"

# Las alarmas reaccionan con retraso: una alarma en OK no prueba que no haya errores
# recientes. Estos minimos de calma se miden contra el ultimo error registrado.
CALMA_L3_SEGUNDOS = 20
CALMA_L2_SEGUNDOS = 50

tabla = boto3.resource("dynamodb").Table(TABLA)
cloudwatch = boto3.client("cloudwatch")


def segundos_sin_errores(item, ahora):
    ultimo_error = item.get("ultimo_error")
    if ultimo_error is None:
        return float("inf")
    return (ahora - datetime.fromisoformat(ultimo_error)).total_seconds()


def decidir(nivel, errores, alarmas, calma):
    """Devuelve el nivel al que se debe pasar, o None si no corresponde actuar."""
    if nivel == 3 and alarmas.get(ALARMA_L3) == "OK" and calma >= CALMA_L3_SEGUNDOS:
        return 2

    estable = alarmas.get(ALARMA_L2) == "OK" and calma >= CALMA_L2_SEGUNDOS
    if nivel == 2 and estable:
        return 1
    if nivel == 1 and errores > 0 and estable:
        return 1
    return None


def leer_alarmas():
    respuesta = cloudwatch.describe_alarms(AlarmNames=[ALARMA_L3, ALARMA_L2])
    return {a["AlarmName"]: a["StateValue"] for a in respuesta["MetricAlarms"]}


def aplicar(nivel, errores, nuevo_nivel, ahora):
    """Escribe solo si nada cambio desde la lectura; asi no se pierde un error nuevo."""
    try:
        tabla.update_item(
            Key={"id": "estado"},
            UpdateExpression="SET nivel = :nivel, errores = :cero, actualizado_en = :ahora",
            ConditionExpression=Attr("nivel").eq(nivel) & Attr("errores").eq(errores),
            ExpressionAttributeValues={
                ":nivel": nuevo_nivel,
                ":cero": 0,
                ":ahora": ahora.isoformat(timespec="milliseconds"),
            },
        )
    except tabla.meta.client.exceptions.ConditionalCheckFailedException:
        return False
    return True


def lambda_handler(event, context):
    ahora = datetime.now(timezone.utc)
    item = tabla.get_item(Key={"id": "estado"}, ConsistentRead=True)["Item"]
    nivel = int(item["nivel"])
    errores = int(item["errores"])
    calma = segundos_sin_errores(item, ahora)

    nuevo_nivel = decidir(nivel, errores, leer_alarmas(), calma)
    if nuevo_nivel is None:
        return

    aplicado = aplicar(nivel, errores, nuevo_nivel, ahora)
    print(json.dumps({
        "evento": "RECUPERACION",
        "desde": nivel,
        "hacia": nuevo_nivel,
        "aplicado": aplicado,
        "segundos_sin_errores": round(calma, 1),
    }))
