#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

Texture::Texture( Graphics& gfx,const Surface& s )
{
	INFOMAN( gfx );

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = s.GetWidth();
	textureDesc.Height = s.GetHeight();
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1u;
	textureDesc.ArraySize = 1u;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0u;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = s.GetBufferPtr();
	sd.SysMemPitch = s.GetWidth() * sizeof( Surface::Color );
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D( &textureDesc,&sd,&pTexture ) );

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = textureDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MostDetailedMip = 0u;
	viewDesc.Texture2D.MipLevels = 1u;
	GFX_THROW_INFO( GetDevice( gfx )->CreateShaderResourceView( pTexture.Get(),&viewDesc,&pTextureView ) );
}

void Texture::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetShaderResources( 0u,1u,pTextureView.GetAddressOf() );
}
