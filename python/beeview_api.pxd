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

		void setCameraPosition(float, float, float)
		void getCameraPosition(float &, float &, float &)

		void setCameraDirVector(float, float, float)
		void getCameraDirVector(float &, float &, float &)

		void setRenderMode(int)
		int getRenderMode()

		float getDistance(float, float, float, float, float, float);

		void rotateCameraUp(float)
		void rotateCameraDown(float)
		void rotateCameraRight(float)
		void rotateCameraLeft(float)
		void rollCameraRight(float)
		void rollCameraLeft(float)

		void setPinholeCameraFov(float)
		void setPinholeCameraWidth(int)
		void setPinholeCameraHeight(int)

		void setBeeEyeCameraAcceptanceAngle(float)
		void setBeeEyeCameraNumSamplePoints(int)
		void setBeeEyeCameraOmmatidiumSize(int)

		void setPanoramicCameraXfov(float)
		void setPanoramicCameraYfov(float)
		void setPanoramicCameraWidth(int)

		int getImageWidth()
		int getImageHeight()
		
		void setVerboseLvl(int)

		void getSceneBounds(float &, float &, float &, float &, float &, float &)
	
		RenderSettings getSettings()