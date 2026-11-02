"""Servicio de Nivel 2 (Degradado): conserva solo las capacidades esenciales."""

from metricas import emitir

CAPACIDADES = ["transacciones", "monitoreo"]


def lambda_handler(event, context):
    fallo = bool(event.get("error"))
    emitir(Requests=1, Errors=int(fallo), ServiceLevel=2)

    if fallo:
        return {
            "ok": False,
            "statusCode": 500,
            "body": {"nivel": 2, "mensaje": "Nivel 2: Error procesando la solicitud"},
        }

    return {
        "ok": True,
        "statusCode": 200,
        "body": {
            "nivel": 2,
            "mensaje": "Nivel 2: Servicio degradado",
            "capacidades": CAPACIDADES,
        },
    }
