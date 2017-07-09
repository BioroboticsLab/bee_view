#pragma once

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

		int m_zone; // for sorting. sort zone, elevation, azimuth

		// bee eye coordinate system. center is (0,0)
		int m_x;
		int m_y;

		Ommatidium() : m_azimuth(0), m_elevation(0) {}
		Ommatidium(float azimuth, float elevation) : m_azimuth(azimuth), m_elevation(elevation) {}

		/* return the Vec3f direction vector, calculated from elevation and azimuth */
		Vec3f getDirVector() const;

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

		typedef std::shared_ptr<BeeEye> Ptr;

		/* sorted array of ommatidia (sortkey 0: sortkey 1: elevation, sortkey 2: azimuth)*/
		std::vector<Ommatidium> m_ommatidia;

		/* LEFT or RIGHT */
		Side m_side;

		// max expanse of bee eye coordinate system, gets set when csv is loaded
		int m_max_x = 0; 
		int m_min_x = -0;
		int m_max_y = 0;
		int m_min_y = -0;

		/* creates the right bee eye. use createOtherEye() to construct the left eye. */
		BeeEye() : m_side(Side::RIGHT) 	{}

		/*
		create ommatidialarray from csv file.
		Expects csv file to have 2 columns, first column is azimuth angle, second column is elevation angle.
		one row for each ommatidium.
		comma seperated, no quotation marks, no header, no spaces, only floats (no scientific notation e.g. 2.4e04, minus sign must be minus sign)
		Example:
		0,0.5
		0.1,4
		-34,3
		-43,210.5
		*/
		void loadFromCSV(std::string fileName);

		/* creates the other eye and computes the complementary angles for each ommatidium */
		BeeEye createOtherEye() const;

	};
}