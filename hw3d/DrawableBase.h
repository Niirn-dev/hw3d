#pragma once
#include "Drawable.h"
#include "Bindable.h"

template<class D>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty();
	}
	static void AddStaticBind( std::unique_ptr<Bindable> pBind ) noexcept( !IS_DEBUG )
	{
		assert( "*Must* use AddStaticIndexBuffer to bind static IndexBuffer" && typeid( *pBind ) != typeid( IndexBuffer ) );
		staticBinds.push_back( std::move( pBind ) );
	}
	void AddStaticIndexBuffer( std::unique_ptr<class IndexBuffer> pBind ) noexcept( !IS_DEBUG )
	{
		assert( "Attempting to add static index buffer a second time" && pIndexBuffer == nullptr );
		pIndexBuffer = pBind.get();
		staticBinds.push_back( std::move( pBind ) );
	}
	void SetIndexFromStatic() noexcept( !IS_DEBUG )
	{
		assert( "Attempting to set second index buffer from static binds" && pIndexBuffer == nullptr );
		for ( const auto& b : staticBinds )
		{
			if ( const auto pi = dynamic_cast<IndexBuffer*>( b.get() ) )
			{
				pIndexBuffer = pi;
				return;
			}
		}
		assert( "Failed to set index buffer from static binds" && pIndexBuffer == nullptr );
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
protected:
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class D>
std::vector<std::unique_ptr<Bindable>> DrawableBase<D>::staticBinds;