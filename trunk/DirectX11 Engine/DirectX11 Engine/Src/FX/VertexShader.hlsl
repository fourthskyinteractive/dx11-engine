cbuffer cbPerObject
{
	float4x4 world;
	float4x4 viewProjection;
};

struct VertexIn
{
	float3 pos		: POSITION;
	float3 color	: COLOR;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR;
};

VertexOut VS( VertexIn vIn)
{
	VertexOut vOut;
	float4 pos = {vIn.pos, 1.0f};
	float4 worldPos = mul(pos, world);
	
	vOut.pos = mul(worldPos, viewProjection);

	vOut.color = float4(vIn.color, 1.0f);

	return vOut;
}