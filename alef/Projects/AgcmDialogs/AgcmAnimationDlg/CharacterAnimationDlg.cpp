#include "stdafx.h"
#include "../resource.h"
#include "AgcmAnimationDlg.h"
#include "characteranimationdlg.h"
#include "AgcmFileListDlg.h"
#include "CharAnimAttachedDataDlg.h"
#include ".\characteranimationdlg.h"

IMPLEMENT_DYNAMIC(CCharacterAnimationDlg, CDialog)

BEGIN_MESSAGE_MAP(CCharacterAnimationDlg::CMyTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCharacterAnimationDlg::CMyTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == 'c' || nChar == 'C' )
	{
		if ( GetKeyState(VK_LCONTROL) < 0 )
		{
			HTREEITEM hSelectedItem	= GetSelectedItem();
			if (!hSelectedItem)
				return;

			CString strItemText = GetItemText(hSelectedItem);
			
			if ( OpenClipboard() ) 
			{ 
				HANDLE hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (_tcslen(strItemText.GetString()) + 1) * sizeof(TCHAR)); 
				if(hglbCopy) { 
					EmptyClipboard(); 
					LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
					_tcscpy(lptstrCopy, strItemText.GetString()); 
					
					GlobalUnlock(hglbCopy); 

#ifdef _UNICODE 
					SetClipboardData(CF_UNICODETEXT,hglbCopy); 
#else 
					SetClipboardData(CF_TEXT,hglbCopy); 
#endif 								
				} 

				CloseClipboard();
			}
		}
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

//-------------------------- CCharacterAnimationDlg ----------------------------
CCharacterAnimationDlg::CCharacterAnimationDlg(AgcdCharacterTemplate* pstAgcdCharacterTemplate, CWnd* pParent /*=NULL*/)
 : CDialog(CCharacterAnimationDlg::IDD, pParent)
{
	ASSERT( AgcmAnimationDlg::GetInstance() );

	m_pstAgcdCharacterTemplate = pstAgcdCharacterTemplate;
}

CCharacterAnimationDlg::~CCharacterAnimationDlg()
{
}

void CCharacterAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ANIMATION, m_csTreeAnimation);
}

BEGIN_MESSAGE_MAP(CCharacterAnimationDlg, CDialog)
	ON_COMMAND(ID_CAD_ON_COMMAND_DATA, OnCommandData)
	ON_COMMAND(ID_CAD_ON_COMMAND_FLAGS, OnCommandFlags)
	ON_COMMAND(ID_CAD_ON_COMMAND_REMOVE_1, OnCommandRemove)
	ON_COMMAND(ID_CAD_ON_COMMAND_REMOVE, OnCommandRemoveAll)
	ON_COMMAND(ID_CAD_ON_COMMAND_ADD, OnCommandAdd)
	ON_COMMAND(ID_CAD_ON_COMMAND_START, OnCommandStart)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ANIMATION, OnNMRclickTreeAnimation)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ANIMATION, OnNMDblclkTreeAnimation)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

BOOL CCharacterAnimationDlg::Create(AgcdCharacterTemplate *pstAgcdCharacterTemplate, CWnd* pParentWnd)
{
	m_pstAgcdCharacterTemplate	= pstAgcdCharacterTemplate;

	return CDialog::Create(IDD_CHARACTER_ANIMATION, pParentWnd);
}

HTREEITEM CCharacterAnimationDlg::_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData)
{
	HTREEITEM hItem = m_csTreeAnimation.InsertItem(lpszItem, hParent, 0);

	if( dwData && hItem )
		m_csTreeAnimation.SetItemData(hItem, dwData);

	return hItem;
}

CMenu *CCharacterAnimationDlg::_CreatePopupMenu()
{
	CMenu* pcsMenu = new CMenu; 
	if( !pcsMenu )		return NULL;

	if( !pcsMenu->CreatePopupMenu() )
	{
		delete pcsMenu;
		return NULL;
	}

	return pcsMenu;
}

BOOL CCharacterAnimationDlg::_AppendString(CMenu *pcsMenu, UINT32 ulIDCommand, CHAR *pszTextItem)
{
	return pcsMenu->AppendMenu( MF_STRING, ulIDCommand, pszTextItem );
}

BOOL CCharacterAnimationDlg::_AppendSeparator(CMenu *pcsMenu)
{
	return pcsMenu->AppendMenu( MF_SEPARATOR );
}

BOOL CCharacterAnimationDlg::_TrackPopupMenu(CMenu *pcsMenu)
{
	POINT	stPoint;
	GetCursorPos(&stPoint);

	if (!pcsMenu->TrackPopupMenu(TPM_LEFTALIGN, stPoint.x, stPoint.y, this))
		return FALSE;

	if (!pcsMenu->DestroyMenu())
		return FALSE;

	delete pcsMenu;
	pcsMenu = NULL;

	return TRUE;
}

