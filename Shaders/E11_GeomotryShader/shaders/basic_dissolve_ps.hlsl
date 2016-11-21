// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry


Texture2D texture0 : register(t0);
Texture2D dissolveMap : register(t1);
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
    float4 textureColor;
	float4 dissolveColor;
	float dissolveMapValue;
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = texture0.Sample(Sampler0, input.tex);

	//Get the dissolve colour from dissolve map
	dissolveColor = dissolveMap.Sample(Sampler0, input.tex);
	//as this should be a grayscale image we can assume rgb values will all be the same
	//we can then use the value for R as the dissolve map value which will be compare against the amount
	dissolveMapValue = dissolveColor.x;
	

	if (dissolveMapValue <= dissolveAmount)
	{
		textureColor = float4(0.0f,0.0f, 0.0f, 0.0f);		
	}
	else if ((dissolveMapValue <= dissolveAmount + fringeAmount) && dissolveAmount != 0)
	{
		textureColor = float4(0.0f, 10.0f, 10.0f, 10.0f);
	}


    return textureColor;
}