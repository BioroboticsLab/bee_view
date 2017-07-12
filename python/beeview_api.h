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

		BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded = false, int verbose = 2);

		~BeeViewApplication();

		/* renders an image with the current active camera and the settings specified be the camera */
		std::vector<std::vector<std::vector<float>>> render();

		void setCameraPosition(float x, float y, float z);
		void getCameraPosition(float &out_x, float &out_y, float &out_z);
		void setCameraDirVector(float x, float y, float z);
		void getCameraDirVector(float &out_x, float &out_y, float &out_z);

		// camera setters: position, rotation, cameratype
		void setRenderModeBeeEye();
		void setRenderModePinhole();
		void setRenderModePanoramic();

		/* reutrns the camera height above ground. returns negativ values if camera is below ground. */
		float heightAboveGround();

#if 0
		void rotateCameraUp(float degrees);
		void rotateCameraDown(float degrees);
		void rotateCameraRight(float degrees);
		void rotateCameraLeft(float degrees);
		void rollCameraRight(float degrees);
		void rollCameraLeft(float degrees);
		
		// camera options: pinhole: fov, width height; panoramic: xfov, yfov, width, height?; beeeye: acceptanceangle, numsamples, ommatidiumsize
		void setPinholeCameraFov(float fov);
		void setPinholeCameraWidth(int width);
		void setPinholeCameraHeight(int height);

		void setPanoramicCameraXfov(float xFov);
		/* overwrites height */
		void setPanoramicCameraYfov(float yFov);
		void setPanoramicCameraWidth(int width);
		/* overwrites yFov */
		void setPanoramicCameraHeight(int height);

		void setBeeEyeCameraAcceptanceAngle(float acceptanceAngle);
		/* set the number of samplepoints taken per ommatidium */
		void setBeeEyeCameraNumSamplePoints(int numSamples);
		/* ommatidium size in pixels, must be an even number */
		void setBeeEyeCameraOmmatidiumSize(int size);
#endif

	};
}