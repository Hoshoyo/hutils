#!/bin/bash

count=10

# Addition benchmark for Go
echo "Starting Go addition benchmark..."
for ((i=1;i<=count;i++)); do
    bin/go_benchmark sum | tee -a test/go_addition_result.txt
done

# Subtraction benchmark for Go
echo "Starting Go subtraction benchmark..."
for ((i=1;i<=count;i++)); do
    bin/go_benchmark sub | tee -a test/go_subtraction_result.txt
done

# Addition benchmark for C
echo "Starting C addition benchmark..."
for ((i=1;i<=count;i++)); do
    bin/c_benchmark sum | tee -a test/c_addition_result.txt
done

# Subtraction benchmark for C
echo "Starting C subtraction benchmark..."
for ((i=1;i<=count;i++)); do
    bin/c_benchmark sub | tee -a test/c_subtraction_result.txt
done