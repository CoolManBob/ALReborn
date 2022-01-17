#include "stdafx.h"
#include "../EffTool.h"
#include "../MainFrm.h"
#include "DlgTreeEffSet.h"
#include "Dlg_NewEffAnim.h"
#include "Dlg_NewAnimItem.h"
#include "DlgNewDpnd.h"

#include "../ToolOption.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//-----------------------------------------------------------------------------
// debug 유틸 함수들.
//-----------------------------------------------------------------------------
VOID tempShowTV_ITEM(const TV_ITEM& item, LPCSTR szName=NULL);
VOID tempShowNMHDR(const NMHDR& hdr, LPCSTR szName=NULL);
VOID tempShowNM_TREEVIEW(const NM_TREEVIEW& treeView, LPCSTR szName=NULL);
VOID tempShowTV_DISPINFO(const TV_DISPINFO& dspInfo, LPCSTR szName=NULL);

VOID tempShowTV_ITEM(const TV_ITEM& item, LPCSTR szName)
{
	ToWnd(Eff2Ut_FmtMsg("--== tempShowTVITEM (%s)==--\n"		, szName ? szName : "unknown"));
	ToWnd(Eff2Ut_FmtMsg("%20s : 0x%08x\n"	,"mask"				,item.mask			));
	ToWnd(Eff2Ut_FmtMsg("%20s : 0x%08x\n"	,"hItem"			,item.hItem			));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"state"			,item.state			));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"stateMask"		,item.stateMask		));
	ToWnd(Eff2Ut_FmtMsg("%20s : %s\n"		,"pszText"			,DEF_FLAG_CHK(item.mask, TVIF_TEXT) ? item.pszText : ""));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"cchTextMax"		,item.cchTextMax	));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"iImage"			,item.iImage		));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"iSelectedImage"	,item.iSelectedImage));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"		,"cChildren"		,item.cChildren		));
	ToWnd(Eff2Ut_FmtMsg("%20s : 0x%08x\n"	,"lParam"			,item.lParam		));
};

VOID tempShowNMHDR(const NMHDR& hdr, LPCSTR szName)
{
	ToWnd(Eff2Ut_FmtMsg("--== tempShowNMHDR (%s)==--\n", szName ? szName : "unknown"));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"	,"hwndFrom"	,hdr.hwndFrom	));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"	,"idFrom"	,hdr.idFrom		));
	ToWnd(Eff2Ut_FmtMsg("%20s : %d\n"	,"code"		,hdr.code		));
};

VOID tempShowNM_TREEVIEW(const NM_TREEVIEW& treeView, LPCSTR szName)
{
	ToWnd( Eff2Ut_FmtMsg("#### tempShowNM_TREEVIEW (%s) ####\n", szName ? szName : "unknown") );
	tempShowNMHDR( treeView.hdr );
	ToWnd( Eff2Ut_FmtMsg( "%20s : %d\n","action" ,treeView.action ) );
	tempShowTV_ITEM( treeView.itemOld, "old" );
	tempShowTV_ITEM( treeView.itemNew, "new" );
	ToWnd(Eff2Ut_FmtMsg( "%20s : %d, %d\n","ptDrag",treeView.ptDrag.x,treeView.ptDrag.y ) );
};

VOID tempShowTV_DISPINFO(const TV_DISPINFO& dspInfo, LPCSTR szName)
{
	ToWnd(Eff2Ut_FmtMsg("#### tempShowTV_DISPINFO (%s) ####\n", szName ? szName : "unknown"));
	tempShowNMHDR( dspInfo.hdr );
	tempShowTV_ITEM( dspInfo.item );
};

//--------------------------- CDlgTreeEffSet -------------------------
CDlgTreeEffSet::CDlgTreeEffSet(CWnd* pParent) : CDlgTree(pParent), m_pSelParam(NULL), m_lpEffBaseCopy(NULL)
{
}

BEGIN_MESSAGE_MAP(CDlgTreeEffSet, CDlgTree)
	//{{AFX_MSG_MAP(CDlgTreeEffSet)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DATA, OnSelChangedTree)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_DATA, OnDeleteItemTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DATA, OnDblclkTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_DATA, OnRclickTree)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENUITEM_TRCTRL_INS, OnMenuItemTrCtrlIns)
	ON_COMMAND(ID_MENUITEM_TRCTRL_DEL, OnMenuItemTrCtrlDel)
	ON_COMMAND(ID_MENUITEM_TRCTRL_EDIT, OnMenuItemTrCtrlEdit)
	ON_COMMAND(ID_TRCTRLEFFSET_SAVE, OnMenuItemTrCtrltSave)
	ON_COMMAND(ID_TRCTRLEFFSET_INSERTDEPENDANCY, OnTrctrleffsetInsertdependancy)
	ON_COMMAND(ID_MENUITEM_TRCTRL_BASECOPY, OnMenuitemTrctrlBasecopy)
	ON_COMMAND(ID_MENUITEM_TRCTRL_BASEPASTE, OnMenuitemTrctrlBasepaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTree message handlers
void CDlgTreeEffSet::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if( !pNMTreeView )		return;

	m_ctrlEditInfo.SetWindowText(_T("selected info"));
	
	m_pSelParam = reinterpret_cast<LPLparam>(CDlgTree::tGetTV_ITEM(pNMTreeView->itemNew.hItem).lParam);
	if( !m_pSelParam )
	{
		CMainFrame::bGetThis()->m_dlgGridEffSet.bSetBlank();
		return;
	}

	TCHAR	szInfo[MAX_PATH] = "";
	sprintf( szInfo, "[%d] : %s", m_pSelParam->GetEffSet()->bGetID(), m_pSelParam->GetEffSet()->bGetTitle() );
	m_ctrlEditInfo.SetWindowText(szInfo);

	switch( m_pSelParam->m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffSet( m_pSelParam->GetEffSet() );
		CMainFrame::bGetThis()->m_dlgGridEffSet.bSetForEffSet( m_pSelParam->GetEffSet() );
		break;
	case Lparam::E_LPARAM_EFFBASE:
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffSet( m_pSelParam->GetEffSet() );
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffBase( m_pSelParam->GetEffBase() );
		CMainFrame::bGetThis()->m_dlgGridEffSet.bSetForEffBase( m_pSelParam->GetEffSet(), m_pSelParam->GetEffBase() );
		break;
	case Lparam::E_LPARAM_ANIM:
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffSet( m_pSelParam->GetEffSet() );
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffBase( m_pSelParam->GetEffBase() );
		NSUtFn::CSelectedInfo::bGetInst().bOnSelEffAnim( m_pSelParam->GetEffAnim() );
		CMainFrame::bGetThis()->m_dlgGridEffSet.bSetForEffAnim( m_pSelParam->GetEffSet(), m_pSelParam->GetEffBase(), m_pSelParam->GetEffAnim() );
		break;
	default:
		CMainFrame::bGetThis()->m_dlgGridEffSet.bSetBlank();
		break;
	}

	*pResult = 0;
};

