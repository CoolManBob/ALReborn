// XTPControl.cpp : implementation of the CXTPControl class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#include "Resource.h"
#include "Common/Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"

#include "XTPControl.h"
#include "XTPControls.h"
#include "XTPMouseManager.h"
#include "XTPCommandBar.h"
#include "XTPPaintManager.h"
#include "XTPToolbar.h"
#include "XTPCommandBars.h"
#include "XTPPopupBar.h"
#include "XTPCustomizeTools.h"
#include "XTPSoundManager.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPControlAction, CCmdTarget)

//////////////////////////////////////////////////////////////////////////
// CXTPControlAction

CXTPControlAction::CXTPControlAction(CXTPControlActions* pActions)
{

	m_nId = 0;
	m_nIconId = 0;
	m_nHelpId = 0;
	m_nTag = 0;
	m_bVisible = TRUE;
	m_bChecked = FALSE;
	m_bEnabled = TRUE;

	m_pTarget = NULL;

	m_pActions = pActions;
}

CXTPControlAction::~CXTPControlAction()
{
	ASSERT(m_arrControls.GetSize() == 0);

	if (m_pTarget)
	{
		CMDTARGET_RELEASE(m_pTarget);
	}
}

void CXTPControlAction::AddControl(CXTPControl* pControl)
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		if (m_arrControls[i] == pControl)
			return;
	}

	m_arrControls.Add(pControl);

	pControl->m_bEnabled = -1;
	pControl->m_bChecked = -1;
	pControl->m_nId = m_nId;

	pControl->m_strCaption.Empty();
	pControl->m_strTooltipText.Empty();
	pControl->m_strDescriptionText.Empty();
	pControl->m_strCategory.Empty();
	pControl->m_strShortcutText.Empty();
}

void CXTPControlAction::RemoveControl(CXTPControl* pControl)
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		if (m_arrControls[i] == pControl)
		{
			m_arrControls.RemoveAt(i);

			pControl->m_bEnabled = TRUE;
			pControl->m_bChecked = FALSE;
			return;
		}
	}
}

void CXTPControlAction::OnChanged(int nProperty)
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		m_arrControls[i]->OnActionChanged(nProperty);
	}
}

void CXTPControlAction::OnChanging(int nProperty)
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		m_arrControls[i]->OnActionChanging(nProperty);
	}
}

void CXTPControlAction::RedrawControls()
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		m_arrControls[i]->RedrawParent();
	}
}

void CXTPControlAction::RepositionControls()
{
	for (int i = 0; i < m_arrControls.GetSize(); i++)
	{
		m_arrControls[i]->DelayLayoutParent();
	}
}

void CXTPControlAction::OnRemoved()
{
	while (m_arrControls.GetSize() > 0)
	{
		m_arrControls[0]->SetAction(NULL);
	}
}

void CXTPControlAction::SetCaption(UINT nIDCaption)
{
	CString strCaption;
	if (strCaption.LoadString(nIDCaption))
	{
		SetCaption(strCaption);
	}
}

void CXTPControlAction::SetCaption(LPCTSTR lpszCaption)
{
	CString strCaption(lpszCaption);
	int nShortCutPos = strCaption.Find ('\t');
	if (nShortCutPos != -1)
	{
		strCaption.ReleaseBuffer(nShortCutPos);
	}

	if (m_strCaption != strCaption)
	{
		OnChanging(3);
		m_strCaption = strCaption;
		OnChanged(3);

		RepositionControls();
	}
}

void CXTPControlAction::SetEditHint(LPCTSTR lpszEditHint)
{
	if (m_strEditHint != lpszEditHint)
	{
		OnChanging(4);
		m_strEditHint = lpszEditHint;
		OnChanged(4);
	}
}

void CXTPControlAction::SetPrompt(LPCTSTR lpszPrompt)
{
	if (lpszPrompt == NULL || lpszPrompt[0] == _T('\0'))
		return;

	if (_tcschr(lpszPrompt, _T('\n')) != NULL)
	{
		AfxExtractSubString(m_strTooltipText, lpszPrompt, 1);
		AfxExtractSubString(m_strDescriptionText, lpszPrompt, 0);
	}
	else
	{
		m_strDescriptionText = m_strTooltipText = lpszPrompt;
	}
}

BOOL CXTPControlAction::GetEnabled() const
{
	if (GetCommandBars()->IsCustomizeMode())
		return TRUE;

	return m_bEnabled;
}

CXTPCommandBars* CXTPControlAction::GetCommandBars() const
{
	return m_pActions->m_pCommandBars;
}

CXTPControl* CXTPControlAction::GetControl(int nIndex) const
{
	return m_arrControls[nIndex];
}

int CXTPControlAction::GetCount() const
{
	return (int)m_arrControls.GetSize();
}


//////////////////////////////////////////////////////////////////////////
// CXTPControlActions

CXTPControlActions::CXTPControlActions(CXTPCommandBars* pCommandBars)
{
	m_pCommandBars = pCommandBars;

}

CXTPControlActions::~CXTPControlActions()
{
	RemoveAll();
}

CXTPControlAction* CXTPControlActions::GetAt(int nIndex) const
{
	return m_arrActions.GetAt(nIndex);
}

