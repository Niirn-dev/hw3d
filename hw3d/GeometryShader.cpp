#include "GeometryShader.h"
#include "GraphicsThrowMacros.h"
#include <d3dcompiler.h>

#pragma comment( lib,"d3dcompiler.lib" )

GeometryShader::GeometryShader( Graphics& gfx,const std::optional<std::wstring>& file )
{
	if ( file )
	{
		INFOMAN( gfx );

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO( D3DReadFileToBlob( file->c_str(),&pBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreateGeometryShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pShader
		) );
	}
}

void GeometryShader::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->GSSetShader( pShader.Get(),nullptr,0u );
}