void CDlgTreeEffSet::OnDeleteItemTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	vDelLparam(reinterpret_cast<LPLparam>(pNMTreeView->itemOld.lParam));	
	UpdateData(TRUE);
	*pResult = 0;
};

void CDlgTreeEffSet::OnDblclkTree(NMHDR* pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ErrToWnd("m_ctrlTreeData.GetSelectedItem failed @ CDlgTreeEffSet::OnDblclkTree");
		return;
	}

	LPLparam pParam = vGetLparam(hItem);

	if( CToolOption::bGetInst().bShowOnlyOneEff() )
	{
		ListEffSetItr	it_curr	= m_listStEffSet.begin();
		for( ; it_curr != m_listStEffSet.end(); ++it_curr )
		{
			AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl((*it_curr).m_ulID);
		}
	}
		
	if( pParam->m_eType == Lparam::E_LPARAM_EFFBASE )
	{
		AgcdEffGlobal::bGetInst().bSetShowEffBase( pParam->GetEffBase() );
	}
	else
	{
		AgcdEffGlobal::bGetInst().bSetShowEffBase( NULL );
	}

	CGlobalVar::bGetInst().bUpdateUseInfo(pParam->GetEffSet()->bGetID());
	stEffUseInfo*	pUseInfo	= CGlobalVar::bGetInst().bGetPtrEffUseInfo();
	g_pAgcmEff2->UseEffSet( pUseInfo );

	*pResult = 0;
}

void CDlgTreeEffSet::OnRclickTree(NMHDR* pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem	= m_ctrlTreeData.GetDropHilightItem();

	if( hItem )
	{
		m_ctrlTreeData.SelectItem( hItem );
		m_pSelParam = vGetLparam(hItem);
	}
	
	*pResult = 0;
}

