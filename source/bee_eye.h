#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include "utility.h"

namespace BeeView
{

	enum class	Side
	{
		LEFT,
		RIGHT
	};

	class Ommatidium
	{
	public:
		/* azimuth angle in degrees, range (right eye): -90 - 270 */
		float m_azimuth;

		/* elevation angle, range (right eye): -90 - 90 */
		float m_elevation;

		float m_acceptance_angle;

		int m_zone; // for sorting. sort zone, elevation, azimuth

		// bee eye coordinate system. center is (0,0), also is direction of eye
		int m_x;
		int m_y;

		Ommatidium() : m_azimuth(0), m_elevation(0), m_acceptance_angle(2.6f) {}
		Ommatidium(float azimuth, float elevation) : m_azimuth(azimuth), m_elevation(elevation), m_acceptance_angle(2.6f) {}
		Ommatidium(float azimuth, float elevation, float acceptance_angle) : m_azimuth(azimuth), m_elevation(elevation), m_acceptance_angle(acceptance_angle) {}

		/* return the direction vector for ray tracing, calculated from elevation and azimuth */
		Vec3f getDirVector() const
		{
			// make elvation in range 0 - 180
			float elevation2 = 90 - m_elevation;

			// make azimuth in range 0 - 360
			float azimuth2 = m_azimuth;

			if (azimuth2 < 0)
				azimuth2 = 360 + azimuth2;

			azimuth2 = deg2rad(azimuth2);
			elevation2 = deg2rad(elevation2);

			return Vec3f(sin(azimuth2) * sin(elevation2), cos(elevation2), cos(azimuth2) * sin(elevation2)).normalized();
		}

		/*
		caution: doesn't sort as one might expect: negative values are sorted descendingly, positive values ascendingly!
		*/
		bool operator< (const Ommatidium& rhs) {

			return std::make_tuple(m_zone, abs(m_elevation), abs(m_azimuth)) < std::make_tuple(rhs.m_zone, abs(rhs.m_elevation), abs(rhs.m_azimuth));
		}

		bool operator== (const Ommatidium& rhs) {
			return m_zone == rhs.m_zone && m_elevation == rhs.m_elevation && m_azimuth == rhs.m_azimuth;
		}
	};

	class BeeEye
	{
	public:

		typedef std::shared_ptr<BeeEye> Ptr; // convenience

		/* sorted array of ommatidia (sortkey 0: sortkey 1: elevation, sortkey 2: azimuth)*/
		std::vector<Ommatidium> m_ommatidia;

		/* LEFT or RIGHT */
		Side m_side;

		// max expanse of bee eye coordinate system, gets set when csv is loaded
		int m_max_x = 0; //TODO: 
		int m_min_x = -0;
		int m_max_y = 0;
		int m_min_y = -0;

		BeeEye() : m_side(Side::RIGHT) {} // TODO: is empty beeeye acceptable?
		BeeEye(Side side) : m_side(side) {}

		/*
		create ommatidialarray from csv file.
		expects csv file to have 2 columns, first column is azimuth angle, second column is elevation angle.
		one row for each ommatidium.
		comma seperated, no quotation marks, no header, no spaces, only floats (no scientific notation e.g. 2.4e04, minus sign must be minus sign)
		Example:
		0,0.5
		0.1,4
		-34,3
		-43,210.5

		step 1: create array of ommatidia
		step 2: remove duplicates (operator == for ommatidium?)
		step 3: sort: elevation, azimuth
		*/
		void loadFromCSV(std::string fileName);

		/* creates the other eye and computes the complementary angles for each ommatidium */
		BeeEye createOtherEye() const;

	};
}