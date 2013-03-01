cbuffer cbPerObject : register( b0 )
{
	matrix model;
	matrix view;
	matrix projection;
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
	float4 pos = float4(vIn.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	vOut.pos = pos;

	vOut.color = float4(vIn.color, 1.0f);
	return vOut;
}