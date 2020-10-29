#include "SkinnedBox.h"
#include <vector>
#include "BindableBase.h"
#include "Cube.h"
#include "Surface.h"

SkinnedBox::SkinnedBox( Graphics& gfx,
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
			struct
			{
				float u;
				float v;
			} tex;
		};

		auto itlist = Cube::MakeSkinned<Vertex>();
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,itlist.vertices ) );
		AddStaticBind( std::make_unique<Texture>( gfx,Surface::FromFile( "Images\\cube.png" ) ) );
		AddStaticBind( std::make_unique<Sampler>( gfx ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"TextureVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturePS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,itlist.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
			{ "TexCoord",0u,DXGI_FORMAT_R32G32_FLOAT,0u,sizeof( Vertex::pos ),D3D11_INPUT_PER_VERTEX_DATA,0u },
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

