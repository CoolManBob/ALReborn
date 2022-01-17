// EventButton.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "EventButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventButton

CEventButton::CEventButton(CWnd *pcsParent, RECT &stInitRect, LPCSTR lpszCaption, BOOL bFlat, EventButtonCallback pfCallback, PVOID pvClass)
{
	m_pfCallback	= pfCallback;
	m_pvClass		= pvClass;
	m_hBackBrush	= NULL;

	DWORD dwFlag = (WS_CHILD | WS_VISIBLE);
	if(bFlat)
		dwFlag |= BS_FLAT;

	Create(lpszCaption, dwFlag, stInitRect, pcsParent, NULL);
}

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

void CEventButton::OnClicked() 
{
	// TODO: Add your control notification handler code here
	if(m_pfCallback)
		m_pfCallback(m_pvClass);
}

/*HBRUSH CEventButton::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	if(m_hBackBrush)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_hBackBrush;
	}
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}*/

VOID CEventButton::_ShowButton()
{
	//ModifyStyle(0, WS_VISIBLE);
	ShowWindow(SW_SHOW);
	EnableWindow(TRUE);
}

VOID CEventButton::_HideButton()
{
	//ModifyStyle(WS_VISIBLE, 0);
	ShowWindow(SW_HIDE);
	EnableWindow(FALSE);
}

VOID CEventButton::_SetWindowText(LPCTSTR lpszString)
{
	SetWindowText(lpszString);
}