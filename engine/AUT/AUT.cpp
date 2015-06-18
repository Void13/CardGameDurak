#include "AUT.h"

#define SET_ACCESSOR( x, y )		inline void Set##y( x t )	{ AUTLock l; c_##y = t; };
#define GET_ACCESSOR( x, y )		inline x Get##y()			{ AUTLock l; return c_##y; };
#define GET_SET_ACCESSOR( x, y )	SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

#define SETP_ACCESSOR( x, y )      inline void Set##y( x* t )  { AUTLock l; c_##y = *t; };
#define GETP_ACCESSOR( x, y )      inline x* Get##y()          { AUTLock l; return &c_##y; };
#define GETP_SETP_ACCESSOR( x, y ) SETP_ACCESSOR( x, y ) GETP_ACCESSOR( x, y )


CCamera *pLightCamera;

map<string, int>					mapIDs;

void AUTMapIDsPush(string str, int dwID)
{
	mapIDs.insert(make_pair(str, dwID));
}

int AUTMapIDsGet(string str)
{
	return mapIDs.find(str)->second;
}

class AUT;

CRITICAL_SECTION					g_cs;
bool								g_bThreadSafe = true;
AUT									*g_pAUT = NULL;

class AUTLock
{
public:
	inline AUTLock()
	{
		assert(g_pAUT);
		if( g_bThreadSafe ) EnterCriticalSection( &g_cs );
	}
	inline ~AUTLock()
	{
		assert(g_pAUT);
		if( g_bThreadSafe ) LeaveCriticalSection( &g_cs );
	}
};

class AUT
{
protected:
	LPWNDCLASS						c_WndClass;
	HWND							c_HWND;
	HINSTANCE						c_HINSTANCE;
	bool							c_BWindowCreated;
	bool							c_DeviceLost;
	bool							c_Active;
	bool							c_Closed;
	bool							c_InputCreated;
	bool							c_RenderingPaused;
	bool							c_TimePaused;
	bool							c_ZOnlyPass;

	// Timer
	double c_Time;                      // current time in seconds
	double c_AbsoluteTime;              // absolute time in seconds
	float c_ElapsedTime;                // time elapsed since last frame
	// Present
	float							c_WaitingPresentTime;
	bool							c_WaitingPresent;
	// Stats
	bool c_NoStats;
	double c_LastStatsUpdateTime;		// last time the stats were updated
	DWORD c_LastStatsUpdateFrames;		// frames count since last time the stats were updated
	float c_FPS;						// frames per second
	int c_CurrentFrameNumber;			// the current frame number
	bool c_ConstantFrameTime;			// if true, then elapsed frame time will always be 0.05f seconds which is good for debugging or automated capture
	float c_TimePerFrame;				// the constant time per frame in seconds, only valid if m_ConstantFrameTime==true

	// Direct3D Interfaces

	D3D_DRIVER_TYPE					*c_DriverType;
	D3D_FEATURE_LEVEL				*c_FeautureLevel;
	ID3D11Device					*c_D3DDevice;
	ID3D11DeviceContext				*c_ImmediateContext;
	IDXGISwapChain					*c_SwapChain;
	ID3D11RenderTargetView			*c_RenderTargetView;
	ID3D11Texture2D					*c_DSTexture;
	ID3D11DepthStencilView			*c_DSView;
	DXGI_SWAP_CHAIN_DESC			*c_SwapChainParams;

	// Inputs
	LPDIRECTINPUT8					c_DI;
	LPDIRECTINPUTDEVICE8			c_KeyboardDevice;
	LPDIRECTINPUTDEVICE8			c_MouseDevice;
	LPKEYBOARDSTATE					c_KeysState;
	LPDIMOUSESTATE					c_DIMouseState;
	// Callbacks / Events
	PCALLBACKONINPUT				c_CallbackOnInput;
	PCALLBACKONFRAMEPROCESS			c_CallbackOnFrameProcess;
	PCALLBACKONCREATEDEVICE			c_CallbackOnCreateDevice;
	PCALLBACKONRESETDEVICE			c_CallbackOnResetDevice;
	PCALLBACKONRELEASE				c_CallbackOnRelease;
	PCALLBACKONWNDPROC				c_CallbackOnWndProc;
public:
	AUT()
	{
		ZeroMemory( this, sizeof( AUT ) );
		InitializeCriticalSection( &g_cs );
	}
	~AUT(){
		DeleteCriticalSection( &g_cs );
	}
// Accessors to members of AUT

