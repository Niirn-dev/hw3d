cbuffer LightCBuff
{
    float3 lightPos;
};

static const float3 materialColor = { 0.1f, 0.1f, 0.2f };
static const float3 ambient = { 0.12f, 0.0f, 0.07f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.22f;
static const float attQuad = 0.2f;

float4 main( float3 worldPos : Position,float3 n : Normal ) : SV_Target
{
    // get direction and distance to light source
    const float3 toL = lightPos - worldPos;
    const float distToL = length(toL);
    const float3 dirToL = normalize(toL);
    // diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    // final color
    return float4(saturate(diffuse + ambient + materialColor), 1.0f);
}