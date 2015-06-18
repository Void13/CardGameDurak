#define DIRECTINPUT_VERSION 0x8000
#define _CRT_NON_CONFORMING_SWPRINTFS 1
//#define _CRT_SECURE_NO_WARNINGS 1

#include <Windows.h>

#include <assert.h>
#include <crtdbg.h>
#include <vector>
#include <time.h>

#include "AUT.h"
#include "AUTshaders.h"

#include "Game.h"

/*
	Callback: OnCreateDevice

	Called after creating device.

	Parameters:
	
	Returns:
		Error code.
*/
HRESULT OnCreateDevice();

/*
	Callback: OnResetDevice

	Called after reseting device.

	Parameters:
	
	Returns:
		Error code.
*/
HRESULT OnResetDevice ();

/*
	Callback: OnRelease

	Called after releasing the app.

	Parameters:
	
	Returns:
*/
void OnRelease();

/*
	Callback: OnInput

	Called after getting the codes of keys in keyboard and mouse.

	Parameters:
		pDIMouseState - mouse keys.
		pKeyboardState - keyboard keys.

	Returns:
		Access to kill the app.
*/
bool OnInput( LPDIMOUSESTATE pDIMouseState, LPKEYBOARDSTATE pKeyboardState );

/*
	Callback: OnFrameProcess

	Called in every tick of cycle.

	Parameters:
		pD3DDevice - d3d device.
		pDSTexture - depth stencil texture.
		pDSView - depth stencil view.
		fTime - running time of application.
		fElapsedTime - time from last Present().
	
	Returns:
*/
void OnFrameProcess( ID3D11Device *pD3DDevice, ID3D11Texture2D *pDSTexture, ID3D11DepthStencilView *pDSView, double fTime, double fElapsedTime );


CCard						g_Suits[CARD_SUIT_COUNT];

CCard						g_ExitBut;
CCard						g_NextMoveBut;
CCard						g_TakeBut;

bool						g_useDebInfo;
bool						g_bCreated = false;
bool						g_bFullScreen = false;

int							g_nwScreen = 768,
							g_nhScreen = 540;

HRTimer						g_Timer;


SHADERID					g_SID_PS_LineRenderer;
SHADERID					g_SID_GS_LineRenderer;
SHADERID					g_SID_VS_LineRenderer;

SAMPLERID					g_SAMPID_ANISOTROPIC;

SHADERID					g_SID_PS_CardRenderer;
SHADERID					g_SID_GS_CardRenderer;
SHADERID					g_SID_VS_CardRenderer;

SHADERID					g_SID_PS_CardRendererFlipped;
SHADERID					g_SID_GS_CardRendererFlipped;
SHADERID					g_SID_VS_CardRendererFlipped;

D3D11_VIEWPORT				g_ScreenViewport;
ID3D11RasterizerState		*g_pRastState = nullptr;
ID3D11BlendState			*g_pBlendState = nullptr;

ID3D11Texture2D				*g_pBackBufferTexture = nullptr;
ID3D11ShaderResourceView	*g_pBackBufferSRV = nullptr;
ID3D11RenderTargetView		*g_pBackBufferRTV = nullptr;