int CXTPControlActions::GetCount() const
{
	return (int)m_arrActions.GetSize();
}

CXTPControlAction* CXTPControlActions::FindAction(int nId) const
{
	if (nId <= 0 || nId >= 0xFFFFFFF)
		return NULL;

	int num = GetCount();
	if (num == 0)
		return NULL;

	CXTPControlAction* const* lo = m_arrActions.GetData();
	CXTPControlAction* const* hi = lo + (num - 1);
	CXTPControlAction* const* mid;

	while (lo <= hi)
	{
		int half = num / 2;
		if (half)
		{
			mid = lo + (num & 1 ? half : (half - 1));

			if ((*mid)->GetID() == nId)
				return (*mid);

			if ((*mid)->GetID() > nId)
			{
				hi = mid - 1;
				num = num & 1 ? half : half - 1;
			}
			else
			{
				lo = mid + 1;
				num = half;
			}
		}
		else if (num)
			return (*lo)->GetID() == nId ? (*lo) : NULL;
		else
			break;
	}

	return NULL;
}

void CXTPControlActions::RemoveAll()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControlAction* pAction = GetAt(i);

		pAction->OnRemoved();
		pAction->InternalRelease();
	}
	m_arrActions.RemoveAll();
}

void CXTPControlActions::Insert(CXTPControlAction* pAction)
{
	int nIndex = 0;
	for (; nIndex < GetCount(); nIndex++)
	{
		if (GetAt(nIndex)->GetID() > pAction->GetID())
			break;
	}

	m_arrActions.InsertAt(nIndex, pAction);
}

CXTPControlAction* CXTPControlActions::Add(int nId, CXTPControlAction* pAction)
{
	ASSERT(FindAction(nId) == NULL);

	pAction->m_nId = nId;

	CString strPrompt;
	if (XTPResourceManager()->LoadString(&strPrompt, nId))
	{
		pAction->SetPrompt(strPrompt);
	}

	Insert(pAction);

	return pAction;
}

CXTPControlAction* CXTPControlActions::Add(int nId)
{
	CXTPControlAction* pAction = FindAction(nId);
	if (pAction)
		return pAction;

	pAction = new CXTPControlAction(this);
	return Add(nId, pAction);
}

