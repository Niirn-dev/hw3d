#pragma once

#include "DrawableBase.h"
#include <optional>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bindable>> bindablePtrs );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) const noexcept( !IS_DEBUG );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform = {};
};

class Node
{
	friend class Model;
public:
	Node( std::string name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform_in );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransforms ) noexcept( !IS_DEBUG );
	void ShowTree( int& trackedIndex,std::optional<int>& selectedIndex,Node*& pSelectedNode ) const noexcept( !IS_DEBUG );
	void SetAcquiredTransform( DirectX::FXMMATRIX transform_in ) noexcept;
private:
	void AddChild( std::unique_ptr<Node> pNode ) noexcept( !IS_DEBUG );
private:
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform = {};
	DirectX::XMFLOAT4X4 acquiredTransform = {};
};

class Model
{
public:
	Model( Graphics& gfx,const std::string& file );
	~Model();
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx,const struct aiMesh* pMesh ) noexcept( !IS_DEBUG );
	std::unique_ptr<Node> ParseNode( const struct aiNode* pNode ) noexcept( !IS_DEBUG );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX transform = DirectX::XMMatrixIdentity() ) const noexcept( !IS_DEBUG );
	void SpawnControlWindow() const noexcept( !IS_DEBUG );
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pModelWindow;
};
