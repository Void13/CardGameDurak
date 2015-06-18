#include "AUTshaders.h"
#include "AUTstring.h"
#include <D3Dcompiler.h>

#pragma comment( lib, "D3dcompiler.lib" )

CShaders *pShaders;


INPUTLAYOUTID AUTAddInputLayout(const D3D11_INPUT_ELEMENT_DESC *ILDesc, DWORD dwNumElems) 
{
	return pShaders->AddInputLayout(ILDesc, dwNumElems); 
}
AUTSHADERSFUNC void AUTSetInputLayout(INPUTLAYOUTID _ILID, ID3D11DeviceContext *pImmediateContext) 
{ 
	pShaders->SetInputLayout(_ILID, pImmediateContext); 
}

HRESULT	AUTAddPShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{ 
	return pShaders->AddPShader(szFileName, szEntryPoint, szShaderName, pDefines, pSID); 
}
HRESULT	AUTAddVShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, INPUTLAYOUTID _ILID, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{
	return pShaders->AddVShader(szFileName, szEntryPoint, szShaderName, _ILID, pDefines, pSID); 
}
HRESULT	AUTAddGShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{ 
	return pShaders->AddGShader(szFileName, szEntryPoint, szShaderName, pDefines, pSID); 
}
HRESULT	AUTAddHShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{ 
	return pShaders->AddHShader(szFileName, szEntryPoint, szShaderName, pDefines, pSID); 
}
HRESULT	AUTAddDShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{ 
	return pShaders->AddDShader(szFileName, szEntryPoint, szShaderName, pDefines, pSID); 
}
HRESULT	AUTAddCShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID) 
{ 
	return pShaders->AddCShader(szFileName, szEntryPoint, szShaderName, pDefines, pSID);
}

bool								AUTShadersInit() { return (pShaders = new CShaders) != 0; }
void								AUTShadersDestroy() { delete pShaders; }

AUTSHADERSFUNC ID3D11PixelShader	*AUTGetPShader(SHADERID _SID) { return pShaders->GetPShader(_SID); }
AUTSHADERSFUNC ID3D11VertexShader	*AUTGetVShader(SHADERID _SID) { return pShaders->GetVShader(_SID); }
AUTSHADERSFUNC ID3D11HullShader		*AUTGetHShader(SHADERID _SID) { return pShaders->GetHShader(_SID); }
AUTSHADERSFUNC ID3D11GeometryShader	*AUTGetGShader(SHADERID _SID) { return pShaders->GetGShader(_SID); }
AUTSHADERSFUNC ID3D11DomainShader	*AUTGetDShader(SHADERID _SID) { return pShaders->GetDShader(_SID); }
AUTSHADERSFUNC ID3D11ComputeShader	*AUTGetCShader(SHADERID _SID) { return pShaders->GetCShader(_SID); }

AUTSHADERSFUNC void					AUTSetPShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetPShader(_SID, pImmediateContext); }
AUTSHADERSFUNC void					AUTSetVShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetVShader(_SID, pImmediateContext); }
AUTSHADERSFUNC void					AUTSetGShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetGShader(_SID, pImmediateContext); }
AUTSHADERSFUNC void					AUTSetDShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetDShader(_SID, pImmediateContext); }
AUTSHADERSFUNC void					AUTSetHShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetHShader(_SID, pImmediateContext); }
AUTSHADERSFUNC void					AUTSetCShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext) { pShaders->SetCShader(_SID, pImmediateContext); }

//AUTSHADERSFUNC void					AUTSetCBPerBatch(CCBPerBatch *_CBPerBatch) { pShaders->SetCBPerBatch(_CBPerBatch); }
AUTSHADERSFUNC CCBPerBatch			*AUTGetCBPerBatch() { return pShaders->GetCBPerBatch(); }
AUTSHADERSFUNC void					AUTUpdateD3DCBPerBatch(ID3D11DeviceContext *pImmediateContext) { pShaders->UpdateD3DCBPerBatch(pImmediateContext); }
AUTSHADERSFUNC ID3D11Buffer			* const AUTGetD3DCBPerBatch() { return pShaders->GetD3DCBPerBatch(); }

