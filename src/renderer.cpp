#include "stdafx.h"

#include <chrono>

#include "renderer.h"

// debug variables, for beeEye rendering
#define TEXTURE_SHADING
//#define XYZ_SHADING
//#define SOLID_SHADING
//#define ELEVATION_AZIMUTH_SHADING
//#define SINGLE_RAY_TEXTURE_SHADING
//#define RANDOM_SHADING
//#define CHECKER_BOARD_SHADING
//#define MATERIAL_KD_SHADING

#define DEBUG

namespace BeeView {

	extern int verbose_lvl;

	std::unique_ptr<Image> Renderer::renderToImage()
	{
		if (verbose_lvl > 2)
		{
			Vec3f cam_dir = m_camera->getDir();
			Vec3f cam_pos = m_camera->getPosition();
			std::cout << std::endl << "Pos" << "(" << cam_pos(0) << "," << cam_pos(1) << "," << cam_pos(2) << "), ";
			std::cout << "Dir " << "(" << cam_dir(0) << "," << cam_dir(1) << "," << cam_dir(2) << ")";
		}
		if (verbose_lvl > 0)
			std::cout << std::endl << "Start rendering Image... ";

		if (m_camera->m_type == Camera::Type::PINHOLE)
			return renderToImagePinhole();
		else if (m_camera->m_type == Camera::Type::BEE_EYE)
			return renderToImageBeeEye();
		else if (m_camera->m_type == Camera::Type::PANORAMIC)
			return renderToImagePanoramic();
		else
		{
			std::cerr << "Renderer: Camera Type not supported. " << std::endl;
			return std::make_unique<Image>();
		}
	}

	std::unique_ptr<Image> Renderer::renderToImagePanoramic()
	{
		// horizontal spacing: xFov / width
		// vertical spacing: width/height = xFov/yFov -> height = width * yFov / xFov

		std::shared_ptr<PanoramicCamera> camera = std::static_pointer_cast<PanoramicCamera>(m_camera);
		int height = camera->getHeight();

		float hAngleSpacing = camera->m_xFov / camera->m_width;
		float vAngleSpacing = camera->m_yFov / height;

		std::unique_ptr<Image> img = std::make_unique<Image>(camera->m_width, height);

		// start at: -xfov/2, +yfov/2
		float vAngle = camera->m_yFov / 2.0f;

		for (int y = 0; y < height; y++)
		{
			float hAngle = -camera->m_xFov / 2.0f + 180.0f;

			for (int x = 0; x < camera->m_width; x++)
			{
				Vec3f dir = camera->getDir();

				camera->rotateVecX(dir, -vAngle);
				camera->rotateVecY(dir, hAngle);

				dir = camera->m_viewMatrix.linear() * dir;

				Color color = shootRay(dir);
				img->set(x, y, color);
				hAngle += hAngleSpacing;
			}
			vAngle -= vAngleSpacing; // + instead of - for vert flip problem when lefthand?
		}

		if (verbose_lvl > 0)
			std::cout << "Done. " << std::endl;
		if (verbose_lvl > 2)
			std::cout << "Camera settings: Type PANORAMIC, xFov " << camera->m_xFov << ", yFov " << camera->m_yFov << ", Width " << img->m_width << ", Height " << img->m_height << "." << std::endl;

		return img;
	}

	std::unique_ptr<Image> Renderer::renderToImagePinhole()
	{
		std::shared_ptr<PinholeCamera> camera = std::static_pointer_cast<PinholeCamera>(m_camera);

		std::unique_ptr<Image> img = std::make_unique<Image>(camera->getWidth(), camera->getHeight());

		for (int y = 0; y < camera->getHeight(); y++) {
			for (int x = 0; x < camera->getWidth(); x++) {
				Color c = renderPixel((float)x, (float)y);
				img->set(x, y, c);
			}
		}

		if (verbose_lvl > 0)
			std::cout << "Done. " << std::endl;
		if (verbose_lvl > 2)
			std::cout << "Camera settings: Type PINHOLE, Fov " << camera->getFOV() << ", Width " << img->m_width << ", Height " << img->m_height << "." << std::endl;

		return img;
	}

