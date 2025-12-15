#!/bin/bash

g++ src/*.cpp -pg -ffast-math "-Iinclude/" -o ./bin/pf-emu910d0 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
./bin/pf-emu910d0 $1
gprof ./bin/pf-emu910d0 gmon.out > ./bin/profile.txt
rm ./bin/pf-emu910d0
rm gmon.out
cat ./bin/profile.txt