#include "Box.h"
#include <vector>
#include "BindableBase.h"
#include "Cube.h"

Box::Box( Graphics& gfx,
	std::mt19937& rng,
    std::uniform_real_distribution<float>& rDist,
    std::uniform_real_distribution<float>& angleDist,
    std::uniform_real_distribution<float>& speedDist,
	std::uniform_real_distribution<float>& distortionDist )
	:
	DrawableTest( gfx,rng,rDist,angleDist,speedDist,distortionDist )
{
	if ( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PhongPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices) );

		/*struct LightBuffer
		{
			DirectX::XMFLOAT3 pos;
		};
		AddStaticBind( std::make_unique<PixelConstantBuffer<LightBuffer>>( gfx ) );*/

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
			{ "Normal",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,sizeof( Vertex::pos ),D3D11_INPUT_PER_VERTEX_DATA,0u },
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
