/**
 * @file Pixel2D.hlsl
 * @brief UI�V�F�[�_�[
 * @author T.Shibata
 * @date 2020/11/24 �쐬
 */

#include "Common.hlsli"

// �p�����[�^
struct VS_OUTPUT {
	float4	Position : SV_Position;
	float2	TexCoord : TEXCOORD0;
	float4	Diffuse	 : COLOR0;
};

Texture2D    m_texture : register(t0);	// �e�N�X�`��
SamplerState m_sampler : register(s0);	// �T���v��

float4 main(VS_OUTPUT input) : SV_Target0 {
	float4 Color = input.Diffuse;
	if (TextureMatrix._44 > 0.0f) {
		Color *= m_texture.Sample(m_sampler, input.TexCoord);
	}
	return Color;
}
