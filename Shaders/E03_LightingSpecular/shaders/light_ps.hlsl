// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor;
	float4 ambientColor;
    float3 lightDirection;
   	float specularPower;
	float4 specularColor;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float3 reflection;
	float4 specular;
	float4 finalSpec;

	//set default colour value
	color = ambientColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Invert the light direction for calculations.
    //lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(input.normal, lightDir));

	lightDir = normalize(lightDirection);

	lightIntensity = saturate(dot(input.normal, -lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
		color = saturate(color);

		//reflection
		reflection = reflect(lightDir, input.normal);

		//determin spec
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//sum spec
		finalSpec = specularColor * specular;

	}



    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;
	color = saturate(color + finalSpec);

    return color;
}

