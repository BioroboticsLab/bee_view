// This file defines the API for the bee_view programm.

#pragma once
#include "../src/renderer.h"
#include "../src/obj_loader.h"

namespace BeeView
{
	// auto converted to python dict, -> python naming conventions
	struct RenderSettings
	{
		int renderMode; // 0 BEEEYE, 1 PANORAMIC, 2 PINHOLE
		
		float xPos, yPos, zPos;
		float xDir, yDir, zDir;

		// bee eye settings
		int numSamples;
		float acceptanceAngle;
		int ommatidiumSize;
		int beeImageWidth;
		int beeImageHeight;

		// Panoramic settings
		float xFov;
		float yFov;
		int panoramicWidth;
		int panoramicHeight;

		// pinhole settings
		float fov;
		float aspectRatio;
		float scale;
		int pinholeWidth;
		int pinholeHeight;

	};

	class BeeViewApplication
	{

	private:

		std::shared_ptr<BeeEyeCamera> m_beeEyeCamera;
		std::shared_ptr<PinholeCamera> m_pinholeCamera;
		std::shared_ptr<PanoramicCamera> m_panoramicCamera;

		Camera::Type m_renderMode;
		
		/* A wavefront .obj file which contains the scene data (located in data folder of bee_view) */
		std::string m_sceneFile;
		/* A .csv file, which contains the elevation and azimuth angles for the bee eye (located in data folder of bee_view) */
		std::string m_ommatidiaFile; 

	public:
		Renderer m_renderer;

		
		/* Sets up the scene and renderer. If leftHanded = true: flips the z coordinate and the texture coordinates. 
		   verbose controls the verbose lvl: if 0 no output, if 1 basic output, if 2 stat output, if > 2 debug output. */
		BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded = false, int verbose = 2);

		~BeeViewApplication();

		/* renders an image with the current active camera and the settings specified be the camera */
		std::vector<std::vector<std::vector<float>>> render();

		// camera setters and getter: position, rotation, cameratype
		void setCameraPosition(float x, float y, float z);
		void getCameraPosition(float &out_x, float &out_y, float &out_z);
		void setCameraDirVector(float x, float y, float z);
		void getCameraDirVector(float &out_x, float &out_y, float &out_z);

		/* 0 = BEE_EYE, 1 = PANORAMIC, 2 = PINHOLE */
		void setRenderMode(int mode);
		int getRenderMode();

		/* gets distance from pos to next object in direction of dir. returns -1 if no object is in direction of dir. */
		float getDistance(float posX, float posY, float posZ, float dirX, float dirY, float dirZ);

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

		int getImageWidth();
		int getImageHeight();

		void getSceneBounds(float &out_lower_x, float &out_lower_y, float &out_lower_z, float &out_upper_x, float &out_upper_y, float &out_upper_z);

		/* overwrites yFov, not implemented */
		//void setPanoramicCameraHeight(int height);

		void setVerboseLvl(int verboseLvl);

		RenderSettings getSettings();
	};
}