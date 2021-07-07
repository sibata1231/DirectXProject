#include "Noise.hlsli"
// CameraBuffer
cbuffer CameraBuffer : register(b0) {
    float4x4 View;
    float4x4 Projection;
    float4	 CameraPos;
    float4	 CameraVector;
};

// LightBuffer
cbuffer LightBuffer : register(b1) {
    float4   LightPos;
    float4   LightVector;
    float4   LightAmbient;
    float4   LightDiffuse;
    float4   LightSpecula;
    float4x4 LightMatrix;
};

// WorldBuffer
cbuffer WorldBuffer : register(b2){
    float4x4 WVP;
    float4x4 World;
};

// MaterialBuffer
cbuffer MaterialBuffer : register(b3) {
    float4 MaterialAmbient;	 // アンビエント
    float4 MaterialDiffuse;	 // ディフューズ
    float4 MaterialSpecula;  // スペキュラー
    float4 MaterialEmissive; // エミッシブ
};

// TextureBuffer
cbuffer TextureBuffer : register(b4) {
    float4x4 TextureMatrix;	 // テクスチャ行列(転置行列)
};

// InstanceBuffer
cbuffer InstanceBuffer : register(b5) {
    matrix WorldInstance[5 * 5];	 // インスタンスバッファ行列
};

// POMBuffer
cbuffer POMBuffer : register(b6) {
    float  specularArea;  //!< スペキュラー範囲
    float  specularPower; //!< スペキュラー強度
    float  UFrame;        //!< UVフレーム
    float  VFrame;        //!< UVフレーム
}

struct Wave {
	float2 dir;
	float amplitude;
	float waveLength;
};

// Tessellation
cbuffer TessellationBuffer : register(b7) {
    float tessellationAmount; //!< Tessellation係数
	float Hscale;             //!< サイズ
	float Hbias;              //!< バイアス
    float time;               //!< 予備
    Wave  waves[100];
}

//============================================================================================
// Struct_Input
//============================================================================================

struct VS_INPUT_PNCT {
    float3	Position : POSITION;
    float3	Normal   : NORMAL;
    float4	Color    : COLOR;
    float2	Texture  : TEXTURE;
};

struct VS_INPUT_PNCTI {
    float3	Position : POSITION;
    float3	Normal   : NORMAL;
    float4	Color    : COLOR;
    float2	Texture  : TEXTURE;
    uint   ID        : SV_InstanceID;
};

struct VS_INPUT_P {
    float3	Position : POSITION;
};

struct VS_INPUT_PCT {
    float3	Position : POSITION;
    float4	Color    : COLOR;
    float2	Texture  : TEXTURE;
};

struct VS_INPUT_PNTTI {
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 Texture  : TEXTURE;
    float3 Tangent  : TANGENT;
    uint   ID       : SV_InstanceID;
};

// Struct representing a single vertex worth of data
struct VS_INPUT_PNTT {
	float3 position		: POSITION;
    float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
};

struct  VS_INPUT_P3T {
	float4 position		: SV_POSITION;
	float3 uvw			: TEXCOORD;
};

struct VertexToHull {
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

struct HullToDomain {
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

struct DomainToPixel {
	float4 vPosition	: SV_POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};
#define NUM_CONTROL_POINTS 3

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT {
	float EdgeTessFactor[NUM_CONTROL_POINTS] : SV_TessFactor;       // e.g. would be [4] for a quad domain
	float InsideTessFactor                   : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip,uint PatchID : SV_PrimitiveID) {
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = tessellationAmount;
	Output.EdgeTessFactor[1] = tessellationAmount;
	Output.EdgeTessFactor[2] = tessellationAmount;
	//Output.EdgeTessFactor[3] = tessellationAmount;
	Output.InsideTessFactor = tessellationAmount; // e.g. could calculate dynamic tessellation factors instead
	//Output.InsideTessFactor[1]  = tessellationAmount;
	return Output;
}