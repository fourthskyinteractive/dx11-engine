//Globals
Texture2D shaderTexture;
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
	float4 color = shaderTexture.Sample(sampleType, input.tex);
	pOut.color = color;
	return pOut;
	//color[3] = .5f;
	//return color;
}