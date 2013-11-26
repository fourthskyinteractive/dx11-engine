#define horizontalTiles 16
#define verticalTiles 16

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
						  in float4 _lightColor,
						  out float3 _normal,
						  out float3 _position,
						  out float3 _diffuseAlbedo, 
						  out float3 _specularAlbedo,
						  out float3 _depth,
						  out float _specularPower);

[numthreads(horizontalTiles, verticalTiles, 1)]
void CS(ComputeIn _input)
{
	//FIGURE OUT WHAT LIGHTS HIT THIS TILE!
	LightCulling(_input);
	LightTile(_input);

	GroupMemoryBarrierWithGroupSync();
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
	int column = _input.groupThreadID.x;
	int row = _input.groupThreadID.y;
	
	int pixelsPerColumn = (screenWidthHeightNearFar.y / horizontalTiles);
	int pixelsPerRow = (screenWidthHeightNearFar.x / verticalTiles);
	
	int3 textureLocation = int3( 0, 0, 0 );
	for(int i = 0; i < pixelsPerColumn; ++i)
	{
		for(int j = 0; j < pixelsPerRow; ++j)
		{
			textureLocation.x = (column * pixelsPerRow) + j;
			textureLocation.y = (row * pixelsPerColumn) + i;

			LightPixel(textureLocation);
		}
	}
}

