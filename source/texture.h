#pragma once 

#include "image.h"
#include "utility.h"

namespace BeeView
{
	/* wrapper class for image, with extra functions, also used as material class */
	class Texture : public Image
	{

	public:

		/* Ambient and difuse values */
		Vec3f Ka = Vec3f(1, 1, 1);
		Vec3f Kd = Vec3f(1, 1, 1);
		std::string name;

		Texture();
		Texture(std::string filename);

		// get color at point u,v (nearest neighbour)
		Color getTexel(float u, float v);

		/* load image into img */
		void loadImage(std::string fileName);

	};

}