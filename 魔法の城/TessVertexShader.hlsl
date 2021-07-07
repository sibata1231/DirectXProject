#include "Common.hlsli"

VertexToHull main(VS_INPUT_PNTT input) {
	VertexToHull output;
	output.normal   = mul(input.normal,  (float3x3)World);
	output.tangent  = mul(input.tangent, (float3x3)World);
	output.worldPos = mul(float4(input.position, 1.0f), World).xyz;
	//output.worldPos = input.position;
	output.uv       = input.uv;

	return output;

}