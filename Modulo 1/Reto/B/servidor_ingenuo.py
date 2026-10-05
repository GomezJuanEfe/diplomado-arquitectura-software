# mínimo: 200us
# p50:    300us
# p90:    720us
# máximo: 1000us
from http.server import BaseHTTPRequestHandler, HTTPServer

class Handler(BaseHTTPRequestHandler):
  protocol_version = "HTTP/1.1"
  disable_nagle_algorithm = True
  def do_GET(self):
    cuerpo = b"respuesta"
    self.send_response(200)
    self.send_header("Content-Length", str(len(cuerpo)))
    self.end_headers()
    self.wfile.write(cuerpo)

  def log_message(self, *args):
    pass  #silenciamos el log: escribir en consola cuesta tiempo

HTTPServer(("127.0.0.1", 8080), Handler).serve_forever()