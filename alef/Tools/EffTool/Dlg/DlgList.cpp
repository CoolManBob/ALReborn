// DlgList.cpp : implementation file
//

#include "stdafx.h"
#include "..\EffTool.h"
#include "DlgList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------- CBrowseForFolder -----------------------
CBrowseForFolder::CBrowseForFolder() : 
 hWndOwner(NULL),
 Image(0),
 flags( BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS )
{
}

bool CBrowseForFolder::GetFolder(CString &returnPath, const char *i_InitialDirectory)
{
	// set initial directory
	strStartupDir = i_InitialDirectory;

	// get the pidl of the root 
	LPITEMIDLIST pidlRoot = NULL;
	GetPidl(strDirRoot, pidlRoot);
	
	TCHAR strBuff[MAX_PATH + 1];
	
	returnPath.Empty();

	LPMALLOC pMalloc;
	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		BROWSEINFO brInfo = 
		{
			hWndOwner, 
			pidlRoot, 
			strBuff, 
			strTitle,
			flags,
			BrowseCallbackProc,
			(LPARAM)&strStartupDir,
			Image
		};

		LPITEMIDLIST pidl = SHBrowseForFolder(&brInfo);
		if ( pidl )
		{
			SHGetPathFromIDList(pidl, strBuff);
			pMalloc->Free(pidl);
			returnPath = strBuff;
		}
		if( pidlRoot ) 
			pMalloc->Free(pidlRoot);

		pMalloc->Release();					
	}

	return returnPath.IsEmpty() ? false : true;
}

void CBrowseForFolder::GetPidl(CString name, LPITEMIDLIST pidl)
{
	LPSHELLFOLDER pshf;
	ULONG chEaten;
	
#ifdef _UNICODE
	OLECHAR* oleRoot = name.GetBuffer(name.GetLength());
#else
	OLECHAR oleRoot[MAX_PATH];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, name, -1, oleRoot, MAX_PATH );	// convert path to Unicode string
#endif	

	if (SUCCEEDED(SHGetDesktopFolder(&pshf)))
	{
		// get pidl
		pshf->ParseDisplayName(hWndOwner, NULL, oleRoot, &chEaten, &pidl, NULL);
		pshf->Release();
	}
}

void CBrowseForFolder::SetEditBox(bool val)
{
	if (val)	flags |= BIF_EDITBOX;
	else		flags &= ~BIF_EDITBOX;
}

void CBrowseForFolder::SetStatusBar(bool val)
{
	if (val)	flags |= BIF_STATUSTEXT;
	else		flags &= ~BIF_STATUSTEXT;
		
}

int CALLBACK CBrowseForFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED: 
		{
			// set the initial directory
			CString* pString = (CString*)lpData;
			if( !pString )		return 0;
				
			LPTSTR szDir = pString->GetBuffer( pString->GetLength() );
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);             
		}
		break;
	case BFFM_SELCHANGED: 
		{
			// Set the status window to the currently selected path.
			TCHAR szDir[MAX_PATH];
			if (SHGetPathFromIDList((LPITEMIDLIST) lParam , szDir)) 
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
		}
		break;
	default:
		break;         
	}         
	return 0;
}

//------------------------------ CDlgList ------------------------------
CDlgList::CDlgList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgList::IDD, pParent)
	, m_bInit(FALSE)
	, ptEdit(1,5)
	, PTLIST(1,35)
{
	//{{AFX_DATA_INIT(CDlgList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgList)
	DDX_Control(pDX, IDC_LIST_DATA, m_ctrlListData);
	DDX_Control(pDX, IDC_EDIT_INFO, m_ctrlEditInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgList, CDialog)
	//{{AFX_MSG_MAP(CDlgList)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgList message handlers
BOOL CDlgList::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_ctrlListData.SetExtendedStyle( m_ctrlListData.GetExtendedStyle() /*| LVS_EX_FULLROWSELECT*/ );
	
	return TRUE;
}