void CDlgTreeEffSet::OnContextMenu(CWnd* pWnd, CPoint point)
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		return;
	}

	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	CMenu	menu;
	VERIFY( menu.LoadMenu( IDR_MENU_TRCTRL_SET_POPMENU ) );

	CMenu*	pPopup	= menu.GetSubMenu( 0 );
	ASSERT( pPopup );

	/*
	ON_COMMAND(ID_MENUITEM_TRCTRL_INS, OnMenuItemTrCtrlIns)
	ON_COMMAND(ID_MENUITEM_TRCTRL_DEL, OnMenuItemTrCtrlDel)
	ON_COMMAND(ID_MENUITEM_TRCTRL_EDIT, OnMenuItemTrCtrlEdit)
	ON_COMMAND(ID_TRCTRLEFFSET_SAVE, OnMenuItemTrCtrltSave)*/
	pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_EDIT, TRUE );
	pPopup->EnableMenuItem( ID_TRCTRLEFFSET_SAVE, TRUE );
	pPopup->EnableMenuItem( ID_TRCTRLEFFSET_INSERTDEPENDANCY, TRUE );
	
	pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_BASEPASTE, TRUE );
	pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_BASECOPY, TRUE );
	switch( pParam->m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_BASEPASTE, m_lpEffBaseCopy ? FALSE : TRUE );

		pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_EDIT, FALSE );
		pPopup->EnableMenuItem( ID_TRCTRLEFFSET_SAVE, FALSE );
		pPopup->EnableMenuItem( ID_TRCTRLEFFSET_INSERTDEPENDANCY, FALSE );
		break;

	case Lparam::E_LPARAM_EFFBASE:
		pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_BASECOPY, FALSE );
		break;

	case Lparam::E_LPARAM_ANIM:
		{
			if( pParam->GetEffAnim()->bGetAnimType() == AgcdEffAnim::E_EFFANIM_MISSILE ||
				pParam->GetEffAnim()->bGetAnimType() == AgcdEffAnim::E_EFFANIM_RPSPLINE ||
				pParam->GetEffAnim()->bGetAnimType() == AgcdEffAnim::E_EFFANIM_RTANIM 
				)
			{
				pPopup->EnableMenuItem( ID_MENUITEM_TRCTRL_INS, TRUE );
			}
		}break;
	}

	pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON
		, point.x
		, point.y
		, this );
};
void CDlgTreeEffSet::OnMenuItemTrCtrlIns()
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("can't know to insert what kind of...");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	switch( pParam->m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		{
			CDlgNewEffBase	dlgNewBase(this);
			if( IDOK == dlgNewBase.DoModal() )
			{
				if( T_ISMINUS4(dlgNewBase.m_nEffBaseType) )
				{
					ToWnd("unknown effbase type!");
					return;
				}
				LPEFFSET	pEffSet = pParam->GetEffSet();
				if( !pEffSet )
				{
					ErrToWnd("!pEffSet");
					return;
				}
				pEffSet->bForTool_InsEffBase( (AgcdEffBase::E_EFFBASETYPE) (dlgNewBase.m_nEffBaseType) );
				LPEFFBASE	pEffBase = pEffSet->bGetPtrEffBase( pEffSet->bGetVarSizeInfo().m_nNumOfBase -1 );
				if( pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_OBJECT )
				{
					ASSERT( AgcdEffBase::E_EFFBASE_OBJECT == (AgcdEffBase::E_EFFBASETYPE)dlgNewBase.m_nEffBaseType );

					if( dlgNewBase.m_bDummyObj )
					{
						AgcdEffObj* pEffObj = dynamic_cast<AgcdEffObj*>(pEffBase);
						ASSERT(pEffObj);
						pEffObj->bFlagOn( FLAG_EFFBASEOBJ_DUMMY );
						if( T_ISMINUS4(pEffObj->bSetClumpName( "" )))
						{
							ErrToWnd( "pEffObj->bSetClumpName( "" ) failed" );
							pEffSet->bForTool_DelEffBase(pEffSet->bGetVarSizeInfo().m_nNumOfBase -1);
							return;
						}
					}
					else
					{
						AgcdEffObj* pEffObj = dynamic_cast<AgcdEffObj*>(pEffBase);
						ASSERT(pEffObj);
						char	dir[MAX_PATH]="";
						::GetCurrentDirectory(MAX_PATH, dir);
						pEffObj->bFlagOff( FLAG_EFFBASEOBJ_DUMMY );
						if( T_ISMINUS4(pEffObj->bSetClumpName( dlgNewBase.m_strFName.GetBuffer() )))
						{
							ErrToWnd( "pEffObj->bSetClumpName failed" );
							pEffSet->bForTool_DelEffBase(pEffSet->bGetVarSizeInfo().m_nNumOfBase -1);
							return;
						}
					}
				}


				Lparam	lparam( Lparam::E_LPARAM_EFFBASE, (LPVOID)pEffSet, (LPVOID)pEffBase );

				vSetNewEffBase( pEffSet, pEffBase, &dlgNewBase );
				bInsLparam( lparam, hItem );

				bRefreshSelect();
			}
		}break;
	case Lparam::E_LPARAM_EFFBASE:
		{
			LPEFFBASE	pEffBase = pParam->GetEffBase();
			if( !pEffBase )
			{
				ErrToWnd( "!pEffBase" );
				return;
			}
			CDlg_NewEffAnim	dlgNewAnim(pEffBase,this);
			if( IDOK == dlgNewAnim.DoModal() )
			{

				STCREATEPARAM_EFFANIM	cparam_anim;
				switch( (AgcdEffAnim::E_EFFANIMTYPE)dlgNewAnim.m_nSelBtn )
				{
				case AgcdEffAnim::E_EFFANIM_RPSPLINE:
					{
						cparam_anim.bSetForRpSpline(0LU,1000LU,e_TblDir_none,dlgNewAnim.bGetFName());
					}break;
				case AgcdEffAnim::E_EFFANIM_RTANIM:
					{
						cparam_anim.bSetForRtAnim(0LU,0LU,e_TblDir_infinity,dlgNewAnim.bGetFName());
					}break;
				default:
					{
						cparam_anim.m_eEffAnimType = (AgcdEffAnim::E_EFFANIMTYPE)dlgNewAnim.m_nSelBtn;
						cparam_anim.m_eLoopOpt = e_TblDir_infinity;
					}break;
				}

				if( T_ISMINUS4( pEffBase->bForTool_InsAnim(&cparam_anim) ) )
				{
					ErrToWnd("pEffBase->bForTool_InsAnim failed");
					return;
				}
				Lparam	lparam( 
					Lparam::E_LPARAM_ANIM
					, (LPVOID)pParam->GetEffSet()
					, (LPVOID)pEffBase
					,(LPVOID)pEffBase->bGetPtrEffAnim(pEffBase->m_stVarSizeInfo.m_nNumOfAnim-1)  
					);
				bInsLparam( lparam, hItem );
				bRefreshSelect();
			}			
		}break;
	case Lparam::E_LPARAM_ANIM:
		{
			if( !pParam->GetEffAnim() )
			{
				ErrToWnd("!pParam->GetEffAnim()");
				return;
			}

			CDlg_NewAnimItem	dlgItem(
				  dynamic_cast<AgcdEffRenderBase*>(pParam->GetEffBase())
				, pParam->GetEffAnim()
				, this);
			if( IDOK == dlgItem.DoModal() )
			{
				AgcdEffAnim_TuTv* pAnimUV	=  dynamic_cast<AgcdEffAnim_TuTv*>(pParam->GetEffAnim());
				if( pAnimUV && dlgItem.bGetMultUV() )
				{
					CDlg_NewAnimItem::stMultUV	tmp	= dlgItem.bGetStMultUV();
					STUVRECT	rc;
					pAnimUV->m_stTblRect.m_stlvecTimeVal.clear();
					for( UINT i=tmp.bgn; i<=tmp.end; ++i )
					{
						int	portion	= (i-tmp.bgn)/tmp.row;
						int rest	= (i-tmp.bgn)%tmp.row;
						float xstep	= 1.f/(float)tmp.row;
						float ystep	= 1.f/(float)tmp.col;
						rc.m_fLeft	= xstep*(float)(rest);
						rc.m_fTop	= ystep*(float)(portion);
						rc.m_fRight	= rc.m_fLeft+xstep;
						rc.m_fBottom= rc.m_fTop+ystep;

						pAnimUV->bInsTVal( (LPVOID)&rc, i*tmp.tdst );
					}
					bRefreshSelect();
				}
				else
				{
					pParam->GetEffAnim()->bInsTVal(	dlgItem.bGetPtrVal(), dlgItem.m_ulTime );
					bRefreshSelect();
				}
			}
		}break;
	}
	
	vEffSetFlagUpdate( pParam->GetEffSet() );
};
void CDlgTreeEffSet::OnMenuItemTrCtrlDel()
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("m_ctrlTree.GetSelectedItem failed @ CDlgTreeEffSet::OnMenuItemTrCtrlDel");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	const char* szType[] = {
		"EFFSET",
		"EFFBASE",
		"ANIM",
	};

	if( IDYES != AfxMessageBox( Eff2Ut_FmtMsg("%d 를 삭제 합니다",pParam->GetEffSet()->bGetID()), MB_YESNO ) )
		return;

	switch( pParam->m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		{
		}break;
	case Lparam::E_LPARAM_EFFBASE:
		{
			if( T_ISMINUS4( vDelEffBase(pParam) ) )
			{
				ErrToWnd( "vDelEffBase(pParam) failed" );
				return;
			}
			bOnEditEffSet( pParam->GetEffSet()->bGetID() );
		}break;
	case Lparam::E_LPARAM_ANIM:
		{
			if( T_ISMINUS4( vDelEffAnim(pParam) ) )
			{
				ErrToWnd( "vDelEffAnim(pParam) failed" );
				return;
			}
			bOnEditEffSet( pParam->GetEffSet()->bGetID() );
		}break;
	}

	m_ctrlTreeData.DeleteItem( hItem );

	bRefreshSelect();
};
void CDlgTreeEffSet::OnMenuItemTrCtrlEdit()
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("m_ctrlTree.GetSelectedItem failed @ CDlgTreeEffSet::OnMenuItemTrCtrlEdit");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam @ CDlgTreeEffSet::OnMenuItemTrCtrlEdit");
		return;
	}

	const char* szType[] = {
		"EFFSET",
		"EFFBASE",
		"ANIM",
	};

	switch( pParam->m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		{
			AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( pParam->GetEffSet()->bGetID() );
			theApp.bCalcBound(pParam->GetEffSet()->bGetID());
		}break;

	default:
		ToWnd("바운딩계산은 이펙트 셋을 선택한후 하세요.");
	}
};
void CDlgTreeEffSet::OnMenuItemTrCtrltSave()
{	
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("m_ctrlTree.GetSelectedItem failed @ CDlgTreeEffSet::OnMenuItemTrCtrlDel");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	UINT	ulEffSetID = pParam->GetEffSet()->bGetID();
	
	EffSet	the(ulEffSetID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffSetID) );
		return;
	}

	
	// TODO : 왜 사용되지 않는 텍스쳐가 들어갔는지 근본적 해결이 필요.
	{//texture
		char notusedTex[512] = {'\0', };
		strcpy( notusedTex, "사용되지 않는 텍스쳐 : \r\n" );
		if( !pParam->GetEffSet()->bForTool_IsAllTexUsed(notusedTex) )
		{
			strcat( notusedTex, "가 있습니다. 지울까요?" );
			if( IDOK == AfxMessageBox( notusedTex ) )
			{
				INT delcnt = pParam->GetEffSet()->bForTool_DelAllEffTexNotUsed();
				ToWnd( Eff2Ut_FmtMsg("%d개 지움\r\n", delcnt) );
			};
		}
	}
	
	if( IDOK == AfxMessageBox("바운딩정보를 갱신할까요?") )
	{
		AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( ulEffSetID );
		theApp.bCalcBound(ulEffSetID);
	}

	if( T_ISMINUS4( vSaveEffSet(ulEffSetID) ) )
	{
		ErrToWnd(_T("저장 실패!"));
	}
	else
	{
		(*it_f).FlagOff(EffSet::FLAG_NOTSAVED);
		

		LPEFFSET	pEffSet = vGetPtrEffSet(ulEffSetID);
		if( !pEffSet ){
			ErrToWnd(_T("저장은 하였으나 리스트에 없습니다. 이런경우를 알려주세요. -문경삼-"));
		}else if((*it_f).FlagChk(EffSet::FLAG_NEW)) {
			(*it_f).FlagOff(EffSet::FLAG_NEW);
			INT ir =
				CMainFrame::bGetThis()->m_dlgListEffSet.bInsItem( 
					ulEffSetID
					, pEffSet->bGetTitle()
					, pEffSet->bGetVarSizeInfo().m_nNumOfBase
					, pEffSet->bGetLife() );
			if( T_ISMINUS4( ir ) ){
				ErrToWnd( _T("새로운 이펙트를 저장후 이펙트셋 리스트창에 추가하지 못하였습니다.") );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// util func
CDlgTreeEffSet::LPLparam CDlgTreeEffSet::vGetLparam(HTREEITEM hItem)
{
	if( hItem )
		return (reinterpret_cast<LPLparam>(tGetTV_ITEM( hItem ).lParam));
	return NULL;
};
INT CDlgTreeEffSet::bClear()
{
	if( vBeExistNotSaved() )
	{
		ToWnd(_T("저장하지 안은 이펙트 가 있으나.. 바로 종료 합니다."));
	}
	if( m_ctrlTreeData.GetSafeHwnd() )
		m_ctrlTreeData.DeleteAllItems();
	return 0;
};

HTREEITEM CDlgTreeEffSet::vInsTrCtrlItem(LPCTSTR szBuff
										 , LPARAM lparam
										 , HTREEITEM hParent
										 , HTREEITEM hInsertAfter)
{
	return m_ctrlTreeData.InsertItem( 
		TVIF_TEXT | TVIF_PARAM
		, szBuff
		, 0L, 0L, 0LU, 0LU
		, lparam
		, hParent, hInsertAfter
		);
};
INT	CDlgTreeEffSet::bInsLparam(Lparam& lparam
						 , HTREEITEM hParent/*=TVI_ROOT*/
						 , HTREEITEM item/*=TVI_LAST*/)
{
	INT		ir	= 0;
	TCHAR	szBuff[MAX_PATH]	= "";
	m_listParam.push_back( lparam );
	switch( lparam.m_eType )
	{
	case Lparam::E_LPARAM_EFFSET:
		{
			//*************************************************
			ir	= vInsStEffSet( lparam.GetEffSet()->bGetID() );
			//*************************************************

			if( T_ISMINUS4( ir ) )
			{
				//err
				return ir;
			}

			strcpy( szBuff, "set : " );
			strcat( szBuff, lparam.GetEffSet()->bGetTitle() );
			HTREEITEM hEffSet	
				= vInsTrCtrlItem( szBuff
				, reinterpret_cast<LPARAM>(&m_listParam.back())
				, hParent
				, item );

			ASSERT( hEffSet );
			if( !hEffSet )
			{
				ErrToWnd( _T("vInsTrCtrlItem failed") );
				//에러나믄 처리해주자.
				return -1;
			}

			for( int i=0; i<lparam.GetEffSet()->bGetVarSizeInfo().m_nNumOfBase; ++i )
			{
				Lparam	stparam( Lparam::E_LPARAM_EFFBASE
					, lparam.GetEffSet()
					, lparam.GetEffSet()->bGetPtrEffBase(i)  );

				ir = bInsLparam( stparam, hEffSet );
				if( T_ISMINUS4( ir ) )
				{
					//err
					return ir;
				}
			}
		}break;
	case Lparam::E_LPARAM_EFFBASE:
		{
			sprintf( szBuff, "%s : %s"
				, EFFBASETYPE_NAME[lparam.GetEffBase()->bGetBaseType()]
				, lparam.GetEffBase()->bGetPtrTitle() );
			HTREEITEM hEffBase
				= vInsTrCtrlItem( szBuff
				, reinterpret_cast<LPARAM>(&m_listParam.back())
				, hParent
				, item );

			ASSERT( hEffBase );
			if( !hEffBase )
			{
				ErrToWnd( _T("vInsTrCtrlItem failed") );
				//에러나믄 처리해주자.
				return -1;
			}

			for( int i=0; i<lparam.GetEffBase()->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
			{
				Lparam	stparam( Lparam::E_LPARAM_ANIM
					, lparam.GetEffSet()
					, lparam.GetEffBase()
					, lparam.GetEffBase()->bGetPtrEffAnim(i) );

				ir = bInsLparam( stparam, hEffBase );
				if( T_ISMINUS4( ir ) )
				{
					//err
					return ir;
				}
			}
		}break;
	case Lparam::E_LPARAM_ANIM:
		{
			strncpy( szBuff, EFFANIMTYPE_NAME[lparam.GetEffAnim()->bGetAnimType()], MAX_PATH-1 );
			HTREEITEM hEffAnim
				= vInsTrCtrlItem( szBuff
				, reinterpret_cast<LPARAM>(&m_listParam.back())
				, hParent
				, item );

			ASSERT( hEffAnim );
			if( !hEffAnim )
			{
				ErrToWnd( _T("vInsTrCtrlItem failed") );
				//에러나믄 처리해주자.
				return -1;
			}
		}break;
	}


	UpdateData(TRUE);
	return 0;
};
void CDlgTreeEffSet::vDelLparam(LPLparam lpLparam)
{
	ASSERT( lpLparam );
	if( !lpLparam )
		return;

	ListLparamItr	it_del
		= std::find( m_listParam.begin()
		, m_listParam.end()
		, (*lpLparam) );
	if( it_del != m_listParam.end() )
	{
		if( lpLparam->m_eType == Lparam::E_LPARAM_EFFSET)
		{
			vDelStEffSet( lpLparam );
		}

		if( m_pSelParam == &(*it_del) )
			m_pSelParam = NULL;
		m_listParam.erase(it_del);
		lpLparam = NULL;
	}
	else
	{
		ErrToWnd( _T("can not find the lparam") );
	}
};
BOOL CDlgTreeEffSet::vBeExistEffSet(UINT ulEffSetID)
{
	EffSet	the(ulEffSetID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);
	if( it_f == m_listStEffSet.end() )
		return FALSE;
	return TRUE;
};
BOOL CDlgTreeEffSet::vBeExistNotSaved(void)
{
	ListEffSetItr it_curr = m_listStEffSet.begin();
	for( ; it_curr != m_listStEffSet.end(); ++it_curr )
	{
		if( (*it_curr).FlagChk(EffSet::FLAG_NOTSAVED) )
			return TRUE;
	}
	return FALSE;
};
BOOL CDlgTreeEffSet::vBeSavedEffset(UINT ulEffSetID)
{
	EffSet	the(ulEffSetID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffSetID) );
		return TRUE;//CAN'T FIND
	}

	return ((*it_f).FlagChk(EffSet::FLAG_NOTSAVED) ? FALSE : TRUE);
};
INT CDlgTreeEffSet::vSaveEffSet(UINT ulEffSetID)
{
	LPEFFSET	pEffSet = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet(ulEffSetID);
	if( pEffSet )
	{
		if( T_ISMINUS4(AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->bWrite_txt( pEffSet ) ))
		{
			ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 저장에 실패하였습니다.", ulEffSetID) );
			return -1;
		}
	}
	else
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 를 찾지 못하였습니다.", ulEffSetID) );
		return -1;
	}

	return 0;
};
AgcdEffSet* CDlgTreeEffSet::vGetPtrEffSet(UINT ulEffSetID)
{
	return
	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet( ulEffSetID );
};


INT CDlgTreeEffSet::vInsStEffSet(UINT ulEffSetID)
{
	if( vBeExistEffSet( ulEffSetID ) )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 이미 추가된 아이디 입니다.", ulEffSetID) );
		return -1;
	}

	LPEFFSET	pEffSet = vGetPtrEffSet(ulEffSetID);
	if( pEffSet )
	{
		if( T_ISMINUS4( pEffSet->bAddRef() ) )
		{
			ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가에 실패하였습니다.\npEffSet->bAddRef() failed", ulEffSetID) );
			return -1;
		}

		EffSet ins(ulEffSetID);
		m_listStEffSet.push_back( ins );
	}
	else
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 를 찾지 못하였습니다.", ulEffSetID) );
		return -1;
	}

	return 0;
};


