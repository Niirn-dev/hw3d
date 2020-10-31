#pragma once

#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere( Graphics& gfx,float radius );
	void SetPosition( DirectX::XMFLOAT3 pos ) noexcept;
	void SetColor( DirectX::XMFLOAT3 c ) noexcept;
	void UpdateCBuffer( Graphics& gfx ) noexcept;
	void Update( float ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	struct SphereCBuffer
	{
		DirectX::XMFLOAT3 c;
		float padding;
	};
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
};