
#define horizontalTiles 16
#define verticalTiles 12

struct ComputeIn
{
	uint3 groupID : SV_GroupID;
	uint3 dispatchThreadID : SV_DispatchThreadID;
	uint3 groupThreadID : SV_GroupThreadID;
	uint groupIndex : SV_GroupIndex;
};

struct TileData
{
	float affectingLights[32];
};

struct PointLightData
{
	float4 color;
	float3 pos;
	float radius;
};

struct DirectionalLightData
{
	float4 color;
	float3 direction;
};

cbuffer LightingInformation : register(b0)
{
	float4 numLights;
};

cbuffer ScreenInformation : register(b1)
{
	float4 screenWidthHeight;
};

Texture2D<float4> normalTexture			: register(t0);
Texture2D<float4> diffuseAlbedoTexture	: register(t1);
Texture2D<float4> specularAlbedoTexture : register(t2);
Texture2D<float4> positionTexture		: register(t3);
Texture2D<float4> depthTexture			: register(t4);

RWTexture2D<float4> outputTexture : register(u0);

StructuredBuffer<PointLightData> pointLights : register(t4);
//StructuredBuffer<DirectionalLightData> directionalLights : register(t5);

groupshared TileData tileData[horizontalTiles * verticalTiles];

void LightCulling(ComputeIn input);
void LightPixel(ComputeIn input);

[numthreads(horizontalTiles, verticalTiles, 1)]
//[numthreads(1, 1, 1)]
void CS( ComputeIn input )
{

 	for(int i = 0; i < (int)numLights.x; ++i)
 	{
 		tileData[input.groupIndex].affectingLights[i] = 0.0f;
 	}

	//FIGURE OUT WHAT LIGHTS HIT THIS TILE!
	LightCulling(input);
	LightPixel(input);
	tileData[input.groupIndex].affectingLights[0] = 1.0f; 

	GroupMemoryBarrierWithGroupSync();
}

void LightCulling(ComputeIn input)
{

}

void LightPixel(ComputeIn input)
{
	int3 texturelocation = int3( 0, 0, 0 );
	texturelocation.x = input.groupID.x * horizontalTiles + input.groupThreadID.x;
	texturelocation.y = input.groupID.y * verticalTiles + input.groupThreadID.y;

	outputTexture[texturelocation.xy] = diffuseAlbedoTexture.Load(texturelocation);
}