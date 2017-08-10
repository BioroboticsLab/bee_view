#pragma once
#include <iomanip>

#include "renderer.h"
#include "obj_loader.h"
#include "sampler.h"
#include "../python/beeview_api.h"

namespace BeeView {
	namespace Test {

		void testPanoramicCamera()
		{
			// load the scene from .obj file
			std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj"; //\\cornell\\cornell_box.obj";// 
			std::shared_ptr<Scene> scene = loadOBJ(file);

			// setup the camera
			std::shared_ptr<PanoramicCamera> camera = std::make_shared<PanoramicCamera>(1500,270.0f,170.0f);			

			camera->setPosition(Vec3f(0, -70, 0));

			Vec3f dir = Vec3f(0, 0, 1).normalized();
			camera->setDir(dir);

			Renderer renderer = Renderer(scene, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_panoramic.ppm");

			scene->cleanupEmbree();

		}

		// for shading where the scene doesnt have to be loaded, use #define ... in renderer to define shading type
		void testCameraNoScene()
		{
			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee_eye_model\\ommatidia_linear2.csv";

			beeEye->loadFromCSV(csvfile);
			
			// setup the camera
			std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);

			Renderer renderer = Renderer(nullptr, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			//img->saveToPPM("test_ommatidia_distribution.ppm");
			img->saveToPPM("test_checker_board.ppm");
		}

		// test the pinhole camera and the coordinates (move camera around)
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
			std::shared_ptr<PinholeCamera> camera = std::make_shared<PinholeCamera>(600, 400, 60.0f); 
			//std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);

			//camera->setPosition(Vec3f(278, 273, -800)); // cornell box defalut
			camera->setPosition(Vec3f(0, -70, 0));

			Vec3f dir = Vec3f(0, 0, 1).normalized();
			camera->setDir(dir);

			Renderer renderer = Renderer(scene, camera);

			// render the image
			std::unique_ptr<Image> img = renderer.renderToImage();
			img->saveToPPM("test_cam12.ppm");

			// test directions: move along neg z
			camera->setPosition(Vec3f(0, -70, -40));
			
			img = renderer.renderToImage();
			img->saveToPPM("test_cam22_m_negz.ppm");

			// move along pos x
			camera->setPosition(Vec3f(40, -70, -40));

			img = renderer.renderToImage();
			img->saveToPPM("test_cam32_m_x.ppm");

			//test rotation x down
			camera->rotateVecX(dir, 20);
			camera->setDir(dir);
			//camera->rotateUp(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam42_r_downx.ppm");		

			// rotate back up
			camera->rotateVecX(dir, -20);
			camera->setDir(dir);
			//camera->rotateUp(-20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam52_r_upx.ppm");

			// rotate to left
			camera->rotateVecY(dir, 20);
			camera->setDir(dir);
			//camera->rotateRight(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam62_r_lefty.ppm");

			// roll around z
			camera->rotateVecZ(dir, 10);
			camera->setDir(dir);
			//camera->rollRight(20);
			img = renderer.renderToImage();
			img->saveToPPM("test_cam72_r_leftz.ppm");

			// cleanup embree
			scene->cleanupEmbree();
		}

		// test the rotation of vectors around camera axis
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

