#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo'

clang++ CNC_Main.mm -o window ${FRAMEWORKS}