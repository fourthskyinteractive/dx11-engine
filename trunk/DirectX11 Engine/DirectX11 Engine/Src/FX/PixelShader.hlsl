struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float colorValue : TEXCOORD;
};

struct PixelShaderOutput
{
	float4 color	: SV_Target0;
};

PixelShaderOutput PS(PixelShaderInput pIn)
{
	PixelShaderOutput pOut;
	pOut.color = float4(pIn.colorValue, 0.0f, 0.0f, 1.0f);
	return pOut;
}