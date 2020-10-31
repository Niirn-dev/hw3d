cbuffer CBuff
{
    matrix worldView;
    matrix worldViewProj;
};

struct VSOut
{
    float3 worldViewPos : Position;
    float3 n : Normal;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float3 n : Normal )
{
    VSOut vso;
    vso.worldViewPos = (float3)mul(float4(pos, 1.0f), worldView);
    vso.n = (float3) mul(float4(n, 0.0f), worldView);
    vso.pos = mul(float4(pos, 1.0f), worldViewProj);
	return vso;
}