#include "Pyramid.h"
#include "Cone.h"
#include "BindableBase.h"

Pyramid::Pyramid( Graphics& gfx,
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
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} c;
		};
		auto model = Cone::MakeTesselated<Vertex>( 4 );
		model.vertices[0].c = { 255,0,0,255 };
		model.vertices[1].c = { 255,255,0,255 };
		model.vertices[2].c = { 255,0,255,255 };
		model.vertices[3].c = { 0,255,255,255 };
		model.vertices[4].c = { 255,255,255,255 };

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"BlendVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"BlendPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
			{ "Position",0u,DXGI_FORMAT_R32G32B32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
			{ "Color",0u,DXGI_FORMAT_R8G8B8A8_UNORM,0u,sizeof( Vertex::pos ),D3D11_INPUT_PER_VERTEX_DATA,0u },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
		DirectX::XMStoreFloat3x3( 
			&mt,
			DirectX::XMMatrixScaling( 1.0f,1.0f,distortionDist( rng ) )
		);
	}

	AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
}

void Pyramid::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3( &mt ) *
		DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
		DirectX::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
