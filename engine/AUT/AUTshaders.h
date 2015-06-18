#ifndef H__AUTSHADERS
#define H__AUTSHADERS

#define AUTSHADERSFUNC

#include "AUT.h"

class CCBPerBatch
{
public:
	XMFLOAT2 vStart;
	XMFLOAT2 vEnd;
	float fScale;
	float fWidth;
	XMFLOAT2 vScreenSize;
	XMFLOAT4 vColor;
};

class CVertexTemplate
{
public:
	CVertexTemplate() { }
	CVertexTemplate(const XMFLOAT3 &_vPos, const XMFLOAT2 &_vTex, const XMFLOAT3 &_vNormal) : vPosition(_vPos), vTexCoords(_vTex), vNormal(_vNormal) { }
	CVertexTemplate(const XMFLOAT3 &_vPos) : vPosition(_vPos), vTexCoords(XMFLOAT2()), vNormal(XMFLOAT3()) { }

    XMFLOAT3					vPosition;
	XMFLOAT3					vNormal;
	XMFLOAT2					vTexCoords;
};

class CInputLayout
{
public:
	CInputLayout() : pInputLayout(NULL), pILDesc(NULL) { }
	~CInputLayout()
	{
		SAFE_RELEASE(pInputLayout);
		delete[] pILDesc; 
	}
	D3D11_INPUT_ELEMENT_DESC	*pILDesc;
	ID3D11InputLayout			*pInputLayout;
	DWORD						dwNumElems;
};

typedef DWORD INDEX;

class FACE
{
public:
	INDEX &operator [](const INDEX e)
	{
		return i[e];
	}
	INDEX i[3];
};

class CSamplerState
{
public:
	CSamplerState() : pSamplerState(NULL), pSamplerDesc(NULL)
	{
	}
	~CSamplerState()
	{
		delete pSamplerDesc;
		SAFE_RELEASE(pSamplerState);
	}
	D3D11_SAMPLER_DESC			*pSamplerDesc;
	ID3D11SamplerState			*pSamplerState;
};

typedef DWORD SHADERID;
typedef DWORD INPUTLAYOUTID;
typedef DWORD SAMPLERID;

class CShaders
{
public:
	CShaders();
	~CShaders();
	HRESULT					AddPShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);
	HRESULT					AddVShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, INPUTLAYOUTID ILID, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);
	HRESULT					AddGShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);
	HRESULT					AddHShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);
	HRESULT					AddDShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);
	HRESULT					AddCShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, SHADERID *pSID);

	ID3D11PixelShader		*GetPShader(SHADERID _SID) { return PixelShaders.at(_SID); }
	ID3D11VertexShader		*GetVShader(SHADERID _SID) { return VertexShaders.at(_SID); }
	ID3D11HullShader		*GetHShader(SHADERID _SID) { return HullShaders.at(_SID); }
	ID3D11GeometryShader	*GetGShader(SHADERID _SID) { return GeometryShaders.at(_SID); }
	ID3D11DomainShader		*GetDShader(SHADERID _SID) { return DomainShaders.at(_SID); }
	ID3D11ComputeShader		*GetCShader(SHADERID _SID) { return ComputeShaders.at(_SID); }

	void SetPShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->PSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->PSSetShader(PixelShaders.at(_SID), NULL, NULL);
	}

	void SetVShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->VSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->VSSetShader(VertexShaders.at(_SID), NULL, NULL);
	}

	void SetGShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->GSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->GSSetShader(GeometryShaders.at(_SID), NULL, NULL);
	}

	void SetHShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->HSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->HSSetShader(HullShaders.at(_SID), NULL, NULL);
	}

	void SetDShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->DSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->DSSetShader(DomainShaders.at(_SID), NULL, NULL);
	}

	void SetCShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext)
	{
		if (_SID == -1)
			pImmediateContext->CSSetShader(NULL, NULL, NULL);
		else
			pImmediateContext->CSSetShader(ComputeShaders.at(_SID), NULL, NULL);
	}

	void SetCBPerBatch(CCBPerBatch *_CBPerBatch)	{ memcpy(&CBPerBatch, _CBPerBatch, sizeof(CCBPerBatch)); }
	CCBPerBatch *GetCBPerBatch()				{ return &CBPerBatch; }
	ID3D11Buffer * const GetD3DCBPerBatch()			{ return pD3DCBPerBatch; }

	void UpdateD3DCBPerBatch(ID3D11DeviceContext *pImmediateContext)
	{
		pImmediateContext->UpdateSubresource( pD3DCBPerBatch, 0, NULL, &CBPerBatch, 0, 0 ); 
	}

	HRESULT CreateAllConstantBuffers()
	{
		HRESULT hr;
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(CCBPerBatch);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		V_RETURN(AUTGetD3DDevice()->CreateBuffer( &bufferDesc, NULL, &pD3DCBPerBatch ));

		AUTGetImmediateContext()->VSSetConstantBuffers(0, 1, &pD3DCBPerBatch);
		AUTGetImmediateContext()->PSSetConstantBuffers(0, 1, &pD3DCBPerBatch);
		AUTGetImmediateContext()->GSSetConstantBuffers(0, 1, &pD3DCBPerBatch);
		AUTGetImmediateContext()->CSSetConstantBuffers(0, 1, &pD3DCBPerBatch);
		AUTGetImmediateContext()->DSSetConstantBuffers(0, 1, &pD3DCBPerBatch);
		AUTGetImmediateContext()->HSSetConstantBuffers(0, 1, &pD3DCBPerBatch);

		return S_OK;
	}
	
	INPUTLAYOUTID AddInputLayout(const D3D11_INPUT_ELEMENT_DESC *ILDescs, DWORD dwNumElems)
	{
		CInputLayout *InputLayout = new CInputLayout;
		InputLayout->dwNumElems = dwNumElems;
		InputLayout->pILDesc = new D3D11_INPUT_ELEMENT_DESC[dwNumElems];
		memcpy(InputLayout->pILDesc, ILDescs, dwNumElems * sizeof(D3D11_INPUT_ELEMENT_DESC));
		InputLayout->pInputLayout = NULL;

		InputLayouts.push_back(InputLayout);
		return InputLayouts.size() - 1;
	}

	void SetInputLayout(INPUTLAYOUTID _ILID, ID3D11DeviceContext *pImmediateContext) 
	{
		if (_ILID == -1)
			pImmediateContext->IASetInputLayout(NULL);
		else
			pImmediateContext->IASetInputLayout(InputLayouts.at(_ILID)->pInputLayout);
	}


	HRESULT CreateSampler(const D3D11_SAMPLER_DESC &_SamplerDesc, OUT DWORD *dwID)
	{
		HRESULT hr;

		ID3D11SamplerState *pSampler;
		V_RETURN(AUTGetD3DDevice()->CreateSamplerState(&_SamplerDesc, &pSampler));

		CSamplerState *pSamplerState = new CSamplerState;
		pSamplerState->pSamplerDesc = new D3D11_SAMPLER_DESC;
		memcpy(pSamplerState->pSamplerDesc, &_SamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		pSamplerState->pSamplerState = pSampler;

		SamplerStates.push_back(pSamplerState);

		*dwID = SamplerStates.size() - 1;

		return S_OK;
	}

	void SetPSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{
		pImmediateContext->PSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}

	void SetVSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{ 
		pImmediateContext->VSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}

	void SetGSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{ 
		pImmediateContext->GSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}

	void SetHSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{ 
		pImmediateContext->HSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}

	void SetDSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{ 
		pImmediateContext->DSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}

	void SetCSSampler(SHADERID dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext) 
	{ 
		pImmediateContext->CSSetSamplers( dwSlot, 1, &SamplerStates.at(dwID)->pSamplerState);
	}
protected:
	vector<ID3D11VertexShader *>		VertexShaders;
	vector<ID3D11PixelShader *>			PixelShaders;
	vector<ID3D11HullShader *>			HullShaders;
	vector<ID3D11DomainShader *>		DomainShaders;
	vector<ID3D11GeometryShader *>		GeometryShaders;
	vector<ID3D11ComputeShader *>		ComputeShaders;

	CCBPerBatch							CBPerBatch;

	ID3D11Buffer						*pD3DCBPerFrame;
	ID3D11Buffer						*pD3DCBPerMesh;
	ID3D11Buffer						*pD3DCBPerBatch;

	vector<CInputLayout *>				InputLayouts;

	vector<CSamplerState *>				SamplerStates;
};

