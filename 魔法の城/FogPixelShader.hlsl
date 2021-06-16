struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float  fogFactor   : FOG;
};

Texture2D    Texture : register(t0); // Texture���X���b�g0�e�N�X�`�����W�X�^�ɐݒ�
SamplerState Sampler : register(s0); // Sampler���X���b�g0�T���v�����W�X�^�ɐݒ�

float4 main(PS_IN input) : SV_Target {
	// �e�N�X�`���J���[�̎擾
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);

    // �t�H�O�v�Z
    float4 fogColor  = tex_color;
    fogColor.w = 0.0f;
    return input.fogFactor * tex_color + (1.0f - input.fogFactor) * fogColor;

	// �A���r�G���g�J���[ + �f�B�t���[�Y�J���[ + �e�N�X�`���J���[
	return tex_color;
}