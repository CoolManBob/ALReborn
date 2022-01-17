#include "AgcuEffUtil.h"
#include "AgcdEffBase.h"
#include "AgcdEffBoard.h"
#include "AgcdEffLight.h"
#include "AgcdEffMFrm.h"
#include "AgcdEffObj.h"
#include "AgcdEffParticleSystem.h"
#include "AgcdEffSound.h"
#include "AgcdEffTail.h"
#include "AgcdEffTerrainBoard.h"
#include "AgcdEffPostFX.h"
#include "AgcdEffCamera.h"

#include "AgcuBillBoard.h"

#include <d3d9.h>

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

using namespace std;

const char* EFFBASETYPE_NAME[AgcdEffBase::E_EFFBASE_NUM] = {
	"board",
	"psys",
	"psys_sb",
	"tail",
	"object",
	"light",
	"sound",
	"mframe",
	"terrainboard",
	"postFX",
	"camera"
};

AgcdEffBase::AgcdEffBase(E_EFFBASETYPE eEffBaseType) : 
 m_eEffBaseType ( eEffBaseType ),
 m_dwDelay ( 0 ),
 m_dwLife ( 0 ),
 m_eLoopOpt( e_TblDir_none ),
 m_dwBitFlags ( 0 ),
 m_vecLPEffAni( 0 )
{
	Eff2Ut_ZEROBLOCK( m_szBaseTitle );
}

AgcdEffBase::~AgcdEffBase()
{
	vEffAnimClear();
}

void AgcdEffBase::vEffAnimClear()
{
	for( LPEffAniVecItr Itr = m_vecLPEffAni.begin(); Itr != m_vecLPEffAni.end(); ++Itr )
		AgcuEffAnimCreater::bDestroy( (*Itr) );
}

RwInt32 AgcdEffBase::bSetTitle(const RwChar* szTitle)
{
	if( !szTitle )		return -1;

	ASSERT( sizeof(m_szBaseTitle) > strlen(szTitle) );
	strcpy( m_szBaseTitle, szTitle );
	return 0;
}

void AgcdEffBase::bSetVarSizeInfo(const StVarSizeInfo& rStVarSizeInfo)
{
	m_stVarSizeInfo	= rStVarSizeInfo;
	m_vecLPEffAni.reserve( rStVarSizeInfo.m_nNumOfAnim );
}

RwInt32 AgcdEffBase::bInsEffAnim( LPSTCREATEPARAM_EFFANIM lpCreateParam_EffAnim )
{
	LPEFFANIM	pNew	= AgcuEffAnimCreater::bCreate( lpCreateParam_EffAnim );
	if( !pNew )
	{
		Eff2Ut_ERR( "AgcdEffBase::bInsEffAnim failed : AgcuEffAnimCreater::bCreate failed" );
		return -1;
	}

	m_vecLPEffAni.push_back( pNew );
	return 0;
}

RwInt32 AgcdEffBase::bDelEffAnim( RwInt32 nIndex )
{
	if( nIndex >= static_cast<RwInt32>(m_vecLPEffAni.size()) )
	{
		Eff2Ut_ERR( "AgcdEffBase::bDelEffAnim failed : nIndex not available" );
		return -1;
	}

	LPEffAniVecItr	Itr = std::find( m_vecLPEffAni.begin(), m_vecLPEffAni.end(), m_vecLPEffAni[nIndex] );
	if( Itr == m_vecLPEffAni.end() )
	{
		Eff2Ut_ERR( "nIndex < m_vecLPEffAni.size() but std::find failed" );
		return -1;
	}

	AgcuEffAnimCreater::bDestroy( (*Itr) );
	m_vecLPEffAni.erase( Itr );
	return 0;
}

