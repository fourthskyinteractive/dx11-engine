cbuffer cbPerObject
{
	float4x4 worldViewProjection;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float4 Color	: COLOR;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float4 Color	: COLOR;
};

VertexOut VS( VertexIn vIn)
{
	VertexOut vOut;

	vOut.PosH = mul(float4(vIn.PosL, 1.0f), worldViewProjection);
	vOut.Color = vIn.Color;
	return vOut;
}