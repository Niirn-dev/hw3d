#include <Windows.h>
#include <string>

LRESULT CALLBACK WindowProc(
	_In_ HWND	hWnd,
	_In_ UINT	uMsg,
	_In_ WPARAM	wParam,
	_In_ LPARAM	lParam
)
{
	// custom message processing
	switch ( uMsg )
	{
	case WM_CLOSE:
		PostQuitMessage( 69 );
		break;
	case WM_KEYDOWN:
		SetWindowText( hWnd,"Respect" );
		break;
	case WM_KEYUP:
		SetWindowText( hWnd,"Pubes" );
		break;
	default:
		break;
	}

	// default process the rest of the messages
	return DefWindowProc( hWnd,uMsg,wParam,lParam );
}

int WINAPI wWinMain(
	_In_		HINSTANCE	hInstance,
	_In_opt_	HINSTANCE	/*hPrevInstance*/,
	_In_		PWSTR		/*pCmdLine*/,
	_In_		int			/*nCmdShow*/
)
{
	// register window class
	const auto pClassName = "hw3dWndClass";

	WNDCLASSEX wc = { 0 };
	wc.cbSize		 = sizeof( wc );
	wc.style		 = CS_OWNDC;
	wc.lpfnWndProc	 = WindowProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = nullptr;
	wc.hCursor		 = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName	 = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm		 = nullptr;
	RegisterClassEx( &wc );

	// create a window
	HWND hWnd = CreateWindowEx( 
		0,
		pClassName,
		"HW3D Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200,200,800,600,
		nullptr,nullptr,
		hInstance,nullptr
	);
	// show window
	ShowWindow( hWnd,SW_SHOW );

	// process messages
	MSG msg;
	BOOL gResult;
	while ( ( gResult = GetMessage( &msg,nullptr,0,0 ) ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// process quit message
	if ( gResult == -1 )
	{
		return -1;
	}
	else
	{
		return (int)msg.wParam;
	}
}