/*
	Callback: wWinMain

	Called after starting the app.

	Parameters:
		hInstance - handle to an instance.
		hPrevInstance - has no meaning.
		lpCmdLine - the command-line arguments.
		nCmdShow - a flag that says whether the main application window will be minimized, maximized, or shown normally.
	
	Returns:
		A code of error which can used in other app.
*/
int CALLBACK wWinMain(
	__in  HINSTANCE hInstance,
	__in  HINSTANCE hPrevInstance,
	__in  LPWSTR lpCmdLine,
	__in  int nCmdShow
	){
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	HRESULT hr;

	g_Timer.Reset();

	srand(static_cast<unsigned int>(time(NULL)));

	CreateAUT();
	AUTSetCallbackOnCreateDevice( &OnCreateDevice );
	AUTSetCallbackOnResetDevice( &OnResetDevice );
	AUTSetCallbackOnRelease( &OnRelease );
	AUTSetCallbackOnFrameProcess( &OnFrameProcess );
	AUTSetCallbackOnInput( &OnInput );
	g_useDebInfo = true;
	int nX = 100, nY = 100;

	if( g_bFullScreen ) 
	{
		nX = GetSystemMetrics( SM_XVIRTUALSCREEN );
		nY = GetSystemMetrics( SM_YVIRTUALSCREEN );
		g_nwScreen = GetSystemMetrics( SM_CXSCREEN );
		g_nhScreen =  GetSystemMetrics( SM_CYSCREEN );
	}

	hr = AUTCreateWindow( L"Aurora Engine", RGB( 32, 32, 255 ), nX, nY, g_nwScreen, g_nhScreen,
							hInstance, LoadIcon( NULL, IDI_APPLICATION ), LoadCursor( NULL, IDC_ARROW ), NULL );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, L"Error: AUTCreateWindow", L"FATAL ERROR", MB_OK | MB_ICONERROR );
		return 0;
	}

	DXGI_SWAP_CHAIN_DESC SCDesc;
	ZeroMemory( &SCDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	SCDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	SCDesc.BufferDesc.RefreshRate.Numerator = 60;
	SCDesc.BufferDesc.RefreshRate.Denominator = 1;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.SampleDesc.Count = 1;
	SCDesc.SampleDesc.Quality = 0;
	//SCDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SCDesc.OutputWindow = AUTGetHWND();
	SCDesc.BufferDesc.Width = g_nwScreen;
	SCDesc.BufferDesc.Height = g_nhScreen;
	SCDesc.Windowed = !g_bFullScreen;
	SCDesc.BufferCount = 2;
	if (g_bFullScreen) 
		SCDesc.BufferCount = 3;

	AUTSetSwapChainParams( &SCDesc );

	hr = AUTCreateDevice();
	if( FAILED( hr ) )
	{
		MessageBox( NULL, L"Error: AUTCreateDevice", L"FATAL ERROR", MB_OK | MB_ICONERROR );
		return 0;
	}

	int nExCode = AUTMainLoop();
	AUTRelease();
	return nExCode;
}

