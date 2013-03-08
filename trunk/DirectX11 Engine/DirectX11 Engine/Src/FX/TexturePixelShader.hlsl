//GLOBALS
Texture2D shaderTexture;
SamplerState sampleType;


//TYPEDEFS
struct PixelIn
{
	float4 pos	: SV_POSITION;
	float2 tex	: TEXCOORD0;
};


float4 PS(PixelIn input) : SV_TARGET
{
	float4 textureColor;
	
	//Sample the pixel color from the texture using the sampler
	//at this texture coordinate location
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	return textureColor;
}