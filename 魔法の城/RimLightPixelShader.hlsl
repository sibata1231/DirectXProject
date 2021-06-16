/**
 * @file RimLightPixelShader.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/26 作成
 */

#include "Common.hlsli"
struct PS_IN {
    float4 pos         : SV_POSITION;
    float3 nor         : NORMAL0;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float3 light_rim   : TEXCOORD0;
};

Texture2D    Texture : register(t0[0]); // Textureをスロット0の0番目のテクスチャレジスタに設定
SamplerState Sampler : register(s0[0]); // Samplerをスロット0の0番目のサンプラレジスタに設定

float4 main(PS_IN input) : SV_Target {
    float4 eye          = dot(input.pos,CameraVector);          // 視線ベクトル計算
    float3 eye_vector   = normalize(eye.xyz);                   // 視線ベクトル
    float3 light_vector = normalize(LightVector.xyz);           // ライトベクトル
    float3 normal = normalize(input.nor);                       // 法線
    float rim_power = 2.0f;                                     // リムライトの強さ
    float rim = 1.0 - dot(normal, eye_vector);                  // 内積の値を反転しモデルの縁部分が1.0、視線と平行部分を0.0にする。
    rim = pow(abs(rim), rim_power);                             // リムライトの強さを補正
    float light_rim = max(dot(-light_vector, eye_vector), 0.0); // 太陽との逆光を計算（頂点シェーダでOK）
    float4 color = rim * light_rim;                             // モデルの縁計算と、太陽の逆光計算を乗算
    color.w = 1.0f;
    return color;
}
