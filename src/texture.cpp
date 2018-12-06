#include "stdafx.h"

#include "texture.h"

#define BILINEAR_INTERPOLATION
//#define NEAREST_NEIGHBOUR_INTERPOLATION

namespace BeeView
{

	Texture::Texture() : Image()
	{
		name = "No Name";
	}

	Texture::Texture(std::string fileName) : Image(fileName) { }

	void Texture::loadImage(std::string fileName)
	{
		loadFromPPM(fileName);
	}

	Color Texture::getTexel(float u, float v)
	{
#ifdef NEAREST_NEIGHBOUR_INTERPOLATION
		/* nearest neighbour */
		// u and v are [0..1]
		// get integer part for u
		int iu = (int)floor(u * (float)(m_width));

		// fix negative texture coordinates -> wrap around
		iu = iu % m_width; if (iu < 0) iu += m_width;

		// same for v
		int iv = (int)floor(v * (float)(m_height));
		iv = iv % m_height; if (iv < 0) iv += m_height;

		return m_data[iv*m_width + iu];
#endif

#ifdef BILINEAR_INTERPOLATION

		/* bilinear interpolation */
		float x = u * (float)(m_width);
		float y = v * (float)(m_height);

		int x0 = (int)floor(u * (float)(m_width));
		int y0 = (int)floor(v * (float)(m_height));

		// prevent overflow
		x0 = clamp(x0, 0, m_width - 1);
		y0 = clamp(y0, 0, m_height - 1);
		int x1 = clamp(x0 + 1, 0, m_width - 1);
		int y1 = clamp(y0 + 1, 0, m_height - 1);

		// get 2x2 nearest pixels around u,v
		Color c00 = m_data[y0*m_width + x0];
		Color c10 = m_data[y0*m_width + x1];
		Color c01 = m_data[y1*m_width + x0];
		Color c11 = m_data[y1*m_width + x1];

		// weight the colors
		float w00 = ((float)x1 - x) * ((float)y1 - y);
		float w01 = ((float)x1 - x) * (y - (float)y0);
		float w10 = (x - (float)x0) * ((float)y1 - y);
		float w11 = (x - (float)x0) * (y - (float)y0);

		Color result = Color(w00*c00.toVec3f() + w01*c01.toVec3f() + w10*c10.toVec3f() + w11*c11.toVec3f());
        return result;
#endif	


	}

}
