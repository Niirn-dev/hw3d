#pragma once

#include "Bindable.h"
#include "TransformCbuf.h"

namespace Bind
{
	class TransformCbufDoubleboi : public TransformCbuf
	{
	public:
		TransformCbufDoubleboi( Graphics& gfx,const Drawable& parent,std::optional<UINT> vsSlot = {},std::optional<UINT> psSlot = {} );
		void Bind( Graphics& gfx ) noexcept override;
	private:
		void UpdateBindImpl( Graphics& gfx,Transforms transforms ) noexcept;
	private:
		static std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> pPcbuf;
	};
}
