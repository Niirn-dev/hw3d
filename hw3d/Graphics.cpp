#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include "GraphicsThrowMacros.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

#pragma comment( lib,"d3d11.lib" )


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
	sd.SampleDesc.Count = 1u;
	sd.SampleDesc.Quality = 0u;
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

	// depth stencil description
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	// fill in depth stancil state
	wrl::ComPtr<ID3D11DepthStencilState> pDepthState;
	GFX_THROW_INFO( pDevice->CreateDepthStencilState( &dsDesc,&pDepthState ) );

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = 800u;
	tDesc.Height = 600u;
	tDesc.Format = DXGI_FORMAT_D32_FLOAT;
	tDesc.MipLevels = 1u;
	tDesc.ArraySize = 1u;
	tDesc.SampleDesc.Count = 1u;
	tDesc.SampleDesc.Quality = 0u;
	tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO( pDevice->CreateTexture2D( &tDesc,nullptr,&pDepthStencil ) );

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO( pDevice->CreateDepthStencilView( pDepthStencil.Get(),&dsvDesc,&pDSV ) );

	// bind render target and depth stencil view to the Output Merger
	pContext->OMSetRenderTargets( 1u,pTarget.GetAddressOf(),pDSV.Get() );

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports( 1u,&vp );

	// init dx11 implementation for ImGui
	ImGui_ImplDX11_Init( pDevice.Get(),pContext.Get() );
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::SetProjection( DirectX::XMMATRIX proj ) noexcept
{
	projection = proj;
}
DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::DrawIndexed( UINT count ) noexcept( !IS_DEBUG )
{
	GFX_THROW_ONLYINFO( pContext->DrawIndexed( count,0u,0 ) );
}

void Graphics::BeginFrame( float r,float g,float b ) noexcept
{
	if ( IsImguiEnabled() )
	{
		// begin imgui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[] = { r,g,b,1.0f };
	pContext->ClearRenderTargetView(
		pTarget.Get(),
		color
	);
	pContext->ClearDepthStencilView( pDSV.Get(),D3D11_CLEAR_DEPTH,1.0f,0u );
}

void Graphics::EndFrame()
{
#ifndef NDEBUG
	infoManager.Set();
#endif // !NDEBUG

	if ( IsImguiEnabled() )
	{
		// render ingui frame
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	}

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

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
	return imguiEnabled;
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
