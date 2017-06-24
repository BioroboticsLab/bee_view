#pragma once

//#include "vec.h"
#include "utility.h"
#include "scene.h"
#include "camera.h"
#include "bee_eye.h"

namespace BeeView {

	class Renderer
	{
	private:
		std::shared_ptr<Camera> m_camera;

	public:
		std::shared_ptr<Scene> m_scene;

		Renderer(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera) : m_scene(scene), m_camera(camera) 
		{ 
			m_scene->initEmbree();
		}

		std::unique_ptr<Image> renderToImage();

		void changeCamera(std::shared_ptr<Camera> camera)
		{
			m_camera = camera;
		}

		static float gaussPDF(int version, int x, int y, float hw);
	private:

		std::unique_ptr<Image> renderToImagePinhole();
		std::unique_ptr<Image> renderToImageBeeEye();

		/* Params for Renderer::renderPixel() */
		struct RenderPixelParams
		{
			float imageAspectRatio;
			float width;
			float height;
			float scale;
		};

		/* PINHOLE: renders single pixel */
		Color renderPixel(float x, float y, const RenderPixelParams &params);

		/* BEE_EYE: renders single ommatidium */
		Color renderOmmatidium(const Ommatidium &o);

		/* Shoot a ray from camera position in direction dir, return texture color at hitpoint */
		Color shootRay(const Vec3f &dir);

		/* returns a pseudo random number calsculated from ID (very not random) */
		Color randomColor(const int ID);

		/* returns color where azimuth is the red part and elevation the blue part */
		Color azimuthElevationColor(const int a, const int e);

		/* draws the given beeeye onto the given image */
		void drawBeeEye(std::unique_ptr<Image> &img, BeeEye::Ptr &beeEye);

		struct ConvertCoordsParams
		{
			int x_min; // min x of beyeye
			int x_max;
			int y_min;
			int y_max;
		};

		/* converts ommatidium.m_x and ommatidium.m_y to image coordinates, saves them in out_x, out_y */
		void convert2ImageCoords(const Ommatidium &ommatidium, const ConvertCoordsParams &params, int &out_x, int &out_y);

		void drawCross(std::unique_ptr<Image> &img, const int x, const int y, const int length = 6, Color color = Color(0,1,0))
		{
			for (int i = 0; i < length; i++)
			{
				img->set(x - i, y, color);
				img->set(x + i, y, color);
				img->set(x, y - i, color);
				img->set(x, y + i, color);
			}
		}

	};

}