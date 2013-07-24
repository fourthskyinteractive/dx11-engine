#pragma pack_matrix(row_major)

cbuffer Transforms
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

//Input/Output structures
struct VSIn
{
	float4 position			: POSITION;
	float2 texCoord			: TEXCOORDS0;
	float3 normal			: NORMAL;
};

struct VSOut
{
	float4 positionCS		: SV_POSITION;
	float2 texCoord			: TEXCOORD;
	float3 normalWS			: NORMALWS;
	float3 positionWS		: POSITIONWS;
};

VSOut VS(in VSIn input)
{
	VSOut output;

	output.positionWS = mul(input.position, worldMatrix).xyz;
	output.normalWS = normalize(mul(input.normal, (float3x3)worldMatrix));

	output.positionCS = mul(input.position, worldMatrix);
	output.positionCS = mul(output.positionCS, viewProjectionMatrix);

	output.texCoord = input.texCoord;

	return output;
}