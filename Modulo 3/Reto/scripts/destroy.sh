#!/usr/bin/env bash
# Elimina todos los recursos del reto 3. Es irreversible: exige pasar --confirmar.
# Las metricas personalizadas no se pueden borrar; CloudWatch las expira solo.
set -uo pipefail

if [ "${1:-}" != "--confirmar" ]; then
  echo "Esto borra la API, las Lambdas, la tabla, las alarmas y los logs del reto 3." >&2
  echo "Para continuar: $0 --confirmar" >&2
  exit 1
fi

REGION="${AWS_REGION:-us-east-1}"
LAMBDAS="reto3-svc-full reto3-svc-degradado reto3-svc-minimo reto3-auditor reto3-controlador"

echo "== Reglas de EventBridge"
for regla in reto3-alarmas-ok reto3-reconciliador; do
  aws events remove-targets --region "$REGION" --rule "$regla" --ids controlador >/dev/null
  aws events delete-rule --region "$REGION" --name "$regla"
done

echo "== Alarmas y dashboard"
aws cloudwatch delete-alarms --region "$REGION" --alarm-names reto3-recupera-l3 reto3-recupera-l2
aws cloudwatch delete-dashboards --region "$REGION" --dashboard-names reto3-resiliencia >/dev/null

echo "== API Gateway"
API_ID="$(aws apigateway get-rest-apis --region "$REGION" \
  --query "items[?name=='reto3-api'].id | [0]" --output text)"
if [ "$API_ID" != "None" ]; then
  aws apigateway delete-rest-api --region "$REGION" --rest-api-id "$API_ID"
fi

echo "== Step Functions"
CUENTA="$(aws sts get-caller-identity --query Account --output text)"
aws stepfunctions delete-state-machine --region "$REGION" \
  --state-machine-arn "arn:aws:states:$REGION:$CUENTA:stateMachine:reto3-enrutador"

echo "== Stream y Lambdas"
for uuid in $(aws lambda list-event-source-mappings --region "$REGION" \
  --function-name reto3-auditor --query 'EventSourceMappings[].UUID' --output text); do
  aws lambda delete-event-source-mapping --region "$REGION" --uuid "$uuid" >/dev/null
done
for funcion in $LAMBDAS; do
  aws lambda delete-function --region "$REGION" --function-name "$funcion"
  aws logs delete-log-group --region "$REGION" --log-group-name "/aws/lambda/$funcion"
done

echo "== DynamoDB"
aws dynamodb delete-table --region "$REGION" --table-name reto3-estado >/dev/null

echo "== IAM"
aws iam detach-role-policy --role-name reto3-servicios-role \
  --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
aws iam delete-role --role-name reto3-servicios-role
for rol in reto3-sfn-role reto3-apigw-role reto3-auditor-role reto3-controlador-role; do
  aws iam delete-role-policy --role-name "$rol" --policy-name "$rol-inline"
  aws iam delete-role --role-name "$rol"
done

echo "Limpieza terminada en $REGION. Revisa arriba si algun comando reporto error."
