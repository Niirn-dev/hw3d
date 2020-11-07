#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "BindableCommon.h"
#include "Vertex.h"
#include "imgui\imgui.h"
#include <unordered_map>

namespace dx = DirectX;

Mesh::Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bindable>> bindablePtrs )
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

void Mesh::Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) const noexcept( !IS_DEBUG )
{
	DirectX::XMStoreFloat4x4( &transform,accumulatedTransforms );
	Drawable::Draw( gfx );
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4( &transform );
}

Node::Node( std::string name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform_in )
	:
	name( name ),
	meshPtrs( std::move( meshPtrs ) )
{
	dx::XMStoreFloat4x4( &transform,transform_in );
	dx::XMStoreFloat4x4( &acquiredTransform,DirectX::XMMatrixIdentity() );
}

void Node::Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) noexcept( !IS_DEBUG )
{
	const auto combinedTransform = dx::XMLoadFloat4x4( &acquiredTransform ) * 
		dx::XMLoadFloat4x4( &transform ) *
		accumulatedTransforms;

	for ( auto& pm : meshPtrs )
	{
		pm->Draw( gfx,combinedTransform );
	}

	for ( auto& pc : childPtrs )
	{
		pc->Draw( gfx,combinedTransform );
	}
}

void Node::ShowTree( int& trackedIndex,std::optional<int>& selectedIndex,Node*& pSelectedNode ) const noexcept( !IS_DEBUG )
{
	const auto curIndex = trackedIndex++;
	const auto treeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		( selectedIndex.value_or( -1 ) == curIndex ? ImGuiTreeNodeFlags_Selected : 0 ) |
		( childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0 );

	if ( ImGui::TreeNodeEx( (void*)(intptr_t)curIndex,treeFlags,name.c_str() ) )
	{
		if ( ImGui::IsItemClicked() )
		{
			selectedIndex = curIndex;
			pSelectedNode = const_cast<Node*>( this );
		}
		for ( auto& c : childPtrs )
		{
			c->ShowTree( trackedIndex,selectedIndex,pSelectedNode );
		}

		ImGui::TreePop();
	}
}

void Node::SetAcquiredTransform( DirectX::FXMMATRIX transform_in ) noexcept
{
	DirectX::XMStoreFloat4x4( &acquiredTransform,transform_in );
}

void Node::AddChild( std::unique_ptr<Node> pNode ) noexcept( !IS_DEBUG )
{
	assert( pNode );
	childPtrs.push_back( std::move( pNode ) );
}

class ModelWindow
{
public:
	void Show( const char* windowName,const Node& root ) noexcept( !IS_DEBUG )
	{
		if ( ImGui::Begin( windowName ) )
		{
			ImGui::Columns( 2 );
			int trackedIndex = 0;
			root.ShowTree( trackedIndex,selectedIndex,pSelectedNode );

			ImGui::NextColumn();
			if ( pSelectedNode )
			{
				assert( selectedIndex != std::nullopt );
				auto& meshTransformation = meshTransformMap[*selectedIndex];
				ImGui::SliderFloat( "X",&meshTransformation.pos.x,-20.0f,20.0f,"%.2f" );
				ImGui::SliderFloat( "Y",&meshTransformation.pos.y,-20.0f,20.0f,"%.2f" );
				ImGui::SliderFloat( "Z",&meshTransformation.pos.z,-20.0f,20.0f,"%.2f" );

				ImGui::SliderAngle( "Roll",&meshTransformation.angle.roll,-180.0f,180.0f,"%.1f deg" );
				ImGui::SliderAngle( "Pitch",&meshTransformation.angle.pitch,-180.0f,180.0f,"%.1f deg" );
				ImGui::SliderAngle( "Yaw",&meshTransformation.angle.yaw,-180.0f,180.0f,"%.1f deg" );

				pSelectedNode->SetAcquiredTransform(
					dx::XMMatrixRotationRollPitchYaw( meshTransformation.angle.pitch,meshTransformation.angle.yaw,meshTransformation.angle.roll ) *
					dx::XMMatrixTranslation( meshTransformation.pos.x,meshTransformation.pos.y,meshTransformation.pos.z )
				);
			}
		}
		ImGui::End();
	}
private:
	struct TransformationParams
	{
		DirectX::XMFLOAT3 pos = {};
		struct
		{
			float pitch = 0.0f;
			float yaw = 0.0f;
			float roll = 0.0f;
		} angle;
	};
	std::unordered_map<int,TransformationParams> meshTransformMap;
	std::optional<int> selectedIndex;
	Node* pSelectedNode = nullptr;
};

Model::Model( Graphics& gfx,const std::string& file )
	:
	pModelWindow( std::make_unique<ModelWindow>() )
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

Model::~Model()
{}

std::unique_ptr<Mesh> Model::ParseMesh( Graphics& gfx,const aiMesh* pMesh ) noexcept( !IS_DEBUG )
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

std::unique_ptr<Node> Model::ParseNode( const aiNode* pNode ) noexcept( !IS_DEBUG )
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

	auto node = std::make_unique<Node>( pNode->mName.C_Str(),std::move( curMeshPtrs ),transform );
	for ( size_t i = 0; i < pNode->mNumChildren; ++i )
	{
		node->AddChild( ParseNode( pNode->mChildren[i] ) );
	}

	return std::move( node );
}

void Model::Draw( Graphics& gfx,DirectX::FXMMATRIX transform ) const noexcept( !IS_DEBUG )
{
	pRoot->Draw( gfx,transform );
}

void Model::SpawnControlWindow() const noexcept( !IS_DEBUG )
{
	pModelWindow->Show( "Model window",*pRoot );
}