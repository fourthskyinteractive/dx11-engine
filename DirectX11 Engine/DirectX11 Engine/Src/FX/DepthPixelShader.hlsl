//TYPEDEFS
struct PixelIn
{
	float4 pos			: SV_POSITION;
	float4 depthPos		: TEXCOORD0;
};


float4 PS(PixelIn input) : SV_TARGET
{
	float depthValue;
	float4 color;

	depthValue = input.depthPos.z / input.depthPos.w;

	if(depthValue < 0.9f)
	{
		color = float4(1.0f, 0.0, 0.0, 1.0f);
	}

	if(depthValue > 0.9f)
	{
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	if(depthValue > 0.925)
	{
		color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}