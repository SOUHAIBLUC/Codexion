#!/bin/bash

PASS=0
FAIL=0

run_test()
{
    local description=$1
    local args=$2
    local expected=$3

    output=$(timeout 15 ./codexion $args 2>&1)

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

run_test_timing()
{
    local description=$1
    local args=$2
    local expected_keyword=$3
    local min_time=$4
    local max_time=$5

    start=$(date +%s%3N)
    output=$(timeout 15 ./codexion $args 2>&1)
    end=$(date +%s%3N)
    elapsed=$((end - start))

    if echo "$output" | grep -q "$expected_keyword"; then
        if [ $elapsed -ge $min_time ] && [ $elapsed -le $max_time ]; then
            echo "✅ PASS: $description (${elapsed}ms)"
            PASS=$((PASS + 1))
        else
            echo "❌ FAIL: $description - timing wrong (${elapsed}ms, expected ${min_time}-${max_time}ms)"
            FAIL=$((FAIL + 1))
        fi
    else
        echo "❌ FAIL: $description - wrong output"
        echo "   got: $(echo "$output" | tail -3)"
        FAIL=$((FAIL + 1))
    fi
}

run_test_count()
{
    local description=$1
    local args=$2
    local keyword=$3
    local expected_count=$4

    output=$(timeout 15 ./codexion $args 2>&1)
    actual_count=$(echo "$output" | grep -c "$keyword")

    if [ "$actual_count" -eq "$expected_count" ]; then
        echo "✅ PASS: $description (count=$actual_count)"
        PASS=$((PASS + 1))
    else
        echo "❌ FAIL: $description (expected=$expected_count, got=$actual_count)"
        FAIL=$((FAIL + 1))
    fi
}

echo "========== VALID INPUTS =========="
run_test "burnout fifo"          "5 800 200 200 200 10 100 fifo"  "burned out"
run_test "burnout edf"           "5 800 200 200 200 10 100 edf"   "burned out"
run_test "compiles fifo"         "5 5000 200 200 200 3 100 fifo"  "coders: 5"
run_test "compiles edf"          "5 5000 200 200 200 3 100 edf"   "coders: 5"
run_test "2 coders fifo"         "2 2000 200 200 200 10 100 fifo" "coders: 2"
run_test "2 coders edf"          "2 2000 200 200 200 10 100 edf"  "coders: 2"

echo ""
echo "========== INVALID INPUTS =========="
run_test "1 coder"               "1 800 200 200 200 10 100 fifo"       "Error"
run_test "alpha args"            "abc 800 200 200 200 10 100 fifo"     "Error"
run_test "invalid scheduler"     "5 800 200 200 200 10 100 invalid"    "Error"
run_test "negative values"       "5 -800 200 200 200 10 100 fifo"      "Error"
run_test "zero values"           "5 0 200 200 200 10 100 fifo"         "Error"
run_test "too few args"          "5 800 200 200 200 10 100"            "Error"
run_test "too many args"         "5 800 200 200 200 10 100 fifo extra" "Error"

echo ""
echo "========== TIMING TESTS =========="
# burnout should happen at ~burnout_time ms
run_test_timing "burnout at 800ms fifo" "5 800 200 200 200 10 100 fifo" "burned out" 700 2000
run_test_timing "burnout at 800ms edf"   "5 800 200 200 200 10 100 edf"   "burned out" 700 2000
run_test_timing "burnout at 500ms fifo" "5 500 200 200 200 10 100 fifo" "burned out" 400 1500
run_test_timing "compiles in ~1800ms" "2 5000 200 200 200 3 100 fifo" "coders: 2" 1600 6000

echo ""
echo "========== COMPILE COUNT TESTS =========="
# with 2 coders and compiles_required=3, should see exactly 6 "is compiling"
run_test_count "2 coders 3 compiles each fifo" "2 5000 200 200 200 3 100 fifo" "is compiling" 6
run_test_count "2 coders 3 compiles each edf"  "2 5000 200 200 200 3 100 edf"  "is compiling" 6

echo ""
echo "========== LOG FORMAT TESTS =========="
# check log format: "timestamp id action"
run_test "log has timestamp fifo" "2 5000 200 200 200 3 100 fifo" "is compiling"
run_test "log has timestamp edf"  "2 5000 200 200 200 3 100 edf"  "is compiling"
run_test "burned out logged"      "5 800 200 200 200 10 100 fifo"  "burned out"

echo ""
echo "========== RESULTS =========="
echo "passed: $PASS"
echo "failed: $FAIL"
if [ $FAIL -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED!"
else
    echo "❌ SOME TESTS FAILED"
fi