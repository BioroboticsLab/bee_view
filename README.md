
# bee_view

bee_view is a rendering engine based on raycasting that simulates insect vision.
Features:
* The renderer can load arbitrary scenes from .obj files. 
* Written in C++ and utilizing Intel’s high performance raytracing kernel library Embree, it is fast. 
* It is not limited to rendering bee views, but can also render normal images from a pinhole or a panoramic camera. 
* It has a simple API so it can easily be interfaced with from other applications. 
* bee_view also includes a Python package that provides bindings to the C++ functions. 
* The renderer is implemented in a way that all the settings of the renderer are flexible. This means that the renderer can also be used with different interommatidial and acceptance angles for simulating the vision of other insects.

The bee's eye model implemented is a model of the spatial resolution and field of view. It does not include the spectral sensitivity and light intensity.

The Model is based on the paper: 
>W. Stürzl, N. Boeddeker, L. Dittmar, and M. Egelhaaf, ‘Mimicking honeybee eyes with a 280 degrees field of view catadioptric imaging system’, Bioinspir. Biomim., vol. 5, no. 3, p. 036002, Sep. 2010.

The script for rendering the interommatidial angles can be found [here](data/calc_ommatidial_array.R).
The renderer was built as part of a bachelor's thesis. Implementation details and other information can be found in the [thesis](data/bachelorarbeit_polster_final_corrected_web.pdf).

![An example rendering](data/test_beeEye_s21_a26.png?raw=true "Example rendering")


An example video that was rendered with the engine can be found [here](https://www.youtube.com/watch?v=xc-4FWyoQM4).

# beeview Python package

The Python package needs Python >3.5 64 bit.

## Installation

Dependencies:
* numpy
* Cython (only needed if building from source)

### Windows
A precompiled .pyd file for Windows 10 64 bit can be found [here](api/build/lib.win-amd64-3.6/beeview.cp36-win_amd64.pyd). Either place the .pyd file and the .dlls in the same folder as your python script or add them to your PATH variable. Then simply use `import beeview` to use the package.

### Linux
TODO

## Building from source
For building and installing the python package with Cython from source run:
```
python setup.py install
```
in the api folder of bee_view.


## Usage
Example Python scripts and Notebooks can be found [here](api/examples).

# C++ Library

## Installation
### Windows 
Building with Visual Studio on Windows:

* clone the repository
* add the "external" folder to Project Settings -> VC++ Directories -> Include Directories
* add the "lib" folder to Project Settings -> VC++ Directories -> Library Directories
* add "embree.lib" to Project Settings -> Linker -> Input -> Additional Dependencies
* add embree.dll, tbb.dll and tbbmalloc.dll (they can be found in the "lib" folder) to your PATH variable, or put them in the same folder as your application.

Then build for x64.

To include bee_view with your project, you can build it as shared library and include the [beeview_api.h](api/beeview_api.h) file.


# Credits
The renderer uses the following libraries:
* [Embree library](http://embree.github.io)
* [Eigen library](http://eigen.tuxfamily.org)

The bee eye model is based on:
* [W. Stürzl, N. Boeddeker, L. Dittmar, and M. Egelhaaf, ‘Mimicking honeybee eyes with a 280 degrees field of view catadioptric imaging system’, Bioinspir. Biomim., vol. 5, no. 3, p. 036002, Sep. 2010.](https://pub.uni-bielefeld.de/download/1930189/2516828)

The 3D model was recorded from the [FU Biorotobotics lab](http://berlinbiorobotics.blog/).
The location of the model on Google maps: [link](https://www.google.de/maps/place/50%C2%B048'55.0%22N+8%C2%B052'41.6%22E/@50.8152778,8.8432033,8382m/data=!3m1!1e3!4m5!3m4!1s0x0:0x0!8m2!3d50.815272!4d8.878231)

##############

# TODO: 
- setup.py detect os, change linker objects depending on os
- make a make file that can do: make python, or make install for c++
- bluild for linux
- generate doc with pyrex
- make c++ doc compatible with doxygen
- examples for c++
