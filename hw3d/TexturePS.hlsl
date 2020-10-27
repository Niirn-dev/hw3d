Texture2D tex;

SamplerState smplr;

float4 main( float2 tc : TexCoord ) : SV_TARGET
{
	return tex.Sample( smplr,tc );
}