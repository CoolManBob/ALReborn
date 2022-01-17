// XTPDockingPane.cpp : implementation of the CXTPDockingPaneclass.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TabManager/XTPTabManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPDockingPane.h"
#include "XTPDockingPaneContext.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPaneAutoHidePanel.h"
#include "XTPDockingPaneMiniWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPDockingPane::CXTPDockingPane(CXTPDockingPaneLayout* pLayout)
	: CXTPDockingPaneBase(xtpPaneTypeDockingPane, pLayout)
{
	m_dwOptions = 0;
	m_hwndChild = 0;
	m_nID = 0;
	m_nIconID = -1;
	m_dwData = 0;
	m_clrItemTab = COLORREF_NULL;
	m_nIDHelp = 0;
	m_bEnabled = xtpPaneEnabled;

	m_ptMinTrackSize = CPoint(0, 0);
	m_ptMaxTrackSize = CPoint(32000, 32000);

	EnableAutomation();


}

CXTPDockingPane::~CXTPDockingPane()
{

}

void CXTPDockingPane::SetID(int nID)
{
	ASSERT(nID != 0);

	m_nID = nID;
	m_strTitle.LoadString(nID);

}

void CXTPDockingPane::SetWindowRect(CRect rc)
{
	m_rcWindow = rc;
	m_szDocking = m_rcWindow.Size();
}

XTPXTPDockingPaneEnableOptions CXTPDockingPane::GetEnabled() const
{
	return m_bEnabled;
}

void CXTPDockingPane::SetEnabled(XTPXTPDockingPaneEnableOptions bEnabled)
{
	if (m_bEnabled != bEnabled)
	{
		m_bEnabled = bEnabled;
		InvalidatePane(TRUE);
	}
}

void CXTPDockingPane::OnSizeParent(CWnd* pParent, CRect rect, LPVOID lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	SetDockingSite(pParent);

	if (lpLayout == 0 || lpLayout->hDWP != NULL)
	{
		m_rcWindow = rect;

		if (pParent && m_pParentContainer)
		{
			MapWindowPoints(pParent->GetSafeHwnd(), m_pParentContainer->GetPaneHwnd(), (LPPOINT)&rect, 2);
			if (m_hwndChild) ::MoveWindow(m_hwndChild, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
			if (m_hwndChild) ::EnableWindow(m_hwndChild, m_bEnabled & xtpPaneEnableClient ? TRUE : FALSE);
		}
	}
}

void CXTPDockingPane::SetFocus()
{
	if (m_hwndChild)
	{
		if (!IsChild(m_hwndChild, ::GetFocus()))
			::SetFocus(m_hwndChild);
	}
}
BOOL CXTPDockingPane::IsFocus() const
{
	HWND hwndFocus = ::GetFocus();
	return (m_hwndChild != 0) && (hwndFocus == m_hwndChild || IsChild(m_hwndChild, hwndFocus));

}

void CXTPDockingPane::SetParentContainer(CXTPDockingPaneBase* pContainer)
{
	m_pParentContainer = pContainer;

	if (m_hwndChild == 0)
		return;

	if (pContainer)
	{
		ASSERT(pContainer->GetType() == xtpPaneTypeTabbedContainer);
		::SetParent(m_hwndChild, pContainer->GetPaneHwnd());

	}
	else
	{
		::ShowWindow(m_hwndChild, SW_HIDE);
		::SetParent(m_hwndChild, GetDockingPaneManager()->GetSafeHwnd());

		m_pDockingSite = NULL;
	}
}

void CXTPDockingPane::ShowWindow(BOOL bShow)
{
	if (bShow)
	{
		GetDockingPaneManager()->NotifyOwner(XTP_DPN_SHOWWINDOW, (LPARAM)this);
	}
	if (m_hwndChild)
	{
		::ShowWindow(m_hwndChild, bShow ? SW_SHOW : SW_HIDE);
	}
}

void CXTPDockingPane::Close()
{
	GetDockingPaneManager()->ClosePane(this);
}

void CXTPDockingPane::Hide()
{
	GetDockingPaneManager()->HidePane(this);
}

void CXTPDockingPane::Select()
{
	GetDockingPaneManager()->ShowPane(this);
}

CWnd* CXTPDockingPane::GetDockingSite() const
{
	return m_pParentContainer ? m_pParentContainer->m_pDockingSite : 0;

}

CFrameWnd* CXTPDockingPane::GetParentFrame() const
{
	CWnd* pSite = GetDockingSite();

	return pSite && pSite->IsFrameWnd() ? (CFrameWnd*)pSite : NULL;
}

void CXTPDockingPane::Copy(CXTPDockingPane* pClone)
{
	m_szDocking = pClone->m_szDocking;
	m_strTitle = pClone->m_strTitle;
	m_nIconID = pClone->m_nIconID;
	m_dwOptions = pClone->m_dwOptions;
	m_dwData = pClone->m_dwData;
	m_nID = pClone->m_nID;
	m_bEnabled = pClone->m_bEnabled;

	m_ptMinTrackSize = pClone->m_ptMinTrackSize;
	m_ptMaxTrackSize = pClone->m_ptMaxTrackSize;

	m_hwndChild = 0;
}

CXTPDockingPaneBase* CXTPDockingPane::Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD /*dwIgnoredOptions*/)
{
	ASSERT(pMap);

	CXTPDockingPane* pPane = (CXTPDockingPane*)GetDockingPaneManager()->OnCreatePane(GetType(), pLayout);

	pPane->Copy(this);

	pMap->SetAt(this, pPane);


	return pPane;
}