void CXTPControlActions::CreateFromMenu(CMenu* pMenu)
{
	int nCount = ::GetMenuItemCount(pMenu->m_hMenu);

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		// Check to see if the item is a separator, we don't want
		// to use GetMenuItemID(i) because it may not equal zero.

		MENUITEMINFO info = { sizeof(MENUITEMINFO), MIIM_TYPE | MIIM_STATE};
		::GetMenuItemInfo(pMenu->m_hMenu, nIndex, TRUE, &info);

		int nID = (int)pMenu->GetMenuItemID(nIndex);
		BOOL bSeparator = ((info.fType & MFT_SEPARATOR) == MFT_SEPARATOR) || (nID == 0);

		if (!bSeparator)
		{
			CMenu* pPopupMenu = pMenu->GetSubMenu(nIndex);

			CString strCaption;
			pMenu->GetMenuString(nIndex, strCaption, MF_BYPOSITION);

			if (pPopupMenu)
			{
				nID = 0;
			}

			int iPos = strCaption.Find(_T('\t'));
			if (pPopupMenu &&  iPos > 0)
			{
				nID = _ttoi(strCaption.Mid(iPos + 1));
				strCaption.ReleaseBuffer(iPos);
			}

			if (nID > 0)
			{
				CXTPControlAction* pAction = Add(nID);

				pAction->SetCaption(strCaption);

				CString strPrompt;
				if (XTPResourceManager()->LoadString(&strPrompt, nID))
				{
					pAction->SetPrompt(strPrompt);
				}
			}

			if (pPopupMenu)
			{
				CreateFromMenu(pPopupMenu);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// CXTPControl

CXTPControl::CXTPControl()
{
	EnableAutomation();


	m_nId = 0;
	m_nIconId = 0;
	m_nHelpId = 0;
	m_nCustomIconId = 0;
	m_nTag = 0;
	m_nIndex = 0;
	m_dwFlags = 0;
	m_rcControl.SetRectEmpty();
	m_rcRow.SetRectEmpty();

	m_pParent = 0;
	m_pControls = 0;
	m_bExpanded = FALSE;

	m_bChecked = FALSE;
	m_bEnabled = TRUE;
	m_bBeginGroup = FALSE;
	m_bEnabled = TRUE;
	m_bTemporary = FALSE;
	m_bSelected = FALSE;
	m_bPressed = FALSE;

	m_bWrap = FALSE;

	m_dwHideFlags = xtpNoHide;
	m_controlType = xtpControlError;

	m_bDefaultItem = FALSE;
	m_buttonStyle = xtpButtonAutomatic;
	m_buttonCustomStyle = xtpButtonUndefined;
	m_buttonRibbonStyle = xtpButtonAutomatic;

	m_bCloseSubMenuOnClick = TRUE;

	m_pRibbonGroup = NULL;

	m_pAction = 0;

	m_nWidth = 0;
	m_nHeight = 0;

	m_nExecuteOnPressInterval = 0;
}

CXTPControl::~CXTPControl()
{
	if (m_pAction)
	{
		m_pAction->RemoveControl(this);
		m_pAction = NULL;
	}
}

void CXTPControl::SetAction(CXTPControlAction* pAction)
{
	if (m_pAction)
	{
		m_pAction->RemoveControl(this);
		m_pAction = NULL;
	}

	if (pAction)
	{
		m_pAction = pAction;
		m_pAction->AddControl(this);
	}
}

void CXTPControl::OnRemoved()
{
	SetAction(NULL);
}

XTPButtonStyle CXTPControl::GetStyle() const
{
	return m_buttonCustomStyle != xtpButtonUndefined ? m_buttonCustomStyle :
		m_buttonStyle != xtpButtonAutomatic ? m_buttonStyle :
	m_buttonRibbonStyle != xtpButtonAutomatic || !m_pParent ? m_buttonRibbonStyle : m_pParent->GetDefaultButtonStyle();
}

CSize CXTPControl::GetSize(CDC* pDC)
{
	return GetPaintManager()->DrawControl(pDC, this, FALSE);
}

void CXTPControl::Draw(CDC* pDC)
{
	GetPaintManager()->DrawControl(pDC, this);
}

void CXTPControl::DelayRedrawParent()
{
	if (m_pParent && IsWindow(m_pParent->GetSafeHwnd()))
		((CXTPCommandBar*)m_pParent)->DelayRedraw();
}

void CXTPControl::DelayLayoutParent()
{
	if (m_pParent) m_pParent->DelayLayout();
}

void CXTPControl::RedrawParent(BOOL bAnimate)
{
	if (!IsVisible())
		return;

	if (m_pParent) m_pParent->Redraw(GetRect(), bAnimate);
}

void CXTPControl::ScreenToClient(CPoint* point)
{
	ASSERT(m_pParent);
	if (m_pParent)
		((CWnd*)m_pParent)->ScreenToClient(point);
}


BOOL CXTPControl::OnSetSelected(int bSelected)
{
	if (bSelected == m_bSelected)
		return FALSE;

	if (IsKeyboardSelected(bSelected) && m_pParent->m_nPopuped != GetIndex())
		m_pParent->SetPopuped(-1);

	m_bSelected = bSelected;

	m_pParent->OnControlSelected(bSelected, this);

	if (GetEnabled() || IsKeyboardSelected(bSelected) || !bSelected)
		RedrawParent(!bSelected);

	return TRUE;
}

void CXTPControl::OnMouseHover()
{
	if (m_pParent->SetPopuped(-1))
	{
		if (m_pParent->IsTrackingMode() == TRUE_POPUP && m_pParent->GetPosition() != xtpBarPopup)
			m_pParent->SetTrackingMode(FALSE);
	}
}

void CXTPControl::SetPrompt(LPCTSTR lpszPrompt)
{
	if (lpszPrompt == NULL || lpszPrompt[0] == _T('\0'))
		return;

	if (_tcschr(lpszPrompt, _T('\n')) != NULL)
	{
		AfxExtractSubString(m_strTooltipText, lpszPrompt, 1);
		AfxExtractSubString(m_strDescriptionText, lpszPrompt, 0);
	}
	else
	{
		m_strDescriptionText = m_strTooltipText = lpszPrompt;
	}

	if (m_strCaption.IsEmpty())
	{
		if (!AfxExtractSubString(m_strCaption, lpszPrompt, 2))
			m_strCaption = m_strTooltipText;
	}
}

void CXTPControl::SetID(int nId)
{
	if (m_nId == nId)
		return;

	CString strPrompt;

	if (XTPResourceManager()->LoadString(&strPrompt, nId))
	{
		SetPrompt(strPrompt);
	}

	m_nId = nId;
}

void CXTPControl::SetCaption(UINT nIDCaption)
{
	CString strCaption;
	if (strCaption.LoadString(nIDCaption))
	{
		SetCaption(strCaption);
	}
}

void CXTPControl::SetCaption(LPCTSTR lpszCaption)
{
	CString strCaption(lpszCaption);
	BOOL bDelayLayout = FALSE;

	int nShortCutPos = strCaption.Find ('\t');
	if (nShortCutPos != -1)
	{
		CString strShortcutTextAuto = strCaption.Mid(nShortCutPos + 1);

		if (m_strShortcutTextAuto != strShortcutTextAuto)
		{
			m_strShortcutTextAuto = strShortcutTextAuto;
			bDelayLayout = TRUE;
		}
		strCaption.ReleaseBuffer(nShortCutPos);
	}

	if (m_strCaption != strCaption)
	{
		m_strCaption = strCaption;
		bDelayLayout = TRUE;
	}

	if (bDelayLayout)
	{
		DelayLayoutParent();
	}
}

AFX_INLINE void NotifyExecute(CXTPControl* pControl, CWnd* pOwner)
{
	NMXTPCONTROL tagNMCONTROL;
	if (pControl->NotifySite(pOwner, CBN_XTP_EXECUTE, &tagNMCONTROL) == 0)
	{
		pOwner->SendMessage(WM_COMMAND, pControl->GetID());
	}
}

//Click Helper
void CXTPControl::ClickToolBarButton(CRect rcActiveRect)
{
	#define XTP_TID_CLICKTICK 0x10AD

	if (rcActiveRect.IsRectEmpty())
		rcActiveRect = m_rcControl;

	m_pParent->SetSelected(m_nIndex);
	m_bPressed = TRUE;
	RedrawParent();

	InternalAddRef();

	BOOL bExecuteOnTimer = m_nExecuteOnPressInterval > 0;
	CWnd* pOwner = m_pParent->GetOwnerSite();

	m_pParent->SetCapture();
	HWND hWndCapture = m_pParent->GetSafeHwnd();
	CPoint pt(0, 0);
	BOOL bClick = FALSE;

	if (bExecuteOnTimer)
	{
		m_pParent->SetTimer(XTP_TID_CLICKTICK, m_nExecuteOnPressInterval, NULL);
		NotifyExecute(this, pOwner);
	}

	while (::GetCapture() == hWndCapture)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (msg.message == WM_LBUTTONUP)
		{
			bClick = m_bSelected && ((!pt.x && !pt.y) || rcActiveRect.PtInRect(pt));
			break;
		}

		if (m_pParent == NULL)
			break;

		if (msg.message == WM_TIMER && msg.wParam == XTP_TID_CLICKTICK)
		{
			if (m_bSelected)
			{
				NotifyExecute(this, pOwner);
			}
		}

		if (msg.message == WM_MOUSEMOVE)
		{
			pt = CPoint(LOWORD(msg.lParam), HIWORD(msg.lParam));
			if (OnSetSelected(rcActiveRect.PtInRect(pt)))
			{
				RedrawParent();
			}
			continue;
		}

		DispatchMessage (&msg);
	}

	if (bExecuteOnTimer && m_pParent->GetSafeHwnd())
	{
		m_pParent->KillTimer(XTP_TID_CLICKTICK);
	}

	m_bPressed = bClick && !bExecuteOnTimer && NeedPressOnExecute() ? TRUE_KEYBOARD : FALSE;

	ReleaseCapture();

	if (m_bSelected && m_pParent)
	{
		m_pParent->SetSelected(-1);
		m_bSelected = FALSE;
		m_pParent->m_nClickedControl = GetIndex();
	}

	if (bClick && !bExecuteOnTimer)
	{
		OnExecute();

		if (m_bPressed)
		{
			m_bPressed = FALSE;
			RedrawParent();
		}
	}
	else
	{
		RedrawParent();
	}

	InternalRelease();
}

LRESULT CXTPControl::NotifySite(UINT code)
{
	if (!m_pParent)
		return 0;

	NMXTPCONTROL tagNMCONTROL;
	return NotifySite(m_pParent->GetOwnerSite(), code, &tagNMCONTROL);
}

LRESULT CXTPControl::NotifySite(UINT code, NMXTPCONTROL* pNM)
{
	if (!m_pParent)
		return 0;

	return NotifySite(m_pParent->GetOwnerSite(), code, pNM);
}

LRESULT CXTPControl::NotifySite(CWnd* pSite, UINT code, NMXTPCONTROL* pNM)
{
	if (pSite == 0)
	{
		if (!m_pParent)
			return 0;

		pSite = m_pParent->GetOwnerSite();
	}

	pNM->hdr.code = code ;
	pNM->hdr.idFrom = GetID();
	pNM->hdr.hwndFrom = 0;
	pNM->pControl = this;

	LRESULT lResult = pSite->SendMessage(WM_XTP_COMMAND, GetID(), (LPARAM)pNM);

	if (lResult || !m_pParent)
		return lResult;

	AFX_NOTIFY notify;
	notify.pResult = &lResult;
	notify.pNMHDR = (NMHDR*)pNM;

	if (pSite->OnCmdMsg(GetID(), MAKELONG(code, WM_NOTIFY), &notify, NULL))
	{
		return lResult;
	}

	return 0;
}


BOOL CXTPControl::NeedPressOnExecute() const
{
	if (m_pParent && m_pParent->GetPosition() == xtpBarPopup)
		return FALSE;

	return TRUE;
}

void CXTPControl::OnExecute()
{
	XTPSoundManager()->PlaySystemSound(xtpSoundMenuCommand);

	m_bPressed = NeedPressOnExecute() ? TRUE_KEYBOARD : FALSE;

	CXTPCommandBar* pCommandBar = m_pParent->GetRootParent();
	BOOL bRecursePopup = pCommandBar->m_bRecursePopup;

	CWnd* pOwner = m_pParent->GetOwnerSite();
	CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();

	if (pCommandBar->m_pReturnCmd)
	{
		if (bRecursePopup)
			pCommandBar->OnTrackLost();
		else pCommandBars->ClosePopups();

		*pCommandBar->m_pReturnCmd = m_nId;
		return;
	}


	if (pCommandBars)
	{
		if (m_nId) pCommandBars->SetCommandUsed(m_nId);

		CXTPPopupBar* pPopupBar = DYNAMIC_DOWNCAST(CXTPPopupBar, m_pParent);
		if (pPopupBar && pPopupBar->GetControlPopup() && ((CXTPControl*)pPopupBar->GetControlPopup())->GetID())
		{
			pCommandBars->SetCommandUsed(((CXTPControl*)pPopupBar->GetControlPopup())->GetID());
		}
	}
	InternalAddRef();

	if (m_bCloseSubMenuOnClick)
	{
		if (bRecursePopup)
			pCommandBar->OnTrackLost();
		else pCommandBars->ClosePopups();
	}

	if (m_nId == 0)
	{
		InternalRelease();
		return;
	}

	NotifyExecute(this, pOwner);

	if ((!m_bCloseSubMenuOnClick || m_bPressed) && m_pParent)
	{
		m_bPressed = FALSE;

		m_pParent->OnIdleUpdateCmdUI(0, 0);
		RedrawParent();
	}

	InternalRelease();
}

BOOL CXTPControl::IsCursorOver() const
{
	CPoint pt;
	GetCursorPos(&pt);
	m_pParent->ScreenToClient(&pt);
	return m_rcControl.PtInRect(pt);
}

void CXTPControl::Copy(CXTPControl* pControl, BOOL /*bRecursive*/)
{
	m_nId = pControl->m_nId;
	SetAction(pControl->GetAction());

	m_nTag = pControl->m_nTag;
	m_dwFlags = pControl->m_dwFlags;
	m_controlType = pControl->m_controlType;
	m_strCaption = pControl->m_strCaption;
	m_strShortcutText = pControl->m_strShortcutText;
	m_strShortcutTextAuto = pControl->m_strShortcutTextAuto;
	m_strTooltipText = pControl->m_strTooltipText;
	m_strDescriptionText = pControl->m_strDescriptionText;
	m_strParameter = pControl->m_strParameter;
	m_nCustomIconId = pControl->m_nCustomIconId;
	m_nIconId = pControl->m_nIconId;
	m_nHelpId = pControl->m_nHelpId;
	m_bTemporary = pControl->m_bTemporary;
	m_strCustomCaption = pControl->m_strCustomCaption;
	m_strCategory = pControl->m_strCategory;
	m_dwHideFlags = pControl->m_dwHideFlags;
	m_bDefaultItem = pControl->m_bDefaultItem;
	m_bEnabled = pControl->m_bEnabled;
	m_bChecked = pControl->m_bChecked;
	m_buttonCustomStyle = pControl->m_buttonCustomStyle;
	m_buttonStyle = pControl->m_buttonStyle;

	m_nWidth = max(GetCustomizeMinWidth(), pControl->m_nWidth);
	m_nHeight = pControl->m_nHeight;

	m_bBeginGroup = pControl->m_bBeginGroup;

	m_bCloseSubMenuOnClick = pControl->m_bCloseSubMenuOnClick;
	m_nExecuteOnPressInterval = pControl->m_nExecuteOnPressInterval;

	m_mapDocTemplatesAssigned.Copy(pControl->m_mapDocTemplatesAssigned);
	m_mapDocTemplatesExcluded.Copy(pControl->m_mapDocTemplatesExcluded);
}

BOOL CXTPControl::Compare(CXTPControl* pOther)
{
	if (GetStyle() != pOther->GetStyle())
		return FALSE;

	if (GetType() != pOther->GetType())
		return FALSE;

	if (GetID() != pOther->GetID())
		return FALSE;

	if (GetFlags() != pOther->GetFlags())
		return FALSE;

	if (GetBeginGroup() != pOther->GetBeginGroup())
		return FALSE;

	if (GetIconId() != pOther->GetIconId())
		return FALSE;

	if (m_strCaption != pOther->m_strCaption)
		return FALSE;

	if (m_strTooltipText != pOther->m_strTooltipText)
		return FALSE;

	if (m_strDescriptionText != pOther->m_strDescriptionText)
		return FALSE;

	if (m_strCustomCaption != pOther->m_strCustomCaption)
		return FALSE;

	if (m_buttonCustomStyle != pOther->m_buttonCustomStyle)
		return FALSE;

	if (m_strParameter != pOther->m_strParameter)
		return FALSE;

	if ((m_dwHideFlags & xtpHideCustomize) != (pOther->m_dwHideFlags & xtpHideCustomize))
		return FALSE;

	if (m_nWidth != pOther->m_nWidth)
		return FALSE;

	return TRUE;
}

IMPLEMENT_XTP_CONTROL(CXTPControl, CCmdTarget)


BOOL CXTPControl::IsCustomizeMode() const
{
	return m_pParent ? m_pParent->IsCustomizeMode() : FALSE;
}

void CXTPControl::OnInvertTracker(CDC* pDC, CRect rect)
{
	ASSERT(!rect.IsRectEmpty());

	pDC->InvertRect(CRect(rect.left, rect.top, rect.right, rect.top + 2));
	pDC->InvertRect(CRect(rect.left, rect.bottom - 2, rect.right, rect.bottom));
	pDC->InvertRect(CRect(rect.left, rect.top + 2, rect.left + 2, rect.bottom - 2));
	pDC->InvertRect(CRect(rect.right - 2, rect.top + 2, rect.right, rect.bottom - 2));

}

void CXTPControl::OnCustomizeMouseMove(CPoint point)
{
	if (IsCustomizeResizeAllow() && m_rcControl.PtInRect(point) && ((point.x - m_rcControl.left <= 2) || (m_rcControl.right - point.x <= 2)))
	{
		::SetCursor(XTPResourceManager()->LoadCursor(XTP_IDC_VRESIZE));
	}
}

BOOL CXTPControl::CustomizeStartResize(CPoint point)
{
	if (m_rcControl.PtInRect(point) && ((point.x - m_rcControl.left <= 2) || (m_rcControl.right - point.x <= 2)))
	{
		CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();
		ASSERT(pCommandBars);
		ASSERT(pCommandBars->m_pDragSelected == this);
		pCommandBars->m_pDragSelected = NULL;
		m_pParent->Redraw();
		m_pParent->UpdateWindow();

		CRect rectTracker = m_rcControl;
		m_pParent->ClientToScreen(rectTracker);

		m_pParent->SetCapture();
		::SetCursor(XTPResourceManager()->LoadCursor(XTP_IDC_VRESIZE));

		CDC* pDC = 0;

		CWnd* pWnd = CWnd::GetDesktopWindow();
		if (pWnd->LockWindowUpdate())
			pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
		else
			pDC = pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE);

		OnInvertTracker(pDC, rectTracker);
		int nMinWidth = GetCustomizeMinWidth();
		BOOL bLeftAnchor = (point.x - m_rcControl.left <= 2);

		LONG& lTrackerAnchor = bLeftAnchor ? rectTracker.left : rectTracker.right;
		int nOffset = bLeftAnchor ? m_rcControl.left - point.x : m_rcControl.right - point.x;

		BOOL bAccept = FALSE;
		while (CWnd::GetCapture() == m_pParent)
		{
			MSG msg;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;

			if (msg.message == WM_MOUSEMOVE)
			{
				point = CPoint(msg.lParam);
				m_pParent->ClientToScreen(&point);
				point.x += nOffset;

				point.x = bLeftAnchor ? min(point.x, rectTracker.right - nMinWidth) :
					max(point.x, rectTracker.left + nMinWidth);

				if (lTrackerAnchor != point.x)
				{
					OnInvertTracker(pDC, rectTracker);
					lTrackerAnchor = point.x;
					OnInvertTracker(pDC, rectTracker);
				}
			}
			else if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) break;
			else if (msg.message == WM_LBUTTONUP)
			{
				bAccept = TRUE;
				break;
			}
			else  ::DispatchMessage(&msg);
		}

		OnInvertTracker(pDC, rectTracker);

		if (CWnd::GetCapture() == m_pParent) ReleaseCapture();

		pWnd->UnlockWindowUpdate();
		if (pDC != NULL)
		{
			pWnd->ReleaseDC(pDC);
			pDC = NULL;
		}

		pCommandBars->m_pDragSelected = this;

		int nWidth = rectTracker.Width();

		if (bAccept && rectTracker.Width() != m_rcControl.Width())
		{
			if (GetParent()->GetType() == xtpBarTypePopup)
				nWidth -= GetPaintManager()->GetPopupBarGripperWidth(GetParent());

			SetWidth(nWidth);
			m_pParent->OnRecalcLayout();
		}
		m_pParent->Redraw();

		return TRUE;

	}

	return FALSE;
}

