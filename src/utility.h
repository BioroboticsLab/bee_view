#pragma once
#include "stdafx.h"

/*
This File contains small general purpose functions and classes.
*/

#ifndef M_PI
#define M_PI 3.1415926535f
#endif

namespace BeeView
{
	typedef Eigen::Vector3f Vec3f;
	typedef Eigen::Vector2f Vec2f;

	template<typename T> __forceinline T clamp(const T& x, const T& lower = T(0), const T& upper = T(1)) { return std::max(std::min(x, upper), lower); }
	
	inline float deg2rad(const float &deg)	{ return deg * (float)M_PI / 180.0f; }

	inline bool validate(Vec3f vec)
	{
		return !(std::isnan(vec(0)) || std::isinf(vec(0)) || std::isnan(vec(1)) || std::isinf(vec(1)) || std::isnan(vec(2)) || std::isinf(vec(2)));

	}

	/* C:\\Users\\Documents\\file.txt -> return C:\\Users\\Documents\\,
	 * home/user/Docements/file.txt -> return home/user/Docements/ */
	inline std::string getFilePath(std::string fullPath)
	{
		if (fullPath.find_last_of("\\/") != -1) {
			return fullPath.substr(0, fullPath.find_last_of("\\/")+1);
		}
		return "";
	}

	/* returns vector of rows of floats */
	inline std::vector<std::vector<float>> readFloatsFromCSV(std::string fileName)
	{
		std::vector<std::vector<float>> ret;

			/* open file */
			std::ifstream  data;
			data.open(fileName.c_str());
			if (!data.is_open()) {
				std::cerr << "cannot open " + fileName;
				return ret;
			}

			std::string line;


			/* load the rows */
			while (std::getline(data, line))
			{
				std::stringstream lineStream(line);
				std::string token;

				std::vector<float> row;

				/* split line by comma and extract floats */
				while (std::getline(lineStream, token, ','))
				{
					row.push_back(std::stof(token));
				}
				ret.push_back(row);
			}
			return ret;
	}

	/* writes contents of vec to a .h and .cpp file */
	inline void vec2hFile(std::vector<std::vector<float>> vec, std::string vectorName)
	{
		std::ofstream hFile;

		hFile.open(vectorName + ".h");
		hFile << "#pragma once" << std::endl
			<< "#include <vector>" << std::endl
			<< "extern std::vector<std::vector<float>> " << vectorName << ";" << std::endl;

		std::ofstream cppFile;

		cppFile.open(vectorName + ".cpp");
		cppFile << "#include \"" << vectorName << ".h\"" << std::endl;
		cppFile << vectorName << " = {" << std::endl;

		for (std::vector<float> &row : vec)
		{
							
			cppFile << "{";
			for (float &f : row)
			{
				cppFile << f;
				if (&f != &row.back())
					cppFile << ", ";
			}

			if (&row != &vec.back())
				cppFile << "}," << std::endl;
			else
				cppFile << "}" << std::endl << "};" << std::endl;
		}
		return;
	}

	inline Vec3f sphericalToCartesian(const float &theta, const float &phi)
	{
		float theta2 = (M_PI / 2.0f) - theta;
		return Vec3f(cos(phi) * sin(theta2), sin(phi) * sin(theta2), cos(theta2));
	}

	/* RNG: https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c */
	static unsigned long rng_x = 123456789, rng_y = 362436069, rng_z = 521288629;

	/* Marsaglia xorshift generator */
	inline unsigned long xorshf96(void) {          //period 2^96-1
		unsigned long t;
		rng_x ^= rng_x << 16;
		rng_x ^= rng_x >> 5;
		rng_x ^= rng_x << 1;

		t = rng_x;
		rng_x = rng_y;
		rng_y = rng_z;
		rng_z = t ^ rng_x ^ rng_y;

		return rng_z;
	}

	/* return [0..1] */
	inline float randf()
	{
		return (float)xorshf96() / std::numeric_limits<unsigned long>::max();
	}

	/* return [-1..1] */
	inline float randfu()
	{
		return (float)xorshf96() / ((~0UL >> 1)*1.0f) - 1.0f;
	}

	/* scatter plot to txt for displaying it externaly (e.g. with R). x and y must be same size */
	inline void plot2txt(const std::vector<float> &x, const std::vector<float> &y, std::string txtFile)
	{
		if (x.size() != y.size())
		{
			std::cerr << "vectors x and y must be same size!";
			return;
		}

		std::ofstream outFile(txtFile);

		for (int i = 0; i < x.size(); i++)
		{
			outFile << x[i] << "," << y[i] << "\n";
		}

		outFile.close();
		return;
	}

	/* plot points */
	inline void plot2txt(const std::vector<Vec2f> &vec, std::string txtFile)
	{

		std::ofstream outFile(txtFile);

		for (int i = 0; i < vec.size(); i++)
		{
			outFile << vec[i](0) << "," << vec[i](1) << "\n";
		}

		outFile.close();
		return;
	}

	/* plot points with data, TODO should be generic */
	inline void plot2txt(const std::vector<Vec2f> &vec, std::vector<float> data, std::string txtFile)
	{
		if (vec.size() != data.size())
		{
			std::cerr << "vectors must be same size!";
			return;
		}

		std::ofstream outFile(txtFile);

		for (int i = 0; i < vec.size(); i++)
		{
			outFile << vec[i](0) << "," << vec[i](1) << "," << data[i] << "\n";
		}

		outFile.close();
		return;
	}

	/* normalize a single float to be in range a..b */
	inline void normalize(float &x, float a, float b, float min_x, float max_x)
	{
		x = (b - a)*(x - min_x) / (max_x - min_x) + a;
	}

	/* normalize point to be in range a..b */
	inline void normalizePoint(Vec2f &point, float a, float b, float min_x, float max_x)
	{
		normalize(point(0), a, b, min_x, max_x);
		normalize(point(1), a, b, min_x, max_x);
	}

	// normalize all x in vec to be in range a..b
	inline void normalize(std::vector<float> &vec, float a, float b)
	{
		float min_x = *std::min_element(vec.begin(), vec.end());
		float max_x = *std::max_element(vec.begin(), vec.end());

		std::cout << "min: " << min_x << " max: " << max_x << std::endl;
		for (int i = 0; i < vec.size(); i++)
		{
			normalize(vec[i], a, b, min_x, max_x);
		}
	}

	
}