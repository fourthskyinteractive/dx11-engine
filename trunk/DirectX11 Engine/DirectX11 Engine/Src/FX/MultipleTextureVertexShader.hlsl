cbuffer cbPerObject : register(cb0)
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

struct VertexIn
{
	float4 pos		: POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float textureIndex: TEXINDEX;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.textureIndex = vIn.pos.w;
	vIn.pos.w = 1.0f;
	float4 pos = vIn.pos;

	vOut.pos = mul(pos, worldMatrix);
	vOut.pos = mul(vOut.pos, viewProjectionMatrix);

	vOut.tex = vIn.tex;

	//Calculate the normal vector against the world matrix only
	vOut.normal = mul(vIn.normal, (float3x3)worldMatrix);

	vOut.normal = normalize(vOut.normal);

	return vOut;
}