void CXTPControl::CustomizeStartDrag(CPoint pt)
{
	ASSERT(m_pParent);
	CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();
	ASSERT(pCommandBars);

	if ((pCommandBars->m_pDragSelected == this) && IsCustomizeResizeAllow() &&
		CustomizeStartResize(pt))
	{
		return;
	}

	pCommandBars->SetDragControl(this);

	CXTPCustomizeDropSource* pDropSource = pCommandBars->GetDropSource();
	DROPEFFECT dropEffect = pDropSource->DoDragDrop(this);

	if (dropEffect == DROPEFFECT_NONE || dropEffect == DROPEFFECT_MOVE)
	{
		CXTPCommandBar* pParent = m_pParent;
		pParent->SetPopuped(-1);
		pParent->SetSelected(-1);

		if (pCommandBars->GetDragControl() == this)
			pCommandBars->SetDragControl(NULL);


		if (GetBeginGroup() && m_nIndex < m_pControls->GetCount() - 1)
			m_pControls->GetAt(m_nIndex + 1)->SetBeginGroup(TRUE);

		if (m_nIndex == 0 && m_pControls->GetCount() > 1)
			m_pControls->GetAt(1)->SetBeginGroup(FALSE);

		m_pControls->Remove(this);

		pParent->OnRecalcLayout();
	}
	pCommandBars->SetDragControl(pCommandBars->GetDragControl());
}

