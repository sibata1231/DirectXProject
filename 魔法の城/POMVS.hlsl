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

    // ���[���h��Ԃ̌�����ڃx�N�g����Ԃֈړ�
    float3 T = VIN.Tangent;
    float3 N = VIN.Normal;
    float3 B = cross(N, T);
    B = normalize(B);
    
    float3 normalWS   = mul(VIN.Normal,  (float3x3) World);
    float3 binormalWS = mul(B,           (float3x3) World);
    float3 tangentWS  = mul(VIN.Tangent, (float3x3) World);

    // �e�N�X�`��
    VOUT.normalWS = normalWS;

    // �ڐ��x�N�g���A�]�ڐ��x�N�g��
    normalWS   = normalize(normalWS);
    binormalWS = normalize(binormalWS);
    tangentWS  = normalize(tangentWS);

    // �����x�N�g��
    float4 positionWS = mul(VIN.Position, World);
    // �������Ⴄ
    // float3 eye = viewInv[3].xyz; (View�s���xyz���W)
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

    // �J�����[�x�v�Z
    //float3 camera = CameraPos * viewWS;
    //float4 cameraPosition = float4(VIN.Position + camera, 1.0f);
    //cameraPosition = mul(cameraPosition, world);
    //cameraPosition = mul(cameraPosition, View);
    //
    //VOUT.fogFactor = saturate((1.0 - cameraPosition.z) / (1.0 - 0.0f));

    return VOUT;
}