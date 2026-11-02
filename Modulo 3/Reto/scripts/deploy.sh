#!/usr/bin/env bash
# Despliega el reto 3 con AWS CLI. Se puede ejecutar varias veces sin duplicar recursos.
set -euo pipefail

REGION="${AWS_REGION:-us-east-1}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CUENTA="$(aws sts get-caller-identity --query Account --output text)"
TABLA="reto3-estado"
ROL_SERVICIOS="reto3-servicios-role"
ROL_SFN="reto3-sfn-role"
ROL_APIGW="reto3-apigw-role"
ROL_AUDITOR="reto3-auditor-role"
DASHBOARD="reto3-resiliencia"
ROL_CONTROLADOR="reto3-controlador-role"
NAMESPACE="UltraSeguros/Resiliencia"
MAQUINA="reto3-enrutador"
API="reto3-api"
STAGE="niveles"
BUILD="$(mktemp -d)"
trap 'rm -rf "$BUILD"' EXIT

echo "== DynamoDB: $TABLA"
if ! aws dynamodb describe-table --region "$REGION" --table-name "$TABLA" >/dev/null 2>&1; then
  aws dynamodb create-table --region "$REGION" \
    --table-name "$TABLA" \
    --attribute-definitions AttributeName=id,AttributeType=S \
    --key-schema AttributeName=id,KeyType=HASH \
    --billing-mode PAY_PER_REQUEST \
    --stream-specification StreamEnabled=true,StreamViewType=NEW_AND_OLD_IMAGES >/dev/null
  aws dynamodb wait table-exists --region "$REGION" --table-name "$TABLA"
fi

# La condicion evita reiniciar el nivel si se vuelve a desplegar durante una prueba.
aws dynamodb put-item --region "$REGION" \
  --table-name "$TABLA" \
  --item "{\"id\":{\"S\":\"estado\"},\"nivel\":{\"N\":\"1\"},\"errores\":{\"N\":\"0\"},\"actualizado_en\":{\"S\":\"$(date -u +%Y-%m-%dT%H:%M:%SZ)\"}}" \
  --condition-expression "attribute_not_exists(id)" >/dev/null 2>&1 || true
ESTADO="$(aws dynamodb get-item --region "$REGION" --table-name "$TABLA" \
  --key '{"id":{"S":"estado"}}' --query 'Item.nivel.N' --output text)"
if [ "$ESTADO" = "None" ]; then
  echo "No se pudo inicializar el estado en $TABLA" >&2
  exit 1
fi

echo "== IAM: $ROL_SERVICIOS"
if ! aws iam get-role --role-name "$ROL_SERVICIOS" >/dev/null 2>&1; then
  aws iam create-role --role-name "$ROL_SERVICIOS" \
    --assume-role-policy-document "file://$ROOT/infra/policies/lambda-trust.json" >/dev/null
  aws iam attach-role-policy --role-name "$ROL_SERVICIOS" \
    --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
fi
ROL_ARN="$(aws iam get-role --role-name "$ROL_SERVICIOS" --query Role.Arn --output text)"

desplegar_lambda() {
  local nombre="$1" carpeta="$2" rol="$3" timeout="${4:-3}" zip="$BUILD/$2.zip"
  python3 -m zipfile -c "$zip" "$ROOT/lambdas/$carpeta/handler.py" "$ROOT/lambdas/comun/metricas.py"

  if aws lambda get-function --region "$REGION" --function-name "$nombre" >/dev/null 2>&1; then
    aws lambda update-function-code --region "$REGION" \
      --function-name "$nombre" --zip-file "fileb://$zip" >/dev/null
    aws lambda wait function-updated --region "$REGION" --function-name "$nombre"
    return
  fi

  # Un rol recien creado tarda unos segundos en propagarse dentro de IAM.
  for intento in 1 2 3 4 5 6; do
    if aws lambda create-function --region "$REGION" \
      --function-name "$nombre" \
      --runtime python3.13 \
      --handler handler.lambda_handler \
      --role "$rol" \
      --timeout "$timeout" \
      --zip-file "fileb://$zip" >/dev/null 2>&1; then
      aws lambda wait function-active-v2 --region "$REGION" --function-name "$nombre"
      return
    fi
    echo "   esperando propagacion de IAM (intento $intento)..."
    sleep 5
  done
  echo "No se pudo crear la Lambda $nombre" >&2
  return 1
}

