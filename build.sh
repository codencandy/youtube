#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit -framework GameController'
IGNORE='-Wno-nullability-completeness'
FLAGS='-O2 -std=c++20 -Ilibs -Ilibs/imgui'
TIMEFORMAT=%R
BUILD_TYPE=$1

imgui ()
{
    echo "build imgui"
    clang++ -c CNC_ImGui.mm ${FLAGS} ${IGNORE} -o bin/CNC_ImGui.o
}

christmas ()
{
    echo "build christmas app"
    clang++ -dynamiclib -o bin/christmas.dylib CNC_Christmas.cpp -nostdlib -nodefaultlibs ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

clock ()
{
    echo "build clock app"
    clang++ -dynamiclib -o bin/clock.dylib CNC_ClockApp.cpp -nostdlib -nodefaultlibs ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

platform ()
{
    echo "build platform"
    clang++ CNC_Main.mm -o demo bin/CNC_ImGui.o ${FRAMEWORKS} ${IGNORE}  ${FLAGS}
}

main ()
{
    if [ "$BUILD_TYPE" == "G" ]
    then
        time imgui
    else
        time platform
        time christmas
        time clock
    fi

    LINES_OF_CODE=$(cloc . --exclude-list-file=clocignore | grep -o -E '([0-9]+)' | tail -n 1)
    echo "lines of code:" $LINES_OF_CODE
}

time main
