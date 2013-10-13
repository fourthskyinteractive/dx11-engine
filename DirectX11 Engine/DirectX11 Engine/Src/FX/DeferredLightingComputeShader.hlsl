struct ComputeIn
{
	uint3 DTid	: SV_DispatchThreadID;
	int index	: SV_GroupIndex;
};

struct TileData
{
	float4 position;
	float3 normal;
	float2 texCoord;
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

RWStructuredBuffer<TileData> tileData : register(u0);
StructuredBuffer<PointLightData> pointLights : register(u1);
StructuredBuffer<DirectionalLightData> directionalLights : register(u2);

[numthreads(1, 1, 1)]
void CS( ComputeIn input )
{
	
}