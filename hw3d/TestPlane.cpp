#include "TestPlane.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "Plane.h"
#include "imgui\imgui.h"

TestPlane::TestPlane( Graphics& gfx,float size )
{
	namespace dx = DirectX;
	using namespace Bind;
	using namespace std::string_literals;

	auto model = Plane::MakeTextured( 1 );
	model.Transform( dx::XMMatrixScaling( size,size,1.0f ) );
	const auto geometryTag = "$TestPlane."s + std::to_string( size );
	AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

	auto pvs = VertexShader::Resolve( gfx,"PhongVS.cso"s );
	const auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx,"PhongPSNormal.cso"s ) );

	AddBind( PixelConstantBuffer<TestPlane::PSMaterialConstant>::Resolve( gfx,pmc,1u ) );

	AddBind( Texture::Resolve( gfx,"Images\\brickwall.jpg"s ) );
	AddBind( Texture::Resolve( gfx,"Images\\brickwall_normal.jpg"s,1u ) );
	AddBind( Sampler::Resolve( gfx ) );

	AddBind( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

	AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbufDoubleboi>( gfx,*this,std::nullopt,2u ) );
}

void TestPlane::SetPosition( DirectX::XMFLOAT3 pos_in ) noexcept
{
	pos = pos_in;
}

void TestPlane::SetRotation( float roll_in,float pitch_in,float yaw_in ) noexcept
{
	roll = roll_in;
	pitch = pitch_in;
	yaw = yaw_in;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) * 
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}

void TestPlane::SpawnControlWindow( Graphics& gfx ) noexcept
{
	if ( ImGui::Begin( "Test plane" ) )
	{
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );

		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&pos.y,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&pos.z,-60.0f,60.0f,"%.1f" );

		if ( ImGui::Button( "Reset" ) )
		{
			ResetControls();
		}

		ImGui::Text( "Shading" );
		const auto changedIntensity = ImGui::SliderFloat( "Spec. Intensity",&pmc.specularIntensity,0.0f,10.0f,"%.2f" );
		const auto changedPower = ImGui::SliderFloat( "Spec. Power",&pmc.specularPower,0.0f,160.0f,"%.1f" );
		bool normMapEnabled = pmc.normalMapEnabled == TRUE;
		const auto changedNormMode = ImGui::Checkbox( "Normal map enabled",&normMapEnabled );
		
		if ( changedIntensity || changedPower || changedNormMode )
		{
			pmc.normalMapEnabled = normMapEnabled ? TRUE : FALSE;
			QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx,pmc );
		}
	}
	ImGui::End();
}

void TestPlane::ResetControls() noexcept
{
	pos = { 1.0f,17.0f,-1.0f };
	roll = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
}