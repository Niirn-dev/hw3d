struct GSInOut
{
    float3 worldViewPos : Position;
    float3 n : Normal;
    float2 tc : TexCoord;
	float4 pos : SV_Position;
};

[maxvertexcount(3)]
void main(
	triangle GSInOut input[3] : SV_Position,
	inout TriangleStream<GSInOut> triStream
)
{
    GSInOut gso;
	
	// calculate normals
    float3 edge1 = input[1].worldViewPos - input[0].worldViewPos;
    float3 edge2 = input[2].worldViewPos - input[0].worldViewPos;
    float3 faceNormal = normalize(cross( edge1,edge2 ));
	
    // add normal data to each vertex of the triangle
	for (uint i = 0; i < 3; i++)
	{
        gso.worldViewPos = input[i].worldViewPos;
        gso.n = faceNormal;
        gso.tc = input[i].tc;
        gso.pos = input[i].pos;
        // append augmented vertex to the output stream
        triStream.Append(gso);
    }
    triStream.RestartStrip();
}