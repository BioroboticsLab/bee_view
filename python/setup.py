from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import os

project_path =  os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

sourcefiles  = [
    os.path.join(project_path,'python','beeview.pyx'), 
    os.path.join(project_path,'python','beeview_api.cpp'),
    os.path.join(project_path,'src','bee_eye.cpp'),
    os.path.join(project_path,'src','camera.cpp'),
    os.path.join(project_path,'src','image.cpp'),
    os.path.join(project_path,'src','obj_loader.cpp'),
    os.path.join(project_path,'src','renderer.cpp'),
    os.path.join(project_path,'src','sampler.cpp'),
    os.path.join(project_path,'src','scene.cpp'),
    os.path.join(project_path,'src','stdafx.cpp'),
    os.path.join(project_path,'src','texture.cpp')
    ]

ext=[Extension('beeview',
            sources = sourcefiles,
            libraries=["embree"],  
            extra_link_args=["/LIBPATH:"+ os.path.join(project_path,'lib')],
            extra_compile_args=["-I" + os.path.join(project_path,'external')],
            language='c++')]

setup(
  ext_modules=cythonize(ext)
)