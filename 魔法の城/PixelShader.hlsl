/**
 * @file PixelShader.hlsl
 * @brief �ʏ탂�f��
 * @author T.Shibata
 * @date 2020/11/24 �쐬
 */
struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

Texture2D    Texture : register(t0); // Texture���X���b�g0��0�Ԗڂ̃e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0); // Sampler���X���b�g0��0�Ԗڂ̃T���v�����W�X�^�ɐݒ�

float4 main(PS_IN input) : SV_Target {
	// �e�N�X�`���J���[�̎擾
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    
	// �A���r�G���g�J���[ + �f�B�t���[�Y�J���[ + �e�N�X�`���J���[
	return tex_color;
}