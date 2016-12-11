Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : TEXCOORD1;
};


float4 main(InputType input) : SV_TARGET
{
	return float4(input.colour.x, input.colour.y, input.colour.z, 1.0);
}