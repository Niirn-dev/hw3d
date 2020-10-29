#include "Camera.h"
#include "imgui\imgui.h"

DirectX::XMMATRIX Camera::GetTranformXM() const noexcept
{
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet( 0.0f,0.0f,r,0.0f ),
		DirectX::XMMatrixRotationRollPitchYaw( phi,-theta,0.0f )
	);

	return DirectX::XMMatrixLookAtLH(
		pos,
		DirectX::XMVectorZero(),
		DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f )
	) * DirectX::XMMatrixRotationRollPitchYaw( pitch,-yaw,roll );
}

void Camera::SpawnControlWindow() noexcept
{
	if ( ImGui::Begin( "Camera control" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "R",&r,1.0f,50.0f,"%.1f" );
		ImGui::SliderAngle( "Theta",&theta,-180.0f,180.0f );
		ImGui::SliderAngle( "Phi",&phi,-89.0f,89.0f );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Pitch",&pitch,-90.0f,90.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		ImGui::SliderAngle( "Roll",&roll,-45.0f,45.0f );
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;

	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}