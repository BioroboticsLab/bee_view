#pragma once 

#include <vector>
#include <string>

#include "image.h"
#include "utility.h"

namespace BeeView
{

	// image, material, texture class in one
	class Texture
	{
	private:
		Image img;

	public:
		Texture();
		Texture(std::string filename);

		// get color at point u,v (nearest neighbour)
		Color getTexel(float u, float v);

		/* load image into img */
		void loadImage(std::string fileName);

		/* Ambient and difuse values are multiplied with each Color by renderer */
		Vec3f Ka = Vec3f(1, 1, 1);
		Vec3f Kd = Vec3f(1, 1, 1);
		std::string name;


	};

}