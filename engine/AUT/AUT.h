
#ifndef H__AUT
#define H__AUT

#pragma warning( disable : 4996 )
#pragma warning( disable : 4533 )


#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

#include <Windows.h>
#include <wchar.h>
#include <assert.h>
#include <memory.h>

//#define _XM_NO_INTRINSICS_ 1

// D3D Includes:
#include <d3d11.h>
#include <dinput.h>
#include <dxerr.h>
#include <dxgi.h>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <DirectXSH.h>

#include <D3Dcompiler.h>

#include <D3DX11.h>

// DXUTtimer, ...

#include "DXUTmisc.h"
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <set>

using namespace DirectX;
using namespace std;

// Forward declaration
class CCamera;

#define BEGIN_ALIGNED_DATA
#define END_ALIGNED_DATA
#define ALIGN16 __declspec(align(16))
#ifndef INOUT
#	define INOUT
#endif

#define fzero(x, eps) fabs(x) < eps
#define fequ(x, y, eps) fabs(x - y) < eps
#define fless(x, y, eps) x < y - eps
#define flessequ(x, y, eps) x < y + eps
#define fgreater(x, y, eps) x > y + eps
#define fgreaterequ(x, y, eps) x < y - eps

int AUTMapIDsGet(string str);
void AUTMapIDsPush(string str, int dwID);

void CreateAUT();

#define MESHID_NONE	((DWORD)-1)

#define TEXID_TEXTURE			0
#define TEXID_BUMPMAP			1
#define TEXID_NORMALMAP			2
#define TEXID_DISPLACEMENTMAP	3
#define TEXID_HEIGHTMAP			4

#define E_FNOTFOUND				_HRESULT_TYPEDEF_(0x87100000L)
#define E_FBAD					_HRESULT_TYPEDEF_(0x87100002L)
#define E_FNOTEXISTS			_HRESULT_TYPEDEF_(0x87100004L)

#define maligned(type)			(type*)_aligned_malloc(sizeof(type), 16);

#ifndef DIRECTINPUT_VERSION
#	define DIRECTINPUT_VERSION 0x8000
#endif

#if defined(DEBUG) || defined(_DEBUG)
#	ifndef V
#		define V(x)           { hr = (x); if( FAILED(hr) ) { DXTrace( __FILEW__, (DWORD)__LINE__, hr, L#x, true ); } }
#	endif
#	ifndef V_RETURN
#		define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { hr = DXTrace( __FILEW__, (DWORD)__LINE__, hr, L#x, true ); return hr; } }
#	endif
#	ifndef V_JMP
#		define V_JMP(x, y)   { hr = (x); if( FAILED(hr) ) { hr = DXTrace( __FILEW__, (DWORD)__LINE__, hr, L#x, true ); goto y; } }
#	endif
#	ifndef AUTERR
#		define AUTERR(x, y)		DXTrace( __FILEW__, (DWORD)__LINE__, y, x, true )
#	endif
#else
#	ifndef V
#		define V(x)           { hr = (x); }
#	endif
#	ifndef V_RETURN
#		define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#	endif
#	ifndef V_JMP
#		define V_JMP(x, y)   { hr = (x); if( FAILED(hr) ) { goto y; } }
#	endif
#	ifndef AUTERR
#		define AUTERR(x, y)	  y
#	endif
#endif

#define SAFE_RELEASE(x) { if ((x)) { (x)->Release(); } (x) = nullptr; }
#define SAFE_RELEASEWO(x) { if ((x)) (x)->Release(); }

// Input
typedef char KEYBOARDSTATE[256];
typedef char *LPKEYBOARDSTATE;

#define KEY_PRESSED(x, y)	((x)[(y)] & 0x80)

