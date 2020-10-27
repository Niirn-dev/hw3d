cbuffer CBuf
{
	matrix transform;
};

struct VSOut
{
	float2 tc : TexCoord;
	float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float2 tc : TexCoord )
{
	VSOut vso;
	vso.pos = mul( float4( pos,1.0f ),transform );
	vso.tc = tc;
	return vso;
}