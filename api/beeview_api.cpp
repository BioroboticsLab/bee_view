#include "../src/stdafx.h"

#include "beeview_api.h"
#include "../src/renderer.h"
#include "../src/obj_loader.h"

namespace BeeView
{
	/* lvl 0: no output, lvl 1: basic output, lvl 2: stat output, lvl 3: Debug output */
	int verbose_lvl = 2;



	class BeeViewApplication::Impl 
	{ 
	public:
		std::shared_ptr<BeeEyeCamera> m_beeEyeCamera;
		std::shared_ptr<PinholeCamera> m_pinholeCamera;
		std::shared_ptr<PanoramicCamera> m_panoramicCamera;

		Camera::Type m_renderMode;

		/* A wavefront .obj file which contains the scene data (located in data folder of bee_view) */
		std::string m_sceneFile;
		/* A .csv file, which contains the elevation and azimuth angles for the bee eye (located in data folder of bee_view) */
		std::string m_ommatidiaFile;

	
		Renderer m_renderer;

		Impl(std::string sceneFile, std::string ommatidiaFile)
			: m_renderer(nullptr,nullptr), m_sceneFile(sceneFile), m_ommatidiaFile(ommatidiaFile)
		{

		}
	};

	/* entry point of bee_view */
	BeeViewApplication::BeeViewApplication(std::string sceneFile, std::string ommatidiaFile, bool leftHanded, int verbose)
		: impl_(new Impl(sceneFile,ommatidiaFile))
	{
		verbose_lvl = verbose;

		// load scene
		//std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj";
		std::shared_ptr<Scene> scene = loadOBJ(impl_->m_sceneFile, leftHanded);

		// load beeEye
		// load the ommatidial array from csv file
		BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
		beeEye->loadFromCSV(impl_->m_ommatidiaFile);

		// setup the cameras
		impl_->m_beeEyeCamera = std::make_shared<BeeEyeCamera>(beeEye);
		impl_->m_panoramicCamera = std::make_shared<PanoramicCamera>(1000);
		impl_->m_pinholeCamera = std::make_shared<PinholeCamera>(500, 400);

		// setup renderer
		impl_->m_renderer = Renderer(scene, impl_->m_beeEyeCamera);

		impl_->m_renderMode = Camera::Type::BEE_EYE;
	}

	BeeViewApplication::~BeeViewApplication()
	{
		impl_->m_renderer.m_scene->cleanupEmbree();
	}

	BeeViewApplication::BeeViewApplication(BeeViewApplication &&) noexcept = default;
	BeeViewApplication& BeeViewApplication::operator=(BeeViewApplication &&) noexcept = default;

