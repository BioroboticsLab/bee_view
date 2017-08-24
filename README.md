
# bee_view

bee_view is a renderer based on raycasting, that simulates insect vision.
* The renderer can load arbitrary scenes from .obj files. 
* Written in C++, utilizing Intel’s high performance raytracing kernel library Embree, it is fast. 
* It is not limited to rendering bee views, but can also render normal images from a pinhole or a panoramic camera. 
* It has a simple API so it can easily be interfaced with from other applications. 
* bee_view also includes a python package that provides bindings to the C++ functions. 
* The renderer is implemented in a way, that all the settings of the renderer are flexible. This means that the renderer can also be used with different interommatidial and acceptance angles for simulating the vision of other insects.

TODO: Model based on stürzl et al
TODO: Example output as image
TODO: link to doc: Make doc compatible with doxygen, host the doc

## Installation
The renderer depends on Embree. Precompiled Embree shared libraries can be found in the lib folder.

### beeview Python package
Dependencies:

* Cython
* numpy

For building the python package run:
```
python setup.py install
```
in the python folder of bee_view.

TODO: add to path, precompiled binaries, where put embree library, Linux / Windows
TODO: make file for c++. (also make python?)

## Usage
TODO: Write usage instructions
TODO: Example folder with ipynb examples of beeview api, example c++ programm

## Credits
TODO: Write credits
* Eigen library, Embree library, Sürzl model, Biorobotics 3D model

## License
TODO: Write license