void CXTPControl::OnCustomizeDragOver(CXTPControl* /*pDataObject*/, CPoint /*point*/, DROPEFFECT& /*dropEffect*/)
{
	m_pParent->SetPopuped(-1);
	m_pParent->SetSelected(-1);
}
BOOL CXTPControl::IsCustomizeDragOverAvail(CXTPCommandBar* /*pCommandBar*/, CPoint /*point*/, DROPEFFECT& /*dropEffect*/)
{
	return TRUE;
}

BOOL CXTPControl::IsCustomizeResizeAllow() const
{
	return FALSE;
}

CXTPImageManager* CXTPControl::GetImageManager() const
{
	if (m_pParent)
		return m_pParent->GetImageManager();

	if (m_pControls)
	{
		CXTPCommandBars* pCommandBars = m_pControls->GetCommandBars();
		if (pCommandBars)
			return pCommandBars->GetImageManager();
	}
	return XTPImageManager();
}
CXTPPaintManager* CXTPControl::GetPaintManager() const
{
	if (m_pParent)
		return m_pParent->GetPaintManager();

	if (m_pControls)
	{
		CXTPCommandBars* pCommandBars = m_pControls->GetCommandBars();
		if (pCommandBars)
			return pCommandBars->GetPaintManager();
	}
	return XTPPaintManager();

}
CXTPImageManagerIcon* CXTPControl::GetImage(int nWidth) const
{
	if (GetIconId() != 0)
		return GetImageManager()->GetImage(GetIconId(), nWidth);
	return NULL;

}

