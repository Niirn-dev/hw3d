#include "App.h"
#include <sstream>
#include <iomanip>

App::App( std::optional<int> wndWidth,std::optional<int> wndHeight,std::optional<std::string> wndName )
    :
    wnd( Window{ wndWidth.value_or( wndWidthDefault ),wndHeight.value_or( wndHeightDefault ),wndName.value_or( "HW3D Window" ).c_str() } )
{}

int App::Go()
{
	// process messages
	MSG msg;
	BOOL gResult;
	while ( ( gResult = GetMessage( &msg,nullptr,0,0 ) ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );

		DoFrame();
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

void App::DoFrame()
{
	const auto t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision( 1 ) << std::fixed << t << "s";
	wnd.SetTitle( oss.str() );
}
