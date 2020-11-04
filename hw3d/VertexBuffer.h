#pragma once
#include "Bindable.h"
#include <vector>
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	template<class V>
	VertexBuffer( Graphics& gfx,const std::vector<V>& vertices )
		:
		stride( (UINT)sizeof( V ) )
	{
		INFOMAN( gfx );
		// make description for vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = stride * (UINT)vertices.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.StructureByteStride = stride;
		// make subresource data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();
		// create vertex buffer
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd,&sd,&pVertexBuffer ) );
	}
	VertexBuffer( Graphics& gfx,const VertexData& vertices )
		:
		stride( (UINT)vertices.GetLayout().Size() )
	{
		INFOMAN( gfx );
		// make description for vertex buffer
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = (UINT)vertices.Size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.StructureByteStride = stride;
		// make subresource data
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.GetData();
		// create vertex buffer
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd,&sd,&pVertexBuffer ) );
	}
	void Bind( Graphics& gfx ) noexcept override;
private:
	UINT stride = 0u;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
};
