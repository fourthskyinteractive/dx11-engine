#pragma pack_matrix(row_major)

cbuffer cbPerObject : register(cb0)
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

struct VertexIn
{
	float4 pos		: POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct VertexOut
{
	float4 posCS		: SV_Position;
	float3 posWS		: POSITIONWS;
	float2 texCoord		: TEXCOORD;
	float3 normalWS		: NORMALWS;
	bool hasTexture		: COLOR;
};


VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;
	if(vIn.pos.w == 1.0f)
	{
		vOut.hasTexture = false;
	}
	else
	{
		vOut.hasTexture = true;
	}
	vIn.pos.w = 1.0f;
	float4 pos = vIn.pos;

	//Calculate the World Space Position
	vOut.posWS = mul(pos, worldMatrix).xyz;
	
	//Calculate the Clip Space Position
	vOut.posCS = mul(pos, worldMatrix);
	vOut.posCS = mul(vOut.posCS, viewProjectionMatrix);

	//Copy over the texcoords
	vOut.texCoord = vIn.texCoord;

	//Calculate the normal vector against the world matrix only
	vOut.normalWS = normalize(mul(vIn.normal, (float3x3)worldMatrix));
	//vOut.normalWS = normalize(vOut.normalWS);

	return vOut;
}