HRESULT								AUTCreateAllConstantBuffers() { return pShaders->CreateAllConstantBuffers(); }

HRESULT AUTCreateSampler(const D3D11_SAMPLER_DESC &SamplerDesc, OUT DWORD *dwID)
{
	return pShaders->CreateSampler(SamplerDesc, dwID);
}
AUTSHADERSFUNC void AUTSetPSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetPSSampler(dwID, dwSlot, pImmediateContext);
}
AUTSHADERSFUNC void AUTSetVSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetVSSampler(dwID, dwSlot, pImmediateContext);
}
AUTSHADERSFUNC void AUTSetGSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetGSSampler(dwID, dwSlot, pImmediateContext);
}
AUTSHADERSFUNC void AUTSetHSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetHSSampler(dwID, dwSlot, pImmediateContext);
}
AUTSHADERSFUNC void AUTSetDSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetDSSampler(dwID, dwSlot, pImmediateContext);
}
AUTSHADERSFUNC void AUTSetCSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext)
{
	pShaders->SetCSSampler(dwID, dwSlot, pImmediateContext);
}

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, const D3D_SHADER_MACRO* pDefines = NULL )
{
    HRESULT hr = S_OK;
	HRESULT hr2 = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile( szFileName, pDefines, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

CShaders::CShaders()
{
	VertexShaders.clear();
	PixelShaders.clear();
	HullShaders.clear();
	DomainShaders.clear();
	GeometryShaders.clear();
	ComputeShaders.clear();
	
	pD3DCBPerFrame = NULL;
	pD3DCBPerMesh = NULL;
	pD3DCBPerBatch = NULL;

	InputLayouts.clear();

	SamplerStates.clear();
}

CShaders::~CShaders()
{
	for (ID3D11VertexShader *iShader : VertexShaders)
		SAFE_RELEASE(iShader);

	for (ID3D11PixelShader *iShader : PixelShaders)
		SAFE_RELEASE(iShader);

	for (ID3D11HullShader *iShader : HullShaders)
		SAFE_RELEASE(iShader);

	for (ID3D11DomainShader *iShader : DomainShaders)
		SAFE_RELEASE(iShader);

	for (ID3D11GeometryShader *iShader : GeometryShaders)
		SAFE_RELEASE(iShader);

	for (ID3D11ComputeShader *iShader : ComputeShaders)
		SAFE_RELEASE(iShader);

	VertexShaders.clear();
	PixelShaders.clear();
	HullShaders.clear();
	DomainShaders.clear();
	GeometryShaders.clear();
	ComputeShaders.clear();

	SAFE_RELEASE(pD3DCBPerFrame);
	SAFE_RELEASE(pD3DCBPerMesh);
	SAFE_RELEASE(pD3DCBPerBatch);

	SamplerStates.clear();

	InputLayouts.clear();
}

#define AUTSHADERS_SAVE_TO_FILE 1

/*
HRESULT WriteReadShader(ID3DBlob **ppSBlob, const wchar_t *const sTargetFileName)
{
	HRESULT hr;
#ifdef AUTSHADERS_SAVE_TO_FILE
	V_RETURN(D3DWriteBlobToFile(*ppSBlob, sTargetFileName, true));
#endif
	
	V_RETURN(D3DReadFileToBlob(sTargetFileName, ppSBlob));

	return S_OK;
}
*/

HRESULT WriteReadShader(ID3DBlob **ppSBlob, const wchar_t *const sTargetFileName)
{
	HRESULT hr;

	FILE *pFile;

#ifdef AUTSHADERS_SAVE_TO_FILE
	_wfopen_s(&pFile, sTargetFileName, L"wb");
	fwrite((*ppSBlob)->GetBufferPointer(), (*ppSBlob)->GetBufferSize(), 1, pFile);
	fclose(pFile);
#endif
	
	int nSize = 0;
	_wfopen_s(&pFile, sTargetFileName, L"rb");

	fseek(pFile, 0, SEEK_END);
	nSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	
	*ppSBlob = NULL;
	V_RETURN(D3DCreateBlob(nSize, ppSBlob));

	fread_s((*ppSBlob)->GetBufferPointer(), nSize, nSize, 1, pFile);
	
	fclose(pFile);

	return S_OK;
}


HRESULT CShaders::AddPShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11PixelShader *PStmp = NULL;
	
	static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "ps_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".pso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = AUTGetD3DDevice()->CreatePixelShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &PStmp );
	SAFE_RELEASE(pSBlob);
	if( FAILED( hr ) )
		return hr;

	PixelShaders.push_back(PStmp);
	*pSID = PixelShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}

