#include "Common.hlsli"

struct VS_OUT {
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float3 eye   : TEXCOORD1;
    float3 light : TEXCOORD2;
};

VS_OUT main(VS_INPUT_PNTTI VIN) {
    VS_OUT VOUT;

    // 頂点計算
    VOUT.pos = float4(VIN.Position, 1);
    VOUT.pos = mul(VOUT.pos, World);
    VOUT.pos = mul(VOUT.pos, View);
    VOUT.pos = mul(VOUT.pos, Projection);

    // UV
    VOUT.uv = VIN.Texture;

    // ワールド空間の光源を接ベクトル空間変換
    float3 T = VIN.Tangent;
    float3 N = VIN.Normal;
    float3 B = cross(N, T);
    B = normalize(B);

    //視線ベクトル計算
    float3 eye = normalize(CameraVector.xyz - VIN.Position.xyz);
    VOUT.eye.x = dot(T, eye);
    VOUT.eye.y = dot(B, eye);
    VOUT.eye.z = dot(N, eye);

    // ライトベクトルを計算
    float3 light = normalize(-LightVector.xyz);

    //ライトベクトルを頂点座標系に変換
    VOUT.light.x = dot(T, light);
    VOUT.light.y = dot(B, light);
    VOUT.light.z = dot(N, light);

    return VOUT;
}