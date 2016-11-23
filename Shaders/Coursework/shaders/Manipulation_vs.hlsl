// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D displacementMap : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer CameraBuffer : register(b1)
{
	float3 cameraPosition;
	float padding;
}

cbuffer TimeBuffer : register(b2)
{
	float time;
	float height;
	float isDisplacementMap;
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
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;
	

	
};

OutputType main(InputType input)
{
    OutputType output;
	
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	float4 alpha;

	[branch] switch(isDisplacementMap )
	{
	case 0:
		//do sin wave stuff here
		input.position.y = input.position.y + (height * sin(input.position.x + time))* (height * cos(input.position.z + time));
		input.position.z = input.position.z + (height * sin(input.position.x + time))* (height * cos(input.position.y + time));
		break;
	case 1:
		alpha = displacementMap.SampleLevel(Sampler0, input.tex,0 );
		input.position.y += alpha.x * height;

		break;
	}
	

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	 // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	//world pos of vertex
	output.position3D = mul(input.position, worldMatrix);


	//view direction
	float4 worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	

    return output;
}