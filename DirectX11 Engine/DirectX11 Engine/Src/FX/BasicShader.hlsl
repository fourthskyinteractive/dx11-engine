cbuffer cbPerObject
{
	float4x4 worldViewProj;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float4 Color	: COLOR;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float4 Color	: COLOR;
};

VertexOut VS(VertexIn _vIn)
{
	VertexOut vOut;

	//Transform to homogeneous clip space
	vOut.PosH = mul(float4(_vIn.PosL, 1.0f), worldViewProj);

	//Just pass vertex color into the pixel shader
	vOut.Color = _vIn.Color;

	return vOut;
}

float4 PS(VertexOut _pIn) : SV_TARGET
{
	return _pIn.Color;
}