#include "stdafx.h"
#include "../EffTool.h"
#include "DlgStaticPropGrid.h"
#include "../MainFrm.h"
#include "../ToolOption.h"

#include "../XTPPropertyGridItemCustum/XTPGI_UINT.h"
#include "../XTPPropertyGridItemCustum/XTPPropertyGridItemEnum.h"
#include "../XTPPropertyGridItemCustum/XTPGI_EffSet.h"
#include "../XTPPropertyGridItemCustum/XTPGI_Board.h"
#include "../XTPPropertyGridItemCustum/XTPGI_Light.h"
#include "../XTPPropertyGridItemCustum/XTPGI_MFrm.h"
#include "../XTPPropertyGridItemCustum/XTPGI_Obj.h"
#include "../XTPPropertyGridItemCustum/XTPGI_Sound.h"
#include "../XTPPropertyGridItemCustum/XTPGI_Tail.h"
#include "../XTPPropertyGridItemCustum/XTPGI_PSys.h"
#include "../XTPPropertyGridItemCustum/XTPGI_PSysSBH.h"
#include "../XTPPropertyGridItemCustum/XTPGI_TerrainBoard.h"
#include "../XTPPropertyGridItemCustum/XTPGI_AColr.h"

#include "AgcdEffSet.h"
#include "AgcdEffBoard.h"
#include "AgcdEffLight.h"
#include "AgcdEffMFrm.h"
#include "AgcdEffObj.h"
#include "AgcdEffParticleSystem.h"
#include "AgcdEffSound.h"
#include "AgcdEffTail.h"
#include "AgcdEffTerrainBoard.h"
#include "AgcdEffAnim.h"
#include "AgcdEffGlobal.h"

#include "AcuMathFunc.h"
USING_ACUMATH;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------- CDlgStaticPropGrid -----------------------------
CDlgStaticPropGrid::CDlgStaticPropGrid(CWnd* pParent)
: CDlgStatic(pParent),m_pGrid(NULL)
{
}

BEGIN_MESSAGE_MAP(CDlgStaticPropGrid, CDlgStatic)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


BOOL CDlgStaticPropGrid::OnInitDialog()
{
	CDlgStatic::OnInitDialog();

	CRect	rc;
	CDlgStatic::m_ctrlStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	DEF_SAFEDELETE(m_pGrid);
	m_pGrid	= new CXTPPropertyGrid;

	if( !m_pGrid->Create(rc, this, IDC_STATIC_RECT) )
		return FALSE;

	m_ctrlStatic.ShowWindow(SW_HIDE);
	t1stInitGrid();

	return (m_bInit = TRUE);
}

void CDlgStaticPropGrid::OnSize(UINT nType, int cx, int cy)
{
	CDlgStatic::OnSize(nType, cx, cy);

	if( m_bInit )
	{
		CRect	rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_pGrid->MoveWindow( &rc );
	}
};

LRESULT CDlgStaticPropGrid::OnGridNotify(WPARAM wp, LPARAM lp)
{
	switch( wp )
	{
	case XTP_PGN_SORTORDER_CHANGED:
	case XTP_PGN_SELECTION_CHANGED:
	case XTP_PGN_ITEMVALUE_CHANGED:
	case XTP_PGN_EDIT_CHANGED:
	case XTP_PGN_INPLACEBUTTONDOWN:
	case XTP_PGN_DRAWITEM:
	case XTP_PGS_OWNERDRAW:
		break;
	}

	return 0;
}

//------------------------- CDlgGridEffSet -----------------------------
CDlgGridEffSet::CDlgGridEffSet(CWnd* pParent) : CDlgStaticPropGrid(pParent), m_ulEffSetID(0LU)
{
};

BEGIN_MESSAGE_MAP(CDlgGridEffSet, CDlgStaticPropGrid)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()

BOOL CDlgGridEffSet::t1stInitGrid()
{
	bSetBlank();
	return TRUE;
};

