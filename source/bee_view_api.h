// This file defines the API for the bee_view programm.

#pragma once
#include "renderer.h"
#include "obj_loader.h"

namespace BeeView
{
	class BeeViewApplication
	{

	private:

		Renderer m_renderer;
		std::shared_ptr<BeeEyeCamera> m_beeEyeCamera;
		std::shared_ptr<PinholeCamera> m_pinholeCamera;
		std::shared_ptr<PanoramicCamera> m_panoramicCamera;
		
		std::string m_modelLocation;
		std::string m_ommatidiaCsvFile; // TODO: convert csv file to code and include the code. automated on build.

	public:

		BeeViewApplication(std::string modelLocation);

		/* renders an image with the current active camera and the settings specified be the camera */
		std::vector<std::vector<std::vector<float>>> render();
			
		// camera setters: position, rotation, cameratype (int)
		void setRenderModeBeeEye();
		void setRenderModePinhole();
		void setRenderModePanoramic();

		void setCameraPosition(float x, float y, float z);
		void getCameraPosition(float &out_x, float &out_y, float &out_z);
		void setCameraDirVector(float x, float y, float z);
		void getCameraDirVector(float &out_x, float &out_y, float &out_z);

		void rotateCameraUp(float degrees);
		void rotateCameraDown(float degrees);
		void rotateCameraRight(float degrees);
		void rotateCameraLeft(float degrees);
		void rollCameraRight(float degrees);
		void rollCameraLeft(float degrees);

		/* reutrns the camera height above ground. returns negativ values if camera is below ground. */
		float getCameraHeight();
		
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

	};
}