	std::vector<std::vector<std::vector<float>>> BeeViewApplication::render()
	{
		std::shared_ptr<Image> img = impl_->m_renderer.renderToImage();

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

	void BeeViewApplication::setPosition(float x, float y, float z)
	{

		Vec3f pos = Vec3f(x, y, z);

		if (!validate(pos))
		{
			std::cerr << "Invalid Vector. ";
			return;
		}
		impl_->m_beeEyeCamera->setPosition(pos);
		impl_->m_pinholeCamera->setPosition(pos);
		impl_->m_panoramicCamera->setPosition(pos);

		return;
	}

	void BeeViewApplication::getPosition(float &out_x, float &out_y, float &out_z)
	{
		Vec3f pos = impl_->m_panoramicCamera->getPosition();

		out_x = pos(0);
		out_y = pos(1);
		out_z = pos(2);

		return;
	}

	void BeeViewApplication::setDirection(float x, float y, float z)
	{
		Vec3f dir = Vec3f(x, y, z);
		if (!validate(dir))
		{
			std::cerr << "Invalid Vector. ";
			return;
		}
		dir.normalize();


		impl_->m_beeEyeCamera->setDir(dir);
		impl_->m_pinholeCamera->setDir(dir);
		impl_->m_panoramicCamera->setDir(dir);

		return;
	}

	void BeeViewApplication::getDirection(float &out_x, float &out_y, float &out_z)
	{
		Vec3f dir = impl_->m_panoramicCamera->getDir();

		out_x = dir(0);
		out_y = dir(1);
		out_z = dir(2);

		return;
	}

	void BeeViewApplication::setMode(int mode)
	{
		if (mode == 1)
		{
			impl_->m_renderMode = Camera::Type::PANORAMIC;
			impl_->m_renderer.setCamera(impl_->m_panoramicCamera);
		}
		else if (mode == 2)
		{
			impl_->m_renderMode = Camera::Type::PINHOLE;
			impl_->m_renderer.setCamera(impl_->m_pinholeCamera);
		}
		else
		{
			impl_->m_renderMode = Camera::Type::BEE_EYE;
			impl_->m_renderer.setCamera(impl_->m_beeEyeCamera);
		}
		return;
	}

	int BeeViewApplication::getMode()
	{
		if (impl_->m_renderMode == Camera::Type::BEE_EYE)
			return 0;
		if (impl_->m_renderMode == Camera::Type::PANORAMIC)
			return 1;
		if (impl_->m_renderMode == Camera::Type::PINHOLE)
			return 2;
		return -1;
	}


	float BeeViewApplication::getDistance(float posX, float posY, float posZ, float dirX, float dirY, float dirZ)
	{
		Vec3f pos = Vec3f(posX, posY, posZ);
		Vec3f dir = Vec3f(dirX, dirY, dirZ);
		return impl_->m_renderer.getDistance(pos, dir);
	}

	void BeeViewApplication::setPanoramicHfov(float xFov)
	{
		if (xFov < 0.0f || std::isnan(xFov) || std::isinf(xFov))
		{
			std::cerr << "Invalid value for xFov." << std::endl;
		}

		impl_->m_panoramicCamera->m_xFov = xFov;
	}

	/* overwrites height */
	void BeeViewApplication::setPanoramicVfov(float yFov)
	{
		if (yFov < 0.0f || std::isnan(yFov) || std::isinf(yFov))
		{
			std::cerr << "Invalid value for yFov." << std::endl;
		}
		impl_->m_panoramicCamera->m_yFov = yFov;
	}

	void BeeViewApplication::setPanoramicWidth(int width)
	{
		if (width < 0)
		{
			std::cerr << "Invalid width." << std::endl;
			return;
		}
		impl_->m_panoramicCamera->m_width = width;
	}

	void BeeViewApplication::rotateUp(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rotateUp(degrees);
		impl_->m_panoramicCamera->rotateUp(degrees);
		impl_->m_pinholeCamera->rotateUp(degrees);
	}

	void BeeViewApplication::rotateDown(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rotateDown(degrees);
		impl_->m_panoramicCamera->rotateDown(degrees);
		impl_->m_pinholeCamera->rotateDown(degrees);
	}

	void BeeViewApplication::rotateRight(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rotateRight(degrees);
		impl_->m_panoramicCamera->rotateRight(degrees);
		impl_->m_pinholeCamera->rotateRight(degrees);
	}

	void BeeViewApplication::rotateLeft(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rotateLeft(degrees);
		impl_->m_panoramicCamera->rotateLeft(degrees);
		impl_->m_pinholeCamera->rotateLeft(degrees);
	}

	void BeeViewApplication::rollRight(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rollRight(degrees);
		impl_->m_panoramicCamera->rollRight(degrees);
		impl_->m_pinholeCamera->rollRight(degrees);
	}

	void BeeViewApplication::rollLeft(float degrees)
	{
		if (std::isnan(degrees) || std::isinf(degrees))
		{
			std::cerr << "Invalid input (degrees)." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->rollLeft(degrees);
		impl_->m_panoramicCamera->rollLeft(degrees);
		impl_->m_pinholeCamera->rollLeft(degrees);
	}

	void BeeViewApplication::setPinholeFov(float fov)
	{
		if (fov > 179.0f || fov < 0.0f || std::isnan(fov) || std::isinf(fov))
		{
			std::cerr << "Invalid fov for PINHOLE camera. Use Panoramic camera instead" << std::endl;
		}
		impl_->m_pinholeCamera->setFOV(fov);
	}

	void BeeViewApplication::setPinholeWidth(int width)
	{
		if (width < 0)
		{
			std::cerr << "Invalid width. " << std::endl;
			return;
		}
		impl_->m_pinholeCamera->setWidth(width);
	}

	void BeeViewApplication::setPinholeHeight(int height)
	{
		if (height < 0)
		{
			std::cerr << "Invalid height." << std::endl;
			return;
		}
		impl_->m_pinholeCamera->setHeight(height);
	}

	void BeeViewApplication::setAcceptanceAngle(float acceptanceAngle)
	{
		if (acceptanceAngle > 180 || acceptanceAngle < 0 || std::isnan(acceptanceAngle) || std::isinf(acceptanceAngle))
		{
			std::cerr << "Invalid acceptance Angle." << std::endl;
			return;
		}
		impl_->m_beeEyeCamera->m_sampler.setAcceptanceAngle(acceptanceAngle);
	}

	/* set the number of samplepoints taken per ommatidium */
	void BeeViewApplication::setNumSamples(int numSamples)
	{
		int sqrtNumSamples = numSamples - (int)std::sqrt(numSamples);
		sqrtNumSamples = (int)floor(std::sqrt(sqrtNumSamples));
		if (sqrtNumSamples < 1)
			sqrtNumSamples = 1; // TODO bug when 1 -> endless rendering
		impl_->m_beeEyeCamera->m_sampler.setSqrtNumSamplePoints(sqrtNumSamples);
	}

	/* ommatidium size in pixels, should be an even number (if not: incremented to next number) */
	void BeeViewApplication::setOmmatidiumSize(int size)
	{
		if (size < 1)
			size = 2;
		if (size % 2 != 0)
			size++;
		impl_->m_beeEyeCamera->setOmmatidiumSize(size);
	}

	void BeeViewApplication::setVerboseLvl(int verboseLvl)
	{
		verbose_lvl = verboseLvl;
	}

	int BeeViewApplication::getImageHeight()
	{
		if (impl_->m_renderMode == Camera::Type::BEE_EYE)
			return impl_->m_beeEyeCamera->getImageHeight();
		else if (impl_->m_renderMode == Camera::Type::PANORAMIC)
			return impl_->m_panoramicCamera->getHeight();
		else
			return impl_->m_pinholeCamera->getHeight();
	}

	int BeeViewApplication::getImageWidth()
	{
		if (impl_->m_renderMode == Camera::Type::BEE_EYE)
			return impl_->m_beeEyeCamera->getImageWidth();
		else if (impl_->m_renderMode == Camera::Type::PANORAMIC)
			return impl_->m_panoramicCamera->m_width;
		else
			return impl_->m_pinholeCamera->getWidth();
	}

	void BeeViewApplication::getSceneBounds(float &out_lower_x, float &out_lower_y, float &out_lower_z, float &out_upper_x, float &out_upper_y, float &out_upper_z)
	{
		RTCBounds aabb = impl_->m_renderer.m_scene->m_bounds;

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

		if (impl_->m_renderMode == Camera::Type::BEE_EYE)
			settings.renderMode = 0;
		if (impl_->m_renderMode == Camera::Type::PANORAMIC)
			settings.renderMode = 1;
		if (impl_->m_renderMode == Camera::Type::PINHOLE)
			settings.renderMode = 2;

		Vec3f pos = impl_->m_panoramicCamera->getPosition();
		settings.xPos = pos(0);
		settings.yPos = pos(1);
		settings.zPos = pos(2);

		Vec3f dir = impl_->m_panoramicCamera->getDir();
		settings.xDir = dir(0);
		settings.yDir = dir(1);
		settings.zDir = dir(2);

		settings.acceptanceAngle = impl_->m_beeEyeCamera->m_sampler.getAcceptanceAngle();
		settings.ommatidiumSize = impl_->m_beeEyeCamera->getOmmatidiumSize();
		settings.beeImageWidth = impl_->m_beeEyeCamera->getImageWidth();
		settings.beeImageHeight = impl_->m_beeEyeCamera->getImageHeight();
		settings.numSamples = impl_->m_beeEyeCamera->m_sampler.getTotalSamplePoints();

		settings.xFov = impl_->m_panoramicCamera->m_xFov;
		settings.yFov = impl_->m_panoramicCamera->m_yFov;
		settings.panoramicWidth = impl_->m_panoramicCamera->m_width;
		settings.panoramicHeight = impl_->m_panoramicCamera->getHeight();

		settings.aspectRatio = impl_->m_pinholeCamera->getImageAspectRatio();
		settings.fov = impl_->m_pinholeCamera->getFOV();
		settings.scale = impl_->m_pinholeCamera->getScale();
		settings.pinholeWidth = impl_->m_pinholeCamera->getWidth();
		settings.pinholeHeight = impl_->m_pinholeCamera->getHeight();

		return settings;
	}
}