INT CDlgTreeEffSet::vDelStEffSet(LPLparam pStLParam)
{
	UINT	ulEffSetID = pStLParam->GetEffSet()->bGetID();

	EffSet	the(ulEffSetID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffSetID) );
		return -1;//CAN'T FIND
	}

	if( !vBeSavedEffset(ulEffSetID) )
	{
		if( IDYES == AfxMessageBox( Eff2Ut_FmtMsg("[id : %d] 추가된 혹은 변경된 이펙트입니다.\n저장하시겠습니까?",ulEffSetID), MB_YESNO ) )
		{
			if( T_ISMINUS4( vSaveEffSet(ulEffSetID) ) )
			{
				ErrToWnd(_T("저장에 실패하여... 이데이터는 소멸됩니다."));
			}
			else
			{
				(*it_f).FlagOff(EffSet::FLAG_NOTSAVED);
				

				LPEFFSET	pEffSet = vGetPtrEffSet(ulEffSetID);
				if( !pEffSet ){
					ErrToWnd(_T("저장은 하였으나 리스트에 없습니다. 이런경우를 알려주세요. -문경삼-"));
				}else if((*it_f).FlagChk(EffSet::FLAG_NEW)) {
					(*it_f).FlagOff(EffSet::FLAG_NEW);
					INT ir =
						CMainFrame::bGetThis()->m_dlgListEffSet.bInsItem( 
							ulEffSetID
							, pEffSet->bGetTitle()
							, pEffSet->bGetVarSizeInfo().m_nNumOfBase
							, pEffSet->bGetLife() );
					if( T_ISMINUS4( ir ) ){
						ErrToWnd( _T("새로운 이펙트를 저장후 이펙트셋 리스트창에 추가하지 못하였습니다.") );
					}
				}
			}
		}
	}

	
	LPEFFSET	pEffSet = vGetPtrEffSet(ulEffSetID);
	if( pEffSet )
	{
		if( (*it_f).FlagChk(EffSet::FLAG_NEW) ){
			pEffSet->bRelease();
			AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffSet( ulEffSetID );
		}else{
			AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( ulEffSetID );
			if( T_ISMINUS4( pEffSet->bRelease() ) )
			{
				ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 삭제에 실패하였습니다.\npEffSet->bRelease() failed", ulEffSetID) );
				return -1;
			}
		}
	}
	else
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 를 찾지 못하였습니다.", ulEffSetID) );
		return -1;
	}

	m_listStEffSet.erase( it_f );
	return 0;
};

