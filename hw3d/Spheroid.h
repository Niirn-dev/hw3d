#pragma once

#include "DrawableTest.h"

class Spheroid : public DrawableTest<Spheroid>
{
public:
	Spheroid( Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& angleDist,
		std::uniform_real_distribution<float>& speedDist,
		std::uniform_real_distribution<float>& distortionDist,
		std::uniform_int_distribution<int>& divDist );
};