	GET_SET_ACCESSOR( D3D_DRIVER_TYPE *, DriverType );
    GET_SET_ACCESSOR( D3D_FEATURE_LEVEL *, FeautureLevel );
	GET_SET_ACCESSOR( ID3D11Device *, D3DDevice );
    GET_SET_ACCESSOR( ID3D11DeviceContext *, ImmediateContext );
	GET_SET_ACCESSOR( IDXGISwapChain *, SwapChain );
    GET_SET_ACCESSOR( ID3D11RenderTargetView *, RenderTargetView );
	GET_SET_ACCESSOR( DXGI_SWAP_CHAIN_DESC *, SwapChainParams );
	GET_SET_ACCESSOR( ID3D11Texture2D *, DSTexture );
	GET_SET_ACCESSOR( ID3D11DepthStencilView *, DSView );

	GET_SET_ACCESSOR( LPWNDCLASS, WndClass );
	GET_SET_ACCESSOR( HWND, HWND );
	GET_SET_ACCESSOR( HINSTANCE, HINSTANCE );
	GET_SET_ACCESSOR( bool, BWindowCreated );
	GET_SET_ACCESSOR( bool, DeviceLost );
	GET_SET_ACCESSOR( bool, Active );
	GET_SET_ACCESSOR( bool, Closed );
	GET_SET_ACCESSOR( bool, InputCreated );
	GET_SET_ACCESSOR( bool, RenderingPaused );
	GET_SET_ACCESSOR( bool, TimePaused );
	GET_SET_ACCESSOR( bool, ZOnlyPass );

	// Input
	GET_SET_ACCESSOR( LPDIRECTINPUT8, DI );
	GET_SET_ACCESSOR( LPDIRECTINPUTDEVICE8, KeyboardDevice );
	GET_SET_ACCESSOR( LPDIRECTINPUTDEVICE8, MouseDevice );
	GET_SET_ACCESSOR( LPDIMOUSESTATE, DIMouseState );
	GET_SET_ACCESSOR( LPKEYBOARDSTATE, KeysState );

	// Callbacks / Events
	GET_SET_ACCESSOR( PCALLBACKONINPUT, CallbackOnInput );
	GET_SET_ACCESSOR( PCALLBACKONFRAMEPROCESS, CallbackOnFrameProcess );
	GET_SET_ACCESSOR( PCALLBACKONCREATEDEVICE, CallbackOnCreateDevice );
	GET_SET_ACCESSOR( PCALLBACKONRESETDEVICE, CallbackOnResetDevice );
	GET_SET_ACCESSOR( PCALLBACKONRELEASE, CallbackOnRelease );
	GET_SET_ACCESSOR( PCALLBACKONWNDPROC, CallbackOnWndProc );

	// Timer
	GET_SET_ACCESSOR( double, Time );
	GET_SET_ACCESSOR( double, AbsoluteTime );
	GET_SET_ACCESSOR( float, ElapsedTime );
	// Present
	GET_SET_ACCESSOR( bool, WaitingPresent );
	GET_SET_ACCESSOR( float, WaitingPresentTime );

	// Stats
	GET_SET_ACCESSOR( double, LastStatsUpdateTime );   
	GET_SET_ACCESSOR( DWORD, LastStatsUpdateFrames );   
	GET_SET_ACCESSOR( float, FPS );    
	GET_SET_ACCESSOR( int, CurrentFrameNumber );
	GET_SET_ACCESSOR( bool, NoStats );
	GET_SET_ACCESSOR( bool, ConstantFrameTime );
    GET_SET_ACCESSOR( float, TimePerFrame );
};

// Forward declarations
bool AUTIsActive();
HRESULT EnvReset();

