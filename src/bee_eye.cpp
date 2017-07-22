#include "stdafx.h"

#include "bee_eye.h"

namespace BeeView
{
	extern int verbose_lvl;

	/* return the Vec3f direction vector, calculated from elevation and azimuth */
	Vec3f Ommatidium::getDirVector() const
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

	Vec3f Ommatidium::getDirVector(float offset_a, float offset_e) const
	{
		// make elvation in range 0 - 180
		float elevation2 = 90 - m_elevation + offset_e;

		// make azimuth in range 0 - 360
		float azimuth2 = m_azimuth;

		if (azimuth2 < 0)
			azimuth2 = 360 + azimuth2;

		azimuth2 += offset_a;
		azimuth2 = deg2rad(azimuth2);
		elevation2 = deg2rad(elevation2);

		return Vec3f(sin(azimuth2) * sin(elevation2), cos(elevation2), cos(azimuth2) * sin(elevation2)).normalized();
	}

	void BeeEye::loadFromCSV(std::string fileName)
	{
		if (verbose_lvl > 0)
			std::cout << std::endl << "Loading elevation and azimuth angles from " << fileName << "... ";

		/* open file */
		std::ifstream  data;
		data.open(fileName.c_str());
		if (!data.is_open()) {
			std::cerr << "cannot open " + fileName;
			return;
		}

		std::string line;

		/*
		zones:
		1: 0 <= azimuth <= 270, 0 <= elevation <= 90; +x, +y
		2: 0 <= azimuth <= 270, 0 >= elevation >= -90; +x, -y
		3: 0 >= azimuth <= -90, 0 >= elevation >= -90; -x, -y
		4: 0 >= azimuth <= -90, 0 <= elevation <= 90; -x, +y
		*/

		/* load the ommatidia */
		while (std::getline(data, line))
		{
			std::stringstream lineStream(line);
			std::string token;

			Ommatidium ommatidium;

			std::vector<float> xy; // x,y pair

			/* split line by comma and extract x,y */
			while (std::getline(lineStream, token, ','))
			{
				xy.push_back(std::stof(token));
			}

			ommatidium.m_azimuth = xy[0];
			ommatidium.m_elevation = xy[1];

			/* assign zone */
			if (ommatidium.m_azimuth <= 0 && ommatidium.m_elevation >= 0) ommatidium.m_zone = 4;
			if (ommatidium.m_azimuth <= 0 && ommatidium.m_elevation <= 0) ommatidium.m_zone = 3;
			if (ommatidium.m_azimuth >= 0 && ommatidium.m_elevation <= 0) ommatidium.m_zone = 2;
			if (ommatidium.m_azimuth >= 0 && ommatidium.m_elevation >= 0) ommatidium.m_zone = 1;
			// -> 0,0 zone 1

			m_ommatidia.push_back(ommatidium);

		}

		// sort ommatidia by zone, elevation, azimuth (overwritten < operator in ommatidium)
		std::sort(m_ommatidia.begin(), m_ommatidia.end());

		// remove duplicates
		size_t size_before = m_ommatidia.size();
		m_ommatidia.erase(std::unique(m_ommatidia.begin(), m_ommatidia.end()), m_ommatidia.end());
		size_t size_after = m_ommatidia.size();
		size_t nr_duplicates = size_before - size_after;

		if(verbose_lvl > 2)
			std::cout << std::endl << "Removed " << nr_duplicates << " duplicates form Ommatidial array." << std::endl;
	
		float cur_elevation = m_ommatidia[0].m_elevation;
		int cur_zone = 1;

		int x = 0;
		int y = 0;

		int sign_x = 1;
		int sign_y = 1;

		for (int i = 0; i < m_ommatidia.size(); i++)
		{
			Ommatidium &ommatidium = m_ommatidia[i];

			// create "rows": every time elevation changes -> new row!
			if (cur_elevation != ommatidium.m_elevation)
			{
				if (cur_zone == 1 || cur_zone == 2) x = 0;
				if (cur_zone == 3 || cur_zone == 4) x = 1;
				cur_elevation = ommatidium.m_elevation;
				
				++y;
			}

			// if zone changes, start at center again
			if (ommatidium.m_zone != cur_zone)
			{

				cur_zone = ommatidium.m_zone;
				if (cur_zone == 1) { x = 0; y = 0; } // y 0
				if (cur_zone == 2) { x = 0; y = 1; } // y -1
				if (cur_zone == 3) { x = 1; y = 0; } // y -1
				if (cur_zone == 4) { x = 1; y = 1; } // y 0

			}

			assert(ommatidium.m_zone == cur_zone);

			// fix sign
			if (cur_zone == 1) { sign_x = 1; sign_y = 1; }
			if (cur_zone == 2) { sign_x = 1; sign_y = -1; }
			if (cur_zone == 3) { sign_x = -1; sign_y = -1; }
			if (cur_zone == 4) { sign_x = -1; sign_y = 1; }

			ommatidium.m_x = sign_x*x;
			ommatidium.m_y = sign_y*y;

			++x;
#if 0
			std::cout <<
				"zone: " << ommatidium.m_zone <<
				", azimuth: " << ommatidium.m_azimuth <<
				", elevation: " << ommatidium.m_elevation <<
				", x: " << ommatidium.m_x <<
				", y: " << ommatidium.m_y <<
				std::endl;
#endif

		}

		// save min max of coordinate expansion:
		auto minmax_x = std::minmax_element(m_ommatidia.begin(), m_ommatidia.end(),
			[](Ommatidium const& lhs, Ommatidium const& rhs) {return lhs.m_x < rhs.m_x; });
		auto minmax_y = std::minmax_element(m_ommatidia.begin(), m_ommatidia.end(),
			[](Ommatidium const& lhs, Ommatidium const& rhs) {return lhs.m_y < rhs.m_y; });

		m_x_min = minmax_x.first->m_x;
		m_x_max = minmax_x.second->m_x;
		m_y_min = minmax_y.first->m_y;
		m_y_max = minmax_y.second->m_y;

		if (verbose_lvl > 2)
		{
			std::cout << "Minimum x: " << m_x_min << std::endl;
			std::cout << "Maximum x: " << m_x_max << std::endl;

			std::cout << "Minimum y: " << m_y_min << std::endl;
			std::cout << "Maximum y: " << m_y_max << std::endl;
		}

		if (verbose_lvl > 0)
			std::cout << "Done." << std::endl;
		if (verbose_lvl > 1)
			std::cout << "Number of ommatidia per Eye: " << std::to_string(m_ommatidia.size()) << std::endl;

		return;
	}

	/* creates the other eye and computes the complementary angles for each ommatidium */
	BeeEye BeeEye::createOtherEye() const
	{
		BeeEye otherEye = BeeEye();
		otherEye.m_side = Side(!static_cast<int>(m_side)); // flip sides
		for each (const auto &my_o in m_ommatidia)
		{
			Ommatidium other_o = Ommatidium(-my_o.m_azimuth, my_o.m_elevation);

			other_o.m_x = -my_o.m_x;
			other_o.m_y = my_o.m_y;

			otherEye.m_ommatidia.push_back(other_o);
			//TODO:  sort?
		}
		otherEye.m_x_max = abs(m_x_min);
		otherEye.m_y_max = m_y_max;
		otherEye.m_x_min = -m_x_max;
		otherEye.m_y_min = m_y_min;

		return otherEye;
	}
}