echo "== Lambdas de nivel"
desplegar_lambda reto3-svc-full full "$ROL_ARN"
desplegar_lambda reto3-svc-degradado degradado "$ROL_ARN"
desplegar_lambda reto3-svc-minimo minimo "$ROL_ARN"

# Sustituye los marcadores de una plantilla de infra/ y deja el resultado en $BUILD.
renderizar() {
  local origen="$1" destino="$BUILD/$(basename "$1")"
  sed -e "s/__REGION__/$REGION/g" -e "s/__ACCOUNT__/$CUENTA/g" "$origen" > "$destino"
  echo "$destino"
}

asegurar_rol() {
  local rol="$1" confianza="$2" politica="$3"
  if ! aws iam get-role --role-name "$rol" >/dev/null 2>&1; then
    aws iam create-role --role-name "$rol" \
      --assume-role-policy-document "file://$(renderizar "$confianza")" >/dev/null
  fi
  aws iam put-role-policy --role-name "$rol" --policy-name "$rol-inline" \
    --policy-document "file://$(renderizar "$politica")"
}

echo "== IAM: $ROL_SFN y $ROL_APIGW"
asegurar_rol "$ROL_SFN" "$ROOT/infra/policies/sfn-trust.json" "$ROOT/infra/policies/sfn-policy.json"
asegurar_rol "$ROL_APIGW" "$ROOT/infra/policies/apigw-trust.json" "$ROOT/infra/policies/apigw-policy.json"

echo "== Step Functions: $MAQUINA"
MAQUINA_ARN="arn:aws:states:$REGION:$CUENTA:stateMachine:$MAQUINA"
ASL="$(renderizar "$ROOT/infra/statemachine.asl.json")"
if aws stepfunctions describe-state-machine --region "$REGION" --state-machine-arn "$MAQUINA_ARN" >/dev/null 2>&1; then
  aws stepfunctions update-state-machine --region "$REGION" \
    --state-machine-arn "$MAQUINA_ARN" --definition "file://$ASL" >/dev/null
else
  creada=""
  for intento in 1 2 3 4 5 6; do
    if aws stepfunctions create-state-machine --region "$REGION" \
      --name "$MAQUINA" --type EXPRESS \
      --role-arn "arn:aws:iam::$CUENTA:role/$ROL_SFN" \
      --definition "file://$ASL" >/dev/null 2>&1; then
      creada="si"
      break
    fi
    echo "   esperando propagacion de IAM (intento $intento)..."
    sleep 5
  done
  if [ -z "$creada" ]; then
    echo "No se pudo crear la maquina $MAQUINA" >&2
    exit 1
  fi
fi

echo "== API Gateway: $API (stage $STAGE)"
API_ID="$(aws apigateway get-rest-apis --region "$REGION" \
  --query "items[?name=='$API'].id | [0]" --output text)"
if [ "$API_ID" = "None" ]; then
  API_ID="$(aws apigateway create-rest-api --region "$REGION" --name "$API" \
    --endpoint-configuration types=REGIONAL --query id --output text)"
fi
RAIZ_ID="$(aws apigateway get-resources --region "$REGION" --rest-api-id "$API_ID" \
  --query "items[?path=='/'].id | [0]" --output text)"

aws apigateway put-method --region "$REGION" --rest-api-id "$API_ID" --resource-id "$RAIZ_ID" \
  --http-method GET --authorization-type NONE >/dev/null 2>&1 || true

PLANTILLA_PETICION="$(sed "s|__STATE_MACHINE_ARN__|$MAQUINA_ARN|" "$ROOT/infra/request.vtl")"
aws apigateway put-integration --region "$REGION" --rest-api-id "$API_ID" --resource-id "$RAIZ_ID" \
  --http-method GET --type AWS --integration-http-method POST \
  --uri "arn:aws:apigateway:$REGION:states:action/StartSyncExecution" \
  --credentials "arn:aws:iam::$CUENTA:role/$ROL_APIGW" \
  --passthrough-behavior WHEN_NO_TEMPLATES \
  --request-templates "$(python3 -c 'import json,sys; print(json.dumps({"application/json": sys.argv[1]}))' "$PLANTILLA_PETICION")" >/dev/null

aws apigateway put-method-response --region "$REGION" --rest-api-id "$API_ID" --resource-id "$RAIZ_ID" \
  --http-method GET --status-code 200 --response-models '{"application/json":"Empty"}' >/dev/null 2>&1 || true

aws apigateway put-integration-response --region "$REGION" --rest-api-id "$API_ID" --resource-id "$RAIZ_ID" \
  --http-method GET --status-code 200 --selection-pattern "" \
  --response-templates "$(python3 -c 'import json,sys; print(json.dumps({"application/json": open(sys.argv[1], encoding="utf-8").read()}))' "$ROOT/infra/response.vtl")" >/dev/null

