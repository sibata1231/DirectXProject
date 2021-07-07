#include "Common.hlsli"

// External texture-related data
TextureCube Sky				: register(t0);
SamplerState basicSampler	: register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader

// Entry point for this pixel shader
float4 main(VS_INPUT_P3T input) : SV_TARGET {
	return Sky.Sample(basicSampler, input.uvw);
}