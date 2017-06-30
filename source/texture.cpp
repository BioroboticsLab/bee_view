#include "../build/BeeView/BeeView/stdafx.h"

#include "texture.h"

namespace BeeView
{

	Texture::Texture() : Image()
	{
		name = "empty Texture";
	}

	Texture::Texture(std::string fileName) : Image(fileName) { }

	void Texture::loadImage(std::string fileName)
	{
		loadFromPPM(fileName);
	}

	Color Texture::getTexel(float u, float v)
	{
		//TODO understand
		int iu = (int)floor(u * (float)(m_width));
		iu = iu % m_width; if (iu < 0) iu += m_width;
		int iv = (int)floor(v * (float)(m_height));
		iv = iv % m_height; if (iv < 0) iv += m_height;

		
		return m_data[iv*m_width + iu];
	}

}
