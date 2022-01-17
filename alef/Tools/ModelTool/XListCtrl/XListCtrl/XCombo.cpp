// XCombo.cpp : implementation file
//

#include "stdafx.h"
#include "XCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT WM_XCOMBOLIST_COMPLETE  = ::RegisterWindowMessage(_T("WM_XCOMBOLIST_COMPLETE"));
//UINT WM_XCOMBOLIST_VK_ESCAPE = ::RegisterWindowMessage(_T("WM_XCOMBOLIST_VK_ESCAPE"));

/////////////////////////////////////////////////////////////////////////////
// CXCombo

BEGIN_MESSAGE_MAP(CXCombo, CAdvComboBox)
	//{{AFX_MSG_MAP(CXCombo)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXCombo::CXCombo(CWnd * pParent) :
	m_pParent(pParent),
	CAdvComboBox(FALSE, pParent)
{
	XLISTCTRL_TRACE(_T("in CXCombo::CXCombo\n"));
}

CXCombo::~CXCombo()
{
	XLISTCTRL_TRACE(_T("in CXCombo::~CXCombo\n"));
}

///////////////////////////////////////////////////////////////////////////////
// SendRegisteredMessage
void CXCombo::SendRegisteredMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd = m_pParent;
	if (pWnd)
		pWnd->SendMessage(nMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// CXCombo message handlers

void CXCombo::OnKillFocus(CWnd* pNewWnd) 
{
	XLISTCTRL_TRACE(_T("in CXCombo::OnKillFocus\n"));

	CAdvComboBox::OnKillFocus(pNewWnd);

	if (pNewWnd != GetDropWnd())
	{
		SendRegisteredMessage(WM_XCOMBOLIST_COMPLETE, 0, 0);
	}
}

void CXCombo::OnEscapeKey()
{
	XLISTCTRL_TRACE(_T("in CXCombo::OnEscapeKey\n"));

	SendRegisteredMessage(WM_XCOMBOLIST_VK_ESCAPE, 0, 0);
}

void CXCombo::OnComboComplete()
{
	XLISTCTRL_TRACE(_T("in CXCombo::OnComboComplete\n"));

	SendRegisteredMessage(WM_XCOMBOLIST_COMPLETE, 0, 0);
}
