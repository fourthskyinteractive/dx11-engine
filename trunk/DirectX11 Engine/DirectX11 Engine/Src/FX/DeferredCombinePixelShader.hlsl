//Globals
Texture2D shaderTexture;
SamplerState sampleType;

//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};


float4 PS(PixelIn input) : SV_TARGET
{	
	//Sample the pixel color from the texture using the sampler
	//at this texture coordinate location
	return shaderTexture.Sample(sampleType, input.tex);;
}