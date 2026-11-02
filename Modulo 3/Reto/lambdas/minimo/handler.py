"""Servicio de Nivel 3 (Operacion Minima): solo responde mensajes de mantenimiento."""

from metricas import emitir

MENSAJE_FALLO = "Nivel 3: Sistema bajo mantenimiento, intente más tarde"
MENSAJE_EXITO = "Nivel 3: Operación al mínimo"


def lambda_handler(event, context):
    fallo = bool(event.get("error"))
    emitir(Requests=1, Errors=int(fallo), ServiceLevel=3)

    if fallo:
        return {
            "ok": False,
            "statusCode": 503,
            "body": {"nivel": 3, "mensaje": MENSAJE_FALLO},
        }

    return {
        "ok": True,
        "statusCode": 200,
        "body": {"nivel": 3, "mensaje": MENSAJE_EXITO},
    }
