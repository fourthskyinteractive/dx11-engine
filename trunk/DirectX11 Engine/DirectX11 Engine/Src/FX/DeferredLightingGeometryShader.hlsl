struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 skyboxTex : TEXCOORD1;
};

cbuffer Transforms : register(cb0)
{
	matrix invViewProjectionMatrix;
};

[maxvertexcount(4)]
void GS(
	point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput gsOut;

	float4 verts[4] = {-1, 1, 0.5, 1,
						1, 1, 0.5, 1,
						-1, -1, 0.5, 1,
						1, -1, 0.5, 1};

		gsOut.pos = verts[0];
		gsOut.tex = float2(0.0f, 0.0f);
		gsOut.skyboxTex = mul(verts[0], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[1];
		gsOut.tex = float2(1.0f, 0.0f);
		gsOut.skyboxTex = mul(verts[1], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[2];
		gsOut.tex = float2(0.0f, 1.0f);
		gsOut.skyboxTex = mul(verts[2], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[3];
		gsOut.tex = float2(1.0f, 1.0f);
		gsOut.skyboxTex = mul(verts[3], invViewProjectionMatrix);
		output.Append(gsOut);
}