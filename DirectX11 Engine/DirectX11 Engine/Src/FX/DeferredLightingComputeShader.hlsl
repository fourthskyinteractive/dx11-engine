#pragma pack_matrix(row_major)

#define horizontalTiles 32
#define verticalTiles 32

//PlaneOrder{PLANE_LEFT, PLANE_RIGHT, PLANE_TOP, PLANE_BOTTOM, PLANE_NEAR, PLANE_FAR}
//PLANES ARE WRAPPED FROM THE INSIDE


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
	float3 position;
	float radius;
};

struct DirectionalLightData
{
	float4 color;
	float3 direction;
};

struct PlaneData
{
	float3 TL;
	float3 TR;
	float3 BL;
	float3 BR;
	float3 normal;
};

struct FrustumData
{
	PlaneData planes[6];
};

struct SphereData
{
	float3 position;
	float radius;
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

groupshared int tileData[1024];

void LightPixel(float3 _textureLocation);
void LightCulling(ComputeIn _input);
void LightTile(ComputeIn _input);
void CreateFrustum(out FrustumData _frustum, ComputeIn _input);
void FindFurthestAndNearest(ComputeIn _input, out float3 _zNear, out float3 _zFar);
float FindDistance(float3 _startPos, float3 _endPos, float3 _direction);
bool SphereToFrustumCollision(FrustumData _frustum, PointLightData _pointLight);
bool SphereFrustumCollision(FrustumData _frustum, SphereData _sphere);

//(POINT LIGHT, SPOT LIGHT, DIRECTIONAL LIGHT, AMBIENT LIGHT)
float3 CalculateLighting(in float3 _normal,
						 in float3 _position,
						 in float3 _diffuseAlbedo,
						 in float3 _specularAlbedo,
						 in float _specularPower,
						 in float4 _lightType,
						 in float3 _lightPosition,
						 in float4 _lightColor,
						 in float _lightRange,
						 in float3 _lightDirection);

void GetGBufferAttributes(in float3 _textureLocation,
						  out float3 _normal,
						  out float3 _position,
						  out float3 _diffuseAlbedo, 
						  out float3 _specularAlbedo,
						  out float3 _depth,
						  out float _specularPower);

[numthreads(32, 24, 1)]
void CS(ComputeIn _input)
{
	//FIGURE OUT WHAT LIGHTS HIT THIS TILE!
	if(_input.groupThreadID.x == 0 && _input.groupThreadID.y == 0 && _input.groupThreadID.z == 0)
	{
		LightCulling(_input);
	}

	GroupMemoryBarrierWithGroupSync();

	LightTile(_input);
}

void LightCulling(ComputeIn _input)
{
	FrustumData frustum;
	CreateFrustum(frustum, _input);
	SphereData sphere;
	int currentIndex = 0;
	for(int i = 0; i < (int)numLights.y; ++i)
	{
		sphere.radius = pointLights[i].radius;
		sphere.position = pointLights[i].position;
		if(SphereFrustumCollision(frustum, sphere))
		{
			tileData[currentIndex] = i;
			currentIndex++;
		}
	}

	tileData[currentIndex] = -1;
}

void LightTile(ComputeIn _input)
{
	int column = _input.groupID.x;
	int row = _input.groupID.y;
	
	int pixelsPerColumn = (screenWidthHeightNearFar.x / horizontalTiles);
	int pixelsPerRow = (screenWidthHeightNearFar.y / verticalTiles);
	int texLocX = column * pixelsPerColumn + _input.groupThreadID.x;
	int texLocY = row * pixelsPerRow + _input.groupThreadID.y;
	int3 textureLocation = int3( texLocX, texLocY, 0 );

	LightPixel(textureLocation);
}

void LightPixel(float3 _textureLocation)
{
//  	if(tileData[0] == -1)
//  	{
//  		outputTexture[_textureLocation.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
//  		return;
// 	}

	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float3 depth;
	float specularPower;

	GetGBufferAttributes(_textureLocation, normal, position, diffuseAlbedo, specularAlbedo, depth, specularPower);

	float3 totalLight = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 1024; ++i)
	{
		if(tileData[i] == -1)
			break;
		
		totalLight += CalculateLighting(normal, position, diffuseAlbedo, specularAlbedo, specularPower, float4(1.0f, 0.0f, 0.0f, 0.0f),
										pointLights[tileData[i]].position, pointLights[tileData[i]].color, pointLights[tileData[i]].radius, float3(0.0f, 0.0f, 0.0f));
	}

