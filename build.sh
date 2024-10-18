#! /bin/bash

FRAMEWORKS='-framework AppKit'

clang++ CNC_Main.mm -o window ${FRAMEWORKS}