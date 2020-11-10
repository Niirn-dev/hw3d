cbuffer ModelCBuff
{
    float specularIntensity;
    float specularPower;
};

cbuffer LightCBuff
{
    float3 lightWorldViewPos;
    float3 diffuseColor;
    float3 ambient;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

Texture2D tex;

SamplerState smplr;

float4 main(float3 worldViewPos : Position, float3 n : Normal, float2 tc : TexCoord) : SV_Target
{
    // get direction and distance to light source
    const float3 toL = lightWorldViewPos - worldViewPos;
    const float distToL = length(toL);
    const float3 dirToL = normalize(toL);
        
    // diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    // reflection vector
    const float3 r = 2.0f * n * dot(toL, n) - toL;
    // specular intensity
    const float3 specular = att * diffuseColor * diffuseIntensity * specularIntensity * max(0.0f, pow(dot(normalize(-worldViewPos), normalize(r)), specularPower));
    
    // final color
    return saturate(float4((diffuse + ambient + specular), 1.0f) * tex.Sample(smplr, tc));
}