HRESULT	CShaders::AddVShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, INPUTLAYOUTID _ILID, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11VertexShader *VStmp = NULL;

	ID3D11Device *pD3DDevice = AUTGetD3DDevice();
	
static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "vs_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".vso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = pD3DDevice->CreateVertexShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &VStmp );
	if( FAILED( hr ) )
		return hr;

	if (_ILID != -1)
	{
		CInputLayout *pLayout = InputLayouts.at(_ILID);
		hr = pD3DDevice->CreateInputLayout( pLayout->pILDesc, pLayout->dwNumElems, 
			pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), &pLayout->pInputLayout );
		SAFE_RELEASE(pSBlob);
		if (FAILED(hr))
			return hr;
	}

	VertexShaders.push_back(VStmp);
	*pSID = VertexShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}

HRESULT	CShaders::AddGShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11GeometryShader *GStmp = NULL;
	ID3D11Device *pD3DDevice = AUTGetD3DDevice();

static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "gs_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".gso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = pD3DDevice->CreateGeometryShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &GStmp );
	if( FAILED( hr ) )
		return hr;

	GeometryShaders.push_back(GStmp);
	*pSID = GeometryShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}

HRESULT	CShaders::AddHShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11HullShader *HStmp = NULL;

static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "hs_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".hso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = AUTGetD3DDevice()->CreateHullShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &HStmp );
	SAFE_RELEASE(pSBlob);
	if( FAILED( hr ) )
		return hr;

	HullShaders.push_back(HStmp);
	*pSID = HullShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}

HRESULT	CShaders::AddDShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11DomainShader *DStmp = NULL;

static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "ds_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".dso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = AUTGetD3DDevice()->CreateDomainShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &DStmp );
	SAFE_RELEASE(pSBlob);
	if( FAILED( hr ) )
		return hr;

	DomainShaders.push_back(DStmp);
	*pSID = DomainShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}

HRESULT	CShaders::AddCShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID)
{
	HRESULT hr;
	ID3DBlob* pSBlob = NULL;
	ID3D11ComputeShader *CStmp = NULL;

static wchar_t wszTex[64];
	mbstowcs(wszTex, szShaderName, 63);
	wstring sPath, sFileName;
	SplitFileName(wstring(szFileName), L"/\\", sPath, sFileName);

	wstring sSourceFile = sPath + L"/\\Sources\\" + sFileName;
#ifdef AUTSHADERS_SAVE_TO_FILE
	hr = CompileShaderFromFile( sSourceFile.c_str(), szEntryPoint, "cs_5_0", &pSBlob, pDefines);
	if ( FAILED(hr) ) { 
		MessageBox( NULL, L"Unable to compilte .fx file. Please, reinstall the programm.", L"Error", MB_OK ); 
		return hr;
	}
#endif

	wstring sTargetFileName = sPath + L"/\\" + wszTex + wstring(L".cso");
	V_RETURN(WriteReadShader(&pSBlob, sTargetFileName.c_str()));

	hr = AUTGetD3DDevice()->CreateComputeShader( pSBlob->GetBufferPointer(), pSBlob->GetBufferSize(), NULL, &CStmp );
	SAFE_RELEASE(pSBlob);
	if( FAILED( hr ) )
		return hr;

	ComputeShaders.push_back(CStmp);
	*pSID = ComputeShaders.size() - 1;

	AUTMapIDsPush(szShaderName, *pSID);

	return S_OK;
}