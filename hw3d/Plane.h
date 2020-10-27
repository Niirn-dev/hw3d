#pragma once

#include "IndexedTriangleList.h"

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated( int divisions_x,int divisions_y ) noexcept( !IS_DEBUG )
	{
		namespace dx = DirectX;
		assert( divisions_x >= 1 );
		assert( divisions_y >= 1 );
		constexpr float width = 2.0f;
		constexpr float height = 2.0f;
		const int nverts_x = divisions_x + 1;
		const int nverts_y = divisions_y + 1;

		std::vector<V> vertices;
		vertices.reserve( (size_t)nverts_x * nverts_y );
		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float step_x = width / (float)divisions_x;
			const float step_y = height / (float)divisions_y;
			const auto bottomLeft = dx::XMVectorSet( -side_x,-side_y,0.0f,0.0f );

			for ( int y = 0; y < nverts_y; ++y )
			{
				for ( int x = 0; x < nverts_y; ++x )
				{
					vertices.emplace_back();
					const auto v = dx::XMVectorAdd(
						bottomLeft,
						dx::XMVectorSet( step_x * (float)x,step_y * (float)y,0.0f,0.0f )
					);
					dx::XMStoreFloat3( &vertices.back().pos,v );
				}
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve( (size_t)divisions_x * divisions_y * 6 );
		{
			const auto vxy2i = [nverts_x]( int x,int y ) -> unsigned short
			{
				return (unsigned short)( y * nverts_x + x );
			};

			for ( int y = 0; y < divisions_y; ++y )
			{
				for ( int x = 0; x < divisions_x; ++x )
				{
					indices.emplace_back( vxy2i( x,y ) );
					indices.emplace_back( vxy2i( x,y + 1 ) );
					indices.emplace_back( vxy2i( x + 1,y ) );

					indices.emplace_back( vxy2i( x + 1,y ) );
					indices.emplace_back( vxy2i( x,y + 1 ) );
					indices.emplace_back( vxy2i( x + 1,y + 1 ) );
				}
			}
		}

		return { std::move( vertices ),std::move( indices ) };
	}

	template<class V>
	static IndexedTriangleList<V> Make() noexcept( !IS_DEBUG )
	{
		return MakeTesselated<V>( 1,1 );
	}
};