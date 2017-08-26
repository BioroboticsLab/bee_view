from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "beeview_api.h" namespace "BeeView":

	#cdef cppclass RenderSettings:
	cdef struct RenderSettings:
		int renderMode; # 0 BEEEYE, 1 PANORAMIC, 2 PINHOLE
		
		float xPos, yPos, zPos;
		float xDir, yDir, zDir;

		int numSamples;
		float acceptanceAngle;
		int ommatidiumSize;
		int beeImageWidth;
		int beeImageHeight;

		float xFov;
		float yFov;
		int panoramicWidth;
		int panoramicHeight;

		float fov;
		float aspectRatio;
		float scale;
		float pinholeWidth;
		float pinholeHeight;

	cdef cppclass BeeViewApplication:
		BeeViewApplication(string, string, bool) except +

		#Methods
		vector[vector[vector[float]]] render()
		vector[vector[float]] renderAgent()

		void setPosition(float, float, float)
		void getPosition(float &, float &, float &)

		void setDirection(float, float, float)
		void getDirection(float &, float &, float &)

		void setMode(int)
		int getMode()

		float getDistance(float, float, float, float, float, float);

		void rotateUp(float)
		void rotateDown(float)
		void rotateRight(float)
		void rotateLeft(float)
		void rollRight(float)
		void rollLeft(float)

		void setPinholeFov(float)
		void setPinholeWidth(int)
		void setPinholeHeight(int)

		void setAcceptanceAngle(float)
		void setNumSamples(int)
		void setOmmatidiumSize(int)

		void setPanoramicHfov(float)
		void setPanoramicVfov(float)
		void setPanoramicWidth(int)

		int getImageWidth()
		int getImageHeight()
		
		void setVerboseLvl(int)

		void getSceneBounds(float &, float &, float &, float &, float &, float &)
	
		RenderSettings getSettings()