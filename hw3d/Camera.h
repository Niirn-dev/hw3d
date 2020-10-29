#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() = default;
	DirectX::XMMATRIX GetTranformXM() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	// position in polar coordinates
	float r = 20.0f;
	float theta = 0.0f;
	float phi = 0.0f;
	// orientation
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};