AgcdCharacterAnimation *CCharacterAnimationDlg::GetCharacterAnimation(HTREEITEM hAnimType2)
{
	if (!hAnimType2)		return NULL;
		
	CString		strType2	= m_csTreeAnimation.GetItemText(hAnimType2);
	INT32		lAnimType2	= AgcmAnimationDlg::GetInstance()->GetAnimType2Index(strType2);
	if (lAnimType2 == -1)	return NULL;

	HTREEITEM	hAnimType	= m_csTreeAnimation.GetParentItem(hAnimType2);
	if (!hAnimType)			return NULL;

	CString		strType		= m_csTreeAnimation.GetItemText(hAnimType);
	INT32		lAnimType	= AgcmAnimationDlg::GetInstance()->GetAnimTypeIndex(strType);
	if (lAnimType == -1)	return NULL;
		

	if ( !m_pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2] )
		m_pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]	= new AgcdCharacterAnimation();

	return m_pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2];
}

VOID CCharacterAnimationDlg::OnCommandAdd()
{
	HTREEITEM				hAnimType2		= m_csTreeAnimation.GetSelectedItem();
	AgcdCharacterAnimation*	pcsCharAnim		= GetCharacterAnimation(hAnimType2);
	if ( !pcsCharAnim )
		return;

	AgcmFileListDlg dlg;
	CHAR* pszTemp = dlg.OpenFileList( AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath1(), AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath2(), NULL );
	if ( !pszTemp )
		return;

	AAD_AddAnimationParams	csAddAnimationParams;
	csAddAnimationParams.m_ppSrcAnimation	= &pcsCharAnim->m_pcsAnimation;
	csAddAnimationParams.m_pszSrcAnimation	= pszTemp;
	AgcmAnimationDlg::GetInstance()->AddAnimation(&csAddAnimationParams);
	if( !csAddAnimationParams.m_pDestData )
		return;

	AAD_ReadRtAnimParams	csReadRtAnimParams;
	csReadRtAnimParams.m_pcsSrcAnimData		= csAddAnimationParams.m_pDestData;
	AgcmAnimationDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);
	if ( !csReadRtAnimParams.m_bRead )
		return;

	if ( !_InsertItem(pszTemp, hAnimType2, (DWORD)(csReadRtAnimParams.m_pcsSrcAnimData) ) )
		return;
}

VOID	CCharacterAnimationDlg::OnCommandRemove()
{
	HTREEITEM	hSelectedItem	= m_csTreeAnimation.GetSelectedItem();
	if( !hSelectedItem )	return;

	AgcdAnimData2* pcsData		= (AgcdAnimData2*)m_csTreeAnimation.GetItemData( hSelectedItem );
	if( !pcsData )			return;

	HTREEITEM	hNextItem	= m_csTreeAnimation.GetNextItem( hSelectedItem, TVGN_NEXT );

	//AgcdCharacter* pClientCharacter = AgcmAnimationDlg::GetInstance()->GetCurAgcdCharacter();
	//if( pClientCharacter && pClientCharacter->m_pcsCurAnimData )
	//{
	//	if( !strcmp( pClientCharacter->m_pcsCurAnimData->m_pszRtAnimName, pcsData->m_pszRtAnimName ) )
	//	{
	//		MessageBox( "현재 Play 되고 있는 Animation 파일입니다.", "Error", MB_OK );
	//		return;
	//	}
	//}

	HTREEITEM	hParentItem = m_csTreeAnimation.GetParentItem( hSelectedItem );
	if( !hParentItem )		return;

	AgcdCharacterAnimation*	pcsCharAnim	= GetCharacterAnimation( hParentItem );
	if( !pcsCharAnim )		return;
		
	AAD_RemoveAnimationParams	cRemoveRarams;
	cRemoveRarams.m_pSrcAnimation	= pcsCharAnim->m_pcsAnimation;
	cRemoveRarams.m_szName			= pcsData->m_pszRtAnimName;

	AgcmAnimationDlg::GetInstance()->RemoveAnimation( &cRemoveRarams );

	if( cRemoveRarams.m_bRemovedAnimation )
		m_csTreeAnimation.DeleteItem( hSelectedItem );
}

