#!/bin/bash

make ring || { echo "❌ Falló la compilación"; exit 1; }

echo "🧪 Iniciando tests..."

TEMP_OUT=$(mktemp)

run_test() {
    local n="$1"
    local c="$2"
    local s="$3"
    local expected="$4"
    local label="$5"

    echo -e "\n🔸 Test: $label"
    ./ring "$n" "$c" "$s" > "$TEMP_OUT"

    if grep -q "Resultado final recibido en el padre: $expected" "$TEMP_OUT"; then

        echo "✅ OK - Resultado esperado: $expected"
    else
        echo "❌ ERROR - Se esperaba: $expected"
        echo "   Salida obtenida:"
        cat "$TEMP_OUT"
    fi
}

run_test 3 10 0 13 "3 procesos, inicio en 0"
run_test 4 20 2 24 "4 procesos, inicio en 2"
run_test 5 100 2 105 "5 procesos, inicio en 2"
run_test 6 0 5 6 "6 procesos, inicio en 5"
run_test 10 1 0 11 "10 procesos, inicio en 0"

rm "$TEMP_OUT"
