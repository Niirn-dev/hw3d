#pragma once
#include "Bindable.h"
#include "Drawable.h"
#include "ConstantBuffers.h"
#include <DirectXMath.h>

class TransformCBuf : public Bindable
{
public:
	TransformCBuf( Graphics& gfx,const Drawable& parent );
	void Bind( Graphics& gfx ) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> vcbuf;
	const Drawable& parent;
};
