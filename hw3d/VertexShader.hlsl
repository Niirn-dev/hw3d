struct VSOut
{
	float3 color : COLOR;
	float4 pos : SV_POSITION;
};

cbuffer cBuff
{
	matrix transformation;
};

VSOut main( float2 pos : POSITION,float3 c : COLOR )
{
	VSOut vsout;
	vsout.pos = mul( float4( pos.x,pos.y,0.0f,1.0f ),transformation );
	vsout.color = c;
	return vsout;
}