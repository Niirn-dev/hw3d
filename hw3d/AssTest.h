#pragma once

#include "DrawableTest.h"

class AssTest : public DrawableTest<AssTest>
{
public:
	AssTest( Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& angleDist,
		std::uniform_real_distribution<float>& speedDist,
		std::uniform_real_distribution<float>& distortionDist,
		DirectX::XMFLOAT3 material,float specIntensity,float specPower,float scale );
};