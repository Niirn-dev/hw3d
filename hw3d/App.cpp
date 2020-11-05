#include "App.h"
#include "Box.h"
#include "Spheroid.h"
#include <memory>
#include <random>
#include <algorithm>
#include <iterator>
#include "SkinnedBox.h"
#include "AssTest.h"
#include "GDIPlusManager.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_impl_dx11.h"

GDIPlusManager gdipm;

App::App( std::optional<int> wndWidth,std::optional<int> wndHeight,std::optional<std::string> wndName )
	:
	wnd( Window{ wndWidth.value_or( wndWidthDefault ),wndHeight.value_or( wndHeightDefault ),wndName.value_or( "HW3D Window" ).c_str() } ),
	light( wnd.Gfx() )
{
	wnd.Gfx().SetProjection( DirectX::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,60.0f ) );
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
	wnd.Gfx().BeginFrame( bkgColor.r,bkgColor.g,bkgColor.b );
	wnd.Gfx().SetView( cam.GetTranformXM() );

	light.Bind( wnd.Gfx() );
	light.Draw( wnd.Gfx() );
	nano.Draw( wnd.Gfx() );

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

	if ( ImGui::Begin( "Simulation control" ) )
	{
		ImGui::ColorEdit3( "Background",&bkgColor.r );
		ImGui::Text( "Simulation framerate: %.1f",ImGui::GetIO().Framerate );
	}
	ImGui::End();

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
