#!/bin/bash

PASS=0
FAIL=0

run_test()
{
    local description=$1
    local args=$2
    local expected=$3  # "burnout", "finished", "error"
    
    output=$(./codexion $args 2>&1)
    
    if echo "$output" | grep -q "$expected"; then
        echo "✅ PASS: $description"
        PASS=$((PASS + 1))
    else
        echo "❌ FAIL: $description"
        echo "   args: $args"
        echo "   expected: $expected"
        echo "   got: $(echo "$output" | tail -3)"
        FAIL=$((FAIL + 1))
    fi
}

echo "========== VALID INPUTS =========="
run_test "burnout fifo"     "5 800 200 200 200 10 100 fifo"   "burned out"
run_test "burnout edf"      "5 800 200 200 200 10 100 edf"    "burned out"
run_test "compiles fifo"    "5 5000 200 200 200 3 100 fifo"   "coders: 5"
run_test "compiles edf"     "5 5000 200 200 200 3 100 edf"    "coders: 5"
run_test "2 coders fifo"    "2 2000 200 200 200 10 100 fifo"  "coders: 2"
run_test "2 coders edf"     "2 2000 200 200 200 10 100 edf"   "coders: 2"

echo ""
echo "========== INVALID INPUTS =========="
run_test "1 coder"          "1 800 200 200 200 10 100 fifo"   "Error"
run_test "alpha args"       "abc 800 200 200 200 10 100 fifo" "Error"
run_test "invalid scheduler""5 800 200 200 200 10 100 invalid" "Error"
run_test "negative values"  "5 -800 200 200 200 10 100 fifo"  "Error"
run_test "zero values"      "5 0 200 200 200 10 100 fifo"     "Error"
run_test "too few args"     "5 800 200 200 200 10 100"         "Error"
run_test "too many args"    "5 800 200 200 200 10 100 fifo extra" "Error"

echo ""
echo "========== RESULTS =========="
echo "passed: $PASS"
echo "failed: $FAIL"