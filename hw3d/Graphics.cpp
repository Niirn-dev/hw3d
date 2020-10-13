#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment( lib,"d3d11.lib" )
#pragma comment( lib,"d3dcompiler.lib" )

#define GFX_EXEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( HRESULT _hr; FAILED( _hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,_hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if ( HRESULT _hr; FAILED( _hr = (hrcall) ) ) throw GFX_EXCEPT( _hr )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_ONLYINFO(call) infoManager.Set(); (call); if ( auto msgs = infoManager.GetMessages(); !msgs.empty() ) throw Graphics::InfoException( __LINE__,__FILE__,msgs )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO( (hrcall) )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_ONLYINFO(call) (call)
#endif // !NDEBUG

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics( HWND hWnd )
{
	// set up swap chain description
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = TRUE;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !NDEBUG

	// create device, swap chain and device context handles
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	) );

	// gain access to texture subresource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_INFO( pSwap->GetBuffer( 0,__uuidof(ID3D11Resource),&pBackBuffer ) );
	GFX_THROW_INFO( pDevice->CreateRenderTargetView( pBackBuffer.Get(),nullptr,&pTarget ) );
}

void Graphics::DrawTestTriangle( float angle )
{
	struct Vertex
	{
		float x;
		float y;
		unsigned char r;
		unsigned char g;
		unsigned char b;

	};
	// vertices for the triangle
	const Vertex vertices[] = {
		{  0.0f,	 0.375f,255,0,0 },
		{  0.125f,	 0.125f,125,125,0 },
		{ -0.125f,	 0.125f,0,0,175 },
		{ -0.125f,	 0.125f,17,156,79 },
		{  0.0f,	-0.125f,192,192,168 },
		{ -0.25f,	-0.125f,123,241,69 },
		{  0.125f,	 0.125f,69,42,217 },
		{  0.25f,	-0.125f,82,43,10 },
		{  0.0f,	-0.125f,200,0,222 }
	};

	// make description for vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = (UINT)sizeof( vertices );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof( Vertex );

	// make subresource data
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = &vertices;
	sd.SysMemPitch = 0u;
	sd.SysMemSlicePitch = 0u;

	// create vertex buffer
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	GFX_THROW_INFO( pDevice->CreateBuffer( &bd,&sd,&pVertexBuffer ) );

	// bind vertex buffer to the context
	UINT stride = (UINT)sizeof( Vertex );
	UINT offset = 0u;
	pContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );

	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO( D3DReadFileToBlob( L"PixelShader.cso",&pBlob ) );
	GFX_THROW_INFO( pDevice->CreatePixelShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr,&pPixelShader ) );

	// bind pixel shader
	pContext->PSSetShader( pPixelShader.Get(),nullptr,0u );

	// bind render target
	pContext->OMSetRenderTargets( 1u,pTarget.GetAddressOf(),nullptr );

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports( 1u,&vp );

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO( D3DReadFileToBlob( L"VertexShader.cso",&pBlob ) );
	GFX_THROW_INFO( pDevice->CreateVertexShader( pBlob->GetBufferPointer(),pBlob->GetBufferSize(),nullptr,&pVertexShader ) );
	// bind vertex shader
	pContext->VSSetShader( pVertexShader.Get(),nullptr,0u );

	struct ConstantBuffer
	{
		dx::XMMATRIX transformation;
	};
	// rotation matrix
	const ConstantBuffer transform = {
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ( angle ) *
			dx::XMMatrixScaling( 3.0f / 4.0f,1.0f,1.0f )
		)
	};
	// make description for transformation buffer
	D3D11_BUFFER_DESC cbd = {};
	cbd.ByteWidth = (UINT)sizeof( transform );
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.StructureByteStride = 0u;
	// subresource
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &transform;
	csd.SysMemPitch = 0u;
	csd.SysMemSlicePitch = 0u;
	// create the buffer
	wrl::ComPtr<ID3D11Buffer> pTransformBuffer;
	GFX_THROW_INFO( pDevice->CreateBuffer( &cbd,&csd,&pTransformBuffer ) );
	// bind buffer to vertex shader
	pContext->VSSetConstantBuffers( 0u,1u,pTransformBuffer.GetAddressOf() );

	// define input (vertex) layout
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION",0u,DXGI_FORMAT_R32G32_FLOAT,0u,0u,D3D11_INPUT_PER_VERTEX_DATA,0u },
		{ "COLOR",0u,DXGI_FORMAT_R8G8B8A8_UNORM,0u,8u,D3D11_INPUT_PER_VERTEX_DATA,0u }
	};
	GFX_THROW_INFO( pDevice->CreateInputLayout( ied,(UINT)std::size( ied ),pBlob->GetBufferPointer(),pBlob->GetBufferSize(),&pInputLayout ) );
	// bind input layout
	pContext->IASetInputLayout( pInputLayout.Get() );

	// set primitive topology
	pContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	GFX_THROW_ONLYINFO( pContext->Draw( (UINT)std::size( vertices ),0u ) );
}

void Graphics::EndFrame()
{
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG

	if ( HRESULT hr; FAILED( hr = pSwap->Present( 1u,0u ) ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( hr );
		}
		else
		{
			throw GFX_EXCEPT( hr );
		}
	}
}

void Graphics::ClearBuffer( float r,float g,float b ) noexcept
{
	const float color[] = { r,g,b,1.0f };
	pContext->ClearRenderTargetView(
		pTarget.Get(),
		color
	);
}

/*********** EXCEPTION DIFINITIONS ***********/
Graphics::HrException::HrException( int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{
	std::stringstream oss = {};
	for ( const auto& msg : infoMsgs )
	{
		oss << msg << std::endl;
	}
	info = oss.str();
	// remove last endl if info isn't empty
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	if ( !info.empty() )
	{
		oss << "\n[Error info]\n" << GetErrorInfo() << std::endl;
	}
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString( hr );
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription( hr,buf,sizeof( buf ) );
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException( int line,const char* file,std::vector<std::string> infoMsgs )
	:
	Exception( line,file )
{
	std::stringstream oss = {};
	for ( const auto& msg : infoMsgs )
	{
		oss << msg << std::endl;
	}
	info = oss.str();
	// remove last endl if info isn't empty
	if ( !info.empty() )
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl;
	if ( !info.empty() )
	{
		oss << "\n[Error info]\n" << GetErrorInfo() << std::endl;
	}
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Exception [Only Info]";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
/********* EXCEPTION DIFINITIONS END *********/
