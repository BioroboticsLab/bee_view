#include "../src/stdafx.h"

#include "beeview_api.h"

namespace BeeView
{
	/* lvl 0: no output, lvl 1: basic output, lvl 2: stat output, lvl 3: Debug output */
	int verbose_lvl = 2;

	/* entry point of bee_view */
	BeeViewApplication::BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded, int verbose)
		: m_sceneFile(sceneFile), m_ommatidiaFile(ommatidiaFile), m_renderer(nullptr, nullptr)
	{
		verbose_lvl = verbose;

		// load scene
		//std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj";
		std::shared_ptr<Scene> scene = loadOBJ(m_sceneFile, leftHanded);

		// load beeEye
		// load the ommatidial array from csv file
		BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
		beeEye->loadFromCSV(m_ommatidiaFile);

		// setup the cameras
		m_beeEyeCamera = std::make_shared<BeeEyeCamera>(beeEye);
		m_panoramicCamera = std::make_shared<PanoramicCamera>(1000);
		m_pinholeCamera = std::make_shared<PinholeCamera>(500,400);

		// setup renderer
		m_renderer = Renderer(scene, m_beeEyeCamera);

		m_renderMode = Camera::Type::BEE_EYE;
	}

	BeeViewApplication::~BeeViewApplication()
	{
		m_renderer.m_scene->cleanupEmbree();
	}

	std::vector<std::vector<std::vector<float>>> BeeViewApplication::render()
	{
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

	void BeeViewApplication::setCameraPosition(float x, float y, float z)
	{
		Vec3f pos = Vec3f(x, y, z);

		if (m_renderMode == Camera::Type::BEE_EYE)
			m_beeEyeCamera->moveTo(pos);
		else if (m_renderMode == Camera::Type::PINHOLE)
			m_pinholeCamera->moveTo(pos);
		else if (m_renderMode == Camera::Type::PANORAMIC)
			m_panoramicCamera->moveTo(pos);

		return;
	}
	void BeeViewApplication::getCameraPosition(float &out_x, float &out_y, float &out_z)
	{
		Vec3f pos = Vec3f::Zero();

		if(m_renderMode == Camera::Type::BEE_EYE)
			pos = m_beeEyeCamera->m_viewMatrix.translation();
		else if (m_renderMode == Camera::Type::PINHOLE)
			pos = m_pinholeCamera->m_viewMatrix.translation();
		else if (m_renderMode == Camera::Type::PANORAMIC)
			pos = m_panoramicCamera->m_viewMatrix.translation();

		out_x = pos(0);
		out_y = pos(1);
		out_z = pos(2);

		return;
	}
	void BeeViewApplication::setCameraDirVector(float x, float y, float z)
	{
		Vec3f dir = Vec3f(x, y, z);

		dir.normalize();

		if (m_renderMode == Camera::Type::BEE_EYE)
			m_beeEyeCamera->setDir(dir);
		else if (m_renderMode == Camera::Type::PINHOLE)
			m_pinholeCamera->setDir(dir);
		else if (m_renderMode == Camera::Type::PANORAMIC)
			m_panoramicCamera->setDir(dir);
		return;
	}

	void BeeViewApplication::getCameraDirVector(float &out_x, float &out_y, float &out_z)
	{
		Vec3f dir = Vec3f::Zero();

		if (m_renderMode == Camera::Type::BEE_EYE)
			dir = m_beeEyeCamera->getDir();
		else if (m_renderMode == Camera::Type::PINHOLE)
			dir = m_pinholeCamera->getDir();
		else if (m_renderMode == Camera::Type::PANORAMIC)
			dir = m_panoramicCamera->getDir();

		out_x = dir(0);
		out_y = dir(1);
		out_z = dir(2);

		return;
	}

	void BeeViewApplication::setRenderModeBeeEye()
	{
		m_renderMode = Camera::Type::BEE_EYE;
		m_renderer.setCamera(m_beeEyeCamera);
		return;
	}
	void BeeViewApplication::setRenderModePinhole()
	{
		m_renderMode = Camera::Type::PINHOLE;
		m_renderer.setCamera(m_pinholeCamera);
		return;
	}
	void BeeViewApplication::setRenderModePanoramic()
	{
		m_renderMode = Camera::Type::PANORAMIC;
		m_renderer.setCamera(m_panoramicCamera);
		return;
	}

}