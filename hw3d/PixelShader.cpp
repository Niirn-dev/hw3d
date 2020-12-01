#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	using namespace std::string_literals;

	PixelShader::PixelShader( Graphics& gfx,const std::string& path )
		:
		path( path )
	{
		INFOMAN( gfx );

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO( D3DReadFileToBlob( std::wstring{ path.begin(),path.end() }.c_str(),&pBlob ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr,&pPixelShader ) );
	}

	void PixelShader::Bind( Graphics& gfx ) noexcept
	{
		GetContext( gfx )->PSSetShader( pPixelShader.Get(),nullptr,0u );
	}

	std::shared_ptr<Bindable> PixelShader::Resolve( Graphics& gfx,const std::string& path ) noxnd
	{
		return Codex::Resolve<PixelShader>( gfx,path );
	}
	std::string PixelShader::GenerateUID( const std::string& path ) noexcept
	{
		return typeid( PixelShader ).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}
}
