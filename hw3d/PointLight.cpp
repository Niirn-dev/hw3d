#include "PointLight.h"
#include "imgui\imgui.h"

PointLight::PointLight( Graphics& gfx )
	:
	pos( 0.0f,0.0f,0.0f ),
	mesh( gfx,0.5f ),
	psCBuff( gfx )
{
}

void PointLight::Bind( Graphics& gfx ) noexcept
{
	psCBuff.Update( gfx,PointLightCBuffer{ pos } );
	psCBuff.Bind( gfx );
}

void PointLight::Reset() noexcept
{
	pos = { 0.0f,0.0f,0.0f };
}

void PointLight::Draw( Graphics& gfx ) const noexcept( !IS_DEBUG )
{
	mesh.SetPosition( pos );
	mesh.Draw( gfx );
}

void PointLight::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Light control" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "x",&pos.x,-20.0f,20.0f,"%.1f" );
		ImGui::SliderFloat( "y",&pos.y,-20.0f,20.0f,"%.1f" );
		ImGui::SliderFloat( "z",&pos.z,-20.0f,20.0f,"%.1f" );
		if ( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}
