cbuffer cbPerObject : register(cb0)
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

struct VertexIn
{
	float3 pos		: POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	
	float4 pos = float4(vIn.pos, 1.0f);

	vOut.pos = mul(pos, worldMatrix);
	vOut.pos = mul(vOut.pos, viewProjectionMatrix);

	vOut.tex = vIn.tex;

	//Calculate the normal vector against the world matrix only
	vOut.normal = mul(vIn.normal, (float3x3)worldMatrix);

	vOut.normal = normalize(vOut.normal);

	return vOut;
}