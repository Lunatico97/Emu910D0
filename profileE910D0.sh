#!/bin/bash

g++ include/imgui/*.cpp src/*.cpp -pg -ffast-math -O2 "-Iinclude/" -o ./bin/pf-emu910d0 -lSDL2main -lSDL2
./bin/pf-emu910d0 $1
gprof ./bin/pf-emu910d0 gmon.out > ./bin/profile.txt
rm ./bin/pf-emu910d0
rm gmon.out
cat ./bin/profile.txt