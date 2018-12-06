#pragma once

#include "utility.h"
#include "scene.h"
#include "camera.h"
#include "bee_eye.h"
#include "sampler.h"

namespace BeeView {

	class Renderer
	{
	private:
		std::shared_ptr<Camera> m_camera;

	public:
		std::shared_ptr<Scene> m_scene;

		Renderer(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) : m_scene(scene), m_camera(camera) 
		{ 
			if(m_scene != nullptr)
				m_scene->initEmbree();
		}

		/* renders the image of scene according to set camera */
		std::unique_ptr<Image> renderToImage();

		/* renders the image according to parameters in beeEyeCamera, returns
		3 arrays for each eye: elevation angles, azimuth angles, and color values with length of n ommatidia
		*/
		void renderAgent(std::vector<float> &out_leftElevation, std::vector<float> &out_leftAzimuth, std::vector<Color> &out_leftColor, std::vector<float> &out_rightElevation, std::vector<float> &out_rightAzimuth, std::vector<Color> &out_rightColor,  std::vector<int> &out_x, std::vector<int> &out_y);


		/* switch camera */
		void setCamera(std::shared_ptr<Camera> camera)
		{
			m_camera = camera;
		}

		/* gets distance to next object in direction of dir. returns -1 if no object is in direction of dir. */
		float getDistance(Vec3f pos, Vec3f dir);

	private:

		/* render normal image according to parameters in pinholecamera*/
		std::unique_ptr<Image> renderToImagePinhole();

		/* renders the bee eyes according to parameters in beeEyeCamera, returns bee view */
		std::unique_ptr<Image> renderToImageBeeEye();

		std::unique_ptr<Image> renderToImagePanoramic();
	
		/* PINHOLE: renders single pixel */
		Color renderPixel(float x, float y);

		/* Shoot a ray from camera position in direction dir, return texture color at hitpoint */
		Color shootRay(const Vec3f &dir);

		/* returns a pseudo random number calsculated from ID (very not random) */
		Color randomColor(const int ID);

		/* returns color where azimuth is the red part and elevation the blue part */
		Color azimuthElevationColor(const int a, const int e);

		/* renders the given beeeye onto the given image */
		void renderBeeEye(std::unique_ptr<Image> &img, Side side);
		void renderBeeEye(std::unique_ptr<Image> &img, Side side, std::vector<float> &out_elevation, std::vector<float> &out_azimuth, std::vector<Color> &out_color, std::vector<int> &out_x, std::vector<int> &out_y, bool agent = false);


		struct ConvertCoordsParams
		{
			int x_min; // min x of beyeye
			int x_max;
			int y_min;
			int y_max;
		};

		/* converts ommatidium.m_x and ommatidium.m_y to image coordinates, saves them in out_x, out_y */
		void convert2ImageCoords(const Ommatidium &ommatidium, const BeeEye::Ptr beeEye, int &out_x, int &out_y);

		/* draw a square a x,y (top left corner of sqaure) position, size a square side length */
		void drawSquare(std::unique_ptr<Image> &img, const int x, const int y, const int size, Color color)
		{
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					img->set(x + i, y + j, color);
		}

		/* draw a cross at x,y position */
		void drawCross(std::unique_ptr<Image> &img, const int x, const int y, const int length = 6, Color color = Color(0,1,0))
		{
			for (int i = 0; i < length; i++)
			{
				img->set(x - i, y, color);
				img->set(x + i, y, color);
				img->set(x, y - i, color);
				img->set(x, y + i, color);
			}
			return;
		}
	};

}