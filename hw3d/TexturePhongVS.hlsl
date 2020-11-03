cbuffer CBuff
{
    matrix worldView;
    matrix worldViewProj;
};

struct VSOut
{
    float3 worldViewPos : Position;
    float3 n : Normal;
    float2 tc : TexCoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : TexCoord)
{
    VSOut vso;
    vso.worldViewPos = (float3) mul(float4(pos, 1.0f), worldView);
    vso.tc = tc;
    vso.pos = mul(float4(pos, 1.0f), worldViewProj);
    return vso;
}