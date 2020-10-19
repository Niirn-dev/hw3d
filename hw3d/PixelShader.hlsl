cbuffer CBuff
{
	float4 color[6];
};

float4 main( uint id : SV_PrimitiveID ) : SV_TARGET
{
	return float4( color[id / 2] );
}