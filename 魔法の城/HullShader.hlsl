#include "Common.hlsli"

[domain("tri")]
[partitioning("fractional_odd")]
//[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
HullToDomain main(InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip,uint i : SV_OutputControlPointID,uint PatchID : SV_PrimitiveID) {
	HullToDomain Output;

	// Insert code to compute Output here
	Output.normal   = ip[i].normal;
	Output.tangent  = ip[i].tangent;
	Output.worldPos = ip[i].worldPos;
	Output.uv       = ip[i].uv;

	return Output;
}

/*
	// Insert code to compute Output here
	//Output.vPosition = ip[i].vPosition;
	Output.normal = ip[i].normal;
	Output.tangent = ip[i].tangent;
	Output.worldPos = ip[i].worldPos;
	Output.uv = ip[i].uv;
*/