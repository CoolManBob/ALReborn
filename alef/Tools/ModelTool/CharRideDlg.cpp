// CharRideDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "ModelToolDlg.h"
#include "CharRideDlg.h"
#include ".\charridedlg.h"

// CCharRideDlg dialog
IMPLEMENT_DYNAMIC( CCharRideDlg, CDialog )
CCharRideDlg::CCharRideDlg( CWnd* pParent /*=NULL*/ ) : CDialog( CCharRideDlg::IDD, pParent )
{
	m_pcsTreeCharacter = NULL;
}

CCharRideDlg::~CCharRideDlg()
{
}

void CCharRideDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHAR_RIDE_TREE, m_TreeRide);
}

BEGIN_MESSAGE_MAP(CCharRideDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_CHAR_RIDE_TREE, OnNMDblclkCharRideTree)
	ON_BN_CLICKED(IDC_BUTTON_DEATCH_RIDE, OnBnClickedButtonDeatchRide)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

BOOL CCharRideDlg::Create( CWnd* pParentWnd )
{
	return CDialog::Create( IDD, pParentWnd );
}

void CCharRideDlg::InsertChildTree( HTREEITEM hSourceParent, HTREEITEM hDestParent )
{
	HTREEITEM hNextItem, hChild, hDestChild;
	if( m_pcsTreeCharacter->ItemHasChildren( hSourceParent ) ) {
		hChild = m_pcsTreeCharacter->GetChildItem( hSourceParent );
		while( hChild )
		{
			hNextItem = m_pcsTreeCharacter->GetNextItem(hChild, TVGN_NEXT);

			CString	  cstrText;
			CString	  cstrTemp		= m_pcsTreeCharacter->GetItemText( hChild );
			DWORD_PTR dwpItemData	= m_pcsTreeCharacter->GetItemData( hChild );
			if( dwpItemData != E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER ) {
				cstrText.Format( "[%d]%s", dwpItemData, cstrTemp );
			}
			else {
				cstrText = cstrTemp;
			}
			hDestChild				= m_TreeRide._InsertItem( cstrText, hDestParent, dwpItemData );

			InsertChildTree( hChild, hDestChild );
			hChild = hNextItem;
		}
	}
}

BOOL CCharRideDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pcsTreeCharacter			= CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter();

	//@{ 2006/05/04 burumal
	//HTREEITEM hSourceRideParent	= m_pcsTreeCharacter->_FindItem( "[Å»°Í]" );
	HTREEITEM hSourceRideParent	= m_pcsTreeCharacter->_FindItemEx( "[Å»°Í]" );
	//@}

	if( hSourceRideParent == NULL )
		return TRUE;

	CString	  cstrText		 = m_pcsTreeCharacter->GetItemText( hSourceRideParent );
	DWORD_PTR dwpItemData	 = m_pcsTreeCharacter->GetItemData( hSourceRideParent );
	HTREEITEM hDestRideParent= m_TreeRide._InsertItem( cstrText, TVI_ROOT, dwpItemData );

	InsertChildTree( hSourceRideParent, hDestRideParent );	
	
	return TRUE;
}

void CCharRideDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CCharRideDlg::OnClose()
{
	CDialog::OnClose();
	CModelToolDlg::GetInstance()->CloseRideDlg();
}

void CCharRideDlg::OnNMDblclkCharRideTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelected		= m_TreeRide.GetSelectedItem();
	DWORD_PTR dwpItemData	= m_TreeRide.GetItemData( hSelected );

	if( dwpItemData == E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER ) {
		return;
	}

	int nID = CModelToolApp::GetInstance()->GetCurrentID();
	AgpdCharacter *pcsAgpdCharacter	= CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule()->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return;

	AgcdCharacter *pcsAgcdCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;

	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	pcsAgpdCharacter->m_bRidable = TRUE;
	pAgcmCharacter->AttachRide( pcsAgcdCharacter, dwpItemData );
	pAgcmCharacter->AttachRideClump( pcsAgcdCharacter->m_pRide );

	*pResult = 0;
}

void CCharRideDlg::OnBnClickedButtonDeatchRide()
{
	int nID = CModelToolApp::GetInstance()->GetCurrentID();
	AgpdCharacter *pcsAgpdCharacter	= CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule()->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return;

	AgcdCharacter *pcsAgcdCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;

	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	pcsAgpdCharacter->m_bRidable = FALSE;
	pAgcmCharacter->DetachRide( pcsAgcdCharacter );
}
