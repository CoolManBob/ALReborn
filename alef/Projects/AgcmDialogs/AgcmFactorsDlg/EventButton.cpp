// EventButton.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "EventButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventButton

CEventButton::CEventButton(CWnd *pcsParent, RECT &stInitRect, LPCSTR lpszCaption, BOOL bFlat, EventButtonCallback pfCallback, PVOID pvClass, PVOID pvData1, PVOID pvData2, PVOID pvData3)
{
	m_pfCallback	= pfCallback;
	m_pvClass		= pvClass;
	m_pvData1		= pvData1;
	m_pvData2		= pvData2;
	m_pvData3		= pvData3;

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
		m_pfCallback(m_pvClass, m_pvData1, m_pvData2, m_pvData3);
}

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
