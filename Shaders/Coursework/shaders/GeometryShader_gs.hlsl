cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 CameraPosition;
	float padding;
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
};

void AddTriangleArroundPoint(inout TriangleStream< OutputType > triStream, point InputType input[1], float xOffset, float yOffset, float zOffset)
{
	OutputType output;

	float4 Cam = float4(CameraPosition.x, CameraPosition.y, CameraPosition.z, 1);
	

	output.position = input[0].position + float4(xOffset, yOffset, zOffset, 0.0);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

}

[maxvertexcount(4)]
void main(
	point InputType input[1], 
	inout TriangleStream< OutputType > triStream)
{
	OutputType output;

	



	input[0].position.w = 1.0f;

	AddTriangleArroundPoint(triStream, input, -1, 1, 0);
	AddTriangleArroundPoint(triStream, input, -1, -1, 0);
	AddTriangleArroundPoint(triStream, input, 1, 1, 0);
	AddTriangleArroundPoint(triStream, input, 1, -1, 0);


	triStream.RestartStrip();

}


