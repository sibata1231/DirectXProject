/**
 * @file VertexShader.hlsl
 * @brief �ʏ탂�f��
 * @author T.Shibata
 * @date 2020/11/24 �쐬
 */

#include "Common.hlsli"

struct VS_OUT {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

VS_OUT main(VS_INPUT_PNCT input) {
	VS_OUT output;
	// ���[�J�����W * ���[���h���W�ϊ��s��
	output.pos = mul(float4(input.Position,1), World);
	// ���[���h���W * �r���[���W�ϊ��s��
	output.pos = mul(output.pos, View);
	// �r���[���W * �v���W�F�N�V�������W�ϊ��s��
	output.pos = mul(output.pos, Projection);
    
	// ���_�J���[
	output.color = input.Color;
    
	// Texture���W�w��
	output.texture_pos = input.Texture;
    
    float4 normal = float4(input.Normal.x, input.Normal.y, input.Normal.z, 0.0f);
	// ���_�̖@���Ƀ��[���h�s����|�����킹��
	// ���[���h���W��ł̖@���̌����ɕϊ�����
	normal = mul(normal, World).xyzw;
	normal = normalize(normal);
	// saturate => �����Ŏw�肵���l��0�`1�Ԃł͈̔͂Ɏ��߂�
	// dot => ���όv�Z
	output.color *= saturate(dot(normal, LightVector));

    return output;
}
