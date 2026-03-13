#!/bin/bash

echo "=== Starting Data Analysis Pipeline ==="

echo "0. Cleaning old plots..."
rm -f /home/emily/CLionProjects/plotter/*.png

echo "1. Running C++ data generator..."
cd data_generator/
./run.sh

echo "2. Copying data to plotter..."
cp results.txt ../plotter/data/

# 4. Запускаем C# построение графиков
echo "3. Running C# plotter..."
cd ../plotter
./run.sh

echo "=== Pipeline complete ==="
