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

// 乱数
float rand(float3 value) {
    return frac(sin(dot(value, float3(12.9898, 78.233,56787))) * 43758.5453);
}

// ノイズ
float noise(float3 value) {
    float3 ip = floor(value);
    float3 fp = smoothstep(0, 1, frac(value));
    float4 a = float4(
        rand(ip + float3(0, 0, 0)),
        rand(ip + float3(1, 0, 0)),
        rand(ip + float3(0, 1, 0)),
        rand(ip + float3(1, 1, 0)));
    float4 b = float4(
        rand(ip + float3(0, 0, 1)),
        rand(ip + float3(1, 0, 1)),
        rand(ip + float3(0, 1, 1)),
        rand(ip + float3(1, 1, 1)));
    a = lerp(a, b, fp.z);
    a.xy = lerp(a.xy, a.zw, fp.y);
    return lerp(a.x, a.y, fp.x);
}

float perlin(float3 value) {
    return (noise(value) +
        noise(value * 2) +
        noise(value * 4) +
        noise(value * 8) +
        noise(value * 16) +
        noise(value * 32)) / 6;
}

float _perlin(float3 value) {
    return (noise(value)      * 32 +
            noise(value * 2)  * 16 +
            noise(value * 4)  *  8 +
            noise(value * 8)  *  4 +
            noise(value * 16) *  2 +
            noise(value * 32)) / 63;
}