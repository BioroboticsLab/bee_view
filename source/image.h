#pragma once

#include <vector>
#include "utility.h"

namespace BeeView {

	/* RGB Color class, values expected to be between 0 and 1 */
	class Color
	{
	public:
		float m_r, m_g, m_b;
		Color() : m_r(0), m_g(0), m_b(0) {	}
		Color(float c) : m_r(c), m_g(c), m_b(c) {	}
		Color(float r, float g, float b) : m_r(r), m_g(g), m_b(b) {	}
		Color(Vec3f vec): m_r(vec(0)), m_g(vec(1)), m_b(vec(2)) { }

		/* changes the color values */
		void set(float r, float g, float b)	{ m_r = r; m_g = g; m_b = b; }
	};

	/* simple image class */
	class Image
	{
	public:
		/* width of the image */
		int m_width;

		/* height of the image */
		int m_height;

		/* continouse array of color data, use m_data[y*width+x] to select pixel at x,y */
		std::vector<Color> m_data;

		/* creates image of height 0 and width 0 */
		Image();

		/* load image from ppm file */
		Image(std::string file);
		
		/* black image */
		Image(int width, int height);
		
		/* image of uniform color */
		Image(int width, int height, Color color);
		
		/* get color at positon x,y */
		const Color &getPixel(int x, int y);

		/* set x,y to color */
		void set(int x, int y, const Color &pixel);
		
		/* not implemented */
		int saveToPNG(const std::string fileName);

		/* save image to ppm file */
		void saveToPPM(const std::string fileName);

		/* overwrites image with data in ppm file */
		void loadFromPPM(const std::string fileName);
		
	};
}