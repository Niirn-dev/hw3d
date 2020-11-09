#pragma once

#include "WinFlags.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <optional>
#include <memory>
#include "Graphics.h"

class Window
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode( HRESULT hr ) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException( int line,const char* file,HRESULT hr ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
		using Exception::Exception;
	public:
		const char* GetType() const noexcept override;
	};
private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass( const WindowClass& ) = delete;
		WindowClass& operator=( const WindowClass& ) = delete;

		static WindowClass wndClass;
		static constexpr const char* wndClassName = "Chili Direct3D Engine Window";
		HINSTANCE hInst;
	};

public:
	Window( int width,int height,const char* name );
	~Window();
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;
	void SetTitle( const std::string& title ) const;
	static std::optional<int> ProcessMessages() noexcept;
	void EnableCursor();
	void DisableCursor();
private:
	void ShowCursor();
	void HideCursor();
	void DisableImguiMouse();
	void EnableImguiMouse();

	static LRESULT CALLBACK HangleMsgSetup(
		_In_ HWND	hWnd,
		_In_ UINT	msg,
		_In_ WPARAM	wParam,
		_In_ LPARAM	lParam
	) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(
		_In_ HWND	hWnd,
		_In_ UINT	msg,
		_In_ WPARAM	wParam,
		_In_ LPARAM	lParam
	) noexcept;
	LRESULT HandleMsg(
		_In_ HWND	hWnd,
		_In_ UINT	msg,
		_In_ WPARAM	wParam,
		_In_ LPARAM	lParam
	) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
	Graphics& Gfx();
private:
	int width;
	int height;
	HWND hWnd = nullptr;
	std::unique_ptr<Graphics> pGfx;
	bool isCursorEnabled = true;
private:
	bool IsInClientRegion( int x,int y );
};
