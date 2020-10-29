#include "Sheet.h"
#include "BindableBase.h"
#include "Plane.h"
#include "Surface.h"

Sheet::Sheet( Graphics& gfx,
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
	namespace dx = DirectX;
	if ( !IsStaticInitialized() )
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};

		auto model = Plane::Make<Vertex>();
		model.vertices[0].tex = { 0.0f,0.0f };
		model.vertices[1].tex = { 1.0f,0.0f };
		model.vertices[2].tex = { 0.0f,1.0f };
		model.vertices[3].tex = { 1.0f,1.0f };

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );
		AddStaticBind( std::make_unique<Texture>( gfx,Surface::FromFile( "Images/that-guy.png" ) ) );
		AddStaticBind( std::make_unique<Sampler>( gfx ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"TextureVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturePS.cso" ) );

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
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling( distortionDist( rng ),distortionDist( rng ),1.0f )
	);
}

void Sheet::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3( &mt ) *
		DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi );
}
