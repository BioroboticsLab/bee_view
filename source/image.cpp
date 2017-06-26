
// Modified by Johannes Polster. TODO license
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


#include "../build/BeeView/BeeView/stdafx.h"

#include <algorithm> 
#include <memory> // for std::unique_ptr
#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>

#include "image.h"
//#include "vec.h"
#include "utility.h"

namespace BeeView {

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
		// TODO load image
		/*
		int channels_in_file;
		float *dataf;
		std::cout << "Beginn loading image: " << file << std::endl;
		dataf = stbi_loadf(file.c_str(), &m_width, &m_height, &channels_in_file, 3);
		std::cout << "Finished loading" << std::endl << "image width: " << m_width << "image height: " << m_height << std::endl;

		
		for (int i = 0; i < m_width*m_height * 3; i += 3) {
			m_data.push_back(Color(dataf[i], dataf[i + 1], dataf[i + 2]));
		}

		std::cout << m_data.size() / m_width << " " << m_data.size() / m_height;
		*/

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
		std::cout << "Image Loader: Loading " << fileName << std::endl;

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

		std::cout << "Image Loader: Finished loading. w: " << m_width << ", h: " << m_height << ", len: " << m_data.size() << std::endl;

	}

	/*! store PPM file to disk */
	void Image::saveToPPM(const std::string fileName)
	{

		std::cout << "Writing image to " << fileName << std::endl;

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

		std::cout << "Success: " << fileName << std::endl;

	}
}