//------------------------- CDlgGridEffSet -----------------------------
LRESULT CDlgGridEffSet::OnGridNotify(WPARAM wp, LPARAM lp)
{
	ToWnd("-------------------CDlgGridEffSet::OnGridNotify--------------------");

	switch( wp )
	{
	case XTP_PGN_SORTORDER_CHANGED:
		break;
	case XTP_PGN_SELECTION_CHANGED:
		ToWnd("XTP_PGN_SELECTION_CHANGED");
		break;
	case XTP_PGN_ITEMVALUE_CHANGED:
		{
			ToWnd("XTP_PGN_ITEMVALUE_CHANGED");
			if( !AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2() )
				return 0;
			
			LPEFFSET pEffSet = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet(m_ulEffSetID );
			if( !pEffSet )
			{
				ErrToWnd("AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet failed!");
				return 0;
			}

			AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( m_ulEffSetID );
			if( !CMainFrame::bGetThis()->m_dlgTreeEffSet.bOnEditEffSet( m_ulEffSetID ) )
				ToWnd( Eff2Ut_FmtMsg("[id : %d, title : %s] 값이 변경되었습니다.", m_ulEffSetID , pEffSet->bGetTitle() ) );
			else
				ErrToWnd( Eff2Ut_FmtMsg("[id : %d, title : %s] 값이 변경되었으나, 저장할 방법이 없습니다.", m_ulEffSetID, pEffSet->bGetTitle() )  );

			CXTPPropertyGridItem*	host = (CXTPPropertyGridItem*)lp;
			if( dynamic_cast<CXTPGI_FileName*>(host) )
			{
				if( NSUtFn::CSelectedInfo::bGetInst().bFind_CXTPGI_TexMask( dynamic_cast<CXTPGI_FileName*>(host) )  )
					NSUtFn::CSelectedInfo::bGetInst().bOnChangedTex();
				else if( NSUtFn::CSelectedInfo::bGetInst().bFind_CXTPGI_TexClump( dynamic_cast<CXTPGI_FileName*>(host) ) )
					NSUtFn::CSelectedInfo::bGetInst().bOnChangedClump();
				else if( NSUtFn::CSelectedInfo::bGetInst().bFind_CXTPGI_Spline( dynamic_cast<CXTPGI_FileName*>(host) ) )
					NSUtFn::CSelectedInfo::bGetInst().bOnChangedSpline();
			}
		}
		break;
	case XTP_PGN_EDIT_CHANGED:
		ToWnd("XTP_PGN_EDIT_CHANGED");
		break;
	case XTP_PGN_INPLACEBUTTONDOWN:
		ToWnd("XTP_PGN_INPLACEBUTTONDOWN");
		break;
	case XTP_PGN_DRAWITEM:
		ToWnd("XTP_PGN_DRAWITEM");
		break;
	case XTP_PGS_OWNERDRAW:
		ToWnd("XTP_PGS_OWNERDRAW");
		break;
	case XTP_PGN_ITEMVALUE_DELANIMITEM_BTN:
		{
			ToWnd("XTP_PGN_ITEMVALUE_DELANIMITEM_BTN");
			CXTPPropertyGridItem*	pItem = (CXTPPropertyGridItem*)(lp);
			ASSERT( pItem && "impossible");
			if( pItem )
			{			
				ToWnd( Eff2Ut_FmtMsg("item index : %d", atoi( pItem->GetValue() )) );
				NSUtFn::CSelectedInfo::bGetInst().bDelAnimItem( atol(pItem->GetValue()) );
			}

		}
		break;
	case XTP_PGN_ITEMVALUE_CHANGED_TIME:
		CMainFrame::bGetThis()->m_dlgTreeEffSet.bRefreshSelect();
		break;
	}
	return 0;
}

BOOL CDlgGridEffSet::vIsVisible()
{
	return ShowWindow(SW_HIDE);
};

INT CDlgGridEffSet::vNewGrid()
{
	CRect	rc;
	CDlgStatic::m_ctrlStatic.GetWindowRect(&rc);
	ScreenToClient(&rc);
	
	DEF_SAFEDELETE(m_pGrid);
	m_pGrid	= new CXTPPropertyGrid;
	if( !m_pGrid ){
		ErrToWnd( "!m_pGrid" );
		return -1;
	}

	if( !m_pGrid->Create(rc, this, IDC_STATIC_RECT) ){
		DEF_SAFEDELETE(m_pGrid);
		ErrToWnd( "!m_pGrid->Create" );
		return -1;
	}

	return 0;
};

INT CDlgGridEffSet::bSetBlank(void)
{
	if( !m_bInit ){
		ToWnd("ProperyGrid 창이 초기화되어있지 안습니다!");
		return 0;
	}
	BOOL	bVisible = vIsVisible();
	if( T_ISMINUS4( vNewGrid() ) ){
		return -1;
	}
	m_pGrid->AddCategory( _T( "선택된 이펙트가 없습니다." ) );
	if( bVisible )
		ShowWindow(SW_SHOW);
	return 0;
};

