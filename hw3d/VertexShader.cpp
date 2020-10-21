#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

#pragma comment( lib,"d3dcompiler.lib" )

VertexShader::VertexShader( Graphics& gfx,const std::wstring& file )
{
	namespace wrl = Microsoft::WRL;
	INFOMAN( gfx );
	
	GFX_THROW_INFO( D3DReadFileToBlob( file.c_str(),&pBytecode ) );
	GFX_THROW_INFO( GetDevice( gfx )->CreateVertexShader( 
		pBytecode->GetBufferPointer(),
		pBytecode->GetBufferSize(),
		nullptr,
		&pVertexShader 
	) );
}

void VertexShader::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->VSSetShader( pVertexShader.Get(),nullptr,0u );
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
    return pBytecode.Get();
}
