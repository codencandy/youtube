## codencandy computer graphics series

This repository contains the code shown in my graphics programming videos for MacOS and Windows on my YouTube channel [codencandy](https://www.youtube.com/@codencandy)

## How to build

Since the project contains 3 distinct parts you need to understand which build switches to use to build
- the platform executable
- the ImGui part
- the individual application plugins

The build script ```sh build.sh``` is the same for all 3.
- ```sh build.sh P``` builds the platform
- ```sh build.sh G``` builds ImGui
- ```sh build.sh``` without any switches buildls all the current plugins

The executable that gets build is ```demo```. In order to start that execute ```./demo``` in the terminal.

## Dependencies (MacOS)

- XCode commandline tools
```xcode-select --install```
- [cloc](https://formulae.brew.sh/formula/cloc)
- [Sean T. Berret header only libraries](https://github.com/nothings/stb)

## External links or documents

- [Edwin Catmull 1974 Thesis: A subdivision  for computer display of curved surfaces](https://ohiostate.pressbooks.pub/app/uploads/sites/45/2017/09/catmull_thesis.pdf)

## YouTube videos

|   Title    | YouTube       |  git tag     |       |
|  ---  |  ---  |  ---  |  ---  |
|   Simple MacOS window     |  [open a window](https://youtu.be/Q2Mfc369QtI)                   | Simple-MacOS-window      |       |
|   Constant frame rate     |  [application with 60 FPS](https://youtu.be/-3rmEsQ_3Wg)         | Constant-frame-rate      |       |
|   GPU based rendering     |  [setting up Metal](https://youtu.be/UrHKvAPFxOI)                | GPU-based-rendering      |       |
|   Speed up you build time |  [unity builds](https://youtu.be/49kQwhnBlHs)                    | unity-vs-standard-build  |       |
|   Metal shader pipeline   |  [shader pipeline](https://www.youtube.com/watch?v=9iE76DMMlvI)  | metal-shader-pipelin     |       |
|   Metal texture mapping   |  [render images](https://www.youtube.com/watch?v=dxJziwIALBk)    | metal-texture-mapping    |       |
|   Analaog Clock Part 1    |  [part #1](https://www.youtube.com/watch?v=dS3natV-HzQ)          | analog-clock-part1       |       |
|   Analaog Clock Part 2    |  [part #2](https://youtu.be/rscUEhVD3sQ)                         | analog-clock-part2       |       |
|   Analaog Clock Part 3    |  [part #3](https://youtu.be/FKXx3EizUyU)                         | instanced-rendering      |       |
|   Snow Particle System    |  [snow particles](https://youtu.be/jeTSjqUOKJ4)                  | christmas-particles      |       |
|   Load Code At Runtime    |  [load code](https://)                                           | load-code-dynamically    |       |