RwInt32 AgcdEffBase::bInsEffAnimVal( RwInt32 nlpEffAnimIndex, RwUInt32 dwTime, void* pIns )
{
	if( nlpEffAnimIndex >= static_cast<RwInt32>(m_vecLPEffAni.size()) )
	{
		Eff2Ut_ERR( "AgcdEffBase::bInsEffAnimVal failed : nlpEffAnimIndex not available" );
		return -1;
	}

	if( !m_vecLPEffAni[nlpEffAnimIndex] )
	{
		Eff2Ut_ERR( "AgcdEffBase::bInsEffAnimVal failed : m_vecLPEffAni[nlpEffAnimIndex] == NULL" );
		return -2;
	}

	if( m_vecLPEffAni[nlpEffAnimIndex]->bInsTVal( ( void* ) pIns, dwTime ) )
		return 0;
	else
	{
		Eff2Ut_ERR( "AgcdEffBase::bInsEffAnimVal failed : m_vecLPEffAni[nlpEffAnimIndex].bInsTVal failed" );
		return -3;
	}
}

RwInt32 AgcdEffBase::bDelEffAnimVal( RwInt32 nlpEffAnimIndex, RwUInt32 dwTime )
{
	if( nlpEffAnimIndex < static_cast<RwInt32>(m_vecLPEffAni.size()) ){
		if( m_vecLPEffAni[nlpEffAnimIndex] ){
			if( m_vecLPEffAni[nlpEffAnimIndex]->bDelTVal( dwTime ) )
				return 0;
			else{
				Eff2Ut_ERR( "AgcdEffBase::bDelEffAnimVal failed : m_vecLPEffAni[nlpEffAnimIndex].bDelTVal failed" );
				return -3;
			}
		}else{
			Eff2Ut_ERR( "AgcdEffBase::bDelEffAnimVal failed : m_vecLPEffAni[nlpEffAnimIndex] == NULL" );
			return -2;
		}
	}else{
		Eff2Ut_ERR( "AgcdEffBase::bDelEffAnimVal failed : nlpEffAnimIndex not available" );
		return -1;
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffBase::bEditEffAnim(RwInt32 nlpEffAnimIndex
							   , RwInt32 nIndexAtEffAnim
							   , RwUInt32 dwTime
							   , void* pEdit)
{
	if( nlpEffAnimIndex < static_cast<RwInt32>(m_vecLPEffAni.size()) )
	{
		if( m_vecLPEffAni[nlpEffAnimIndex] )
		{
			if( m_vecLPEffAni[nlpEffAnimIndex]->bSetTVal( nIndexAtEffAnim, pEdit, dwTime ) )
				return 0;
			else
			{
				Eff2Ut_ERR( "AgcdEffBase::bEditEffAnim failed : m_vecLPEffAni[nlpEffAnimIndex].bSetTVal failed" );
				return -3;
			}
		}
		
		else
		{
			Eff2Ut_ERR( "AgcdEffBase::bEditEffAnim failed : m_vecLPEffAni[nlpEffAnimIndex] == NULL" );
			return -2;
		}
	}

	else
	{
		Eff2Ut_ERR( "AgcdEffBase::bEditEffAnim failed : nlpEffAnimIndex not available" );
		return -1;
	}
}

//for tool
#ifdef USE_MFC
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffBase::bForTool_InsAnim( LPSTCREATEPARAM_EFFANIM lpCreateParam_EffAnim )
{
	if( bGetBaseType() == AgcdEffBase::E_EFFBASE_POSTFX )
	{

		if( lpCreateParam_EffAnim->m_eEffAnimType != AgcdEffAnim::E_EFFANIM_SCALE )
		{
			Eff2Ut_ERR( "only scale animation is available for AgcdEffPostFX" );
			return -1;
		}

		if( this->m_stVarSizeInfo.m_nNumOfAnim != 0 )
		{
			Eff2Ut_ERR( "only one animation is available for AgcdEffPostFX" );
			return -1;
		}

	}

	LPEFFANIM	pNew	= AgcuEffAnimCreater::bCreate( lpCreateParam_EffAnim );
	if( pNew )
	{
		m_vecLPEffAni.push_back( pNew );
	}
	
	else
	{
		Eff2Ut_ERR( "AgcdEffBase::bInsEffAnim failed : AgcuEffAnimCreater::bCreate failed" );
		return -1;
	}

	return (++m_stVarSizeInfo.m_nNumOfAnim);
}
#endif//USE_MFC

#ifdef USE_MFC
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffBase::bForTool_FindAnimIndex(LPEFFANIM pAnim)
{
	LPEffAniVecItr	it_curr = m_vecLPEffAni.begin();
	INT32	nIndex	= 0;
	for( ; it_curr != m_vecLPEffAni.end(); ++it_curr, ++nIndex )
	{
		if( pAnim == (*it_curr) )
			return nIndex;
	}
	return -1;
}
#endif//USE_MFC

#ifdef USE_MFC
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffBase::bForTool_DelAnim( RwInt32 nIndex )
{
	if( T_ISMINUS4( bDelEffAnim(nIndex) ) )
	{
		return -1;
	}

	return (--m_stVarSizeInfo.m_nNumOfAnim);
}
#endif//USE_MFC

#ifdef USE_MFC
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffBase::bForTool_Clone( AgcdEffBase* pEffBase )
{
	strcpy( m_szBaseTitle, pEffBase->m_szBaseTitle );
	m_dwDelay = pEffBase->m_dwDelay;
	m_dwLife = pEffBase->m_dwLife;
	m_eLoopOpt = pEffBase->m_eLoopOpt;
	m_dwBitFlags = pEffBase->m_dwBitFlags;

	for( int i=0; i<pEffBase->m_stVarSizeInfo.m_nNumOfAnim; ++i )
	{
		AgcdEffAnim*	pEffAnim = pEffBase->m_vecLPEffAni.at(i);

		STCREATEPARAM_EFFANIM	cparam_anim;
		switch( pEffAnim->bGetAnimType() )
		{
		case AgcdEffAnim::E_EFFANIM_COLOR:
			{
				LPEFFANIM_COLR pAnimColr =
					static_cast<LPEFFANIM_COLR>(pEffAnim);
				ASSERT( "kday" && pAnimColr );
				cparam_anim.bSetForColr( 
					pAnimColr->bGetFlag()
					, pAnimColr->bGetLife()
					, pAnimColr->bGetLoopOpt()
					, pAnimColr->m_stTblColr.bForTool_GetNum()
					, (VOID*)(&pAnimColr->m_stTblColr.bForTool_GetTbl().front())
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_TUTV:
			{
				LPEFFANIM_TUTV	pAnimUV =
					static_cast<LPEFFANIM_TUTV>(pEffAnim);
				ASSERT( "kday" && pAnimUV );
				cparam_anim.bSetForTuTv( 
					pAnimUV->bGetFlag()
					, pAnimUV->bGetLife()
					, pAnimUV->bGetLoopOpt()
					, pAnimUV->m_stTblRect.bForTool_GetNum()
					, (VOID*)(&pAnimUV->m_stTblRect.bForTool_GetTbl().front())
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_MISSILE:
			{
			AgcdEffAnim_Missile*	pAnimMissile =
					static_cast<AgcdEffAnim_Missile*>(pEffAnim);
				ASSERT( "kday" && pAnimMissile );
				cparam_anim.bSetForMissile(
					pAnimMissile->bGetFlag()
					, pAnimMissile->bGetLife()
					, pAnimMissile->bGetSpeed()
					, pAnimMissile->bGetAccel()
					, pAnimMissile->GetRotate()
					, pAnimMissile->GetRadius()
					, pAnimMissile->GetZigzagLength()
					, pAnimMissile->GetMinSpeed()
					, pAnimMissile->GetMaxSpeed()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_LINEAR:
			{
				AgcdEffAnim_Linear*	pAnimLinear =
					static_cast<AgcdEffAnim_Linear*>(pEffAnim);
				ASSERT( "kday" && pAnimLinear );
				cparam_anim.bSetForLinear(
					pAnimLinear->bGetFlag()
					, pAnimLinear->bGetLife()
					, pAnimLinear->bGetLoopOpt()
					, pAnimLinear->m_stTblPos.bForTool_GetNum()
					, (void*)&pAnimLinear->m_stTblPos.bForTool_GetTbl().front()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_REVOLUTION:
			{
				LPEFFANIM_REV	pAnimRev	= 
					static_cast<LPEFFANIM_REV>(pEffAnim);
				ASSERT( "kday" && pAnimRev );
				cparam_anim.bSetForRev(
					pAnimRev->bGetFlag()
					, pAnimRev->bGetLife()
					, pAnimRev->bGetLoopOpt()
					, pAnimRev->m_stTblRev.bForTool_GetNum()
					, pAnimRev->bGetRotAxis()
					, (void*)&pAnimRev->m_stTblRev.bForTool_GetTbl().front()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_ROTATION:
			{
				LPEFFANIM_ROT	pAnimRot =
					static_cast<LPEFFANIM_ROT>(pEffAnim);
				ASSERT( "kday" && pAnimRot );
				cparam_anim.bSetForRot(
					pAnimRot->bGetFlag()
					, pAnimRot->bGetLife()
					, pAnimRot->bGetLoopOpt()
					, pAnimRot->m_stTblDeg.bForTool_GetNum()
					, pAnimRot->bGetRotAxis()
					, (void*)&pAnimRot->m_stTblDeg.bForTool_GetTbl().front()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_RPSPLINE:
			{
				LPEFFANIM_RPSPLINE pAnimSpline =
					static_cast<LPEFFANIM_RPSPLINE>(pEffAnim);
				ASSERT( "kday" && pAnimSpline );
				cparam_anim.bSetForRpSpline(
					pAnimSpline->bGetFlag()
					, pAnimSpline->bGetLife()
					, pAnimSpline->bGetLoopOpt()
					, pAnimSpline->bGetSplineFName()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_RTANIM:
			{
				LPEFFANIM_RTANIM pAnimRtAnim =
					static_cast<LPEFFANIM_RTANIM>(pEffAnim);
				ASSERT( "kday" && pAnimRtAnim );
				cparam_anim.bSetForRtAnim(
					pAnimRtAnim->bGetFlag()
					, pAnimRtAnim->bGetLife()
					, pAnimRtAnim->bGetLoopOpt()
					, pAnimRtAnim->bGetAnimFName()
					);
			}break;

		case AgcdEffAnim::E_EFFANIM_SCALE:
			{
				LPEFFANIM_SCALE pAnimScale =
					static_cast<LPEFFANIM_SCALE>(pEffAnim);
				ASSERT( "kday" && pAnimScale );
				cparam_anim.bSetForScale(
					pAnimScale->bGetFlag()
					, pAnimScale->bGetLife()
					, pAnimScale->bGetLoopOpt()
					, pAnimScale->m_stTblScale.bForTool_GetNum()
					, (void*)&pAnimScale->m_stTblScale.bForTool_GetTbl().front()
					);
			}break;

		default:{
			ASSERT( "kday" && !"unknown animation type!" );
			return -1;
			}break;
		}

		if( T_ISMINUS4( bForTool_InsAnim(&cparam_anim) ) )
		{
			Eff2Ut_ERR("pEffBase->bForTool_InsAnim failed");
			return -1;
		}
	}
	ASSERT( "kday" && m_stVarSizeInfo.m_nNumOfAnim == pEffBase->m_stVarSizeInfo.m_nNumOfAnim );

	return 0;
}
#endif//USE_MFC

RwInt32 AgcdEffBase::tToFile(FILE* fp)
{
	//binary
	ASSERT(fp);

	int nSize = 
	fwrite( (LPCVOID)(&m_eEffBaseType), 1
		, sizeof(m_eEffBaseType)
		+ sizeof(m_szBaseTitle)
		+ sizeof(m_dwDelay)
		+ sizeof(m_dwLife)
		+ sizeof(m_eLoopOpt)
		+ sizeof(m_dwBitFlags)
		+ sizeof(m_stVarSizeInfo)
		, fp
		);
	return nSize;
}

RwInt32 AgcdEffBase::tFromFile(FILE* fp)
{
	//binary
	ASSERT(fp);

	int nSize = 
	fread( (LPVOID)(&m_szBaseTitle), 1
		, sizeof(m_szBaseTitle)
		+ sizeof(m_dwDelay)
		+ sizeof(m_dwLife)
		+ sizeof(m_eLoopOpt)
		+ sizeof(m_dwBitFlags)
		+ sizeof(m_stVarSizeInfo)
		, fp
		);
	
	return nSize;
}

RwInt32 AgcdEffBase::tToFileVariableData(FILE* fp)
{
	int nSize = 0;
	for( LPEffAniVecItr Itr = m_vecLPEffAni.begin(); Itr != m_vecLPEffAni.end(); ++Itr )
		nSize += (*Itr)->bToFile(fp);
	return nSize;
}

LPEFFANIM CreateEffAnim(AgcdEffAnim::E_EFFANIMTYPE eAnimtype)
{
	switch( eAnimtype )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:		return static_cast<LPEFFANIM>(new AgcdEffAnim_Colr);
	case AgcdEffAnim::E_EFFANIM_TUTV:		return static_cast<LPEFFANIM>(new AgcdEffAnim_TuTv);
	case AgcdEffAnim::E_EFFANIM_MISSILE:	return static_cast<LPEFFANIM>(new AgcdEffAnim_Missile);
	case AgcdEffAnim::E_EFFANIM_LINEAR:		return static_cast<LPEFFANIM>(new AgcdEffAnim_Linear);
	case AgcdEffAnim::E_EFFANIM_REVOLUTION:	return static_cast<LPEFFANIM>(new AgcdEffAnim_Rev);
	case AgcdEffAnim::E_EFFANIM_ROTATION:	return static_cast<LPEFFANIM>(new AgcdEffAnim_Rot);
	case AgcdEffAnim::E_EFFANIM_RPSPLINE:	return static_cast<LPEFFANIM>(new AgcdEffAnim_RpSpline);
	case AgcdEffAnim::E_EFFANIM_RTANIM:		return static_cast<LPEFFANIM>(new AgcdEffAnim_RtAnim);
	case AgcdEffAnim::E_EFFANIM_SCALE:		return static_cast<LPEFFANIM>(new AgcdEffAnim_Scale);
	default:
		ASSERT(!"unknown anim type");
		break;
	}

	return NULL;
};
RwInt32 AgcdEffBase::tFromFileVariableData(FILE* fp)
{
	ASSERT( m_stVarSizeInfo.m_nNumOfAnim < 20 );
	if( m_stVarSizeInfo.m_nNumOfAnim >= 20 )		return -1;

	int nSize = 0;
	for( int i=0; i<m_stVarSizeInfo.m_nNumOfAnim; ++i )
	{
		AgcdEffAnim::E_EFFANIMTYPE eAnimtype;
		nSize += fread(&eAnimtype, 1, sizeof(eAnimtype), fp);
		LPEFFANIM pAnim = CreateEffAnim(eAnimtype);
		ASSERT(pAnim);
		if( !pAnim )	return -1;

		int nCurSize = pAnim->bFromFile(fp);
		if( T_ISMINUS4( nCurSize ) )
		{
			ASSERT( !"pAnim->bToFile failed" );
			AgcuEffAnimCreater::bDestroy( pAnim );
			return -1;
		}
		nSize += nCurSize;

		m_vecLPEffAni.push_back( pAnim );
	}

	return nSize;
}

//---------------------------- AgcdEffRenderBase -----------------------------
AgcdEffRenderBase::AgcdEffRenderBase( AgcdEffBase::E_EFFBASETYPE eEffBaseType, E_EFFBLENDTYPE eBlendType ) : AgcdEffBase( eEffBaseType ),
 m_eBlendType( eBlendType )
{
	ASSERT( eEffBaseType == AgcdEffBase::E_EFFBASE_BOARD ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_PSYS ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_TAIL ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_OBJECT ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_TERRAINBOARD ||
			eEffBaseType == AgcdEffBase::E_EFFBASE_OBJECT  );

	Eff2Ut_ZEROBLOCK( m_v3dInitPos );

	RwMatrixSetIdentity( &m_matTrans );
	RwMatrixSetIdentity( &m_matRot );

	if( eEffBaseType == AgcdEffBase::E_EFFBASE_OBJECT )
	{
		m_cEffTexInfo.bSetIndex( -1 );
	}
}

void AgcdEffRenderBase::bSetInitPos( const RwV3d& v3Pos )	
{
	m_v3dInitPos = v3Pos;
	RwMatrixTranslate (&m_matTrans, &m_v3dInitPos, rwCOMBINEREPLACE);
}

void AgcdEffRenderBase::bSetInitAngle( const STANGLE& stAngle )	
{ 
	m_stInitAngle = stAngle;

	//E_EFFBASE_OBJECT 순서의 문제가 있군. ^^
	if( AgcdEffBase::E_EFFBASE_OBJECT == bGetBaseType() )
	{
		RwMatrixRotate( &m_matRot, PTRAXISWX, m_stInitAngle.m_fPitch, rwCOMBINEREPLACE );
		RwMatrixRotate( &m_matRot, PTRAXISWY, m_stInitAngle.m_fYaw, rwCOMBINEPOSTCONCAT );
		RwMatrixRotate( &m_matRot, PTRAXISWZ, m_stInitAngle.m_fRoll, rwCOMBINEPOSTCONCAT );
	}
	else
	{
		RwMatrixRotate( &m_matRot, PTRAXISWZ, m_stInitAngle.m_fRoll, rwCOMBINEREPLACE );
		RwMatrixRotate( &m_matRot, PTRAXISWY, m_stInitAngle.m_fYaw, rwCOMBINEPOSTCONCAT );
		RwMatrixRotate( &m_matRot, PTRAXISWX, m_stInitAngle.m_fPitch, rwCOMBINEPOSTCONCAT );
	}
}

void AgcdEffRenderBase::bSet_Renderstate( void )
{
	RwD3D9SetVertexShader( NULL );
	RwD3D9SetPixelShader( NULL );

	RwD3D9SetRenderState( D3DRS_LIGHTING, FALSE );
	RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE  );
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF , (void*) 10  );

	switch( m_eBlendType )
	{
	case EFFBLEND_ADD_TEXSTAGE:
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
		RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );

		RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE , (void*)FALSE  );
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)TRUE  );
		break;

	case EFFBLEND_ADD_ONE_ONE:
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDONE );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE );
		break;

	case EFFBLEND_ADD_SRCA_ONE:
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE );
		break;

	case EFFBLEND_ADD_SRCA_INVSRCA:
		break;

	case EFFBLEND_ADD_SRCC_INVSRCC:
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );
		break;

	case EFFBLEND_REVSUB_ONE_ONE:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDONE );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE );
		break;

	case EFFBLEND_REVSUB_SRCA_ONE:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );
		break;

	case EFFBLEND_REVSUB_SRCA_INVSRCA:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		break;

	case EFFBLEND_REVSUB_SRCC_INVSRCC:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );
		break;

	case EFFBLEND_SUB_ONE_ONE:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );
		break;

	case EFFBLEND_SUB_SRCA_ONE:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );
		break;

	case EFFBLEND_SUB_SRCA_INVSRCA:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		break;

	case EFFBLEND_SUB_SRCC_INVSRCC:
		RwD3D9SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );
		break;

	default:
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
		break;
	}
}

