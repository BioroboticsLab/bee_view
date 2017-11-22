import numpy as np
import os
from  beeview_api cimport *

cdef class Renderer:
	"""This class wraps the C++ BeeViewApplication class.

	It provides all the functions necessary for rendering Images from a loaded scene.

	Properties
	----------
	mode : int
		Current render mode. 0 : Renderer.BeeEye, 1 : Renderer.Panoramic, 2 : Renderer.Pinhole
	position : sequence of floats
		x,y,z coordinates
	direction : sequence of floats
		x,y,z coordinate. Auto normalized.

	"""
	
	""" The render Mode """
	BeeEye, Panoramic, Pinhole = range(3)

	cdef BeeViewApplication *C_Class
	
	def __cinit__(self, scene_file, ommatidia_file, left_handed = False):
		"""Loads the scene and the ommatidia from the files.

		May take a while.

		Parameters
		----------
		scene_file : str
			The File Path to the .obj file. 
		ommatidia_file : str
			The filepath to the .csv file containing the elevation and azimuth angles of the ommatidia.
		left_handed : bool
			Wether to load the scene as right handed or left handed coordinatesystem.

		"""

		scene_file = os.path.abspath(scene_file)
		ommatidia_file = os.path.abspath(ommatidia_file)

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
		Array of shape: [height, width, 3]. Color data is uint8 with range 0-255.	

		"""

		img = self.C_Class.render()
		img = (np.array(img) * 255).astype(np.uint8)
		return img
		#return Image.fromarray(img)
	
	def render_agent(self):
		"""Renders a flat array of image data.

		Returns
		-------
		If BeeEye mode: 
			Tuple of 2 dictionaries (one for left eye, one for right eye) with keys: “azimuth”, “elevation” and “color”.
			Azimuth contains a list of 	azimuth angles, Elevation contains list of elevation angles 
			and color a list of lists containing the color values for each ommatidium.
		If Panoramic or Pinhole mode: 
			Array of shape [width*height,3] containing the color data
		"""

		# if beeeye mode
		if self.mode == 0:
			
			r = self.C_Class.renderAgent()
			left_color = np.transpose(np.array([r[2],r[3],r[4]]))
			right_color = np.transpose(np.array([r[7],r[8],r[9]]))
			left = {"azimuth":r[0],"elevation":r[1],"color":left_color}
			right = {"azimuth":r[5],"elevation":r[6],"color":right_color}
			return (left,right)
		else:
			img = self.C_Class.render();

			# make img flat
			return np.array([color for row in img for color in row])

	def set_position(self,pos):
		"""	
		Parameters
		----------
		pos : sequence of float
			The x, y, z coordinates
		"""
		self.C_Class.setPosition(pos[0],pos[1],pos[2]);

	def get_position(self):
		"""
		Returns
		-------
		list of float
			The x, y, z coordinates.

		"""

		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getPosition(x,y,z);
		return [x,y,z]

	position = property(get_position, set_position)

	def set_direction(self,dir):
		"""
		Parameters
		----------
		dir : list of float
			The x, y, z coordinates of dir Vector. Will be normalized by the function.

		"""

		self.C_Class.setDirection(dir[0],dir[1],dir[2]);

	def get_direction(self):
		"""
		Returns
		-------
		list of float
			The x, y, z coordinates of dir Vector.

		"""

		cdef float x = 0.0, y = 0.0, z = 0.0
		self.C_Class.getDirection(x,y,z)
		return [x,y,z]

	direction = property(get_direction, set_direction)

	def set_mode(self, mode):
		"""Sets the camera mode for rendering. 
		
		Parameters
		----------
		mode : int
			Camera mode. Options are Renderer.BeeEye (0), Renderer.Panoramic (1) or Renderer.Pinhole (2)
			BeeEye: Renders the view from a Bee.
			Panoramic: Renders a Panoramic Image.
			Pinhole: Max FOV is 89 degrees. Use panoramic camera for wider angles.

		"""

		self.C_Class.setMode(mode)

	def get_mode(self):
		return self.C_Class.getMode()

	mode = property(get_mode, set_mode)

	def rotate_up(self, degrees):
		self.C_Class.rotateUp(degrees)

	def rotate_down(self, degrees):
		self.C_Class.rotateDown(degrees)

	def rotate_left(self, degrees):
		self.C_Class.rotateLeft(degrees)

	def rotate_right(self, degrees):
		self.C_Class.rotateRight(degrees)

	def roll_left(self, degrees):
		self.C_Class.rollLeft(degrees)

	def roll_right(self, degrees):
		self.C_Class.rollRight(degrees)

	def measure_distance(self, pos, dir):
		"""Measures distance to next object in direction of dir.

		Parameters
		----------
		pos : list of float
			The x, y, z coordinates of position. 
		dir : list of float
			The x, y, z coordinates of dir Vector. Will be normalized by the function.

		Returns
		-------
		float
			The distance from pos to the next object in direction of dir. If no object in that direction return -1.

		"""

		return self.C_Class.getDistance(pos[0],pos[1],pos[2],dir[0],dir[1],dir[2])

	def set_panoramic_hfov(self, hfov):
		self.C_Class.setPanoramicHfov(hfov)

	def set_panoramic_vfov(self, vfov):
		self.C_Class.setPanoramicVfov(vfov)

	def set_panoramic_width(self, width):
		self.C_Class.setPanoramicWidth(width)

	def set_pinhole_fov(self, fov):
		self.C_Class.setPinholeFov(fov)

	def set_pinhole_width(self, width):
		self.C_Class.setPinholeWidth(width)

	def set_pinhole_height(self, height):
		self.C_Class.setPinholeHeight(height)

	def set_acceptance_angle(self, acceptance_angle):
		self.C_Class.setAcceptanceAngle(acceptance_angle)

	def set_num_samples(self, num_samples):
		self.C_Class.setNumSamples(num_samples)

	def set_ommatidium_size(self, ommatidium_size):
		self.C_Class.setOmmatidiumSize(ommatidium_size)

	def get_image_size(self):
		"""Get the rendered Image Dimensions (width, height)
		
		Returns
		-------
		Tuple of ints
			Width, Height of rendered Image in Pixels.

		"""
		return (self.C_Class.getImageWidth(),self.C_Class.getImageHeight())


	def set_verbose_lvl(self, verbose_lvl):
		self.C_Class.setVerboseLvl(verbose_lvl)

	def get_scene_bounds(self):
		""" Returns the axis aligned bounding box of the scene.

		Returns
		-------
		List of Tuples
			The upper x,y,z coordinates and the lower x,y,z coordinates of the bounding box.

		"""

		cdef float lower_x = 0.0, lower_y = 0.0, lower_z = 0.0
		cdef float upper_x = 0.0, upper_y = 0.0, upper_z = 0.0

		self.C_Class.getSceneBounds(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z)

		return [(lower_x, lower_y, lower_z), (upper_x, upper_y, upper_z)]

	def get_settings(self):
		"""Get all render settings as Dictionary.

		Returns : dict
			Dictionary of render settings.

		"""

		cdef RenderSettings s = self.C_Class.getSettings()
		cdef object sobj = s 
		return s
		