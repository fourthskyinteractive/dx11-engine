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
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};

struct VertexOut
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	int textureIndex: TEXINDEX;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	vOut.textureIndex = (int)vIn.pos.w;
	vIn.pos.w = 1.0f;
	float4 pos = vIn.pos;

	vOut.pos = mul(pos, worldMatrix);
	vOut.pos = mul(vOut.pos, viewProjectionMatrix);

	vOut.tex = vIn.tex;

	//Calculate the normal vector against the world matrix only
	vOut.normal = mul(vIn.normal, (float3x3)worldMatrix);

	vOut.normal = normalize(vOut.normal);

	 // Calculate the tangent vector against the world matrix only and then normalize the final value.
    vOut.tangent = mul(vIn.tangent, (float3x3)worldMatrix);
    vOut.tangent = normalize(vOut.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    vOut.binormal = mul(vIn.binormal, (float3x3)worldMatrix);
    vOut.binormal = normalize(vOut.binormal);

	return vOut;
}