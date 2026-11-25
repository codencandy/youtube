#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit'
IGNORE='-Wno-nullability-completeness -Wno-int-to-pointer-cast'
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INCLUDES="-I$PROJECT_DIR -I$PROJECT_DIR/engine -I$PROJECT_DIR/platform"
FLAGS='--debug'
TIMEFORMAT=%R

demo ()
{
    echo "build textdemo app"
    clang++ -dynamiclib -o bin/textdemo.dylib CNC_TextDemo.cpp $INCLUDES ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

platform ()
{
    echo "build platform"
    clang++ platform/macos/CNC_Main.mm -o demo $INCLUDES ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

main ()
{
    rm -rf bin/textdemo.dylib
    time platform
    time demo

    LINES_OF_CODE=$(cloc . --exclude-list-file=clocignore | grep -o -E '([0-9]+)' | tail -n 1)
    echo "lines of code:" $LINES_OF_CODE
}

time main