 	totalLight += float4(.15f, .15f, .15f, 1.0f);//CalculateLighting(normal, position, diffuseAlbedo, specularAlbedo, specularPower, float4(0.0f, 0.0f, 0.0f, 1.0f),
 										//float3(0.0f, 0.0f, 0.0f), float4(0.05f, 0.05f, 0.05f, 1.0f), 0.0f, float3(0.0f, 0.0f, 0.0f));

	outputTexture[_textureLocation.xy] = float4(diffuseAlbedo * totalLight, 1.0f);
}

void FindFurthestAndNearest(ComputeIn input, out float3 zNear, out float3 zFar)
{
	int column = input.groupID.x;
	int row = input.groupID.y;

	//TODO: OPTIMIZE OUT AND PUT INTO A CONSTANT BUFFER
	int pixelsPerColumn = (screenWidthHeightNearFar.x / horizontalTiles);
	int pixelsPerRow = (screenWidthHeightNearFar.y / verticalTiles);

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
	int column = _input.groupID.x;
	int row = _input.groupID.y;

	float3 cameraPosition = float3(cameraView._41, cameraView._42, cameraView._43);
	float3 cameraLook = normalize(float3(cameraView._31, cameraView._32, cameraView._33));
	float3 cameraRight = normalize(float3(cameraView._11, cameraView._12, cameraView._13));
	float3 cameraUp = normalize(float3(cameraView._21, cameraView._22, cameraView._23));

	float farX = frustumExtentsXY.x;// * screenWidthHeightNearFar.w;
	float farY = frustumExtentsXY.y;// * screenWidthHeightNearFar.w;
	float nearX = frustumExtentsXY.z;// * screenWidthHeightNearFar.z;
	float nearY = frustumExtentsXY.w;// * screenWidthHeightNearFar.z;

	float pixelsPerRowNear = nearY / verticalTiles;
	float pixelsPerRowFar = farY / verticalTiles;
	float pixelsPerColumnNear = nearX / horizontalTiles;
	float pixelsPerColumnFar = farX / horizontalTiles;

	float3 farMiddle = cameraPosition + (cameraLook * screenWidthHeightNearFar.w);
	float3 nearMiddle = cameraPosition + (cameraLook * screenWidthHeightNearFar.z);

	float3 xVector;
	float3 yVector;

	float3 fullNTL = (nearMiddle - (cameraRight * (nearX / 2.0f))) + (cameraUp * (nearY / 2.0f));
	float3 fullNTR = (nearMiddle + (cameraRight * (nearX / 2.0f))) + (cameraUp * (nearY / 2.0f));
	float3 fullNBL = (nearMiddle - (cameraRight * (nearX / 2.0f))) - (cameraUp * (nearY / 2.0f));
	float3 fullNBR = (nearMiddle + (cameraRight * (nearX / 2.0f))) - (cameraUp * (nearY / 2.0f));

	float3 fullFTL = (farMiddle - (cameraRight * (farX / 2.0f))) + (cameraUp * (farY / 2.0f));
	float3 fullFTR = (farMiddle + (cameraRight * (farX / 2.0f))) + (cameraUp * (farY / 2.0f));
	float3 fullFBL = (farMiddle - (cameraRight * (farX / 2.0f))) - (cameraUp * (farY / 2.0f));
	float3 fullFBR = (farMiddle + (cameraRight * (farX / 2.0f))) - (cameraUp * (farY / 2.0f));

	float3 tiledNTL = fullNTL + ((column * pixelsPerColumnNear) * cameraRight) - ((row * pixelsPerRowNear) * cameraUp);
	float3 tiledNTR = tiledNTL + (pixelsPerColumnNear * cameraRight);
	float3 tiledNBL = tiledNTL - (pixelsPerRowNear * cameraUp);
	float3 tiledNBR = tiledNBL + (pixelsPerColumnNear * cameraRight);
  			   
	float3 tiledFTL = fullFTL + ((column * pixelsPerColumnFar) * cameraRight) - ((row * pixelsPerRowFar) * cameraUp);
	float3 tiledFTR = tiledFTL + (pixelsPerColumnFar * cameraRight);
	float3 tiledFBL = tiledFTL - (pixelsPerRowFar * cameraUp);
	float3 tiledFBR = tiledFBL + (pixelsPerColumnFar * cameraRight);

	//LEFT
	_frustum.planes[0].TL = tiledNTL;//NTL
	_frustum.planes[0].TR = tiledFTL;//FTL
	_frustum.planes[0].BL = tiledNBL;//NBL
	_frustum.planes[0].BR = tiledFBL;//FBL
	xVector = _frustum.planes[0].TR - _frustum.planes[0].TL;
	yVector = _frustum.planes[0].BL - _frustum.planes[0].TL;
	_frustum.planes[0].normal = normalize(cross(xVector, yVector));

	//RIGHT
	_frustum.planes[1].TL = tiledFTR;//FTR
	_frustum.planes[1].TR = tiledNTR;//NTR
	_frustum.planes[1].BL = tiledFBR;//FBR
	_frustum.planes[1].BR = tiledNBR;//NBR
	xVector = _frustum.planes[1].TR - _frustum.planes[1].TL;
	yVector = _frustum.planes[1].BL - _frustum.planes[1].TL;
	_frustum.planes[1].normal = normalize(cross(xVector, yVector));

	//TOP
	_frustum.planes[2].TL = tiledNTL;//NTL
	_frustum.planes[2].TR = tiledNTR;//NTR
	_frustum.planes[2].BL = tiledFTL;//FTL
	_frustum.planes[2].BR = tiledFTR;//FTR
	xVector = _frustum.planes[2].TR - _frustum.planes[2].TL;
	yVector = _frustum.planes[2].BL - _frustum.planes[2].TL;
	_frustum.planes[2].normal = normalize(cross(xVector, yVector));

	//BOTTOM
	_frustum.planes[3].TL = tiledFBL;//FBL
	_frustum.planes[3].TR = tiledFBR;//FBR
	_frustum.planes[3].BL = tiledNBL;//NBL
	_frustum.planes[3].BR = tiledNBR;//NBR
	xVector = _frustum.planes[3].TR - _frustum.planes[3].TL;
	yVector = _frustum.planes[3].BL - _frustum.planes[3].TL;
	_frustum.planes[3].normal = normalize(cross(xVector, yVector));

	//NEAR
	_frustum.planes[4].TL = tiledNTR;//NTR
	_frustum.planes[4].TR = tiledNTL;//NTL
	_frustum.planes[4].BL = tiledNBR;//NBR
	_frustum.planes[4].BR = tiledNBL;//NBL
	xVector = _frustum.planes[4].TR - _frustum.planes[4].TL;
	yVector = _frustum.planes[4].BL - _frustum.planes[4].TL;
	_frustum.planes[4].normal = normalize(cross(xVector, yVector));

	//FAR
	_frustum.planes[5].TL = tiledFTL;//FTL
	_frustum.planes[5].TR = tiledFTR;//FTR
	_frustum.planes[5].BL = tiledFBL;//FBL
	_frustum.planes[5].BR = tiledFBR;//FBR
	xVector = _frustum.planes[5].TR - _frustum.planes[5].TL;
	yVector = _frustum.planes[5].BL - _frustum.planes[5].TL;
	_frustum.planes[5].normal = normalize(cross(xVector, yVector));
}

