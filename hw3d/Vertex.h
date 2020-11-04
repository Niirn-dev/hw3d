#pragma once
#include <vector>
#include <typeinfo>
#include <type_traits>
#include <DirectXMath.h>
#include "Graphics.h"

struct ARGBColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

class VertexLayout
{
public:
	enum class ElementType
	{
		Position2D,
		Position3D,
		Texture2D,
		Normal,
		Float3Color,
		Float4Color,
		ARGBColor,
		Count,
	};

	template<ElementType> struct Map;
	template<> struct Map<ElementType::Position2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "Position";
	};
	template<> struct Map<ElementType::Position3D>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Position";
	};
	template<> struct Map<ElementType::Texture2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "Texcoord";
	};
	template<> struct Map<ElementType::Normal>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Normal";
	};
	template<> struct Map<ElementType::Float3Color>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Color";
	};
	template<> struct Map<ElementType::Float4Color>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* semantic = "Color";
	};
	template<> struct Map<ElementType::ARGBColor>
	{
		using SysType = ARGBColor;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr const char* semantic = "Color";
	};
private:
	class Element
	{
		using ElType = VertexLayout::ElementType;
	public:
		Element( ElType type,size_t offset )
			:
			type( type ),
			offset( offset )
		{}
		size_t GetOffsetAfter() const noexcept( !IS_DEBUG )
		{
			return offset + Size();
		}
		size_t GetOffset() const noexcept( !IS_DEBUG )
		{
			return offset;
		}
		size_t Size() const noexcept( !IS_DEBUG )
		{
			return SizeOf( type );
		}
		ElType GetType() const noexcept
		{
			return type;
		}
		D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept( !IS_DEBUG )
		{
			switch ( type )
			{
			case ElType::Position2D:
				return GenerateDesc<ElType::Position2D>( offset );
			case ElType::Position3D:
				return GenerateDesc<ElType::Position3D>( offset );
			case ElType::Texture2D:
				return GenerateDesc<ElType::Texture2D>( offset );
			case ElType::Normal:
				return GenerateDesc<ElType::Normal>( offset );
			case ElType::Float3Color:
				return GenerateDesc<ElType::Float3Color>( offset );
			case ElType::Float4Color:
				return GenerateDesc<ElType::Float4Color>( offset );
			case ElType::ARGBColor:
				return GenerateDesc<ElType::ARGBColor>( offset );
			case ElType::Count:
			default:
				assert( "Invalid element type" && false );
				return { "INVALID",0u,DXGI_FORMAT_UNKNOWN,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u };
			}
		}

	private:
		template<ElType Type>
		static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc( size_t offset ) noexcept( !IS_DEBUG )
		{
			return { Map<Type>::semantic,0u,Map<Type>::dxgiFormat,0u,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0u };
		}
	public:
		static constexpr size_t SizeOf( ElType type ) noexcept( !IS_DEBUG )
		{
			switch ( type )
			{
			case ElType::Position2D:
				return sizeof( Map<ElType::Position2D>::SysType );
			case ElType::Position3D:
				return sizeof( Map<ElType::Position3D>::SysType );
			case ElType::Texture2D:
				return sizeof( Map<ElType::Texture2D>::SysType );
			case ElType::Normal:
				return sizeof( Map<ElType::Normal>::SysType );
			case ElType::Float3Color:
				return sizeof( Map<ElType::Float3Color>::SysType );
			case ElType::Float4Color:
				return sizeof( Map<ElType::Float4Color>::SysType );
			case ElType::ARGBColor:
				return sizeof( Map<ElType::ARGBColor>::SysType );
			case ElType::Count:
			default:
				assert( "Imvalid element type" && false );
				return 0u;
			}
		}

	private:
		ElType type;
		size_t offset;
	};
public:
	template<ElementType Type>
	const Element& Resolve() const noexcept( !IS_DEBUG )
	{
		for ( auto& e : elements )
		{
			if ( e.GetType() == Type )
			{
				return e;
			}
		}
		assert( "Could not resolve element type: " && typeid( Type ).name() && false );
		return elements.front();
	}
	const Element& ResolveByIndex( size_t i ) const noexcept( !IS_DEBUG )
	{
		return elements[i];
	}
	VertexLayout& Append( ElementType type ) noexcept( !IS_DEBUG )
	{
		elements.emplace_back( type,Size() );
		return *this;
	}
	size_t Size() const noexcept( !IS_DEBUG )
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}
	size_t GetElementCount() const noexcept
	{
		return elements.size();
	}
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3D11Layout() const noexcept( !IS_DEBUG )
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve( GetElementCount() );
		for ( const auto& e : elements )
		{
			desc.push_back( std::move( e.GetDesc() ) );
		}
		return std::move( desc );
	}

