/**
 * @file RimLightVertexShader.hlsl
 * @brief
 * @author T.Shibata
 * @date 2020/11/26 作成
 */

#include "Common.hlsli"
struct VS_OUT {
    float4 pos         : SV_POSITION;
    float3 nor         : NORMAL0;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float3 light_rim   : NORMAL1;
};

VS_OUT main(VS_INPUT_PNCT input) {
    VS_OUT output;
    float4 vNormal = float4(input.Normal.x, input.Normal.y, input.Normal.z, 0.0f);

    float4 normal = normalize(mul(vNormal, World));
    float3 light_dir = normalize(LightVector.xyz);

    // 頂点
    output.pos = mul(float4(input.Position, 1), World);
    output.pos = mul(output.pos, View);
    output.pos = mul(output.pos, Projection);

    // 色
    output.color = LightDiffuse * max(dot(light_dir, normal.xyz), 0) + LightAmbient;

    // テクスチャ
    output.texture_pos = input.Texture;

    // 法線
    output.nor = normal.xyz;

    // リムライト
    output.light_rim = max(dot(-light_dir, float3(0.0f, 0.0f, 1.0f)), 0.0f);
    return output;
}