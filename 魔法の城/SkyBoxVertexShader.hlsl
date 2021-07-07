#include "Common.hlsli"

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VS_INPUT_P3T main(VS_INPUT_P input) {
	// Set up output
	VS_INPUT_P3T output;

	// Make a view matrix with NO translation
	matrix viewNoMovement = View;
	viewNoMovement._41 = 0;
	viewNoMovement._42 = 0;
	viewNoMovement._43 = 0;

	// Calculate output position
	matrix viewProj = mul(viewNoMovement, Projection);
	output.position = mul(float4(input.Position, 1.0f), viewProj);

	// Ensure our polygons are at max depth
	output.position.z = output.position.w;

	// Use the cube's vertex position as a direction in space
	// from the origin (center of the cube)
	output.uvw = input.Position;

	return output;
}