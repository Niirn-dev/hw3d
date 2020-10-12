cbuffer cBuff
{
	matrix transformation;
};

float4 main( float2 pos : POSITION ) : SV_POSITION
{
	float4 o = mul( float4(pos.x,pos.y,0.0f,1.0f),transformation );
	return o;
}