INT CDlgTreeEffSet::vDelEffBase(LPLparam pStLParam)
{
	if( pStLParam->m_eType != Lparam::E_LPARAM_EFFBASE )
	{
		ErrToWnd("pStLParam->eLparamType != Lparam::E_LPARAM_EFFBASE");
		return -1;
	}

	int nBaseIndex = 
		pStLParam->GetEffSet()->bForTool_FindBaseIndex(pStLParam->GetEffBase());

	if( T_ISMINUS4( nBaseIndex ) )
	{
		ErrToWnd( "T_ISMINUS4( nBaseIndex )" );
		return -1;
	}

	if( pStLParam->GetEffSet()->bForTool_ChkDependancy( nBaseIndex ) )
	{
		if( IDYES == AfxMessageBox(
				"디펜던시가 있는 베이스 입니다.\n"
				"지우면 디펜던시도 같이 지워집니다.\n"
				"지울까요?\n"
				, MB_YESNO 
				)
				)
		{
			INT nDel = pStLParam->GetEffSet()->bForTool_DelDependancyAll( nBaseIndex );
			ToWnd( Eff2Ut_FmtMsg("%d 개의 디펜던시가 지워졌습니다.", nDel) );
			bOnEditEffSet( pStLParam->GetEffSet()->bGetID() );
		}
		else
			return -1;
	}

	vEffBaseDpndUpdate( pStLParam->GetEffSet(), nBaseIndex );

	RwInt32	nTexIndex	= -1;
	if( dynamic_cast<AgcdEffRenderBase*>(pStLParam->GetEffBase()) )
	{
		AgcdEffRenderBase*	pRenderBase	= 
			dynamic_cast<AgcdEffRenderBase*>(pStLParam->GetEffBase());
		nTexIndex	= pRenderBase->m_cEffTexInfo.m_nIndex;
	}

	pStLParam->GetEffSet()->bForTool_DelEffBase( nBaseIndex );

	if( !T_ISMINUS4(nTexIndex) )
	{
		if( !pStLParam->GetEffSet()->bForTool_IsTexUsed( nTexIndex ) )
		{
			pStLParam->GetEffSet()->bForTool_DelEffTex( nTexIndex );
		}
	}

	bOnEditEffSet( pStLParam->GetEffSet()->bGetID() );
	
	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( pStLParam->GetEffSet()->bGetID() );

	
	vEffSetFlagUpdate( pStLParam->GetEffSet() );

	return 0;
};

INT CDlgTreeEffSet::vDelEffAnim(LPLparam pStLParam)
{
	if( pStLParam->m_eType != Lparam::E_LPARAM_ANIM )
	{
		ErrToWnd("pStLParam->eLparamType != Lparam::E_LPARAM_ANIM");
		return -1;
	}

	int nAnimIndex = 
		pStLParam->GetEffBase()->bForTool_FindAnimIndex(pStLParam->GetEffAnim());

	if( T_ISMINUS4( nAnimIndex ) )
	{
		ErrToWnd(" T_ISMINUS4( nAnimIndex ) " );
		return -1;
	}

	
	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_DeleteEffCtrl( pStLParam->GetEffSet()->bGetID() );

	INT ir = pStLParam->GetEffBase()->bForTool_DelAnim( nAnimIndex );
	vEffSetFlagUpdate( pStLParam->GetEffSet() );

	return ir;
};

