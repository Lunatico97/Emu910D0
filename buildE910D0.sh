#!/bin/bash

CODE="e910D0"
rm "bin/$CODE"
g++ src/*.cpp "-Iinclude/" -o "bin/$CODE" -lSDL2main -lSDL2
echo $CODE
./bin/$CODE
