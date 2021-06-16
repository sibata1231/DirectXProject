struct PS_IN {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 lightPos : TEXCOORD1;
};

Texture2D    tex    : register(t0);
Texture2D    shadow : register(t1);
SamplerState samp   : register(s0);

float4 main(PS_IN PIN) : SV_Target {
    float4 color       = tex.Sample(samp, PIN.uv);
    float2 shadowUV    = PIN.lightPos.xy / PIN.lightPos.w;
    float  depth       = PIN.lightPos.z  / PIN.lightPos.w;
    float  shadowDepth = shadow.Sample(samp, shadowUV).r;
    if (depth > shadowDepth + 0.01f) {
        color.rgb *= 0.7f;
    }
    return color;
}