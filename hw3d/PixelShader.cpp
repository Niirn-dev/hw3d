#include "PixelShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

#pragma comment( lib,"d3dcompiler.lib" )

PixelShader::PixelShader( Graphics& gfx,const std::wstring& file )
{
	namespace wrl = Microsoft::WRL;
	INFOMAN( gfx );

	wrl::ComPtr<ID3DBlob> pBytecode;
	GFX_THROW_INFO( D3DReadFileToBlob( file.c_str(),&pBytecode ) );
	GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader( 
		pBytecode->GetBufferPointer(),
		pBytecode->GetBufferSize(),
		nullptr,
		&pPixelShader 
	) );
}

void PixelShader::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetShader( pPixelShader.Get(),nullptr,0u );
}