	// only used by pinhole camera
	Color Renderer::renderPixel(float x, float y)
	{
		std::shared_ptr<PinholeCamera> camera = std::static_pointer_cast<PinholeCamera>(m_camera);


		float p_x = static_cast<float>((2.0f * (x + 0.5f) / camera->getWidth() - 1.0f) * camera->getImageAspectRatio() * camera->getScale());
		float p_y = static_cast<float>((1.0f - 2.0f * (y + 0.5f) / camera->getHeight()) * camera->getScale());

		//float p_x = (2 * (x + 0.5) / (float)camera.m_width - 1) * scale;
		//float p_y = (1 - 2 * (y + 0.5) / (float)camera.m_height) * scale * 1 / imageAspectRatio;

		// apply transformation to point
		// Vec3f ray_p_world = m_camera->m_viewMatrix * Vec3f(p_x, p_y, -1);
		// Vec3f ray_origin_world = m_camera->m_viewMatrix * Vec3f::Zero();
		// Vec3f ray_dir = ray_p_world - ray_origin_world;
		// equivelant to:

		Vec3f ray_dir = m_camera->m_viewMatrix.linear() * Vec3f(p_x, p_y, 1);
		ray_dir.normalize();

		return shootRay(ray_dir);
	}

	Color Renderer::shootRay(const Vec3f &dir)
	{

		Vec3f cam_pos = m_camera->getPosition();

		/* initialize ray */
		RTCRay ray;

		ray.org[0] = cam_pos(0);
		ray.org[1] = cam_pos(1);
		ray.org[2] = cam_pos(2);
		ray.dir[0] = dir(0);
		ray.dir[1] = dir(1);
		ray.dir[2] = dir(2);

		ray.tnear = 0.0f;
		ray.tfar = std::numeric_limits<float>::infinity();
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = -1;
		ray.time = 0;

		/* intersect ray with scene */
		rtcIntersect(m_scene->m_rtcscene, ray);
		/* shade pixels */

		// no Objects hit -> Backgroundcolor
		if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
			return Color(0.5f, 0.5f, 0.5f);


#ifdef MATERIAL_KD_SHADING
		return(Color(m_scene->m_objects[ray.geomID]->texture->Kd));
#endif	

#ifdef UV_SHADING
		return Color(ray.u, ray.v, 1.0f - ray.u - ray.v);
#endif	

		/* Default: texture shading */
		std::shared_ptr<Mesh> mesh = m_scene->m_objects[ray.geomID]; // get the hit object
		Triangle *tri = &mesh->triangles[ray.primID]; // get the hit triangle

		if (mesh->texcoords.size() > 0) // if object has tex coords
		{
			const Vec2f st0 = mesh->texcoords[tri->v0]; // get the texcoordinate for vertex 1 
			const Vec2f st1 = mesh->texcoords[tri->v1]; // get the texcoordinate for vertex 2 
			const Vec2f st2 = mesh->texcoords[tri->v2]; // get the texcoordinate for vertex 3 
			const float u = ray.u, v = ray.v, w = 1.0f - ray.u - ray.v; // w = 1 - u - v
			const Vec2f st = w*st0 + u*st1 + v*st2; // calc texture coordinate of hitpoint

			return mesh->texture->getTexel(st(0), st(1)); // 1.0f - st(1) for lefthanded
		}

		return Color(0.0f, 0.0f, 0.0f); // if nothing hit: black
	}

