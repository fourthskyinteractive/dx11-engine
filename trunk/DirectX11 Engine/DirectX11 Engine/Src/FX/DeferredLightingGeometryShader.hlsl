struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(4)]
void GS(
	point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput gsOut;

	float4 verts[4] = {-1, -1, 0.5, 1,
						-1, 1, 0.5, 1,
						1, -1, 0.5, 1,
						1, 1, 0.5, 1};


		gsOut.pos = verts[0];
		output.Append(gsOut);

		gsOut.pos = verts[1];
		output.Append(gsOut);

		gsOut.pos = verts[2];
		output.Append(gsOut);

		gsOut.pos = verts[3];
		output.Append(gsOut);
}