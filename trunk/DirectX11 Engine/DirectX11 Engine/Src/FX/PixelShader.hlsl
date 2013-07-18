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
	float3 desiredColor = float3(92,51,23);
	float red = desiredColor.x / 255.0f * pIn.colorValue;
	float green = desiredColor.y / 255.0f * pIn.colorValue;
	float blue = desiredColor.z / 255.0f * pIn.colorValue;

	pOut.color = float4(red, green, blue, 1.0f);
	return pOut;
}