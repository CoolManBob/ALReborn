#include "stdafx.h"
#include "..\EffTool.h"
#include "DlgEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgEdit::CDlgEdit(CWnd* pParent /*=NULL*/) : CDialog(CDlgEdit::IDD, pParent), 
 m_bInit(FALSE)
{
	m_strBuff = _T("");
}

void CDlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_INFO, m_ctrlWnd);
	DDX_Text(pDX, IDC_EDIT_INFO, m_strBuff);
}

BEGIN_MESSAGE_MAP(CDlgEdit, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CDlgEdit message handlers

BOOL CDlgEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return m_bInit = TRUE;
}


void CDlgEdit::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( !m_bInit )		return;
	
	CRect	rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_ctrlWnd.MoveWindow( &rc );
}

void CDlgEdit::bAddString(LPCTSTR szIn)
{
	if( !m_bInit )		return;
			
	m_strBuff += szIn;
	m_strBuff += _T("\r\n");
	UpdateData(FALSE);

	INT	nMin, nMax;
	m_ctrlWnd.GetScrollRange(SB_VERT, &nMin, &nMax);
	m_ctrlWnd.LineScroll(nMax-nMin);
	
	//UpdateData(FALSE);
};

void CDlgEdit::bClear(void)
{
	if( !m_bInit )		return;

	m_strBuff.Delete(0, m_strBuff.GetLength());
	m_ctrlWnd.Clear();

	UpdateData(FALSE);
};

void CDlgEdit::bSetMargins(UINT nLeft, UINT nRight)
{
	if( !m_bInit )		return;
		

	m_ctrlWnd.SetMargins(nLeft, nRight);
};
