#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit'
IGNORE='-Wno-nullability-completeness'
FLAGS='-D UNITY_BUILD'
TIMEFORMAT=%R
BUILD_TYPE=$1

main ()
{
    if [ "$BUILD_TYPE" == "STANDARD" ]
    then
    echo "standard build"
    clang++ CNC_Main.mm CNC_Window.mm CNC_Renderer.mm -o window ${FRAMEWORKS} ${IGNORE}
    else
    echo "unity build"
    clang++ CNC_Main.mm -o window ${FRAMEWORKS} ${IGNORE} ${FLAGS}
    fi
}

time main