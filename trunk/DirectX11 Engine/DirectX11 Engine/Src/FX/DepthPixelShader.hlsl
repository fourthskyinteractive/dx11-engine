//TYPEDEFS
struct PixelIn
{
	float4 pos			: SV_POSITION;
	float4 depthPos		: TEXCOORD0;
};

struct PixelOut
{
	float4 color		: SV_Target0;
};

PixelOut PS(PixelIn input)// : SV_TARGET
{
	float depthValue;
	PixelOut pOut;

	depthValue = input.depthPos.z / input.depthPos.w;

	if(depthValue < 0.9f)
	{
		pOut.color = float4(1.0f, 0.0, 0.0, 1.0f);
	}

	if(depthValue > 0.9f)
	{
		pOut.color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	if(depthValue > 0.925)
	{
		pOut.color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	pOut.color = float4(depthValue, depthValue, depthValue, 1.0f);

	return pOut;
}