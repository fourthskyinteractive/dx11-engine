//Globals
Texture2D shaderTexture;
SamplerState sampleType;

//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float4 depthPos	: TEXCOORD1;
};

struct PixelOut
{
	float4 deferredColor	: SV_Target1;
	float4 depthColor		: SV_Target2;
	float4 normalColor		: SV_Target3;
};


PixelOut PS(PixelIn input)
{
	float4 textureColor;

	textureColor = shaderTexture.Sample(sampleType, input.tex);

	//Normalize the resulting bump normal
	input.normal = normalize(input.normal);

	PixelOut pOut;
	pOut.deferredColor = textureColor;
	pOut.normalColor = float4(input.normal, 1.0f);

	float depthValue;

	depthValue = input.depthPos.z / input.depthPos.w;

	if(depthValue < 0.9f)
	{
		pOut.depthColor = float4(1.0f, 0.0, 0.0, 1.0f);
	}

	if(depthValue > 0.9f)
	{
		pOut.depthColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	if(depthValue > 0.925)
	{
		pOut.depthColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	pOut.depthColor = float4(depthValue, depthValue, depthValue, 1.0f);

	return pOut;
}