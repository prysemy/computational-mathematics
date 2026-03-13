#!/bin/bash

echo "=== Building C++ Data Generator ==="

# Сборка C++ проекта
g++ -std=c++11 -O2 -o data_generator src/main.cpp -lm

echo "=== Generating Data ==="
./data_generator

echo "✅ Data generated: results.txt"