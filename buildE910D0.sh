#!/bin/bash

CODE="e910D0"
rm "bin/$CODE"
g++ include/imgui/*.cpp src/*.cpp -ffast-math -O2 "-Iinclude/" -o "bin/$CODE" -lSDL2main -lSDL2
echo $CODE
./bin/$CODE $1
