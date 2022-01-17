// ToolOption.cpp: implementation of the CToolOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffTool.h"
#include "ToolOption.h"

#include "MainFrm.h"

#include "AgcdEffGlobal.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const	TCHAR* STRELBTNACT[CToolOption::e_lbtn_actnum]	= {
	_T("camrot"),
	_T("movefrmOnVirtualPlane"),
	_T("movefrmOnField"),
	_T("frmrot"),
};

const	TCHAR* STREFRM[CGlobalVar::e_frm_num] = {
	_T("blue"),
	_T("red"),
	_T("yeloow"),
	_T("clump"),
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolOption::CToolOption()
: m_bShowGrid(TRUE)
, m_bShowWorldAxis(TRUE)
, m_bShowWorldAxisCone(TRUE)
, m_bShowCameraAxis(TRUE)
, m_bShowField(FALSE)
, m_bShowFieldWire(FALSE)
, m_bShowFramesAxis(TRUE)
, m_bShowFrame(TRUE)
, m_bShowEffFrame(FALSE)

, m_bShowOnlyOneEff(TRUE)

, m_eLBtnAct(e_lbtn_camrot)
, STRELBTNACT( ::STRELBTNACT )
{
}

CToolOption::~CToolOption()
{
}

CToolOption& CToolOption::bGetInst(void)
{
	static CToolOption	inst;
	return inst;
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGlobalVar::CGlobalVar()
: m_bInitEngin(FALSE)
, STREFRM(::STREFRM)
, m_eFrmParent(e_frm_blue)
, m_eFrmTarget(e_frm_red)
, m_pClump(NULL)
{
	memset(m_szClump, 0, sizeof(m_szClump));
	strcpy(m_szClump, "");
}
CGlobalVar::~CGlobalVar(){};
CGlobalVar& CGlobalVar::bGetInst(void)
{
	static CGlobalVar	inst;
	return inst;
};

INT CGlobalVar::bOnEngineInitAccessory()
{
	vInitShowFrm();
	vInitShowClump();

	return 0;
}

INT CGlobalVar::bOnEngineDestroyAccessory()
{
	if( m_pClump )
	{		
		RwFrame*	pFrame = RpClumpGetFrame(m_pClump);
		if(pFrame)
		{
			RwFrameRemoveChild( pFrame );
		}
	}

	ShowFrmVecItr	it_curr		= m_showFrmContainer.begin();
	ShowFrmVecItr	it_last		= m_showFrmContainer.end();
	ShowFrm*			pShowFrm	= NULL;
	for( ; it_curr != it_last; ++it_curr )
	{
		pShowFrm	= (*it_curr);
		if( pShowFrm )
			DEF_SAFEDELETE(pShowFrm);
	}
	m_showFrmContainer.clear();

	return 0;
}


struct	stCreateShowFrmParam
{
	eFrmGeoType		geotype;
	RwUInt32		colr;
	RwV3d			pos;
};
INT CGlobalVar::vInitShowFrm()
{
	const stCreateShowFrmParam		param[] =
	{
		{ eShowFrm_box,		0xff0000ff,	{ 0.f, 0.f, 0.f }			},	//e_frm_blue
		{ eShowFrm_sphere,	0xffff0000, { 1050.f, 0.f, 1050.f }		},	//e_frm_red
		{ eShowFrm_sphere,	0xffffff00, { 1050.f, 0.f, -1050.f }	},	//e_frm_yeloow
									
		{ eShowFrm_box,		0xffffffff,	{ 1050.f, 50.f, -1050.f }	},	//e_frm_clump
	};

	ShowFrm*	pShowFrm	= NULL;
	for( int i=0; i<e_frm_num; ++i )
	{
		pShowFrm	= new ShowFrm( param[i].pos, param[i].geotype, param[i].colr );
		m_showFrmContainer.push_back( pShowFrm );
	}

	return 0;
}

INT CGlobalVar::vInitShowClump()
{
	return 0;
}

INT CGlobalVar::bRenderShowFrm()
{
	
	ShowFrmVecItr	it_curr		= m_showFrmContainer.begin();
	ShowFrmVecItr	it_last		= m_showFrmContainer.end();
	ShowFrm*			pShowFrm	= NULL;
	RwUInt32			flag		= rwIM3D_ALLOPAQUE;
	for( ; it_curr != it_last; ++it_curr )
	{
		pShowFrm	= (*it_curr);
		if( pShowFrm )
		{
			pShowFrm->bRender();
			if( CToolOption::bGetInst().bShowFramesAxis() )
			{
				extern MyEngine g_MyEngine;
				g_MyEngine.bGetPtrFrmAxis()->bRender( pShowFrm->bGetPtrLTM(), &flag );
			}
		}
	}
	
	return 0;
};

ShowFrm* CGlobalVar::bPickShowFrm(POINT ptMouse)
{
	CPickUser::GetInst().Make_Ray( NULL, NULL, CCamera::bGetInst().bGetPtrRwCam(), ptMouse );

	ShowFrmVecItr	it_curr		= m_showFrmContainer.begin();
	ShowFrmVecItr	it_last		= m_showFrmContainer.end();
	ShowFrm*			pShowFrm	= NULL;
	ShowFrm*			pr			= NULL;
	BOOL				bPicked		= FALSE;
	RwReal				dist		= 0.f;
	RwReal				mindist		= 0.f;
	for( ; it_curr != it_last; ++it_curr )
	{
		pShowFrm	= (*it_curr);
		if( pShowFrm )
		{
			if(pr)
			{
				if(pShowFrm->bPick( CPickUser::GetInst(), &dist ))
				{
					if( dist < mindist )
					{
						mindist = dist;
						pr	= pShowFrm;
					}
				}
			}
			else if(pShowFrm->bPick( CPickUser::GetInst(), &mindist ))
			{
				pr	= pShowFrm;
			}
		}
	}

	return pr;
};

INT CGlobalVar::bSetShowFrmHeight(CEffUt_HFMap<VTX_PNDT1>& heightmap)
{
	BOOL	bShowHeightMap	= (CToolOption::bGetInst().bShowField() | CToolOption::bGetInst().bShowFieldWire()) ? TRUE : FALSE;
	
	ShowFrmVecItr	it_curr		= m_showFrmContainer.begin();
	ShowFrmVecItr	it_last		= m_showFrmContainer.end();
	ShowFrm*			pShowFrm	= NULL;
	for( ; it_curr != it_last; ++it_curr )
	{
		pShowFrm	= (*it_curr);
		if( pShowFrm )
		{
			if( bShowHeightMap )
			{
				RwV3d*	pos = pShowFrm->bGetPtrPos();
				float height = heightmap.bGetHeight(pos->x, pos->z);
				if(pos->y != height)
				{
					RwV3d newpos = {pos->x, height, pos->z};
					pShowFrm->bMoveTo(newpos);
				}
			}
			else
			{
				RwV3d*	pos = pShowFrm->bGetPtrPos();
				if( pos->y != 0 )
				{
					RwV3d newpos = {pos->x, 0.f, pos->z};
					pShowFrm->bMoveTo(newpos);
				}
			}
		}
	}
	return 0;
};

INT CGlobalVar::bSetShowFrmScale(RwReal scale)
{
	ShowFrmVecItr	it_curr		= m_showFrmContainer.begin();
	ShowFrmVecItr	it_last		= m_showFrmContainer.end();
	ShowFrm*			pShowFrm	= NULL;
	for( ; it_curr != it_last; ++it_curr )
		if( pShowFrm = (*it_curr) )
			pShowFrm->bSetScale(scale);

	return 0;
};

INT CGlobalVar::bUpdateUseInfo(RwUInt32 ulEffID)
{
	m_stEffUseInfo.m_ulEffID	= ulEffID;
	m_stEffUseInfo.m_pFrmParent	= bGetPtrFrame(m_eFrmParent);
	m_stEffUseInfo.m_pFrmTarget	= bGetPtrFrame(m_eFrmTarget);
	m_stEffUseInfo.m_stMissileTargetInfo.m_pFrmTarget	= bGetPtrFrame(m_eFrmTarget);

	if( DEF_FLAG_CHK(m_stEffUseInfo.m_ulFlags, stEffUseInfo::E_FLAG_EMITER_WITH_CLUMP) )
	{
		//TODO : clump 를 읽으시오..
		if( m_pClump )
		{
			if(RwFrameGetParent(RpClumpGetFrame(m_pClump)))
				RwFrameRemoveChild(RpClumpGetFrame(m_pClump));
			Eff2Ut_SAFE_DESTROY_CLUMP(m_pClump);
		}

		RwChar	szFull[MAX_PATH]	= "CHARACTER\\";
		strcat(szFull, m_szClump);

		m_pClump = 
		AgcdEffGlobal::bGetInst().bGetPtrResoruceLoader()->LoadClump(
			szFull
			, NULL
			, NULL
			, rwFILTERLINEAR
			, NULL );

		if( m_pClump )
		{
			if( m_showFrmContainer.at(e_frm_clump)->bGetPtrFrm() && RpClumpGetFrame( m_pClump ) )
				RwFrameAddChild( m_showFrmContainer.at(e_frm_clump)->bGetPtrFrm(), RpClumpGetFrame( m_pClump ) );

			m_stEffUseInfo.m_pClumpEmiter	= m_pClump;
		}
	}
	else
		m_stEffUseInfo.m_pClumpEmiter	= NULL;

	return 0;
};

//////////////////////////////////////////////////////////////////////
// namespace NSUtFn
//////////////////////////////////////////////////////////////////////
namespace NSUtFn //NameSpaceUtilFunction
{
	CSelectedInfo::CSelectedInfo()
		: m_lpCurrSelectedEffSet(NULL)
		, m_lpCurrSelectedEffBase(NULL)
		, m_lpCurrSelectedEffAnim(NULL)
		, m_pCXTPGI_Tex(NULL)
		, m_pCXTPGI_Mask(NULL)
		, m_pCXTPGI_Clump(NULL)
		, m_pCXTPGI_Spline(NULL)
	{
		Eff2Ut_ZEROBLOCK(m_szTex);
		Eff2Ut_ZEROBLOCK(m_szMask);
	};
	CSelectedInfo::~CSelectedInfo()
	{
	};

	CSelectedInfo& CSelectedInfo::bGetInst()
	{
		static CSelectedInfo inst;
		return inst;
	};

	void CSelectedInfo::bOnSelEffSet(AgcdEffSet* pEffSet)
	{
		m_lpCurrSelectedEffSet = pEffSet;
		m_lpCurrSelectedEffBase = NULL;
		m_lpCurrSelectedEffAnim = NULL;
		
		Eff2Ut_ZEROBLOCK(m_szTex);
		Eff2Ut_ZEROBLOCK(m_szMask);

		m_pCXTPGI_Tex = NULL;
		m_pCXTPGI_Mask = NULL;
		m_pCXTPGI_Clump = NULL;
	};
	void CSelectedInfo::bOnSelEffBase(AgcdEffBase* pEffBase)
	{	
		Eff2Ut_ZEROBLOCK(m_szTex);
		Eff2Ut_ZEROBLOCK(m_szMask);
		m_pCXTPGI_Tex = NULL;
		m_pCXTPGI_Mask = NULL;
		m_pCXTPGI_Clump = NULL;

		m_lpCurrSelectedEffBase = pEffBase;
		m_lpCurrSelectedEffAnim = NULL;
		AgcdEffRenderBase*	pRenderBase = dynamic_cast<AgcdEffRenderBase*>(pEffBase);
		if( pRenderBase ){
			if( dynamic_cast<AgcdEffObj*>(pEffBase) )
				return;
				
			if( !pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex )
			{
				ErrToWnd( "pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex == NULL" );
				m_lpCurrSelectedEffBase = NULL;
				return;
			}
			strcpy( m_szTex, pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szTex );
			strcpy( m_szMask, pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szMask );
		}
	};
	void CSelectedInfo::bOnSelEffAnim(AgcdEffAnim* pEffAnim)
	{
		m_lpCurrSelectedEffAnim = pEffAnim;
	};

	//etc	
	void CSelectedInfo::bOnAddCTXPGI_TexMask( CXTPGI_FileName* pCXTPGI_Tex, CXTPGI_FileName* pCXTPGI_Mask )
	{
		m_pCXTPGI_Tex = pCXTPGI_Tex;
		m_pCXTPGI_Mask = pCXTPGI_Mask;
	};
	void CSelectedInfo::bOnAddCTXPGI_Clump( CXTPGI_FileName* pCXTPGI_Clump )
	{
		m_pCXTPGI_Clump = pCXTPGI_Clump;
	};
	BOOL CSelectedInfo::bFind_CXTPGI_TexMask( CXTPGI_FileName* pCXTPGI_Tex )		
	{
		if( pCXTPGI_Tex == m_pCXTPGI_Tex ||
			pCXTPGI_Tex == m_pCXTPGI_Mask )
			return TRUE;
		return FALSE;
	};
	BOOL CSelectedInfo::bFind_CXTPGI_TexClump( CXTPGI_FileName* pCXTPGI_Clump )
	{
		if( pCXTPGI_Clump == m_pCXTPGI_Clump )
			return TRUE;
		return FALSE;
	};
	BOOL CSelectedInfo::bFind_CXTPGI_Spline( CXTPGI_FileName* pCXTPGI_Spline )
	{
		if( pCXTPGI_Spline == m_pCXTPGI_Spline )
			return TRUE;
		return FALSE;
	};

	//reset
	void CSelectedInfo::bOnChangedSpline(void)
	{
		if( !m_lpCurrSelectedEffSet ||
			!m_lpCurrSelectedEffBase ||
			!dynamic_cast<AgcdEffRenderBase*>(m_lpCurrSelectedEffBase) ||
			!dynamic_cast<AgcdEffAnim_RpSpline*>(m_lpCurrSelectedEffAnim)
			)
		{
			ErrToWnd("CSelectedInfo::bOnChangedSpline Error!");
			return;
		}

		AgcdEffAnim_RpSpline* pEffAnimSpline = 
		dynamic_cast<AgcdEffAnim_RpSpline*>(m_lpCurrSelectedEffAnim);
		pEffAnimSpline->bSetSplineFName(pEffAnimSpline->m_szSplineFName);
	};
	void CSelectedInfo::bOnChangedClump(void)
	{
		if( !m_lpCurrSelectedEffSet ||
			!m_lpCurrSelectedEffBase ||
			!dynamic_cast<AgcdEffRenderBase*>(m_lpCurrSelectedEffBase)
			)
		{
			ErrToWnd("CSelectedInfo::bOnChangedClump Error!");
			return;
		}

		AgcdEffObj*	pEffBaseObj = 
		dynamic_cast<AgcdEffObj*>(m_lpCurrSelectedEffBase);
		if( !pEffBaseObj )
		{
			ErrToWnd("dynamic_cast<AgcdEffObj*>(m_lpCurrSelectedEffBase) failed!");
			return;
		}

		pEffBaseObj->bSetClumpName(pEffBaseObj->m_szClumpFName);

	};
	void CSelectedInfo::bOnChangedTex(void)
	{
		if( !m_lpCurrSelectedEffSet ||
			!m_lpCurrSelectedEffBase ||
			!dynamic_cast<AgcdEffRenderBase*>(m_lpCurrSelectedEffBase)
			)
		{
			ErrToWnd("CSelectedInfo::bOnChangedTex Error!");
			return;
		}

		AgcdEffRenderBase*	pRenderBase = 
		dynamic_cast<AgcdEffRenderBase*>(m_lpCurrSelectedEffBase);

		if( !pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex )
		{
			ErrToWnd("NULL == pRenderBase->m_cEffTexInfo.m_lpAgcdEffTex");
			return;
		}
		//이펙트 셋에 해당 택스쳐가 있는지 확인..
		{
			INT nTexIndex	= 
			m_lpCurrSelectedEffSet->bForTool_FindEffTex( m_szTex, m_szMask );

			if( nTexIndex == -1 )
			{
				INT	nNewIndex	=
				m_lpCurrSelectedEffSet->bForTool_InsEffEffTex(m_szTex, m_szMask);
				if( T_ISMINUS4(nNewIndex) )
				{
					ErrToWnd("m_lpCurrSelectedEffSet->bForTool_InsEffEffTex failed");
					return;
				}

				INT	ir =
				m_lpCurrSelectedEffSet->bForTool_ChangeBaseTexIndex( 
					pRenderBase
					, pRenderBase->m_cEffTexInfo.m_nIndex
					, nNewIndex
					);
				if( T_ISMINUS4(ir) )
				{
					ErrToWnd("m_lpCurrSelectedEffSet->bForTool_ChangeBaseTexIndex failed");
					return;
				}
			}
			else
			{			
				INT	ir =
				m_lpCurrSelectedEffSet->bForTool_ChangeBaseTexIndex( 
					pRenderBase
					, pRenderBase->m_cEffTexInfo.m_nIndex
					, nTexIndex
					);
				if( T_ISMINUS4(ir) )
				{
					ErrToWnd("m_lpCurrSelectedEffSet->bForTool_ChangeBaseTexIndex failed");
					return;
				}
			}
		}
	};

	void CSelectedInfo::bDelAnimItem(RwUInt32 ulTime)
	{
		if( !m_lpCurrSelectedEffAnim || !m_lpCurrSelectedEffBase )
		{
			ErrToWnd("!m_lpCurrSelectedEffAnim || !m_lpCurrSelectedEffBase");
			return;
		}

		if( !m_lpCurrSelectedEffAnim->bDelTVal(ulTime) )
		{
			ErrToWnd("m_lpCurrSelectedEffAnim->bDelTVal(ulTime) failed!");
			return;
		}

		CMainFrame::bGetThis()->m_dlgTreeEffSet.bRefreshSelect();
		
	};

	void CSelectedInfo::bDelEffBaseDpnd(INT delBaseDpndIndex)
	{
		CMainFrame::bGetThis()->m_dlgTreeEffSet.bDelEffBaseDpnd( delBaseDpndIndex );
	};
};