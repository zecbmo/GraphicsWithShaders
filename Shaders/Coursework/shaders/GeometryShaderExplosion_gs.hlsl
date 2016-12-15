cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 CameraPosition;
	float explosionoffset;
	float3 CameraUpVec;
	float padding2;
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 colour : TEXCOORD1;

};


//help from http://www.gamedev.net/topic/656598-rotate-billboard-in-geometry-shader/

[maxvertexcount(3)]
void main(
	triangle InputType input[3],
	inout TriangleStream< OutputType > triStream)
{
	OutputType output;


	float4 pos[3];
	for (int k = 0; k < 3; k++)
	{
		pos[k] = input[k].position;
		pos[k].w = 1;
	}


	float4 faceEdgeA = pos[1] - pos[0];
	float4 faceEdgeB = pos[2] - pos[0];
	float3 faceNormal = normalize(cross(faceEdgeA, faceEdgeB));

	faceNormal = mul(faceNormal, (float3x3)worldMatrix);
	faceNormal = normalize(faceNormal);

	float3 offset = faceNormal * -explosionoffset;
	float4 posOffset;

	posOffset.x = offset.x;
	posOffset.y = offset.y;
	posOffset.z = offset.z;
	posOffset.w = 1;

	for (int i = 0; i < 3; i++)
	{



		output.normal = mul(input[i].normal, (float3x3)worldMatrix);
		output.normal = normalize(output.normal);

		output.position = mul(pos[i], worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);

		output.tex = input[i].tex;
		
		output.position = output.position + posOffset;
		

		output.colour = float4(1, 0, 0, 1);

		triStream.Append(output);
	}

	triStream.RestartStrip();
}


