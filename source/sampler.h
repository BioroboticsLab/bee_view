#pragma once

#include <vector>
#include <algorithm>

#include "utility.h"

namespace BeeView
{

	class Sampler
	{

	public:

		enum class Mode
		{
			DISK,
			SQUARE
		};

		std::vector<Vec2f> m_samplePoints; 
		std::vector<float> m_weights;
		Sampler(){}
		
		Sampler(int numSamples, float acceptanceAngle) 
		{
			if (numSamples > 1)
			{
				m_samplePoints = concentricDiskSamples(numSamples, acceptanceAngle);
				m_weights = computeWeightVector(m_samplePoints, acceptanceAngle);
			}
		}

		Sampler(int numSamples, float acceptanceAngle, Mode mode)
		{
			if (numSamples > 1)
			{
				if (mode == Mode::DISK)
				{
					m_samplePoints = concentricDiskSamples(numSamples, acceptanceAngle);
				}
				else if(mode == Mode::SQUARE)
				{
					m_samplePoints = squareSamples(numSamples);
					for (Vec2f &p : m_samplePoints) // also make square in range -aa:aa
						p = acceptanceAngle*p;
				}
				m_weights = computeWeightVector(m_samplePoints, acceptanceAngle);
			}
		}


		/* Bivariate Gaussian function, TODO: delete versions */
		float gaussPDF(int version, float x, float y, float hw);

		/* map square to disk (https://www.aanda.org/articles/aa/pdf/2010/12/aa15278-10.pdf) */
		Vec2f sampleDisk(Vec2f p)
		{
			if (p(0) == 0 && p(1) == 0)
				return p;
			if (abs(p(1)) <= abs(p(0)))
				return Vec2f(
				(2 * p(0)) / sqrt(M_PI) * cos((p(1)*M_PI) / (4 * p(0))),
					(2 * p(0)) / sqrt(M_PI) * sin((p(1)*M_PI) / (4 * p(0)))
				);
			if (abs(p(0)) < abs(p(1)))
				return Vec2f(
				(2 * p(1)) / sqrt(M_PI) * sin((p(0)*M_PI) / (4 * p(1))),
					(2 * p(1)) / sqrt(M_PI) * cos((p(0)*M_PI) / (4 * p(1)))
				);
		}

		/* creates numSamples*numSamples + 2*numSamples + 1 uniformly distributed points on a disk,
		 * that is because i want samples on x = 0 and y = 0 and x = -1 and y = -1.
		 * with x,y equal to deviation from centerpoint (0,0), with maximum extent acceptanceAngle */
		std::vector<Vec2f> concentricDiskSamples(int numSamples, float acceptanceAngle);

		/* creates numSamples*numSamples + 2*numSamples + 1 x,y coordinates, evenly spaced on a square in range -1..1,
		 * use odd numSamples, samples start at x=0 and y=0 */
		std::vector<Vec2f> squareSamples(int numSamples);

		/* compute gaussian weights based on x,y coordinates of sample, TODO normalize so sum(weights) = 1 */
		std::vector<float> computeWeightVector(std::vector<Vec2f> &samples, float acceptanceAngle, int version = 2);

	};


}