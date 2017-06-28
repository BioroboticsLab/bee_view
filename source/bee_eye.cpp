#include "../build/BeeView/BeeView/stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "bee_eye.h"
#include "utility.h"

namespace BeeView
{


	void BeeEye::loadFromCSV(std::string fileName)
	{

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
		int size_before = m_ommatidia.size();
		m_ommatidia.erase(std::unique(m_ommatidia.begin(), m_ommatidia.end()), m_ommatidia.end());
		int size_after = m_ommatidia.size();
		int nr_duplicates = size_before - size_after;

		std::cout << "Removed " << nr_duplicates << " duplicates form Ommatidial array." << std::endl;
	
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
#if 0
				// also fix "feathered edge, bad solution, since some ommatidia will be overwritten" 
				if (cur_zone == 3)
					if (y % 2 == 0 && i > 0)
						m_ommatidia[i - 1].m_y = -y + 1;
				if (cur_zone == 4)
					if (y % 2 == 0 && i > 0)
						m_ommatidia[i - 1].m_y = y - 1;
#endif
				
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

		// don't need fix, because code above was bugged: now corrected, fix edge obsolete
#if 0
		// fix edge by going through rows and checking size, not optimized for efficiency ;-)
		std::vector<Ommatidium> allOmmatidia = m_ommatidia;

		// sort array only bei elevation (not zone)
		std::sort(allOmmatidia.begin(), allOmmatidia.end(),
			[](const Ommatidium &a, const Ommatidium &b) -> bool
		{
			return  std::make_tuple(a.m_elevation,a.m_azimuth) < std::make_tuple(b.m_elevation, b.m_azimuth);
		});
		
		// for fixing edge: create rows
		std::vector<std::vector<Ommatidium>> rows; // index rows = y
		std::vector<Ommatidium> row;

		int curY = allOmmatidia[0].m_y;
		for (Ommatidium &o : allOmmatidia)
		{
			if (curY != o.m_y)
			{
				
				// add row to rows
				rows.push_back(row);
				curY = o.m_y;

				//new row
				row = std::vector<Ommatidium>();
			}
			row.push_back(o);
		}
		//add last row
		rows.push_back(row);
		
		// if row has more members than both neighmours -> add first ommatidium to row above
		for (int i = 1; i < rows.size()-1; i++)
		{
			std::vector<Ommatidium> &prevRow = rows[i-1];
			std::vector<Ommatidium> &curRow = rows[i];
			std::vector<Ommatidium> &nextRow = rows[i+1];

			if (curRow.size() > prevRow.size() && curRow.size() > nextRow.size())
			{
				// put first ommatidium in row above
				curRow[0].m_y += 1;
				curRow[0].m_x = nextRow[0].m_x - 1;
			}
		}

		// put the fixed ommatidia in m_ommatidia
		m_ommatidia = std::vector<Ommatidium>();
		
		for (auto oRow : rows)
		{
			for (auto o : oRow)
			{
				m_ommatidia.push_back(o);
			}
		}
			
#endif
		// sort ommatidia by zone, elevation, azimuth (overwritten < operator in ommatidium)
		//std::sort(m_ommatidia.begin(), m_ommatidia.end());

		// save min max of coordinate expansion:
		auto minmax_x = std::minmax_element(m_ommatidia.begin(), m_ommatidia.end(),
			[](Ommatidium const& lhs, Ommatidium const& rhs) {return lhs.m_x < rhs.m_x; });
		auto minmax_y = std::minmax_element(m_ommatidia.begin(), m_ommatidia.end(),
			[](Ommatidium const& lhs, Ommatidium const& rhs) {return lhs.m_y < rhs.m_y; });

		/*auto min_it = std::min_element(m_ommatidia.begin(),  m_ommatidia.end(), 
		[](Ommatidium const& lhs, Ommatidium const& rhs) {return lhs.m_x < rhs.m_x; });*/

		m_min_x = minmax_x.first->m_x;
		m_max_x = minmax_x.second->m_x;
		m_min_y = minmax_y.first->m_y;
		m_max_y = minmax_y.second->m_y;


		std::cout << "Minimum x: " << m_min_x << std::endl;
		std::cout << "Maximum x: " << m_max_x << std::endl;

		std::cout << "Minimum y: " << m_min_y << std::endl;
		std::cout << "Maximum y: " << m_max_y << std::endl;

	}

	/* creates the other eye and computes the complementary angles for each ommatidium */
	BeeEye BeeEye::createOtherEye() const
	{
		BeeEye otherEye = BeeEye();
		otherEye.m_side = Side(!static_cast<int>(m_side)); // flip sides
		for each (const auto &my_o in m_ommatidia)
		{
			Ommatidium other_o = Ommatidium(-my_o.m_azimuth, my_o.m_elevation, my_o.m_acceptance_angle);
			//TODO:  recalc acceptance angle (or Ommatidium.getDynAcceptanceAngle(), because aa is function of elevation?)

			other_o.m_x = -my_o.m_x;
			other_o.m_y = my_o.m_y;

			otherEye.m_ommatidia.push_back(other_o);
			//TODO:  sort?
		}
		otherEye.m_max_x = abs(m_min_x);
		otherEye.m_max_y = m_max_y;
		otherEye.m_min_x = -m_max_x;
		otherEye.m_min_y = m_min_y;

		return otherEye;
	}
}
