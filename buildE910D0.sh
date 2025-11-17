#!/bin/bash

CODE="e910D0"
rm "bin/$CODE"
g++ src/*.cpp -ffast-math "-Iinclude/" -o "bin/$CODE" -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
echo $CODE
./bin/$CODE
