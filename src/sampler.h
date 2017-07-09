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

		// precomputed viewing direction offset array (x,y offsets in degrees)
		std::vector<Vec2f> m_samplePoints;

		// precomputed gauss weight array
		std::vector<float> m_weights;

		Sampler(){}
		
		/* precomputes sample points on construction */
		Sampler(int numSamples, float acceptanceAngle);

		Sampler(int numSamples, float acceptanceAngle, Mode mode);

		/* Bivariate Gaussian function */
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
		void setMode(Mode mode);

		Mode getMode();

		void setAcceptanceAngle(float acceptanceAngle);

		float getAcceptanceAngle();

		void setSqrtNumSamplePoints(int sqrtNumSamplePoints);

		int getNumSamplePoints();

		private:
			void recalcSamplesAndWeights();

	};


}