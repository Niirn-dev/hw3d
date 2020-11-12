#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Translate( DirectX::XMFLOAT3 translation ) noexcept;
	void Rotate( float dx,float dy ) noexcept;
private:
	DirectX::XMFLOAT3 pos = { 0.0f,1.0f,-10.0f };
	static constexpr float maxPitch = 0.995f * 90.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	static constexpr float travelSpeed = 10.0f;
	static constexpr float rotationSpeed = 0.1f;
};