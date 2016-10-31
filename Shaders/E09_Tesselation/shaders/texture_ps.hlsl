// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry


Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
	
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = texture0.Sample(Sampler0, input.tex);
	
	//float4 red = float4(1,0,0,1);
	//float invert = float4(1 - textureColor.x, 1 - textureColor.y, 1 - textureColor.z, 1);

	//textureColor = lerp(textureColor, red, 0.50);

    return textureColor;
}