//Globals
Texture2D normalTexture : register(t0);
Texture2D diffuseAlbedoTexture : register(t1);
Texture2D specularAlbedoTexture : register(t2);
Texture2D positionTexture : register(t3);

//Constants
cbuffer LightandCameraParams
{
	//(POINT LIGHT, SPOT LIGHT, DIRECTIONAL LIGHT, AMBIENT LIGHT)
	float3 lightColor;
	float4 lightType;
	float3 lightPos;
	
	float3 lightDirection;
	float2 spotlightAngles;
	float4 lightRange;
	float3 cameraPos;
	float2 padding;
};

//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};

struct PixelOut
{
	float4 color : SV_TARGET0;
};


void GetGBufferAttributes(in float2 screenPos, out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo, out float3 specularAlbedo,
						  out float specularPower)
{
	//Determine our indices for sampling the texture based on current screen position
	int3 sampleIndices = int3(screenPos.xy, 0);

	normal = normalTexture.Load(sampleIndices).xyz;
	position = positionTexture.Load(sampleIndices).xyz;
	diffuseAlbedo = diffuseAlbedoTexture.Load(sampleIndices).xyz;
	float4 spec = specularAlbedoTexture.Load(sampleIndices);

	specularAlbedo = spec.xyz;
	specularPower = spec.w;
}

float3 CalculateLighting(in float3 normal,
						 in float3 position,
						 in float3 diffuseAlbedo,
						 in float3 specularAlbedo,
						 in float specularPower)
{
	float3 L = 0;
	float attenuation = 1.0f;

	//If point light or spot light
 	if(lightType.x == 1.0f || lightType.y == 1.0f)
	{
 		L = lightPos - position;
		float dist = length(L);
		attenuation = max(0, 1.0f - (dist / lightRange.x));
		L /= dist;
	}
	//If directional light
	else
	{
		L = -lightDirection;
	}
	//If spot light
	if(lightType.y == 1.0f)
	{
		float3 L2 = lightDirection;
		float rho = dot(-L, L2);
		attenuation *= saturate((rho - spotlightAngles.y) /
								(spotlightAngles.x - spotlightAngles.y));
	}

	float nDotL = saturate(dot(normal, L));
	float3 diffuse = nDotL * lightColor * diffuseAlbedo;

	//Calculate the speculat term
	float3 V = cameraPos - position;
	float3 H = normalize(L + V);
	float3 specular = pow(saturate(dot(normal, H)), specularPower)
							* lightColor * specularAlbedo.xyz * nDotL;

	//Final value is the sum of the albedo and diffuse with attenuation applied.
	return(diffuse * specular) * attenuation;
}


PixelOut PS(PixelIn input)
{	
	PixelOut pOut;
	
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	//Sample the G-Buffer properties from the textures
	GetGBufferAttributes(input.pos.xy, normal, position, diffuseAlbedo,
						specularAlbedo, specularPower);

	float3 lighting = CalculateLighting(normal, position, diffuseAlbedo, specularAlbedo, specularPower);

	pOut.color = float4(lightColor, 1.0f);//float4(lighting, 1.0f);

	return pOut;
}