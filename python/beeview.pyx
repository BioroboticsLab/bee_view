from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

import numpy as np
from PIL import Image

# this could also go into a seperate .pxd file
cdef extern from "beeview_api.h" namespace "BeeView":

	cdef cppclass BeeViewApplication:
		BeeViewApplication(string, string, bool) except +

		#Methods
		vector[vector[vector[float]]] render()

		void setCameraPosition(float, float, float)
		void getCameraPosition(float &, float &, float &)
		void setCameraDirVector(float, float, float)
		void getCameraDirVector(float &, float &, float &)
		void setRenderModeBeeEye()
		void setRenderModePinhole()
		void setRenderModePanoramic()

cdef class API:
	cdef BeeViewApplication *C_Class
	"""
	Initializes the API class. Loads the scene and the ommatidia from the files. May take a while.
	IN: sceneFile: the absolute File Paths to the .obj file. 
	ommatidiaFile: the absolute filepath to the .csv file containing the elevation and azimuth angles of the ommatidia.
	"""
	def __cinit__(self, scene_file, ommatidia_file, left_handed = False):
		cdef string c_scene_file = scene_file.encode('UTF-8')
		cdef string c_ommatidia_file = ommatidia_file.encode('UTF-8')

		self.C_Class = new BeeViewApplication(c_scene_file,c_ommatidia_file,left_handed)

	def __dealloc__(self):
		del self.C_Class

	"""
	renders an image with the current camera.
	OUT: rendered PIL image
	"""
	def render(self):
		img = self.C_Class.render()
		img = (np.array(img) * 255).astype(np.uint8)
		return Image.fromarray(img)

	"""
	IN pos: list of 3 floats (x,y,z)
	"""
	def set_camera_position(self,pos):
		self.C_Class.setCameraPosition(pos[0],pos[1],pos[2]);

	"""
	OUT: list of x,y,z floats
	"""
	def get_camera_position(self):
		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getCameraPosition(x,y,z);
		return [x,y,z]

	"""
	IN: list of x,y,z floats
	"""
	def set_camera_dir(self,dir):
		self.C_Class.setCameraDirVector(dir[0],dir[1],dir[2]);
	
	"""
	OUT: list of x,y,z floats
	"""
	def get_camera_dir(self):
		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getCameraDirVector(x,y,z)
		return [x,y,z]

	def set_render_mode_beeeye(self):
		self.C_Class.setRenderModeBeeEye()

	def set_render_mode_pinhole(self):
		self.C_Class.setRenderModePinhole()

	def set_render_mode_panoramic(self):
		self.C_Class.setRenderModePanoramic()
