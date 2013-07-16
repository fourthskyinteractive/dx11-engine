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
	float4 pos			: SV_POSITION;
	float colorValue	: TEXCOORD;
};

VertexOut VS( VertexIn vIn)
{
	VertexOut vOut;

	float4 pos = {vIn.pos, 1.0f};
	float4x4 newWorld = world;
	float smallest = newWorld._41;
	float largest = newWorld._42;
	newWorld._41 = 0.0f;
	newWorld._42 = 0.0f;
	float denominator = largest - smallest;
	float numerator = vIn.pos.y - smallest;  
	vOut.colorValue = numerator / denominator;

	float4 worldPos = mul(pos, newWorld);
	
	vOut.pos = mul(worldPos, viewProjection);

	return vOut;
}