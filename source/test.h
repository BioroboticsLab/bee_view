#pragma once
#include <iomanip>


#include "renderer.h"
#include "obj_loader.h"
#include "sampler.h"

namespace BeeView {
	namespace Test {

		void testCameraNoScene()
		{
			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee-eye-model\\ommatidia.csv";
			beeEye->loadFromCSV(csvfile);

			// setup the camera
			std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);

			Renderer renderer = Renderer(nullptr, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_ommatidia_shift.ppm");
		}

		void testCamera()
		{
			// load the scene from .obj file
			std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj"; //\\cornell\\cornell_box.obj";// 
			std::shared_ptr<Scene> scene = loadOBJ(file);

			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee-eye-model\\ommatidia.csv";
			beeEye->loadFromCSV(csvfile);

			// setup the camera
			std::shared_ptr<PinholeCamera> camera = std::make_shared<PinholeCamera>(600, 400, 60); 
			//std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);

			//camera->moveTo(Vec3f(278, 273, -800)); // cornell box defalut
			camera->moveTo(Vec3f(0, -70, 0));

			Vec3f dir = Vec3f(0, 0, 1).normalized();
			camera->setDir(dir);

			Renderer renderer = Renderer(scene, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_cam12.ppm");

			// test directions: move along neg z
			camera->moveTo(Vec3f(0, -70, -40));
			
			img = renderer.renderToImage();
			img->saveToPPM("test_cam22_m_negz.ppm");

			// move along pos x
			camera->moveTo(Vec3f(40, -70, -40));

			img = renderer.renderToImage();
			img->saveToPPM("test_cam32_m_x.ppm");

			//test rotation x down
			camera->rotateVecX(dir, 20);
			camera->setDir(dir);
			//camera->rotateX(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam42_r_downx.ppm");

		

			// rotate back up
			camera->rotateVecX(dir, -20);
			camera->setDir(dir);
			//camera->rotateX(-20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam52_r_upx.ppm");

			// rotate to left
			camera->rotateVecY(dir, 20);
			camera->setDir(dir);
			//camera->rotateY(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam62_r_lefty.ppm");

			// roll around z
			camera->rotateVecZ(dir, 10);
			camera->setDir(dir);
			//camera->rotateZ(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam72_r_leftz.ppm");

			// cleanup embree
			scene->cleanupEmbree();
		}

		void testCamera2()
		{
			Camera cam = Camera();

			// make camera coordinate system aligned with wordle coordinate system
			cam.setDir(Vec3f(0, 0, 1));
			std::cout << cam.m_viewMatrix.matrix();
			std::cout << std::endl << std::endl;

			Vec3f testVec = Vec3f(0, 0, 1);

			// rotate to right, pos x and z: expect 1,0,1 norm
			cam.rotateVecY(testVec, 45);

			std::cout << testVec;
			std::cout << std::endl << std::endl;

			// rotate to left, neg x, pos z: expect -1,0,1 norm
			cam.rotateVecY(testVec, -90);

			std::cout << testVec;
			std::cout << std::endl << std::endl;

			// rotate back to 0,0,1
			cam.rotateVecY(testVec, 45);

			std::cout << testVec;
			std::cout << std::endl << std::endl;

			// 0,1,1 norm
			cam.rotateVecX(testVec, 45);

			std::cout << testVec;
			std::cout << std::endl << std::endl;

			// 0,-1,1 norm
			cam.rotateVecX(testVec, -90);

			std::cout << testVec;
			std::cout << std::endl << std::endl;

			// 1,-1,1 norm
			cam.rotateVecY(testVec, 45);

			std::cout << testVec;
			std::cout << std::endl << std::endl;
		}

		void testCamera3()
		{
			// load the scene from .obj file
			std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj"; //\\cornell\\cornell_box.obj";// 
			std::shared_ptr<Scene> scene = loadOBJ(file);

			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee-eye-model\\ommatidia.csv";
			beeEye->loadFromCSV(csvfile);

			// setup the camera
			std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);

			camera->m_acceptanceAngle = 2.6;
			camera->m_sqrtNumSamplePoints = 11;
		
			camera->moveTo(Vec3f(0, -70, 0));

			Vec3f dir = Vec3f(0, 0, 1).normalized();
			camera->setDir(dir);

			Renderer renderer = Renderer(scene, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a26.ppm");

			// render the image
			camera->m_sqrtNumSamplePoints = 21;
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s21_a26.ppm");


			// render the image
			camera->m_sqrtNumSamplePoints = 11;
			camera->m_acceptanceAngle = 5.2;
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a52.ppm");

			// render the image
			camera->m_acceptanceAngle = 1.3;
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a13.ppm");

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
					float w = 0; // TODO Renderer::gaussPDF(version,i, j, 2.6);
					sum += w;
					std::cout << std::fixed << std::setprecision(5) << w << "\t";
					img.set(i + img.m_width / 2, j + img.m_width / 2, Color(0));//w / Renderer::gaussPDF(version,0, 0, 2.6), 0, 0));
				}
				std::cout << std::endl;
			}

