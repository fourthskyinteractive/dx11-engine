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
	return float4(1.0f, 0.0f, 0.0f, 1.0f);//shaderTexture.Sample(sampleType, input.tex);;
}