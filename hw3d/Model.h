#pragma once

#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <optional>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bindable>> bindablePtrs )
	{
		if ( !IsStaticInitialized() )
		{
			AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
		}

		for ( auto& pb : bindablePtrs )
		{
			if ( auto pi = dynamic_cast<IndexBuffer*>( pb.get() ) )
			{
				AddIndexBuffer( std::make_unique<IndexBuffer>( *pi ) );
				pb.release();
			}
			else
			{
				AddBind( std::move( pb ) );
			}
		}

		AddBind( std::make_unique<TransformCBuf>( gfx,*this ) );
	}
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) const noexcept( !IS_DEBUG )
	{
		DirectX::XMStoreFloat4x4( &transform,accumulatedTransforms );
		Drawable::Draw( gfx );
	}
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat4x4( &transform );
	}
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	Node( std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform_in )
		:
		meshPtrs( std::move( meshPtrs ) )
	{
		DirectX::XMStoreFloat4x4( &transform,transform_in );
	}
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) noexcept( !IS_DEBUG )
	{
		const auto combinedTransform = DirectX::XMLoadFloat4x4( &transform ) * accumulatedTransforms;

		for ( auto& pm : meshPtrs )
		{
			pm->Draw( gfx,combinedTransform );
		}

		for ( auto& pc : childPtrs )
		{
			pc->Draw( gfx,combinedTransform );
		}
	}
private:
	void AddChild( std::unique_ptr<Node> pNode ) noexcept( !IS_DEBUG )
	{
		assert( pNode );
		childPtrs.push_back( std::move( pNode ) );
	}
private:
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model( Graphics& gfx,const std::string& file )
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(
			file,
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
		);
		assert( pScene->mNumMeshes != 0 );

		for ( size_t i = 0; i < pScene->mNumMeshes; ++i )
		{
			meshPtrs.push_back( ParseMesh( gfx,pScene->mMeshes[i] ) );
		}

		pRoot = ParseNode( pScene->mRootNode );
	}
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx,const aiMesh* pMesh ) noexcept( !IS_DEBUG )
	{
		VertexData vertices{
			VertexLayout{}
			.Append( VertexLayout::ElementType::Position3D )
			.Append( VertexLayout::ElementType::Normal )
		};

		for ( size_t i = 0; i < pMesh->mNumVertices; ++i )
		{
			vertices.EmplaceBack(
				*reinterpret_cast<DirectX::XMFLOAT3*>( &pMesh->mVertices[i] ),
				*reinterpret_cast<DirectX::XMFLOAT3*>( &pMesh->mNormals[i] )
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve( (size_t)pMesh->mNumFaces * 3 );
		for ( size_t i = 0; i < pMesh->mNumFaces; ++i )
		{
			const auto face = pMesh->mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( (unsigned short)face.mIndices[0] );
			indices.push_back( (unsigned short)face.mIndices[1] );
			indices.push_back( (unsigned short)face.mIndices[2] );
		}

		std::vector<std::unique_ptr<Bindable>> bindPtrs;
		bindPtrs.push_back( std::make_unique<VertexBuffer>( gfx,vertices ) );
		bindPtrs.push_back( std::make_unique<IndexBuffer>( gfx,indices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		bindPtrs.push_back( std::move( pvs ) );

		bindPtrs.push_back( std::make_unique<GeometryShader>( gfx ) );

		bindPtrs.push_back( std::make_unique<PixelShader>( gfx,L"PhongPS.cso" ) );

		bindPtrs.push_back( std::make_unique<InputLayout>( gfx,vertices.GetLayout().GetD3D11Layout(),pvsbc ) );

		struct ColorBuffer
		{
			alignas( 16 ) DirectX::XMFLOAT3 pos = { 0.6f,0.6f,0.8f };
			float specIntensity = 0.6f;
			float specPower = 30.0f;
			float padding[2] = {};
		} cBuff;
		bindPtrs.push_back( std::make_unique<PixelConstantBuffer<ColorBuffer>>( gfx,cBuff,0u ) );

		return std::make_unique<Mesh>( gfx,std::move( bindPtrs ) );
	}
	std::unique_ptr<Node> ParseNode( const aiNode* pNode ) noexcept( !IS_DEBUG )
	{
		const auto transform = DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4(
				reinterpret_cast<const DirectX::XMFLOAT4X4*>( &pNode->mTransformation )
		) );

		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve( pNode->mNumMeshes );
		for ( size_t i = 0; i < pNode->mNumMeshes; ++i )
		{
			const auto meshIdx = pNode->mMeshes[i];
			curMeshPtrs.push_back( meshPtrs.at( meshIdx ).get() );
		}

		auto node = std::make_unique<Node>( std::move( curMeshPtrs ),transform );
		for ( size_t i = 0; i < pNode->mNumChildren; ++i )
		{
			node->AddChild( ParseNode( pNode->mChildren[i] ) );
		}

		return std::move( node );
	}
	void Draw( Graphics& gfx,DirectX::FXMMATRIX transform ) const noexcept( !IS_DEBUG )
	{
		pRoot->Draw( gfx,transform );
	}
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};
