#include "../build/BeeView/BeeView/stdafx.h"

#include "sampler.h"

//#define DEBUG

namespace BeeView
{
	// just rely on compiler to move and not copy return value, c++11
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

#ifdef DEBUG
		plot2txt(cSamples, "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\plot_circle2.txt");
#endif

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

#ifdef DEBUG
		plot2txt(samples, "D:\\Documents\\bachelorarbeit\\raytracing\\beeView\\R\\plot_grid2.txt");
#endif // DEBUG

		return samples;
	}

	float Sampler::gaussPDF(int version, int x, int y, float hw)
	{
		// halfwidth to varianz
		float var = hw / (2.3548);

		float dist = sqrt(x*x + y*y);

		if (version == 1)
		{
			return 1 / (2 * M_PI) * std::exp(-0.5 * (x * x + y * y) / var / var);
		}
		if (version == 2)
		{
			return (1 / (var * sqrt(2 * M_PI))) * std::exp(-pow((0.5*dist / var), 2));
		}
		if (version == 3) // for halfwidth = 2
		{
			return 0.0109*std::exp(-0.6932*dist*dist);
		}
		return 0;
	}

	std::vector<float> Sampler::computeWeightVector(std::vector<Vec2f> &samples, float acceptanceAngle)
	{
		std::vector<float> weights;
		for (Vec2f &p : samples)
			weights.push_back(gaussPDF(2, p(0), p(1), acceptanceAngle));

		// normalize weights to sum 1
		double sum = std::accumulate(weights.begin(), weights.end(), 0.0);

		for (float &w : weights)
			w = w / sum;


		return weights;
	}

}