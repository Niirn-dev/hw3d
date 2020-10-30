#pragma once

#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere( Graphics& gfx,float radius );
	void SetPosition( DirectX::XMFLOAT3 pos ) noexcept;
	void Update( float ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 position;
};