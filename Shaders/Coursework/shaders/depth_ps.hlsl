struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	float r = 0;
	float g = 0;
	float b = 0;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = (input.depthPosition.z / input.depthPosition.w);

	//if(depthValue < 0.9 )
	//{
	//	r = 1;
	//}
	//if (depthValue > 0.9)
	//{
	//	g = 1;
	//}
	//if (depthValue > 0.925 )
	//{
	//	b = 1;
	//}

	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}