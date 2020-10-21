#include "Drawable.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw( Graphics& gfx ) noexcept( !IS_DEBUG )
{
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::AddBind( std::unique_ptr<Bindable> pBind ) noexcept
{
	assert( "*Must* use AddIndexBuffer to bind IndexBuffer" && typeid( *pBind ) != typeid( IndexBuffer ) );
	binds.push_back( std::move( pBind ) );
}

void Drawable::AddIndexBuffer( std::unique_ptr<IndexBuffer> pBind ) noexcept
{
	assert( "Attempting to add index buffer a second time" && pIndexBuffer == nullptr );
	pIndexBuffer = pBind.get();
	binds.push_back( std::move( pBind ) );
}
