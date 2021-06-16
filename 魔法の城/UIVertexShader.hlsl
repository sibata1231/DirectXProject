/**
 * @file Vertex2D.hlsl
 * @brief UI
 * @author T.Shibata
 * @date 2020/11/24 çÏê¨
 */
#include "Common.hlsli"

// ÉpÉâÉÅÅ[É^
struct VS_OUTPUT {
	float4	Position	: SV_Position;
	float2	TexCoord	: TEXCOORD0;
	float4	Diffuse		: COLOR0;
};

VS_OUTPUT main(VS_INPUT_PCT input) {
	VS_OUTPUT output;
	float4 P = mul(float4(input.Position, 1.0f), World);
	P = mul(P, View);
	output.Position = mul(P, Projection);
	output.TexCoord = mul(float4(input.Texture, 0.0f, 1.0f), TextureMatrix).xy;
	output.Diffuse = input.Color;
	return output;
}