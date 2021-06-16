/**
 * @file RimLightPixelShader.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/26 �쐬
 */

#include "Common.hlsli"
struct PS_IN {
    float4 pos         : SV_POSITION;
    float3 nor         : NORMAL0;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float3 light_rim   : TEXCOORD0;
};

Texture2D    Texture : register(t0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�

float4 main(PS_IN input) : SV_Target {
    float4 eye          = dot(input.pos,CameraVector);          // �����x�N�g���v�Z
    float3 eye_vector   = normalize(eye.xyz);                   // �����x�N�g��
    float3 light_vector = normalize(LightVector.xyz);           // ���C�g�x�N�g��
    float3 normal = normalize(input.nor);                       // �@��
    float rim_power = 2.0f;                                     // �������C�g�̋���
    float rim = 1.0 - dot(normal, eye_vector);                  // ���ς̒l�𔽓]�����f���̉�������1.0�A�����ƕ��s������0.0�ɂ���B
    rim = pow(abs(rim), rim_power);                             // �������C�g�̋�����␳
    float light_rim = max(dot(-light_vector, eye_vector), 0.0); // ���z�Ƃ̋t�����v�Z�i���_�V�F�[�_��OK�j
    float4 color = rim * light_rim;                             // ���f���̉��v�Z�ƁA���z�̋t���v�Z����Z
    color.w = 1.0f;
    return color;
}
