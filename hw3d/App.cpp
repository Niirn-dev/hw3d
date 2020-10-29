#include "App.h"
#include "Box.h"
#include "Pyramid.h"
#include "Spheroid.h"
#include <memory>
#include <random>
#include <algorithm>
#include <iterator>
#include "Surface.h"
#include "Sheet.h"
#include "SkinnedBox.h"
#include "GDIPlusManager.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

GDIPlusManager gdipm;

App::App( std::optional<int> wndWidth,std::optional<int> wndHeight,std::optional<std::string> wndName )
    :
    wnd( Window{ wndWidth.value_or( wndWidthDefault ),wndHeight.value_or( wndHeightDefault ),wndName.value_or( "HW3D Window" ).c_str() } )
{
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> rDist( 0.0f,20.0f );
	std::uniform_real_distribution<float> aDist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> sDist( 0.0f,3.1415f * 0.3f );
	std::uniform_real_distribution<float> distortionDist( 0.8f,1.6f );
	std::uniform_int_distribution<int> divDist( 4,48 );
	std::uniform_int_distribution<int> shapeDist( 0,4 );

	std::generate_n(
		std::back_inserter( drawables ),
		140,
		[&]() -> std::unique_ptr<Drawable>
		{
			switch ( shapeDist( rng ) )
			{
			case 0:
				return std::make_unique<Box>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			case 1:
				return std::make_unique<Pyramid>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			case 2:
				return std::make_unique<Spheroid>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist,divDist );
			case 3:
				return std::make_unique<Sheet>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			case 4:
				return std::make_unique<SkinnedBox>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			default:
				assert( "Wrong shape type" && false );
				return std::make_unique<Box>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
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
	const float dt = timer.Mark() * simulationSpeedFactor;
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetView( cam.GetTranformXM() );

	for ( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}

	while ( !wnd.kbd.KeyIsEmpty() )
	{
		const auto& e = wnd.kbd.ReadKey();
		if ( e->IsPress() && e->GetCode() == 'I' )
		{
			if ( wnd.Gfx().IsImguiEnabled() )
			{
				wnd.Gfx().DisableImgui();
			}
			else
			{
				wnd.Gfx().EnableImgui();
			}
		}
	}

	if ( ImGui::Begin( "Simulation speed control" ) )
	{
		ImGui::SliderFloat( "Factor",&simulationSpeedFactor,0.2f,4.0f,"%.1f" );
		ImGui::Text( "Simulation framerate: %.1f",ImGui::GetIO().Framerate );
		ImGui::Text( wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (Hold space bar to pause)" );
	}
	ImGui::End();

	cam.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
