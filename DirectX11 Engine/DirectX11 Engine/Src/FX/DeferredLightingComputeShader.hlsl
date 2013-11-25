#define horizontalTiles 16
#define verticalTiles 16

struct ComputeIn
{
	uint3 groupID : SV_GroupID;
	uint3 dispatchThreadID : SV_DispatchThreadID;
	uint3 groupThreadID : SV_GroupThreadID;
	uint groupIndex : SV_GroupIndex;
};

struct TileData
{
	float affectingLights[32];
};

struct PointLightData
{
	float4 color;
	float3 pos;
	float radius;
};

struct DirectionalLightData
{
	float4 color;
	float3 direction;
};

struct FrustumData
{
	float3 NTL;
	float3 NTR;
	float3 NBL;
	float3 NBR;

	float3 FTL;
	float3 FTR;
	float3 FBL;
	float3 FBR;
};

cbuffer LightingInformation : register(b0)
{
	//Directional,Point,Spot
	float4 numLights;
};

cbuffer ScreenInformation : register(b1)
{
	float4 screenWidthHeightNearFar;
};

cbuffer CameraInformation : register(b2)
{
	matrix cameraView;
}

cbuffer FrustumExtents : register(b3)
{
	float4 frustumExtentsXY;
}

Texture2D<float4> normalTexture			: register(t0);
Texture2D<float4> diffuseAlbedoTexture	: register(t1);
Texture2D<float4> specularAlbedoTexture : register(t2);
Texture2D<float4> positionTexture		: register(t3);
Texture2D<float4> depthTexture			: register(t4);

RWTexture2D<float4> outputTexture : register(u0);

StructuredBuffer<PointLightData> pointLights : register(t5);
//StructuredBuffer<DirectionalLightData> directionalLights : register(t5);

groupshared TileData tileData[1024];

void LightCulling(ComputeIn _input);
void LightPixel(ComputeIn _input);
void CreateFrustum(out FrustumData _frustum, ComputeIn _input);
void FindFurthestAndNearest(ComputeIn _input, out float3 _zNear, out float3 _zFar);
float FindDistance(float3 _startPos, float3 _endPos, float3 _direction);
bool SphereToFrustumCollision(FrustumData _frustum, PointLightData _pointLight);

[numthreads(horizontalTiles, verticalTiles, 1)]
void CS(ComputeIn _input)
{

 	for(int i = 0; i < (int)numLights.x; ++i)
 	{
 		tileData[_input.groupIndex].affectingLights[i] = 0.0f;
 	}

	//FIGURE OUT WHAT LIGHTS HIT THIS TILE!
	//LightCulling(input);
	LightPixel(_input);

	GroupMemoryBarrierWithGroupSync();
}

void LightCulling(ComputeIn _input)
{
	FrustumData frustum;
	CreateFrustum(frustum, _input);
}

void LightPixel(ComputeIn _input)
{
	int column = _input.groupThreadID.x;
	int row = _input.groupThreadID.y;
	
	int pixelsPerColumn = (screenWidthHeightNearFar.y / horizontalTiles);
	int pixelsPerRow = (screenWidthHeightNearFar.x / verticalTiles);
	
	int3 texturelocation = int3( 0, 0, 0 );
	for(int i = 0; i < pixelsPerColumn; ++i)
	{
		for(int j = 0; j < pixelsPerRow; ++j)
		{
			texturelocation.x = (column * pixelsPerRow) + j;
			texturelocation.y = (row * pixelsPerColumn) + i;

			if(_input.groupThreadID.x == 0 &&  _input.groupThreadID.y == 0)
				outputTexture[texturelocation.xy] = pointLights[0].color;
			else
				outputTexture[texturelocation.xy] = diffuseAlbedoTexture.Load(texturelocation);
		}
	}
}

void FindFurthestAndNearest(ComputeIn input, out float3 zNear, out float3 zFar)
{
	int column = input.groupThreadID.x;
	int row = input.groupThreadID.y;

	int pixelsPerColumn = (screenWidthHeightNearFar.y / horizontalTiles);
	int pixelsPerRow = (screenWidthHeightNearFar.x / verticalTiles);

	int3 farTextureLocation = int3(0, 0, 0);
	int3 nearTextureLocation = int3(0, 0, 0);
	float nearDistance = 99999999.0f;
	float farDistance = -1.0f;
	for(int i = 0; i < pixelsPerColumn; ++i)
	{
		for(int j = 0; j < pixelsPerRow; ++j)
		{
			int3 textureLocation = int3( 0, 0, 0 );
			textureLocation.x = (column * pixelsPerRow) + j;
			textureLocation.y = (row * pixelsPerColumn) + i;

			float depth = depthTexture.Load(textureLocation).x;
			if(depth > farDistance)
			{
				farDistance = depth;
				farTextureLocation = textureLocation;
			}

			if(depth < nearDistance)
			{
				nearDistance = depth;
				nearTextureLocation = textureLocation;
			}
		}
	}

	zFar = positionTexture.Load(farTextureLocation).xyz;
	zNear = positionTexture.Load(nearTextureLocation).xyz;
}

void CreateFrustum(out FrustumData _frustum, ComputeIn _input)
{
	float3 zNear;
	float3 zFar;
	FindFurthestAndNearest(_input, zNear, zFar);

	float3 cameraPosition = float3(cameraView._41, cameraView._42, cameraView._43);
	float3 cameraLook = normalize(float3(cameraView._31, cameraView._32, cameraView._33));
	float3 cameraRight = normalize(float3(cameraView._11, cameraView._12, cameraView._13));
	float3 cameraUp = normalize(float3(cameraView._21, cameraView._22, cameraView._23));


	float distanceToNear = FindDistance(cameraPosition, zNear, cameraLook);
	float distanceToFar = FindDistance(cameraPosition, zFar, cameraLook);

	if(distanceToNear < screenWidthHeightNearFar.z)
		distanceToNear = screenWidthHeightNearFar.z;
	if(distanceToFar > screenWidthHeightNearFar.w)
		distanceToFar = screenWidthHeightNearFar.w;

	float farX = frustumExtentsXY.x * distanceToFar;
	float farY = frustumExtentsXY.x * distanceToFar;
	float nearX = frustumExtentsXY.x * distanceToFar;
	float nearY = frustumExtentsXY.x * distanceToFar;

	float3 farMiddle = cameraPosition + (cameraLook * distanceToFar);
	float3 nearMiddle = cameraPosition + (cameraLook * distanceToNear);

	_frustum.FTL = (farMiddle - (cameraRight * farX)) + (cameraUp * farY);
	_frustum.FTR = (farMiddle + (cameraRight * farX)) + (cameraUp * farY);
	_frustum.FBL = (farMiddle - (cameraRight * farX)) - (cameraUp * farY);
	_frustum.FBR = (farMiddle + (cameraRight * farX)) - (cameraUp * farY);

	_frustum.NTL = (nearMiddle - (cameraRight * nearX)) + (cameraUp * nearY);
	_frustum.NTR = (nearMiddle + (cameraRight * nearX)) + (cameraUp * nearY);
	_frustum.NBL = (nearMiddle - (cameraRight * nearX)) - (cameraUp * nearY);
	_frustum.NBR = (nearMiddle + (cameraRight * nearX)) - (cameraUp * nearY);
}

float FindDistance(float3 _startPos, float3 _endPos, float3 _direction)
{
	float3 diff = _endPos - _startPos;

	return dot(_direction, diff);
}