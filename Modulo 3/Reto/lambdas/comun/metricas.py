"""Metricas por Embedded Metric Format: CloudWatch las extrae del log de la Lambda."""

import json
import time

NAMESPACE = "UltraSeguros/Resiliencia"


def _metrica(nombre, unidad):
    return {"Name": nombre, "Unit": unidad, "StorageResolution": 1}


def emitir(**valores):
    """Publica cada valor como metrica de alta resolucion, sin dimensiones."""
    print(json.dumps({
        "_aws": {
            "Timestamp": int(time.time() * 1000),
            "CloudWatchMetrics": [{
                "Namespace": NAMESPACE,
                "Dimensions": [[]],
                "Metrics": [_metrica(nombre, "None") for nombre in valores],
            }],
        },
        **valores,
    }))
