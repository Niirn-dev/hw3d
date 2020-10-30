cbuffer CBuff
{
    matrix world;
    matrix worldViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 n : Normal;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position,float3 n : Normal )
{
    VSOut vso;
    vso.worldPos = (float3)mul(float4(pos, 1.0f), world);
    vso.n = mul(n, (float3x3)world);
    vso.pos = mul(float4(pos, 1.0f), worldViewProj);
	return vso;
}