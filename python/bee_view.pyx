from libcpp.string cimport string
from libcpp.vector cimport vector

# this could also go into a seperate .pxd file
cdef extern from "bee_view_api.h" namespace "BeeView":

	cdef cppclass BeeViewApplication:
		BeeViewApplication(string) except +

		#Methods
		vector[vector[vector[float]]] render()

cdef class BeeViewPy:
	cdef BeeViewApplication *C_Class

	def __cinit__(self, string modelLocation):
		self.C_Class = new BeeViewApplication(modelLocation)
	def __dealloc__(self):
		del self.C_Class

	def render(self):
		return self.C_Class.render()
