#include "TransformCBuf.h"

TransformCBuf::TransformCBuf( Graphics& gfx,const Drawable& parent )
	:
	vcbuf( gfx ),
	parent( parent )
{}

void TransformCBuf::Bind( Graphics& gfx ) noexcept
{
	auto transform = parent.GetTransformXM() * gfx.GetProjection();
	vcbuf.Update( gfx,DirectX::XMMatrixTranspose( transform ) );
	vcbuf.Bind( gfx );
}
