#pragma once

#include <vector>
#include <array>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Plane
{
public:
	static IndexedTriangleList MakeTextured( unsigned short divisions = 7,float size = 1.0f )
	{
		namespace dx = DirectX;
		using ElType = Dvtx::VertexLayout::ElementType;

		const unsigned short nVerticesSide = divisions + 1;
		Dvtx::VertexBuffer vertices{
			Dvtx::VertexLayout{}
				.Append( ElType::Position3D )
				.Append( ElType::Normal )
				.Append( ElType::Texture2D )
		};
		
		{
			const float side = size / 2.0f;
			const float divisionSize = size / float( divisions );
			const auto topLeft = dx::XMVectorSet( -side,side,0.0f,0.0f );
			const auto n = dx::XMVectorSet( 0.0f,0.0f,-1.0f,0.0f );
			const auto dtc = size / (float)nVerticesSide;

			for( int y = 0,i = 0; y < nVerticesSide; y++ )
			{
				const float y_pos = -float( y ) * divisionSize;
				for( int x = 0; x < nVerticesSide; x++,i++ )
				{
					auto v = dx::XMVectorAdd( 
						topLeft,
						dx::XMVectorSet( float( x ) * divisionSize,y_pos,0.0f,0.0f )
					);
					auto tc = dx::XMVectorAdd(
						dx::XMVectorSet( 0.0f,0.0f,0.0f,0.0f ),
						dx::XMVectorSet( dtc * float( x ),dtc * float( y ),0.0f,0.0f )
					);

					vertices.EmplaceBack(
						*reinterpret_cast<dx::XMFLOAT3*>( &v ),
						*reinterpret_cast<const dx::XMFLOAT3*>( &n ),
						*reinterpret_cast<dx::XMFLOAT2*>( &tc )
					);
				}
			}
		}
		
		std::vector<unsigned short> indices;
		indices.reserve( sq( divisions ) * 6ull );
		{
			const auto vxy2i = [nVerticesSide]( unsigned short x,unsigned short y ) -> unsigned short
			{
				return y * nVerticesSide + x;
			};
			for( unsigned short y = 0; y < divisions; y++ )
			{
				for( unsigned short x = 0; x < divisions; x++ )
				{
					const std::array<unsigned short,4> indexArray =
					{ vxy2i( x,y ),vxy2i( x + 1,y ),vxy2i( x,y + 1 ),vxy2i( x + 1,y + 1 ) };
					indices.push_back( indexArray[0] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[3] );
				}
			}
		}

		return{ std::move( vertices ),std::move( indices ) };
	}
};