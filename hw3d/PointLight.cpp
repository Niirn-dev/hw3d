#include "PointLight.h"
#include "imgui\imgui.h"

PointLight::PointLight( Graphics& gfx )
	:
	mesh( gfx,0.5f ),
	psCBuff( gfx,1u )
{
}

void PointLight::Bind( Graphics& gfx ) noexcept
{
	PointLightCBuffer plCBuf = {
		pos,
		color,
		ambient,
		intensity,
		attConst,
		attLin,
		attQuad
	};
	psCBuff.Update( gfx,plCBuf );
	psCBuff.Bind( gfx );
}

void PointLight::Draw( Graphics& gfx ) const noexcept( !IS_DEBUG )
{
	mesh.SetPosition( pos );
	mesh.SetColor( color );
	mesh.UpdateCBuffer( gfx );
	mesh.Draw( gfx );
}

void PointLight::SpawnControlWindow() noexcept
{
	static enum class WindowContents
	{
		POSITION,
		COLORS,
		ATTENUATION
	} contents = WindowContents::POSITION;

	if ( ImGui::Begin( "Light control",nullptr,ImGuiWindowFlags_MenuBar ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			if ( ImGui::BeginMenu( "Settings selection" ) )
			{
				if ( ImGui::MenuItem( "Position" ) )
				{
					contents = WindowContents::POSITION;
				}
				if ( ImGui::MenuItem( "Light colors" ) )
				{
					contents = WindowContents::COLORS;
				}
				if ( ImGui::MenuItem( "Attenuation" ) )
				{
					contents = WindowContents::ATTENUATION;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		switch ( contents )
		{
		case WindowContents::POSITION:
			// ImGui::Text( "Position" );
			ImGui::SliderFloat( "X",&pos.x,-20.0f,20.0f,"%.1f" );
			ImGui::SliderFloat( "Y",&pos.y,-20.0f,20.0f,"%.1f" );
			ImGui::SliderFloat( "Z",&pos.z,-20.0f,20.0f,"%.1f" );
			if ( ImGui::Button( "Reset" ) )
			{
				ResetPosition();
			}
			break;
		case WindowContents::COLORS:
			// ImGui::Text( "Light colors" );
			ImGui::ColorEdit3( "Point light",&color.x );
			ImGui::SliderFloat( "Intensity",&intensity,0.2f,2.0f,"%.1f" );
			ImGui::ColorEdit3( "Ambient light",&ambient.x );
			if ( ImGui::Button( "Reset" ) )
			{
				ResetColors();
			}
			break;
		case WindowContents::ATTENUATION:
			// ImGui::Text( "Attenuation" );
			ImGui::SliderFloat( "Constant",&attConst,0.0f,2.0f,"%.1f" );
			ImGui::SliderFloat( "Linear",&attLin,0.0f,2.0f,"%.2f" );
			ImGui::SliderFloat( "Quadratic",&attQuad,0.0f,2.0f,"%.2f" );
			if ( ImGui::Button( "Reset" ) )
			{
				ResetAttenuation();
			}
			break;
		default:
			break;
		}
	}
	ImGui::End();
}

void PointLight::ResetPosition() noexcept
{
	pos = { 0.0f,0.0f,0.0f };
}

void PointLight::ResetColors() noexcept
{
	color = { 1.0f,1.0f,1.0f };
	intensity = 0.0f;
	ambient = { 0.12f,0.0f,0.07f };
}

void PointLight::ResetAttenuation() noexcept
{
	attConst = 1.0f;
	attLin = 0.22f;
	attQuad = 0.2f;
}
