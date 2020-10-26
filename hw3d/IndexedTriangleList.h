#pragma once

#include <vector>
#include <cassert>
#include <DirectXMath.h>

template<class V>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList( std::vector<V> verts_in,std::vector<unsigned short> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.size() > 2 );
		assert( indices.size() % 3 == 0 );
	}
	void Transform( DirectX::FXMMATRIX matrix ) noexcept
	{
		for ( auto& v : vertices )
		{
			const auto pos = DirectX::XMLoadFloat3( &v.pos );
			DirectX::XMStoreFloat3(
				&v.pos,
				DirectX::XMVector3Transform( pos,matrix )
			);
		}
	}

public:
	std::vector<V> vertices;
	std::vector<unsigned short> indices;
};
