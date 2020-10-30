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
	struct Transforms
	{
		DirectX::XMMATRIX worldView;
		DirectX::XMMATRIX worldViewProj;
	};
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> vcbuf;
	const Drawable& parent;
};