HRESULT OnCreateDevice()
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();
	ID3D11Device *pD3DDevice = AUTGetD3DDevice();

	HRESULT hr;

	SetWaitingPresent(false);

	DXGI_SWAP_CHAIN_DESC *pSCDesc;
	pSCDesc = AUTGetSwapChainParams();

	AUTShadersInit();
	
	V_RETURN(InitDeck());
	InitField();

	g_Suits[CARD_SUIT_CLUB].Init(L"data\\models\\images\\Club.png");
	g_Suits[CARD_SUIT_DIAMOND].Init(L"data\\models\\images\\Diamond.png");
	g_Suits[CARD_SUIT_HEART].Init(L"data\\models\\images\\Heart.png");
	g_Suits[CARD_SUIT_SPADE].Init(L"data\\models\\images\\Spade.png");

	g_ExitBut.Init(L"data\\models\\images\\Exit.png");
	g_NextMoveBut.Init(L"data\\models\\images\\NextMove.png");
	g_TakeBut.Init(L"data\\models\\images\\Take.png");

	g_ExitBut.SetCardStartPos(XMFLOAT2(g_nwScreen - 108.0f, 2.0f));
	g_ExitBut.SetCardEndPos(XMFLOAT2(90.0f, 20.0f));

	g_NextMoveBut.SetCardStartPos(XMFLOAT2(100.0f, 320.0f));
	g_NextMoveBut.SetCardEndPos(XMFLOAT2(90.0f, 20.0f));

	g_TakeBut.SetCardStartPos(XMFLOAT2(100.0f, 320.0f));
	g_TakeBut.SetCardEndPos(XMFLOAT2(90.0f, 20.0f));

	V_RETURN(AUTCreateAllConstantBuffers());

	DWORD dwNull = -1;
	AUTSetVShader(dwNull, pImmediateContext);
	AUTSetPShader(dwNull, pImmediateContext);
	AUTSetGShader(dwNull, pImmediateContext);
	AUTSetHShader(dwNull, pImmediateContext);
	AUTSetDShader(dwNull, pImmediateContext);
	AUTSetCShader(dwNull, pImmediateContext);

	CCBPerBatch *cbPerBatch = AUTGetCBPerBatch();
	cbPerBatch->fScale = CARD_SCALE;
	cbPerBatch->fWidth = 0.25f;
	cbPerBatch->vColor = XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f);
	cbPerBatch->vEnd = XMFLOAT2(394, 500);
	cbPerBatch->vStart = XMFLOAT2(200, 200);
	cbPerBatch->vScreenSize = XMFLOAT2(1.0f / g_nwScreen, 1.0f / g_nhScreen);
	//AUTSetCBPerBatch(cbPerBatch);

	AUTUpdateD3DCBPerBatch(pImmediateContext);

	V_RETURN(AUTAddVShader(L"engine\\shaders\\LineRenderer.hlsl", "VS_LineRenderer", "VS_LineRenderer", -1, NULL, &g_SID_VS_LineRenderer));
	V_RETURN(AUTAddGShader(L"engine\\shaders\\LineRenderer.hlsl", "GS_LineRenderer", "GS_LineRenderer", NULL, &g_SID_GS_LineRenderer));
	V_RETURN(AUTAddPShader(L"engine\\shaders\\LineRenderer.hlsl", "PS_LineRenderer", "PS_LineRenderer", NULL, &g_SID_PS_LineRenderer));

	V_RETURN(AUTAddVShader(L"engine\\shaders\\CardRenderer.hlsl", "VS_CardRenderer", "VS_CardRenderer", -1, NULL, &g_SID_VS_CardRenderer));
	V_RETURN(AUTAddGShader(L"engine\\shaders\\CardRenderer.hlsl", "GS_CardRenderer", "GS_CardRenderer", NULL, &g_SID_GS_CardRenderer));
	V_RETURN(AUTAddPShader(L"engine\\shaders\\CardRenderer.hlsl", "PS_CardRenderer", "PS_CardRenderer", NULL, &g_SID_PS_CardRenderer));

	D3D_SHADER_MACRO def[2] = { { "FLIPPED", "1" }, { 0, 0 } };

	V_RETURN(AUTAddVShader(L"engine\\shaders\\CardRenderer.hlsl", "VS_CardRenderer", "VS_CardRendererFlipped", -1, NULL, &g_SID_VS_CardRendererFlipped));
	V_RETURN(AUTAddGShader(L"engine\\shaders\\CardRenderer.hlsl", "GS_CardRenderer", "GS_CardRendererFlipped", def, &g_SID_GS_CardRendererFlipped));
	V_RETURN(AUTAddPShader(L"engine\\shaders\\CardRenderer.hlsl", "PS_CardRenderer", "PS_CardRendererFlipped", NULL, &g_SID_PS_CardRendererFlipped));

	D3D11_RASTERIZER_DESC RastDesc;
	ZeroMemory(&RastDesc, sizeof(D3D11_RASTERIZER_DESC));
	RastDesc.AntialiasedLineEnable = true;
	RastDesc.MultisampleEnable = true;
	RastDesc.DepthBias = 0;
	RastDesc.DepthBiasClamp = 0.0f;
	RastDesc.ScissorEnable = false;
	RastDesc.DepthClipEnable = true;
	RastDesc.SlopeScaledDepthBias = 0.0f;
	RastDesc.FrontCounterClockwise = false;
	RastDesc.CullMode = D3D11_CULL_BACK;
	RastDesc.FillMode = D3D11_FILL_SOLID;
	V_RETURN(pD3DDevice->CreateRasterizerState(&RastDesc, &g_pRastState));

	// set the states of pipeline
	D3D11_BLEND_DESC bsDesc;
	ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
	bsDesc.AlphaToCoverageEnable = true;
	bsDesc.IndependentBlendEnable = true;

	for (int i = 0; i < 8; i++)
	{
		bsDesc.RenderTarget[i].BlendEnable = false;
		bsDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;

		bsDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[i].RenderTargetWriteMask = 	D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	V_RETURN(pD3DDevice->CreateBlendState(&bsDesc, &g_pBlendState));

	g_ScreenViewport.TopLeftX = 0.0f;
    g_ScreenViewport.TopLeftY = 0.0f;
    g_ScreenViewport.Width = (float)pSCDesc->BufferDesc.Width;
    g_ScreenViewport.Height = (float)pSCDesc->BufferDesc.Height;
    g_ScreenViewport.MinDepth = 0.0f;
    g_ScreenViewport.MaxDepth = 1.0f;

	D3D11_SAMPLER_DESC SampDesc;
	ZeroMemory(&SampDesc, sizeof(SampDesc));
	SampDesc.Filter = D3D11_FILTER_ANISOTROPIC; // D3D11_FILTER_MIN_MAG_MIP_POINT D3D11_FILTER_ANISOTROPIC
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER; // default - CLAMP
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SampDesc.MipLODBias = 0.0f;
	SampDesc.BorderColor[0] = SampDesc.BorderColor[1] = SampDesc.BorderColor[2] = 0.0f;
	SampDesc.BorderColor[3] = 1.0f;
	SampDesc.MaxAnisotropy = 16;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SampDesc.MinLOD = 0.0f;
	SampDesc.MaxLOD = 0.0f;
	V_RETURN(AUTCreateSampler(SampDesc, &g_SAMPID_ANISOTROPIC));
	AUTMapIDsPush("SamplerAnisotropic", g_SAMPID_ANISOTROPIC);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = pSCDesc->BufferDesc.Width;
	desc.Height = pSCDesc->BufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	V_RETURN(pD3DDevice->CreateTexture2D(&desc, NULL, &g_pBackBufferTexture));
	V_RETURN(pD3DDevice->CreateRenderTargetView(g_pBackBufferTexture, NULL, &g_pBackBufferRTV));
	V_RETURN(pD3DDevice->CreateShaderResourceView(g_pBackBufferTexture, NULL, &g_pBackBufferSRV));

	pImmediateContext->RSSetState(g_pRastState);
	pImmediateContext->RSSetViewports(1, &g_ScreenViewport);
	static float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xFFFFFFFF);

	g_bCreated = true;
	return S_OK;
}

