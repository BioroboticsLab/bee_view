from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension

src_folder = '../src/'

sourcefiles  = [
    'bee_view.pyx', 
    'bee_view_api.cpp',
    src_folder + 'bee_eye.cpp',
    src_folder + 'camera.cpp',
    src_folder + 'image.cpp',
    src_folder + 'obj_loader.cpp',
    src_folder + 'renderer.cpp',
    src_folder + 'sampler.cpp',
    src_folder + 'scene.cpp',
    src_folder + 'stdfx.cpp',
    src_folder + 'texture.cpp'
    ]

ext=[Extension('bee_view',
            sources = sourcefiles,
            libraries=["tbb","tbbmalloc","embree"],  
            extra_link_args=["-L../lib/"],
            extra_compile_args=["-I../external/"],
            language='c++')]

setup(
  ext_modules=cythonize(ext)
)