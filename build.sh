#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit'
IGNORE='-Wno-nullability-completeness'
FLAGS='--debug'
TIMEFORMAT=%R

textdemo ()
{
    echo "build textdemo app"
    clang++ -dynamiclib -o textdemo.dylib CNC_TextDemo.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

christmas ()
{
    echo "build christmas app"
    clang++ -dynamiclib -o christmas.dylib CNC_Christmas.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

clock ()
{
    echo "build clock app"
    clang++ -dynamiclib -o clock.dylib CNC_ClockApp.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

platform ()
{
    echo "build platform"
    clang++ CNC_Main.mm -o demo ${FRAMEWORKS} ${IGNORE}  ${FLAGS}
}

main ()
{
    time platform
    time christmas
    time textdemo
    time clock

    LINES_OF_CODE=$(cloc . --exclude-list-file=clocignore | grep -o -E '([0-9]+)' | tail -n 1)
    echo "lines of code:" $LINES_OF_CODE
}

time main
