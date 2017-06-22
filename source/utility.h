#pragma once

/*
This File contains small general purpose functions and classes.
*/

#include <eigen\Eigen\Dense>
#include <math.h>

#define M_PI 3.14159265

namespace BeeView
{
	typedef Eigen::Vector3f Vec3f;
	typedef Eigen::Vector2f Vec2f;

	template<typename T> __forceinline T clamp(const T& x, const T& lower = T(0), const T& upper = T(1)) { return std::max(std::min(x, upper), lower); }
	
	inline
		float deg2rad(const float &deg)
	{
		return deg * M_PI / 180.0f;
	}

	/* C:\\Users\\Documents\\file.txt -> return C:\\Users\\Documents\\, home/user/Docements/file.txt -> return home/user/Docements/ */
	inline std::string getFilePath(std::string fullPath)
	{
		if (fullPath.find_last_of("\\/") != -1) {
			return fullPath.substr(0, fullPath.find_last_of("\\/")+1);
		}
		return "";
	}

	inline Vec3f sphericalToCartesian(const float &theta, const float &phi)
	{
		float theta2 = (M_PI / 2) - theta;
		return Vec3f(cos(phi) * sin(theta2), sin(phi) * sin(theta2), cos(theta2));
	}
}