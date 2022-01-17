// AdvComboEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AdvComboEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit

BEGIN_MESSAGE_MAP(CAdvComboEdit, CEdit)
	//{{AFX_MSG_MAP(CAdvComboEdit)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAdvComboEdit::CAdvComboEdit()
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::CAdvComboEdit\n"));
}

CAdvComboEdit::~CAdvComboEdit()
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::~CAdvComboEdit\n"));
}

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit message handlers

void CAdvComboEdit::OnTimer(UINT nIDEvent) 
{
	KillTimer(nIDEvent);
	SetForegroundWindow();
	
	CEdit::OnTimer(nIDEvent);
}

void CAdvComboEdit::PreSubclassWindow() 
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::PreSubclassWindow\n"));
	SetTimer(1, 100, NULL);	
	CEdit::PreSubclassWindow();
}
