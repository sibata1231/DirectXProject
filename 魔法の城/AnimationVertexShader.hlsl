/**
 * @file AnimationVertex.hlsl
 * @brief 
 * @author T.Shibata
 * @date 2020/11/25 �쐬
 */

#include "Common.hlsli"

// <���_�V�F�[�_>
float4 main(float4 Pos : POSITION) : SV_POSITION { 
	return mul(Pos, WVP);
}