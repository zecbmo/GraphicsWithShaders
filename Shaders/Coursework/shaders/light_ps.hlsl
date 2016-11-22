// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 diffuseColor[4];
	float4 ambientColor[4];
	float4 specularColor[4];	
};
cbuffer LightArgsBuffer : register(b1)
{
	float4 position[4]; //w == 0 directional light ----- w == 1 then positional light
	int4 Args; // (x = number of lights, y = texture on)
}

cbuffer Specbuffer : register(b2)
{
	float4 specularPower;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;

};


void CalculateLight(InputType input, inout float4 color, inout float4 finalSpec, float4 textureColor, int lightNumber, float specPower )
{
	
	float3 lightDir;
	float lightIntensity;
	float4 specular;
	//float4 finalSpec;
	float3 reflection;


	lightDir = normalize(input.position3D - position[lightNumber]);
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	if (lightIntensity > 0)
	{
		color += saturate((diffuseColor[lightNumber])* lightIntensity);
		color = saturate(color);

		//reflection
		reflection = reflect(lightDir, input.normal);

		//determin spec
		specular = pow(saturate(dot(reflection, input.viewDirection)), specPower);

		//sum spec
		finalSpec = specularColor[lightNumber] * specular;

	}
	
	
}


float4 main(InputType input) : SV_TARGET
{

    float4 color[4];
	float4 finalSpec[4];
	float4 finalColor;
	float4 textureColor;

	//set default colour value
	finalColor = ambientColor[0];

	color[0] = ambientColor[0]; //ambient controlled by first colour
	color[1] = float4(0,0,0,1); //ambient controlled by first colour
	color[2] = float4(0, 0, 0, 1); //ambient controlled by first colour
	color[3] = float4(0, 0, 0, 1); //ambient controlled by first colour

	finalSpec[0] = float4(0, 0, 0, 1);
	finalSpec[1] = float4(0, 0, 0, 1);
	finalSpec[2] = float4(0, 0, 0, 1);
	finalSpec[3] = float4(0, 0, 0, 1);

	


    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	//[loop]
	//for (int i = 0; i < Args.x; i++)
	//{
	//	CalculateLight(input,color[i], textureColor, i);
	//}
	CalculateLight(input, color[0], finalSpec[0], textureColor, 0, specularPower.x);
	CalculateLight(input, color[1], finalSpec[1], textureColor, 1, specularPower.y);
	CalculateLight(input, color[2], finalSpec[2], textureColor, 2, specularPower.z);
	CalculateLight(input, color[3], finalSpec[3], textureColor, 3, specularPower.w);

	
	[branch]switch (Args.x)
	{
	case 1:

		finalColor = saturate(color[0]) * textureColor;
		finalColor = saturate(finalColor + finalSpec[0]);
		break;
	case 2:
		finalColor = saturate(color[0] + color[1]) * textureColor;
		finalColor = saturate(finalColor +  finalSpec[0] + finalSpec[1]);
		break;
	case 3:
		finalColor = saturate(color[0] + color[1] + color[2]) * textureColor;
		finalColor = saturate(finalColor + finalSpec[0] + finalSpec[1] + finalSpec[2]);
		break;
	case 4:
		finalColor = saturate(color[0] + color[1] + color[2] + color[3]) * textureColor;
		finalColor = saturate(finalColor + finalSpec[0] + finalSpec[1] + finalSpec[2] + finalSpec[3]);
		break;
	}
	
	return finalColor;



		
}

