cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
};

Texture2D tex;
Texture2D normalMap;
SamplerState smplr;

float4 main( float3 worldPos : Position,float3 n : Normal,float2 tc : Texcoord ) : SV_Target
{
	if ( normalMapEnabled )
	{
		// read raw normal values from the map
		const float3 norm = normalMap.Sample( smplr,tc ).rgb;
		// convert normal values
		n.x = norm.x * 2.0f - 1.0f;
		n.y = -norm.y * 2.0f - 1.0f;
		n.z = -norm.z * 2.0f - 1.0f;
	}
	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	// attenuation
	const float att = 1.0f / ( attConst + attLin * distToL + attQuad * ( distToL * distToL ) );
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f,dot( dirToL,n ) );
	// reflected light vector
	const float3 w = n * dot( vToL,n );
	const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * ( diffuseColor * diffuseIntensity ) * specularIntensity * pow( max( 0.0f,dot( normalize( -r ),normalize( worldPos ) ) ),specularPower );
	// final color
	return float4( saturate( ( diffuse + ambient ) * tex.Sample( smplr, tc ).rgb + specular ), 1.0f );
}