#pragma once

#include "IndexedTriangleList.h"

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make() noexcept( !IS_DEBUG )
	{
		namespace dx = DirectX;
		constexpr float side = 0.5f;

		std::vector<dx::XMFLOAT3> vertices;
		vertices.emplace_back( -side,-side,-side );
		vertices.emplace_back(  side,-side,-side );
		vertices.emplace_back( -side, side,-side );
		vertices.emplace_back(  side, side,-side );
		vertices.emplace_back( -side,-side, side );
		vertices.emplace_back(  side,-side, side );
		vertices.emplace_back( -side, side, side );
		vertices.emplace_back(  side, side, side );

		std::vector<V> verts( vertices.size() );
		for ( int i = 0; i < verts.size(); ++i )
		{
			verts[i].pos = vertices[i];
		}

		const std::vector<unsigned short> indices = {
			0,2,1,	2,3,1,
			1,3,5,	3,7,5,
			2,6,3,	3,6,7,
			4,5,7,	4,7,6,
			0,4,2,	2,4,6,
			0,1,4,	1,5,4,
		};

		return { std::move( verts ),std::move( indices ) };
	}
};