// -----------------------------------
// Functions are not accessed to user
// -----------------------------------
inline AUT &GetAUT()
{
	assert( g_pAUT );
	return *g_pAUT;
}

LRESULT WINAPI AUTWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	PCALLBACKONWNDPROC pOnWndProc = GetAUT().GetCallbackOnWndProc();
	DXGI_SWAP_CHAIN_DESC *pDesc = GetAUT().GetSwapChainParams();
    if( pOnWndProc )
    {
        bool bNoFurtherProcessing = false;
        LRESULT nResult = pOnWndProc( hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing );
        if( bNoFurtherProcessing )
            return nResult;
    }
	switch( uMsg )
	{
		case WM_ACTIVATEAPP:
			if( wParam == TRUE && !AUTIsActive() ) // Handle only if previously not active 
			{
				GetAUT().SetActive( true );
				AUTPause( false, false );

			}
			else if( wParam == FALSE && AUTIsActive() ) // Handle only if previously active 
			{
				GetAUT().SetActive( false );
				AUTPause( true, true );
			}
			break;
		case WM_ENTERSIZEMOVE:
			{
				RECT rct;
				GetClientRect(AUTGetHWND(), &rct );
				static int	nWidth = 0,
							nHeight = 0;
				nWidth = rct.right - rct.left;
				nHeight = rct.bottom - rct.top;

				AUTPause( true, true );
				break;
			}
			
		case WM_EXITSIZEMOVE:
			AUTPause( false, false );
			if( pDesc->Windowed )
			{
				RECT rct;
				GetClientRect(AUTGetHWND(), &rct );

				if( rct.right != pDesc->BufferDesc.Width || rct.bottom != pDesc->BufferDesc.Height )
				{
					pDesc->BufferDesc.Width = rct.right;
					pDesc->BufferDesc.Height = rct.bottom;

					EnvReset();
				}
			}
			break;
		case WM_SIZE:
			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED || wParam == SIZE_RESTORED)
			{
				AUTPause( false, false );
				if( pDesc->Windowed )
				{
					RECT rct;
					GetClientRect(AUTGetHWND(), &rct );

					if( rct.right != pDesc->BufferDesc.Width || rct.bottom != pDesc->BufferDesc.Height )
					{
						pDesc->BufferDesc.Width = rct.right;
						pDesc->BufferDesc.Height = rct.bottom;

						EnvReset();
					}
				}
			}
			break;
			
		case WM_CLOSE:
			GetAUT().SetClosed( true );
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//--------------------------------------------------------------------------------------
// Updates the frames/sec stat once per second
//--------------------------------------------------------------------------------------
void AUTUpdateFrameStats()
{
	if( GetAUT().GetNoStats() )
		return;

	// Keep track of the frame count
	double fLastTime = GetAUT().GetLastStatsUpdateTime();
	DWORD dwFrames = GetAUT().GetLastStatsUpdateFrames();
	double fAbsTime = GetAUT().GetAbsoluteTime();
	dwFrames++;
	GetAUT().SetLastStatsUpdateFrames( dwFrames );

	// Update the scene stats once per second
	if( fAbsTime - fLastTime > 1.0f )
	{
		float fFPS = ( float )( dwFrames / ( fAbsTime - fLastTime ) );
		GetAUT().SetFPS( fFPS );
		GetAUT().SetLastStatsUpdateTime( fAbsTime );
		GetAUT().SetLastStatsUpdateFrames( 0 );
	}
}

//--------------------------------------------------------------------------------------
// Internal helper function to handle calling the user defined timer callbacks
//--------------------------------------------------------------------------------------
void AUTHandleTimers()
{
    /*float fElapsedTime = AUTGetElapsedTime();

    CGrowableArray <DXUT_TIMER>* pTimerList = GetAUT().GetTimerList();
    if( pTimerList == NULL )
        return;

    // Walk through the list of timer callbacks
    for( int i = 0; i < pTimerList->GetSize(); i++ )
    {
        DXUT_TIMER DXUTTimer = pTimerList->GetAt( i );
        if( DXUTTimer.bEnabled )
        {
            DXUTTimer.fCountdown -= fElapsedTime;

            // Call the callback if count down expired
            if( DXUTTimer.fCountdown < 0 )
            {
                DXUTTimer.pCallbackTimer( DXUTTimer.nID, DXUTTimer.pCallbackUserContext );
                // The callback my have changed the timer.
                DXUTTimer = pTimerList->GetAt( i );
                DXUTTimer.fCountdown = DXUTTimer.fTimeoutInSecs;
            }
            pTimerList->SetAt( i, DXUTTimer );
        }
    }*/
}

