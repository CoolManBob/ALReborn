// AgcdEffPostFX.cpp
// -----------------------------------------------------------------------------
//                          _ ______  __  __ _____            _   ________   __                     
//     /\                  | |  ____|/ _|/ _|  __ \          | | |  ____\ \ / /                     
//    /  \    __ _  ___  __| | |__  | |_| |_| |__) | ___  ___| |_| |__   \ V /      ___ _ __  _ __  
//   / /\ \  / _` |/ __|/ _` |  __| |  _|  _|  ___/ / _ \/ __| __|  __|   > <      / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| (_| | |____| | | | | |    | (_) \__ \ |_| |     / . \  _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|______|_| |_| |_|     \___/|___/\__|_|    /_/ \_\(_) \___| .__/| .__/ 
//            __/ |                                                                    | |   | |    
//           |___/                                                                     |_|   |_|    
//
// post fx effect
//
// -----------------------------------------------------------------------------
// Originally created on 04/21/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcdEffPostFX.h"
#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffPostFX);

LPCSTR AgcdEffPostFX::tech[e_fx_typenum] = {
	"ClampingCircle",	//e_fx_ClampingCircle	
	"Wave",				//e_fx_Wave			
	"Shockwave",		//e_fx_Shockwave		
	"Darken",			//e_fx_Darken			
	"Brighten",			//e_fx_Brighten		
	"Ripple",			//e_fx_Ripple			
	"Twist",			//e_fx_Twist
};

// -----------------------------------------------------------------------------
AgcdEffPostFX::AgcdEffPostFX()
: AgcdEffBase( AgcdEffBase::E_EFFBASE_POSTFX )
{
	EFFMEMORYLOG_CON;

	m_sharedparam.center.x =
	m_sharedparam.center.y = 0.f;
	m_sharedparam.type	= e_fx_ClampingCircle;

	memset(&m_unionparam, 0, sizeof(m_unionparam));
}

// -----------------------------------------------------------------------------
AgcdEffPostFX::~AgcdEffPostFX()
{
	EFFMEMORYLOG_DES;
}

// set
// -----------------------------------------------------------------------------
void AgcdEffPostFX::bSetSharedParam(const RwV2d& center, eFXType type)
{
	RwV2dAssign( &m_sharedparam.center, &center );
	m_sharedparam.type = type;
};

// -----------------------------------------------------------------------------
void AgcdEffPostFX::bSetWaveParam(RwReal amplitude, RwReal frequency, LPCSTR tex)
{
	ASSERT( "kday" && m_sharedparam.type == e_fx_Wave );

	m_unionparam.wave.amplitude = amplitude;
	m_unionparam.wave.frequency = frequency;
	strcpy(	m_unionparam.wave.tex, tex );
};

// -----------------------------------------------------------------------------
void AgcdEffPostFX::bSetShockwaveParam(RwReal bias, RwReal width, LPCSTR tex)
{
	ASSERT( "kday" && m_sharedparam.type == e_fx_Shockwave );

	m_unionparam.shockwave.bias = bias;
	m_unionparam.shockwave.width = width;
	strcpy(	m_unionparam.shockwave.tex, tex );
};

// -----------------------------------------------------------------------------
void AgcdEffPostFX::bSetRippleParam(RwReal scale, RwReal frequency)
{
	ASSERT( "kday" && m_sharedparam.type == e_fx_Ripple );

	m_unionparam.ripple.scale = scale;
	m_unionparam.ripple.frequency = frequency;
};

// file in out
// -----------------------------------------------------------------------------
RwInt32 AgcdEffPostFX::bToFile(FILE* fp)
{
	RwInt32	ir = AgcdEffBase::tToFile( fp );
	ir += AgcdEffBase::tToFile( fp );

	ir += fwrite( &m_sharedparam, 1, sizeof(m_sharedparam), fp );
	ir += fwrite( &m_unionparam, 1, sizeof(m_unionparam), fp );

	ir += AgcdEffBase::tToFileVariableData( fp );
	return ir;
};

// -----------------------------------------------------------------------------
RwInt32 AgcdEffPostFX::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);

	ir += fread( &m_sharedparam, 1, sizeof(m_sharedparam), fp );
	ir += fread( &m_unionparam, 1, sizeof(m_unionparam), fp );

	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4( ir2 ) )
	{
		ASSERT( !"kday" && "AgcdEffBase::tFromFileVariableData failed" );
		return -1;
	}

	return ir + ir2;
};

#ifdef USE_MFC
// -----------------------------------------------------------------------------
INT32 AgcdEffPostFX::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && !"PostFX는 한이펙트에 2개를 넣지 못합니다." );
	return -1;
}
#endif//USE_MFC

// -----------------------------------------------------------------------------
// AgcdEffPostFX.cpp - End of file
// -----------------------------------------------------------------------------
