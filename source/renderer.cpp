#include "../build/BeeView/BeeView/stdafx.h"

#include <iostream>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <memory>

#include <eigen\Eigen\Dense>

#include "renderer.h"


namespace BeeView {

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

	Color Renderer::shootRay(const Vec3f &dir)
	{

		Vec3f cam_pos = m_camera->m_viewMatrix.translation();
		

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
#if 1
		/* shade pixels */
		if (ray.geomID == RTC_INVALID_GEOMETRY_ID) {
			//std::cout << "Ray missed!" << std::endl;
			return Color(0.5f, 0.5f, 0.5f);
		}

		// Material Kd shading
		 //return(Color(m_scene->m_objects[ray.geomID]->texture->Kd));
		/* texture shading */
		std::shared_ptr<Mesh> mesh = m_scene->m_objects[ray.geomID]; // get the hit object
		Triangle *tri = &mesh->triangles[ray.primID]; // get the hit triangle

		if (mesh->texcoords.size() > 0) // if object has tex coords
		{
			const Vec2f st0 = mesh->texcoords[tri->v0]; // get the texcoordinate for vertex 1 
			const Vec2f st1 = mesh->texcoords[tri->v1]; // get the texcoordinate for vertex 2 
			const Vec2f st2 = mesh->texcoords[tri->v2]; // get the texcoordinate for vertex 3 
			const float u = ray.u, v = ray.v, w = 1.0f - ray.u - ray.v; // w = 1 - u - v
			const Vec2f st = w*st0 + u*st1 + v*st2; // linearly interpolate the texture coordinates

			return mesh->texture->getTexel(st(0), 1.0f - st(1));
		}


		// uv shading
		// return Color(ray.u, ray.v, 1.0f - ray.u - ray.v);

#endif

		// visualise ray dir as color
		Vec3f ray_color = (dir + Vec3f(1, 1, 1)) * 0.5;

		return Color(ray_color(0), ray_color(1), ray_color(2));
	}

	Color Renderer::renderPixel(float x, float y, const RenderPixelParams &params)
	{

#if 0
		/* https://stackoverflow.com/questions/14094042/calculating-camera-ray-direction-to-3d-world-pixel */
		/* compute ray directions */

		Vec3f upVector = Vec3f(0, 0, 1); // no rolling
		Vec3f leftVector = upVector.cross(camera.m_dir); //vec_x
		Vec3f vec_y = camera.m_dir.cross(leftVector);

		float aspect = static_cast<float>(camera.m_height) / camera.m_width;

		float y_fov = aspect * camera.m_fov * 0.5; //phi


		Vec3f u = tan(deg2rad(camera.m_fov * 0.5)) * leftVector; // caution: tan 90 not defined
		Vec3f v = tan(deg2rad(y_fov)) * vec_y;

		float rel_x = static_cast<float>(2 * (x + 0.5)) / camera.m_width - 1;
		float rel_y = 1 - static_cast<float>(2 * (y + 0.5)) / camera.m_height;


		Vec3f p = rel_x * u + rel_y * v;
		Vec3f ray_dir = p; // (p - camera.m_position).normalized(); ray dir doesnt care about camera position?
#else // 1

		float p_x = (2 * (x + 0.5) / params.width - 1) * params.imageAspectRatio * params.scale;
		float p_y = (1 - 2 * (y + 0.5) / params.height) * params.scale;

		//float p_x = (2 * (x + 0.5) / (float)camera.m_width - 1) * scale;
		//float p_y = (1 - 2 * (y + 0.5) / (float)camera.m_height) * scale * 1 / imageAspectRatio;

		// apply transformation to point
		//Vec3f ray_p_world = m_camera->m_viewMatrix * Vec3f(p_x, p_y, -1);
		//Vec3f ray_origin_world = m_camera->m_viewMatrix * Vec3f::Zero();
		//Vec3f ray_dir = ray_p_world - ray_origin_world;
		// equivelant to:
		Vec3f ray_dir = m_camera->m_viewMatrix.linear() * Vec3f(p_x, p_y, 1);
		ray_dir.normalize();
#endif
		return shootRay(ray_dir);
	}

	std::unique_ptr<Image> Renderer::renderToImagePinhole()
	{
		std::cout << "Start rendering Image... ";
		std::shared_ptr<PinholeCamera> camera = std::static_pointer_cast<PinholeCamera>(m_camera);

		std::unique_ptr<Image> img = std::make_unique<Image>(camera->m_width, camera->m_height);

		// setup constant params for renderPixel()
		RenderPixelParams params;
		params.width = (float)camera->m_width;
		params.height = (float)camera->m_height;
		params.imageAspectRatio = camera->m_width / (float)camera->m_height;
		params.scale = tan(deg2rad(camera->m_fov * 0.5));

		for (int y = 0; y < camera->m_height; y++) {
			for (int x = 0; x < camera->m_width; x++) {
				Color c = renderPixel((float)x, (float)y, params);
				img->set(x, y, c);
			}
		}

		std::cout << "Done" << std::endl;
		return img;
	}

