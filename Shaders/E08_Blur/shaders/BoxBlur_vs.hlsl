cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	
};

cbuffer ScreenSizeBuffer : register(b1)
{
	float screenWidth;
	float screenHeight;
	float2 padding;
};


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
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
};


OutputType main(InputType input)
{
	OutputType output;
	float texelWidth, texelHeight;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Determine the floating point size of a texel for a screen with this specific width.
	texelWidth = 1.0f / screenWidth;
	texelHeight = 1.0f / screenHeight;

	// Create UV coordinates for the pixel and its four horizontal neighbors on either side.
	output.texCoord1 = input.tex + float2(-texelWidth, -texelHeight);
	output.texCoord2 = input.tex + float2(0.0f, texelHeight);
	output.texCoord3 = input.tex + float2(texelWidth, -texelHeight);
	output.texCoord4 = input.tex + float2(-texelWidth, 0.0f);
	output.texCoord5 = input.tex + float2(texelWidth, 0.0f);
	output.texCoord6 = input.tex + float2(-texelWidth, texelHeight);
	output.texCoord7 = input.tex + float2(0.0f, texelHeight);
	output.texCoord8 = input.tex + float2(texelWidth, texelHeight);

	return output;
}
