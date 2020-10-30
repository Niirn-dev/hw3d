#include "TransformCBuf.h"

TransformCBuf::TransformCBuf( Graphics& gfx,const Drawable& parent )
	:
	parent( parent )
{
	if ( !vcbuf )
	{
		vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>( gfx );
	}
}

void TransformCBuf::Bind( Graphics& gfx ) noexcept
{
	const auto worldView = parent.GetTransformXM();
	Transforms transform = {
		DirectX::XMMatrixTranspose( worldView ),
		DirectX::XMMatrixTranspose( worldView * gfx.GetView() * gfx.GetProjection() )
	};
	vcbuf->Update( gfx,transform );
	vcbuf->Bind( gfx );
}

std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::vcbuf;
