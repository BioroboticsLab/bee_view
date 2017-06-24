#pragma once
#include <iomanip>


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

		void test_gauss_sampler()
		{
			int version = 3;
			Image img = Image(21, 21);
			float sum = 0;
			for (int i = -img.m_width/2; i < img.m_width/2; i++)
			{
				for (int j = -img.m_width/2; j < img.m_height/2; j++)
				{
					float w = Renderer::gaussPDF(version,i, j, 2.6);
					sum += w;
					std::cout << std::fixed << std::setprecision(5) << w << "\t";
					img.set(i + img.m_width/2, j + img.m_width/2, Color(w/ Renderer::gaussPDF(version,0, 0, 2.6), 0, 0));
				}
				std::cout << std::endl;
			}

			std::cout << sum << std::endl;
				
					//img.set(i, j, Color(Renderer::gaussPDF(i,j,img.m_width/2)*20,0,0));

			img.saveToPPM("test_gauss.ppm");				
		}
	}
}