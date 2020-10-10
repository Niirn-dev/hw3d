#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

#pragma comment( lib,"d3d11.lib" )

#define GFX_THROW_FAILED(hrcall) if( HRESULT _hr; FAILED( _hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,_hr )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )

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
	sd.Windowed = TRUE;
	sd.Flags = 0;

	// create device, swap chain and device context handles
	GFX_THROW_FAILED( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
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
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED( pSwap->GetBuffer( 0,__uuidof(ID3D11Resource),reinterpret_cast<void**>( &pBackBuffer ) ) );
	GFX_THROW_FAILED( pDevice->CreateRenderTargetView( pBackBuffer,nullptr,&pTarget ) );
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if ( pTarget != nullptr )
	{
		pTarget->Release();
	}
	if ( pContext != nullptr )
	{
		pContext->Release();
	}
	if ( pSwap != nullptr )
	{
		pSwap->Release();
	}
	if ( pDevice != nullptr )
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	if ( HRESULT hr; FAILED( hr = pSwap->Present( 1u,0u ) ) )
	{
		if ( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( hr );
		}
		else
		{
			GFX_THROW_FAILED( hr );
		}
	}
}

void Graphics::ClearBuffer( float r,float g,float b ) noexcept
{
	const float color[] = { r,g,b,1.0f };
	pContext->ClearRenderTargetView(
		pTarget,
		color
	);
}

/*********** EXCEPTION DIFINITIONS ***********/
Graphics::HrException::HrException( int line,const char* file,HRESULT hr ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
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


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
/********* EXCEPTION DIFINITIONS END *********/
