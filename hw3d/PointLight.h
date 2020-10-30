#pragma once

#include "SolidSphere.h"
#include <DirectXMath.h>
#include "ConstantBuffers.h"

class PointLight
{
public:
	PointLight( Graphics& gfx );
	void Bind( Graphics& gfx ) noexcept;
	void Reset() noexcept;
	void Draw( Graphics& gfx ) const noexcept( !IS_DEBUG );
	void SpawnControlWindow() noexcept;
private:
	struct PointLightCBuffer
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};
private:
	DirectX::XMFLOAT3 pos;
	mutable SolidSphere mesh;
	PixelConstantBuffer<PointLightCBuffer> psCBuff;
};