#include "TransformCBuf.h"

TransformCBuf::TransformCBuf( Graphics& gfx,const Drawable& parent )
	:
	parent( parent )
{
	if ( !vcbuf )
	{
		vcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>( gfx );
	}
}

void TransformCBuf::Bind( Graphics& gfx ) noexcept
{
	auto transform = parent.GetTransformXM() * gfx.GetView() * gfx.GetProjection();
	vcbuf->Update( gfx,DirectX::XMMatrixTranspose( transform ) );
	vcbuf->Bind( gfx );
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuf::vcbuf;
