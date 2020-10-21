#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<class C>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer( Graphics& gfx,const C& consts )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC cbd = {};
		cbd.ByteWidth = (UINT)sizeof( consts );
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		csd.SysMemPitch = 0u;
		csd.SysMemSlicePitch = 0u;

		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd,&csd,&pConstantBuffer ) );
	}
	ConstantBuffer( Graphics& gfx )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC cbd = {};
		cbd.ByteWidth = (UINT)sizeof( C );
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.StructureByteStride = 0u;

		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &cbd,nullptr,&pConstantBuffer ) );
	}
	void Update( Graphics& gfx,const C& consts )
	{
		INFOMAN( gfx );

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO( GetContext( gfx )->Map( pConstantBuffer.Get(),0u,D3D11_MAP_WRITE_DISCARD,0u,&msr ) );

		memcpy( msr.pData,&consts,sizeof( consts ) );

		GetContext( gfx )->Unmap( pConstantBuffer.Get(),0u );
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
};

template<class C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetContext;
	using ConstantBuffer<C>::pConstantBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind( Graphics& gfx ) noexcept override
	{
		GetContext( gfx )->VSSetConstantBuffers( 0u,1u,pConstantBuffer.GetAddressOf() );
	}
};

template<class C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetContext;
	using ConstantBuffer<C>::pConstantBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind( Graphics& gfx ) noexcept override
	{
		GetContext( gfx )->PSSetConstantBuffers( 0u,1u,pConstantBuffer.GetAddressOf() );
	}
};