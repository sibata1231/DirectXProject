/**
 * @file PixelParicle.hlsl
 * @brief �p�[�e�B�N���p
 * @author T.Shibata
 * @date 2020/11/25 �쐬
 */

#include "Common.hlsli"

// �p�����[�^
struct VS_OUTPUT {
	float4	Position	: SV_POSITION;
	float3	Normal		: TEXCOORD0;
	float2	TexCoord	: TEXCOORD1;
	float4	Diffuse		: COLOR0;
};

Texture2D    g_texture : register(t0);	// �e�N�X�`��
SamplerState g_sampler : register(s0);	// �T���v��

float4 main(VS_OUTPUT input) : SV_Target0 {
	float3 Diff  = input.Diffuse.rgb * MaterialDiffuse.rgb;
	float  Alpha = input.Diffuse.a   * MaterialDiffuse.a;
    float4 vTd   = g_texture.Sample(g_sampler, input.TexCoord);
    Diff        *= vTd.rgb;
    Alpha       *= vTd.a;
	return float4(Diff, Alpha);
}