void AgcdEffRenderBase::bRestore_Renderstate( void )
{
	RwD3D9SetRenderState( D3DRS_LIGHTING, TRUE );

	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );

	RwRenderStateSet( rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0  );
	
	if( EFFBLEND_ADD_TEXSTAGE == m_eBlendType )
	{
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
		RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE , (void*)TRUE );
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE , (void*)FALSE );
	}
	else if( EFFBLEND_ADD_SRCA_INVSRCA != m_eBlendType )
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );	

		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA );
	}
}

RwInt32 AgcdEffRenderBase::tToFile(FILE* fp)
{
	RwInt32 nSize = fwrite( &m_eBlendType, 1, sizeof(m_eBlendType), fp );
	
	RwInt32 nTex = m_cEffTexInfo.bGetIndex();
	nSize += fwrite(&nTex, 1, sizeof(RwInt32), fp );
	nSize += fwrite(&m_v3dInitPos, 1, sizeof(m_v3dInitPos), fp );
	nSize += fwrite(&m_stInitAngle, 1, sizeof(m_stInitAngle), fp );
	return nSize;
};

RwInt32 AgcdEffRenderBase::tFromFile(FILE* fp)
{
	RwInt32 nSize = fread( &m_eBlendType, 1, sizeof(m_eBlendType), fp );
	RwInt32 nTex = 0;
	nSize += fread(&nTex, 1, sizeof(RwInt32), fp );
	m_cEffTexInfo.bSetIndex(nTex);
	nSize += fread(&m_v3dInitPos, 1, sizeof(m_v3dInitPos), fp );
	nSize += fread(&m_stInitAngle, 1, sizeof(m_stInitAngle), fp );

	this->bSetInitAngle(m_stInitAngle);
	this->bSetInitPos(m_v3dInitPos);
	return nSize;
};

