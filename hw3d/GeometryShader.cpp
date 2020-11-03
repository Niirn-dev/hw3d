#include "GeometryShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

#pragma comment( lib,"d3dcompiler.lib" )

GeometryShader::GeometryShader( Graphics& gfx,const std::wstring& file )
{
	if ( !file.empty() )
	{
		INFOMAN( gfx );

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO( D3DReadFileToBlob( file.c_str(),&pBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreateGeometryShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pShader
		) );
		isEnabled = true;
	}
}

void GeometryShader::Bind( Graphics& gfx ) noexcept
{
	if ( isEnabled )
	{
		GetContext( gfx )->GSSetShader( pShader.Get(),nullptr,0u );
	}
	else
	{
		GetContext( gfx )->GSSetShader( nullptr,nullptr,0u );
	}
}
