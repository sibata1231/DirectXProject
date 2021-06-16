#include "Common.hlsli"

struct VS_OUT {
    float2 uv               : TEXCOORD0;
    float3 lightTS          : TEXCOORD1;   
    float3 viewTS           : TEXCOORD2;   
    float2 parallaxOffsetTS : TEXCOORD3;   
    float3 normalWS         : TEXCOORD4;   
    float3 viewWS           : TEXCOORD5;   
    float  fogFactor        : FOG;
   
    float4 pos              : SV_POSITION; 
};

VS_OUT main(VS_INPUT_PNTTI VIN) {
    VS_OUT VOUT;
    VOUT.pos = float4(VIN.Position, 1.0f);
    float4x4 world = mul(World,WorldInstance[VIN.ID]);
    VOUT.pos = mul(VOUT.pos, world);
    VOUT.pos = mul(VOUT.pos, View);
    VOUT.pos = mul(VOUT.pos, Projection);

    VOUT.uv = VIN.Texture;

    // ワールド空間の光源を接ベクトル空間へ移動
    float3 T = VIN.Tangent;
    float3 N = VIN.Normal;
    float3 B = cross(N, T);
    B = normalize(B);
    
    float3 normalWS   = mul(VIN.Normal,  (float3x3) World);
    float3 binormalWS = mul(B,           (float3x3) World);
    float3 tangentWS  = mul(VIN.Tangent, (float3x3) World);

    // テクスチャ
    VOUT.normalWS = normalWS;

    // 接線ベクトル、従接線ベクトル
    normalWS   = normalize(normalWS);
    binormalWS = normalize(binormalWS);
    tangentWS  = normalize(tangentWS);

    // 視線ベクトル
    float4 positionWS = mul(VIN.Position, World);
    // ここが違う
    // float3 eye = viewInv[3].xyz; (View行列のxyz座標)
    float3 viewWS = CameraVector - positionWS;
    VOUT.viewWS = viewWS;

    float3x3 worldToTangent = float3x3(tangentWS, binormalWS, normalWS);
    VOUT.lightTS = mul(worldToTangent, LightVector.xyz);
    VOUT.viewTS  = mul(worldToTangent, -viewWS);

    float2 parallaxDirection = normalize(VOUT.viewTS.xy);

    float viewLength = length(VOUT.viewTS);
    float parallaxLength = sqrt(viewLength * viewLength - VOUT.viewTS.z * VOUT.viewTS.z) / VOUT.viewTS.z;
    VOUT.parallaxOffsetTS = parallaxDirection * parallaxLength;
    VOUT.parallaxOffsetTS *= 1.0f;

    // カメラ深度計算
    //float3 camera = CameraPos * viewWS;
    //float4 cameraPosition = float4(VIN.Position + camera, 1.0f);
    //cameraPosition = mul(cameraPosition, world);
    //cameraPosition = mul(cameraPosition, View);
    //
    //VOUT.fogFactor = saturate((1.0 - cameraPosition.z) / (1.0 - 0.0f));

    return VOUT;
}