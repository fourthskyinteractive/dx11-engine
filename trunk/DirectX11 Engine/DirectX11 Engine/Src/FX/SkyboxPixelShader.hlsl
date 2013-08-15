//Globals
TextureCube skybox : register(t0);
SamplerState sampleType : register(s0);

//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float3 tex		: TEXCOORD0;
};

struct PixelOut
{
	float4 color : SV_Target0;
};

PixelOut PS(PixelIn input)
{
	PixelOut pOut;

	pOut.color = skybox.Sample(sampleType, input.tex);

	return pOut;
}