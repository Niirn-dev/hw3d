#include "Camera.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{

	const auto vecPos = dx::XMLoadFloat3( &pos );
	const auto vecLookAt = dx::XMVectorAdd( 
		vecPos, 
		dx::XMVector3Transform( 
			dx::XMVectorSet( 0.0f,0.0f,1.0f,0.0f ),
			dx::XMMatrixRotationRollPitchYaw( -pitch,-yaw,0.0f )
		)
	);

	return dx::XMMatrixLookAtLH(
		vecPos,
		vecLookAt,
		dx::XMVectorSet( 0.0f,1.0f,0.0f,0.0f )
	);
}

void Camera::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Camera" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-40.0f,40.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-40.0f,40.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-40.0f,40.0f,"%.1f" );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Pitch",&pitch,-maxPitch,maxPitch );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	dx::XMStoreFloat3( &pos,{ 0.0f,1.0f,-10.0f } );
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	dx::XMStoreFloat3( &translation,dx::XMVector3Transform(
		dx::XMLoadFloat3( &translation ),
		dx::XMMatrixRotationRollPitchYaw( -pitch,-yaw,0.0f ) *
		dx::XMMatrixScaling( travelSpeed,travelSpeed,travelSpeed )
	) );

	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z,
	};
}

void Camera::Rotate( float dx,float dy ) noexcept
{
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = std::clamp( pitch + dy * rotationSpeed,-maxPitch,maxPitch );
}
