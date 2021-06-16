/**
 * @file PixelShader.hlsl
 * @brief 通常モデル
 * @author T.Shibata
 * @date 2020/11/24 作成
 */
struct PS_IN {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

Texture2D    Texture : register(t0); // Textureをスロット0の0番目のテクスチャレジスタに設定
SamplerState Sampler : register(s0); // Samplerをスロット0の0番目のサンプラレジスタに設定

float4 main(PS_IN input) : SV_Target {
	// テクスチャカラーの取得
	float4 tex_color = Texture.Sample(Sampler, input.texture_pos);
    
	// アンビエントカラー + ディフューズカラー + テクスチャカラー
	return tex_color;
}