void CXTPControl::SetExpanded(BOOL bExpanded)
{
	m_bExpanded = bExpanded;
	if (!bExpanded) SetHideFlags(GetHideFlags() & ~xtpHideExpand);
}

BOOL CXTPControl::IsItemDefault() const
{
	return m_bDefaultItem;
}

void CXTPControl::SetItemDefault(BOOL bDefault)
{
	if (bDefault != m_bDefaultItem)
	{
		m_bDefaultItem = bDefault;
		DelayLayoutParent();
	}
}

void CXTPControl::SetFlags(DWORD dwFlags)
{
	m_dwFlags = dwFlags;
}

DWORD CXTPControl::GetFlags() const
{
	return m_dwFlags;
}

BOOL CXTPControl::IsCustomizeMovable() const
{
	if (m_pParent && m_pParent->IsCustomizable() && ((GetFlags() & xtpFlagNoMovable) == 0))
		return TRUE;

	CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();

	if (pCommandBars && pCommandBars->m_bDesignerMode)
		return TRUE;

	return FALSE;
}

void CXTPControl::CDocTemplateMap::Copy(CDocTemplateMap& map)
{
	RemoveAll();

	UINT nIDResource;
	BOOL bValue;
	POSITION pos = map.GetStartPosition();
	while (pos)
	{
		map.GetNextAssoc(pos, nIDResource, bValue);
		SetAt(nIDResource, bValue);
	}
}