private:
	std::vector<Element> elements;
};

class Vertex
{
	friend class VertexData;
public:
	template<VertexLayout::ElementType Type>
	auto& Attr() noexcept( !IS_DEBUG )
	{
		auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
		return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>( pAttribute );
	}
	template<typename T>
	void SetAttributeByIndex( size_t i,T&& val ) noexcept( !IS_DEBUG )
	{
		using ElType = VertexLayout::ElementType;
		const auto& element = layout.ResolveByIndex( i );
		auto pAttribute = pData + element.GetOffset();
		switch ( element.GetType() )
		{
		case ElType::Position2D:
			SetAttribute<ElType::Position2D>( pAttribute,std::forward<T>( val ) );
			break;
		case ElType::Position3D:
			SetAttribute<ElType::Position3D>( pAttribute,std::forward<T>( val ) );
			break;
		case ElType::Texture2D:
			SetAttribute<ElType::Texture2D>( pAttribute,std::forward<T>( val ) );
			break;
		case ElType::Normal:
			SetAttribute<ElType::Normal>( pAttribute,std::forward<T>( val ) );
			break;
		case ElType::Float3Color:
			SetAttribute<ElType::Float3Color>( pAttribute,std::forward<T>( val ) );
			break;
		case ElType::Float4Color:
			SetAttribute<ElType::Float4Color>( pAttribute,std::forward<T>( val ) );
			break;
		default:
			assert( "Bad element type" && false );
			break;
		}
	}
protected:
	Vertex( char* pData,const VertexLayout& layout )
		:
		pData( pData ),
		layout( layout )
	{
		assert( pData != nullptr );
	}
private:
	template<typename First,typename... Rest>
	void SetAttributeByIndex( size_t i,First&& first,Rest&&... rest )
	{
		SetAttributeByIndex( i,std::forward<First>( first ) );
		SetAttributeByIndex( i + 1,std::forward<Rest>( rest )... );
	}
	// helper to reduce code duplication in SetAttributeName
	template<VertexLayout::ElementType DestLayoutType,typename SrcType>
	void SetAttribute( char* pAttribute,SrcType&& val ) noexcept( !IS_DEBUG )
	{
		using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
		if constexpr ( std::is_assignable<Dest,SrcType>::value )
		{
			*reinterpret_cast<Dest*>( pAttribute ) = val;
		}
		else
		{
			assert( "Parameter attribute type mismatch" && false );
		}
	}

private:
	char* pData = nullptr;
	const VertexLayout& layout;
};

class ConstVertex
{
	friend class VertexData;
protected:
	ConstVertex( const Vertex& v ) noexcept( !IS_DEBUG )
		:
		vertex( v )
	{}
public:
	template<VertexLayout::ElementType Type>
	const auto& Attr() const noexcept( !IS_DEBUG )
	{
		return const_cast<Vertex&>( vertex ).Attr<Type>();
	}

private:
	Vertex vertex;
};

class VertexData
{
public:
	VertexData( VertexLayout layout )
		:
		layout( std::move( layout ) )
	{}
	const char* GetData() const noexcept( !IS_DEBUG )
	{
		return buffer.data();
	}
	const VertexLayout& GetLayout() const noexcept
	{
		return layout;
	}
	size_t NumVertices() const noexcept( !IS_DEBUG )
	{
		return buffer.size() / layout.Size();
	}
	size_t Size() const noexcept( !IS_DEBUG )
	{
		return buffer.size();
	}
	template<typename... Params>
	void EmplaceBack( Params&&... params ) noexcept( !IS_DEBUG )
	{
		assert( sizeof...(params) == layout.GetElementCount() && "Param count doesn't match up number of vertex elements" );
		buffer.resize( buffer.size() + layout.Size() );
		Back().SetAttributeByIndex( 0u,std::forward<Params>( params )... );
	}
	Vertex Back() noexcept( !IS_DEBUG )
	{
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex Front() noexcept( !IS_DEBUG )
	{
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data(),layout };
	}
	Vertex operator[]( size_t i ) noexcept( !IS_DEBUG )
	{
		assert( i < NumVertices() );
		return Vertex{ buffer.data() + i * layout.Size(),layout };
	}
	ConstVertex Back() const noexcept( !IS_DEBUG )
	{
		return const_cast<VertexData*>( this )->Back();
	}
	ConstVertex Front() const noexcept( !IS_DEBUG )
	{
		return const_cast<VertexData*>( this )->Front();
	}
	ConstVertex operator[]( size_t i ) const noexcept( !IS_DEBUG )
	{
		return const_cast<VertexData&>( *this )[i];
	}

private:
	std::vector<char> buffer;
	VertexLayout layout;
};
