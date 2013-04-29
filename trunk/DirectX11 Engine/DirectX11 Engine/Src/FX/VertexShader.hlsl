cbuffer cbPerObject : register(cb0)
{
	float4x4 world;
	float4x4 viewProjection;
};

struct VertexIn
{
	float3 pos		: POSITION;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
};

VertexOut VS( VertexIn vIn)
{
	VertexOut vOut;

	float4 pos = {vIn.pos, 1.0f};

	float4 worldPos = mul(pos, world);
	
	vOut.pos = mul(worldPos, viewProjection);

	return vOut;
}