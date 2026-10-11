"""
Lambda handler: recibe eventos de vehiculos via API Gateway, detecta eventos
"Emergency" y envia una alerta por Gmail API.

Sin dependencias externas (solo stdlib) para poder pegarse directo en el
editor inline de la consola de Lambda, sin zip ni layers.

Variables de entorno requeridas:
  GMAIL_CLIENT_ID
  GMAIL_CLIENT_SECRET
  GMAIL_REFRESH_TOKEN   (generado una vez con scripts/generate_gmail_refresh_token.py)
  GMAIL_TO              (direccion Gmail que recibe la alerta)
"""
import base64
import json
import os
import urllib.parse
import urllib.request
from datetime import datetime, timezone
from email.mime.text import MIMEText

TOKEN_ENDPOINT = "https://oauth2.googleapis.com/token"
SEND_ENDPOINT = "https://gmail.googleapis.com/gmail/v1/users/me/messages/send"


def _now() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="milliseconds")


def _get_access_token() -> str:
    data = urllib.parse.urlencode(
        {
            "client_id": os.environ["GMAIL_CLIENT_ID"],
            "client_secret": os.environ["GMAIL_CLIENT_SECRET"],
            "refresh_token": os.environ["GMAIL_REFRESH_TOKEN"],
            "grant_type": "refresh_token",
        }
    ).encode("utf-8")
    request = urllib.request.Request(TOKEN_ENDPOINT, data=data, method="POST")
    with urllib.request.urlopen(request, timeout=5) as response:
        return json.loads(response.read().decode("utf-8"))["access_token"]


def _send_alert_email(plate: str, status: str) -> None:
    body = (
        "## Alerta de Emergencia\n\n"
        f"Placa: {plate}\n\n"
        f"Estado: {status}\n\n"
        "Evento: Emergency"
    )
    message = MIMEText(body)
    message["to"] = os.environ["GMAIL_TO"]
    message["subject"] = "Alerta de Emergencia"
    raw = base64.urlsafe_b64encode(message.as_bytes()).decode("utf-8")

    access_token = _get_access_token()
    request = urllib.request.Request(
        SEND_ENDPOINT,
        data=json.dumps({"raw": raw}).encode("utf-8"),
        headers={
            "Authorization": f"Bearer {access_token}",
            "Content-Type": "application/json",
        },
        method="POST",
    )
    with urllib.request.urlopen(request, timeout=5) as response:
        response.read()


def lambda_handler(event, context):
    payload = json.loads(event.get("body") or "{}")
    event_type = payload.get("type")

    if event_type == "Emergency":
        plate = payload.get("vehicle_plate", "?")
        print(f"[EMERGENCY_RECEIVED] {_now()} plate={plate}")
        try:
            _send_alert_email(plate=plate, status=payload.get("status", "?"))
            print(f"[EMAIL_SENT] {_now()} plate={plate}")
        except Exception as exc:  # queremos loguear cualquier fallo de envio sin romper la respuesta HTTP
            print(f"[EMAIL_SEND_FAILED] {_now()} plate={plate} error={exc}")

    return {
        "statusCode": 200,
        "body": json.dumps({"status": "ok"}),
    }
