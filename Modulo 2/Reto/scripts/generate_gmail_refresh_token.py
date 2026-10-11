"""
Genera un refresh_token de Gmail API (scope gmail.send) para usar desde la Lambda.

Ejecutar UNA SOLA VEZ, en tu maquina local (no en AWS).

Requisitos previos:
1. Crear un proyecto en https://console.cloud.google.com/
2. Habilitar la "Gmail API" para ese proyecto.
3. Crear credenciales OAuth 2.0 de tipo "Aplicacion de escritorio" (Desktop app).
4. Tener a mano el client_id y client_secret de esas credenciales.

Uso:
    python scripts/generate_gmail_refresh_token.py

El script abre el navegador para el consentimiento de Google, captura el
codigo via un servidor local efimero en http://localhost:8080/, y lo
intercambia por un refresh_token. Ese refresh_token es el que se configura
como variable de entorno GMAIL_REFRESH_TOKEN en la Lambda.
"""
import http.server
import json
import os
import urllib.parse
import urllib.request
import webbrowser

SCOPE = "https://www.googleapis.com/auth/gmail.send"
REDIRECT_PORT = 8080
REDIRECT_URI = f"http://localhost:{REDIRECT_PORT}/"
AUTH_ENDPOINT = "https://accounts.google.com/o/oauth2/v2/auth"
TOKEN_ENDPOINT = "https://oauth2.googleapis.com/token"


class _CallbackHandler(http.server.BaseHTTPRequestHandler):
    auth_code = None

    def do_GET(self):
        query = urllib.parse.urlparse(self.path).query
        params = urllib.parse.parse_qs(query)
        _CallbackHandler.auth_code = params.get("code", [None])[0]
        self.send_response(200)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.end_headers()
        self.wfile.write("Listo, ya puedes cerrar esta pestana.".encode("utf-8"))

    def log_message(self, format, *args):  # silenciar logs del servidor HTTP
        pass


def _get_authorization_code(client_id: str) -> str:
    params = {
        "client_id": client_id,
        "redirect_uri": REDIRECT_URI,
        "response_type": "code",
        "scope": SCOPE,
        "access_type": "offline",
        "prompt": "consent",  # fuerza a Google a devolver refresh_token
    }
    url = f"{AUTH_ENDPOINT}?{urllib.parse.urlencode(params)}"
    print(f"Abriendo el navegador para autorizar...\n{url}\n")
    webbrowser.open(url)

    server = http.server.HTTPServer(("localhost", REDIRECT_PORT), _CallbackHandler)
    server.handle_request()  # bloquea hasta recibir el callback de Google
    server.server_close()

    if not _CallbackHandler.auth_code:
        raise RuntimeError("No se recibio el codigo de autorizacion.")
    return _CallbackHandler.auth_code


def _exchange_code_for_tokens(client_id: str, client_secret: str, code: str) -> dict:
    data = urllib.parse.urlencode(
        {
            "client_id": client_id,
            "client_secret": client_secret,
            "code": code,
            "redirect_uri": REDIRECT_URI,
            "grant_type": "authorization_code",
        }
    ).encode("utf-8")
    request = urllib.request.Request(TOKEN_ENDPOINT, data=data, method="POST")
    with urllib.request.urlopen(request) as response:
        return json.loads(response.read().decode("utf-8"))


def main() -> None:
    client_id = os.environ.get("GMAIL_CLIENT_ID") or input("Client ID: ").strip()
    client_secret = os.environ.get("GMAIL_CLIENT_SECRET") or input("Client secret: ").strip()

    code = _get_authorization_code(client_id)
    tokens = _exchange_code_for_tokens(client_id, client_secret, code)

    refresh_token = tokens.get("refresh_token")
    if not refresh_token:
        raise RuntimeError(
            "Google no devolvio refresh_token. Revoca el acceso previo en "
            "https://myaccount.google.com/permissions y vuelve a correr este script."
        )

    print("\nRefresh token generado. Guardalo como variable de entorno GMAIL_REFRESH_TOKEN en la Lambda:\n")
    print(refresh_token)


if __name__ == "__main__":
    main()
