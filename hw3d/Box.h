#pragma once
#include "DrawableTest.h"
#include <random>

class Box : public DrawableTest<Box>
{
public:
	Box( Graphics& gfx,
		 std::mt19937& rng,
		 std::uniform_real_distribution<float>& rDist,
		 std::uniform_real_distribution<float>& angleDist,
		 std::uniform_real_distribution<float>& speedDist,
		 std::uniform_real_distribution<float>& distortionDist );
};
