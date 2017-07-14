import numpy as np
from PIL import Image
from  beeview_api cimport *

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

	def rotate_camera_up(self, degrees):
		self.C_Class.rotateCameraUp(degrees)

	def rotate_camera_down(self, degrees):
		self.C_Class.rotateCameraDown(degrees)

	def rotate_camera_left(self, degrees):
		self.C_Class.rotateCameraLeft(degrees)

	def rotate_camera_right(self, degrees):
		self.C_Class.rotateCameraRight(degrees)

	def roll_camera_left(self, degrees):
		self.C_Class.rollCameraLeft(degrees)

	def roll_camera_right(self, degrees):
		self.C_Class.rollCameraRight(degrees)

	def height_above_ground(self):
		"""
		Returns
		-------
		float
			The height of the camera above ground. If not above ground return -1.

		"""

		return self.C_Class.heightAboveGround()

	def set_panoramic_xfov(self, xfov):
		self.C_Class.setPanoramicCameraXfov(xfov)

	def set_panoramic_yfov(self, yfov):
		self.C_Class.setPanoramicCameraYfov(yfov)

	def set_panoramic_width(self, width):
		self.C_Class.setPanoramicCameraWidth(width)

	def set_pinhole_fov(self, fov):
		self.C_Class.setPinholeCameraFov(fov)

	def set_pinhole_width(self, width):
		self.C_Class.setPinholeCameraWidth(width)

	def set_pinhole_height(self, height):
		self.C_Class.setPinholeCameraHeight(height)

	def set_beeeye_acceptance_angle(self, acceptance_angle):
		self.C_Class.setPinholeCameraHeight(acceptance_angle)

	def set_beeeye_sample_points(self, num_sample_points):
		self.C_Class.setBeeEyeCameraNumSamplePoints(num_sample_points)

	def set_beeeye_ommatidium_size(self, ommatidium_size):
		self.C_Class.setBeeEyeCameraOmmatidiumSize(ommatidium_size)

	def set_verbose_lvl(self, verbose_lvl):
		self.C_Class.setVerboseLvl(verbose_lvl)
