#pragma pack_matrix(row_major)

cbuffer cbPerObject : register(cb0)
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

struct VertexIn
{
	float4 pos		: POSITION;
};

struct VertexOut
{
	float4 pos			: SV_POSITION;
	float4 depthPos		: TEXCOORD0;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	vIn.pos.w = 1.0f;
	float4 pos = vIn.pos;

	vOut.pos = mul(pos, worldMatrix);
	vOut.pos = mul(vOut.pos, viewProjectionMatrix);

	vOut.depthPos = vOut.pos;

	return vOut;
}