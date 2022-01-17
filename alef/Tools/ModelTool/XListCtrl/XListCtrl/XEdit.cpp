// XEdit.cpp : implementation file
//

#include "stdafx.h"
#include "XEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT WM_XEDIT_KILL_FOCUS = ::RegisterWindowMessage(_T("WM_XEDIT_KILL_FOCUS"));
UINT WM_XEDIT_VK_ESCAPE  = ::RegisterWindowMessage(_T("WM_XEDIT_VK_ESCAPE"));

///////////////////////////////////////////////////////////////////////////////
// message map

BEGIN_MESSAGE_MAP(CXEdit, CEdit)
	//{{AFX_MSG_MAP(CXEdit)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXEdit::CXEdit(CWnd *pParent, LPCTSTR lpszText) :
	m_pParent(pParent),
	m_strText(lpszText),
	m_bMessageSent(FALSE)
{
	XLISTCTRL_TRACE(_T("in CXEdit::CXEdit\n"));
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXEdit::~CXEdit()
{
	XLISTCTRL_TRACE(_T("in CXEdit::~CXEdit\n"));
}

///////////////////////////////////////////////////////////////////////////////
// SendRegisteredMessage
void CXEdit::SendRegisteredMessage(UINT nMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	BOOL bMessageSent = m_bMessageSent;
	m_bMessageSent = TRUE;
	if (m_pParent && ::IsWindow(m_pParent->m_hWnd) && !bMessageSent)
		m_pParent->SendMessage(nMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CXEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	XLISTCTRL_TRACE(_T("in CXEdit::OnCreate\n"));

	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the proper font
	if (!m_pParent)
	{
		m_pParent = GetParent();
	}

	if (m_pParent && ::IsWindow(m_pParent->m_hWnd))
	{
		CFont * pFont = m_pParent->GetFont();
		if (pFont)
			SetFont(pFont);
	}
	else
	{
		m_pParent = NULL;
	}

	SetWindowText(m_strText);
	SetSel(0, -1);
	SetFocus();
	SetCapture();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXEdit::OnPaint() 
{
	CRect rect;
	GetClientRect(&rect);
	rect.right += 2;

	CDC * pDC = GetDC();
	pDC->FillSolidRect(&rect, ::GetSysColor(COLOR_WINDOW));
	ReleaseDC(pDC);

	CEdit::OnPaint();		// let CEdit draw the text

	//rect.right -= 2;
	rect.left -= 1;
	rect.top -= 1;
	rect.bottom -=1;

	pDC = GetDC();

	// don't erase the text that CEdit has just drawn
	CBrush * pOldBrush = (CBrush *) pDC->SelectStockObject(NULL_BRUSH);
	//CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_INACTIVECAPTION));	// same as combobox
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->Rectangle(&rect);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// PreTranslateMessage
BOOL CXEdit::PreTranslateMessage(MSG* pMsg)
{
	// handle WM_KEYDOWN in case the edit has focus - otherwise
	// it will be sent to parent, bypassing WM_CHAR
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			SendRegisteredMessage(WM_XEDIT_KILL_FOCUS);
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			SendRegisteredMessage(WM_XEDIT_VK_ESCAPE);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////////////////////////////////
// OnChar
void CXEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// handle escape and return, in case edit does NOT have focus

	if (m_pParent)
	{
		if (nChar == VK_ESCAPE)
		{
			SendRegisteredMessage(WM_XEDIT_VK_ESCAPE);
			return;
		}
		else if (nChar == VK_RETURN)
		{
			SendRegisteredMessage(WM_XEDIT_KILL_FOCUS);
			return;
		}
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////
// OnKillFocus
void CXEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	SendRegisteredMessage(WM_XEDIT_KILL_FOCUS);
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXEdit::OnDestroy() 
{
	XLISTCTRL_TRACE(_T("in CXEdit::OnDestroy\n"));
	ReleaseCapture();
	CEdit::OnDestroy();
}
