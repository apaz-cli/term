#!/bin/sh

# Build SDL program
gcc term.c -o term -lSDL2 -lSDL2_ttf -lSDL2_image
./term