VOID CCharacterAnimationDlg::OnCommandRemoveAll()
{
	HTREEITEM				hAnimType2	= m_csTreeAnimation.GetSelectedItem();
	AgcdCharacterAnimation*	pcsCharAnim	= GetCharacterAnimation(hAnimType2);
	if (!pcsCharAnim)
		return;

	AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;
	csRemoveAllAnimationParams.m_ppSrcAnimation	= &pcsCharAnim->m_pcsAnimation;

	AgcmAnimationDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);
	if (!csRemoveAllAnimationParams.m_bRemovedAnimation)
		return;

	HTREEITEM hNextItem = m_csTreeAnimation.GetNextItem(hAnimType2, TVGN_CHILD);
	HTREEITEM hRemoveItem = NULL;
	while (hNextItem)
	{
		hRemoveItem	= hNextItem;
		hNextItem	= m_csTreeAnimation.GetNextItem( hNextItem, TVGN_NEXT );

		m_csTreeAnimation.DeleteItem(hRemoveItem);
	}
}

VOID CCharacterAnimationDlg::OnCommandFlags()
{
	AgcdCharacterAnimation	*pcsCharAnim	= GetCharacterAnimation(m_csTreeAnimation.GetSelectedItem());
	if (!pcsCharAnim)
		return;

	if (!pcsCharAnim->m_pstAnimFlags)
		AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->GetAgcaAnimation2()->AddFlags(&pcsCharAnim->m_pstAnimFlags );

	if (!pcsCharAnim->m_pstAnimFlags)
		return;

	AgcmAnimationDlg::GetInstance()->OpenAnimFlags(pcsCharAnim->m_pstAnimFlags, NULL );
}

VOID CCharacterAnimationDlg::OnCommandData()
{
	HTREEITEM	hSelectedItem	= m_csTreeAnimation.GetSelectedItem();
	if (!hSelectedItem)
		return;

	AgcdAnimData2*	pcsData		= (AgcdAnimData2 *)(m_csTreeAnimation.GetItemData(hSelectedItem));
	if (!pcsData)
		return;

	CCharAnimAttachedDataDlg	dlg(pcsData);
	dlg.DoModal();
}

VOID	CCharacterAnimationDlg::OnCommandStart()
{
	HTREEITEM	hSelectedItem = m_csTreeAnimation.GetSelectedItem();
	if( !hSelectedItem )		return;

	HTREEITEM	hAnimType2Item = m_csTreeAnimation.GetParentItem(hSelectedItem);
	if( !hAnimType2Item )		return;

	HTREEITEM	hAnimTypeItem = m_csTreeAnimation.GetParentItem(hAnimType2Item);
	if( !hAnimTypeItem )		return;

	CString		strAnimType	= m_csTreeAnimation.GetItemText( hAnimTypeItem );
	CString		strAnimType2= m_csTreeAnimation.GetItemText( hAnimType2Item );
	INT32		lAnimType	= AgcmAnimationDlg::GetInstance()->GetAnimTypeIndex( strAnimType );
	INT32		lAnimType2	= AgcmAnimationDlg::GetInstance()->GetAnimType2Index( strAnimType2 );
	if (lAnimType == -1)		return;
		
	DWORD		dwData		= m_csTreeAnimation.GetItemData( hSelectedItem );
	if (!dwData)				return;

	AgcdAnimData2* pcsAnimData = (AgcdAnimData2 *)(dwData);

	AAD_StartAnimationParams	csStartAnimationParams;
	csStartAnimationParams.m_lAnimType		= lAnimType;
	csStartAnimationParams.m_lAnimType2		= lAnimType2;
	csStartAnimationParams.m_pcsAnimData	= pcsAnimData;

	AgcmAnimationDlg::GetInstance()->StartAnimation( &csStartAnimationParams );
}

