#include "Window.h"
#include <sstream>

// macros for easier exception throwing
#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )

// create wndClass instance
Window::WindowClass Window::WindowClass::wndClass;

// window exception stuff
Window::Exception::Exception( int line,const char* file,HRESULT hr )
	:
	ChiliException( line,file ),
	hr( hr )
{}

const char* Window::Exception::what() const noexcept
{	
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{	
	return "Chili Window Exception";
}

std::string Window::Exception::TranslateErrorCode( HRESULT hr ) noexcept
{	
	char* pMsgBuff = nullptr;
	DWORD nMsgLen = FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
		reinterpret_cast<LPSTR>( &pMsgBuff ),
		0,
		nullptr );
	if ( nMsgLen == 0 )
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuff;
	LocalFree( pMsgBuff );
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{	
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{	 
	return TranslateErrorCode( hr );
}


const char* Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HangleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass( GetName(),GetInstance() );
}

Window::Window( int width,int height,const char* name )
{
	// calculate window size based on the desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = wr.left + width;
	wr.top = 100;
	wr.bottom = wr.top + height;
	if ( AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE ) == 0 )
	{
		throw CHWND_LAST_EXCEPT();
	}
	// create window and get hWnd
	hWnd = CreateWindowEx(
		0,
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,WindowClass::GetInstance(),this
	);
	if ( hWnd == nullptr )
	{
		throw CHWND_LAST_EXCEPT();
	}
	// show window
	ShowWindow( hWnd,SW_SHOWDEFAULT );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

LRESULT Window::HangleMsgSetup( _In_ HWND hWnd,_In_ UINT msg,_In_ WPARAM wParam,_In_ LPARAM lParam ) noexcept
{
	// if window is created
	if ( msg == WM_NCCREATE )
	{
		// extract pointer to window class from creation data
		const LPCREATESTRUCT pCrtStruct = reinterpret_cast<LPCREATESTRUCT>( lParam );
		Window* const pWnd = static_cast<Window*>( pCrtStruct->lpCreateParams );
		// set WinAPI-managed user data to store ptr to window class object
		SetWindowLongPtr( hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>( pWnd ) );
		// set message proc to normal handler now that set-up is finished
		SetWindowLongPtr( hWnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>( &Window::HandleMsgThunk ) );
		// forward message to main window message handler
		return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
	}
	// if we get any message before the WM_NCCREATE just use the default handler
	return DefWindowProc( hWnd,msg,wParam,lParam );
}

LRESULT Window::HandleMsgThunk( _In_ HWND hWnd,_In_ UINT msg,_In_ WPARAM wParam,_In_ LPARAM lParam ) noexcept
{
	// retrieve pointer to window class object
	Window* const pWnd = reinterpret_cast<Window*>( GetWindowLongPtr( hWnd,GWLP_USERDATA ) );
	// forward message to main window message handler
	return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
}

LRESULT Window::HandleMsg( _In_ HWND hWnd_in,_In_ UINT msg,_In_ WPARAM wParam,_In_ LPARAM lParam ) noexcept
{
	// do custom message handling
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		// return without going through the default proc because the window is going to be destroyed in the destructor
		return 0;
	}
	// invoke default window proc for all of the unhandled messages
	return DefWindowProc( hWnd_in,msg,wParam,lParam );
}