	std::unique_ptr<Image> Renderer::renderToImageBeeEye()
	{
		std::shared_ptr<BeeEyeCamera> camera = std::static_pointer_cast<BeeEyeCamera>(m_camera);

		// create black image
		std::unique_ptr<Image> img = std::make_unique<Image>(camera->getImageWidth(), camera->getImageHeight());

		//int ommatidiumSize = camera->getOmmatidiumSize();

#ifdef DEBUG
		// write to log
		std::ofstream benchmarkLog;
		benchmarkLog.open("log_table.txt", std::ios_base::app);
		//std::string cam_mode = (camera->m_sampler.getMode() == Sampler::Mode::DISK) ? ", Disk" : ", Square";

		//benchmarkLog << std::endl << "ns: " << std::to_string((camera->m_sampler.getNumSamplePoints() + 1) * camera->m_sampler.getNumSamplePoints()) << ", aa: " << std::to_string(camera->m_sampler.getAcceptanceAngle()) << cam_mode << ", bilinear" << std::endl;
		benchmarkLog << std::to_string((camera->m_sampler.getTotalSamplePoints())) << ",";
#endif

		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		// draw bee eye on image
		renderBeeEye(img, Side::LEFT);

		renderBeeEye(img, Side::RIGHT);

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

#ifdef DEBUG
		benchmarkLog << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
		//benchmarkLog << "Time difference (microseconds) = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl;
		//benchmarkLog << "Time difference (ms) = " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << std::endl;
#endif

		if (verbose_lvl > 0)
			std::cout << "Done." << std::endl;
		if (verbose_lvl > 1)
			std::cout << "Rendering Stats: Time " << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << " ms, Number of Rays cast " << camera->m_sampler.getTotalSamplePoints()*camera->m_rightEye->m_ommatidia.size() * 2 << "." << std::endl;
		if (verbose_lvl > 2)
			std::cout << "Camera settings: Type BEE_EYE, Acceptance angle " << camera->m_sampler.getAcceptanceAngle() << ", Ommatidium size " << camera->getOmmatidiumSize() << ", Num Samples per Ommatidium " << camera->m_sampler.getTotalSamplePoints() << ", Number of Ommatidia " << camera->m_rightEye->m_ommatidia.size() * 2 << ", Width " << img->m_width << ", Height " << img->m_height << "." << std::endl;
		return img;
	}

	void  Renderer::renderAgent(std::vector<float> &out_leftElevation, std::vector<float> &out_leftAzimuth, std::vector<Color> &out_leftColor, std::vector<float> &out_rightElevation, std::vector<float> &out_rightAzimuth, std::vector<Color> &out_rightColor, std::vector<int> &out_x, std::vector<int> &out_y)
	{

		#ifdef DEBUG
			std::cout << "rendering agent..." << std::endl;;
		#endif

		std::unique_ptr<Image> tmp_image = std::unique_ptr<Image>(nullptr);

		renderBeeEye(tmp_image, Side::LEFT, out_leftElevation, out_leftAzimuth, out_leftColor, out_x, out_y, true);
		renderBeeEye(tmp_image, Side::RIGHT, out_rightElevation, out_rightAzimuth, out_rightColor, out_x, out_y, true);

		#ifdef DEBUG
			std::cout << "done rendering agent." << std::endl;;
		#endif
		return;
	}

	void Renderer::renderBeeEye(std::unique_ptr<Image> &img, Side side) {


		std::vector<float> tmp_elevation = std::vector<float>();
        std::vector<float>tmp_azimuth = std::vector<float>();
        std::vector<Color>tmp_color = std::vector<Color>();
        std::vector<int> tmp_x = std::vector<int>();
        std::vector<int>tmp_y = std::vector<int>();

        renderBeeEye(img, side, tmp_elevation, tmp_azimuth, tmp_color, tmp_x, tmp_y, false);
	}


