#pragma once

#include "SolidSphere.h"
#include <DirectXMath.h>
#include "ConstantBuffers.h"

class PointLight
{
public:
	PointLight( Graphics& gfx );
	void Bind( Graphics& gfx ) noexcept;
	void Draw( Graphics& gfx ) const noexcept( !IS_DEBUG );
	void SpawnControlWindow() noexcept;
private:
	void ResetPosition() noexcept;
	void ResetColors() noexcept;
	void ResetAttenuation() noexcept;
private:
	struct PointLightCBuffer
	{
		alignas( 16 ) DirectX::XMFLOAT3 worldViewPos;
		alignas( 16 ) DirectX::XMFLOAT3 color;
		alignas( 16 ) DirectX::XMFLOAT3 ambient;
		float intensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 ambient = { 0.12f,0.0f,0.07f };
	float intensity = 1.0f;
	float attConst = 1.0f;
	float attLin = 0.22f;
	float attQuad = 0.2f;
	mutable SolidSphere mesh;
	PixelConstantBuffer<PointLightCBuffer> psCBuff;
};