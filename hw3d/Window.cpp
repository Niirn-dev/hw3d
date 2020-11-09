#include "Window.h"
#include "resource.h"
#include <sstream>
#include "WindowThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

// create wndClass instance
Window::WindowClass Window::WindowClass::wndClass;

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
	wc.hIcon = static_cast<HICON>( LoadImage( GetInstance(),MAKEINTRESOURCE( IDI_ICON1 ),IMAGE_ICON,32,32,0 ) );
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>( LoadImage( GetInstance(),MAKEINTRESOURCE( IDI_ICON1 ),IMAGE_ICON,16,16,0 ) );;
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass( GetName(),GetInstance() );
}

Window::Window( int width,int height,const char* name )
	:
	width( width ),
	height( height )
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
	// window is set to hidden upon creation
	ShowWindow( hWnd,SW_SHOWDEFAULT );
	// init win32 implementation for ImGui
	ImGui_ImplWin32_Init( hWnd );
	// initialize graphics
	pGfx = std::make_unique<Graphics>( hWnd,this->width,this->height );
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow( hWnd );
}

void Window::SetTitle( const std::string& title ) const
{
	if ( SetWindowText( hWnd,title.c_str() ) == 0 )
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	if ( PeekMessage( &msg,nullptr,0,0,PM_REMOVE ) )
	{
		// test for quit message
		if ( msg.message == WM_QUIT )
		{
			return static_cast<int>( msg.wParam );
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return std::nullopt;
}

void Window::EnableCursor()
{
	ShowCursor();
	EnableImguiMouse();
	isCursorEnabled = true;
}

void Window::DisableCursor()
{
	HideCursor();
	DisableImguiMouse();
	isCursorEnabled = false;
}

void Window::ShowCursor()
{
	while ( ::ShowCursor( TRUE ) < 0 );
}

void Window::HideCursor()
{
	while ( ::ShowCursor( FALSE ) >= 0 );
}

void Window::DisableImguiMouse()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void Window::EnableImguiMouse()
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
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
	// let ImGui process window messages
	if ( ImGui_ImplWin32_WndProcHandler( hWnd_in,msg,wParam,lParam ) )
	{
		// ImGui_ImplWin32_WndProcHandler returns true if all messages were handled
		return true;
	}

	auto& imguiIO = ImGui::GetIO();

	// do custom message handling
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		// return without going through the default proc because the window is going to be destroyed in the destructor
		return 0;
	/************* KEYBOARD MESSAGES *************/
	case WM_SYSKEYDOWN:
		[[fallthrough]];
	case WM_KEYDOWN:
		// don't pass keyboard input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureKeyboard )
		{
			return true;
		}

		if ( !( lParam & 0x40000000 ) || kbd.AutorepeatIsEnabled() )
		{
			kbd.OnKeyPressed( static_cast<unsigned char>( wParam ) );
		}
		break;
	case WM_SYSKEYUP:
		[[fallthrough]];
	case WM_KEYUP:
		// don't pass keyboard input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureKeyboard )
		{
			return true;
		}

		kbd.OnKeyReleased( static_cast<unsigned char>( wParam ) );
		break;
	case WM_CHAR:
		kbd.OnChar( static_cast<char>( wParam ) );
		break;

	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	/*********** END KEYBOARD MESSAGES ***********/
	/************** MOUSE MESSAGES ***************/
	case WM_LBUTTONDOWN:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto p = MAKEPOINTS( lParam );
		mouse.OnLeftPress( p.x,p.y );
	}
		break;
	case WM_RBUTTONDOWN:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto p = MAKEPOINTS( lParam );
		mouse.OnRightPress( p.x,p.y );
	}
	break;
	case WM_LBUTTONUP:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto p = MAKEPOINTS( lParam );
		mouse.OnLeftRelease( p.x,p.y );
		// release mouse capture if not in the client region
		if ( !IsInClientRegion( p.x,p.y ) )
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
	}
		break;
	case WM_RBUTTONUP:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto p = MAKEPOINTS( lParam );
		mouse.OnRightRelease( p.x,p.y );
		// release mouse capture if not in the client region
		if ( !IsInClientRegion( p.x,p.y ) )
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
	}
		break;
	case WM_MOUSEWHEEL:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto delta = GET_WHEEL_DELTA_WPARAM( wParam );
		mouse.OnWheelDelta( delta );
	}
		break;
	case WM_MOUSEMOVE:
	{
		// don't pass mouse input to the application if imgui wants to use it
		if ( imguiIO.WantCaptureMouse )
		{
			return true;
		}

		const auto p = MAKEPOINTS( lParam );
		// in client region -> log move and log capture/mouse enter if wasn't in the window before
		if ( IsInClientRegion( p.x,p.y ) )
		{
			mouse.OnMouseMove( p.x,p.y );
			if ( !mouse.IsInWindow() )
			{
				SetCapture( hWnd );
				mouse.OnMouseEnter();
			}
		}
		else // outside of the client region -> log and capture the mouse for as long as the LMB or RMB are pressed
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
				mouse.OnMouseMove( p.x,p.y );
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
	}
		break;
	/************ END MOUSE MESSAGES *************/
	}
	// invoke default window proc for all of the unhandled messages
	return DefWindowProc( hWnd_in,msg,wParam,lParam );
}

Graphics& Window::Gfx()
{
	if ( !pGfx )
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

bool Window::IsInClientRegion( int x,int y )
{
	return x >= 0 && x < width && y >= 0 && y < height;
}


/*********** EXCEPTION DIFINITIONS ***********/
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

Window::HrException::HrException( int line,const char* file,HRESULT hr ) noexcept
	:
	Exception( line,file ),
	hr( hr )
{}

const char* Window::HrException::what() const noexcept
{
	std::stringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}


HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode( hr );
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}
/********* END EXCEPTION DIFINITIONS *********/
