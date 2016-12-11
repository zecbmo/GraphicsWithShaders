cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 CameraPosition;
	float padding;
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

[maxvertexcount(4)]
void main(
	point InputType input[1],
	inout TriangleStream< OutputType > triStream)
{
	OutputType output;

	input[0].position.w = 1.0f;

	float4 NewInputPosition = input[0].position;
	NewInputPosition = mul(NewInputPosition, worldMatrix);

	float3 LookAtVec = normalize(CameraPosition - NewInputPosition.xyz);
	float3 RightVec = normalize(cross(LookAtVec, CameraUpVec));
	float3 UpVec = normalize(cross(RightVec, LookAtVec));

	RightVec *= 0.02f;
	UpVec *= 0.02f;

	float3 Pos[4];

	Pos[0] = NewInputPosition.xyz - RightVec - UpVec; // Get bottom left vertex
	Pos[1] = NewInputPosition.xyz + RightVec - UpVec; // Get bottom right vertex
	Pos[2] = NewInputPosition.xyz - RightVec + UpVec; // Get top left vertex
	Pos[3] = NewInputPosition.xyz + RightVec + UpVec; // Get top right vertex



	for (int i = 0; i < 4; i++)
	{

		float4 NewPos;
		NewPos.x = Pos[i].x;
		NewPos.y = Pos[i].y;
		NewPos.z = Pos[i].z;
		NewPos.w = 1;


		output.position = NewPos;

		//output.position = mul(output.position, worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);

		output.tex = input[0].tex;

		output.normal = mul(input[0].normal, (float3x3)worldMatrix);
		output.normal = normalize(output.normal);


		output.colour = RightVec;

		triStream.Append(output);
	}


	triStream.RestartStrip();

}


