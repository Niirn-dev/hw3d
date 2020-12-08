#include "TransformCbufDoubleboi.h"

namespace Bind
{
	TransformCbufDoubleboi::TransformCbufDoubleboi( Graphics& gfx,const Drawable& parent,std::optional<UINT> vsSlot,std::optional<UINT> psSlot )
		:
		TransformCbuf( gfx,parent,vsSlot )
	{
		if ( !pPcbuf )
		{
			pPcbuf = std::make_unique<PixelConstantBuffer<TransformCbuf::Transforms>>( gfx,psSlot.value_or( 0u ) );
		}
	}

	void TransformCbufDoubleboi::Bind( Graphics& gfx ) noexcept
	{
		UpdateBindImpl( gfx,TransformCbuf::GetTransforms( gfx ) );
	}

	void TransformCbufDoubleboi::UpdateBindImpl( Graphics& gfx,Transforms transforms ) noexcept
	{
		TransformCbuf::UpdateBindImpl( gfx,transforms );
		pPcbuf->Update( gfx,transforms );
		pPcbuf->Bind( gfx );
	}

	std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCbufDoubleboi::pPcbuf;
}