// ------------------------------------------------
// Functions for manage AUT and accessed to user
// ------------------------------------------------
void CreateAUT()
{
	assert( !g_pAUT );
	g_pAUT = new AUT;
}

// Set callbacks
void AUTSetCallbackOnCreateDevice( PCALLBACKONCREATEDEVICE pCallback )		{ GetAUT().SetCallbackOnCreateDevice( pCallback ); }
void AUTSetCallbackOnResetDevice( PCALLBACKONRESETDEVICE pCallback )		{ GetAUT().SetCallbackOnResetDevice( pCallback ); }
void AUTSetCallbackOnRelease( PCALLBACKONRELEASE pCallback )				{ GetAUT().SetCallbackOnRelease( pCallback ); }
void AUTSetCallbackOnFrameProcess( PCALLBACKONFRAMEPROCESS pCallback )		{ GetAUT().SetCallbackOnFrameProcess( pCallback ); }
void AUTSetCallbackOnInput( PCALLBACKONINPUT pCallback )					{ GetAUT().SetCallbackOnInput( pCallback ); }
void AUTSetCallbackOnWndProc( PCALLBACKONWNDPROC pCallback )				{ GetAUT().SetCallbackOnWndProc( pCallback ); }

bool AUTIsActive()									{ return GetAUT().GetActive(); }
bool AUTIsWindowCreated()							{ return GetAUT().GetBWindowCreated(); }

D3D_DRIVER_TYPE *AUTGetDriverType()					{ return GetAUT().GetDriverType(); }
D3D_FEATURE_LEVEL *AUTGetFeautureLevel()			{ return GetAUT().GetFeautureLevel(); }
ID3D11Device *AUTGetD3DDevice()						{ return GetAUT().GetD3DDevice(); }
ID3D11DeviceContext *AUTGetImmediateContext()		{ return GetAUT().GetImmediateContext(); }
IDXGISwapChain *AUTGetSwapChain()					{ return GetAUT().GetSwapChain(); }
ID3D11RenderTargetView *AUTGetRenderTargetView()	{ return GetAUT().GetRenderTargetView(); }
ID3D11Texture2D *AUTGetDSTexture()					{ return GetAUT().GetDSTexture(); }
ID3D11DepthStencilView *AUTGetDSView()				{ return GetAUT().GetDSView(); }

bool AUTGetZOnlyPass()								{ return GetAUT().GetZOnlyPass(); }
void AUTSetZOnlyPass(bool b)						{ GetAUT().SetZOnlyPass(b); }
// Noob version
CCamera *AUTGetLight()								{ return pLightCamera; }
void AUTSetLight(CCamera *pCamera)					{ pLightCamera = pCamera; }

void AUTSetSwapChainParams( DXGI_SWAP_CHAIN_DESC *pSets )		{ GetAUT().SetSwapChainParams( pSets ); }
DXGI_SWAP_CHAIN_DESC *AUTGetSwapChainParams()					{ return GetAUT().GetSwapChainParams(); }

