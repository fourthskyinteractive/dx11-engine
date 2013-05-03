//Globals
Texture2D diffuseTexture : register(t0);
Texture2D depthTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D lightTexture : register(t3);
SamplerState sampleType;

//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};

struct PixelOut
{
	float4 color : SV_TARGET0;
};


PixelOut PS(PixelIn input)
{	
	PixelOut pOut;
	//Sample the pixel color from the texture using the sampler
	//at this texture coordinate location
	float4 color = normalTexture.Sample(sampleType, input.tex);
	pOut.color = color;
	return pOut;
	//color[3] = .5f;
	//return color;
}