INT CDlgTreeEffSet::vInsEffBaseDpnd(LPLparam pStLParam)
{
	CDlgNewDpnd	dlg(pStLParam->GetEffSet(), this);
	if( dlg.DoModal() == IDOK )
	{
		for( AgcdEffSet::BaseDependancyVecItr Itr = dlg.m_vecDependancy.begin(); Itr != dlg.m_vecDependancy.end(); ++Itr )
			pStLParam->GetEffSet()->bForTool_InsDependancy( (*Itr) );

		vEffSetFlagUpdate( pStLParam->GetEffSet() );
	}

	return bRefreshSelect();
};

INT CDlgTreeEffSet::bDelEffBaseDpnd(INT delBaseDpndIndex)
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("m_ctrlTree.GetSelectedItem failed @ CDlgTreeEffSet::OnMenuItemTrCtrlDel");
		return -1;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return -1;
	}

	UINT	ulEffSetID = pParam->GetEffSet()->bGetID();
	
	EffSet	the(ulEffSetID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffSetID) );
		return -1;
	}

	if( delBaseDpndIndex >= pParam->GetEffSet()->bGetVarSizeInfo().m_nNumOfBaseDependancy )
	{
		ErrToWnd( "디펜던시 인덱스가 잘못되었습니다." );
		return -1;
	}

	pParam->GetEffSet()->bForTool_DelDependancy( delBaseDpndIndex );

	vEffSetFlagUpdate( pParam->GetEffSet() );

	return bRefreshSelect();
};

INT CDlgTreeEffSet::bOnEditEffSet(UINT	ulEffID)
{
	EffSet	the(ulEffID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffID) );
		return -1;//CAN'T FIND
	}

	(*it_f).FlagOn(EffSet::FLAG_NOTSAVED);

	return 0;
};

INT CDlgTreeEffSet::bOnNewEffSet(UINT	ulEffID)
{
	EffSet	the(ulEffID);
	ListEffSetItr it_f = 
		std::find(m_listStEffSet.begin(), m_listStEffSet.end(), the);

	if( it_f == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffID) );
		return -1;//CAN'T FIND
	}

	(*it_f).FlagOn(EffSet::FLAG_NOTSAVED);
	(*it_f).FlagOn(EffSet::FLAG_NEW);

	return 0;
};

INT CDlgTreeEffSet::vSetNewEffBase(AgcdEffSet* pEffSet, AgcdEffBase* pEffBase, CDlgNewEffBase* pDlg)
{
	pEffBase->bSetTitle(pDlg->m_dlgEffBase.m_szTitle);
	pEffBase->m_dwDelay = pDlg->m_dlgEffBase.m_ulDelay;
	pEffBase->m_dwLife = pDlg->m_dlgEffBase.m_ulLife;
	pEffBase->m_eLoopOpt = (E_LOOPOPT)pDlg->m_dlgEffBase.m_nLoopOpt;

	if( dynamic_cast<AgcdEffBoard*>(pEffBase)						||
		dynamic_cast<AgcdEffObj*>(pEffBase)							||
		dynamic_cast<AgcdEffParticleSystem*>(pEffBase)				||
		dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase) )
	{
		if( pDlg->m_dlgEffBase.m_bBillboard )
		{
			pEffBase->bFlagOn(FLAG_EFFBASE_BILLBOARD);
		}
		else if( pDlg->m_dlgEffBase.m_bBillboardY )
		{
			pEffBase->bFlagOn(FLAG_EFFBASE_BILLBOARDY);
		}
	}

	if( dynamic_cast<AgcdEffBoard*>(pEffBase)						||
		dynamic_cast<AgcdEffObj*>(pEffBase)							||
		dynamic_cast<AgcdEffParticleSystem*>(pEffBase)				||
		dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase)	||
		dynamic_cast<AgcdEffTail*>(pEffBase)						||
		dynamic_cast<AgcdEffTerrainBoard*>(pEffBase)				)
	{
		AgcdEffRenderBase*	pEffRenderBase = dynamic_cast<AgcdEffRenderBase*>(pEffBase);
		pEffRenderBase->m_eBlendType = (AgcdEffRenderBase::E_EFFBLENDTYPE)pDlg->m_dlgEffRenderBase.m_nBlendType;
		pEffRenderBase->m_v3dInitPos.x = pDlg->m_dlgEffRenderBase.m_fPoxX;
		pEffRenderBase->m_v3dInitPos.y = pDlg->m_dlgEffRenderBase.m_fPoxY;
		pEffRenderBase->m_v3dInitPos.z = pDlg->m_dlgEffRenderBase.m_fPoxZ;

		pEffRenderBase->m_stInitAngle.m_fPitch = pDlg->m_dlgEffRenderBase.m_fAngleX;
		pEffRenderBase->m_stInitAngle.m_fYaw = pDlg->m_dlgEffRenderBase.m_fAngleY;
		pEffRenderBase->m_stInitAngle.m_fRoll = pDlg->m_dlgEffRenderBase.m_fAngleZ;

		if(!dynamic_cast<AgcdEffObj*>(pEffBase))
			vSetNewEffBase_Tex(pEffSet, pEffRenderBase, pDlg);
	}

	if( dynamic_cast<AgcdEffParticleSystem*>(pEffBase) )
	{
		dynamic_cast<AgcdEffParticleSystem*>(pEffBase)->bSetCapacity(1);
		dynamic_cast<AgcdEffParticleSystem*>(pEffBase)->bGetRefEmiter().m_nNumOfOneShoot	= 1;
		dynamic_cast<AgcdEffParticleSystem*>(pEffBase)->bGetRefPProp().m_dwParticleLife		= 1000;
	}
	if( dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase) )
	{
		dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase)->bSetCapacity(1);
		dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase)->bSetOneShootNum(1, 0);
		dynamic_cast<AgcdEffParticleSys_SimpleBlackHole*>(pEffBase)->bSetPLife(1000);
	}

	return 0;
};

INT CDlgTreeEffSet::vSetNewEffBase_Tex(AgcdEffSet* pEffSet, AgcdEffRenderBase* pEffBase, CDlgNewEffBase* pDlg)
{
	AgcdEffSet::LPEffectExVecItr	it_curr = pEffSet->m_vecLPEffectEx.begin();
	int nTexIndex = 0;
	LPEFFTEX	pEffTex = NULL;
	for( ; it_curr != pEffSet->m_vecLPEffectEx.end(); ++it_curr, ++nTexIndex )
	{
		pEffTex = (*it_curr);
		if( !strcmp( pDlg->m_dlgEffRenderBase.m_szTex, pEffTex->bGetTexName() ) )
		{
			if( !strcmp( pDlg->m_dlgEffRenderBase.m_szMask, pEffTex->bGetMaskName() ) )
			{
				break;
			}
		}
	}

	if( nTexIndex < pEffSet->m_stVarSizeInfo.m_nNumOfTex )
	{
		pEffBase->bSetTexIndex(nTexIndex);
		pEffBase->bSetPtrEffTex(pEffTex);
	}
	else
	{
		pEffSet->bInsEffTex( pDlg->m_dlgEffRenderBase.m_szTex, pDlg->m_dlgEffRenderBase.m_szMask );
		pEffBase->bSetTexIndex( pEffSet->m_stVarSizeInfo.m_nNumOfTex );
		pEffBase->bSetPtrEffTex( pEffSet->bGetPtrEffTex(pEffSet->m_stVarSizeInfo.m_nNumOfTex ) );
		++pEffSet->m_stVarSizeInfo.m_nNumOfTex;
	}

	return 0;
};

