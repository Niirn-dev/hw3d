#pragma once
#include "DrawableBase.h"
#include "Vertex.h"
#include <optional>
#include "ConditionalNoexcept.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelException : public ChiliException
{
public:
	ModelException( int line,const char* file,std::string note ) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh( Graphics& gfx,std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs );
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
	friend class ModelWindow;
public:
	Node( int id,const std::string& name,std::vector<Mesh*> meshPtrs,const DirectX::XMMATRIX& transform ) noxnd;
	void Draw( Graphics& gfx,DirectX::FXMMATRIX accumulatedTransform ) const noxnd;
	void SetAppliedTransform( DirectX::FXMMATRIX transform ) noexcept;
	constexpr int GetID() const noexcept
	{
		return id;
	}
private:
	void AddChild( std::unique_ptr<Node> pChild ) noxnd;
	void ShowTree( Node*& pSelectedNode ) const noexcept;
private:
	const int id;
	std::string name;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:
	Model( Graphics& gfx,const std::string fileName );
	void Draw( Graphics& gfx ) const noxnd;
	void ShowWindow( const char* windowName = nullptr ) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh( Graphics& gfx,const aiMesh& mesh,const aiMaterial* const* pMaterial );
	std::unique_ptr<Node> ParseNode( int& id,const aiNode& node ) noexcept;
private:
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;
	DirectX::XMFLOAT4X4 transform = {};
};