HWND AUTGetHWND()									{ return GetAUT().GetHWND(); }
HINSTANCE AUTGetHINSTANCE()							{ return GetAUT().GetHINSTANCE(); }
// Get stats
double AUTGetTime()									{ return GetAUT().GetTime(); }
float AUTGetElapsedTime()							{ return GetAUT().GetElapsedTime(); }
float AUTGetFPS()									{ return GetAUT().GetFPS(); }
LPDIRECTINPUT8 AUTGetDI()							{ return GetAUT().GetDI(); }
LPDIRECTINPUTDEVICE8 AUTGetKeyboardDevice()			{ return GetAUT().GetKeyboardDevice(); }
LPDIRECTINPUTDEVICE8 AUTGetMouseDevice()			{ return GetAUT().GetMouseDevice(); }
LPKEYBOARDSTATE AUTGetKeysState()					{ return GetAUT().GetKeysState(); }
LPDIMOUSESTATE AUTGetDIMouseState()					{ return GetAUT().GetDIMouseState(); }
void SetWaitingPresent( bool bFlag )				{ GetAUT().SetWaitingPresent( bFlag ); }

void AUTPause( bool bPauseTime, bool bPauseRendering )
{
	if( bPauseTime )
	{
		// Stop the scene from animating
		DXUTGetGlobalTimer()->Stop();
	}
	else
	{
		// Restart the timer
		DXUTGetGlobalTimer()->Start();
	}

	GetAUT().SetRenderingPaused( bPauseRendering );
	GetAUT().SetTimePaused( bPauseTime );
}

HRESULT AUTCreateWindow( const wchar_t *pStrWindowTitle, DWORD dwColor,
						   int nX, int nY, int nWidth, int nHeight,
						   HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, HMENU hMenu )
{
	GetAUT().SetHINSTANCE( hInstance );
	LPWNDCLASS pWndClass= new WNDCLASS();
	GetAUT().SetWndClass( pWndClass );
	if( AUTIsWindowCreated() )
	{
		return S_OK;
	}
	ZeroMemory( pWndClass, sizeof( WNDCLASS ) );
	pWndClass->hbrBackground = CreateSolidBrush( dwColor );
	pWndClass->hCursor = hCursor;
	pWndClass->hIcon = hIcon;
	pWndClass->hInstance = hInstance;
	pWndClass->lpfnWndProc = &AUTWndProc;
	pWndClass->lpszClassName = L"AUTWindowClass";
	pWndClass->style = CS_DBLCLKS;
	if( !RegisterClass( pWndClass ) ) 
		return E_FAIL;
	HWND hWnd;
	hWnd = CreateWindow( L"AUTWindowClass", pStrWindowTitle, 0, nX, nY, nWidth, nHeight, NULL, hMenu, hInstance, NULL);
	if( !hWnd )
		return E_FAIL;
	GetAUT().SetHWND( hWnd );
	GetAUT().SetBWindowCreated( true );
	return S_OK;
}

