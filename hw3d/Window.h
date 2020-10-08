#pragma once

#include "WinFlags.h"

class Window
{
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
	Window( int width,int height,const char* name ) noexcept;
	~Window();
	Window( const Window& ) = delete;
	Window& operator=( const Window& ) = delete;
private:
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
private:
	// int width;
	// int height;
	HWND hWnd = nullptr;
};

