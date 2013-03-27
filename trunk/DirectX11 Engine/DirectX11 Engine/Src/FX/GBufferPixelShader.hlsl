cbuffer MaterialProperties
{
	float3 specularAlbedo;
	float specularPower;
};

//Textures/Samplers
Texture2D diffuseMap		: register(t0);
SamplerState ansioSampler	: register(s0);

struct PSIn
{
	float4 positionSS		: SV_POSITION;
	float2 texCoord			: TEXCOORD;
	float3 normalWS			: NORMALWS;
	float3 positionWS		: positionWS;
};

struct PSOut
{
	float4 normal			: SV_Target0;
	float4 diffuseAlbedo	: SV_Target1;
	float4 specularAlbedo	: SV_Target2;
	float4 position			: SV_Target3;
};

PSOut PS(in PSIn input)
{
	PSOut output;

	float3 diffuseAlbedo = diffuseMap.Sample(ansioSampler, input.texCoord).rgb;

	float3 normalWS = normalize(input.normalWS);

	output.normal = float4(normalWS, 1.0f);
	output.diffuseAlbedo = float4(diffuseAlbedo, 1.0f);
	output.specularAlbedo = float4(specularAlbedo, specularPower);
	output.position = float4(input.positionWS, 1.0f);

	return output;
}