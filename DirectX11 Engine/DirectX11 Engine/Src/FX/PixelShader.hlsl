
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 PS(PixelShaderInput pIn) : SV_TARGET
{
	return pIn.color;
}