	void Renderer::renderBeeEye(std::unique_ptr<Image> &img, Side side, std::vector<float> &out_elevation, std::vector<float> &out_azimuth, std::vector<Color> &out_color, std::vector<int> &out_x, std::vector<int> &out_y, bool agent)
	{
		#ifdef DEBUG
			std::cout << "rendering bee eye" << std::endl;;
		#endif
		int x;
		int y;

		Vec2f center;

		// need beeeyecamera methods
		std::shared_ptr<BeeEyeCamera> camera = std::static_pointer_cast<BeeEyeCamera>(m_camera);

		int ommatidiumSize = camera->getOmmatidiumSize();

		BeeEye::Ptr beeEye;
		if (side == Side::LEFT)
			beeEye = camera->m_leftEye;
		else
			beeEye = camera->m_rightEye;

		// draw the ommatidia
		for(const auto &ommatidium : beeEye->m_ommatidia)
		{
			// fix for "nice" display at elevation = 0 (model returns too many ommatidia for e=0), delete last ommatidium at e=0
			if (ommatidium.m_y == 0)
			{
				if (beeEye->m_side == Side::RIGHT && (ommatidium.m_x == -5 || ommatidium.m_x == -4))
					continue;
				if (beeEye->m_side == Side::LEFT && (ommatidium.m_x == 5 || ommatidium.m_x == 4))
					continue;
			}

			// get main dir
			Vec3f dir = ommatidium.getDirVector();

			Color color = Color(); // 0,0,0

#ifdef ELEVATION_AZIMUTH_SHADING

			color = azimuthElevationColor(ommatidium.m_azimuth, ommatidium.m_elevation);

#endif

#ifdef XYZ_SHADING

			color = Color(dir(0), dir(1), dir(2));

#endif

#ifdef SOLID_SHADING

			if (ommatidium.m_x == 0 || ommatidium.m_y == 0)
				color = Color(0, 1, 0);
			else
				color = Color(1, 0, 0);

#endif

#ifdef RANDOM_SHADING

			color = randomColor(ommatidium.m_x*ommatidium.m_y);

#endif

#ifdef CHECKER_BOARD_SHADING

			if (ommatidium.m_x % 2 == 0)
				color = Color(0.0f, 0.0f, 0.0f);
			else
				color = Color(1.0f, 1.0f, 1.0f);

			// invert every second row
			if (ommatidium.m_y % 2 == 1)
				if (color.m_r > 0)
					color = Color(0.0f, 0.0f, 0.0f);
				else
					color = Color(1.0f, 1.0f, 1.0f);

#endif

#if defined(SINGLE_RAY_TEXTURE_SHADING) || defined(MATERIAL_KD_SHADING) || defined(UV_SHADING)

			// transform to world coordinates
			Vec3f rayDir = m_camera->m_viewMatrix.linear() * dir;
			rayDir.normalize();

			// shoot ray and store color in array
			color = shootRay(rayDir);

#endif

#ifdef TEXTURE_SHADING

			std::vector<Color> colorSamples;

			// for all samplepoints: shoot ray, get color.
			for (Vec2f &angle : camera->m_sampler.m_samplePoints)
			{
				// get dir
				//Vec3f sampleDir = dir;

				// rotate dir vector bei x degrees to right
				//m_camera->rotateVecY(sampleDir, angle(0));

				// rotate dir vector bei y degrees up
				//m_camera->rotateVecX(sampleDir, angle(1));

				Vec3f sampleDir = ommatidium.getDirVector(angle(0), angle(1));

				// transform to world coordinates
				Vec3f rayDir = m_camera->m_viewMatrix.linear() * sampleDir;
				rayDir.normalize();

				// shoot ray and store color in array
				colorSamples.push_back(shootRay(rayDir));
			}

			color = Color(); // 0,0,0

			// weight each color in colorSamples and add up
			for (int i = 0; i < colorSamples.size(); i++)
			{
				float &w = camera->m_sampler.m_weights[i];
				color.m_r += w * colorSamples[i].m_r;
				color.m_g += w * colorSamples[i].m_g;
				color.m_b += w * colorSamples[i].m_b;
			}
#endif

			if (agent)
			{
				out_color.push_back(color);
				out_azimuth.push_back(ommatidium.m_azimuth);
				out_elevation.push_back(ommatidium.m_elevation);
			}

			// convert the relative coords of ommatidium to image coords (see convert2ImageCoords for details)
			convert2ImageCoords(ommatidium, beeEye, x, y);

			int rel_x = x * ommatidiumSize;
			int rel_y = y * ommatidiumSize;

			if (beeEye->m_side == Side::RIGHT)
			{
				// shift every second row, to simulate hexagonal shape
				if (ommatidiumSize > 1 && y % 2 == 0)
					rel_x += ommatidiumSize / 2;

				// also add offset to right side
				rel_x += (camera->getImageWidth() / 2);

				// space between the two eyes
				rel_x += ommatidiumSize;

				if (ommatidiumSize == 1 && y % 2 == 1)
					rel_x -= 1;
			}
			else // left eye
			{
				if (ommatidiumSize > 1 && y % 2 == 1)
					rel_x += ommatidiumSize / 2;

				if (ommatidiumSize == 1 && y % 2 == 1)
					rel_x += 1;
			}

			if (agent) {
				out_x.push_back(rel_x);
				out_y.push_back(rel_y);
			}
			else {

				/* draw the ommatidium as square */
				drawSquare(img, rel_x, rel_y, ommatidiumSize, color);

				/* for the crosses at center of eye */
				if (ommatidium.m_x == 0 && ommatidium.m_y == 0 && ommatidiumSize > 1)
					center = Vec2f(rel_x + ommatidiumSize / 2, rel_y + ommatidiumSize / 2);
			}
		}

		// draw cross at eye center
		if (!agent && ommatidiumSize > 1)
			drawCross(img, static_cast<int>(floor(center(0))), static_cast<int>(floor(center(1))));

		#ifdef DEBUG
			std::cout << "Done rendering bee eye." << std::endl;;
		#endif
		return;
	}

