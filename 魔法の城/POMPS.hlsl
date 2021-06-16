#include "Common.hlsli"

struct PS_IN {
    float2 uv               : TEXCOORD0;
    float3 lightTS          : TEXCOORD1;   // Light vector in tangent space, denormalized
    float3 viewTS           : TEXCOORD2;   // View vector in tangent space, denormalized
    float2 parallaxOffsetTS : TEXCOORD3;   // Parallax offset vector in tangent space
    float3 normalWS         : TEXCOORD4;   // Normal vector in world space
    float3 viewWS           : TEXCOORD5;   // View vector in world space
    float  fogFactor        : FOG;

    float4 position         : SV_POSITION; // Output position
};

// CPU側
Texture2D tex[2] : register(t0);
SamplerState samp : register(s0);

float4 ComputeIllumination(float2 texCoord, float3 lightTS, float3 viewTS, float fogFactor) {
    float2 uv           = float2(texCoord.x + UFrame, texCoord.y + VFrame);
    float3 normalTS     = normalize(tex[1].Sample(samp, uv) * 2.0f - 1.0f);
    float4 baseColor    = tex[0].Sample(samp, uv);
    float4 diffuse      = saturate(dot(normalTS, lightTS)) * MaterialDiffuse;
    float4 specular     = 0;
    float3 reflectionTS = normalize(2 * dot(viewTS, normalTS) * normalTS - viewTS);
    float rdotL         = saturate(dot(reflectionTS, lightTS));
    specular            = pow(rdotL, specularPower) * MaterialSpecula;
    float4 color        = (MaterialAmbient + diffuse) * baseColor + specular;
    float4 fogColor = color;
    fogColor.w = 0.0f;
    return fogFactor * color + (1.0f - fogFactor) * fogColor;
}

float4 main(PS_IN PIN) : SV_TARGET {
    // 正規化
    float3 viewTS   = normalize(PIN.viewTS);
    float3 viewWS   = normalize(PIN.viewWS);
    float3 lightTS  = normalize(PIN.lightTS);
    float3 normalWS = normalize(PIN.normalWS);

    // 最終カラー
    float4 resultColor = float4(0, 0, 0, 1);

    // MIPMAP
    float2 dx = ddx(PIN.uv);
    float2 dy = ddy(PIN.uv);

    // ステップ数計算
    int numSteps = (int)lerp(1.0f, 0.0f, dot(viewWS, normalWS));

    // 演算変数
    float  currHeight       = 0.0f;
    float  stepSize         = 1.0f / (float)numSteps;
    float  prevHeight       = 1.0f;
    float  nextHeight       = 0.0f;
    float  currentBound     = 1.0f;
    float  parallaxAmount   = 0.0f;
    int    stepIndex        = 0;
    float2 texOffsetPerStep = stepSize * PIN.parallaxOffsetTS;
    float2 texCurrentOffset = PIN.uv;
    float2 pt1              = 0;
    float2 pt2              = 0;
    float2 texOffset2       = 0;

    // 深度計算
    while (stepIndex < numSteps) {
       texCurrentOffset -= texOffsetPerStep;
       currHeight = tex[1].SampleGrad(samp, texCurrentOffset, dx, dy).a;
       currentBound -= stepSize;
       if (currHeight > currentBound) {
          pt1 = float2(currentBound, currHeight);
          pt2 = float2(currentBound + stepSize, prevHeight);
          texOffset2 = texCurrentOffset - texOffsetPerStep;
          stepIndex = numSteps + 1;
       } else {
          stepIndex++;
          prevHeight = currHeight;
       }
    }

    // 割合計算
    float delta2 = pt2.x - pt2.y;
    float delta1 = pt1.x - pt1.y;
    float denominator = delta2 - delta1;
    [flatten]
    if (denominator == 0.0f) {
        parallaxAmount = 0.0f;
    } else {
        parallaxAmount = (pt1.x * delta2 - pt2.x * delta1) / denominator;
    }

    // 視線調整
    float2 parallaxOffset = PIN.parallaxOffsetTS * (1.0f - parallaxAmount);
    float2 texSample      = PIN.uv - parallaxOffset;

    // カラー算出
    resultColor = ComputeIllumination(texSample, lightTS, viewTS, PIN.fogFactor);

    return resultColor;
}