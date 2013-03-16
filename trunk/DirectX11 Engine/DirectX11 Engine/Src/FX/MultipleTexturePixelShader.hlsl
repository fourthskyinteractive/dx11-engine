//Globals
Texture2D shaderTexture[17];
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
	int textureIndex: TEXINDEX;
	float3 tangent	: TANGENT;
	float3 binormal	: binormal;
};


float4 PS(PixelIn input) : SV_TARGET
{
	float4 textureColor;
	float4 bumpMap;
	float3 bumpNormal;
	float3 lightDir;
	float lightIntensity;
	float4 color;
	
	//Sample the pixel color from the texture using the sampler
	//at this texture coordinate location
	//HOOD
	if(input.textureIndex == 10)
	{
		textureColor = shaderTexture[7].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[8].Sample(sampleType, input.tex);
	}
	//GLOVES
	else if(input.textureIndex == 7 ||
	   input.textureIndex == 8 ||
	   input.textureIndex == 9)
	{
		textureColor = shaderTexture[5].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[6].Sample(sampleType, input.tex);
	}
	//EYES
	else if(input.textureIndex == 13)
	{
		textureColor = shaderTexture[2].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[2].Sample(sampleType, input.tex);
	}
	//SHIRT
	else if(input.textureIndex == 0 ||
	   input.textureIndex == 1 ||
	   input.textureIndex == 6)
	{
		textureColor = shaderTexture[11].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[12].Sample(sampleType, input.tex);
	}
	//SWORD
	else if(input.textureIndex == 3)
	{
		textureColor = shaderTexture[13].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[14].Sample(sampleType, input.tex);
	}
	//FACE
	else if(input.textureIndex == 12)
	{
		textureColor = shaderTexture[3].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[4].Sample(sampleType, input.tex);
	}
	//BOOTS
	else if(input.textureIndex == 5)
	{
		textureColor = shaderTexture[0].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[1].Sample(sampleType, input.tex);
	}
	//LEGS
	else if(input.textureIndex == 2)
	{
		textureColor = shaderTexture[9].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[10].Sample(sampleType, input.tex);
	}
	//SWORD HANDLE
	else if(input.textureIndex == 4)
	{
		textureColor = shaderTexture[15].Sample(sampleType, input.tex);
		bumpMap = shaderTexture[16].Sample(sampleType, input.tex);
	}
	else
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	//Expant the range of the normal value from (0, +1) to (-1, +1)
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	//Calculate the normal from the data in the bumpMap
	bumpNormal = input.normal + (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal);

	//Normalize the resulting bump normal
	bumpNormal = normalize(bumpNormal);

	//Set the default output color to the ambient light value for all pixels
	color = ambientColor;

	//Invert the light direction for calculations
	lightDir = -lightDirection;

	//Make sure the light direction is normalized
	lightDir = normalize(lightDir);

	//Calculate the amount of light on this pixel
	//lightIntensity = saturate(dot(bumpNormal, lightDir));

	//if(lightIntensity > 0.0f)
	//{
		//Determine the final diffuse color based on the amount of light intensity
		//color += (diffuseColor * lightIntensity);
	//}

	//Determine the final amount of diffuse color based on the diffuse color combined with the light intensity
	//color = saturate(color);

	//Multiply the texture pixel and the final diffuse color to get the final color result
	color = color * textureColor;

	return color;//float4(1.0f, 0.0f, 0.0f, 1.0f);
}