#include "Common.hlsli"

struct VS_OUT {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float  fogFactor   : FOG;
};

VS_OUT main(VS_INPUT_PNCT input) {
	VS_OUT output;
    // ���W
	output.pos = mul(float4(input.Position,1), World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
    
    // ���_�J���[
	output.color = input.Color;
    
    // �e�N�X�`��
	output.texture_pos = input.Texture;
    
    // �@��
    float4 normal = float4(input.Normal.x, input.Normal.y, input.Normal.z, 0.0f);
	normal = mul(normal, World);
	normal = normalize(normal);

	// saturate => �����Ŏw�肵���l��0�`1�Ԃł͈̔͂Ɏ��߂�
	// dot => ���όv�Z
	output.color *= saturate(dot(normal, LightVector));
    output.fogFactor = 1.0f;
    return output;
}