			std::cout << sum << std::endl;
				
					//img.set(i, j, Color(Renderer::gaussPDF(i,j,img.m_width/2)*20,0,0));

			img.saveToPPM("test_gauss.ppm");				
		}

		

		void test_rng()
		{
			std::vector<float> test_rng;
			for (int i = 0; i < 1000; i++)
				test_rng.push_back(randfu());

			double sum = std::accumulate(test_rng.begin(), test_rng.end(), 0.0);
			double mean = sum / test_rng.size();

			std::cout << "RNG mean: " << mean << std::endl;
			std::cout << "RNG max: " << *std::max_element(test_rng.begin(), test_rng.end()) << std::endl;
			std::cout << "RNG min: " << *std::min_element(test_rng.begin(), test_rng.end()) << std::endl;
		}


		// samplers:


		/* http://psgraphics.blogspot.de/2011/01/improved-code-for-concentric-map.html v2*/

		Vec2f ToUnitDisk(Vec2f O) {
			if (O == Vec2f(0, 0))
				return O;
			float phi, r;
			float a = 2 * O(0) - 1;
			float b = 2 * O(1) - 1;
			if (a*a > b*b) { // use squares instead of absolute values
				r = a;
				phi = (M_PI / 4)*(b / a);
			}
			else {
				r = b;
				phi = (M_PI / 4)*(a / b) + (M_PI / 2);
			}
			return Vec2f(r*cos(phi), r*sin(phi));
		}

		/*  http://psgraphics.blogspot.de/2011/01/improved-code-for-concentric-map.html v1 */
		Vec2f to_unit_disk(Vec2f in)
		{
			float seedx = in(0);
			float seedy = in(1);

			double phi, r;

			double a = 2 * seedx - 1; /* (a,b) is now on [-1,1]^2 */
			double b = 2 * seedy - 1;

			if (seedx == 0 && seedy == 0)
				return Vec2f(0, 0);

			if (a > -b) { /* region 1 or 2 */
				if (a > b) { /* region 1, also |a| > |b| */
					r = a;
					phi = (M_PI / 4) * (b / a);
				}
				else { /* region 2, also |b| > |a| */
					phi = (M_PI / 4) * (2 - (a / b));
				}
			}
			else { /* region 3 or 4 */
				if (a < b) { /* region 3, also |a| >= |b|, a != 0 */
					r = -a;
					phi = (M_PI / 4) * (4 + (b / a));
				}
				else { /* region 4, |b| >= |a|, but a==0 and b==0 could occur. */
					r = -b;
					if (b != 0)
						phi = (M_PI / 4) * (6 - (a / b));
					else
						phi = 0;
				}
			}
			return Vec2f(r * cos(phi), r * sin(phi));
		}



