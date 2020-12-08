#pragma once

#include "Drawable.h"

class TestPlane : public Drawable
{
public:
	TestPlane( Graphics& gfx,float size );
	void SetPosition( DirectX::XMFLOAT3 pos_in ) noexcept;
	void SetRotation( float roll_in,float pitch_in,float yaw_in ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SpawnControlWindow() noexcept;
private:
	void ResetControls() noexcept;
private:
	struct PSMaterialConstant
	{
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		BOOL normalMapEnabled = TRUE;
		float padding;
	} pmc;

	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};