#ifdef USE_MFC
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
INT32 AgcdEffRenderBase::bForTool_Clone( AgcdEffBase* pEffBase )
{
	AgcdEffRenderBase* pRenderBase = static_cast<AgcdEffRenderBase*>(pEffBase);
	ASSERT( "kday" && pEffBase->bIsRenderBase() );
	if( !pRenderBase )
	{
		return -1;
	}

	m_eBlendType = pRenderBase->m_eBlendType;
	//only copy texture index
	m_cEffTexInfo.bSetIndex( pRenderBase->m_cEffTexInfo.bGetIndex() );
	if( T_ISMINUS4( pRenderBase->m_cEffTexInfo.bGetIndex() ) )
	{
		ASSERT( "kday"
			&& pRenderBase->m_cEffTexInfo.bGetIndex() == -1
			&& bGetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT
			);		
	}

	m_v3dInitPos = pRenderBase->m_v3dInitPos;
	m_stInitAngle = pRenderBase->m_stInitAngle;
	m_matTrans = pRenderBase->m_matTrans;
	m_matRot = pRenderBase->m_matRot;

	return 0;
}
#endif//USE_MFC

//----------------------- AgcuEffBaseCreater -----------------------
AgcuEffBaseCreater::fptrCreate	AgcuEffBaseCreater::m_fptrCreate[AgcdEffBase::E_EFFBASE_NUM]	= {
	vCreate_Board	,
	vCreate_PSys	,
	vCreate_PSys_SimpleBlackHole	,
	vCreate_Tail	,
	vCreate_Object	,
	vCreate_Light	,
	vCreate_Sound	,
	vCreate_MFrm	,
	vCreate_TerrainBoard			,
	vCreate_PostFX	,
	vCreate_Camera	,
};

