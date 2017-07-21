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
		m_pinholeCamera = std::make_shared<PinholeCamera>(500, 400);

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

		if (!validate(pos))
		{
			std::cerr << "Invalid Vector. ";
			return;
		}
		m_beeEyeCamera->setPosition(pos);
		m_pinholeCamera->setPosition(pos);
		m_panoramicCamera->setPosition(pos);

		return;
	}

	void BeeViewApplication::getCameraPosition(float &out_x, float &out_y, float &out_z)
	{
		Vec3f pos = m_panoramicCamera->getPosition();

		out_x = pos(0);
		out_y = pos(1);
		out_z = pos(2);

		return;
	}

	void BeeViewApplication::setCameraDirVector(float x, float y, float z)
	{
		Vec3f dir = Vec3f(x, y, z);
		if (!validate(dir))
		{
			std::cerr << "Invalid Vector. ";
			return;
		}
		dir.normalize();


		m_beeEyeCamera->setDir(dir);
		m_pinholeCamera->setDir(dir);
		m_panoramicCamera->setDir(dir);

		return;
	}

	void BeeViewApplication::getCameraDirVector(float &out_x, float &out_y, float &out_z)
	{
		Vec3f dir = m_panoramicCamera->getDir();

		out_x = dir(0);
		out_y = dir(1);
		out_z = dir(2);

		return;
	}

	void BeeViewApplication::setRenderMode(int mode)
	{
		if (mode == 1)
		{
			m_renderMode = Camera::Type::PANORAMIC;
			m_renderer.setCamera(m_panoramicCamera);
		}
		else if (mode == 2)
		{
			m_renderMode = Camera::Type::PINHOLE;
			m_renderer.setCamera(m_pinholeCamera);
		}
		else
		{
			m_renderMode = Camera::Type::BEE_EYE;
			m_renderer.setCamera(m_beeEyeCamera);
		}
		return;
	}

	int BeeViewApplication::getRenderMode()
	{
		if (m_renderMode == Camera::Type::BEE_EYE)
			return 0;
		if (m_renderMode == Camera::Type::PANORAMIC)
			return 1;
		if (m_renderMode == Camera::Type::PINHOLE)
			return 2;
		return -1;
	}


	float BeeViewApplication::getDistance(float posX, float posY, float posZ, float dirX, float dirY, float dirZ)
	{
		Vec3f pos = Vec3f(posX, posY, posZ);
		Vec3f dir = Vec3f(dirX, dirY, dirZ);
		return m_renderer.getDistance(pos, dir);
	}

	void BeeViewApplication::setPanoramicCameraXfov(float xFov)
	{
		if (xFov < 0.0f || std::isnan(xFov) || std::isinf(xFov))
		{
			std::cerr << "Invalid value for xFov." << std::endl;
		}

		m_panoramicCamera->m_xFov = xFov;
	}

	/* overwrites height */
	void BeeViewApplication::setPanoramicCameraYfov(float yFov)
	{
		if (yFov < 0.0f || std::isnan(yFov) || std::isinf(yFov))
		{
			std::cerr << "Invalid value for yFov." << std::endl;
		}
		m_panoramicCamera->m_yFov = yFov;
	}

	void BeeViewApplication::setPanoramicCameraWidth(int width)
	{
		if (width < 0)
		{
			std::cerr << "Invalid width." << std::endl;
			return;
		}
		m_panoramicCamera->m_width = width;
	}

	void BeeViewApplication::rotateCameraUp(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rotateUp(degrees);
		m_panoramicCamera->rotateUp(degrees);
		m_pinholeCamera->rotateUp(degrees);
	}

	void BeeViewApplication::rotateCameraDown(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rotateDown(degrees);
		m_panoramicCamera->rotateDown(degrees);
		m_pinholeCamera->rotateDown(degrees);
	}

	void BeeViewApplication::rotateCameraRight(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rotateRight(degrees);
		m_panoramicCamera->rotateRight(degrees);
		m_pinholeCamera->rotateRight(degrees);
	}

	void BeeViewApplication::rotateCameraLeft(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rotateLeft(degrees);
		m_panoramicCamera->rotateLeft(degrees);
		m_pinholeCamera->rotateLeft(degrees);
	}

	void BeeViewApplication::rollCameraRight(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rollRight(degrees);
		m_panoramicCamera->rollRight(degrees);
		m_pinholeCamera->rollRight(degrees);
	}

	void BeeViewApplication::rollCameraLeft(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		m_beeEyeCamera->rollLeft(degrees);
		m_panoramicCamera->rollLeft(degrees);
		m_pinholeCamera->rollLeft(degrees);
	}

	void BeeViewApplication::setPinholeCameraFov(float fov)
	{
		if (fov > 179.0f || fov < 0.0f || std::isnan(fov) || std::isinf(fov))
		{
			std::cerr << "Invalid fov for PINHOLE camera. Use Panoramic camera instead" << std::endl;
		}
		m_pinholeCamera->setFOV(fov);
	}

	void BeeViewApplication::setPinholeCameraWidth(int width)
	{
		if (width < 0)
		{
			std::cerr << "Invalid width. " << std::endl;
			return;
		}
		m_pinholeCamera->setWidth(width);
	}

	void BeeViewApplication::setPinholeCameraHeight(int height)
	{
		if (height < 0)
		{
			std::cerr << "Invalid height." << std::endl;
			return;
		}
		m_pinholeCamera->setHeight(height);
	}

	void BeeViewApplication::setBeeEyeCameraAcceptanceAngle(float acceptanceAngle)
	{
		if (acceptanceAngle > 180 || acceptanceAngle < 0 || std::isnan(acceptanceAngle) || std::isinf(acceptanceAngle))
		{
			std::cerr << "Invalid acceptance Angle." << std::endl;
			return;
		}
		m_beeEyeCamera->m_sampler.setAcceptanceAngle(acceptanceAngle);
	}

	/* set the number of samplepoints taken per ommatidium */
	void BeeViewApplication::setBeeEyeCameraNumSamplePoints(int numSamples)
	{
		int sqrtNumSamples = numSamples - (int)std::sqrt(numSamples);
		sqrtNumSamples = (int)floor(std::sqrt(sqrtNumSamples));
		if (sqrtNumSamples < 1)
			sqrtNumSamples = 1; // TODO bug when 1 -> endless rendering
		m_beeEyeCamera->m_sampler.setSqrtNumSamplePoints(sqrtNumSamples);
	}

	/* ommatidium size in pixels, should be an even number (if not: incremented to next number) */
	void BeeViewApplication::setBeeEyeCameraOmmatidiumSize(int size)
	{
		if (size < 1)
			size = 2;
		if (size % 2 != 0)
			size++;
		m_beeEyeCamera->setOmmatidiumSize(size);
	}

	void BeeViewApplication::setVerboseLvl(int verboseLvl)
	{
		verbose_lvl = verboseLvl;
	}

	int BeeViewApplication::getImageHeight()
	{
		if (m_renderMode == Camera::Type::BEE_EYE)
			return m_beeEyeCamera->getImageHeight();
		else if (m_renderMode == Camera::Type::PANORAMIC)
			return m_panoramicCamera->getHeight();
		else
			return m_pinholeCamera->getHeight();
	}

	int BeeViewApplication::getImageWidth()
	{
		if (m_renderMode == Camera::Type::BEE_EYE)
			return m_beeEyeCamera->getImageWidth();
		else if (m_renderMode == Camera::Type::PANORAMIC)
			return m_panoramicCamera->m_width;
		else
			return m_pinholeCamera->getWidth();
	}

	void BeeViewApplication::getSceneBounds(float &out_lower_x, float &out_lower_y, float &out_lower_z, float &out_upper_x, float &out_upper_y, float &out_upper_z)
	{
		RTCBounds aabb = m_renderer.m_scene->m_bounds;

		out_lower_x = aabb.lower_x;
		out_lower_y = aabb.lower_y;
		out_lower_z = aabb.lower_z;
		out_upper_x = aabb.upper_x;
		out_upper_y = aabb.upper_y;
		out_upper_z = aabb.upper_z;

		return;
	}

	RenderSettings BeeViewApplication::getSettings()
	{
		RenderSettings settings;

		if (m_renderMode == Camera::Type::BEE_EYE)
			settings.renderMode = 0;
		if (m_renderMode == Camera::Type::PANORAMIC)
			settings.renderMode = 1;
		if (m_renderMode == Camera::Type::PINHOLE)
			settings.renderMode = 2;

		Vec3f pos = m_panoramicCamera->getPosition();
		settings.xPos = pos(0);
		settings.yPos = pos(1);
		settings.zPos = pos(2);

		Vec3f dir = m_panoramicCamera->getDir();
		settings.xDir = dir(0);
		settings.yDir = dir(1);
		settings.zDir = dir(2);

		settings.acceptanceAngle = m_beeEyeCamera->m_sampler.getAcceptanceAngle();
		settings.ommatidiumSize = m_beeEyeCamera->getOmmatidiumSize();
		settings.beeImageWidth = m_beeEyeCamera->getImageWidth();
		settings.beeImageHeight = m_beeEyeCamera->getImageHeight();
		settings.numSamples = m_beeEyeCamera->m_sampler.getTotalSamplePoints();

		settings.xFov = m_panoramicCamera->m_xFov;
		settings.yFov = m_panoramicCamera->m_yFov;
		settings.panoramicWidth = m_panoramicCamera->m_width;
		settings.panoramicHeight = m_panoramicCamera->getHeight();

		settings.aspectRatio = m_pinholeCamera->getImageAspectRatio();
		settings.fov = m_pinholeCamera->getFOV();
		settings.scale = m_pinholeCamera->getScale();
		settings.pinholeWidth = m_pinholeCamera->getWidth();
		settings.pinholeHeight = m_pinholeCamera->getHeight();

		return settings;
	}
}