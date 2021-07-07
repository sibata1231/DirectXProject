
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