// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry


//articles that helped http://paulbourke.net/stereographics/anaglyph/

Texture2D texture1 : register(t0);
Texture2D texture2 : register(t1);
SamplerState Sampler0 : register(s0);

cbuffer DissolveBuffer : register(b0)
{
	float dissolveAmount;
	float fringeAmount;
	float2 padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};



float4 main(InputType input) : SV_TARGET
{
	float4 textureColorRed;
	float4 textureColorBlue;

	float4 Red = float4(1.0f, 0,0, 1.0f);
	float4 Blue = float4(0.0f, 1.00, 1.0f, 1.0f);


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	/*textureColorRed = 
	textureColorBlue = texture2.Sample(Sampler0, input.tex);*/

	////saturate the colour values
	//float greyscale = dot(textureColorRed.rgb, float3(0.30, 0.59, 0.11));
	//textureColorRed.rgb = lerp(greyscale, textureColorRed.rgb, 0);	
	////	- multiply the new colour values with the filter colours(red and cyan)
	//textureColorRed = lerp(textureColorRed, Red, 0.5);
	//
	////saturate the colour values
	//float greyscale2 = dot(textureColorBlue.rgb, float3(0.30, 0.59, 0.11));
	//textureColorBlue.rgb = lerp(greyscale2, textureColorBlue.rgb, 0);
	////	- multiply the new colour values with the filter colours(red and cyan)
	//textureColorBlue = lerp(textureColorBlue, Blue, 0.5);


	//float4 FinalColour = textureColorBlue * textureColorRed;

	//	- then add them together and output the colour



	float4 Color1;
	Color1 = texture2.Sample(Sampler0, input.tex);;

	float4 Color2;
	Color2 = texture1.Sample(Sampler0, input.tex);;

	Color1.r = Color2.r;
	Color1.g = Color1.g;
	Color1.b = Color1.b;
	Color1.a = max(Color1.a, Color2.a);

	return Color1;




//return FinalColour;
}