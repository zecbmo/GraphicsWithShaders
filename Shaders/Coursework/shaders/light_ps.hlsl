// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 diffuseColor[4];
	float4 ambientColor[4];
	float4 specularColor[4];
	float specularPower[4];
	float4 position[4]; //w == 0 directional light ----- w == 1 then positional light
	float textureOn;
	float numberOfLights; //only needs one but for padding issues will be defined as an array
	float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;
};


void CalculateLight(InputType input, inout float4 color,  float4 textureColor, int lightNumber )
{
	float3 lightDir;
	float lightIntensity;
	float4 specular;
	float4 finalSpec;
	float3 reflection;

	lightDir = input.position3D - position[lightNumber];
	lightDir = normalize(lightDir);
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));


	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	if (lightIntensity > 0)
	{
		color += saturate((diffuseColor[0])* lightIntensity);

		//reflection
		reflection = reflect(lightDir, input.normal);

		//determin spec
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower[lightNumber]);

		//sum spec
		finalSpec = specularColor[lightNumber] * specular;

	}

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	float4 col2 = color * textureColor;
	color = saturate(col2 + finalSpec);
}


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

	//set default colour value
	color = ambientColor[0];

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	for (int i = 0; i < numberOfLights; i++)
	{
		CalculateLight(input,color, textureColor, i);
	}
	
	if (numberOfLights == 0)
	{
		return float4(1, 1, 1, 1);
	}
	if (numberOfLights == 2)
	{
		return float4(0, 0, 1, 1);
	}
	if (numberOfLights == 4)
	{
		return float4(1, 0, 0, 1);
	}



    return color;
}

