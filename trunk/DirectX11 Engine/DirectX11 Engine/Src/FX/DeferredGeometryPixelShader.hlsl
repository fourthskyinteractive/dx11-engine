//Globals
Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};


//TYPEDEFS
struct PixelIn
{
	float4 pos		: SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;
	float4 depthPos	: TEXCOORD1;
};

struct PixelOut
{
	float4 deferredColor	: SV_Target1;
	float4 depthColor		: SV_Target2;
};


PixelOut PS(PixelIn input)
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	
	//Sample the pixel color from the texture using the sampler
	//at this texture coordinate location
	//HOOD
	textureColor = shaderTexture.Sample(sampleType, input.tex);
	
	//Expant the range of the normal value from (0, +1) to (-1, +1)
	//bumpMap = (bumpMap * 2.0f) - 1.0f;

	//Calculate the normal from the data in the bumpMap
	//bumpNormal = input.normal + (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal);

	//Normalize the resulting bump normal
	input.normal = normalize(input.normal);

	//Set the default output color to the ambient light value for all pixels
	color = ambientColor;

	//Invert the light direction for calculations
	lightDir = -lightDirection;

	//Make sure the light direction is normalized
	lightDir = normalize(lightDir);

	//Calculate the amount of light on this pixel
	lightIntensity = saturate(dot(input.normal, lightDir));

	if(lightIntensity > 0.0f)
	{
		//Determine the final diffuse color based on the amount of light intensity
		color += (diffuseColor * lightIntensity);
	}

	//Determine the final amount of diffuse color based on the diffuse color combined with the light intensity
	color = saturate(color);

	//Multiply the texture pixel and the final diffuse color to get the final color result
	color = color * textureColor;
	PixelOut pOut;
	pOut.deferredColor = color;

	float depthValue;

	depthValue = input.depthPos.z / input.depthPos.w;

	if(depthValue < 0.9f)
	{
		pOut.depthColor = float4(1.0f, 0.0, 0.0, 1.0f);
	}

	if(depthValue > 0.9f)
	{
		pOut.depthColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	if(depthValue > 0.925)
	{
		pOut.depthColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	pOut.depthColor = float4(depthValue, depthValue, depthValue, 1.0f);

	return pOut;//float4(1.0f, 0.0f, 0.0f, 1.0f);
}