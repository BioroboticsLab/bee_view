// This file defines the API for the bee_view programm.

#pragma once
#include <memory>
#include <string>

namespace BeeView
{

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
		class Impl;
		std::unique_ptr<Impl> impl_;

	public:
		
		/* Sets up the scene and renderer. If leftHanded = true: flips the z coordinate and the texture coordinates. 
		   verbose controls the verbose lvl: if 0 no output, if 1 basic output, if 2 stat output, if > 2 debug output. */
		BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded = false, int verbose = 2);

		~BeeViewApplication();

		BeeViewApplication(BeeViewApplication && op) noexcept;              // movable and noncopyable
		BeeViewApplication& operator=(BeeViewApplication && op) noexcept;   //

		/* renders an image with the current active camera and the settings specified be the camera */
		std::vector<std::vector<std::vector<float>>> render();

		/* renders the image according to parameters in beeEyeCamera, returns
		12 arrays: the first 6 for the left eye.
		azimuth angles, elevation angles, r ,g , b with length of n ommatidia
		*/
		std::vector<std::vector<float>> renderAgent();


		// camera setters and getter: position, rotation, cameratype
		void setPosition(float x, float y, float z);
		void getPosition(float &out_x, float &out_y, float &out_z);
		void setDirection(float x, float y, float z);
		void getDirection(float &out_x, float &out_y, float &out_z);

		/* 0 = BEE_EYE, 1 = PANORAMIC, 2 = PINHOLE */
		void setMode(int mode);
		int getMode();

		/* gets distance from pos to next object in direction of dir. returns -1 if no object is in direction of dir. */
		float getDistance(float posX, float posY, float posZ, float dirX, float dirY, float dirZ);

		// camera options: 
		// pinhole: fov, width height; 
		// panoramic: xfov, yfov, width, height?; 
		// beeeye: acceptanceangle, numsamples, ommatidiumsize

		void setPanoramicHfov(float xFov);
		void setPanoramicVfov(float yFov);
		void setPanoramicWidth(int width);


		void rotateUp(float degrees);
		void rotateDown(float degrees);
		void rotateRight(float degrees);
		void rotateLeft(float degrees);
		void rollRight(float degrees);
		void rollLeft(float degrees);

		void setPinholeFov(float fov);
		void setPinholeWidth(int width);
		void setPinholeHeight(int height);

		void setAcceptanceAngle(float acceptanceAngle);
		/* set the number of samplepoints taken per ommatidium */
		void setNumSamples(int numSamples);
		/* ommatidium size in pixels, must be an even number */
		void setOmmatidiumSize(int size);

		int getImageWidth();
		int getImageHeight();

		/* get aabb of scene */
		void getSceneBounds(float &out_lower_x, float &out_lower_y, float &out_lower_z, float &out_upper_x, float &out_upper_y, float &out_upper_z);

		/* overwrites yFov, not implemented */
		//void setPanoramicCameraHeight(int height);

		void setVerboseLvl(int verboseLvl);

		RenderSettings getSettings();
	};
}