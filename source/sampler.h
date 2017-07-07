#pragma once

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

	private:
		Mode m_mode;
		int m_numSamplePoints;
		float m_acceptanceAngle;

	public:

		std::vector<Vec2f> m_samplePoints; 
		std::vector<float> m_weights;

		Sampler(){}
		
		Sampler(int numSamples, float acceptanceAngle) 
		{
			if (numSamples > 0)
			{
				m_samplePoints = concentricDiskSamples(numSamples, acceptanceAngle);
				m_weights = computeWeightVector(m_samplePoints, acceptanceAngle);
				m_mode = Mode::DISK;
				m_numSamplePoints = numSamples;
				m_acceptanceAngle = acceptanceAngle;
			}
		}

		Sampler(int numSamples, float acceptanceAngle, Mode mode)
		{
			if (numSamples > 0)
			{
				m_mode = mode;
				m_numSamplePoints = numSamples;
				m_acceptanceAngle = acceptanceAngle;
				recalcSamplesAndWeights();
			}
		}


		/* Bivariate Gaussian function, TODO: delete versions */
		float gaussPDF(float x, float y, float hw);

		/* map square to disk (https://www.aanda.org/articles/aa/pdf/2010/12/aa15278-10.pdf) */
		Vec2f sampleDisk(Vec2f p);
		
		/* creates numSamples*numSamples + numSamples uniformly distributed points on a disk,
		 * that is because I want samples on x = 0 and y = 0 and x = -1 and y = -1.
		 * with x,y equal to deviation from centerpoint (0,0), with maximum extent acceptanceAngle */
		std::vector<Vec2f> concentricDiskSamples(int numSamples, float acceptanceAngle);

		/* creates numSamples*numSamples + 2*numSamples + 1 x,y coordinates, evenly spaced on a square in range -1..1,
		 * use odd numSamples, samples start at x=0 and y=0 */
		std::vector<Vec2f> squareSamples(int numSamples);

		/* compute gaussian weights based on x,y coordinates of sample, TODO normalize so sum(weights) = 1 */
		std::vector<float> computeWeightVector(std::vector<Vec2f> &samples, float acceptanceAngle);

		// getters and setters for encapsulated members
		void setMode(Mode mode)
		{
			m_mode = mode;
			recalcSamplesAndWeights();
		}

		Mode getMode() { return m_mode; }

		void setAcceptanceAngle(float acceptanceAngle)
		{
			m_acceptanceAngle = acceptanceAngle;
			recalcSamplesAndWeights();
			return;
		}

		float getAcceptanceAngle() { return m_acceptanceAngle; }

		void setSqrtNumSamplePoints(int sqrtNumSamplePoints)
		{
			m_numSamplePoints = sqrtNumSamplePoints;
			recalcSamplesAndWeights();
			return;
		}

		int getNumSamplePoints() { return m_numSamplePoints; }

		private:
			void recalcSamplesAndWeights()
			{
				if (m_mode == Mode::DISK)
				{
					m_samplePoints = concentricDiskSamples(m_numSamplePoints, m_acceptanceAngle);
				}
				else if (m_mode == Mode::SQUARE)
				{
					m_samplePoints = squareSamples(m_numSamplePoints);
					for (Vec2f &p : m_samplePoints) // also make square in range -aa:aa
						p = m_acceptanceAngle*p;
				}
				m_weights = computeWeightVector(m_samplePoints, m_acceptanceAngle);
			}

	};


}