cbuffer Transforms
{
	matrix worldMatrix;
	matrix viewProjectionMatrix;
};

//Input/Output structures
struct VSIn
{
	float4 position			: POSITION;
};

struct VSOut
{
	float4 position			: SV_POSITION;
};

VSOut VS(in VSIn input)
{
	VSOut output;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewProjectionMatrix);

	return output;
}