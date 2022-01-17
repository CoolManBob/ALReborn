#include "stdafx.h"
#include "..\EffTool.h"
#include "DlgStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgStatic::CDlgStatic(CWnd* pParent /*=NULL*/) : CDialog(CDlgStatic::IDD, pParent), m_bInit(FALSE)
{
}

void CDlgStatic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStatic)
	DDX_Control(pDX, IDC_STATIC_RECT, m_ctrlStatic);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgStatic, CDialog)
	//{{AFX_MSG_MAP(CDlgStatic)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStatic message handlers
void CDlgStatic::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( !m_bInit )
		return;

	CRect	rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctrlStatic.MoveWindow( &rc );
}

void CDlgStatic::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgStatic::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
