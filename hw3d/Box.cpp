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
	r( rDist( rng ) ),
	roll( angleDist( rng ) ),
	pitch( angleDist( rng ) ),
	yaw( angleDist( rng ) ),
	theta( angleDist( rng ) ),
	phi( angleDist( rng ) ),
	chi( angleDist( rng ) ),
	droll( speedDist( rng ) ),
	dpitch( speedDist( rng ) ),
	dyaw( speedDist( rng ) ),
	dtheta( speedDist( rng ) ),
	dphi( speedDist( rng ) ),
	dchi( speedDist( rng ) )
{
	if ( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		auto itlist = Cube::Make<Vertex>();
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,itlist.vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"VertexShader.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PixelShader.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,itlist.indices) );

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
			{ "POSITION",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
	DirectX::XMStoreFloat3x3( 
		&mt,
		DirectX::XMMatrixScaling( 1.0f,1.0f,distortionDist( rng ) ) 
	);
}

void Box::Update( float dt ) noexcept
{
	roll	+= droll * dt;
	pitch	+= dpitch * dt;
	yaw		+= dyaw * dt;
	theta	+= dtheta * dt;
	phi		+= dphi * dt;
	chi		+= dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat3x3( &mt ) *
		DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
		DirectX::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
