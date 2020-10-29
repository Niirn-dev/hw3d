#include "Spheroid.h"
#include "Sphere.h"
#include "BindableBase.h"

Spheroid::Spheroid( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& angleDist,
	std::uniform_real_distribution<float>& speedDist,
	std::uniform_real_distribution<float>& distortionDist,
	std::uniform_int_distribution<int>& divDist )
	:
	DrawableTest( gfx,rng,rDist,angleDist,speedDist,distortionDist )
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
	};
	// since tesselation is randomazed, both vertices and indices can't be static
	auto itlist = Sphere::MakeTesselated<Vertex>( divDist( rng ),divDist( rng ) );
	AddBind( std::make_unique<VertexBuffer>( gfx,itlist.vertices ) );
	AddIndexBuffer( std::make_unique<IndexBuffer>( gfx,itlist.indices ) );

	if ( !IsStaticInitialized() )
	{
		auto pvs = std::make_unique<VertexShader>( gfx,L"SolidVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"SolidPS.cso" ) );

		struct ColorBuffer
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ColorBuffer colors = {
			{
				{ 1.0f,0.0f,0.0f,1.0f },
				{ 0.0f,1.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,1.0f,1.0f },
				{ 0.0f,1.0f,1.0f,1.0f },
				{ 1.0f,1.0f,1.0f,1.0f },
			}
		};
		AddStaticBind( std::make_unique<PixelConstantBuffer<ColorBuffer>>( gfx,colors ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}

	AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
}