		// test the beeeye camera
		void testBeeEyeCam()
		{
			// load the scene from .obj file
			//std::string file = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\models\\hessen\\skydome_minus_z_forward.obj"; //\\cornell\\cornell_box.obj";// 
			std::string file = "D:\\Documents\\bachelorarbeit\\bee_view\\data\\sky_white\\skydome_white.obj";

			std::shared_ptr<Scene> scene = loadOBJ(file);

			// load the ommatidial array from csv file
			BeeEye::Ptr beeEye = std::make_shared<BeeEye>();
			std::string csvfile = "D:\\Documents\\bachelorarbeit\\bee_eye_model\\ommatidia.csv";
			beeEye->loadFromCSV(csvfile);

			// setup the camera
			std::shared_ptr<BeeEyeCamera> camera = std::make_shared<BeeEyeCamera>(beeEye);
			camera->setPosition(Vec3f(0, -70, 0));
			Vec3f dir = Vec3f(0, 0, 1).normalized();
			camera->setDir(dir);

			std::unique_ptr<Image> img = std::make_unique<Image>(); // or null_ptr?

			// first 1 panoramic image 
			/*
			std::shared_ptr<PanoramicCamera> pano_camera = std::make_shared<PanoramicCamera>();
			pano_camera->setPosition(Vec3f(0, -70, 0));
			pano_camera->setDir(dir);

			Renderer renderer = Renderer(scene, pano_camera);
			img = renderer.renderToImage();
			img->saveToPPM("test_panoramic.ppm");

			renderer.setCamera(camera);
			*/

			Renderer renderer = Renderer(scene, camera);
#if 1
			camera->m_sampler.setAcceptanceAngle(2.6f);
			camera->m_sampler.setMode(Sampler::Mode::DISK);

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(7);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s7_a26.ppm");

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(11);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a26.ppm");

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(21);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s21_a26.ppm");

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(11);
			camera->m_sampler.setAcceptanceAngle(5.2f);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a52.ppm");

			// render the image
			camera->m_sampler.setAcceptanceAngle(1.3f);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_s11_a13.ppm");


			camera->m_sampler.setAcceptanceAngle(2.6f);
			camera->m_sampler.setMode(Sampler::Mode::SQUARE);

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(7);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_square_s7_a26.ppm");

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(11);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_square_s11_a26.ppm");

			// render the image
			camera->m_sampler.setSqrtNumSamplePoints(21);
			img = renderer.renderToImage();
			img->saveToPPM("test_beeEye_square_s21_a26.ppm");
#endif
			// cleanup embree
			scene->cleanupEmbree();

		}

		void test_gauss_sampler()
		{

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


		// testing of different disk sampling methods :

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

			double phi, r = 1.0;

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
			// if (abs(p(0)) < abs(p(1))) // last case
				return Vec2f(
				(2 * p(1)) / sqrt(M_PI) * sin((p(0)*M_PI) / (4 * p(1))),
					(2 * p(1)) / sqrt(M_PI) * cos((p(0)*M_PI) / (4 * p(1)))
				);
		}

		// test the sampling methods and plot with weights (txt: in R)
		void test_sampler2()
		{
			// create the data for the plots

			Sampler sampler = Sampler();
			float acceptanceAngle = 2.6f;
			int sqrtSamples = 0;
			std::vector<Vec2f> points;
			std::vector<float> weights;
			std::string fileName;

			// 462 samples for square
			sqrtSamples = 21;
			points = sampler.squareSamples(sqrtSamples);
			// also make square in range -2.6:2.6
			for (Vec2f &p : points) 
				p = acceptanceAngle*p;
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_square_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			// 132 samples for square
			sqrtSamples = 11;
			points = sampler.squareSamples(sqrtSamples);
			// also make square in range -2.6:2.6
			for (Vec2f &p : points)
				p = acceptanceAngle*p;
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_square_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			// 56 samples for square
			sqrtSamples = 7;
			points = sampler.squareSamples(sqrtSamples);
			// also make square in range -2.6:2.6
			for (Vec2f &p : points)
				p = acceptanceAngle*p;
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_square_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			// 462 samples for disk
			sqrtSamples = 21;
			points = sampler.concentricDiskSamples(sqrtSamples,acceptanceAngle);
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			// 132 samples for disk
			sqrtSamples = 11;
			points = sampler.concentricDiskSamples(sqrtSamples, acceptanceAngle);
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			// 56 samples for disk
			sqrtSamples = 7;
			points = sampler.concentricDiskSamples(sqrtSamples, acceptanceAngle);
			weights = sampler.computeWeightVector(points, acceptanceAngle);
			fileName = "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\sample_" + std::to_string(points.size()) + "_weights.txt";
			plot2txt(points, weights, fileName);

			return;
		}

		/* maps to rainbow color, max =  maximum value, input = input value
		* https://stackoverflow.com/questions/37876316/map-value-range-to-rainbow-colormap
		*/
		Color getRainbowColor(float max, float input)
		{
			float inc = 6.0f / input;
			float x = max * inc;

			float r = 0.0f;
			float g = 0.0f;
			float b = 0.0f;
			if ((0 <= x && x <= 1) || (5 <= x && x <= 6)) r = 1.0f;
			else if (4 <= x && x <= 5) r = x - 4;
			else if (1 <= x && x <= 2) r = 1.0f - (x - 1);
			if (1 <= x && x <= 3) g = 1.0f;
			else if (0 <= x && x <= 1) g = x - 0;
			else if (3 <= x && x <= 4) g = 1.0f - (x - 3);
			if (3 <= x && x <= 5) b = 1.0f;
			else if (2 <= x && x <= 3) b = x - 2;
			else if (5 <= x && x <= 6) b = 1.0f - (x - 5);

			return Color(r, g, b);
		}

