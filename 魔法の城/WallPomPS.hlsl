#include "Common.hlsli"

struct PS_IN {
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float3 eye   : TEXCOORD1;
    float3 light : TEXCOORD2;
};

// CPU側
Texture2D tex[2] : register(t0);
SamplerState samp : register(s0);


float4 main(PS_IN PIN) : SV_TARGET {
    //noise(tex[0].Sample(samp, uv).xyz)
    float2 uv        = float2(PIN.uv.x + UFrame, PIN.uv.y + VFrame);     // UV
    float  heightMap = tex[1].Sample(samp, uv).a;                       // 法線マップ(高さ取得)
    float2 texel     = PIN.uv + 1.0 * heightMap * PIN.eye.xy + uv;           // 視線ベクトルによってずらす
    float3 normal    = 2.0f * tex[1].Sample(samp, float2(texel.x + UFrame, texel.y +  VFrame)).xyz - 1.0f;        // 法線取得
    float3 phong     = normalize(PIN.light + PIN.eye);                   // Phong
    float  specular  = pow(max(0.0f, dot(normal, phong)), 1.0f) * 1.0f;  // Specular
    float  amb       = MaterialAmbient;
    return tex[0].Sample(samp, uv) * max(amb, dot(normal, PIN.light));     // Color調整
}