#!/bin/bash

echo "Building program..."
make -B all
echo "Running resulting program..."
./main
echo "Cleaning up..."
make clean