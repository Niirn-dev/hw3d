#pragma once

#include "DrawableBase.h"
#include <random>

template<class C>
class DrawableTest : public DrawableBase<C>
{
public:
	DrawableTest( Graphics&,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& rDist,
		std::uniform_real_distribution<float>& angleDist,
		std::uniform_real_distribution<float>& speedDist,
		std::uniform_real_distribution<float>& distortionDist )
		:
		r( rDist( rng ) ),
		roll( angleDist( rng ) ),
		pitch( angleDist( rng ) ),
		yaw( angleDist( rng ) ),
		theta( angleDist( rng ) ),
		phi( angleDist( rng ) ),
		chi( angleDist( rng ) ),
		droll( speedDist( rng ) ),
		dpitch( speedDist( rng ) ),
		dyaw( speedDist( rng ) ),
		dtheta( speedDist( rng ) ),
		dphi( speedDist( rng ) ),
		dchi( speedDist( rng ) )
	{
		DirectX::XMStoreFloat3x3(
			&mt,
			DirectX::XMMatrixScaling( 1.0f,1.0f,distortionDist( rng ) )
		);
	}
	void Update( float dt ) noexcept override
	{
		roll += droll * dt;
		pitch += dpitch * dt;
		yaw += dyaw * dt;
		theta += dtheta * dt;
		phi += dphi * dt;
		chi += dchi * dt;
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat3x3( &mt ) *
			DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
			DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
			DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi );
	}

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
	// model tranform
	DirectX::XMFLOAT3X3 mt = {};
};