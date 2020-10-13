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
	const auto [mx,my] = wnd.mouse.GetPos();
	while ( !wnd.mouse.IsEmpty() )
	{
		const auto e = wnd.mouse.Read();
		switch ( e->GetType() )
		{
		case Mouse::Event::Type::LPress:
			offsetZ += 0.5f;
			break;
		case Mouse::Event::Type::RPress:
			offsetZ -= 0.5f;
			break;
		default:
			break;
		}
	}
	wnd.Gfx().DrawTestTriangle( (float)mx / 400.0f - 1.0f,(float)-my / 300.0f + 1.0f,offsetZ,timer.Peek() );
	wnd.Gfx().EndFrame();
}
