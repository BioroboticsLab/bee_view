
# bee_view

bee_view is a rendering engine based on raycasting that simulates insect vision.
* The renderer can load arbitrary scenes from .obj files. 
* Written in C++ and utilizing Intel’s high performance raytracing kernel library Embree, it is fast. 
* It is not limited to rendering bee views, but can also render normal images from a pinhole or a panoramic camera. 
* It has a simple API so it can easily be interfaced with from other applications. 
* bee_view also includes a Python package that provides bindings to the C++ functions. 
* The renderer is implemented in a way that all the settings of the renderer are flexible. This means that the renderer can also be used with different interommatidial and acceptance angles for simulating the vision of other insects.

The bee's eye model implemented is a model of the spatial resolution and field of view. It does not include the spectral sensitivity and light intensity.
The Model is based on the paper: 
W. Stürzl, N. Boeddeker, L. Dittmar, and M. Egelhaaf, ‘Mimicking honeybee eyes with a 280 degrees field of view catadioptric imaging system’, Bioinspir. Biomim., vol. 5, no. 3, p. 036002, Sep. 2010.

The renderer was built as part of a bachelor's Thesis. Implementation details and other information can be found here: [TODO link to BA]

TODO: Example output as image, Example video
TODO: link to doc: Make doc compatible with doxygen, host the doc

# beeview Python package

The Python package needs Python >3.5 64 bit.

## Installation

Dependencies:
* numpy
* Cython (only needed if building from source)

### Windows
A precompiled .pyd file for Windows 10 64 bit can be found TODO here in the folder api\build. Either place the .pyd file and the .dlls in the same folder as your python script or add them to your PATH variable. Then simply use `import beeview` to use the package.

### Linux
TODO

## Building from source
For building and installing the python package from source run:
```
python setup.py install
```
in the api folder of bee_view.


## Usage
Example Python scripts and Notebooks can be found TODO here: api\examples

# C++ Library

## Installation
### Windows 
Building with Visual Studio on Windows:

* clone the repository
* add the "external" folder to Project Settings -> VC++ Directories -> Include Directories
* add the "lib" folder to Project Settings -> VC++ Directories -> Library Directories
* add "embree.lib" to Project Settings -> Linker -> Input -> Additional Dependencies
* add embree.dll, tbb.dll and tbbmalloc.dll (they can be found in the "lib" folder) to your PATH variable (e.g. by putting it in C:\\Windows\System32), or put them in the same folder as your application.
* Include the beeview_api.h file (located in the api folder)

Then build for x64. 

# Credits
The renderer uses the following libraries:
* Embree library embree.github.io
* Eigen library: http://eigen.tuxfamily.org/
The bee eye model is based on:
* W. Stürzl, N. Boeddeker, L. Dittmar, and M. Egelhaaf, ‘Mimicking honeybee eyes with a 280 degrees field of view catadioptric imaging system’, Bioinspir. Biomim., vol. 5, no. 3, p. 036002, Sep. 2010.
  https://pub.uni-bielefeld.de/download/1930189/2516828
The 3D model was recorded from the FU Biorotobotics lab  http://berlinbiorobotics.blog/. 
The location of the model: https://www.google.de/maps/place/50%C2%B048'55.0%22N+8%C2%B052'41.6%22E/@50.815272,8.8760423,17z/data=!3m1!4b1!4m5!3m4!1s0x0:0x0!8m2!3d50.815272!4d8.878231

# License
TODO: change license to mpl


##############


TODO: 
setup.py detect os, change linker objects depending on os
make a make file that can do: make python, or make install for c++
cleanup python doc mess
make new docs for c++ and python
examples for python and c++
