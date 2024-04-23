#!/bin/bash

OUTPUT="bench.out"

# Function to extract wall clock time from time command output
extract_wall_clock_time() {
    grep 'Elapsed (wall clock)'
}

# Execute tasks with different parameters and redirect output to "bench.out"
task2_cmd="./task2/task2"
task3_cmd="./task3/task3"

echo "$task2_cmd 250 100000" >> "$OUTPUT"
/usr/bin/time -v "$task2_cmd" 250 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "$task3_cmd 250 100000" >> "$OUTPUT"
/usr/bin/time -v "$task3_cmd" 250 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "$task2_cmd 500 100000" >> "$OUTPUT"
/usr/bin/time -v "$task2_cmd" 500 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "$task3_cmd 500 100000" >> "$OUTPUT"
/usr/bin/time -v "$task3_cmd" 500 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "$task2_cmd 1000 100000" >> "$OUTPUT"
/usr/bin/time -v "$task2_cmd" 1000 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "$task3_cmd 1000 100000" >> "$OUTPUT"
/usr/bin/time -v "$task3_cmd" 1000 100000 2>&1 | extract_wall_clock_time >> "$OUTPUT"
