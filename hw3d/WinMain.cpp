#include "Window.h"

int WINAPI wWinMain(
	_In_		HINSTANCE	/*hInstance*/,
	_In_opt_	HINSTANCE	/*hPrevInstance*/,
	_In_		PWSTR		/*pCmdLine*/,
	_In_		int			/*nCmdShow*/
)
{
	// create a window
	Window wnd{ 800,600,"HW3D Window" };

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