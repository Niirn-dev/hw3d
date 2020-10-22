#include "Box.h"
#include <vector>
#include "BindableBase.h"

Box::Box( Graphics& gfx,
	std::mt19937& rng,
    std::uniform_real_distribution<float>& rDist,
    std::uniform_real_distribution<float>& angleDist,
    std::uniform_real_distribution<float>& speedDist )
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
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};

		const std::vector<Vertex> vertices = {
			{ -1.0f,-1.0f,-1.0f },
			{  1.0f,-1.0f,-1.0f },
			{ -1.0f, 1.0f,-1.0f },
			{  1.0f, 1.0f,-1.0f },
			{ -1.0f,-1.0f, 1.0f },
			{  1.0f,-1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{  1.0f, 1.0f, 1.0f },
		};
		AddStaticBind( std::make_unique<VertexBuffer>( gfx,vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"VertexShader.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PixelShader.cso" ) );

		const std::vector<unsigned short> indices = {
			0,2,1,	2,3,1,
			1,3,5,	3,7,5,
			2,6,3,	3,6,7,
			4,5,7,	4,7,6,
			0,4,2,	2,4,6,
			0,1,4,	1,5,4,
		};
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,indices ) );

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
    return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
		DirectX::XMMatrixTranslation( r,0.0f,0.0f ) *
		DirectX::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
		DirectX::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
