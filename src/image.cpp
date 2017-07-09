
// Modified by Johannes Polster. TODO: license
// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //


#include "stdafx.h"

#include "image.h"
#include "utility.h"

namespace BeeView {

	extern int verbose_lvl;

	/* for reading ppm file */
	static void skipSpacesAndComments(std::fstream& file)
	{
		while (true)
		{
			if (isspace(file.peek())) {
				file.ignore();
			}
			else  if (file.peek() == '#') {
				std::string line; std::getline(file, line);
			}
			else break;
		}
	}

	Image::Image() {
		m_width = 0;
		m_height = 0;
	}

	Image::Image(std::string file) 
	{
		loadFromPPM(file);
	}

	Image::Image(int width, int height) : m_width(width), m_height(height)
	{
		m_data.resize(width*height);
		for (int i = 0; i < height*width; i++) m_data[i] = Color();
	}

	Image::Image(int width, int height, Color color) : m_width(width), m_height(height)
	{
		m_data.resize(width*height);
		for (int i = 0; i < height*width; i++) m_data[i] = color;
	}

	const Color &Image::getPixel(int x, int y)
	{
		return m_data[m_width * y + x];
	}

	void Image::set(int x, int y, const Color &pixel)
	{
		m_data[y*m_width + x] = pixel;
	}

	/* not working */
	int Image::saveToPNG(std::string fileName)
	{
		/*
		std::vector<float> dataf;
		for (int i=0; i < m_data.size(); i++)
		{
			dataf.push_back(m_data[i].m_r);
			dataf.push_back(m_data[i].m_g);
			dataf.push_back(m_data[i].m_b);
		}
		*/
		//stbi_write_png(fileName.c_str(), m_width, m_height, 3, &m_data[0], 4);
		//stbi_write_bmp(fileName.c_str(), m_width, m_height, 3, &dataf[0]);
		//stbi_write_hdr(fileName.c_str(), m_width, m_height, 3, &dataf[0]);
		return 0;
	}

	/*! read PPM file from disk */
	void Image::loadFromPPM(const std::string fileName)
	{

		std::string ending = fileName.substr(fileName.find_last_of("."),-1);
		if (ending != std::string(".ppm") && ending != std::string(".PPM")) // TODO: pbm, pgm, pnm, pfm, pam
		{
			std::cerr << "Only PPM files supported! (" << ending << "-files are not supported.)" << std::endl;
			m_height = 0;
			m_width = 0;
			return;
		}

		if(verbose_lvl > 0)
			std::cout << std::endl << "\tLoading " << fileName << "... ";

		/* open file for reading */
		std::fstream file;
		file.exceptions(std::fstream::failbit | std::fstream::badbit);
		file.open(fileName.c_str(), std::fstream::in | std::fstream::binary);

		/* read file type */
		char cty[2]; file.read(cty, 2);
		skipSpacesAndComments(file);
		std::string type(cty, 2);

		/* read width, height, and maximal color value */
		file >> m_width;
		skipSpacesAndComments(file);
		file >> m_height;
		skipSpacesAndComments(file);
		int maxColor; file >> maxColor;
		if (maxColor <= 0) std::cerr << "Invalid maxColor value in PPM file";
		float rcpMaxColor = 1.0f / float(maxColor);
		file.ignore(); // skip space or return
		
		m_data.resize(m_width*m_height);

		/* image in text format */
		if (type == "P3")
		{
			int r, g, b;

			for (auto & c : m_data) {
				file >> r; file >> g; file >> b;
				c.set(float(r)*rcpMaxColor, float(g)*rcpMaxColor, float(b)*rcpMaxColor);
			}
		}

		/* image in binary format 8 bit */
		else if (type == "P6" && maxColor <= 255)
		{
			unsigned char rgb[3];
			for (auto & c : m_data) {
				file.read((char*)rgb, sizeof(rgb));
				c.set(float(rgb[0])*rcpMaxColor, float(rgb[1])*rcpMaxColor, float(rgb[2])*rcpMaxColor);
			}
		}

		/* image in binary format 16 bit */
		else if (type == "P6" && maxColor <= 65535)
		{
			unsigned short rgb[3];
			for (auto & c : m_data) {
				file.read((char*)rgb, sizeof(rgb));
				c.set(float(rgb[0])*rcpMaxColor, float(rgb[1])*rcpMaxColor, float(rgb[2])*rcpMaxColor);
			}
		}

		/* invalid magic value */
		else {
			std::cerr << "Invalid magic value in PPM file";
		}

		if(verbose_lvl > 0)
			std::cout << "\tDone." << std::endl;
		if(verbose_lvl > 1)
			std::cout << "\tImage Stats: Width " << m_width << ", Height " << m_height << ", Number of Pixels " << m_data.size() << "." << std::endl;

	}

	/*! store PPM file to disk */
	void Image::saveToPPM(const std::string fileName)
	{

		if(verbose_lvl > 0)
			std::cout << std::endl << "Saving " << fileName << "... ";

		/* open file for writing */
		std::fstream file;
		file.exceptions(std::fstream::failbit | std::fstream::badbit);
		file.open(fileName.c_str(), std::fstream::out | std::fstream::binary);

		/* write file header */
		file << "P6" << std::endl;
		file << m_width << " " << m_height << std::endl;
		file << 255 << std::endl;

		/* write image */
		for (auto const& c : m_data) {
			file << (unsigned char)(clamp(c.m_r)*255.0f);
			file << (unsigned char)(clamp(c.m_g)*255.0f);
			file << (unsigned char)(clamp(c.m_b)*255.0f);
		}

		if(verbose_lvl > 0)
			std::cout << "Done." << std::endl;

	}
}



