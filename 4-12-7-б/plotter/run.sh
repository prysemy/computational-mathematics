#!/bin/bash

rm -f *.png
echo "=== Generating Plots ==="
dotnet run

echo "✅ Plots generated"