#include "Window.h"
#include "ChiliException.h"
#include <sstream>

int WINAPI wWinMain(
	_In_		HINSTANCE	/*hInstance*/,
	_In_opt_	HINSTANCE	/*hPrevInstance*/,
	_In_		PWSTR		/*pCmdLine*/,
	_In_		int			/*nCmdShow*/
)
{
	try
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

			if ( !wnd.mouse.IsEmpty() )
			{
				static int count = 0;
				const auto e = wnd.mouse.Read();
				switch ( e->GetType() )
				{
				case Mouse::Event::Type::WheelUp:
				{
					++count;
					std::stringstream oss;
					oss << "Up:\t" << count;
					wnd.SetTitle( oss.str() );
				}
					break;
				case Mouse::Event::Type::WheelDown:
				{
					--count;
					std::stringstream oss;
					oss << "Down:\t" << count;
					wnd.SetTitle( oss.str() );
				}
				break;
				default:
					break;
				}
			}
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
	catch ( const ChiliException& e )
	{
		MessageBox( nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( const std::exception& e )
	{
		MessageBox( nullptr,e.what(),"Standard exception",MB_OK | MB_ICONEXCLAMATION );
	}
	catch ( ... )
	{
		MessageBox( nullptr,"No details available","Unknown exception",MB_OK | MB_ICONEXCLAMATION );
	}

	return -1;
}