CXTPControl* CXTPControl::FromUI(CCmdUI* pCmdUI)
{
	CXTPCommandBar* pCommandBar = DYNAMIC_DOWNCAST(CXTPCommandBar, pCmdUI->m_pOther);
	if (!pCommandBar)
		return NULL;

	CXTPControls* pControls = pCommandBar->GetControls();
	if ((int)pCmdUI->m_nIndex < pControls->GetCount())
		return pControls->GetAt(pCmdUI->m_nIndex);

	return NULL;

}

BOOL CXTPControl::OnLButtonDblClk(CPoint point)
{
	OnClick(FALSE, point);
	return TRUE;
}

void CXTPControl::Reset()
{
	if (m_nCustomIconId != 0 || !m_strCustomCaption.IsEmpty() || m_buttonCustomStyle != xtpButtonUndefined)
	{
		m_nCustomIconId = 0;
		m_strCustomCaption = _T("");
		m_buttonCustomStyle = xtpButtonUndefined;
		GetParent()->OnRecalcLayout();
	}
}

void CXTPControl::SetCustomIcon(HICON hIcon)
{
	CXTPImageManager* pImageManager = GetImageManager();
	if (pImageManager)
	{
		m_nCustomIconId = pImageManager->AddCustomIcon(hIcon);
	}
}

CSize CXTPControl::GetButtonSize() const
{
	return m_pParent->GetButtonSize();
}

CSize CXTPControl::GetIconSize() const
{
	return m_pParent->GetIconSize();
}


#ifndef _XTP_INCLUDE_RIBBON
BOOL CXTPControl::HasDwmCompositedRect() const
{
	return FALSE;
}
#endif

void CXTPControl::OnUnderlineActivate()
{
	OnExecute();
}

BOOL CXTPControl::IsCaptionVisible() const
{
	if (GetParent()->GetType() == xtpBarTypePopup)
		return TRUE;

	if (GetCaption().IsEmpty())
		return FALSE;

	XTPButtonStyle buttonStyle = GetStyle();

	if ((buttonStyle == xtpButtonCaption) || (buttonStyle == xtpButtonIconAndCaption) || (buttonStyle == xtpButtonIconAndCaptionBelow))
		return TRUE;

	if (buttonStyle == xtpButtonIcon)
		return FALSE;

	switch (GetType())
	{
		case xtpControlPopup:
		case xtpControlLabel:
		case xtpControlCheckBox:
		case xtpControlRadioButton:
			return TRUE;

		case xtpControlButton:
		case xtpControlButtonPopup:
		case xtpControlSplitButtonPopup:
			CXTPImageManagerIcon* pImage = GetImage(0);
			return pImage == NULL;
	}
	return FALSE;
}


CCmdTarget* CXTPControl::GetAccessible()
{
	return this;
}