BOOL CXTPDockingPane::IsValid() const
{
	return m_hwndChild != NULL;

}
void CXTPDockingPane::Attach(CWnd* pWnd)
{
	if (pWnd)
	{
		m_hwndChild = pWnd->GetSafeHwnd();
		GetDockingPaneManager()->_Redraw();
		if (m_pParentContainer)
		{
			::SetParent(m_hwndChild, m_pParentContainer->GetPaneHwnd());
		}

		if (GetDockingPaneManager()->m_bInitialUpdateCalled)
		{
			pWnd->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);
		}
	}
}

CWnd* CXTPDockingPane::AttachView(CWnd* pParentWnd, CRuntimeClass* pViewClass, CDocument* pDocument/*= NULL*/, CCreateContext* pContext/*= NULL*/)
{
#ifdef _DEBUG
	ASSERT(pContext != NULL || pViewClass != NULL);
	ASSERT(pContext != NULL || pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(pContext != NULL || AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#endif

	CCreateContext contextT;
	if (pContext == NULL)
	{
		// if no context specified, generate one from the
		// currently selected client if possible.
		contextT.m_pLastView = NULL;
		contextT.m_pCurrentFrame = NULL;
		contextT.m_pNewDocTemplate = NULL;
		contextT.m_pCurrentDoc = pDocument;
		contextT.m_pNewViewClass = pViewClass;

		if (pDocument != NULL)
			contextT.m_pNewDocTemplate = pDocument->GetDocTemplate();

		pContext = &contextT;
	}

	CFrameWnd* pFrame = new CFrameWnd;

	pFrame->Create(NULL, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CRect(0, 0, 0, 0), pParentWnd, NULL, 0, pContext);

	pFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	Attach(pFrame);

	CWnd* pView = pFrame->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);

	if (DYNAMIC_DOWNCAST(CView, pView))
	{
		pFrame->SetActiveView((CView*)pView);
	}

	return pView;
}

void CXTPDockingPane::Detach()
{
	if (m_hwndChild)
	{
		::ShowWindow(m_hwndChild, SW_HIDE);
		::SetParent(m_hwndChild, GetDockingPaneManager()->GetSite()->GetSafeHwnd());
		m_hwndChild = 0;
	}
	GetDockingPaneManager()->_Redraw();
}

CString CXTPDockingPane::GetTitle() const
{
	int nIndex = m_strTitle.Find('\n');
	if (nIndex == -1)
		return m_strTitle;
	return m_strTitle.Left(nIndex);
}

CString CXTPDockingPane::GetShortTitle() const
{
	int nIndex = m_strTitle.Find('\n');
	if (nIndex == -1)
		return m_strTitle;
	return m_strTitle.Mid(nIndex + 1);
}

void CXTPDockingPane::SetTitle(LPCTSTR lpszTitle)
{
	m_strTitle = lpszTitle;
	InvalidatePane(FALSE);
}

BOOL CXTPDockingPane::IsClosed() const
{
	return m_pParentContainer == NULL;
}

BOOL CXTPDockingPane::IsHidden() const
{
	if (m_pParentContainer == NULL)
		return FALSE;

	return m_pParentContainer->IsHidden();
}

BOOL CXTPDockingPane::IsSelected() const
{
	if (m_pParentContainer == NULL) return FALSE;
	ASSERT(m_pParentContainer->GetType() == xtpPaneTypeTabbedContainer);
	return ((CXTPDockingPaneTabbedContainer*)m_pParentContainer)->GetSelected() == this;
}

BOOL CXTPDockingPane::IsFloating() const
{
	if (m_pParentContainer == NULL) return FALSE;
	if (IsHidden()) return FALSE;
	ASSERT(m_pParentContainer->GetType() == xtpPaneTypeTabbedContainer);

	if (m_pParentContainer->GetPaneHwnd() == 0) return FALSE;

	CWnd* pFrame = m_pParentContainer->GetDockingSite();
	return pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CXTPDockingPaneMiniWnd));
}


