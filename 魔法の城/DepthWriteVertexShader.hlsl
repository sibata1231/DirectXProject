#include "Common.hlsli"

struct VS_OUT {
    float4 pos       : SV_POSITION;
    float2 uv        : TEXCOORD0;
    float4 shadowPos : TEXCOORD1;
};

VS_OUT main(VS_INPUT_PNCT VIN) {
    VS_OUT VOUT;
    VOUT.pos = float4(VIN.Position, 1);
    VOUT.pos = mul(VOUT.pos, World);
    VOUT.pos = mul(VOUT.pos, View);
    VOUT.pos = mul(VOUT.pos, Projection);
    VOUT.shadowPos = VOUT.pos;

    VOUT.uv = VIN.Texture;

    return VOUT;
}