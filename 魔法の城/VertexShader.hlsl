/**
 * @file VertexShader.hlsl
 * @brief 通常モデル
 * @author T.Shibata
 * @date 2020/11/24 作成
 */

#include "Common.hlsli"

struct VS_OUT {
    float4 pos         : SV_POSITION;
    float4 color       : COLOR;
    float2 texture_pos : TEXTURE0;
};

VS_OUT main(VS_INPUT_PNCT input) {
	VS_OUT output;
	// ローカル座標 * ワールド座標変換行列
	output.pos = mul(float4(input.Position,1), World);
	// ワールド座標 * ビュー座標変換行列
	output.pos = mul(output.pos, View);
	// ビュー座標 * プロジェクション座標変換行列
	output.pos = mul(output.pos, Projection);
    
	// 頂点カラー
	output.color = input.Color;
    
	// Texture座標指定
	output.texture_pos = input.Texture;
    
    float4 normal = float4(input.Normal.x, input.Normal.y, input.Normal.z, 0.0f);
	// 頂点の法線にワールド行列を掛け合わせて
	// ワールド座標上での法線の向きに変換する
	normal = mul(normal, World).xyzw;
	normal = normalize(normal);
	// saturate => 引数で指定した値を0～1間での範囲に収める
	// dot => 内積計算
	output.color *= saturate(dot(normal, LightVector));

    return output;
}