void CDlgList::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( !m_bInit )
		return;

	CRect	rc(0,0,0,0);
	CRect	rcCtrl(0,0,0,0);

	if( m_ctrlEditInfo.GetSafeHwnd() )
	{
		this->GetWindowRect(&rc);
		rc.DeflateRect(ptEdit.x, ptEdit.y, ptEdit.x, 0);
		m_ctrlEditInfo.GetWindowRect(&rcCtrl);
		ScreenToClient(&rc);
		rc.bottom = rc.top+rcCtrl.Height();
		m_ctrlEditInfo.MoveWindow(&rc);
	}

	if( m_ctrlListData.GetSafeHwnd() )
	{
		this->GetWindowRect(&rc);
		rc.DeflateRect(PTLIST.x, PTLIST.y,PTLIST.x, PTLIST.x);
		ScreenToClient(&rc);
		m_ctrlListData.MoveWindow(&rc);	
	}
}

INT	CALLBACK CDlgList::CB_CmpByInt_Up(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem)
{
	ListCtrlAndSubItem* pCtrlAndSubItem	= (ListCtrlAndSubItem*)pListCtrlAndSubItem;

	LVFINDINFO	fi;
	fi.flags	= LVFI_PARAM;
	fi.lParam	= lp1;
	INT n1	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	fi.lParam	= lp2;
	INT n2	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	CString sz1	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n1, pCtrlAndSubItem->m_nSubItem );
	CString sz2	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n2, pCtrlAndSubItem->m_nSubItem );
	
	return ( atoi(sz1.GetBuffer(0)) > atoi( sz2.GetBuffer(0) ) );
};

INT	CALLBACK CDlgList::CB_CmpByInt_Down(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem)
{
	ListCtrlAndSubItem* pCtrlAndSubItem	= (ListCtrlAndSubItem*)pListCtrlAndSubItem;

	LVFINDINFO	fi;
	fi.flags	= LVFI_PARAM;
	fi.lParam	= lp1;
	INT n1	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	fi.lParam	= lp2;
	INT n2	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	CString sz1	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n1, pCtrlAndSubItem->m_nSubItem );
	CString sz2	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n2, pCtrlAndSubItem->m_nSubItem );
	
	return ( atoi(sz1.GetBuffer(0)) < atoi( sz2.GetBuffer(0) ) );
};

INT	CALLBACK CDlgList::CB_CmpByString_Up(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem)
{
	ListCtrlAndSubItem* pCtrlAndSubItem	= (ListCtrlAndSubItem*)pListCtrlAndSubItem;

	LVFINDINFO	fi;
	fi.flags	= LVFI_PARAM;
	fi.lParam	= lp1;
	INT n1	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	fi.lParam	= lp2;
	INT n2	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	CString sz1	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n1, pCtrlAndSubItem->m_nSubItem );
	CString sz2	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n2, pCtrlAndSubItem->m_nSubItem );
	
	return strcmp( sz1, sz2 );
};

INT	CALLBACK CDlgList::CB_CmpByString_Down(LPARAM lp1, LPARAM lp2, LPARAM pListCtrlAndSubItem)
{
	ListCtrlAndSubItem* pCtrlAndSubItem	= (ListCtrlAndSubItem*)pListCtrlAndSubItem;

	LVFINDINFO	fi;
	fi.flags	= LVFI_PARAM;
	fi.lParam	= lp1;
	INT n1	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	fi.lParam	= lp2;
	INT n2	= pCtrlAndSubItem->m_pListCtrl->FindItem( &fi );

	CString sz1	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n1, pCtrlAndSubItem->m_nSubItem );
	CString sz2	= pCtrlAndSubItem->m_pListCtrl->GetItemText( n2, pCtrlAndSubItem->m_nSubItem );
	
	return strcmp( sz2, sz1 );
};
