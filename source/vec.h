#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip> 
#include <math.h> 
/*
Simple vec3 implementation, no fancy stuff
Taken from www.scratchapixel.com
TODO:  license
*/

namespace BeeView
{
	template<typename T> __forceinline T clamp(const T& x, const T& lower = T(0), const T& upper = T(1)) { return std::max(std::min(x, upper), lower); }


	template<typename T>
	class Vec3
	{
	public:
		Vec3() : x(0), y(0), z(0) {}
		Vec3(T xx) : x(xx), y(xx), z(xx) {}
		Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
		Vec3 operator + (const Vec3 &v) const
		{
			return Vec3(x + v.x, y + v.y, z + v.z);
		}
		Vec3 operator - (const Vec3 &v) const
		{
			return Vec3(x - v.x, y - v.y, z - v.z);
		}
		Vec3 operator * (const T &r) const
		{
			return Vec3(x * r, y * r, z * r);
		}
		T dotProduct(const Vec3<T> &v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}
		T crossProduct(const Vec3<T> &v) const
		{
			return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}
		T norm() const
		{
			return x * x + y * y + z * z;
		}
		T length() const
		{
			return sqrt(norm());
		}
		const T& operator [] (uint8_t i) const { return (&x)[i]; }
		T& operator [] (uint8_t i) { return (&x)[i]; }
		Vec3& normalize()
		{
			T n = norm();
			if (n > 0) {
				T factor = 1 / sqrt(n);
				x *= factor, y *= factor, z *= factor;
			}

			return *this;
		}

		friend std::ostream& operator << (std::ostream &s, const Vec3<T> &v)
		{
			return s << '(' << v.x << ' ' << v.y << ' ' << v.z << ')';
		}

		T x, y, z;
	};

	typedef Vec3<float> Vec3f;
	typedef Vec3<int> Vec3i;

	template<typename T>
	Vec3<T> sphericalToCartesian(const T &theta, const T &phi)
	{
		return Vec3<T>(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
	};


	template<typename T>
	class Vec2
	{
	public:
		Vec2() : x(0), y(0) {}
		Vec2(T xx) : x(xx), y(xx) {}
		Vec2(T xx, T yy) : x(xx), y(yy) {}
		Vec2 operator + (const Vec2 &v) const
		{
			return Vec3(x + v.x, y + v.y);
		}
		Vec2 operator - (const Vec2 &v) const
		{
			return Vec3(x - v.x, y - v.y);
		}
		Vec2 operator * (const T &r) const
		{
			return Vec2(x * r, y * r);
		}
		T dotProduct(const Vec2<T> &v) const
		{
			return x * v.x + y * v.y;
		}
		T norm() const
		{
			return x * x + y * y;
		}
		T length() const
		{
			return sqrt(norm());
		}
		const T& operator [] (uint8_t i) const { return (&x)[i]; }
		T& operator [] (uint8_t i) { return (&x)[i]; }
		Vec2& normalize()
		{
			T n = norm();
			if (n > 0) {
				T factor = 1 / sqrt(n);
				x *= factor, y *= factor;
			}

			return *this;
		}

		friend std::ostream& operator << (std::ostream &s, const Vec2<T> &v)
		{
			return s << '(' << v.x << ' ' << v.y << ')';
		}

		T x, y;
	};

	typedef Vec2<float> Vec2f;
	typedef Vec2<int> Vec2i;

}
