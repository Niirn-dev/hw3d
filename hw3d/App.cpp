#include "App.h"
#include "Box.h"
#include "Spheroid.h"
#include <memory>
#include <random>
#include <algorithm>
#include <iterator>
#include "SkinnedBox.h"
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
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> rDist( 5.0f,25.0f );
	std::uniform_real_distribution<float> aDist( 0.0f,3.1415f * 2.0f );
	std::uniform_real_distribution<float> sDist( 0.0f,3.1415f * 0.3f );
	std::uniform_real_distribution<float> distortionDist( 0.8f,1.6f );
	std::uniform_int_distribution<int> divDist( 4,48 );
	 std::uniform_int_distribution<int> shapeDist( 0,1 );

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
			 	return std::make_unique<SkinnedBox>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			// case 2:
			// 	return std::make_unique<Spheroid>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist,divDist );
			// case 3:
			// 	return std::make_unique<Sheet>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			// case 4:
			// 	return std::make_unique<Pyramid>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			 default:
			 	assert( "Wrong shape type" && false );
			 	return std::make_unique<Box>( wnd.Gfx(),rng,rDist,aDist,sDist,distortionDist );
			 }
		} );

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

#include "Vertex.h"
void f()
{
	namespace dx = DirectX;
	VertexData vd{ std::move( 
		VertexLayout{}.Append( VertexLayout::ElementType::Position3D )
			.Append( VertexLayout::ElementType::Normal )
			.Append( VertexLayout::ElementType::Texture2D )
	) };
	vd.EmplaceBack(
		dx::XMFLOAT3{ 1.0f,1.0f,5.0f },
		dx::XMFLOAT3{ 2.0f,1.0f,4.0f },
		dx::XMFLOAT2{ 6.0f,9.0f }
	);
	vd.EmplaceBack(
		dx::XMFLOAT3{ 6.0f,9.0f,6.0f },
		dx::XMFLOAT3{ 9.0f,6.0f,9.0f },
		dx::XMFLOAT2{ 4.2f,0.0f }
	);
	auto pos = vd[0].Attr<VertexLayout::ElementType::Position3D>();
	auto nor = vd[0].Attr<VertexLayout::ElementType::Normal>();
	auto tex = vd[1].Attr<VertexLayout::ElementType::Texture2D>();
	vd.Back().Attr<VertexLayout::ElementType::Position3D>().z = 420.0f;
	pos = vd.Back().Attr<VertexLayout::ElementType::Position3D>();
	const auto& cvd = vd;
	pos = cvd[1].Attr<VertexLayout::ElementType::Position3D>();
}

void App::DoFrame()
{
	f();

	const float dt = timer.Mark() * simulationSpeedFactor;
	wnd.Gfx().BeginFrame( bkgColor.r,bkgColor.g,bkgColor.b );
	wnd.Gfx().SetView( cam.GetTranformXM() );

	light.Bind( wnd.Gfx() );

	for ( auto& d : drawables )
	{
		d->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt );
		d->Draw( wnd.Gfx() );
	}
	light.Draw( wnd.Gfx() );

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
		ImGui::SliderFloat( "Factor",&simulationSpeedFactor,0.2f,4.0f,"%.1f" );
		ImGui::Text( "Simulation framerate: %.1f",ImGui::GetIO().Framerate );
		ImGui::Text( wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (Hold space bar to pause)" );
	}
	ImGui::End();

	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	wnd.Gfx().EndFrame();
}