	std::unique_ptr<Image> Renderer::renderToImage()
	{
		if (m_camera->m_type == Camera::Type::PINHOLE)
			return renderToImagePinhole();
		else if (m_camera->m_type == Camera::Type::BEE_EYE)
			return renderToImageBeeEye();
		else
			std::cerr << "Renderer: Camera Type not supported. " << std::endl;
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
	void Renderer::convert2ImageCoords(const Ommatidium &ommatidium, const ConvertCoordsParams &params, int &out_x, int &out_y)
	{
		/*
		convert to:
		(+y)
		|
		|
		+-------(+x)
		*/
		out_x = ommatidium.m_x + abs(params.x_min);
		out_y = ommatidium.m_y + abs(params.y_min);

		/*
		convert to:
		+-------(+x)
		|
		|
		(+y)
		*/
		out_y = abs(out_y - (params.y_max + abs(params.y_min)));
		assert(out_x >= 0 && out_y >= 0);
	}

	std::unique_ptr<Image> Renderer::renderToImageBeeEye()
	{
		std::shared_ptr<BeeEyeCamera> camera = std::static_pointer_cast<BeeEyeCamera>(m_camera);

		// get image dimensions
		int x_dim = (camera->m_leftEye->m_max_x + abs(camera->m_leftEye->m_min_x) + 1) * camera->m_ommatidium_size;
		int y_dim = (camera->m_leftEye->m_max_y + abs(camera->m_leftEye->m_min_y) + 1)*camera->m_ommatidium_size;

		// add to x_dim because of ommatidia shift
		x_dim += camera->m_ommatidium_size / 2;

		// 2 eyes
		x_dim = x_dim * 2;

		// space between eyes
		x_dim += 2 * camera->m_ommatidium_size;

		// create black image
		std::unique_ptr<Image> img = std::make_unique<Image>(x_dim, y_dim);

		// setup sampler
		Sampler sampler = Sampler(camera->m_sqrtNumSamplePoints,camera->m_acceptanceAngle); // TODO: where set acceptanceangle and num_samplepoints per ommatidium?

		// draw bee eye on image
		drawBeeEye(img, camera->m_leftEye, sampler);
		drawBeeEye(img, camera->m_rightEye, sampler);

		return img;
	}

	void Renderer::drawBeeEye(std::unique_ptr<Image> &img,  BeeEye::Ptr &beeEye, Sampler &sampler)
	{
		int x;
		int y;

		Vec2f center;

		ConvertCoordsParams params;
		params.x_min = beeEye->m_min_x; //TODO refactor inconsistent order, should be beeEye.m_x_min
		params.x_max = beeEye->m_max_x;
		params.y_min = beeEye->m_min_y;
		params.y_max = beeEye->m_max_y;

		// draw the ommatidia
		for each (const auto &ommatidium in beeEye->m_ommatidia)
		{
			// get main dir
			Vec3f dir = ommatidium.getDirVector();

			// visualize elevation and azimuth
			//Color color = azimuthElevationColor(ommatidium.m_azimuth, ommatidium.m_elevation);

			// visualize x,y,z of dir vector
			Color color = Color(dir(0),dir(1),dir(2));

			// random shading
			//Color color = randomColor(ommatidium.m_x*ommatidium.m_y);
			//std::cout << x_out << ", " << y_out << std::endl;
#if 0
			/* texture shading */

			std::vector<Color> colorSamples;

			// for all samplepoints: shoot ray, get color
			for (Vec2f &dev : sampler.m_samplePoints)
			{
				// make copy of dir
				Vec3f sampleDir = dir;

				// rotate dir vector bei x degrees to right
				m_camera->rotateVecY(sampleDir, dev(0));

				// rotate dir vector bei y degrees up
				m_camera->rotateVecX(sampleDir, dev(1));

				// transform to world coordinates
				Vec3f rayDir = m_camera->m_viewMatrix.linear() * sampleDir;
				rayDir.normalize();

				// shoot ray and store color in array
				colorSamples.push_back(shootRay(rayDir));
			}

			Color color = Color(); // 0,0,0

			// weight each color in colorSamples and add up
			for (int i = 0; i < colorSamples.size(); i++)
			{
				float &w = sampler.m_weights[i];
				color.m_r += w * colorSamples[i].m_r;
				color.m_g += w * colorSamples[i].m_g;
				color.m_b += w * colorSamples[i].m_b;
			}
#endif
			// need beeeyecamera methods
			std::shared_ptr<BeeEyeCamera> camera = std::static_pointer_cast<BeeEyeCamera>(m_camera);

			// convert the relative coords of ommatidium to image coords (see convert2ImageCoords for details)
			convert2ImageCoords(ommatidium, params, x, y);

			int rel_x = x * camera->m_ommatidium_size;
			int rel_y = y * camera->m_ommatidium_size;

			// shift every second row, to simulate hexagonal shape // TODO fix "feathered" edge
			if (beeEye->m_side == Side::RIGHT)
			{
				if (y % 2 == 0)
					rel_x += camera->m_ommatidium_size / 2;

				// also add offset to right side
				rel_x += (img->m_width / 2);

				// space between the two eyes
				rel_x += camera->m_ommatidium_size;
			}
			else // left eye
			{
				if (y % 2 == 1)
					rel_x += camera->m_ommatidium_size / 2;
			}

			/* draw the ommatidium as square */
			drawSquare(img, rel_x, rel_y, camera->m_ommatidium_size, color);
			
			/* for the crosses at center of eye */
			if (ommatidium.m_x == 0 && ommatidium.m_y == 0)
				center = Vec2f(rel_x, rel_y);

		}

		// draw cross at eye center
		drawCross(img, center(0), center(1));

		return;
	}
}
