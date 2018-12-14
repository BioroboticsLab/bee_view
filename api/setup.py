from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension
import os
import platform	

project_path =  os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

sourcefiles  = [
    os.path.join(project_path,'api','beeview.pyx'), 
    os.path.join(project_path,'api','beeview_api.cpp'),
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

	
if platform.system() == 'Linux':
    link_args = ["-L"+ os.path.join(project_path,'lib'),"-Wl,-rpath=/usr/lib/","-l:libtbbmalloc.so.2","-l:libtbb.so.2","-l:libembree.so.2"]
    comp_args = ["-std=c++17", "-I" + os.path.join(project_path,'external'), "-O3"]
    macros = []
else:
    link_args = ["/LIBPATH:"+ os.path.join(project_path,'lib')]
    comp_args=["-I" + os.path.join(project_path,'external')]
    macros=[('_ENABLE_EXTENDED_ALIGNED_STORAGE', None)]
	
ext=[Extension('beeview',
            sources = sourcefiles,
            libraries=["embree"],  
            extra_link_args=link_args,
            extra_compile_args=comp_args,
			define_macros=macros,
            language='c++')]

setup(
  name = 'beeview',
  version = '1.0',
  author = 'Johannes Polster',
  url = 'https://github.com/tschopo/bee_view',
  description = 'Bee Eye Camera for Virtual Environments',
  ext_modules=cythonize(ext),
  py_modules = ['beeview']
)