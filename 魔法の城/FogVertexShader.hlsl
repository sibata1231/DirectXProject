#include "Common.hlsli"

struct VS_OUT {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float  fogFactor   : FOG;
};

VS_OUT main(VS_INPUT_PNCT input) {
	VS_OUT output;
    // 座標
	output.pos = mul(float4(input.Position,1), World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
    
    // 頂点カラー
	output.color = input.Color;
    
    // テクスチャ
	output.texture_pos = input.Texture;
    
    // 法線
    float4 normal = float4(input.Normal.x, input.Normal.y, input.Normal.z, 0.0f);
	normal = mul(normal, World);
	normal = normalize(normal);

	// saturate => 引数で指定した値を0～1間での範囲に収める
	// dot => 内積計算
	output.color *= saturate(dot(normal, LightVector));
    output.fogFactor = 1.0f;
    return output;
}
