struct PixelShaderInput
{
	float4 pos : SV_POSITION;
};

struct PixelShaderOutput
{
	float4 color	: SV_Target3;
};

PixelShaderOutput PS(PixelShaderInput pIn)
{
	PixelShaderOutput pOut;
	pOut.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return pOut;
}