/**
 * @file AnimationPixel.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/25 作成
 */

// <ピクセルシェーダ>
float4 main(float4 Pos : SV_POSITION) : SV_Target {
	return float4(1.5, 0.5, 0.5, 1.0);
}