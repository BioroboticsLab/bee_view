#pragma once

#include "renderer.h"
#include "obj_loader.h"

namespace BeeView {
	namespace Test {
		void testCamera()
		{
			// load the scene from .obj file
			std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_foward.obj";
			std::shared_ptr<Scene> scene = loadOBJ(file);

			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee-eye-model\\ommatidia.csv";
			beeEye->loadFromCSV(csvfile);

			// setup the camera
			std::shared_ptr<PinholeCamera> camera = std::make_shared<PinholeCamera>(600, 400, 60); //hessen
			camera->moveTo(Vec3f(0, -75, 0)); // default camera looks along negative z
			camera->setDir(Vec3f(0, 0, 1)); // TODO: problem viewmatrix not changed -> not allaw acces to m_dir!!
											// init the renderer
			Renderer renderer = Renderer(scene, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_render.ppm");

			// cleanup embree
			scene->cleanupEmbree();
		}
	}
}