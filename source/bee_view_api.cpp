#include "../build/BeeView/BeeView/stdafx.h"

#include "bee_view_api.h"

namespace BeeView
{
	BeeViewApplication::BeeViewApplication(std::string modelLocation) : m_modelLocation(modelLocation), m_renderer(nullptr, nullptr)
	{
		// load scene
		//std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj";
		std::shared_ptr<Scene> scene = loadOBJ(modelLocation);

		// load beeEye
		// load the ommatidial array from csv file
		BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
		std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee-eye-model\\ommatidia.csv";
		beeEye->loadFromCSV(csvfile);

		// setup the cameras
		m_beeEyeCamera = std::make_shared<BeeEyeCamera>(beeEye);
		m_panoramicCamera = std::make_shared<PanoramicCamera>(500);
		m_pinholeCamera = std::make_shared<PinholeCamera>(500,400);

		// setup renderer
		m_renderer = Renderer(scene, m_beeEyeCamera);
	}

	std::vector<std::vector<std::vector<float>>> BeeViewApplication::render()
	{
		// adjust cam position (5m above ground)
		// sets camera to be 5 m above ground, returns camera y position
		// prevent to much up and down: smooth out the flight, eg dont allow to large changes? problem with trees / bushes?

		std::shared_ptr<Image> img = m_renderer.renderToImage();

		// make return image a height x width x rgb matrix
		std::vector<std::vector<std::vector<float>>> returnImage;
		returnImage.resize(img->m_height, std::vector<std::vector<float> >(img->m_width, std::vector<float>(3)));

		for (int y = 0; y < img->m_height; y++)
			for (int x = 0; x < img->m_width; x++)
			{
				Color color = img->getPixel(x, y);
				returnImage[y][x][0] = color.m_r;
				returnImage[y][x][1] = color.m_g;
				returnImage[y][x][2] = color.m_b;
			}

		return returnImage;
	}

}