// fTime - time elapsed from last reset timer
// fTimeElapsed - time elapsed from last update timer
typedef bool					( *PCALLBACKONINPUT )( DIMOUSESTATE *pDIMouseState, LPKEYBOARDSTATE pKeysState );
typedef void					( *PCALLBACKONFRAMEPROCESS )( ID3D11Device *pD3DDevice, ID3D11Texture2D *pDSTexture, ID3D11DepthStencilView *pDSView, double fTime, double fTimeElapsed );
typedef void					( *PCALLBACKONRELEASE )();
typedef HRESULT					( *PCALLBACKONCREATEDEVICE )();
typedef HRESULT					( *PCALLBACKONRESETDEVICE )();
typedef LRESULT					( *PCALLBACKONWNDPROC )( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *bNoFurtherProcessing );

// Set callbacks
void AUTSetCallbackOnCreateDevice( PCALLBACKONCREATEDEVICE pCallback );
void AUTSetCallbackOnResetDevice( PCALLBACKONRESETDEVICE pCallback );
void AUTSetCallbackOnRelease( PCALLBACKONRELEASE pCallback );
void AUTSetCallbackOnFrameProcess( PCALLBACKONFRAMEPROCESS pCallback );
void AUTSetCallbackOnInput( PCALLBACKONINPUT pCallback );
void AUTSetCallbackOnWndProc( PCALLBACKONWNDPROC pCallback );

bool AUTIsActive();
bool AUTIsWindowCreated();

DXGI_SWAP_CHAIN_DESC			*AUTGetSwapChainParams();
D3D_DRIVER_TYPE					*AUTGetDriverType();
D3D_FEATURE_LEVEL				*AUTGetFeautureLevel();
ID3D11Device					*AUTGetD3DDevice();
ID3D11DeviceContext				*AUTGetImmediateContext();
IDXGISwapChain					*AUTGetSwapChain();
ID3D11RenderTargetView			*AUTGetRenderTargetView();

ID3D11Texture2D					*AUTGetDSTexture();
ID3D11DepthStencilView			*AUTGetDSView();
bool							AUTGetZOnlyPass();
void							AUTSetZOnlyPass(bool b);
CCamera							*AUTGetLight();
void							AUTSetLight(CCamera *pCamera);

void AUTSetSwapChainParams( DXGI_SWAP_CHAIN_DESC *pSets );
DXGI_SWAP_CHAIN_DESC *AUTGetSwapChainParams();


HWND AUTGetHWND();
HINSTANCE AUTGetHINSTANCE();
// Get stats
double AUTGetTime();
float AUTGetElapsedTime();
float AUTGetFPS();
LPDIRECTINPUT8 AUTGetDI();
LPDIRECTINPUTDEVICE8 AUTGetKeyboardDevice();
LPDIRECTINPUTDEVICE8 AUTGetMouseDevice();
void AUTPause( bool bPauseTime, bool bPauseRendering );

char *AUTGetKeysState();
DIMOUSESTATE *AUTGetDIMouseState();
void SetWaitingPresent( bool bFlag );

HRESULT AUTCreateWindow( const wchar_t *pStrWindowTitle, DWORD dwColor,
						   int nX, int nY, int nWidth, int nHeight,
						   HINSTANCE hInstance, HICON hIcon, HCURSOR hCursor, HMENU hMenu );
HRESULT AUTCreateDevice();

void AUTShutdown( int nExitCode );
void AUTRelease();
int AUTMainLoop();

class HRTimer {
public:
	HRTimer() : frequency(GetFrequency()) { }
    LONGLONG GetFrequency()
	{
		LARGE_INTEGER proc_freq;
		QueryPerformanceFrequency(&proc_freq);
		return proc_freq.QuadPart;
	}
    void Reset() 
	{
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
		QueryPerformanceCounter(&start);
		SetThreadAffinityMask(GetCurrentThread(), oldmask);
	}
    double GetElapsed() 
	{
		DWORD_PTR oldmask = SetThreadAffinityMask(GetCurrentThread(), 0);
		QueryPerformanceCounter(&stop);
		SetThreadAffinityMask(GetCurrentThread(), oldmask);
		return ((stop.QuadPart - start.QuadPart) / (double)frequency);
	}
private:
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
    LONGLONG frequency;
};

#endif