//Globals
Texture2D DiffuseMap	: register(t0);
SamplerState sampleType : register(s0);

//TYPEDEFS
struct PixelIn
{
	float4 posSS			: SV_Position;
	float3 posWS			: POSITIONWS;
	float2 texCoord			: TEXCOORD;
	float3 normalWS			: NORMALWS;
	float3 cameraPos		: TEXCOORD1;
	float3 cameraZ			: TEXCOORD2;
};

struct PixelOut
{
	float4 Normal			: SV_Target0;
	float4 DiffuseAlbedo	: SV_Target1;
	float4 SpecularAlbedo	: SV_Target2;
	float4 Position			: SV_Target3;
	float4 Depth			: SV_Target4;
};

PixelOut PS(PixelIn input)
{
	PixelOut pOut;

	//Sample the diffuse texture

	float3 diffuseAlbedo = DiffuseMap.Sample(sampleType, input.texCoord).rgb;
	pOut.DiffuseAlbedo = float4(diffuseAlbedo, 1.0f);
	//Normalize the normal after interpolation
	float3 normalWS = input.normalWS;

	//output our G-Buffer values
	pOut.Normal = float4(normalWS, 1.0f);
	//pOut.Normal = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//Specual for white color and a power that resembles skin
	pOut.SpecularAlbedo = float4(0.0f, 0.0f, 0.0f, 80.0f);
	pOut.Position = float4(input.posWS, 1.0f);

	float depth = input.posSS.z / input.posSS.w;//;dot((input.posWS - input.cameraPos), input.cameraZ);
	pOut.Depth = float4(depth, depth, depth, depth);

// 	float4 textureColor;
// 
// 	textureColor = shaderTexture.Sample(sampleType, input.tex);
// 
// 	//Normalize the resulting bump normal
// 	input.normal = normalize(input.normal);
// 
// 	PixelOut pOut;
// 	pOut.deferredColor = textureColor;
// 	pOut.normalColor = float4(input.normal, 1.0f);
// 
// 	float depthValue;
// 
// 	depthValue = input.depthPos.z / input.depthPos.w;
// 
// 	if(depthValue < 0.9f)
// 	{
// 		pOut.depthColor = float4(1.0f, 0.0, 0.0, 1.0f);
// 	}
// 
// 	if(depthValue > 0.9f)
// 	{
// 		pOut.depthColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
// 	}
// 
// 	if(depthValue > 0.925)
// 	{
// 		pOut.depthColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
// 	}
// 
// 	pOut.depthColor = float4(depthValue, depthValue, depthValue, 1.0f);

	return pOut;
}