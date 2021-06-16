/**
 * @file BokehPixelShader.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/26 作成
 */
struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

Texture2D    Texture : register(t0[0]); // Textureをスロット0の0番目のテクスチャレジスタに設定
SamplerState Sampler : register(s0[0]); // Samplerをスロット0の0番目のサンプラレジスタに設定

float4 main(PS_IN input) : SV_Target {
    float4 tex_color                      = Texture.Sample(Sampler, input.texture_pos);
    const float coc                       = input.color.a;
    const float APERTURE_BLADE            = 1.0f; 
    const float APERTURE_ROTATE           = 1.0f;
    const float APERTURE_CIRCULARITY      = 1.0f;
    const float APERTURE_BRIGHTNESS_SCALE = 1.0f;
    const float PI                        = 3.141592f;

    float3 color = input.color.rgb * tex_color.rgb;
    float2 rPos  = input.texture_pos;

    if (coc < 0.0f) { // 前ボケのときの形状反転
        rPos *= 1.0f; 
    }
    const float deltaTheta = 2 * PI / APERTURE_BLADE;                      // ボケ形状の計算
          float theta      = atan2(rPos.y, rPos.x) + PI + APERTURE_ROTATE; // 描画点の光軸中心からの角度 (0~2pi)
    const float modTheta   = frac(theta / deltaTheta) * deltaTheta;        // APERTURE_BLADE角形のi番目頂点の角度を基準にした相対theta (0~2pi/APERTURE_BLADE)

    const float effBound = 1.0f - (1.0f - APERTURE_CIRCULARITY) * (1.0f - cos(deltaTheta / 2.0f)) * sin(APERTURE_BLADE / 2.0f * modTheta);    // bokehRadius=1.0としたときの,APERTURE_BLADE角形の境界となる境界
    const float bokehRadius2 = rPos.x * rPos.x + rPos.y * rPos.y;  // (0~1)
    const float effBokehRadius = sqrt(bokehRadius2) / effBound;
    if (effBokehRadius > 1.0f) { // ボケ形状範囲外ならば描画しない
        discard;    
    }

    // ボケの形状による減衰(明度)を計算(0.0~1.0)
    float attenuation = saturate(1.0f - pow(effBokehRadius, 10.0f)); // 後ボケは中心ほど明るい
    // 前ボケならば明度を反転,
    if (coc < 0.0f) {
        attenuation = 1.0f - attenuation;
    }
    // CoCの大きさによる減衰
    // 本来は面積に反比例するが,マスクによって,積分されるボケ減っているので,面積反比例だと,あまりに薄い
    // 厳密な計算をするべきだが,coc^-1の重みでなんとなく良さそうだったのでこれで
    const float cocAttenuationWeight = min(1.0f, 1.0f / coc);
    attenuation *= cocAttenuationWeight;
    // 明度の計算
    color.rgb *= attenuation * APERTURE_BRIGHTNESS_SCALE;
    
    return float4(color, coc);
}