float FindDistance(float3 _startPos, float3 _endPos, float3 _direction)
{
	float3 diff = _endPos - _startPos;

	return dot(_direction, diff);
}

bool SphereFrustumCollision(FrustumData _frustum, SphereData _sphere)
{
	float distance;
	for(int i = 0; i < 6; ++i)
	{
		distance = dot(_sphere.position - _frustum.planes[i].TL, _frustum.planes[i].normal);
		if(distance <= -_sphere.radius)
		{
			return false;
		}
	}

	return true;
}

void GetGBufferAttributes(in float3 _textureLocation,
						  out float3 _normal,
						  out float3 _position,
						  out float3 _diffuseAlbedo, 
						  out float3 _specularAlbedo,
						  out float3 _depth,
						  out float _specularPower)
{
	//Determine our indices for sampling the texture based on current screen position
	_normal = normalTexture.Load(_textureLocation).xyz;
	_position = positionTexture.Load(_textureLocation).xyz;
	_diffuseAlbedo = diffuseAlbedoTexture.Load(_textureLocation).xyz;
	_depth = depthTexture.Load(_textureLocation).xyz;

	float4 spec = specularAlbedoTexture.Load(_textureLocation);
	_specularAlbedo = float3(1.0f, 1.0f, 1.0f);//(_lightColor * 0.5f).xyz;
	//_specularAlbedo = ((_lightColor.xyz * _diffuseAlbedo) *  0.5f);;
	_specularPower = 0.15f;//spec.w;
}

