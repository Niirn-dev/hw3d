#pragma once

#include "WinFlags.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics( HWND hWnd );
	~Graphics();
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;

	void EndFrame() noexcept;
	void ClearBuffer( float r,float g,float b ) noexcept;
private:
	IDXGISwapChain* pSwap = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};

