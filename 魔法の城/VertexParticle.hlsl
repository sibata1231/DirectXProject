/**
 * @file VertexParticle.hlsl
 * @brief パーティクル用
 * @author T.Shibata
 * @date 2020/11/25 作成
 */
#include "Common.hlsli"

// パラメータ
struct VS_OUTPUT {
	float4	Position	: SV_POSITION;
	float3	Normal		: TEXCOORD0;
	float2	TexCoord	: TEXCOORD1;
	float4	Diffuse		: COLOR0;
};

VS_OUTPUT main(VS_INPUT_PNCTI input) {
	VS_OUTPUT output;
    output.Position = float4(input.Position, 1.0f);
    float4x4 world  = mul(World, WorldInstance[input.ID]);
    output.Position = mul(output.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
	output.Normal   = mul(float4(input.Normal, 0.0f), world).xyz;
	output.TexCoord = mul(float4(input.Texture, 0.0f, 1.0f), TextureMatrix).xy;
	output.Diffuse  = input.Color;
	return output;
}
