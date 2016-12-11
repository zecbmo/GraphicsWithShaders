// Tessellation pixel shader
// Output colour passed to stage.

Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColor;

	textureColor = texture0.Sample(Sampler0, input.tex);

    return textureColor;
}