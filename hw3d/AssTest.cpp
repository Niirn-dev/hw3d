#include "AssTest.h"
#include "BindableBase.h"
#include "Vertex.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

AssTest::AssTest( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& angleDist,
	std::uniform_real_distribution<float>& speedDist,
	std::uniform_real_distribution<float>& distortionDist,
	DirectX::XMFLOAT3 material,float specIntensity,float specPower,float scale )
	:
	DrawableTest( gfx,rng,rDist,angleDist,speedDist,distortionDist )
{
	namespace dx = DirectX;
	if ( !IsStaticInitialized() )
	{
		Assimp::Importer imp;
		auto pScene = imp.ReadFile( "Models/suzanne.obj",
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices );
		auto pMesh = pScene->mMeshes[0];

		VertexData vertices = { std::move( 
			VertexLayout{}.Append( VertexLayout::ElementType::Position3D )
				.Append( VertexLayout::ElementType::Normal ) ) };

		for ( size_t i = 0; i < pMesh->mNumVertices; ++i )
		{
			vertices.EmplaceBack( 
				dx::XMFLOAT3{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
				*reinterpret_cast<dx::XMFLOAT3*>( &pMesh->mNormals[i] )
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( (size_t)pMesh->mNumFaces * 3 );
		for ( size_t i = 0; i < pMesh->mNumFaces; ++i )
		{
			const auto& face = pMesh->mFaces[i];
			assert( face.mNumIndices == 3 && "unsupported number of indices in a primitive" );
			indices.push_back( (unsigned short)face.mIndices[0] );
			indices.push_back( (unsigned short)face.mIndices[1] );
			indices.push_back( (unsigned short)face.mIndices[2] );
		}

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,vertices ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,indices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<GeometryShader>( gfx ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PhongPS.cso" ) );

		AddStaticBind( std::make_unique<InputLayout>( gfx,vertices.GetLayout().GetD3D11Layout(),pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	struct ColorBuffer
	{
		alignas( 16 ) dx::XMFLOAT3 materialColor;
		float specIntensity = 1.0f;
		float specPower = 1.0f;
	};
	ColorBuffer cBuff = { material,specIntensity,specPower };
	AddBind( std::make_unique<PixelConstantBuffer<ColorBuffer>>( gfx,cBuff,0u ) );

	AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
}
