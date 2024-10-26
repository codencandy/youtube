#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit'
IGNORE='-Wno-nullability-completeness'

clang++ CNC_Main.mm -o window ${FRAMEWORKS} ${IGNORE}