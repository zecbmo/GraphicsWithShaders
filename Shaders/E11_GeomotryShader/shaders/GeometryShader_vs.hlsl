// texture vertex shader
// Basic shader for rendering textured geometry

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};
InputType main(InputType input)
{


	return input;
}