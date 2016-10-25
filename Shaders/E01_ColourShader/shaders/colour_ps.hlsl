// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{


	float r = 0.0;
	float g = 0;
	float b =  1;

	
	if(fmod(input.position.x, 2) == 0)
	{
		r = 1.0;
	}
	
	
	
	float4 colour = float4(r, g, b, 1.0);
	

	return colour;
}