		/* uniform -> not uniform ;-) */
		Vec2f UniformSampleDisk(const Vec2f &u) {
			float r = std::sqrt(u(0));
			float theta = 2 * M_PI * u(1);
			return Vec2f(r * std::cos(theta), r * std::sin(theta));
		}


		/* pbrt2 v3 : BAD */
		Vec2f ConcentricSampleDisk(const Vec2f &u) {
			// Map uniform random numbers to $[-1,1]^2$
			Vec2f uOffset = 2.f * u - Vec2f(1, 1);

			// Handle degeneracy at the origin
			if (uOffset(0) == 0 && uOffset(1) == 0) return Vec2f(0, 0);

			// Apply concentric mapping to point
			float theta, r;
			if (std::abs(uOffset(0)) > std::abs(uOffset(1))) {
				r = uOffset(0);
				theta = (M_PI / 4) * (uOffset(1) / uOffset(0));
			}
			else {
				r = uOffset(1);
				theta = (M_PI / 4) - (M_PI / 4) * (uOffset(0) / uOffset(1));
			}
			return r * Vec2f(std::cos(theta), std::sin(theta));
		}

		/* https://www.aanda.org/articles/aa/pdf/2010/12/aa15278-10.pdf BEST */
		Vec2f sampleDisk(Vec2f p)
		{
			if (p(0) == 0 && p(1) == 0)
				return p;
			if (abs(p(1)) <= abs(p(0)))
				return Vec2f(
				(2 * p(0)) / sqrt(M_PI) * cos((p(1)*M_PI) / (4 * p(0))),
					(2 * p(0)) / sqrt(M_PI) * sin((p(1)*M_PI) / (4 * p(0)))
				);
			if (abs(p(0)) < abs(p(1)))
				return Vec2f(
				(2 * p(1)) / sqrt(M_PI) * sin((p(0)*M_PI) / (4 * p(1))),
					(2 * p(1)) / sqrt(M_PI) * cos((p(0)*M_PI) / (4 * p(1)))
				);
		}

		void test_sampler2()
		{
			Sampler sampler = Sampler();
			//std::vector<Vec2f> points = sampler.concentricDiskSamples(21, 2.6);
			std::vector<Vec2f> points = sampler.squareSamples(21);

			std::vector<float> weights = sampler.computeWeightVector(points, 2.6);

			float sum_weights = std::accumulate(weights.begin(), weights.end(), 0.0f);

			std::cout << "sum weights: " << sum_weights << std::endl;

			plot2txt(points, weights,"D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_weights.txt");
		}

		void test_sampler()
		{
			// test concentric map:
			// create evenly spaced points in -2.6:2.6
			// numpoints
			int n = 11; // sqrt numpoints, should be odd
			float x = 0;
			float y = 0;
			
			float range = 1.f;
			float spacing = 2*(range / n) + 2* range/(n*n); // for square range -1..1

			std::vector<float> x_grid;
			std::vector<float> y_grid;
			std::vector<float> x_circle;
			std::vector<float> y_circle;

			for (int i = -n / 2; i <= (n + 1) / 2; i++)
			{
				for (int j = -n / 2; j <= (n) / 2; j++)
				{
					x_grid.push_back(x);
					y_grid.push_back(y);

					//Vec2f p = ToUnitDisk(Vec2f(x, y));
					Vec2f p = sampleDisk(Vec2f(x, y));
					x_circle.push_back(p(0));
					y_circle.push_back(p(1));

					//std::cout << x << " " << y << ", ";
					x = j*spacing;
				}
				y = i*spacing;
				//std::cout << std::endl;
			}

			normalize(x_circle, -2.6, 2.6);
			normalize(y_circle, -2.6, 2.6);

			plot2txt(x_grid, y_grid, "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\plot_grid.txt");
			plot2txt(x_circle, y_circle, "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\plot_circle.txt");
		}
	}
}