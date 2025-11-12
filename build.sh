#! /bin/bash

FRAMEWORKS='-framework AppKit -framework CoreVideo -framework Metal -framework MetalKit -framework GameController'
IGNORE='-Wno-nullability-completeness -Wno-writable-strings'
FLAGS='--debug -std=c++20 -Ilibs -Ilibs/imgui -Iplatform -Iplatform/.. -Iengine -Iengine/..'
TIMEFORMAT=%R
BUILD_TYPE=$1

imgui ()
{
    echo "build imgui"
    clang++ -c platform/CNC_ImGui.mm ${FLAGS} ${IGNORE} -o bin/CNC_ImGui.o
}

pong ()
{
    echo "build pong"
    clang++ -dynamiclib -o bin/pong.dylib CNC_Pong.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

shape ()
{
    echo "build shape demo"
    clang++ -dynamiclib -o bin/shape.dylib CNC_ShapeDemo.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

christmas ()
{
    echo "build christmas app"
    clang++ -dynamiclib -o bin/christmas.dylib CNC_Christmas.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

clock ()
{
    echo "build clock app"
    clang++ -dynamiclib -o bin/clock.dylib CNC_ClockApp.cpp ${FRAMEWORKS} ${IGNORE} ${FLAGS}
}

platform ()
{
    echo "build platform"
    clang++ platform/CNC_Main.mm -o demo bin/CNC_ImGui.o ${FRAMEWORKS} ${IGNORE}  ${FLAGS}
}

main ()
{
    if [ "$BUILD_TYPE" == "G" ]
    then
        time imgui
    elif [ "$BUILD_TYPE" == "P" ]
    then
        time platform
    else
        time christmas
        time clock
        time shape
        time pong
    fi

    LINES_OF_CODE=$(cloc . --exclude-list-file=clocignore | grep -o -E '([0-9]+)' | tail -n 1)
    echo "lines of code:" $LINES_OF_CODE
}

time main
