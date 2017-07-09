#include "stdafx.h"

#include "sampler.h"

//#define DEBUG

namespace BeeView
{

	/* precomputes sample points on construction */
	Sampler::Sampler(int numSamples, float acceptanceAngle)
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

	Sampler::Sampler(int numSamples, float acceptanceAngle, Mode mode)
	{
		if (numSamples > 0)
		{
			m_mode = mode;
			m_numSamplePoints = numSamples;
			m_acceptanceAngle = acceptanceAngle;
			recalcSamplesAndWeights();
		}
	}

	Vec2f Sampler::sampleDisk(Vec2f p)
	{
		if (p(0) == 0 && p(1) == 0)
			return p;
		if (abs(p(1)) <= abs(p(0)))
			return Vec2f(
			(2 * p(0)) / sqrt(M_PI) * cos((p(1)*M_PI) / (4 * p(0))),
				(2 * p(0)) / sqrt(M_PI) * sin((p(1)*M_PI) / (4 * p(0)))
			);
		//if (abs(p(0)) < abs(p(1))) // remaining option
			return Vec2f(
			(2 * p(1)) / sqrt(M_PI) * sin((p(0)*M_PI) / (4 * p(1))),
				(2 * p(1)) / sqrt(M_PI) * cos((p(0)*M_PI) / (4 * p(1)))
			);
	}

	// just rely on compiler to move and not copy return value
	std::vector<Vec2f> Sampler::concentricDiskSamples(int numSamples, float acceptanceAngle)
	{
		// first compute square of -1..1 and then map to disk
		std::vector<Vec2f> sSamples = squareSamples(numSamples);
		std::vector<Vec2f> cSamples;

		// convert square coordinates to concentric circle
		for (Vec2f p : sSamples)
			cSamples.push_back(sampleDisk(p));


		/* normalize */
		// get min and max of coordinates, since square: min_x = -max_x = -max_y = min_y
		auto minmax_x = std::minmax_element(cSamples.begin(), cSamples.end(),
			[](Vec2f & lhs, Vec2f & rhs) {return (float)lhs(0) < (float)rhs(0); }); // lambda

		Vec2f min_x = *minmax_x.first;

		for (Vec2f &p : cSamples)
			normalizePoint(p, -acceptanceAngle, acceptanceAngle, min_x(0), -min_x(0));

		return cSamples;
	}

	std::vector<Vec2f> Sampler::squareSamples(int numSamples)
	{

		float x = 0;
		float y = 0;

		float range = 1.f;
		float spacing = 2 * (range / numSamples) + 2 * range / (numSamples*numSamples); // for square range -1..1

		std::vector<Vec2f> samples;

		for (int i = -numSamples / 2; i <= (numSamples + 1) / 2; i++)
		{
			for (int j = -numSamples / 2; j <= numSamples / 2; j++)
			{
				samples.push_back(Vec2f(x, y));
				x = j*spacing;
			}
			y = i*spacing;
		}

		return samples;
	}

	float Sampler::gaussPDF(float x, float y, float hw)
	{
		float dist = sqrt(x*x + y*y);
		// have to change to 3.333, so that FWHM ist correct
		float var = hw / 3.333;
		return std::exp(-pow(0.5 * dist / var, 2));	
	}

	std::vector<float> Sampler::computeWeightVector(std::vector<Vec2f> &samples, float acceptanceAngle)
	{
		std::vector<float> weights;
		for (Vec2f &p : samples)
		{

			float x = p(0);
			float y = p(1);
			weights.push_back(gaussPDF(x, y, acceptanceAngle));
		}

		// normalize weights to sum 1
		double sum = std::accumulate(weights.begin(), weights.end(), 0.0);

		for (float &w : weights)
			w = w / sum;


		return weights;
	}

	void Sampler::setMode(Sampler::Mode mode)
	{
		m_mode = mode;
		recalcSamplesAndWeights();
	}

	Sampler::Mode Sampler::getMode() { return m_mode; }

	void Sampler::setAcceptanceAngle(float acceptanceAngle)
	{
		m_acceptanceAngle = acceptanceAngle;
		recalcSamplesAndWeights();
		return;
	}

	float Sampler::getAcceptanceAngle() { return m_acceptanceAngle; }

	void Sampler::setSqrtNumSamplePoints(int sqrtNumSamplePoints)
	{
		m_numSamplePoints = sqrtNumSamplePoints;
		recalcSamplesAndWeights();
		return;
	}

	int Sampler::getNumSamplePoints() { return m_numSamplePoints; }

	void Sampler::recalcSamplesAndWeights()
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

}