# Reto Módulo 1 — Reto B: Latencia HTTP (ingenuo vs. keep-alive)

Requisito: Python 3.8+ (solo librería estándar, no hay dependencias externas).

## Estructura

- `ruido.py` — mide el "ruido" propio del reloj y del intérprete: cuánto
  tiempo pasa entre dos llamadas consecutivas a `time.perf_counter_ns()`
  sin hacer nada de por medio. Sirve como piso de referencia antes de medir
  cualquier otra cosa.
- `servidor_ingenuo.py` — servidor HTTP mínimo (`http.server`) que responde
  siempre el mismo cuerpo en `127.0.0.1:8080`.
- `cliente_ingenuo.py` — cliente que abre una conexión HTTP nueva
  (`urllib.request.urlopen`) por cada una de las 2000 peticiones y mide el
  round-trip de cada una. Guarda las muestras en `baseline.log`.
- `cliente_keepalive.py` — mismo experimento pero reutilizando una sola
  conexión TCP (`http.client.HTTPConnection`) para las 2000 peticiones.
  Guarda las muestras en `keepalive.log`.

## Cómo ejecutar

El servidor debe quedar corriendo en una terminal mientras el cliente
correspondiente se ejecuta en otra.

### Linux / macOS

```bash
# Terminal 1
python3 servidor_ingenuo.py

# Terminal 2 (con el servidor ya corriendo)
python3 cliente_ingenuo.py
python3 cliente_keepalive.py

# Cuando termines, detén el servidor con Ctrl+C en la Terminal 1
```

Medición del ruido propio del reloj (no necesita el servidor):

```bash
python3 ruido.py
```

### Windows (PowerShell o CMD)

```powershell
# Terminal 1
python servidor_ingenuo.py

# Terminal 2 (con el servidor ya corriendo)
python cliente_ingenuo.py
python cliente_keepalive.py

# Cuando termines, detén el servidor con Ctrl+C en la Terminal 1
```

Medición del ruido propio del reloj (no necesita el servidor):

```powershell
python ruido.py
```

> En Windows el ejecutable suele llamarse `python` (o `py`), no `python3`.
> Si `python` no se reconoce, probá con `py -3 servidor_ingenuo.py`.

## Notas

- Ambos clientes apuntan a `127.0.0.1:8080` — no hace falta abrir puertos
  ni configurar firewall, todo corre en loopback.
- `cliente_ingenuo.py` y `cliente_keepalive.py` sobrescriben `baseline.log`
  y `keepalive.log` respectivamente en cada corrida.
- Si el puerto 8080 ya está en uso, cerrá el proceso que lo ocupa o cambiá
  el puerto en `servidor_ingenuo.py` y en el cliente correspondiente (deben
  coincidir).
