#pragma once

#ifndef EFFUT_BEGIN
#define EFFUT_BEGIN	namespace __EffUtil__ {
#define EFFUT_END	};
#define USING_EFFUT	using namespace __EffUtil__;
#define EFFUT		__EffUtil__
#endif

#include <d3dx9math.h>

EFFUT_BEGIN

typedef struct VTX_PT1 
{
	D3DXVECTOR3	pos;
	D3DXVECTOR2 uv;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PT1();
	VTX_PT1(const VTX_PT1& cpy);
	VTX_PT1& operator= (const VTX_PT1& cpy);

}*PVTX_PT1, *LPVTX_PT1;

typedef struct VTX_PD
{
	D3DXVECTOR3	pos;
	DWORD		diff;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PD();
	VTX_PD(const VTX_PD& cpy);
	VTX_PD& operator= (const VTX_PD& cpy);

}*PVTX_PD, *LPVTX_PD;

typedef struct VTX_PNDT1 
{
	D3DXVECTOR3	pos;
	D3DXVECTOR3 nrm;
	DWORD		diff;
	D3DXVECTOR2 uv;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PNDT1();
	VTX_PNDT1(const VTX_PNDT1& cpy);
	VTX_PNDT1& operator= (const VTX_PNDT1& cpy);

}*PVTX_PNDT1, *LPVTX_PNDT1;

typedef struct VTX_PDT1 
{
	D3DXVECTOR3	pos;
	DWORD		diff;
	D3DXVECTOR2 uv;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PDT1();
	VTX_PDT1(const VTX_PDT1& cpy);
	VTX_PDT1& operator= (const VTX_PDT1& cpy);

}*PVTX_PDT1, *LPVTX_PDT1;

typedef struct VTX_PDST1 
{
	D3DXVECTOR3	pos;
	DWORD		diff;
	DWORD		spec;
	D3DXVECTOR2 uv;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PDST1();
	VTX_PDST1(const VTX_PDST1& cpy);
	VTX_PDST1& operator= (const VTX_PDST1& cpy);
}*PVTX_PDST1, *LPVTX_PDST1;

typedef struct VTX_PDS
{
	D3DXVECTOR3	pos;
	DWORD		diff;
	DWORD		spec;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_PDS();
	VTX_PDS(const VTX_PDS& cpy);
	VTX_PDS& operator= (const VTX_PDS& cpy);
}*PVTX_PDS, *LPVTX_PDS;

typedef struct VTX_RHWT1
{
	D3DXVECTOR4	rhw;
	D3DXVECTOR2	uv;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_RHWT1();
	VTX_RHWT1(const VTX_RHWT1& cpy);
	VTX_RHWT1& operator= (const VTX_RHWT1& cpy);
}*PVTX_RHWT1, *LPVTX_RHWT1;

typedef struct VTX_RHWD
{
	D3DXVECTOR4	rhw;
	DWORD		diff;
	
	static const DWORD	FVF;
	static const DWORD	SIZE;

	VTX_RHWD();
	VTX_RHWD(const VTX_RHWD& cpy);
	VTX_RHWD& operator= (const VTX_RHWD& cpy);
}*PVTX_RHWD, *LPVTX_RHWD;

EFFUT_END