HRESULT OnResetDevice()
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();
	ID3D11Device *pD3DDevice = AUTGetD3DDevice();

	DXGI_SWAP_CHAIN_DESC *pSCDesc;
	pSCDesc = AUTGetSwapChainParams();

	if( g_bCreated )
	{
		SAFE_RELEASE(g_pBackBufferTexture);
		SAFE_RELEASE(g_pBackBufferSRV);
		SAFE_RELEASE(g_pBackBufferRTV);
	}

	g_nwScreen = pSCDesc->BufferDesc.Width;
	g_nhScreen = pSCDesc->BufferDesc.Height;

	CCBPerBatch *cbPerBatch = AUTGetCBPerBatch();
	cbPerBatch->vStart = XMFLOAT2(200, 200);
	cbPerBatch->vScreenSize = XMFLOAT2(1.0f / g_nwScreen, 1.0f / g_nhScreen);
	//AUTSetCBPerBatch(cbPerBatch);

	AUTUpdateD3DCBPerBatch(pImmediateContext);
	/*
	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = pSCDesc->BufferDesc.Width;
	desc.Height = pSCDesc->BufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 8;
	desc.SampleDesc.Quality = 32;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	V_RETURN(pD3DDevice->CreateTexture2D(&desc, NULL, &g_pBackBufferTexture));
	V_RETURN(pD3DDevice->CreateRenderTargetView(g_pBackBufferTexture, NULL, &g_pBackBufferRTV));
	V_RETURN(pD3DDevice->CreateShaderResourceView(g_pBackBufferTexture, NULL, &g_pBackBufferSRV));
	*/
	g_ScreenViewport.Width = (float)pSCDesc->BufferDesc.Width;
    g_ScreenViewport.Height = (float)pSCDesc->BufferDesc.Height;

	return S_OK;
}

void OnRelease() 
{
	if( g_bCreated )
	{
		SAFE_RELEASE(g_pRastState);
		SAFE_RELEASE(g_pBlendState);

		SAFE_RELEASE(g_pBackBufferTexture);
		SAFE_RELEASE(g_pBackBufferSRV);
		SAFE_RELEASE(g_pBackBufferRTV);
	}

	DestroyDeck();
	//DestroyField();


	g_Suits[CARD_SUIT_CLUB].Clear();
	g_Suits[CARD_SUIT_DIAMOND].Clear();
	g_Suits[CARD_SUIT_HEART].Clear();
	g_Suits[CARD_SUIT_SPADE].Clear();

	g_ExitBut.Clear();
	g_NextMoveBut.Clear();
	g_TakeBut.Clear();

	AUTShadersDestroy();
}

