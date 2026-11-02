"""Servicio de Nivel 1 (Full): responde con todas las capacidades activas."""

from metricas import emitir

CAPACIDADES = ["transacciones", "analitica", "monitoreo"]


def lambda_handler(event, context):
    fallo = bool(event.get("error"))
    emitir(Requests=1, Errors=int(fallo), ServiceLevel=1)

    if fallo:
        return {
            "ok": False,
            "statusCode": 500,
            "body": {"nivel": 1, "mensaje": "Nivel 1: Error procesando la solicitud"},
        }

    return {
        "ok": True,
        "statusCode": 200,
        "body": {
            "nivel": 1,
            "mensaje": "Nivel 1: Servicio completo",
            "capacidades": CAPACIDADES,
        },
    }
