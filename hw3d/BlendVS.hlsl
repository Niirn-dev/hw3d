cbuffer CBuff
{
	matrix tranform;
};

struct VSOut
{
	float4 c : Color;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float4 c : Color )
{
	VSOut vso;
	vso.pos = mul( float4( pos,1.0f ),tranform );
	vso.c = c;
	return vso;
}