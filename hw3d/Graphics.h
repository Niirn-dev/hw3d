#pragma once

#include "WinFlags.h"
#include "ChiliException.h"
#include <d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"
#include <wrl.h>

class Graphics
{
public:
	// define types for graphics related exceptions
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr,std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
	class InfoException : public Exception
	{
	public:
		InfoException( int line,const char* file,std::vector<std::string> infoMsgs );
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
public:
	Graphics( HWND hWnd );
	~Graphics() = default;
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;

	void DrawTestTriangle( float angle );
	void EndFrame();
	void ClearBuffer( float r,float g,float b ) noexcept;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG

	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};

