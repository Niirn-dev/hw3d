#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
#include <optional>

namespace Bind
{
	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX model;
		};
	public:
		TransformCbuf( Graphics& gfx,const Drawable& parent,std::optional<UINT> slot = {} );
		void Bind( Graphics& gfx ) noexcept override;
	protected:
		void UpdateBindImpl( Graphics& gfx,Transforms transforms ) noexcept;
		Transforms GetTransforms( Graphics& gfx ) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}