void GetSpecularColor()
{
	
}

//THIS FUNCTION NEEDS WORK
float3 CalculateLighting(in float3 _normal,
						 in float3 _position,
						 in float3 _diffuseAlbedo,
						 in float3 _specularAlbedo,
						 in float _specularPower,
						 in float4 _lightType,
						 in float3 _lightPosition,
						 in float4 _lightColor,
						 in float _lightRange,
						 in float3 _lightDirection)
{
	float3 L = 0;
	float attenuation = 1.0f;

	//Ambient Light
	if(_lightType.w == 1.0f)
	{
		return _lightColor.xyz;
	}

	//If point light or spot light
 	if(_lightType.x == 1.0f || _lightType.y == 1.0f)
	{
		if(_lightRange != 0.0f)
		{
 			L = _lightPosition.xyz - _position;
			float dist = length(L);
			attenuation = max(0, 1.0f - (dist / _lightRange));
			L /= dist;
		}
	}
	//If directional light
	else if(_lightType.z == 1.0f)
	{
		L = -_lightDirection.xyz;
	}
	else
	{

	}
	//If spot light
// 	if(_lightType.y == 1.0f)
// 	{
// 		float3 L2 = _lightDirection.xyz;
// 		float rho = dot(-L, L2);
// 		attenuation *= saturate((rho - spotlightAngles.y) /
// 								(spotlightAngles.x - spotlightAngles.y));
// 	}

	float nDotL = saturate(dot(_normal, L));
	float3 diffuse = nDotL * _lightColor.xyz * _diffuseAlbedo;

	//Calculate the speculat term
	float3 V = float3(cameraView._41, cameraView._42, cameraView._43) - _position;
	float3 H = normalize(L + V);
	float specNormalizationFactor = ((_specularPower + 8.0f) / ( 8.0f * 3.14159265f));

	float3 specular = pow(saturate(dot(_normal, H)), _specularPower)
							* specNormalizationFactor * _lightColor.xyz * _specularAlbedo.xyz * nDotL;

	//Final value is the sum of the albedo and diffuse with attenuation applied.
	return(diffuse + specular) * attenuation;
}