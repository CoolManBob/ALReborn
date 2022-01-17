//@{ Jaewon 20041007
// created.
//@} Jaewon

//@{ Jaewon 20041110
#include "AuMD5Encrypt.h"
//@} Jaewon
#include "effectStateManager.h"
#include "rwcore.h"
#include "rwd3d9.h"
#include <cassert>

// d3dx effect state manager for compatibility with RenderWare state caching.
class RwEffectStateManager : public ID3DXEffectStateManager
{
public:
	STDMETHOD(LightEnable)(DWORD Index, BOOL Enable)
	{
		RwD3D9EnableLight(Index, Enable);

		return S_OK;
	}

	STDMETHOD(SetFVF)(DWORD FVF)
	{
		RwD3D9SetFVF(FVF);

		return S_OK;
	}

	STDMETHOD(SetLight)(DWORD Index, CONST D3DLIGHT9* pLight)
	{
		RwD3D9SetLight(Index, pLight);

		return S_OK;
	}

	STDMETHOD(SetMaterial)(CONST D3DMATERIAL9* pMaterial)
	{
		RwD3D9SetMaterial(pMaterial);

		return S_OK;
	}

	STDMETHOD(SetNPatchMode)(FLOAT nSegments)
	{
		IDirect3DDevice9_SetNPatchMode((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), nSegments);

		return S_OK;
	}

	STDMETHOD(SetPixelShader)(LPDIRECT3DPIXELSHADER9 pShader)
	{
		RwD3D9SetPixelShader(pShader);

		return S_OK;
	}

	STDMETHOD(SetPixelShaderConstantB)(UINT StartRegister, CONST BOOL* pConstantData, UINT RegisterCount)
	{
		IDirect3DDevice9_SetPixelShaderConstantB((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(SetPixelShaderConstantF)(UINT StartRegister, CONST FLOAT* pConstantData, UINT RegisterCount)
	{
		RwD3D9SetPixelShaderConstant(StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(SetPixelShaderConstantI)(UINT StartRegister, CONST INT* pConstantData, UINT RegisterCount)
	{
		IDirect3DDevice9_SetPixelShaderConstantI((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(SetRenderState)(D3DRENDERSTATETYPE State, DWORD Value)
	{
		RwD3D9SetRenderState(State, Value);

		return S_OK;
	}

	STDMETHOD(SetSamplerState)(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
	{
		RwD3D9SetSamplerState(Sampler, Type, Value);

		return S_OK;
	}

	STDMETHOD(SetTexture)(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
	{
		IDirect3DDevice9_SetTexture((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), Stage, pTexture);

		return S_OK;
	}

	STDMETHOD(SetTextureStageState)(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
	{
		RwD3D9SetTextureStageState(Stage, Type, Value);

		return S_OK;
	}

	STDMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
	{
		RwD3D9SetTransform(State, pMatrix);

		return S_OK;
	}

	STDMETHOD(SetVertexShader)(LPDIRECT3DVERTEXSHADER9 pShader)
	{
		RwD3D9SetVertexShader(pShader);

		return S_OK;
	}

	STDMETHOD(SetVertexShaderConstantB)(UINT StartRegister, CONST BOOL* pConstantData, UINT RegisterCount)
	{
		IDirect3DDevice9_SetVertexShaderConstantB((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(SetVertexShaderConstantF)(UINT StartRegister, CONST FLOAT* pConstantData, UINT RegisterCount)
	{
		RwD3D9SetVertexShaderConstant(StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(SetVertexShaderConstantI)(UINT StartRegister, CONST INT* pConstantData, UINT RegisterCount)
	{
		IDirect3DDevice9_SetVertexShaderConstantI((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice(), StartRegister, pConstantData, RegisterCount);

		return S_OK;
	}

	STDMETHOD(QueryInterface)(REFIID iid, LPVOID *ppv);

    STDMETHOD_(ULONG, AddRef)()
	{
		return 1;//IUnknown::AddRef();
	}
    STDMETHOD_(ULONG, Release)()
	{
		return 1;//IUnknown::Release();
	}
};

static RwEffectStateManager _rwEffectStateManager;

HRESULT STDMETHODCALLTYPE RwEffectStateManager::QueryInterface(REFIID iid, LPVOID *ppv)
{
	if(iid == IID_ID3DXEffectStateManager)
		*ppv = &_rwEffectStateManager;

	return S_OK;//IUnknown::QueryInterface(iid, ppv);
}

void EffectSetStageManager(LPD3DXEFFECT pEffect)
{
	//@{ 2005308
	// Don't use a state manager, Strange Nvidia bug! --+
	//HRESULT hr = pEffect->SetStateManager(&_rwEffectStateManager);

	//assert(SUCCEEDED(hr));
	//@} Jaewon
}

//@{ Jaewon 20041110
#define HASH_KEY_STRING "1111"

BOOL DecryptMD5(char *data, unsigned long size)
{
	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	if(MD5.DecryptString(MD5_HASH_KEY_STRING, data, size))
#else
	if(MD5.DecryptString(HASH_KEY_STRING, data, size))
#endif
		return TRUE;
	else
		return FALSE;
}
//@} Jaeown
