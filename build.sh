#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit'
IGNORE='-Wno-nullability-completeness'
TIMEFORMAT=%R

main ()
{
    echo "unity build"
    clang++ CNC_Main.mm -o christmas ${FRAMEWORKS} ${IGNORE}
    LINES_OF_CODE=$(cloc . --exclude-list-file=clocignore | grep -o -E '([0-9]+)' | tail -n 1)
    echo "lines of code:" $LINES_OF_CODE
}

time main