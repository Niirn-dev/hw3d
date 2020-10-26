#pragma once

#include "DrawableBase.h"
#include <random>

class Pyramid : public DrawableBase<Pyramid>
{
public:
	Pyramid( Graphics& gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& angleDist,
		std::uniform_real_distribution<float>& speedDist );
	void Update( float dt ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// position in the world
	float r = 0.0f;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	float chi = 0.0f;
	// speed (delta/s)
	float droll = 0.0f;
	float dpitch = 0.0f;
	float dyaw = 0.0f;
	float dtheta = 0.0f;
	float dphi = 0.0f;
	float dchi = 0.0f;
};