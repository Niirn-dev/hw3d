#include "SolidSphere.h"
#include "Sphere.h"
#include "BindableBase.h"

SolidSphere::SolidSphere( Graphics& gfx,float radius )
	:
	position( 0.0f,0.0f,0.0f )
{

	if ( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};
		// since tesselation is randomazed, both vertices and indices can't be static
		auto itlist = Sphere::Make<Vertex>( radius );
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,itlist.vertices ) );
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,itlist.indices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"SolidVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"SolidPS.cso" ) );

		struct ColorBuffer
		{
			DirectX::XMFLOAT3 c = { 1.0f,1.0f,1.0f };
			float paddig;
		} color;
		AddStaticBind( std::make_unique<PixelConstantBuffer<ColorBuffer>>( gfx,color ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
}

void SolidSphere::SetPosition( DirectX::XMFLOAT3 pos ) noexcept
{
	position = pos;
}

void SolidSphere::Update( float ) noexcept
{
	return;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( position.x,position.y,position.z );
}