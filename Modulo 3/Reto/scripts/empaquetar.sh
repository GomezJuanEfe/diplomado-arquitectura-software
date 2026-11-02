#!/usr/bin/env bash
# Genera el ZIP de entrega con el codigo fuente del reto 3.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SALIDA="$ROOT/reto3-codigo-fuente.zip"

cd "$ROOT"
rm -f "$SALIDA"
zip -r "$SALIDA" README.md "K6 script - reto3.js" lambdas infra scripts \
  docs/decisiones docs/evidencia -x '*/__pycache__/*' >/dev/null
echo "$SALIDA"