void LightPixel(float3 _textureLocation)
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float3 depth;
	float specularPower;
	float3 totalLight = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 1024; ++i)
	{
		if(tileData[i] == -1)
			break;

		GetGBufferAttributes(_textureLocation, pointLights[tileData[i]].color, normal, position, diffuseAlbedo, specularAlbedo, depth, specularPower);
		totalLight += CalculateLighting(normal, position, diffuseAlbedo, specularAlbedo, specularPower, float4(1.0f, 0.0f, 0.0f, 0.0f),
										pointLights[tileData[i]].position, pointLights[tileData[i]].color, pointLights[tileData[i]].radius, float3(0.0f, 0.0f, 0.0f));
	}

	totalLight += CalculateLighting(normal, position, diffuseAlbedo, specularAlbedo, specularPower, float4(0.0f, 0.0f, 0.0f, 1.0f),
										float3(0.0f, 0.0f, 0.0f), float4(0.15f, 0.15f, 0.15f, 1.0f), 0.0f, float3(0.0f, 0.0f, 0.0f));

	outputTexture[_textureLocation.xy] = float4(totalLight * diffuseAlbedo, 1.0f);
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

	float3 xVector;
	float3 yVector;
	//LEFT
	_frustum.planes[0].TL = (nearMiddle - (cameraRight * nearX)) + (cameraUp * nearY);//NTL
	_frustum.planes[0].TR = (farMiddle - (cameraRight * farX)) + (cameraUp * farY);//FTL
	_frustum.planes[0].BL = (nearMiddle - (cameraRight * nearX)) - (cameraUp * nearY);//NBL
	_frustum.planes[0].BR = (farMiddle - (cameraRight * farX)) - (cameraUp * farY);//FBL
	xVector = _frustum.planes[0].TR - _frustum.planes[0].TL;
	yVector = _frustum.planes[0].BL - _frustum.planes[0].TL;
	_frustum.planes[0].normal = cross(xVector, yVector);

	//RIGHT
	_frustum.planes[1].TL = (farMiddle + (cameraRight * farX)) + (cameraUp * farY);//FTR
	_frustum.planes[1].TR = (nearMiddle + (cameraRight * nearX)) + (cameraUp * nearY);//NTR
	_frustum.planes[1].BL = (farMiddle + (cameraRight * farX)) - (cameraUp * farY);//FBR
	_frustum.planes[1].BR = (nearMiddle + (cameraRight * nearX)) - (cameraUp * nearY);//NBR
	xVector = _frustum.planes[1].TR - _frustum.planes[1].TL;
	yVector = _frustum.planes[1].BL - _frustum.planes[1].TL;
	_frustum.planes[1].normal = cross(xVector, yVector);

	//TOP
	_frustum.planes[2].TL = (nearMiddle - (cameraRight * nearX)) + (cameraUp * nearY);//NTL
	_frustum.planes[2].TR = (nearMiddle + (cameraRight * nearX)) + (cameraUp * nearY);//NTR
	_frustum.planes[2].BL = (farMiddle - (cameraRight * farX)) - (cameraUp * farY);//FBL
	_frustum.planes[2].BR = (farMiddle + (cameraRight * farX)) + (cameraUp * farY);//FTR
	xVector = _frustum.planes[2].TR - _frustum.planes[2].TL;
	yVector = _frustum.planes[2].BL - _frustum.planes[2].TL;
	_frustum.planes[2].normal = cross(xVector, yVector);

	//BOTTOM
	_frustum.planes[3].TL = (farMiddle - (cameraRight * farX)) - (cameraUp * farY);//FBL
	_frustum.planes[3].TR = (farMiddle + (cameraRight * farX)) - (cameraUp * farY);//FBR
	_frustum.planes[3].BL = (nearMiddle - (cameraRight * nearX)) - (cameraUp * nearY);//NBL
	_frustum.planes[3].BR = (nearMiddle + (cameraRight * nearX)) - (cameraUp * nearY);//NBR
	xVector = _frustum.planes[3].TR - _frustum.planes[3].TL;
	yVector = _frustum.planes[3].BL - _frustum.planes[3].TL;
	_frustum.planes[3].normal = cross(xVector, yVector);

	//NEAR
	_frustum.planes[4].TL = (nearMiddle + (cameraRight * nearX)) + (cameraUp * nearY);//NTR
	_frustum.planes[4].TR = (nearMiddle - (cameraRight * nearX)) + (cameraUp * nearY);//NTL
	_frustum.planes[4].BL = (nearMiddle + (cameraRight * nearX)) - (cameraUp * nearY);//NBR
	_frustum.planes[4].BR = (nearMiddle - (cameraRight * nearX)) - (cameraUp * nearY);//NBL
	xVector = _frustum.planes[4].TR - _frustum.planes[4].TL;
	yVector = _frustum.planes[4].BL - _frustum.planes[4].TL;
	_frustum.planes[4].normal = cross(xVector, yVector);

	//FAR
	_frustum.planes[5].TL = (farMiddle - (cameraRight * farX)) + (cameraUp * farY);//FTL
	_frustum.planes[5].TR = (farMiddle + (cameraRight * farX)) + (cameraUp * farY);//FTR
	_frustum.planes[5].BL = (farMiddle - (cameraRight * farX)) - (cameraUp * farY);//FBL
	_frustum.planes[5].BR = (farMiddle + (cameraRight * farX)) - (cameraUp * farY);//FBR
	xVector = _frustum.planes[5].TR - _frustum.planes[5].TL;
	yVector = _frustum.planes[5].BL - _frustum.planes[5].TL;
	_frustum.planes[5].normal = cross(xVector, yVector);
}

float FindDistance(float3 _startPos, float3 _endPos, float3 _direction)
{
	float3 diff = _endPos - _startPos;

	return dot(_direction, diff);
}

bool SphereFrustumCollision(FrustumData _frustum, SphereData _sphere)
{
	float offset;
	float distance;
	for(int i = 0; i < 6; ++i)
	{
		offset = dot(_frustum.planes[i].normal, _frustum.planes[i].TL);
		distance = dot(_sphere.position, _frustum.planes[i].normal);
		distance -= offset;
		
		if(distance < -_sphere.radius)
		{
			return false;
		}
	}
	
	return true;
}

void GetGBufferAttributes(in float3 _textureLocation,
						  in float4 _lightColor,
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
	float4 spec = specularAlbedoTexture.Load(_textureLocation);
	_depth = depthTexture.Load(_textureLocation).xyz;

	_specularAlbedo = (_lightColor * 0.5f).xyz;
	_specularAlbedo = ((_lightColor.xyz * _diffuseAlbedo) *  0.5f);;
	_specularPower = spec.w;
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
 		L = _lightPosition.xyz - _position;
		float dist = length(L);
		attenuation = max(0, 1.0f - (dist / _lightRange.x));
		L /= dist;
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