HRESULT CXTPControl::GetAccessibleParent(IDispatch* FAR* ppdispParent)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispParent = NULL;

	if (m_pParent)
	{
		*ppdispParent = m_pParent->GetIDispatch(TRUE);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CXTPControl::GetAccessibleChildCount(long FAR* pChildCount)
{
	if (pChildCount == 0)
		return E_INVALIDARG;

	*pChildCount = GetCommandBar() ? 1 : 0;

	return S_OK;
}
HRESULT CXTPControl::GetAccessibleChild(VARIANT varChild, IDispatch* FAR* ppdispChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	*ppdispChild = NULL;

	if (GetChildIndex(&varChild) == 1)
	{
		CXTPCommandBar* pCommandBar = GetCommandBar();
		if (pCommandBar)
		{
			*ppdispChild = pCommandBar->GetIDispatch(TRUE);
		}
	}

	return S_OK;
}

HRESULT CXTPControl::GetAccessibleName(VARIANT varChild, BSTR* pszName)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	CString strCaption = GetCaption();
	CXTPPaintManager::StripMnemonics(strCaption);

	if (!GetShortcutText().IsEmpty())
		strCaption = strCaption + _T('\t') + GetShortcutText();

	if (strCaption.IsEmpty())
	{
		strCaption = GetTooltip();
	}

	*pszName = strCaption.AllocSysString();
	return S_OK;
}

HRESULT CXTPControl::GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDescription = GetDescription().AllocSysString();
	return S_OK;
}

HRESULT CXTPControl::GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole)
{
	pvarRole->vt = VT_EMPTY;

	if (GetChildIndex(&varChild) == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = m_pParent->GetType() == xtpBarTypeNormal || m_pParent->GetType() == xtpBarTypeRibbon ?
			GetCommandBar() ? ROLE_SYSTEM_BUTTONMENU: ROLE_SYSTEM_PUSHBUTTON : ROLE_SYSTEM_MENUITEM;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT CXTPControl::AccessibleSelect(long /*flagsSelect*/, VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	if (m_pParent && m_pParent->GetSafeHwnd())
	{
		m_pParent->SetTrackingMode(TRUE, FALSE);
		m_pParent->SetSelected(m_nIndex, TRUE);
	}

	return S_OK;
}

#ifndef STATE_SYSTEM_HASPOPUP
#define STATE_SYSTEM_HASPOPUP   (0x40000000)
#endif //STATE_SYSTEM_HASPOPUP


HRESULT CXTPControl::GetAccessibleState(VARIANT varChild, VARIANT* pvarState)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	pvarState->vt = VT_I4;
	pvarState->lVal = STATE_SYSTEM_FOCUSABLE |
		(m_bSelected ? STATE_SYSTEM_FOCUSED | STATE_SYSTEM_SELECTED | STATE_SYSTEM_HOTTRACKED : 0);

	if (!m_pParent->IsVisible() || !IsVisible())
		pvarState->lVal |= STATE_SYSTEM_INVISIBLE;

	if (!GetEnabled())
		pvarState->lVal |= STATE_SYSTEM_UNAVAILABLE;

	if (GetChecked())
		pvarState->lVal |= STATE_SYSTEM_CHECKED;

	if (GetPressed())
		pvarState->lVal |= STATE_SYSTEM_PRESSED;

	if (GetCommandBar())
		pvarState->lVal |= STATE_SYSTEM_HASPOPUP;

	return S_OK;
}

HRESULT CXTPControl::GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	*pszDefaultAction = SysAllocString(L"Click");

	return S_OK;
}

HRESULT CXTPControl::AccessibleDoDefaultAction(VARIANT varChild)
{
	SAFE_MANAGE_STATE(m_pModuleState);

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	OnClick(TRUE);

	return S_OK;
}


HRESULT CXTPControl::AccessibleLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	*pxLeft = *pyTop = *pcxWidth = *pcyHeight = 0;

	if (GetChildIndex(&varChild) != CHILDID_SELF)
		return E_INVALIDARG;

	if (!m_pParent->GetSafeHwnd())
		return S_OK;

	if (!IsVisible())
		return S_OK;

	CRect rcControl = GetRect();
	m_pParent->ClientToScreen(&rcControl);

	*pxLeft = rcControl.left;
	*pyTop = rcControl.top;
	*pcxWidth = rcControl.Width();
	*pcyHeight = rcControl.Height();

	return S_OK;
}

HRESULT CXTPControl::AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	if (pvarID == NULL)
		return E_INVALIDARG;

	pvarID->vt = VT_EMPTY;

	if (!m_pParent->GetSafeHwnd())
		return S_FALSE;

	if (!CXTPWindowRect(m_pParent).PtInRect(CPoint(xLeft, yTop)))
		return S_FALSE;

	pvarID->vt = VT_I4;
	pvarID->lVal = 0;

	CPoint pt(xLeft, yTop);
	m_pParent->ScreenToClient(&pt);

	if (!GetRect().PtInRect(pt))
		return S_FALSE;

	return S_OK;
}

void CXTPControl::SetVisible(BOOL bVisible)
{
	DWORD dwHideFlags = m_dwHideFlags;
	if (!bVisible) SetHideFlags(m_dwHideFlags | xtpHideGeneric); else SetHideFlags(m_dwHideFlags & ~xtpHideGeneric);
	if (dwHideFlags != m_dwHideFlags) DelayLayoutParent();
}



BEGIN_INTERFACE_MAP(CXTPControl, CCmdTarget)
	INTERFACE_PART(CXTPControl, IID_IAccessible, ExternalAccessible)
END_INTERFACE_MAP()
