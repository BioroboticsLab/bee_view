#include "../build/BeeView/BeeView/stdafx.h"


#include "texture.h"

namespace BeeView
{

	Texture::Texture() 
	{
		name = "empty Texture";
		img = Image();
	}

	Texture::Texture(std::string fileName)
	{
		img = Image(fileName);
	}

	void Texture::loadImage(std::string fileName)
	{
		img = Image(fileName);
	}

	Color Texture::getTexel(float u, float v)
	{
		//TODO understand
		int iu = (int)floor(u * (float)(img.m_width));
		iu = iu % img.m_width; if (iu < 0) iu += img.m_width;
		int iv = (int)floor(v * (float)(img.m_height));
		iv = iv % img.m_height; if (iv < 0) iv += img.m_height;

		
		return img.m_data[iv*img.m_width + iu];
	}

}
