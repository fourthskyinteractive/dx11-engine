Texture2D normalTexture				: register(t0);
Texture2D diffuseAlbedoTexture		: register(t1);
Texture2D specularAlbedoTexture		: register(t2);
Texture2D positionTexture			: register(t3);

//Constants
cbuffer LightParams
{
	float3 lightPosition;
	float3 lightColor;
	float3 lightDirection;
	float2 spotLightAngles;
	float4 lightRange;
};

cbuffer cameraParams
{
	float3 cameraPos;
};

//Helper function for extracting the G-Buffer attributes
void GetGBufferAttributes(in float2 screenPos, out float3 normal,
						  out float3 position,
						  out float3 diffuseAlbedo, out float3 specularAlbedo,
						  out float specularPower)
{
	int3 sampleIndices = int3(screenPos.xy, 0);

	normal = normalTexture.Load(sampleIndices).xyz;
	position = positionTexture.Load(sampleIndices).xyz;
	diffuseAlbedo = diffuseAlbedoTexture.Load(sampleIndices).xyz;
	float4 spec = specularAlbedoTexture.Load(sampleIndices);

	specularAlbedo = spec.xyz;
	specularPower = spec.w;
}

//Calculates the lighting term for a single G-Buffer texel
float3 CalcLighting(in float3 normal,
					in float3 position,
					in float3 diffuseAlbedo,
					in float3 specularAlbedo,
					in float specularPower)
{
	float3 L = 0;
	float attenuation = 1.0f;
#if POINTLIIGHT || SPOTLIGHT
	
		L = lightPos - position;
	
#elif DIRECTIONALLIGHT
	
		L = -lightDirection;
#endif

#if SPOTLIGHT
	
		float3 L2 = lightDirection;
		float rho = dot(-L, L2);
		attenuation *= saturate((rho - spotlightAngles.y)
									/ (spotlightAngles.x - spotlightAngles.y));
#endif

	float nDotL = saturate(dot(L, normal));
	float3 diffuse = nDotL * lightColor * diffuseAlbedo;

	float3 V = cameraPos - position;
	float3 H = normalize(L + V);
	float3 specular = pow(saturate(dot(normal, H)), 64) 
							* lightColor * specularAlbedo.xyz * nDotL;
	return (diffuse + specular) * attenuation;
}

float4 PS(in float4 screenPos : SV_POSITION) : SV_Target0
{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	GetGBufferAttributes(screenPos.xy, normal, position, 
							diffuseAlbedo, specularAlbedo, specularPower);

	float3 lighting = CalcLighting(normal, position, diffuseAlbedo,
									specularAlbedo, specularPower);

	return float4(lighting, 1.0f);
}