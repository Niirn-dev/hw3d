#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexShader.h"
#include "Sampler.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 1280,720,"The Donkey Fart Box" ),
	light( wnd.Gfx() )
{
	plane.SetPosition( { 1.0f,17.0f,-1.0f } );
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,40.0f ) );
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;

	if ( !wnd.CursorEnabled() )
	{
		DoCameraControls( dt );
	}

	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );
		
	nano.Draw( wnd.Gfx() );
	plane.Draw( wnd.Gfx() );
	light.Draw( wnd.Gfx() );

	while( const auto e = wnd.kbd.ReadKey() )
	{
		if( e->IsPress() )
		{
			switch ( e->GetCode() )
			{
			case 'I':
				if ( wnd.CursorEnabled() )
				{
					wnd.DisableCursor();
					wnd.mouse.EnableRaw();
				}
				else
				{
					wnd.EnableCursor();
					wnd.mouse.DisableRaw();
				}
				break;
			case VK_F1:
				show_demo_window = true;
				break;
			default:
				break;
			}
		}
	}
		
	// imgui windows
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	nano.ShowWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::DoCameraControls( float dt ) noexcept
{
	// do rotation
	float dx = 0.0f,dy = 0.0f;
	assert( wnd.mouse.RawEnabled() );
	while ( const auto d = wnd.mouse.ReadRawDelta() )
	{
		dx += (float)d->x;
		dy += (float)d->y;
	}
	cam.Rotate( dx * dt,dy * dt );

	// do movement
	if ( wnd.kbd.KeyIsPressed( 'W' ) )
	{
		cam.Translate( dx::XMFLOAT3{ 0.0f,0.0f,dt } );
	}
	if ( wnd.kbd.KeyIsPressed( 'S' ) )
	{
		cam.Translate( dx::XMFLOAT3{ 0.0f,0.0f,-dt } );
	}
	if ( wnd.kbd.KeyIsPressed( 'A' ) )
	{
		cam.Translate( dx::XMFLOAT3{ -dt,0.0f,0.0f } );
	}
	if ( wnd.kbd.KeyIsPressed( 'D' ) )
	{
		cam.Translate( dx::XMFLOAT3{ dt,0.0f,0.0f } );
	}
	if ( wnd.kbd.KeyIsPressed( 'R' ) )
	{
		cam.Translate( dx::XMFLOAT3{ 0.0f,dt,0.0f } );
	}
	if ( wnd.kbd.KeyIsPressed( 'F' ) )
	{
		cam.Translate( dx::XMFLOAT3{ 0.0f,-dt,0.0f } );
	}
}

void App::ShowImguiDemoWindow()
{
	if( show_demo_window )
	{
		ImGui::ShowDemoWindow( &show_demo_window );
	}
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}