aws apigateway create-deployment --region "$REGION" --rest-api-id "$API_ID" \
  --stage-name "$STAGE" >/dev/null

aws apigateway update-stage --region "$REGION" --rest-api-id "$API_ID" --stage-name "$STAGE" \
  --patch-operations \
  'op=replace,path=/*/*/throttling/rateLimit,value=50' \
  'op=replace,path=/*/*/throttling/burstLimit,value=100' >/dev/null

echo "== Auditor de niveles"
asegurar_rol "$ROL_AUDITOR" "$ROOT/infra/policies/lambda-trust.json" "$ROOT/infra/policies/auditor-policy.json"
desplegar_lambda reto3-auditor auditor "arn:aws:iam::$CUENTA:role/$ROL_AUDITOR"

STREAM_ARN="$(aws dynamodb describe-table --region "$REGION" --table-name "$TABLA" \
  --query Table.LatestStreamArn --output text)"
MAPEOS="$(aws lambda list-event-source-mappings --region "$REGION" --function-name reto3-auditor \
  --query 'length(EventSourceMappings)' --output text)"
if [ "$MAPEOS" = "0" ]; then
  for intento in 1 2 3 4 5 6; do
    if aws lambda create-event-source-mapping --region "$REGION" \
      --function-name reto3-auditor \
      --event-source-arn "$STREAM_ARN" \
      --starting-position LATEST \
      --batch-size 10 >/dev/null 2>&1; then
      break
    fi
    echo "   esperando propagacion de IAM (intento $intento)..."
    sleep 5
    if [ "$intento" = "6" ]; then
      echo "No se pudo conectar el stream al auditor" >&2
      exit 1
    fi
  done
fi

echo "== Alarmas de recuperacion"
crear_alarma() {
  local nombre="$1" periodos="$2"
  aws cloudwatch put-metric-alarm --region "$REGION" \
    --alarm-name "$nombre" \
    --namespace "$NAMESPACE" --metric-name Errors --statistic Sum \
    --period 10 --evaluation-periods "$periodos" --datapoints-to-alarm 1 \
    --threshold 1 --comparison-operator GreaterThanOrEqualToThreshold \
    --treat-missing-data notBreaching
}
crear_alarma reto3-recupera-l3 3
crear_alarma reto3-recupera-l2 6

echo "== Controlador de recuperacion"
asegurar_rol "$ROL_CONTROLADOR" "$ROOT/infra/policies/lambda-trust.json" "$ROOT/infra/policies/controlador-policy.json"
desplegar_lambda reto3-controlador controlador "arn:aws:iam::$CUENTA:role/$ROL_CONTROLADOR" 10
CONTROLADOR_ARN="arn:aws:lambda:$REGION:$CUENTA:function:reto3-controlador"

conectar_regla() {
  local regla="$1"
  aws events put-targets --region "$REGION" --rule "$regla" \
    --targets "Id=controlador,Arn=$CONTROLADOR_ARN" >/dev/null
  # Si el permiso ya existe, add-permission falla y no hay nada que cambiar.
  aws lambda add-permission --region "$REGION" \
    --function-name reto3-controlador \
    --statement-id "$regla" \
    --action lambda:InvokeFunction \
    --principal events.amazonaws.com \
    --source-arn "arn:aws:events:$REGION:$CUENTA:rule/$regla" >/dev/null 2>&1 || true
}

aws events put-rule --region "$REGION" --name reto3-alarmas-ok \
  --event-pattern '{"source":["aws.cloudwatch"],"detail-type":["CloudWatch Alarm State Change"],"detail":{"alarmName":["reto3-recupera-l3","reto3-recupera-l2"],"state":{"value":["OK"]}}}' >/dev/null
conectar_regla reto3-alarmas-ok

aws events put-rule --region "$REGION" --name reto3-reconciliador \
  --schedule-expression "rate(1 minute)" >/dev/null
conectar_regla reto3-reconciliador

echo "== Dashboard: $DASHBOARD"
aws cloudwatch put-dashboard --region "$REGION" --dashboard-name "$DASHBOARD" \
  --dashboard-body "file://$(renderizar "$ROOT/infra/dashboard.json")" >/dev/null

echo "URL: https://$API_ID.execute-api.$REGION.amazonaws.com/$STAGE"
echo "Despliegue terminado en $REGION"
