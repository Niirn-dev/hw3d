#pragma once

#include "DrawableTest.h"
#include <random>

class Sheet : public DrawableTest<Sheet>
{
public:
	Sheet( Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& angleDist,
		std::uniform_real_distribution<float>& speedDist,
		std::uniform_real_distribution<float>& distortionDist );
};