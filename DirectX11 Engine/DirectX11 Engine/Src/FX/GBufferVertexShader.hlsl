#pragma pack_matrix(row_major)

cbuffer worldMatrixCBuffer : register(cb0)
{
	matrix worldMatrix;
};

cbuffer viewMatrixCBuffer : register(cb1)
{
	matrix viewMatrix;
};

cbuffer projectionMatrixCBuffer : register(cb2)
{
	matrix projectionMatrix;
};

struct VertexIn
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 texCoord	: TEXCOORD;
};

struct VertexOut
{
	float4 posCS		: SV_Position;
	float3 posWS		: POSITIONWS;
	float2 texCoord		: TEXCOORD;
	float3 normalWS		: NORMALWS;
	float3 cameraPos	: TEXCOORD1;
	float3 cameraZ		: TEXCOORD2;
};

VertexOut VS(VertexIn vIn)
{
	VertexOut vOut;

	vIn.pos.w = 1.0f;
	float4 pos = vIn.pos;

	//Calculate the World Space Position
	vOut.posWS = mul(pos, worldMatrix).xyz;
	
	//Calculate the Clip Space Position
	vOut.posCS = mul(pos, worldMatrix);
	vOut.posCS = mul(vOut.posCS, viewMatrix);
	vOut.posCS = mul(vOut.posCS, projectionMatrix);

	vOut.cameraPos.x = viewMatrix._41;
	vOut.cameraPos.y = viewMatrix._42;
	vOut.cameraPos.z = viewMatrix._43;

	vOut.cameraZ.x = viewMatrix._31;
	vOut.cameraZ.y = viewMatrix._32;
	vOut.cameraZ.z = viewMatrix._33;

	//Copy over the texcoords
	vOut.texCoord = vIn.texCoord;

	//Calculate the normal vector against the world matrix only
	vOut.normalWS = normalize(mul(vIn.normal, (float3x3)worldMatrix));
	//vOut.normalWS = normalize(vOut.normalWS);

	return vOut;
}