/**
 * @file AnimationVertex.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/25 作成
 */

#include "Common.hlsli"

// <頂点シェーダ>
float4 main(float4 Pos : POSITION) : SV_POSITION { 
	return mul(Pos, WVP);
}