INT CDlgTreeEffSet::bRefreshSelect(void)
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
		return 0;
	m_ctrlTreeData.SelectItem(NULL);
	m_ctrlTreeData.SelectItem(hItem);
	return 0;
};

INT CDlgTreeEffSet::vEffBaseDpndUpdate(AgcdEffSet* pEffSet, INT delBaseIndex)
{
	for( int i=0; i<pEffSet->m_stVarSizeInfo.m_nNumOfBaseDependancy; ++i )
	{
		ASSERT( delBaseIndex != pEffSet->bGetCRefBaseDpnd(i).GetChildIndex()
			 && delBaseIndex != pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() );

		AgcdEffSet::stBaseDependancy	dpnd( pEffSet->bGetCRefBaseDpnd(i) );
		if( pEffSet->bGetCRefBaseDpnd(i).GetChildIndex() > delBaseIndex )
			dpnd.SetChildIndex( dpnd.GetChildIndex() -1 );
		if( pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() > delBaseIndex )
			dpnd.SetParentIndex( dpnd.GetParentIndex() -1 );

		pEffSet->bSetDependancy( i, dpnd );
	}
	return 0;
};

INT CDlgTreeEffSet::vEffSetFlagUpdate(AgcdEffSet* pEffSet)
{
	if( !pEffSet )
	{
		ErrToWnd( "!pEffSet" );
		return -1;
	}

	//Update 1st EffBases' Flag
	for( int i=0; i<pEffSet->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		vEffBaseFlagUpdate( pEffSet, i );
	}


	//FLAG_EFFSET_MISSILE
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MISSILE);
	for( int i=0; i<pEffSet->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		const AgcdEffBase*	pBase	= pEffSet->bGetPtrEffBase(i);
		for( int j=0; j<pBase->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++j )
		{
			const AgcdEffAnim* pAnim	= pBase->bGetPtrEffAnim(j);
			if( dynamic_cast<const AgcdEffAnim_Missile*>(pAnim) )
			{
				DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MISSILE);
				break;
			}
		}
	}

	//FLAG_EFFSET_ONLYSOUND, FLAG_EFFSET_ONLYTAIL
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_ONLYSOUND);
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_ONLYTAIL);
	if( pEffSet->bGetVarSizeInfo().m_nNumOfBase == 1 )
		if( dynamic_cast<AgcdEffSound*>(pEffSet->bGetPtrEffBase(0)) )
			DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_ONLYSOUND);
		else if( dynamic_cast<AgcdEffTail*>(pEffSet->bGetPtrEffBase(0)) )
			DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_ONLYTAIL);

	//FLAG_EFFSET_MFRM, FLAG_EFFSET_MFRM_SRC, FLAG_EFFSET_MFRM_DST, FLAG_EFFSET_MFRM_CAM
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM);
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_SRC);
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_DST);
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_CAM);
	for( int i=0; i<pEffSet->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		
		const AgcdEffMFrm*	pMFrm	
			= dynamic_cast<const AgcdEffMFrm*>(pEffSet->bGetPtrEffBase(i));
		if( pMFrm )
		{
			DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM);
			switch( pMFrm->m_stShakeFrm.m_eWhose )
			{
			case AgcdEffMFrm::EFRM_CAM: DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_CAM); break;
			case AgcdEffMFrm::EFRM_SRC: DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_SRC); break;
			case AgcdEffMFrm::EFRM_DST: DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_MFRM_DST); break;
			}
		}
	}

	//FLAG_EFFSET_HASTAIL	//베이스 디펜던시가 없는 테일을 가지고 있다. ex) 칼궤적
	DEF_FLAG_OFF(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_HASTAIL);
	for( int i=0; i<pEffSet->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		if(!dynamic_cast<const AgcdEffTail*>(pEffSet->bGetPtrEffBase(i)))
			continue;

		BOOL isChild	= FALSE;
		for( int j=0; j<pEffSet->bGetVarSizeInfo().m_nNumOfBaseDependancy; ++j )
		{
			if( pEffSet->bGetCRefBaseDpnd(j).GetChildIndex() == i )
			{
				isChild	= TRUE;
				break;
			}
		}
		if( !isChild )
		{
			DEF_FLAG_ON(pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_HASTAIL);
			break;
		}
	}    

	return 0;
};

INT CDlgTreeEffSet::vEffBaseFlagUpdate(AgcdEffSet* pEffSet, INT baseindex)
{

	//flag associated with dependancy
	AgcdEffBase* pBase = pEffSet->bGetPtrEffBase(baseindex);
	pBase->bFlagOff( FLAG_EFFBASE_BASEDEPENDANCY );
	pBase->bFlagOff( FLAG_EFFBASE_BASEDPND_PARENT );
	pBase->bFlagOff( FLAG_EFFBASE_BASEDPND_CHILD );
	pBase->bFlagOff( FLAG_EFFBASE_BASEDPND_TOOBJ );
	pBase->bFlagOff( FLAG_EFFBASE_BASEDPND_TOMISSILEOBJ );

	DpndInfo	dpndinfo;
	if( vGetDpndInfo( &dpndinfo, pEffSet, baseindex ) )
	{
		pBase->bFlagOn( FLAG_EFFBASE_BASEDEPENDANCY );
		
		if( dpndinfo.m_bdpndParent )
		pBase->bFlagOn( FLAG_EFFBASE_BASEDPND_PARENT );
		else
		pBase->bFlagOn( FLAG_EFFBASE_BASEDPND_CHILD );
		if( dpndinfo.m_bdpndToObj )
		pBase->bFlagOn( FLAG_EFFBASE_BASEDPND_TOOBJ );
		if( dpndinfo.m_bdpndToMissileObj )
		pBase->bFlagOn( FLAG_EFFBASE_BASEDPND_TOMISSILEOBJ );
	}

	//FLAG_EFFBASE_MISSILE
	pBase->bFlagOff( FLAG_EFFBASE_MISSILE );
	for( int i=0; i<pBase->m_stVarSizeInfo.m_nNumOfAnim; ++i )
	{
		if( dynamic_cast<const AgcdEffAnim_Missile*>(pBase->bGetPtrEffAnim(i)) )
		{
			pBase->bFlagOn( FLAG_EFFBASE_MISSILE );
			break;
		}
	}

	return 0;
};

BOOL CDlgTreeEffSet::bDpendancyValidation(LPEFFSET pEffSet)
{
	for( int i=0; i<pEffSet->m_stVarSizeInfo.m_nNumOfBaseDependancy; ++ i )
		if( pEffSet->bGetCRefBaseDpnd(i).GetChildIndex() >= pEffSet->bGetVarSizeInfo().m_nNumOfBase ||
			pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() >= pEffSet->bGetVarSizeInfo().m_nNumOfBase )
		{
			CHAR buff[128] = { '\0', };
			sprintf( buff
				, "invalid dependancy [effid : %d] -- child(%d), parent(%d), numofbase(%d)"
				, pEffSet->bGetID()
				, pEffSet->bGetCRefBaseDpnd(i).GetChildIndex()
				, pEffSet->bGetCRefBaseDpnd(i).GetParentIndex()
				, pEffSet->bGetVarSizeInfo().m_nNumOfBase );
			ToWnd( buff );
			return FALSE;
		}
	return TRUE;
};