bool OnInput( LPDIMOUSESTATE pDIMouseState, LPKEYBOARDSTATE pKeyboardState )
{
	static bool bTaking = false, bNextMove = false;
	if (KEY_PRESSED(pKeyboardState, DIK_ESCAPE))
	{
		AUTShutdown(0);
		return false;
	}

	static double
		fTime = g_Timer.GetElapsed() * 1000.0,
		fTimeToWait = 200.0,
		fTimeChangeRastState = 0.0,
		fTimeClickOnCard = 0.0;

	fTime = g_Timer.GetElapsed() * 1000.0;

	if (KEY_PRESSED(pKeyboardState, DIK_RETURN) && fTime - fTimeChangeRastState > fTimeToWait)
	{
		//bTaking = true;

		ProcessPlayerInput(XMFLOAT2(0.0f, 0.0f), true, false);

		//bTaking = false;

		fTimeChangeRastState = fTime;
	}

	if (KEY_PRESSED(pDIMouseState->rgbButtons, 0) && fTime - fTimeClickOnCard > fTimeToWait)
	{
		POINT lpCPos;
		GetCursorPos(&lpCPos);
		ScreenToClient(AUTGetHWND(), &lpCPos);

		XMFLOAT2 vPos = XMFLOAT2((float)lpCPos.x, (float)lpCPos.y);

		//bool 
		//	bNextMove = false,
		//	bTaking = false;

		XMFLOAT2 const *vStartPos = g_ExitBut.GetCardStartPos();
		XMFLOAT2 vEndPos = *vStartPos;
		XMStoreFloat2(&vEndPos, XMVectorAdd(XMLoadFloat2(&vEndPos), XMLoadFloat2(g_ExitBut.GetCardEndPos())));
		if (vStartPos->x <= vPos.x && vEndPos.x >= vPos.x && vStartPos->y <= vPos.y && vEndPos.y >= vPos.y)
		{
			AUTShutdown(0);
			return false;
		}

		/*
		vStartPos = g_TakeBut.GetCardStartPos();
		vEndPos = *vStartPos;
		XMStoreFloat2(&vEndPos, XMVectorAdd(XMLoadFloat2(&vEndPos), XMLoadFloat2(g_TakeBut.GetCardEndPos())));
		if (vStartPos->x <= vPos.x && vEndPos.x >= vPos.x && vStartPos->y <= vPos.y && vEndPos.y >= vPos.y)
		{
			bTaking = true;
		}

		vStartPos = g_NextMoveBut.GetCardStartPos();
		vEndPos = *vStartPos;
		XMStoreFloat2(&vEndPos, XMVectorAdd(XMLoadFloat2(&vEndPos), XMLoadFloat2(g_NextMoveBut.GetCardEndPos())));
		if (vStartPos->x <= vPos.x && vEndPos.x >= vPos.x && vStartPos->y <= vPos.y && vEndPos.y >= vPos.y)
		{
			bNextMove = true;
		}
		*/

		ProcessPlayerInput(vPos, false, false);
		//ProcessPlayerInput(vPos, bTaking, bNextMove);

		fTimeClickOnCard = fTime;
	}

	return true;
}

