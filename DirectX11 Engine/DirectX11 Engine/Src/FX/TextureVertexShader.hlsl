cbuffer cbPerObject
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

struct VertexIn
{
	float3 pos		: POSITION;
	float2 tex		: TEXCOORD0;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	
	float4 pos = float4(vIn.pos, 1.0f);

	vOut.pos = mul(pos, worldMatrix);
	vOut.pos = mul(vOut.pos, viewProjectionMatrix);

	vOut.tex = vIn.tex;

	return vOut;
}