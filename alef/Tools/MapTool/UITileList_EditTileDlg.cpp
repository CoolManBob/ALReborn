// UITileList_EditTileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "UITileList_EditTileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUITileList_EditTileDlg dialog


CUITileList_EditTileDlg::CUITileList_EditTileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUITileList_EditTileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUITileList_EditTileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUITileList_EditTileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUITileList_EditTileDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUITileList_EditTileDlg, CDialog)
	//{{AFX_MSG_MAP(CUITileList_EditTileDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUITileList_EditTileDlg message handlers
