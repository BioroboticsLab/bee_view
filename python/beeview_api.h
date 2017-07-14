// This file defines the API for the bee_view programm.

#pragma once
#include "../src/renderer.h"
#include "../src/obj_loader.h"

namespace BeeView
{
	class BeeViewApplication
	{

	private:

		Renderer m_renderer;
		std::shared_ptr<BeeEyeCamera> m_beeEyeCamera;
		std::shared_ptr<PinholeCamera> m_pinholeCamera;
		std::shared_ptr<PanoramicCamera> m_panoramicCamera;

		Camera::Type m_renderMode;
		
		/* A wavefront .obj file which contains the scene data (located in data folder of bee_view) */
		std::string m_sceneFile;
		/* A .csv file, which contains the elevation and azimuth angles for the bee eye (located in data folder of bee_view) */
		std::string m_ommatidiaFile; 

	public:

		/* Sets up the scene and renderer. If leftHanded = true: flips the z coordinate and the texture coordinates. 
		   verbose controls the verbose lvl: if 0 no output, if 1 basic output, if 2 stat output, if > 2 debug output. */
		BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded = false, int verbose = 2);

		~BeeViewApplication();

		/* renders an image with the current active camera and the settings specified be the camera */
		std::vector<std::vector<std::vector<float>>> render();

		// camera setters: position, rotation, cameratype
		void setCameraPosition(float x, float y, float z);
		void getCameraPosition(float &out_x, float &out_y, float &out_z);
		void setCameraDirVector(float x, float y, float z);
		void getCameraDirVector(float &out_x, float &out_y, float &out_z);

		void setRenderModeBeeEye();
		void setRenderModePinhole();
		void setRenderModePanoramic();

		/* reutrns the camera height above ground. returns negativ values if camera is below ground. */
		float heightAboveGround();

		// camera options: 
		// pinhole: fov, width height; 
		// panoramic: xfov, yfov, width, height?; 
		// beeeye: acceptanceangle, numsamples, ommatidiumsize

		void setPanoramicCameraXfov(float xFov);
		void setPanoramicCameraYfov(float yFov);
		void setPanoramicCameraWidth(int width);


		void rotateCameraUp(float degrees);
		void rotateCameraDown(float degrees);
		void rotateCameraRight(float degrees);
		void rotateCameraLeft(float degrees);
		void rollCameraRight(float degrees);
		void rollCameraLeft(float degrees);

		void setPinholeCameraFov(float fov);
		void setPinholeCameraWidth(int width);
		void setPinholeCameraHeight(int height);

		void setBeeEyeCameraAcceptanceAngle(float acceptanceAngle);
		/* set the number of samplepoints taken per ommatidium */
		void setBeeEyeCameraNumSamplePoints(int numSamples);
		/* ommatidium size in pixels, must be an even number */
		void setBeeEyeCameraOmmatidiumSize(int size);

		/* overwrites yFov, not implemented */
		//void setPanoramicCameraHeight(int height);
		void setVerboseLvl(int verboseLvl);
	};
}