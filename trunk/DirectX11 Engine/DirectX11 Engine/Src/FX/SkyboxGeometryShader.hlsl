struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 tex : TEXCOORD;
};

cbuffer Transforms
{
	matrix invViewProjectionMatrix;
};

[maxvertexcount(4)]
void GS(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
	GSOutput gsOut;

	float4 verts[4] = {-1, -1, 0.5, 1,
						1, -1, 0.5, 1,
						-1, 1, 0.5, 1,
						1, 1, 0.5, 1};

		gsOut.pos = verts[0];
		gsOut.tex = mul(verts[0], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[1];
		gsOut.tex = mul(verts[1], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[2];
		gsOut.tex = mul(verts[2], invViewProjectionMatrix);
		output.Append(gsOut);

		gsOut.pos = verts[3];
		gsOut.tex = mul(verts[3], invViewProjectionMatrix);
		output.Append(gsOut);
}