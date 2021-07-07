#include "Common.hlsli"

Texture2D heightSRV			: register(t0);
SamplerState heightSampler	: register(s0);

static int numWaves = 20;
static float steepness = 1.5;
static float speed = 5;

float3 CalcGerstnerWaveOffset(float3 v) {
	float3 sum = v;
	[unroll]
	for (int i = 0; i < numWaves; i++)
	{
		Wave wave = waves[i];
		float wi = 2 / wave.waveLength;
		float Qi = steepness / (wave.amplitude * wi * numWaves);
		float phi = speed * wi;
		float rad = dot(wave.dir, v.xz) * wi + time * phi;
		sum.y  += sin(rad) * wave.amplitude + _perlin(v).x;
		sum.xz += cos(rad) * wave.amplitude * Qi * wave.dir;
	}
	return sum;
}

[domain("tri")]
DomainToPixel main(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HullToDomain, NUM_CONTROL_POINTS> patch) {
	DomainToPixel Output;
	
	Output.normal   = domain.x*patch[0].normal   + domain.y*patch[1].normal   + domain.z*patch[2].normal;
	Output.tangent  = domain.x*patch[0].tangent  + domain.y*patch[1].tangent  + domain.z*patch[2].tangent;
	Output.worldPos = domain.x*patch[0].worldPos + domain.y*patch[1].worldPos + domain.z*patch[2].worldPos;
	Output.worldPos = CalcGerstnerWaveOffset(Output.worldPos);
	Output.uv       = domain.x*patch[0].uv       + domain.y*patch[1].uv       + domain.z*patch[2].uv;

	Output.normal = normalize(Output.normal);

	//Height map calc
	float hScale 	 = Hscale;
	float hBias  	 = Hbias;
	float3 vDir  	 = Output.normal;
	float hMap   	 = heightSRV.SampleLevel(heightSampler, Output.uv.xy, 0).r;
	hMap 			*= hScale;
	hMap 			+= hBias;
	Output.worldPos += hMap * vDir;

	matrix viewProj = mul(View, Projection);

	Output.vPosition = mul(float4(Output.worldPos, 1.0f),viewProj);
	return Output;
}

//float3 p1 = lerp( patch[1].worldPos , patch[0].worldPos , domain.x );
//float3 p2 = lerp( patch[2].worldPos, patch[3].worldPos, domain.x );
//float3 p3 = lerp( p1, p2, domain.y);