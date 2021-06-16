/**
 * @file BokehPixelShader.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/26 �쐬
 */
struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

Texture2D    Texture : register(t0[0]); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0[0]); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�

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

    if (coc < 0.0f) { // �O�{�P�̂Ƃ��̌`�󔽓]
        rPos *= 1.0f; 
    }
    const float deltaTheta = 2 * PI / APERTURE_BLADE;                      // �{�P�`��̌v�Z
          float theta      = atan2(rPos.y, rPos.x) + PI + APERTURE_ROTATE; // �`��_�̌������S����̊p�x (0~2pi)
    const float modTheta   = frac(theta / deltaTheta) * deltaTheta;        // APERTURE_BLADE�p�`��i�Ԗڒ��_�̊p�x����ɂ�������theta (0~2pi/APERTURE_BLADE)

    const float effBound = 1.0f - (1.0f - APERTURE_CIRCULARITY) * (1.0f - cos(deltaTheta / 2.0f)) * sin(APERTURE_BLADE / 2.0f * modTheta);    // bokehRadius=1.0�Ƃ����Ƃ���,APERTURE_BLADE�p�`�̋��E�ƂȂ鋫�E
    const float bokehRadius2 = rPos.x * rPos.x + rPos.y * rPos.y;  // (0~1)
    const float effBokehRadius = sqrt(bokehRadius2) / effBound;
    if (effBokehRadius > 1.0f) { // �{�P�`��͈͊O�Ȃ�Ε`�悵�Ȃ�
        discard;    
    }

    // �{�P�̌`��ɂ�錸��(���x)���v�Z(0.0~1.0)
    float attenuation = saturate(1.0f - pow(effBokehRadius, 10.0f)); // ��{�P�͒��S�قǖ��邢
    // �O�{�P�Ȃ�Ζ��x�𔽓],
    if (coc < 0.0f) {
        attenuation = 1.0f - attenuation;
    }
    // CoC�̑傫���ɂ�錸��
    // �{���͖ʐςɔ���Ⴗ�邪,�}�X�N�ɂ����,�ϕ������{�P�����Ă���̂�,�ʐϔ���Ⴞ��,���܂�ɔ���
    // �����Ȍv�Z������ׂ�����,coc^-1�̏d�݂łȂ�ƂȂ��ǂ������������̂ł����
    const float cocAttenuationWeight = min(1.0f, 1.0f / coc);
    attenuation *= cocAttenuationWeight;
    // ���x�̌v�Z
    color.rgb *= attenuation * APERTURE_BRIGHTNESS_SCALE;
    
    return float4(color, coc);
}