bool CDlgTreeEffSet::vGetDpndInfo(DpndInfo* pOut, AgcdEffSet* pEffSet, INT baseindex)
{
	ASSERT( pOut->m_bdpnd == false );
	for( int i=0; i<pEffSet->m_stVarSizeInfo.m_nNumOfBaseDependancy; ++ i )
	{
		if( pEffSet->bGetCRefBaseDpnd(i).GetChildIndex() == baseindex )
		{
			pOut->child		= pEffSet->bGetPtrEffBase( pEffSet->bGetCRefBaseDpnd(i).GetChildIndex() );
			pOut->parent	= pEffSet->bGetPtrEffBase( pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() );

			pOut->m_bdpnd	= true;
			pOut->m_bdpndParent	= false;

			if( dynamic_cast<const AgcdEffObj*>(pOut->parent) )
			{
				pOut->m_bdpndToObj	= true;
				for( int j=0; j<pOut->parent->m_stVarSizeInfo.m_nNumOfAnim; ++j )
				{
					if( dynamic_cast<const AgcdEffAnim_Missile*>( pOut->parent->bGetPtrEffAnim(j) ) )
					{
						pOut->m_bdpndToMissileObj	= true;
						break;
					}
				}
			}
			break;
		}
		else if( pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() == baseindex )
		{
			pOut->child		= pEffSet->bGetPtrEffBase( pEffSet->bGetCRefBaseDpnd(i).GetChildIndex() );
			pOut->parent	= pEffSet->bGetPtrEffBase( pEffSet->bGetCRefBaseDpnd(i).GetParentIndex() );

			pOut->m_bdpnd	= true;
			pOut->m_bdpndParent	= true;
			break;
		}
	}

	return pOut->m_bdpnd;
};
void CDlgTreeEffSet::OnTrctrleffsetInsertdependancy()
{	
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("m_ctrlTree.GetSelectedItem failed @ CDlgTreeEffSet::OnMenuItemTrCtrlDel");
		return;
	}

	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	UINT	ulEffSetID = pParam->GetEffSet()->bGetID();
	EffSet	cEffSet( ulEffSetID );
	ListEffSetItr Itr = std::find(m_listStEffSet.begin(), m_listStEffSet.end(), cEffSet );
	if( Itr == m_listStEffSet.end() )
	{
		ErrToWnd( Eff2Ut_FmtMsg("[id : %d] 추가되지 안은 아이디 입니다.", ulEffSetID) );
		return;
	}

	vInsEffBaseDpnd( pParam );
}

void CDlgTreeEffSet::OnMenuitemTrctrlBasecopy()
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("can't know to insert what kind of...");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	if( pParam->m_eType == Lparam::E_LPARAM_EFFBASE )
	{
		AgcuEffBaseCreater::bDestroy( m_lpEffBaseCopy );

		m_lpEffBaseCopy =
			AgcuEffBaseCreater::bCreate( pParam->GetEffBase()->bGetBaseType() );
		ASSERT( "kday" && m_lpEffBaseCopy );

		if( T_ISMINUS4( m_lpEffBaseCopy->bForTool_Clone( pParam->GetEffBase() ) ) )
		{
			AgcuEffBaseCreater::bDestroy( m_lpEffBaseCopy );
			ErrToWnd( "m_lpEffBaseCopy->bForTool_Clone failed" );
			return;
		}
		else if( dynamic_cast<AgcdEffRenderBase*>(m_lpEffBaseCopy) 
			&& !T_ISMINUS4(dynamic_cast<AgcdEffRenderBase*>(m_lpEffBaseCopy)->bGetTexIndex())
			)
		{
			LPEFFTEX	pEffTex =
			pParam->GetEffSet()->bGetPtrEffTex( dynamic_cast<AgcdEffRenderBase*>(m_lpEffBaseCopy)->bGetTexIndex() );
			ASSERT( "kday" && pEffTex );
			strcpy( this->m_szTexBaseCopy, pEffTex->bGetTexName() );
			strcpy( this->m_szMaskBaseCopy, pEffTex->bGetMaskName() );
		}
	}
}



void CDlgTreeEffSet::OnMenuitemTrctrlBasepaste()
{
	HTREEITEM hItem	= m_ctrlTreeData.GetSelectedItem();
	if( !hItem )
	{
		ToWnd("can't know to insert what kind of...");
		return;
	}
	LPLparam pParam = vGetLparam(hItem);
	if( !pParam )
	{
		ErrToWnd("!pParam");
		return;
	}

	if( pParam->m_eType == Lparam::E_LPARAM_EFFSET )
	{
		if( !m_lpEffBaseCopy )
		{
			ToWnd("붙여넣을 이펙트베이스가 엄슴..");
			return;
		}

		ToWnd( Eff2Ut_FmtMsg( "%s 를 추가합니다", AgcdEffBase::EffBaseTypeString( m_lpEffBaseCopy->bGetBaseType() ) ) );

		//텍스쳐가 필요한 이펙트?
		AgcdEffRenderBase*	pEffRenderBase	= 
			dynamic_cast<AgcdEffRenderBase*>(m_lpEffBaseCopy);
		if(  pEffRenderBase && !T_ISMINUS4(pEffRenderBase->bGetTexIndex()) )
		{
			LPEFFSET pEffSet = pParam->GetEffSet();
			ASSERT( "kday" && pEffSet );

			//있으면 해당 인덱스를, 없으면 추가해서 해당 인덱스를, 추가실패는 -1
			RwInt32 texIndex = 
				pEffSet->bForTool_InsEffEffTex( m_szTexBaseCopy, m_szMaskBaseCopy );

			if( T_ISMINUS4( texIndex ) )
			{
				//Err
				ErrToWnd( "pEffSet->bForTool_InsEffEffTex failed" );
				return;
			}


			pEffRenderBase->bSetTexIndex( texIndex );
			if( T_ISMINUS4( pEffRenderBase->bSetPtrEffTex( pEffSet->bGetPtrEffTex( texIndex ) ) ) )
			{
				//Err
				ErrToWnd( "pEffRenderBase->bSetPtrEffTex" );
				return;
			}
		}

		pParam->GetEffSet()->bForTool_InsEffBase( m_lpEffBaseCopy );
		Lparam	lparam( Lparam::E_LPARAM_EFFBASE, (LPVOID)pParam->GetEffSet(), (LPVOID)m_lpEffBaseCopy );
		bInsLparam( lparam, hItem );
		bRefreshSelect();
		vEffSetFlagUpdate( pParam->GetEffSet() );

		
		m_lpEffBaseCopy = NULL;
		strcpy(m_szMaskBaseCopy, "");
		strcpy(m_szTexBaseCopy, "");
	}
}
