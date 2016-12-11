// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer HullBuffer : register(b0)
{
	float tess_factor;
	float3 padding;
};

struct InputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0; 
	float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tess_factor;
	output.edges[1] = tess_factor;
	output.edges[2] = tess_factor;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside = tess_factor;

    return output;
}


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.colour = patch[pointId].colour;

    return output;
}