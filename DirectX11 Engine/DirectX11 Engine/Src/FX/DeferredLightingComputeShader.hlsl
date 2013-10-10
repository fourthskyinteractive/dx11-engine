struct ComputeIn
{
	uint3 DTid	: SV_DispatchThreadID;
	int index	: SV_GroupIndex;
};

struct TileData
{
	int something;
};

struct LightData
{
	int something;
};

RWStructuredBuffer<TileData> tileData;
StructuredBuffer<LightData> lightData;

[numthreads(1, 1, 1)]
void CS( ComputeIn input )
{
	lightData[input.index];
}