HRESULT InitInputs()
{
	HRESULT hr;

	LPDIRECTINPUT8			pDI = NULL;
	LPDIRECTINPUTDEVICE8	pMouseDevice = NULL;
	LPDIRECTINPUTDEVICE8	pKeyboardDevice = NULL;
	LPKEYBOARDSTATE			pKeysState;
	LPDIMOUSESTATE			pDIMouseState;

	if( !GetAUT().GetInputCreated() )
	{
		V_JMP( DirectInput8Create( AUTGetHINSTANCE(), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&pDI, NULL ), CLEAR );
		V_JMP( pDI->CreateDevice( GUID_SysKeyboard, &pKeyboardDevice, NULL ), CLEAR );
		pKeyboardDevice->SetDataFormat( &c_dfDIKeyboard );
		pKeyboardDevice->SetCooperativeLevel( AUTGetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		pKeyboardDevice->Acquire();
		V_JMP( pDI->CreateDevice( GUID_SysMouse, &pMouseDevice, NULL ), CLEAR );
		pMouseDevice->SetDataFormat( &c_dfDIMouse );
		pMouseDevice->SetCooperativeLevel( AUTGetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		pMouseDevice->Acquire();
		
		pKeysState = new KEYBOARDSTATE;
		if( !pKeysState )
		{
			hr = E_OUTOFMEMORY;
			goto CLEAR;
		}
		ZeroMemory( pKeysState, sizeof( KEYBOARDSTATE ) );
		pDIMouseState = new DIMOUSESTATE;
		if( !pDIMouseState )
		{
			delete[] pKeysState;
			hr = E_OUTOFMEMORY;
			goto CLEAR;
		}
		ZeroMemory( pDIMouseState, sizeof( DIMOUSESTATE ) );

		GetAUT().SetDI( pDI );
		GetAUT().SetKeyboardDevice( pKeyboardDevice );
		GetAUT().SetMouseDevice( pMouseDevice );
		GetAUT().SetKeysState( pKeysState );
		GetAUT().SetDIMouseState( pDIMouseState );
		GetAUT().SetInputCreated( true );
		return S_OK;

CLEAR:
		SAFE_RELEASE( pDI );
		SAFE_RELEASE( pKeyboardDevice );
		SAFE_RELEASE( pMouseDevice );

		return hr;
	}

	return S_OK;
}

HRESULT EnvCreate()
{
	HRESULT hr;

	if( !AUTIsWindowCreated() )
	{
		return E_FAIL;
	}
	
	D3D_FEATURE_LEVEL feautureLevels[] = {
		D3D_FEATURE_LEVEL_11_0
	};
	IDXGISwapChain *pSwapChain;
	ID3D11Device *pD3DDevice;
	ID3D11DeviceContext *pImmediateContext;
	DXGI_SWAP_CHAIN_DESC *pSwapChainParams = GetAUT().GetSwapChainParams();
	D3D_FEATURE_LEVEL *pD3DFeautureLevel = NULL;

	V_RETURN(D3D11CreateDeviceAndSwapChain ( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, feautureLevels, ARRAYSIZE(feautureLevels),
										 D3D11_SDK_VERSION, pSwapChainParams, &pSwapChain, &pD3DDevice, pD3DFeautureLevel, &pImmediateContext));

	//pSwapChain->ResizeBuffers(1, NULL, NULL, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);

	ID3D11Texture2D* pBackBuffer = NULL;
	V_RETURN(pSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer ));

	ID3D11RenderTargetView *pView = 0;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	RTVDesc.Format = AUTGetSwapChainParams()->BufferDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	RTVDesc.Texture2D.MipSlice = 0;

	V_RETURN(pD3DDevice->CreateRenderTargetView( pBackBuffer, &RTVDesc, &pView ));
	SAFE_RELEASE(pBackBuffer);

	ID3D11Texture2D *pDSTexture = NULL;
	ID3D11DepthStencilView *pDSView = NULL;

	D3D11_TEXTURE2D_DESC TexDesc;
	// Creating depth stencil view
	//		need move to AUT.h
	ZeroMemory(&TexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	TexDesc.Width = pSwapChainParams->BufferDesc.Width;
	TexDesc.Height = pSwapChainParams->BufferDesc.Height;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;
	V_RETURN(pD3DDevice->CreateTexture2D(&TexDesc, NULL, &pDSTexture));
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DSVDesc.Format = TexDesc.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	V_RETURN(pD3DDevice->CreateDepthStencilView( pDSTexture, &DSVDesc, &pDSView));

	GetAUT().SetRenderTargetView(pView);
	GetAUT().SetFeautureLevel(pD3DFeautureLevel);
	GetAUT().SetD3DDevice(pD3DDevice);
	GetAUT().SetSwapChain(pSwapChain);
	GetAUT().SetImmediateContext(pImmediateContext);
	GetAUT().SetDSTexture(pDSTexture);
	GetAUT().SetDSView(pDSView);

	PCALLBACKONCREATEDEVICE pOnCreateDevice = GetAUT().GetCallbackOnCreateDevice();
	if( pOnCreateDevice ){
		V_RETURN(pOnCreateDevice());
	}

	PCALLBACKONCREATEDEVICE pOnResetDevice = GetAUT().GetCallbackOnResetDevice();
	if( pOnResetDevice ){
		V_RETURN(pOnResetDevice());
	}

	if( FAILED( hr = InitInputs() ) )
	{
		return AUTERR( L"CreateInputs", hr );
	}

	ShowWindow( AUTGetHWND(), true );
	UpdateWindow( AUTGetHWND() );
	DXUTGetGlobalTimer()->Reset();
	DXUTGetGlobalTimer()->Start();

	return S_OK;
}

HRESULT EnvReset()
{
	HRESULT hr;

	ID3D11Device *pD3DDevice = AUTGetD3DDevice();
	AUTPause( true, true );
	if( pD3DDevice )
	{
		DXGI_SWAP_CHAIN_DESC *pSwapChainParams = GetAUT().GetSwapChainParams();

		ID3D11RenderTargetView *pRTView = GetAUT().GetRenderTargetView();
		SAFE_RELEASE(pRTView);

		// AUTGetSwapChainParams()->BufferDesc.Format DXGI_FORMAT_UNKNOWN
		V_RETURN(AUTGetSwapChain()->ResizeBuffers(1, pSwapChainParams->BufferDesc.Width, pSwapChainParams->BufferDesc.Height, AUTGetSwapChainParams()->BufferDesc.Format, NULL));

		ID3D11Texture2D* pBackBuffer = NULL;
		V_RETURN(AUTGetSwapChain()->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

		D3D11_TEXTURE2D_DESC desc;
		pBackBuffer->GetDesc(&desc);

		ID3D11RenderTargetView *pView = 0;

		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		RTVDesc.Format = AUTGetSwapChainParams()->BufferDesc.Format;
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		RTVDesc.Texture2D.MipSlice = 0;

		V_RETURN(pD3DDevice->CreateRenderTargetView( pBackBuffer, &RTVDesc, &pView ));

		GetAUT().SetRenderTargetView(pView);
		SAFE_RELEASE(pBackBuffer);

		PCALLBACKONCREATEDEVICE pOnResetDevice = GetAUT().GetCallbackOnResetDevice();
		if( pOnResetDevice )
		{
			V_RETURN(pOnResetDevice());
		}

		ID3D11Texture2D *pTex = GetAUT().GetDSTexture();

		SAFE_RELEASE(pTex);
		
		ID3D11Texture2D *pDSTexture = NULL;
		ID3D11DepthStencilView *pDSView = NULL;

		D3D11_TEXTURE2D_DESC TexDesc;
		// Creating depth stencil view
		//		need move to AUT.h
		ZeroMemory(&TexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		TexDesc.Width = pSwapChainParams->BufferDesc.Width;
		TexDesc.Height = pSwapChainParams->BufferDesc.Height;
		TexDesc.MipLevels = 1;
		TexDesc.ArraySize = 1;
		TexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		TexDesc.SampleDesc.Count = 1;
		TexDesc.SampleDesc.Quality = 0;
		TexDesc.Usage = D3D11_USAGE_DEFAULT;
		TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		TexDesc.CPUAccessFlags = 0;
		TexDesc.MiscFlags = 0;
		V_RETURN(pD3DDevice->CreateTexture2D(&TexDesc, NULL, &pDSTexture));
		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		DSVDesc.Format = TexDesc.Format;
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		V_RETURN(pD3DDevice->CreateDepthStencilView( pDSTexture, &DSVDesc, &pDSView));

		GetAUT().SetDSTexture(pDSTexture);
		GetAUT().SetDSView(pDSView);
		
		GetAUT().SetDeviceLost( false );
		AUTPause( false, false );
	}
	else
	{
		hr = EnvCreate();
		AUTPause( false, false );
		return hr;
	}

	return S_OK;
}

HRESULT AUTCreateDevice()
{
	if( !AUTGetD3DDevice() ){
		return EnvCreate();
	}

	return S_OK;
}

void AUTRender()
{
	HRESULT hr;

	if( GetAUT().GetDeviceLost() || !GetAUT().GetActive() || GetAUT().GetRenderingPaused() )
	{
		Sleep( 50 );
	}
	
	ID3D11Device *pD3DDevice = AUTGetD3DDevice();
	IDXGISwapChain *pSwapChain = GetAUT().GetSwapChain();
	
	double fAbsTime, fTime;
	float fElapsedTime;
	DXUTGetGlobalTimer()->GetTimeValues( &fTime, &fAbsTime, &fElapsedTime );
	  // Store the time for the app
    if( GetAUT().GetConstantFrameTime() )
    {
        fElapsedTime = GetAUT().GetTimePerFrame();
        fTime = AUTGetTime() + fElapsedTime;
    }

	GetAUT().SetTime( fTime );
    GetAUT().SetAbsoluteTime( fAbsTime );
    GetAUT().SetElapsedTime( fElapsedTime );

    // Update the FPS stats
    AUTUpdateFrameStats();
	// Call callbacks functions of timers
	AUTHandleTimers();

	PCALLBACKONFRAMEPROCESS pOnFrameProcess = GetAUT().GetCallbackOnFrameProcess();
	if( pOnFrameProcess )
	{
		pOnFrameProcess( pD3DDevice, GetAUT().GetDSTexture(), GetAUT().GetDSView(), fTime, fElapsedTime );
	}

	if ( GetAUT().GetWaitingPresent() )
		hr = pSwapChain->Present( 1, 0 ); // first parameter - VSYNC - ON!!!!!
	else
		hr = pSwapChain->Present( 0, 0 ); // first parameter - VSYNC - OFF!!!!!

	if( FAILED(hr) )
	{
		if( hr == DXGI_ERROR_INVALID_CALL )
		{
			AUTPause( true, true );
			GetAUT().SetDeviceLost( true );
		}
	}
}

int AUTMainLoop()
{
	MSG Msg = { 0 };
	while( Msg.message != WM_QUIT)
	{
		if( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &Msg );
			DispatchMessage( &Msg );
		}
		else if ( !GetAUT().GetClosed() )
		{
			LPDIRECTINPUTDEVICE8 pKeyboardDevice = AUTGetKeyboardDevice(), pMouseDevice = AUTGetMouseDevice();
			LPDIMOUSESTATE pDIMouseState = AUTGetDIMouseState();
			LPKEYBOARDSTATE pKeysState = AUTGetKeysState();
			
			if( FAILED( pKeyboardDevice->GetDeviceState( sizeof( KEYBOARDSTATE ), pKeysState ) ) )
			{
				pKeyboardDevice->Acquire();
			}

			if( FAILED( pMouseDevice->GetDeviceState( sizeof( DIMOUSESTATE ), pDIMouseState ) ) )
			{
				pMouseDevice->Acquire();
			}

			PCALLBACKONINPUT pOnInput = GetAUT().GetCallbackOnInput();
			if( pOnInput )
			{
				if (!pOnInput( pDIMouseState, pKeysState ))
					return Msg.wParam;
			}
			AUTRender();
		}
	}
	return Msg.wParam;
}

void AUTRelease()
{
	//LPDIMOUSESTATE pDIMouseState = AUTGetDIMouseState();
	//LPKEYBOARDSTATE pKeysState = AUTGetKeysState();
	delete[] AUTGetKeysState();
	delete[] AUTGetDIMouseState();

	PCALLBACKONRELEASE pOnRelease = GetAUT().GetCallbackOnRelease();
	if( pOnRelease )
	{
		pOnRelease();
	}

	SAFE_RELEASEWO(AUTGetKeyboardDevice());
	SAFE_RELEASEWO(AUTGetMouseDevice());
	if( AUTGetImmediateContext() )
		AUTGetImmediateContext()->ClearState();
	SAFE_RELEASEWO(AUTGetRenderTargetView());
	SAFE_RELEASEWO(AUTGetSwapChain());
	SAFE_RELEASEWO(AUTGetImmediateContext());
	SAFE_RELEASEWO(AUTGetD3DDevice());
	SAFE_RELEASEWO(AUTGetDSView());
	SAFE_RELEASEWO(AUTGetDSTexture());

	GetAUT().SetKeyboardDevice( NULL );
	GetAUT().SetMouseDevice( NULL );
	GetAUT().SetD3DDevice( NULL );
	GetAUT().SetDeviceLost( true );
	delete GetAUT().GetWndClass();
	delete g_pAUT;
}

void AUTShutdown( int nExitCode )
{
	// some code
	PostQuitMessage( nExitCode );
	SendMessage(GetAUT().GetHWND(), WM_CLOSE, NULL, NULL);
}