struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
    float  fogFactor   : FOG;
};

Texture2D    Texture : register(t0); // Textureをスロット0テクスチャレジスタに設定
SamplerState Sampler : register(s0); // Samplerをスロット0サンプラレジスタに設定

float4 main(PS_IN input) : SV_Target {
	// テクスチャカラーの取得
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);

    // フォグ計算
    float4 fogColor  = tex_color;
    fogColor.w = 0.0f;
    return input.fogFactor * tex_color + (1.0f - input.fogFactor) * fogColor;

	// アンビエントカラー + ディフューズカラー + テクスチャカラー
	return tex_color;
}