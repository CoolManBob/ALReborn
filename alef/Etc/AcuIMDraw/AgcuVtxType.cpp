// AgcuVtxType.cpp
// -----------------------------------------------------------------------------
//                            _     _ _        _______                                       
//     /\                    | |   | | |      |__   __|                                      
//    /  \    __ _  ___ _   _| |   | | |___  __  | |   _   _ _ __   ___      ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | |\ \ / /| __\ \/ /  | |  | | | | '_ \ / _ \    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | \ V / | |_ >  <   | |  | |_| | |_) |  __/ _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|  \_/   \__/_/\_\  |_|   \__, | .__/ \___|(_) \___| .__/| .__/ 
//            __/ |                                     __/ | |                 | |   | |    
//           |___/                                     |___/|_|                 |_|   |_|    
//
// uiLogin
//
// -----------------------------------------------------------------------------
// Originally created on 02/03/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "MagDebug.h"
#include "AgcuVtxType.h"

AGCUVTX_BEGIN	//##
// -----------------------------------------------------------------------------
const DWORD	VTX_PD::FVF	= D3DFVF_XYZ 
						| D3DFVF_DIFFUSE;
const DWORD	VTX_PD::SIZE	= sizeof(VTX_PD);

VTX_PD::VTX_PD(const D3DXVECTOR3& pos, const DWORD diff)
	: pos(pos), diff(diff)
{}
VTX_PD::VTX_PD()
	: pos(0.f,0.f,0.f)	, diff(0xFFFFFFFF)
{}
VTX_PD::VTX_PD(const VTX_PD& cpy)
	: pos(cpy.pos), diff(cpy.diff)
{}
VTX_PD& VTX_PD::operator = (const VTX_PD& cpy)
{
	ASSERT( this != &cpy );
	pos = cpy.pos;
	diff = cpy.diff;

	return *this;
};


// -----------------------------------------------------------------------------
const DWORD VTX_PNDT1::FVF	= D3DFVF_XYZ 
							| D3DFVF_NORMAL 
							| D3DFVF_DIFFUSE 
							| D3DFVF_TEX1;
const DWORD VTX_PNDT1::SIZE= sizeof( VTX_PNDT1 );
VTX_PNDT1::VTX_PNDT1()
	: pos(0.f, 0.f, 0.f)
	, nrm(0.f, 1.f, 0.f)
	, diff(0xFFFFFFFF)
	, uv1(0.f, 0.f)
{
}
VTX_PNDT1::VTX_PNDT1(const VTX_PNDT1& cpy)
	: pos(cpy.pos)
	, nrm(cpy.nrm)
	, diff(cpy.diff)
	, uv1(cpy.uv1)
{
}
VTX_PNDT1& VTX_PNDT1::operator = (const VTX_PNDT1& cpy)
{
	ASSERT( this != &cpy );

	pos	= cpy.pos;
	nrm	= cpy.nrm;
	diff= cpy.diff;
	uv1	= cpy.uv1;

	return *this;
}

AGCUVTX_END	//##
// -----------------------------------------------------------------------------
// AgcuVtxType.cpp - End of file
// -----------------------------------------------------------------------------
