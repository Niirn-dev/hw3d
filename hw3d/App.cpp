#include "App.h"
#include <sstream>
#include <iomanip>

App::App( std::optional<int> wndWidth,std::optional<int> wndHeight,std::optional<std::string> wndName )
    :
    wnd( Window{ wndWidth.value_or( wndWidthDefault ),wndHeight.value_or( wndHeightDefault ),wndName.value_or( "HW3D Window" ).c_str() } )
{}

int App::Go()
{
	while ( true )
	{
		if ( const auto ecode = Window::ProcessMessages() )
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = 0.5f * std::sinf( timer.Peek() ) + 0.5f;
	wnd.Gfx().ClearBuffer( c,1.0f - c,1.0f );

	wnd.Gfx().EndFrame();
}
