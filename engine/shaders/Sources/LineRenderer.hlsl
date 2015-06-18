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

GS_INPUT VS_LineRenderer()
{
	GS_INPUT input;
	input.vPos = float4(0.5f, 0.5f, 0.0f, 1.0f);

	return input;
};

struct PS_INPUT
{
	float4 vPos : SV_POSITION;
};

[maxvertexcount(6)]
void GS_LineRenderer(point GS_INPUT input[1], inout TriangleStream<PS_INPUT> OutStream )
{
	float2 vStart = g_vStart * g_vScreenSize;
	vStart = vStart * 2.0f - 1.0f;
	vStart.y = -vStart.y;

	float2 vEnd = g_vEnd * g_vScreenSize;
	vEnd = vEnd * 2.0f - 1.0f;
	vEnd.y = -vEnd.y;
	
	float2 vNormal = normalize(g_vEnd - g_vStart) * float2(1.0f, g_vScreenSize.x / g_vScreenSize.y);
	vNormal = float2(vNormal.y, vNormal.x);

	PS_INPUT output;

	output.vPos = float4(vStart - vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	output.vPos = float4(vStart + vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	output.vPos = float4(vEnd - vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	
	output.vPos = float4(vStart + vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	output.vPos = float4(vEnd + vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	output.vPos = float4(vEnd - vNormal * g_fWidth, 0.0f, 1.0f);
	OutStream.Append( output );
	
}

float4 PS_LineRenderer(in PS_INPUT input) : SV_TARGET
{
	return g_vColor;
};