bool								AUTShadersInit();
void								AUTShadersDestroy();

HRESULT								AUTAddPShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);
HRESULT								AUTAddVShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, INPUTLAYOUTID _ILID, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);
HRESULT								AUTAddGShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);
HRESULT								AUTAddHShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);
HRESULT								AUTAddDShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);
HRESULT								AUTAddCShader(LPCWSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderName, const D3D_SHADER_MACRO *pDefines, OUT SHADERID *pSID);

AUTSHADERSFUNC ID3D11PixelShader	*AUTGetPShader(SHADERID _SID);
AUTSHADERSFUNC ID3D11VertexShader	*AUTGetVShader(SHADERID _SID);
AUTSHADERSFUNC ID3D11HullShader		*AUTGetHShader(SHADERID _SID);
AUTSHADERSFUNC ID3D11GeometryShader	*AUTGetGShader(SHADERID _SID);
AUTSHADERSFUNC ID3D11DomainShader	*AUTGetDShader(SHADERID _SID);
AUTSHADERSFUNC ID3D11ComputeShader	*AUTGetCShader(SHADERID _SID);

AUTSHADERSFUNC void					AUTSetPShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetVShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetGShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetDShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetHShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetCShader(SHADERID _SID, ID3D11DeviceContext *pImmediateContext);


//AUTSHADERSFUNC void					AUTSetCBPerBatch(CCBPerBatch *_CBPerBatch);
AUTSHADERSFUNC CCBPerBatch			*AUTGetCBPerBatch();

AUTSHADERSFUNC void					AUTUpdateD3DCBPerFrame(ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTUpdateD3DCBPerMesh(ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTUpdateD3DCBPerBatch(ID3D11DeviceContext *pImmediateContext);

AUTSHADERSFUNC ID3D11Buffer			*const AUTGetD3DCBPerFrame();
AUTSHADERSFUNC ID3D11Buffer			*const AUTGetD3DCBPerMesh();
AUTSHADERSFUNC ID3D11Buffer			*const AUTGetD3DCBPerBatch();

HRESULT								AUTCreateSampler(const D3D11_SAMPLER_DESC &_SamplerDesc, OUT DWORD *dwID); 
AUTSHADERSFUNC void					AUTSetPSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetVSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetGSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetHSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetDSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);
AUTSHADERSFUNC void					AUTSetCSSampler(DWORD dwID, DWORD dwSlot, ID3D11DeviceContext *pImmediateContext);

INPUTLAYOUTID						AUTAddInputLayout(const D3D11_INPUT_ELEMENT_DESC *ILDesc, DWORD dwNumElems);
AUTSHADERSFUNC void					AUTSetInputLayout(INPUTLAYOUTID _ILID, ID3D11DeviceContext *pImmediateContext);

HRESULT								AUTCreateAllConstantBuffers();


#endif