INT CDlgGridEffSet::bSetForEffSet(AgcdEffSet* pEffSet)
{
	if( !m_bInit ){
		ToWnd("ProperyGrid 창이 초기화되어있지 안습니다!");
		return 0;
	}
	if( !pEffSet ){
		ErrToWnd( "!pEffSet" );
		return -1;
	}

	BOOL	bVisible = vIsVisible();
	if( T_ISMINUS4( vNewGrid() ) ){
		return -1;
	}

	m_ulEffSetID	= pEffSet->bGetID();

	CXTPPropertyGridItem* pCategory	= m_pGrid->AddCategory( _T( "Category" ) );
	CXTPPropertyGridItem* pSet	= pCategory->AddChildItem( new CXTPGI_EffSet(_T("effect set"), pEffSet) );
	pCategory->Expand();
	pSet->Expand();

	
	if( bVisible )
		ShowWindow(SW_SHOW);
	return 0;
};

INT CDlgGridEffSet::bSetForEffBase(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase)
{
	if( !m_bInit )
	{
		ToWnd("ProperyGrid 창이 초기화되어있지 안습니다!");
		return 0;
	}
	if( !pEffSet )
	{
		ErrToWnd( "!pEffSet" );
		return -1;
	}
	if( !pEffBase )
	{
		ErrToWnd( "!pEffBase" );
		return -1;
	}

	BOOL	bVisible = vIsVisible();
	if( T_ISMINUS4( vNewGrid() ) ){
		return -1;
	}

	m_ulEffSetID	= pEffSet->bGetID();

	CXTPPropertyGridItem* pBase	= m_pGrid->AddCategory( _T( "Category" ) );

	CString	strCaption;
	LPCTSTR	SZTYPE[AgcdEffBase::E_EFFBASE_NUM] = {
		"board",
		"particle",
		"particleSBH",
		"tail",
		"object",
		"light",
		"sound",
		"movingframe",
		"terrainBaord",
	};

	int index;
	for( index = 0; index<pEffSet->m_stVarSizeInfo.m_nNumOfBase; ++index )
		if( pEffBase == pEffSet->bGetPtrEffBase(index) )
			break;
	if( index >= pEffSet->m_stVarSizeInfo.m_nNumOfBase )
	{
		ErrToWnd("이현상을 알려주세요... -문경삼-");
		return -1;
	}

	strCaption.Format("%d:%s", index, SZTYPE[pEffBase->bGetBaseType()]);
	ToWnd(strCaption);

	if( dynamic_cast<AgcdEffBoard*>(pEffBase) )			pBase->AddChildItem( new CXTPGI_Board(strCaption, dynamic_cast<AgcdEffBoard*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffLight*>(pEffBase) )	pBase->AddChildItem( new CXTPGI_Light(strCaption, dynamic_cast<AgcdEffLight*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffMFrm*>(pEffBase) )		pBase->AddChildItem( new CXTPGI_MFrm(strCaption, dynamic_cast<AgcdEffMFrm*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffObj*>(pEffBase) )		pBase->AddChildItem( new CXTPGI_Obj(strCaption, dynamic_cast<AgcdEffObj*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffSound*>(pEffBase) )	pBase->AddChildItem( new CXTPGI_Sound(strCaption, dynamic_cast<AgcdEffSound*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffTail*>(pEffBase) )		pBase->AddChildItem( new CXTPGI_Tail(strCaption, dynamic_cast<AgcdEffTail*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffParticleSystem*>(pEffBase) )	pBase->AddChildItem( new CXTPGI_PSys(strCaption, dynamic_cast<AgcdEffParticleSystem*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase) )	pBase->AddChildItem( new CXTPGI_PSysSBH(strCaption, dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase)) )->Expand();
	else if( dynamic_cast<AgcdEffTerrainBoard*>(pEffBase) )		pBase->AddChildItem( new CXTPGI_TerrainBoard(strCaption, dynamic_cast<AgcdEffTerrainBoard*>(pEffBase)) )->Expand();

	pBase->Expand();

	if( bVisible )
		ShowWindow( SW_SHOW );

	return 0;
};

INT CDlgGridEffSet::bSetForEffAnim(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase, AgcdEffAnim* pEffAnim)
{
	if( !m_bInit )
	{
		ToWnd("ProperyGrid 창이 초기화되어있지 안습니다!");
		return 0;
	}
	if( !pEffSet )
	{
		ErrToWnd( "!pEffSet" );
		return -1;
	}
	if( !pEffBase )
	{
		ErrToWnd( "!pEffBase" );
		return -1;
	}
	if( !pEffAnim )
	{
		ErrToWnd( "!pEffAnim" );
		return -1;
	}

	BOOL	bVisible = vIsVisible();
	if( T_ISMINUS4( vNewGrid() ) ){
		return -1;
	}

	m_ulEffSetID	= pEffSet->bGetID();

	CXTPPropertyGridItem* pAnim	= NULL;
	CXTPPropertyGridItem* pCategory	= m_pGrid->AddCategory( _T( "Category" ) );

	CString	strCaption;

	int index = 0;
	for( ; index<pEffBase->m_stVarSizeInfo.m_nNumOfAnim; ++index )	{
		if( pEffAnim == pEffBase->bGetPtrEffAnim(index) )
			break;
	}

	if( index >= pEffBase->m_stVarSizeInfo.m_nNumOfAnim )
	{
		ErrToWnd("이현상을 알려주세요... -문경삼-");
		return -1;
	}

	strCaption.Format("%d:%s", index, EFFANIMTYPE_NAME[pEffAnim->bGetAnimType()]);
	ToWnd(strCaption);

	switch( pEffAnim->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:		pAnim = pCategory->AddChildItem( new CXTPGI_AColr(strCaption, dynamic_cast<AgcdEffAnim_Colr*>(pEffAnim)) );			break;
	case AgcdEffAnim::E_EFFANIM_TUTV:		pAnim = pCategory->AddChildItem( new CXTPGI_ATuTv(strCaption, dynamic_cast<AgcdEffAnim_TuTv*>(pEffAnim)) );			break;
	case AgcdEffAnim::E_EFFANIM_MISSILE:	pAnim = pCategory->AddChildItem( new CXTPGI_AMissile(strCaption, dynamic_cast<AgcdEffAnim_Missile*>(pEffAnim)) );	break;
	case AgcdEffAnim::E_EFFANIM_LINEAR:		pAnim = pCategory->AddChildItem( new CXTPGI_ALinear(strCaption, dynamic_cast<AgcdEffAnim_Linear*>(pEffAnim)) );		break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION:	pAnim = pCategory->AddChildItem( new CXTPGI_ARev(strCaption, dynamic_cast<AgcdEffAnim_Rev*>(pEffAnim)) );			break;
	case AgcdEffAnim::E_EFFANIM_ROTATION:	pAnim = pCategory->AddChildItem( new CXTPGI_ARot(strCaption, dynamic_cast<AgcdEffAnim_Rot*>(pEffAnim)) );			break;
	case AgcdEffAnim::E_EFFANIM_RPSPLINE:	pAnim = pCategory->AddChildItem( new CXTPGI_ARpSpline(strCaption, dynamic_cast<AgcdEffAnim_RpSpline*>(pEffAnim)) );	break;
	case AgcdEffAnim::E_EFFANIM_RTANIM:		pAnim = pCategory->AddChildItem( new CXTPGI_ARtAnim(strCaption, dynamic_cast<AgcdEffAnim_RtAnim*>(pEffAnim)) );		break;
	case AgcdEffAnim::E_EFFANIM_SCALE:		pAnim = pCategory->AddChildItem( new CXTPGI_AScale(strCaption, dynamic_cast<AgcdEffAnim_Scale*>(pEffAnim)) );		break;
	}
	
	pCategory->Expand();
	pAnim->Expand();
	pAnim->SetReadOnly(TRUE);

	if( bVisible )
		ShowWindow(SW_SHOW);

	return 0;
};

//-------------------------- CDlgGridToolOption --------------------------
CDlgGridToolOption::CDlgGridToolOption(CWnd* pParent)
: CDlgStaticPropGrid(pParent)
{
};

BEGIN_MESSAGE_MAP(CDlgGridToolOption, CDlgStaticPropGrid)
	//{{AFX_MSG_MAP(CDlgGridToolOption)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgGridToolOption::t1stInitGrid()
{
	//Tool View
	CXTPPropertyGridItem* pCategoryToolView = m_pGrid->AddCategory( _T( "Tool View : accessary...." ) );
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowGrid			(), CToolOption::bGetInst().bShowGrid			(),&CToolOption::bGetInst().m_bShowGrid			));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowWorldAxis	(), CToolOption::bGetInst().bShowWorldAxis		(),&CToolOption::bGetInst().m_bShowWorldAxis	));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowWorldAxisCone(), CToolOption::bGetInst().bShowWorldAxisCone	(),&CToolOption::bGetInst().m_bShowWorldAxisCone));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowCameraAxis	(), CToolOption::bGetInst().bShowCameraAxis		(),&CToolOption::bGetInst().m_bShowCameraAxis	));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowField		(), CToolOption::bGetInst().bShowField			(),&CToolOption::bGetInst().m_bShowField		));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowFieldWire	(), CToolOption::bGetInst().bShowFieldWire		(),&CToolOption::bGetInst().m_bShowFieldWire	));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowFramesAxis	(), CToolOption::bGetInst().bShowFramesAxis		(),&CToolOption::bGetInst().m_bShowFramesAxis	));
	pCategoryToolView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowFrame		(), CToolOption::bGetInst().bShowFrame			(),&CToolOption::bGetInst().m_bShowFrame		));
	pCategoryToolView->Expand();

	//Eff View
	CXTPPropertyGridItem* pCategoryEffView = m_pGrid->AddCategory( _T( "Eff View" ) );
	pCategoryEffView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowEffFrame		(), CToolOption::bGetInst().bShowEffFrame		(),&CToolOption::bGetInst().m_bShowEffFrame		));
	pCategoryEffView->AddChildItem( new CXTPPropertyGridItemBool( CToolOption::bGetInst().bCaptionShowOnlyOneEff	(), CToolOption::bGetInst().bShowOnlyOneEff		(),&CToolOption::bGetInst().m_bShowOnlyOneEff	));
	pCategoryEffView->Expand();

	//Eff flags
	CXTPPropertyGridItem* pCategoryEffFlags = m_pGrid->AddCategory( _T( "Eff Flags" ) );
	pCategoryEffFlags->AddChildItem( new CXTPGI_FlagBool( AgcdEffGlobal::bGetInst().bFoorTool_GetPtrFlag(),E_GFLAG_TONEDOWN, _T("tone down") ));
	pCategoryEffFlags->AddChildItem( new CXTPGI_FlagBool( AgcdEffGlobal::bGetInst().bFoorTool_GetPtrFlag(),E_GFLAG_SHOWWIRE, _T("eff wire") ));
	pCategoryEffFlags->AddChildItem( new CXTPGI_FlagBool( AgcdEffGlobal::bGetInst().bFoorTool_GetPtrFlag(),E_GFLAG_SHOWBOX, _T("eff box") ));
	pCategoryEffFlags->AddChildItem( new CXTPGI_FlagBool( AgcdEffGlobal::bGetInst().bFoorTool_GetPtrFlag(),E_GFLAG_SHOWSPHERE, _T("eff sphere") ));
	pCategoryEffFlags->AddChildItem( new CXTPGI_FlagBool( AgcdEffGlobal::bGetInst().bFoorTool_GetPtrFlag(),E_GFLAG_CALCBOUND, _T("auto calc bound") ));
	pCategoryEffFlags->Expand();

	//LButton Act
	CXTPPropertyGridItem* pCategoryLBtnAct = m_pGrid->AddCategory( _T( "LButton Act" ) );
	pCategoryLBtnAct->AddChildItem( new CXTPGI_Enum<CToolOption::ELBTNACT>( CToolOption::bGetInst().bCaptionLBtnAct(),
																			CToolOption::e_lbtn_actnum,
																			CToolOption::bGetInst().bGetLBtnAct(),
																			&CToolOption::bGetInst().m_eLBtnAct,
																			CToolOption::bGetInst().STRELBTNACT	) );
	pCategoryLBtnAct->Expand();

	bSetUseEffSetInfo( CGlobalVar::bGetInst().m_stEffUseInfo );

	return TRUE;
};

