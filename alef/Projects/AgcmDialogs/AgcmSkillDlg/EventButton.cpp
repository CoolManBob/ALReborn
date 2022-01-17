// EventButton.cpp : implementation file
//

#include "stdafx.h"
#include "agcmskilldlg.h"
#include "EventButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventButton
/*
CEventButton::CEventButton
//(CWnd *pcsParent, RECT &stInitRect, LPCSTR lpszCaption, BOOL bFlat, EventButtonCallback pfCallback, CEventButtonParams *pData)
(CWnd *pcsParent, RECT &stInitRect,	LPCSTR lpszCaption, BOOL bFlat, EventButtonCallback pfCallback, CEventButtonParams *pData)
*/

CEventButton::~CEventButton()
{
}


BEGIN_MESSAGE_MAP(CEventButton, CButton)
	//{{AFX_MSG_MAP(CEventButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventButton message handlers
VOID CEventButton::_ShowButton()
{
	ShowWindow(SW_SHOW);
	EnableWindow(TRUE);
}

VOID CEventButton::_HideButton()
{
	ShowWindow(SW_HIDE);
	EnableWindow(FALSE);
}

VOID CEventButton::_SetWindowText(LPCTSTR lpszString)
{
	SetWindowText(lpszString);
}

void CEventButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	if (m_pfCallback)
		m_pfCallback(&m_csData);
}
