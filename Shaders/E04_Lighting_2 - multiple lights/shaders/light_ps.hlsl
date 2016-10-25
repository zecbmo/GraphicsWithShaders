// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColor[2];
	float4 position[2];// lightDirection;
  
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
	float3 lightDir2;
    float lightIntensity;
	float lightIntensity2;
    float4 color;
	float4 ambientColor = float4( 0.2, 0.2, 0.2, 1 );

	
	//set default colour value
	color = ambientColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);



	lightDir = input.position3D - position[0];
	lightDir = normalize(lightDir);
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	if (lightIntensity > 0)
	{
		color += saturate((diffuseColor[0])* lightIntensity);

	}

	lightDir2 = input.position3D - position[1];
	lightDir2 = normalize(lightDir2);
	// Calculate the amount of light on this pixel.
	lightIntensity2 = saturate(dot(input.normal, -lightDir2));
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	if (lightIntensity2 > 0)
	{
		color += saturate((diffuseColor[1])* lightIntensity2);

	}



    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	//color = color * textureColor;

    return color;
}

