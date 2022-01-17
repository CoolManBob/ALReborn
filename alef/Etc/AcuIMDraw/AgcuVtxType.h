// AgcuVtxType.h
// -----------------------------------------------------------------------------
//                            _     _ _        _______                      _     
//     /\                    | |   | | |      |__   __|                    | |    
//    /  \    __ _  ___ _   _| |   | | |___  __  | |   _   _ _ __   ___    | |__  
//   / /\ \  / _` |/ __| | | |\ \ / /| __\ \/ /  | |  | | | | '_ \ / _ \   | '_ \ 
//  / ____ \| (_| | (__| |_| | \ V / | |_ >  <   | |  | |_| | |_) |  __/ _ | | | |
// /_/    \_\\__, |\___|\__,_|  \_/   \__/_/\_\  |_|   \__, | .__/ \___|(_)|_| |_|
//            __/ |                                     __/ | |                   
//           |___/                                     |___/|_|                   
//
// uiLogin
// 
////-------------------------------------------------------------------
//
//// Flexible vertex format bits
////
//#define D3DFVF_RESERVED0        0x001			: NOT USED
//#define D3DFVF_POSITION_MASK    0x400E		: NOT USED
//#define D3DFVF_XYZ              0x002			: pos
//#define D3DFVF_XYZRHW           0x004			: rhw
//#define D3DFVF_XYZB1            0x006			: NOT USED
//#define D3DFVF_XYZB2            0x008			: NOT USED
//#define D3DFVF_XYZB3            0x00a			: NOT USED
//#define D3DFVF_XYZB4            0x00c			: NOT USED
//#define D3DFVF_XYZB5            0x00e			: NOT USED
//#define D3DFVF_XYZW             0x4002		: NOT USED
//
//#define D3DFVF_NORMAL           0x010			: nrm
//#define D3DFVF_PSIZE            0x020			: psize
//#define D3DFVF_DIFFUSE          0x040			: diff
//#define D3DFVF_SPECULAR         0x080			: spec
//
//#define D3DFVF_TEXCOUNT_MASK    0xf00			: NOT USED
//#define D3DFVF_TEXCOUNT_SHIFT   8				: NOT USED
//#define D3DFVF_TEX0             0x000			: NOT USED
//#define D3DFVF_TEX1             0x100			: uv1
//#define D3DFVF_TEX2             0x200			: uv2
//#define D3DFVF_TEX3             0x300			: uv3
//#define D3DFVF_TEX4             0x400			: uv4
//#define D3DFVF_TEX5             0x500			: uv5
//#define D3DFVF_TEX6             0x600			: uv6
//#define D3DFVF_TEX7             0x700			: uv7
//#define D3DFVF_TEX8             0x800			: uv8
//
//#define D3DFVF_LASTBETA_UBYTE4   0x1000
//#define D3DFVF_LASTBETA_D3DCOLOR 0x8000
//
//#define D3DFVF_RESERVED2         0x6000  // 2 reserved bits
// -----------------------------------------------------------------------------
// Originally created on 02/03/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_AGCUVTXTYPE_20050203
#define _H_AGCUVTXTYPE_20050203

#pragma once

//namespace
//AGCUVTX : Alef Global Client Util VerTeX
#ifndef AGCUVTX_BEGIN
#define AGCUVTX_BEGIN	namespace __AGCUVTX__ \
						{
#define AGCUVTX_END		};
#define USING_AGCUVTX	using namespace __AGCUVTX__;
#endif

#include <d3dx9math.h>

AGCUVTX_BEGIN	//##

// -----------------------------------------------------------------------------
typedef struct VTX_PD
{
	static const DWORD	FVF;
	static const DWORD	SIZE;

	D3DXVECTOR3	pos;
	DWORD		diff;

	VTX_PD();
	explicit VTX_PD(const D3DXVECTOR3& pos, const DWORD diff=0xFFFFFFFF);
	VTX_PD(const VTX_PD& cpy);
	VTX_PD& operator = (const VTX_PD& cpy);
}*PVTX_PD, *LPVTX_PD;

// -----------------------------------------------------------------------------
typedef struct VTX_PNDT1
{
	static const DWORD	FVF;
	static const DWORD	SIZE;

	D3DXVECTOR3 pos;
	D3DXVECTOR3 nrm;
	DWORD		diff;
	D3DXVECTOR2	uv1;
	
	VTX_PNDT1();
	VTX_PNDT1(const VTX_PNDT1& cpy);
	VTX_PNDT1& operator = (const VTX_PNDT1& cpy);
}*PVTX_PNDT1, *LPVTX_PNDT1;

AGCUVTX_END //##

#endif // _H_AGCUVTXTYPE_20050203
// -----------------------------------------------------------------------------
// AgcuVtxType.h - End of file
// -----------------------------------------------------------------------------

