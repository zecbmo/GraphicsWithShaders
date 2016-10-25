// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
	float3 position;// lightDirection;
    float padding;
	float4 ambientColor;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D: TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	//attenuation
	float const_factor = 1.0;
	float linear_factor = 0.25;
	float quad_factor = 0.0;
	float range = 25;
	//set default colour value
	color = ambientColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Invert the light direction for calculations.
    lightDir = input.position3D - position;

	float distance = length(lightDir);
	
	if (distance < range)
	{
		normalize(lightDir);
		
		// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(input.normal, -lightDir));

		// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
		if (lightIntensity > 0)
		{
			float4 difComp = diffuseColor * lightIntensity;
			float attenVal = 1.0f / (const_factor + linear_factor * distance + quad_factor * pow(distance,2));
			color += saturate((difComp)* attenVal);
		}
	}

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;

    return color;
}