INT CDlgGridToolOption::bSetUseEffSetInfo(STEFFUSEINFO& stUseEffSet)
{
	CXTPPropertyGridItem* pCategory = m_pGrid->AddCategory( _T( "stUseEffSetInfo" ) );
		
	pCategory->AddChildItem( new CXTPGI_UINT("EffID", &stUseEffSet.m_ulEffID, stUseEffSet.m_ulEffID))->SetReadOnly(TRUE);
	pCategory->AddChildItem( new CXTPGI_RwV3d("center", &stUseEffSet.m_v3dCenter, stUseEffSet.m_v3dCenter));
	pCategory->AddChildItem( new CXTPGI_Float("scale", &stUseEffSet.m_fScale, stUseEffSet.m_fScale ));
	pCategory->AddChildItem( new CXTPGI_RwRGBA("RGBScale", &stUseEffSet.m_rgbScale, stUseEffSet.m_rgbScale ));
	pCategory->AddChildItem( new CXTPGI_Float("particleNumScale", &stUseEffSet.m_fParticleNumScale, stUseEffSet.m_fParticleNumScale ));
	pCategory->AddChildItem( new CXTPGI_Enum<CGlobalVar::E_FRAME>( _T("parent frame"), CGlobalVar::e_frm_num, CGlobalVar::bGetInst().m_eFrmParent, &CGlobalVar::bGetInst().m_eFrmParent, CGlobalVar::bGetInst().STREFRM ) );
	pCategory->AddChildItem( new CXTPGI_Enum<CGlobalVar::E_FRAME>( _T("target frame"), CGlobalVar::e_frm_num, CGlobalVar::bGetInst().m_eFrmTarget, &CGlobalVar::bGetInst().m_eFrmTarget, CGlobalVar::bGetInst().STREFRM ) );
	pCategory->AddChildItem( new CXTPGI_FileName( "clump emiter", EFF2_FILE_NAME_MAX, CGlobalVar::bGetInst().m_szClump, CGlobalVar::bGetInst().m_szClump, _T("OBJECT\\"), _T("(*.RWS)|*.RWS|") ) );
	pCategory->AddChildItem( new CXTPGI_RwV3d( "missileTargetOffset", &stUseEffSet.m_stMissileTargetInfo.m_v3dCenter, stUseEffSet.m_stMissileTargetInfo.m_v3dCenter ) );
	pCategory->AddChildItem( new CXTPGI_UINT("delay", &stUseEffSet.m_ulDelay, stUseEffSet.m_ulDelay));
	pCategory->AddChildItem( new CXTPGI_UINT("life", &stUseEffSet.m_ulLife, stUseEffSet.m_ulLife));

	CXTPPropertyGridItem* pflag = pCategory->AddChildItem( new CXTPPropertyGridItem(_T("Flag")) );
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_LINKTOPARENT			, "LINKTOPARENT"		));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_NOSCALE				, "NOSCALE"				));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_MAINCHARAC			, "MAINCHARAC"			));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_DIR_PAR_TO_TAR		, "DIR_PAR_TO_TAR"		));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_DIR_TAR_TO_PAR		, "DIR_TAR_TO_PAR"		));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_DIR_IGN_HEIGHT		, "DIR_IGN_HEIGHT"		));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_EMITER_WITH_CLUMP	, "EMITER_WITH_CLUMP"	));
	pflag->AddChildItem( new CXTPGI_FlagBool( &stUseEffSet.m_ulFlags, stEffUseInfo::E_FLAG_EMITER_WITH_ATOMIC	, "EMITER_WITH_ATOMIC"	));
    
	pCategory->Expand();
	pflag->Expand();

	return 0;
}

LRESULT CDlgGridToolOption::OnGridNotify(WPARAM wp, LPARAM lp)
{
	ToWnd("-------------------CDlgGridToolOption::OnGridNotify--------------------");

	switch( wp )
	{
	case XTP_PGN_SORTORDER_CHANGED:
		break;
	case XTP_PGN_SELECTION_CHANGED:
		break;
	case XTP_PGN_ITEMVALUE_CHANGED:
		{
			g_MyEngine.bOnOffAxisCone( CToolOption::bGetInst().bShowWorldAxisCone() );
			g_MyEngine.bOnOffEffFrame( CToolOption::bGetInst().bShowEffFrame() );
			g_MyEngine.bSetFrmHeight();
		}
		break;
	case XTP_PGN_EDIT_CHANGED:
		break;
	case XTP_PGN_INPLACEBUTTONDOWN:
		break;
	case XTP_PGN_DRAWITEM:
		break;
	case XTP_PGS_OWNERDRAW:
		break;
	}

	return 0;
}