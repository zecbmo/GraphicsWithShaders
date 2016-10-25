// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
	float4 ambientColor;
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
    float3 lightDir;
    float lightIntensity;
    float4 color;

	//set default colour value
	color = ambientColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));



	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    
	if (lightIntensity > 0)
	{
		color += saturate((diffuseColor) * lightIntensity);
		
	}


    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	if (padding > 0)
	{
		color = color *textureColor;
	}
	

    return color;
}