AgcdEffBase* AgcuEffBaseCreater::bCreate( AgcdEffBase::E_EFFBASETYPE eBaseType )
{
	if( AgcdEffBase::E_EFFBASE_BOARD <= eBaseType && eBaseType < AgcdEffBase::E_EFFBASE_NUM )
		return m_fptrCreate[eBaseType]();
	else
		return NULL;
}

void AgcuEffBaseCreater::bDestroy( AgcdEffBase*& prEffBase )
{
	if( prEffBase )
	{
		if( prEffBase->m_eEffBaseType < AgcdEffBase::E_EFFBASE_NUM && prEffBase->m_eEffBaseType >= 0 )
		{
			DEF_SAFEDELETE( prEffBase );
		}
		else
		{
			Eff2Ut_ERR( "AgcuEffBaseCreater::bDestroy failed : prEffBase->m_eEffBaseType == unknown" );
		}
	}
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_Board(void)
{
	return new AgcdEffBoard;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_PSys(void)
{
	return new AgcdEffParticleSystem;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_PSys_SimpleBlackHole(void)
{
	return new AgcdEffParticleSys_SimpleBlackHole;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_Tail(void)
{
	return new AgcdEffTail;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_Object(void)
{
	return new AgcdEffObj;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_Light(void)
{
	return new AgcdEffLight;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_Sound(void)
{
	return new AgcdEffSound;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_MFrm(void)
{
	return new AgcdEffMFrm;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_TerrainBoard(void)
{
	return new AgcdEffTerrainBoard;
}

AgcdEffBase* AgcuEffBaseCreater::vCreate_PostFX	(void)
{
	return new AgcdEffPostFX;
};

AgcdEffBase* AgcuEffBaseCreater::vCreate_Camera( void )
{
	return new AgcdEffCamera;
}