/*
	Function: DrawCard

	Draws one card.

	Parameters:
		pCard - pointer to drawing card.
		_vPos - position of card.
		_vEnd - size of card.
		_bFlipped - access to flip texture coordinates.

	Returns:
*/
void DrawCard(CCard const *const pCard, XMFLOAT2 const &_vPos, XMFLOAT2 const &_vEnd, bool const _bFlipped)
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();

	static ID3D11ShaderResourceView *pSRVNull[100] = { NULL };
	static ID3D11RenderTargetView *pRTVNull[100] = { NULL };

	static float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	if (_bFlipped)
	{
		AUTSetPShader(g_SID_PS_CardRendererFlipped, pImmediateContext);
		AUTSetGShader(g_SID_GS_CardRendererFlipped, pImmediateContext);
		AUTSetVShader(g_SID_VS_CardRendererFlipped, pImmediateContext);
	} else {
		AUTSetPShader(g_SID_PS_CardRenderer, pImmediateContext);
		AUTSetGShader(g_SID_GS_CardRenderer, pImmediateContext);
		AUTSetVShader(g_SID_VS_CardRenderer, pImmediateContext);
	}

	pImmediateContext->RSSetState(g_pRastState);
	pImmediateContext->RSSetViewports(1, &g_ScreenViewport);
	pImmediateContext->OMSetBlendState(g_pBlendState, BlendFactor, 0xFFFFFFFF);
	AUTSetPSSampler(g_SAMPID_ANISOTROPIC, 0, pImmediateContext);

	pImmediateContext->IASetInputLayout(NULL);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	CCBPerBatch *cbPerBatch = AUTGetCBPerBatch();
	cbPerBatch->vStart = _vPos;
	cbPerBatch->vEnd = _vEnd;
	//AUTSetCBPerBatch(cbPerBatch);
	AUTUpdateD3DCBPerBatch(pImmediateContext);

	ID3D11ShaderResourceView *pSRV = pCard->GetSRV();
	pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
	pImmediateContext->Draw(1, 0);
	pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);

	pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);
	pImmediateContext->OMSetRenderTargets(1, pRTVNull, NULL);
	pImmediateContext->PSSetShader(NULL, NULL, 0);
	pImmediateContext->GSSetShader(NULL, NULL, 0);
	pImmediateContext->VSSetShader(NULL, NULL, 0);
}

void OnFrameProcess(ID3D11Device *pD3DDevice, ID3D11Texture2D *pDSTexture, ID3D11DepthStencilView *pDSView, double fTime, double fElapsedTime)
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();

	double fCurrentTime = g_Timer.GetElapsed() * 1000.0f;

	static ID3D11ShaderResourceView *pSRVNull[100] = { NULL };
	static ID3D11RenderTargetView *pRTVNull[100] = { NULL };

	static float ClearColor[4] = { 0.2f, 0.5f, 0.2f, 0.0f };
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);

	static float fAngle = 90.0f;

	static double fLastTurn = 0.0f;

	ProcessAndRender();

	CCard const *const pTrump = GetTrump();
		
	DrawCard(&g_Suits[pTrump->GetCardSuit()], XMFLOAT2(700.0f, 30.0f), XMFLOAT2(50.0f, 50.0f), false);

	if (GetNumFreeCards())
		DrawCard(pTrump, XMFLOAT2(35.0f, 210.0f), XMFLOAT2(CARD_SIZE.y, CARD_SIZE.x), true);

	for (int i = 0; i < GetNumFreeCards() - 1; i++)
		DrawCard(GetBackCard(), XMFLOAT2(45.0f - i * 2.0f, 180.0f - i * 1.0f), CARD_SIZE, false);


	DrawCard(&g_ExitBut, *g_ExitBut.GetCardStartPos(), *g_ExitBut.GetCardEndPos(), false);

	if (CanAppShowNextMoveButton())
		DrawCard(&g_NextMoveBut, *g_NextMoveBut.GetCardStartPos(), *g_NextMoveBut.GetCardEndPos(), false);

	if (CanAppShowTakeButton())
		DrawCard(&g_TakeBut, *g_TakeBut.GetCardStartPos(), *g_TakeBut.GetCardEndPos(), false);

	static ULONGLONG uuCountAverRender = 0;
	static double fTimeWaitFrom = fCurrentTime;

	uuCountAverRender++;

	if (fCurrentTime - fTimeWaitFrom > 300.0f)
	{
		double fValueToScreen = (fCurrentTime - fTimeWaitFrom) / uuCountAverRender;

		uuCountAverRender = 0;
		fTimeWaitFrom = fCurrentTime;

		wchar_t sw[256];
		swprintf(sw, L"Card game: Durak. Render time: %0.3f ms", fValueToScreen);
		SetWindowText(AUTGetHWND(), sw);
	}
}