		void test_gaussian()
		{
			Sampler sampler = Sampler();

			Image img = Image(500, 500);

			
			std::vector<Vec2f> samples = sampler.squareSamples(499);
			//std::vector<Vec2f> samples = sampler.concentricDiskSamples(499,250);
			for (Vec2f &p : samples)
			{
				p(0) = p(0) * 249;
				p(1) = p(1) * 249;
			}


			std::cout << " test1 ";
			std::vector<float> weights = sampler.computeWeightVector(samples,180);
			std::cout << " test2 ";

			normalize(weights, 0, 1);
			
			int i = 0;
			for (Vec2f &p : samples)
			{

				img.set(static_cast<int>(floor(p(0) + 250)), static_cast<int>(floor(p(1) + 250)), getRainbowColor(weights[i], 1.0)); //Color(weights[i],0,0));// getRainbowColor(1000,weights[i]*1000)); //
				//std::cout << floor(p(0) * 249 + 250) << " " << floor(p(1) * 249 + 250) << std::endl;
				//std::cout << weights[i] << std::endl;
				i++;
			}

			img.saveToPPM("test_gauss4.ppm");
		}

		typedef std::vector<std::vector<std::vector<float>>> PyImage;

		void testApi()
		{
			BeeViewApplication beeView = BeeViewApplication("D:\\Documents\\bachelorarbeit\\bee_view\\data\\sky_white\\skydome_white.obj", "D:\\Documents\\bachelorarbeit\\bee_view\\data\\ommatidia.csv", true);
			
			//beeView.setRenderModePinhole();
			//beeView.setRenderModePanoramic();

			beeView.setCameraPosition(0,0,0);
			float h = beeView.getDistance(0,0,0,0,-1,0);
			beeView.setCameraPosition(0.0f, (0.0f-h)+15.0f, 0.0f); // 5m above ground

			beeView.setCameraDirVector(0.0f, 0.0f, -1.0f);

			beeView.setBeeEyeCameraNumSamplePoints(132);

			// convert python image to my image format
			PyImage img = beeView.render();
			int width = static_cast<int>(img[0].size());
			int height = static_cast<int>(img.size());
			Image c_img = Image(width, height);
			for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				Color color = Color(img[y][x][0], img[y][x][1], img[y][x][2]);
				c_img.set(x, y, color);
			}
			c_img.saveToPPM("bee_eye_nn.ppm");
			return;
		}

		void testGimbalLock()
		{
			BeeViewApplication beeView = BeeViewApplication("D:\\Documents\\bachelorarbeit\\bee_view\\data\\sky_white\\skydome_white.obj", "D:\\Documents\\bachelorarbeit\\bee_view\\data\\ommatidia.csv", true);

			

			beeView.setCameraPosition(0, 0, 0);
			float h = beeView.getDistance(0, 0, 0, 0, -1, 0);
			beeView.setCameraPosition(0.0f, (0.0f - h) + 15.0f, 0.0f); // 15m above ground

			beeView.setCameraDirVector(0.1f, 0.0f, 0.0f);


			std::shared_ptr<PanoramicCamera> camera = std::make_shared<PanoramicCamera>();
			beeView.m_renderer.setCamera(camera);

			camera->setDir(Vec3f(1, 0, 1));
			camera->m_xFov = 200;
			std::unique_ptr<Image> img = beeView.m_renderer.renderToImage();
			img->saveToPPM("test_gimbal_1.ppm");

			camera->setDir(Vec3f(0, 0, 1));
			img = beeView.m_renderer.renderToImage();
			img->saveToPPM("test_gimbal_2.ppm");


			camera->setDir(Vec3f(0, 0, -1));
			img = beeView.m_renderer.renderToImage();
			img->saveToPPM("test_gimbal_3.ppm");


			camera->setDir(Vec3f(-1, 0, 0));
			img = beeView.m_renderer.renderToImage();

			img->saveToPPM("test_gimbal_4.ppm");
		}

	}
}