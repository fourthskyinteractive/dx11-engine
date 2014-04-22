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

cbuffer animationData : register(cb3)
{
	float4 isAnimated_boneCount_frameCount;
};

cbuffer flattenedAnimationFrames : register(cb4)
{
	matrix bonesFlattened[255];
};

cbuffer inverseBindPoses : register(cb5)
{
	matrix bonesInvBindPose[255];
}

struct VertexIn
{
	float4 pos				: POSITION;
	float3 normal			: NORMAL;
	float2 texCoord			: TEXCOORD;
	float4 jointIndex		: TEXCOORD1;
	float4 jointWeight		: TEXCOORD2;
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
	
	float4 temp = pos;

	if(isAnimated_boneCount_frameCount.x == 1.0f)
	{
		temp =  vIn.jointWeight.x * mul(mul(pos, bonesInvBindPose[vIn.jointIndex.x]), bonesFlattened[vIn.jointIndex.x]);
 		temp += vIn.jointWeight.y * mul(mul(pos, bonesInvBindPose[vIn.jointIndex.y]), bonesFlattened[vIn.jointIndex.y]);
 		temp += vIn.jointWeight.z * mul(mul(pos, bonesInvBindPose[vIn.jointIndex.z]), bonesFlattened[vIn.jointIndex.z]);
 		temp += vIn.jointWeight.w * mul(mul(pos, bonesInvBindPose[vIn.jointIndex.w]), bonesFlattened[vIn.jointIndex.w]);
	}

	//Calculate the World Space Position
	vOut.posWS = mul(temp, worldMatrix).xyz;
	
	//Calculate the Clip Space Position
	vOut.posCS = mul(temp, worldMatrix);
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
	vOut.normalWS = vIn.jointWeight.x * mul(vIn.normal, (float3x3)bonesFlattened[vIn.jointIndex.x]);
	vOut.normalWS += vIn.jointWeight.y * mul(vIn.normal, (float3x3)bonesFlattened[vIn.jointIndex.y]);
	vOut.normalWS += vIn.jointWeight.z * mul(vIn.normal, (float3x3)bonesFlattened[vIn.jointIndex.z]);
	vOut.normalWS += vIn.jointWeight.w * mul(vIn.normal, (float3x3)bonesFlattened[vIn.jointIndex.w]);
	vOut.normalWS = normalize(mul(vOut.normalWS, (float3x3)worldMatrix));
	//vOut.normalWS = normalize(vOut.normalWS);

	return vOut;
}