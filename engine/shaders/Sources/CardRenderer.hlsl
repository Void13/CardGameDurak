SamplerState	SamplerAnisotropic		: register(s0);
Texture2D<float4>	 Texture2DCard      : register(t0);

cbuffer CBPerBatch : register ( b0 )
{
	float2 g_vStart;
	float2 g_vEnd;
	float g_fScale;
	float g_fWidth;
	float2 g_vScreenSize;
	float4 g_vColor;
};

struct GS_INPUT
{
	float4 vPos : SV_POSITION;
};

GS_INPUT VS_CardRenderer()
{
	GS_INPUT input;
	input.vPos = float4(0.5f, 0.5f, 0.0f, 1.0f);

	return input;
};

struct PS_INPUT
{
	float4 vPos : SV_POSITION;
	float2 vTex : TEXCOORD0;
};

#define MIN_UV_VALUE -0.027f
#define MAX_UV_VALUE 1.015f

[maxvertexcount(6)]
void GS_CardRenderer(point GS_INPUT input[1], inout TriangleStream<PS_INPUT> OutStream )
{
	float2 vStart = g_vStart * g_vScreenSize;
	vStart = vStart * 2.0f - 1.0f;
	vStart.y = -vStart.y;

	//float2 vEnd = (g_vStart + float2(194.0f, 300.0f) * g_fScale) * g_vScreenSize;
	//float2 vEnd = (g_vStart + (g_vEnd - g_vStart) * g_fScale) * g_vScreenSize;
	float2 vEnd = (g_vStart + g_vEnd) * g_vScreenSize;
	vEnd = vEnd * 2.0f - 1.0f;
	vEnd.y = -vEnd.y;

	PS_INPUT output;

#ifndef FLIPPED
	output.vPos = float4(vStart.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vEnd.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vStart.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );

	
	output.vPos = float4(vEnd.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vEnd.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vStart.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );
#else
	output.vPos = float4(vStart.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vEnd.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vStart.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );

	
	output.vPos = float4(vEnd.x, vStart.y, 0.0f, 1.0f);
	output.vTex = float2(MIN_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vEnd.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MIN_UV_VALUE);
	OutStream.Append( output );

	output.vPos = float4(vStart.x, vEnd.y, 0.0f, 1.0f);
	output.vTex = float2(MAX_UV_VALUE, MAX_UV_VALUE);
	OutStream.Append( output );
#endif
}

float4 PS_CardRenderer(in PS_INPUT input) : SV_TARGET
{
	return Texture2DCard.Sample(SamplerAnisotropic, input.vTex);
};