BOOL CCharacterAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AgcmCharacter	*pcsAgcmCharacter	= AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule();
	if (!pcsAgcmCharacter)
		return FALSE;

	if (!m_pstAgcdCharacterTemplate)
		return FALSE;

	INT32			lMaxAnimType2		= pcsAgcmCharacter->GetAnimType2Num(m_pstAgcdCharacterTemplate);
	CHAR**			paszAnimType		= AgcmAnimationDlg::GetInstance()->GetAnimTypeName();
	CHAR**			paszAnimType2		= AgcmAnimationDlg::GetInstance()->GetAnimType2Name(m_pstAgcdCharacterTemplate->m_lAnimType2);

	HTREEITEM		hType				= NULL;
	HTREEITEM		hType2				= NULL;
	AgcdAnimData2*	pcsCurrentAnimData	= NULL;

	if ( !m_pstAgcdCharacterTemplate->m_pacsAnimationData )
		if ( !AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->AllocateAnimationData( m_pstAgcdCharacterTemplate ) )
			return FALSE;

	HTREEITEM		hFirst;
	for ( INT32 lType = 0; lType < AGCMCHAR_MAX_ANIM_TYPE; ++lType )
	{
		hType	= _InsertItem(paszAnimType[lType], TVI_ROOT, E_CAD_TREE_ITEM_ANIM_TYPE);
		if (!hType)
			return FALSE;

		m_csTreeAnimation.SetItemState(hType, TVIS_BOLD, TVIS_BOLD);

		for(INT32 lType2 = 0; lType2 < lMaxAnimType2; ++lType2)
		{
			hType2	= _InsertItem(paszAnimType2[lType2], hType, E_CAD_TREE_ITEM_ANIM_TYPE2);
			if ( !hType2 )		return FALSE;

			if( !m_pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2] || 
				!m_pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation )
			{
				continue;
			}

			pcsCurrentAnimData	= m_pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation->m_pcsHead;
			while( pcsCurrentAnimData )
			{
				if (pcsCurrentAnimData->m_pszRtAnimName)
					if ( !_InsertItem( pcsCurrentAnimData->m_pszRtAnimName, hType2, (DWORD)pcsCurrentAnimData ) )
						return FALSE;

				pcsCurrentAnimData	= pcsCurrentAnimData->m_pcsNext;
			}
		}

		m_csTreeAnimation.Expand(hType, TVE_EXPAND);
		if ( lType == 0 )
			hFirst = hType;
	}

	m_csTreeAnimation.SelectItem(hFirst);

	return TRUE;
}

void CCharacterAnimationDlg::OnClose()
{
	CDialog::OnClose();

	AgcmAnimationDlg::GetInstance()->CloseCharAnim();
}

void CCharacterAnimationDlg::OnNMRclickTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CPoint csPoint;
	::GetCursorPos(&csPoint);
	m_csTreeAnimation.ScreenToClient(&csPoint);

	HTREEITEM hSelectedItem = m_csTreeAnimation.HitTest(csPoint);
	if  (!hSelectedItem)
		return;

	m_csTreeAnimation.SelectItem(hSelectedItem);

	CMenu *pcsMenu	= _CreatePopupMenu();
	if (!pcsMenu)
		return;

	DWORD_PTR	dwpData	= m_csTreeAnimation.GetItemData(hSelectedItem);
	switch (dwpData)
	{
	case E_CAD_TREE_ITEM_ANIM_TYPE:
		{
		}
		break;

	case E_CAD_TREE_ITEM_ANIM_TYPE2:
		{
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_ADD,		D_CAD_TEMPLATE_NAME_ADD);
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_REMOVE,	D_CAD_TEMPLATE_NAME_REMOVE);
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_FLAGS,		D_CAD_TEMPLATE_NAME_FLAGS);
		}
		break;

	default:	// AgcdAnimData2
		{
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_REMOVE_1,	D_CAD_TEMPLATE_NAME_REMOVE);
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_DATA,		D_CAD_TEMPLATE_NAME_DATA);
			_AppendString(pcsMenu, ID_CAD_ON_COMMAND_START,		D_CAD_TEMPLATE_NAME_START);
		}
		break;
	}

	_TrackPopupMenu(pcsMenu);
}

void CCharacterAnimationDlg::OnNMDblclkTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	HTREEITEM	hSelectedItem = m_csTreeAnimation.GetSelectedItem();
	if( !hSelectedItem )
		return;

	HTREEITEM	hAnimType2Item = m_csTreeAnimation.GetParentItem(hSelectedItem);
	if( !hAnimType2Item )
		return;

	HTREEITEM	hAnimTypeItem = m_csTreeAnimation.GetParentItem(hAnimType2Item);
	if( !hAnimTypeItem )
		return;

	CString		strAnimType	= m_csTreeAnimation.GetItemText( hAnimTypeItem );
	CString		strAnimType2= m_csTreeAnimation.GetItemText( hAnimType2Item );
	INT32		lAnimType	= AgcmAnimationDlg::GetInstance()->GetAnimTypeIndex( strAnimType );
	INT32		lAnimType2	= AgcmAnimationDlg::GetInstance()->GetAnimType2Index( strAnimType2 );
	if (lAnimType == -1)
		return;

	DWORD		dwData		= m_csTreeAnimation.GetItemData( hSelectedItem );
	if (!dwData)
		return;

	AgcdAnimData2	*pcsAnimData	= (AgcdAnimData2 *)(dwData);

	AAD_StartAnimationParams	csStartAnimationParams;
	csStartAnimationParams.m_lAnimType		= lAnimType;
	csStartAnimationParams.m_lAnimType2		= lAnimType2;
	csStartAnimationParams.m_pcsAnimData	= pcsAnimData;

	AgcmAnimationDlg::GetInstance()->StartAnimation( &csStartAnimationParams );
}

void CCharacterAnimationDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