	/*
	Convert beeeye coordinate to image coordinates

	Bee eye:
		   (+y)
			|
	(-x)----+-----(+x)
			|
		  (-y)

	Image:
	+----------(+x)
	|
	|
	(+y)

	*/
	void Renderer::convert2ImageCoords(const Ommatidium &ommatidium, const BeeEye::Ptr beeEye, int &out_x, int &out_y)
	{
		/*
		convert to:
		(+y)
		|
		|
		+-------(+x)
		*/
		out_x = ommatidium.m_x + abs(beeEye->m_x_min);
		out_y = ommatidium.m_y + abs(beeEye->m_y_min);

		/*
		convert to:
		+-------(+x)
		|
		|
		(+y)
		*/
		out_y = abs(out_y - (beeEye->m_y_max + abs(beeEye->m_y_min)));
		assert(out_x >= 0 && out_y >= 0);
	}

	Color Renderer::randomColor(const int ID)
	{
		int r = ((ID + 13) * 17 * 23) & 255;
		int g = ((ID + 15) * 11 * 13) & 255;
		int b = ((ID + 17) * 7 * 19) & 255;
		const float oneOver255f = 1.f / 255.f;
		return Color(r*oneOver255f, g*oneOver255f, b*oneOver255f);
	}

	Color Renderer::azimuthElevationColor(const int a, const int e)
	{
		return Color((e + 90) / 180.f, 0, (a + 270) / 360.f);
	}

	float Renderer::getDistance(Vec3f pos, Vec3f dir)
	{
		/* initialize ray */
		RTCRay ray;

		dir.normalize();

		ray.org[0] = pos(0);
		ray.org[1] = pos(1);
		ray.org[2] = pos(2);
		ray.dir[0] = dir(0);
		ray.dir[1] = dir(1);
		ray.dir[2] = dir(2);

		ray.tnear = 0.0f;
		ray.tfar = std::numeric_limits<float>::infinity();
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = -1;
		ray.time = 0;

		/* intersect ray with scene */
		rtcIntersect(m_scene->m_rtcscene, ray);
		/* shade pixels */

		// no Objects hit -> -1
		if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
			return -1;
		if (std::isinf(ray.tfar))
			return -1;
		if (ray.tfar < 0)
			return -1;
		return ray.tfar;
	}
}