CXTPImageManagerIcon* CXTPDockingPane::GetIcon(int nWidth) const
{
	return GetDockingPaneManager()->GetIcon(GetIconID(), nWidth);
}


void CXTPDockingPane::SetItemColor(COLORREF clr)
{
	m_clrItemTab = clr;
	InvalidatePane(FALSE);
}

COLORREF CXTPDockingPane::GetItemColor() const
{
	if (m_clrItemTab != COLORREF_NULL)
		return m_clrItemTab;

	if (m_hwndChild)
	{
		COLORREF clr = (COLORREF)::SendMessage(m_hwndChild, WM_XTP_GETTABCOLOR, 0, 0);
		if (clr != 0)
			return clr;
	}
	return xtpTabColorBlue + GetID() % 8;
}

void CXTPDockingPane::GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo) const
{
	ZeroMemory(pMinMaxInfo, sizeof(MINMAXINFO));
	pMinMaxInfo->ptMinTrackSize = m_ptMinTrackSize;
	pMinMaxInfo->ptMaxTrackSize = m_ptMaxTrackSize;
}

DWORD CXTPDockingPane::GetOptions() const
{
	return GetDockingPaneManager()->m_dwDefaultPaneOptions | m_dwOptions;
}

void CXTPDockingPane::DeletePane()
{
	InternalRelease();
}

//////////////////////////////////////////////////////////////////////////
// Accessible

CCmdTarget* CXTPDockingPane::GetAccessible()
{
	return this;
}

HRESULT CXTPDockingPane::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispParent = NULL;

	if (m_pParentContainer)
	{
		*ppdispParent = ((CXTPDockingPaneTabbedContainer*)m_pParentContainer)->GetIDispatch(TRUE);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CXTPDockingPane::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = m_hwndChild ? 1 : 0;

	return S_OK;
}

HRESULT CXTPDockingPane::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispChild = NULL;

	if (GetChildIndex(&varChild) == 1)
	{
		return AccessibleObjectFromWindow(m_hwndChild, OBJID_WINDOW, IID_IDispatch, (void**)ppdispChild);
	}

	return S_OK;
}

HRESULT CXTPDockingPane::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszName = GetShortTitle().AllocSysString();
	return S_OK;
}

HRESULT CXTPDockingPane::GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDescription = GetTitle().AllocSysString();
	return S_OK;
}

HRESULT CXTPDockingPane::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;

	if (GetChildIndex(&varChild) == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_PAGETAB;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPDockingPane::AccessibleSelect(long /*flagsSelect*/, VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	Select();

	return S_OK;
}


HRESULT CXTPDockingPane::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_SELECTABLE;

	if (!m_pParentContainer)
		pvarState->lVal |= STATE_SYSTEM_INVISIBLE;

	if (m_pParentContainer && ((CXTPDockingPaneTabbedContainer*)m_pParentContainer)->GetSelected() == this)
		pvarState->lVal |= STATE_SYSTEM_SELECTED;

	return S_OK;
}

HRESULT CXTPDockingPane::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDefaultAction = SysAllocString(L"Switch");

	return S_OK;
}

HRESULT CXTPDockingPane::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	Select();

	return S_OK;
}

HRESULT CXTPDockingPane::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	CRect rc;

	if (GetChildIndex(&varChild) == 1)
	{
		GetWindowRect(m_hwndChild, &rc);
	}
	else if (GetChildIndex(&varChild) != CHILDID_SELF)
	{
		return E_INVALIDARG;
	}
	else
	{
		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)m_pParentContainer;
		if (!pContainer)
			return S_FALSE;

		pContainer->GetWindowRect(&rc);

		if (pContainer->GetItemCount() > 1)
		{
			for (int i = 0; i < pContainer->GetItemCount(); i++)
			{
				CXTPTabManagerItem* pItem = pContainer->GetItem(i);
				if (pItem->GetData() == (DWORD_PTR)this)
				{
					CRect rcItem = pItem->GetRect();
					rc = CRect(CPoint(rc.left + rcItem.left, rc.top + rcItem.top), rcItem.Size());
					break;
				}
			}
		}
	}


	*pxLeft = rc.left;
	*pyTop = rc.top;
	*pcxWidth = rc.Width();
	*pcyHeight = rc.Height();

	return S_OK;
}

HRESULT CXTPDockingPane::AccessibleHitTest(long /*xLeft*/, long /*yTop*/, VARIANT* pvarID)
{
	pvarID->vt = VT_I4;
	pvarID->lVal = 0;

	return S_OK;
}





BEGIN_INTERFACE_MAP(CXTPDockingPane, CCmdTarget)
	INTERFACE_PART(CXTPDockingPane, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()
