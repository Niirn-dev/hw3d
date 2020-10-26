#include "App.h"
#include "Box.h"
#include "Pyramid.h"
#include "Spheroid.h"
#include <memory>
#include <random>
#include <algorithm>
#include <iterator>

App::App( std::optional<int> wndWidth,std::optional<int> wndHeight,std::optional<std::string> wndName )
    :
    wnd( Window{ wndWidth.value_or( wndWidthDefault ),wndHeight.value_or( wndHeightDefault ),wndName.value_or( "HW3D Window" ).c_str() } )
{
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> rDist( 0.0f,20.0f );
	std::uniform_real_distribution<float> aDist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> sDist( 0.0f,3.1415f * 0.3f );
	std::uniform_int_distribution<int> shapeDist( 1,3 );

	std::generate_n(
		std::back_inserter( drawables ),
		180,
		[&]() -> std::unique_ptr<Drawable>
		{
			switch ( shapeDist( rng ) )
			{
			case 1:
				return std::make_unique<Box>( wnd.Gfx(),rng,rDist,aDist,sDist );
			case 2:
				return std::make_unique<Pyramid>( wnd.Gfx(),rng,rDist,aDist,sDist );
			case 3:
				return std::make_unique<Spheroid>( wnd.Gfx(),rng,rDist,aDist,sDist );
			default:
				assert( "Wrong shape type" && false );
				return std::make_unique<Box>( wnd.Gfx(),rng,rDist,aDist,sDist );
			}
		} );

	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

App::~App()
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
	const float dt = timer.Mark();
	wnd.Gfx().ClearBuffer( 0.07f,0.0f,0.12f );
	for ( auto& d : drawables )
	{
		d->Update( dt );
		d->Draw( wnd.Gfx() );
	}
	wnd.Gfx().EndFrame();
}
