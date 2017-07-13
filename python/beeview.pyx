from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

import numpy as np
from PIL import Image

# this should go into a seperate .pxd file
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
		float heightAboveGround()

cdef class Renderer:
	"""This class wraps the C++ BeeViewApplication class.

	It provides all the functions necessary for rendering Images from a loaded scene.

	"""

	cdef BeeViewApplication *C_Class
	
	def __cinit__(self, scene_file, ommatidia_file, left_handed = False):
		"""Loads the scene and the ommatidia from the files.

		May take a while.

		Parameters
		----------
		scene_file : str
			The absolute File Path to the .obj file. 
		ommatidia_file : str
			The absolute filepath to the .csv file containing the elevation and azimuth angles of the ommatidia.
		left_handed : bool
			Wether to load the scene as right handed or left handed coordinatesystem.

		"""

		cdef string c_scene_file = scene_file.encode('UTF-8')
		cdef string c_ommatidia_file = ommatidia_file.encode('UTF-8')

		self.C_Class = new BeeViewApplication(c_scene_file,c_ommatidia_file,left_handed)

	def __dealloc__(self):
		"""Cleans up the C++ Class. """

		del self.C_Class

	
	def render(self):
		"""Renders an image with the current camera.

		Returns
		-------
		PIL.Image object
			The rendered PIL image.

		"""

		img = self.C_Class.render()
		img = (np.array(img) * 255).astype(np.uint8)
		return Image.fromarray(img)

	def set_camera_position(self,pos):
		"""	
		Parameters
		----------
		pos : sequence of float
			The x, y, z coordinates
		"""
		self.C_Class.setCameraPosition(pos[0],pos[1],pos[2]);

	def get_camera_position(self):
		"""
		Returns
		-------
		list of float
			The x, y, z coordinates.

		"""

		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getCameraPosition(x,y,z);
		return [x,y,z]

	def set_camera_dir(self,dir):
		"""
		Parameters
		----------
		dir : list of float
			The x, y, z coordinates of dir Vector. Will be normalized by the function.

		"""

		self.C_Class.setCameraDirVector(dir[0],dir[1],dir[2]);

	def get_camera_dir(self):
		"""
		Returns
		-------
		list of float
			The x, y, z coordinates of dir Vector.

		"""

		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getCameraDirVector(x,y,z)
		return [x,y,z]

	def set_beeeye_mode(self):
		"""Sets the renderer to use beeEye camera for rendering. """

		self.C_Class.setRenderModeBeeEye()

	def set_pinhole_mode(self):
		"""Sets the renderer to use pinhole camera for rendering. 

		Max FOV is 89 degrees. Use panoramic camera for wider angles.
		
		"""

		self.C_Class.setRenderModePinhole()

	def set_panoramic_mode(self):
		"""Sets the renderer to use panoramic camera for rendering. """

		self.C_Class.setRenderModePanoramic()

	def height_above_ground(self):
		"""
		Returns
		-------
		float
			The height of the camera above ground. If not above ground return -1.

		"""

		return self.C_Class.heightAboveGround()

