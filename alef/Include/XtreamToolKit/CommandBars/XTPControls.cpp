// XTPControls.cpp : implementation of the CXTPControls class.
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

#include "Common/XTPImageManager.h"
#include "Common/XTPResourceManager.h"

#include "XTPControls.h"
#include "XTPCommandBar.h"
#include "XTPPopupBar.h"
#include "XTPControl.h"
#include "XTPControlButton.h"
#include "XTPControlPopup.h"
#include "XTPControlComboBox.h"
#include "XTPControlEdit.h"
#include "XTPControlCustom.h"
#include "XTPControlGallery.h"
#include "XTPControlExt.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"
#include "XTPPaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommandBarControls

IMPLEMENT_DYNCREATE(CXTPControls, CCmdTarget)
IMPLEMENT_DYNCREATE(CXTPOriginalControls, CXTPControls)

CXTPControls::CXTPControls()
{
	EnableAutomation();


	m_pParent = 0;
	m_pCommandBars = 0;

	m_pOriginalControls = 0;
	m_bOriginalControls = FALSE;
}

CXTPControls::~CXTPControls()
{
	RemoveAll();

	CMDTARGET_RELEASE(m_pOriginalControls);
}

CXTPOriginalControls::CXTPOriginalControls()
{
	m_bOriginalControls = TRUE;
}

CXTPCommandBars* CXTPControls::GetCommandBars() const
{
	return m_pParent ? m_pParent->GetCommandBars() : m_pCommandBars;
}
void CXTPControls::SetCommandBars(CXTPCommandBars* pCommandBars)
{
	m_pCommandBars = pCommandBars;
}

void CXTPControls::RefreshIndexes()
{
	for (int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		CXTPControl* pControl = GetAt(nIndex);
		pControl->m_nIndex = nIndex;
		pControl->m_pControls = this;
		pControl->SetParent(m_pParent);
	}
	if (m_pParent)
	{
		m_pParent->OnControlsChanged();
	}
}

void CXTPControls::SetParent(CXTPCommandBar* pParent)
{
	m_pParent = pParent;
	if (GetCount() > 0) RefreshIndexes();
}


CXTPControl* CXTPControls::AddMenuItem(CMenu* pMenu, int nIndex)
{
	CMenu* pPopupMenu = pMenu->GetSubMenu(nIndex);

	CXTPControl* pControl = NULL;
	UINT nID = pMenu->GetMenuItemID(nIndex);

	BOOL bDefaultItem = (int)::GetMenuDefaultItem(pMenu->m_hMenu, TRUE, GMDI_USEDISABLED) == nIndex;

	if (nID == AFX_IDM_FIRST_MDICHILD)
		return NULL;

	CString strCaption;
	pMenu->GetMenuString(nIndex, strCaption, MF_BYPOSITION);

	if (pPopupMenu)
	{
		int iPos = strCaption.Find(_T('\t'));
		if (pPopupMenu &&  iPos > 0)
		{
			nID = _ttoi(strCaption.Mid(iPos + 1));
			if (nID > 0) strCaption.ReleaseBuffer(iPos);
		}
	}


	CWnd* pSite = m_pCommandBars ? m_pCommandBars->GetSite() :
		m_pParent ? m_pParent->GetOwnerSite() : NULL;

	CXTPCommandBars* pCommandBars = GetCommandBars();

	XTPControlType controlType = pPopupMenu ? xtpControlPopup : xtpControlButton;

	XTPButtonStyle buttonStyle = xtpButtonAutomatic;

	XTP_COMMANDBARS_CREATECONTROL cs;

	if (pSite)
	{
		cs.nID = nID;
		cs.pControl = NULL;
		cs.bToolBar = FALSE;
		cs.pCommandBar = m_pParent;
		cs.pMenu = pMenu;
		cs.nIndex = nIndex;
		cs.strCaption = strCaption;
		cs.controlType = controlType;
		cs.buttonStyle = buttonStyle;

		if (pSite->SendMessage(WM_XTP_BEFORECREATECONTROL, 0, (LPARAM)&cs) != 0)
		{
			pControl = cs.pControl;
			controlType = cs.controlType;
			strCaption = cs.strCaption;
			nID = cs.nID;
			buttonStyle = cs.buttonStyle;
		}
	}

	if (pControl == NULL)
	{
		switch (controlType)
		{
			case xtpControlButton:
				if (nID == XTP_ID_WINDOWLIST) pControl = (CXTPControl*)CXTPControlWindowList::CreateObject();
				else if (nID == XTP_ID_WORKSPACE_ACTIONS) pControl = (CXTPControl*)CXTPControlWorkspaceActions::CreateObject();
				else if (nID == ID_VIEW_TOOLBAR) pControl = (CXTPControl*)CXTPControlToolbars::CreateObject();
				else if (nID == ID_OLE_VERB_FIRST && strCaption == _T("<<OLE VERBS GO HERE>>")) pControl = (CXTPControl*)CXTPControlOleItems::CreateObject();
				else if (nID == ID_FILE_MRU_FILE1) pControl = (CXTPControl*)CXTPControlRecentFileList::CreateObject();
				else pControl = (CXTPControl*)CXTPControlButton::CreateObject();
				break;

			case xtpControlComboBox:
				pControl = (CXTPControl*)CXTPControlComboBox::CreateObject();
				break;

			case xtpControlEdit:
				pControl = (CXTPControl*)CXTPControlEdit::CreateObject();
				break;
			case xtpControlLabel:
				pControl = (CXTPControl*)CXTPControlLabel::CreateObject();
				break;

			case xtpControlCheckBox:
				pControl = (CXTPControl*)CXTPControlCheckBox::CreateObject();
				break;

			case xtpControlRadioButton:
				pControl = (CXTPControl*)CXTPControlRadioButton::CreateObject();
				break;

			case xtpControlGallery:
				pControl = (CXTPControl*)CXTPControlGallery::CreateObject();
				break;

			case xtpControlPopup:
			case xtpControlSplitButtonPopup:
			case xtpControlButtonPopup:
				pControl = CXTPControlPopup::CreateControlPopup(controlType);
				CXTPPopupBar* pPopupBar = 0;
				CString strCaptionPopup(strCaption);
				CXTPPaintManager::StripMnemonics(strCaptionPopup);

				if (pSite)
				{
					XTP_COMMANDBARS_CREATEBAR cb;
					ZeroMemory(&cb, sizeof(cb));
					cb.bPopup = TRUE;
					cb.lpcstrCaption = strCaptionPopup;
					cb.nID = nID;

					if (pSite->SendMessage(WM_XTP_CREATECOMMANDBAR, 0, (LPARAM)&cb) != 0)
					{
						pPopupBar = DYNAMIC_DOWNCAST(CXTPPopupBar, cb.pCommandBar);
						strCaptionPopup = cb.lpcstrCaption;
					}
				}

				if (pPopupBar == NULL) pPopupBar = CXTPPopupBar::CreatePopupBar(pCommandBars);
				pPopupBar->SetTitle(strCaptionPopup);



				CMenu menu;
				if (!pPopupMenu && nID > 0 && menu.LoadMenu(nID))
				{
					pPopupMenu = menu.GetSubMenu(0);
				}
				if (pPopupMenu) pPopupBar->LoadMenu(pPopupMenu, FALSE);

				((CXTPControlPopup*)pControl)->m_pCommandBar = pPopupBar;

				break;
		}

		ASSERT(pControl);
		pControl->m_controlType = controlType;
		pControl->SetStyle(buttonStyle);
	}

	CXTPControlAction* pAction = pCommandBars->FindAction(nID);

	if (pControl->GetAction() != NULL)
	{

	}
	else if (pAction || (pCommandBars->IsActionsEnabled() && nID > 0 && nID < 0xFFFFFFF))
	{
		if (pAction == NULL) pAction = pCommandBars->CreateAction(nID);
		pControl->SetAction(pAction);

		if (pAction->m_strCaption.IsEmpty())
			pAction->SetCaption(strCaption);
	}
	else
	{
		pControl->SetID(nID);
		pControl->SetCaption(strCaption);
	}

	if (bDefaultItem) pControl->SetItemDefault(bDefaultItem);

	m_arrControls.InsertAt(GetCount(), pControl);
	OnControlAdded(pControl);

	if (pSite)
	{
		cs.pControl = pControl;
		pSite->SendMessage(WM_XTP_AFTERCREATECONTROL, 0, (LPARAM)&cs);
	}


	return pControl;
}

void CXTPControls::OnControlAdded(CXTPControl* /*pControl*/)
{
	RefreshIndexes();
}

void CXTPControls::OnControlRemoved(CXTPControl* pControl)
{
	pControl->m_pControls = NULL;
	pControl->OnRemoved();

	pControl->m_pParent = NULL;

	RefreshIndexes();
}


CXTPControl* CXTPControls::Add(XTPControlType controlType, int nId, LPCTSTR strParameter, int nBefore, BOOL bTemporary)
{
	CXTPControl* pControl = NULL;
	CXTPCommandBars* pCommandBars = GetCommandBars();

	switch (controlType)
	{
		case xtpControlCustom:
			return Add(new CXTPControlCustom(), nId, strParameter, nBefore, bTemporary);

		case xtpControlButton:
			if (nId == XTP_ID_WINDOWLIST) pControl = (CXTPControl*)CXTPControlWindowList::CreateObject();
			else if (nId == XTP_ID_WORKSPACE_ACTIONS) pControl = (CXTPControl*)CXTPControlWorkspaceActions::CreateObject();
			else if (nId == ID_VIEW_TOOLBAR) pControl = (CXTPControl*)CXTPControlToolbars::CreateObject();
			else if (nId == XTP_ID_FILE_MRU) pControl = (CXTPControl*)CXTPControlRecentFileList::CreateObject();
			else
			pControl = (CXTPControl*)CXTPControlButton::CreateObject();
			break;

		case xtpControlComboBox:
			pControl = (CXTPControl*)CXTPControlComboBox::CreateObject();
			break;

		case xtpControlLabel:
			pControl = (CXTPControl*)CXTPControlLabel::CreateObject();
			break;

		case xtpControlCheckBox:
			pControl = (CXTPControl*)CXTPControlCheckBox::CreateObject();
			break;

		case xtpControlRadioButton:
			pControl = (CXTPControl*)CXTPControlRadioButton::CreateObject();
			break;

		case xtpControlGallery:
			pControl = (CXTPControl*)CXTPControlGallery::CreateObject();
			break;

		case xtpControlEdit:
			pControl = (CXTPControl*)CXTPControlEdit::CreateObject();
			break;

		case xtpControlPopup:
		case xtpControlSplitButtonPopup:
		case xtpControlButtonPopup:
			CXTPControlPopup* pControlPopup = (CXTPControlPopup*)CXTPControlPopup::CreateObject();
			CXTPPopupBar* pPopupBar = 0;

			CMenu menu;
			if (nId > 0)
			{
				CString strCaption;
				CMenu* pPopupMenu = NULL;

				if (menu.LoadMenu(nId))
				{
					pPopupMenu = menu.GetSubMenu(0);
					menu.GetMenuString(0, strCaption, MF_BYPOSITION);
				}

				CWnd* pSite = m_pCommandBars ? m_pCommandBars->GetSite() :
					m_pParent ? m_pParent->GetOwnerSite() : NULL;

				if (pSite)
				{
					XTP_COMMANDBARS_CREATEBAR cs;
					ZeroMemory(&cs, sizeof(cs));
					cs.bPopup = TRUE;
					cs.lpcstrCaption = strCaption;
					cs.nID = nId;

					if (pSite->SendMessage(WM_XTP_CREATECOMMANDBAR, 0, (LPARAM)&cs) != 0)
					{
						pPopupBar = DYNAMIC_DOWNCAST(CXTPPopupBar, cs.pCommandBar);
					}
				}
				if (pPopupBar == NULL) pPopupBar = CXTPPopupBar::CreatePopupBar(GetCommandBars());
				pControlPopup->m_strCaption = strCaption;
				if (pPopupMenu)
				{
					pPopupBar->LoadMenu(pPopupMenu);
				}

			}

			if (pPopupBar == NULL) pPopupBar = CXTPPopupBar::CreatePopupBar(pCommandBars);
			pControlPopup->m_pCommandBar = pPopupBar;

			pControl = pControlPopup;
			break;
	}

	ASSERT(pControl);
	pControl->m_controlType = controlType;

	return Add(pControl, nId, strParameter, nBefore, bTemporary);
}

CXTPControl* CXTPControls::Add(CXTPControl* pControl, int nId, LPCTSTR strParameter, int nBefore, BOOL bTemporary)
{
	ASSERT(pControl);
	pControl->m_strParameter = strParameter;
	pControl->m_bTemporary = bTemporary;

	CXTPCommandBars* pCommandBars = GetCommandBars();

	CXTPControlAction* pAction = pCommandBars->FindAction(nId);

	if (pControl->GetAction() != NULL)
	{

	}
	else if (pAction != NULL)
	{
		pControl->SetAction(pAction);
	}
	else if (pCommandBars->IsActionsEnabled() && nId > 0 && nId < 0xFFFFFFF)
	{
		if ((nId >= XTP_COMMANDBARS_RESOURCE_FIRST && nId <= XTP_COMMANDBARS_RESOURCE_LAST)
			|| (nId >= 35000 && nId <= 35003))
		{
			if (!pAction)
			{
				pControl->SetID(nId);
			}
		}
		else
		{
			pAction = pCommandBars->CreateAction(nId);
			pControl->SetAction(pAction);
		}
	}
	else
	{
		pControl->SetID(nId);
	}

	return InsertAt(pControl, nBefore);
}

CXTPControl* CXTPControls::InsertAt(CXTPControl* pControl, int nBefore)
{
	if (nBefore < 0 || nBefore >= GetCount())
		nBefore = GetCount();

	ASSERT(pControl);

	m_arrControls.InsertAt(nBefore, pControl);
	OnControlAdded(pControl);

	return pControl;
}

CXTPControl* CXTPControls::Add(CXTPControl* pControl)
{
	return InsertAt(pControl, -1);
}

CXTPControl* CXTPControls::AddClone(CXTPControl* pClone, int nBefore, BOOL bRecursive)
{
	CXTPControl* pControl = pClone->Clone(bRecursive);
	ASSERT(pControl);

	return InsertAt(pControl, nBefore);
}



void CXTPControls::RemoveAll()
{
	for (int nIndex = GetCount() - 1; nIndex >= 0; nIndex--)
	{
		CXTPControl* pControl = GetAt(nIndex);

		if (OnControlRemoving(pControl))
			continue;

		m_arrControls.RemoveAt(nIndex);

		OnControlRemoved(pControl);

		pControl->InternalRelease();
	}
}

CXTPControl* CXTPControls::GetFirst() const
{
	return GetCount() > 0 ? GetAt(0) : NULL;
}

void CXTPControls::GetNext(CXTPControl*& pControl) const
{
	pControl = (!pControl || pControl->m_nIndex >= GetCount() - 1) ? 0 : GetAt(pControl->m_nIndex + 1);
}

CXTPControl* CXTPControls::FindControl(int nId) const
{
	return FindControl(xtpControlError, nId, FALSE, FALSE);
}

CXTPControl* CXTPControls::FindControl(XTPControlType type, int nId, BOOL bVisible, BOOL bRecursive) const
{
	for (int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		CXTPControl* pControl = GetAt(nIndex);
		if ((type == xtpControlError || pControl->GetType() == type) &&
			(!bVisible || pControl->IsVisible()) &&
			(nId == -1 || nId == pControl->GetID()))
			return pControl;

		if (bRecursive)
		{
			CXTPCommandBar* pBar = pControl->GetCommandBar();
			if (pBar)
			{
				pControl = pBar->GetControls()->FindControl(type, nId, bVisible, bRecursive);
				if (pControl != NULL) return pControl;
			}
		}
	}
	return NULL;
}

BOOL CXTPControls::OnControlRemoving(CXTPControl* /*pControl*/)
{
	return FALSE;
}

void CXTPControls::Remove(CXTPControl* pControl)
{
	if (pControl && pControl->m_pControls == this)
	{
		if (pControl->GetSelected())
			m_pParent->SetSelected(-1);

		ASSERT(pControl->m_pControls == this);

		//Ensure the index falls within the correct range.
		if ((pControl->m_nIndex >= 0) && (pControl->m_nIndex < GetCount()))
		{
			ASSERT(m_arrControls[pControl->m_nIndex] == pControl);

			if (OnControlRemoving(pControl))
				return;

			m_arrControls.RemoveAt(pControl->m_nIndex);

			OnControlRemoved(pControl);

			pControl->SetParent(NULL);
			pControl->InternalRelease();
		}
	}
}

void CXTPControls::Remove(int nIndex)
{
	CXTPControl* pControl = GetAt(nIndex);
	Remove(pControl);
}

CXTPControl* CXTPControls::SetControlType(int nIndex, XTPControlType type)
{
	return SetControlType(GetAt(nIndex), type);
}


CXTPControl* CXTPControls::SetControlType(CXTPControl* pControl, XTPControlType type)
{
	ASSERT(pControl->m_pControls == this);
	ASSERT(pControl);

	if (IsPopupControlType(pControl->GetType()) && IsPopupControlType(type))
	{
		pControl->m_controlType = type;
		return pControl;
	}

	CXTPControl* pNew = Add(type, 0, _T(""), pControl->GetIndex() + 1, FALSE);
	pNew->CXTPControl::Copy(pControl, FALSE);
	pNew->m_controlType = type;

	Remove(pControl);

	return pNew;

}


long CXTPControls::GetNext(long nIndex, int nDirection, BOOL bKeyboard, BOOL bSkipTemporary, BOOL bSkipCollapsed) const
{
	ASSERT(nDirection == +1 || nDirection == -1);
	if (GetCount() == 0) return -1;

	long nNext = nIndex + nDirection;

	if (nDirection == -1 && nIndex == -1)
	{
		nNext = GetCount() - 1;
	}

	DWORD nSkipVisible = xtpHideScroll | (!bSkipCollapsed ? xtpHideExpand : 0);

	BOOL bCircle = FALSE;

	while (nNext != nIndex)
	{
		if (nNext >= GetCount())
		{
			if ((nIndex == -1 && nDirection == +1) || bCircle) return -1;
			nNext = 0;
			bCircle = TRUE;
		}
		if (nNext < 0)
		{
			if ((nIndex == -1 && nDirection == -1) || bCircle) return -1;
			nNext = GetCount() - 1;
			bCircle = TRUE;
		}

		if ((GetAt(nNext)->IsVisible(nSkipVisible)) && (!bKeyboard || !(GetAt(nNext)->m_dwFlags & xtpFlagSkipFocus)))
		{
			if (!bSkipTemporary || !GetAt(nNext)->m_bTemporary)
				return nNext;
		}
		nNext += nDirection;
	}
	return nNext;
}

CXTPControl* CXTPControls::HitTest(CPoint point) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControl* pControl = GetAt(i);

		if (pControl->IsVisible())
		{
			if (pControl->m_rcControl.PtInRect(point))
				return pControl;
		}

	}
	return NULL;
}


struct CXTPControls::XTPBUTTONINFO  // struct for CalcDynamicSize ;
{
	CRect rcControl;
	CRect rcRow;
	CSize szControl;
	BOOL bVisible;
	BOOL bWrap;
	BOOL bHide;
	BOOL bBeginGroup;
	XTPButtonStyle buttonStyle;
	CXTPControl* pControl;

	void EnsureButtonSize(CDC* pDC)
	{
		if (buttonStyle != pControl->GetStyle())
		{
			buttonStyle = pControl->GetStyle();
			szControl = pControl->GetSize(pDC);
		}
	}
	void Attach(CDC* pDC, CXTPControl* p)
	{
		pControl = p;
		bHide = bWrap = FALSE;
		rcRow.SetRectEmpty();

		szControl = pControl->GetSize(pDC);
		rcControl = pControl->GetRect();
		bBeginGroup = pControl->GetBeginGroup();
		bVisible = pControl->IsVisible(xtpHideWrap);
		buttonStyle = pControl->GetStyle();
	}

	void Detach()
	{
		pControl->SetHideWrap(bHide);
		pControl->SetRowRect(rcRow);
		if (!bHide)
		pControl->SetRect(rcControl);
		pControl->SetWrap(bWrap);
	}

};

CSize CXTPControls::_CalcSize(XTPBUTTONINFO* pData, const CSize& szSeparators, BOOL bVert)
{

	CPoint cur(0, 0);
	CSize sizeResult(0, 0);
	int nRow = 0;
	BOOL bFirst = TRUE;
	for (int i = 0; i < GetCount(); i++)
	{
		XTPBUTTONINFO* pControl = &pData[i];

		if (!pControl->bVisible || pControl->bHide)
			continue;

		CSize szControl = pControl->szControl;

		if (bVert)
		{
			if (pControl->bBeginGroup && !pControl->bWrap && !bFirst)
				cur.y += szSeparators.cx;


			if (pControl->bWrap)
			{
				cur.x -= nRow + (pControl->bBeginGroup ? szSeparators.cy : 0);
				nRow = cur.y = 0;
			}
			nRow = max(nRow, szControl.cx);

			pControl->rcControl.SetRect(cur.x - szControl.cx, cur.y, cur.x , cur.y + szControl.cy);

			sizeResult.cx = max(szControl.cx - cur.x, sizeResult.cx);
			sizeResult.cy = max(cur.y + szControl.cy, sizeResult.cy);

			cur.y += szControl.cy;

		}
		else
		{

			if (pControl->bBeginGroup && !pControl->bWrap && !bFirst)
				cur.x += szSeparators.cx;

			if (pControl->bWrap)
			{
				cur.y += nRow + (pControl->bBeginGroup ? szSeparators.cy : 0);
				nRow = cur.x = 0;
			}
			nRow = max(nRow, szControl.cy);

			pControl->rcControl.SetRect(cur.x, cur.y, cur.x + szControl.cx, cur.y + szControl.cy);

			sizeResult.cx = max(cur.x + szControl.cx, sizeResult.cx);
			sizeResult.cy = max(cur.y + szControl.cy, sizeResult.cy);

			cur.x += szControl.cx;
		}
		bFirst = FALSE;
	}
	return sizeResult;
}

int CXTPControls::_WrapToolBar(XTPBUTTONINFO* pData, int nWidth, DWORD& dwMode, const CSize& szSeparators)
{
	int nResult = 0, x = 0;
	BOOL bVert = dwMode & LM_VERTDOCK;
	BOOL bFirst = TRUE;
	int nSeparator = bVert ? szSeparators.cy : szSeparators.cx;

	for (int i = 0; i < GetCount(); i++)
	{
		XTPBUTTONINFO* pControl = &pData[i];
		pControl->bHide = FALSE;
		pControl->bWrap = FALSE;

		if (!pControl->bVisible)
			continue;

		int dx = bVert ? pControl->szControl.cy : pControl->szControl.cx;

		if (pControl->bBeginGroup && !bFirst)
			dx += nSeparator;

		BOOL bWrap = (x + dx > nWidth) || ((dwMode & LM_POPUP) && pControl->bBeginGroup && !bFirst);

		BOOL bRowWrap = FALSE;
		if (pControl->pControl->GetFlags() & xtpFlagWrapRow)
			bRowWrap = TRUE;

		if (bRowWrap)
		{
			x = bVert ? pData[i].szControl.cy : pData[i].szControl.cx;
			pControl->bWrap = TRUE;
			nResult++;
		}
		else if (bWrap)
		{
			if (dwMode & LM_HIDEWRAP && !bFirst)
			{
				BOOL bWrapRowFound = FALSE;
				for (int j = i; j < GetCount(); j++)
				{
					if (pData[j].pControl->GetFlags() & xtpFlagWrapRow)
					{
						bWrapRowFound = TRUE;
						i = j - 1;
						break;
					}
					pData[j].bHide = TRUE;
				}
				dwMode |= LM_STRETCH;

				if (bWrapRowFound)
					continue;

				return nResult;
			}

			for (int j = i; j >= 0 && pData[j].bWrap == FALSE; j--)
			{
				// Find last separator
				if (pData[j].bBeginGroup && pData[j].bVisible)
				{
					i = j;
					break;
				}
			}
			x = bVert ? pData[i].szControl.cy : pData[i].szControl.cx;

			pData[i].bWrap = TRUE;
			if (!(dwMode & LM_HIDEWRAP)) dwMode |= LM_STRETCH;
			nResult++;
		}
		else
			x += dx;

		bFirst = FALSE;

	}
	return nResult + 1;
}

void  CXTPControls::_SizePopupToolBar(XTPBUTTONINFO* pData, DWORD dwMode, const CSize& szSeparators)
{
	CSize sizeMax, sizeMin, sizeMid;

	// Wrap ToolBar vertically
	_WrapToolBar(pData, 0, dwMode, szSeparators);
	sizeMin = _CalcSize(pData, szSeparators);

	// Wrap ToolBar horizontally
	_WrapToolBar(pData, 32767, dwMode, szSeparators);
	sizeMax = _CalcSize(pData, szSeparators);

	while (sizeMin.cx < sizeMax.cx)
	{
		sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;

		_WrapToolBar(pData, sizeMid.cx, dwMode, szSeparators);
		sizeMid = _CalcSize(pData, szSeparators);

		if (sizeMid.cy > sizeMid.cx)
		{
			if (sizeMin == sizeMid)
				return;

			sizeMin = sizeMid;
		}
		else if (sizeMid.cy < sizeMid.cx)
			sizeMax = sizeMid;
		else
			return;
	}
}

void  CXTPControls::_SizeFloatableBar(XTPBUTTONINFO* pData, int nLength, DWORD dwMode, const CSize& szSeparators)
{
	if (! (dwMode & LM_LENGTHY))
	{
		int nMin, nMax, nTarget, nCurrent, nMid;

		// Wrap ToolBar as specified
		nMax = nLength;
		nTarget = _WrapToolBar(pData, nMax, dwMode, szSeparators);

		// Wrap ToolBar vertically
		nMin = 0;
		nCurrent = _WrapToolBar(pData, nMin, dwMode, szSeparators);

		if (nCurrent != nTarget)
		{
			while (nMin < nMax)
			{
				nMid = (nMin + nMax) / 2;
				nCurrent = _WrapToolBar(pData, nMid, dwMode, szSeparators);

				if (nCurrent == nTarget)
					nMax = nMid;
				else
				{
					if (nMin == nMid)
					{
						_WrapToolBar(pData, nMax, dwMode, szSeparators);
						break;
					}
					nMin = nMid;
				}
			}
		}
		CSize size = _CalcSize(pData, szSeparators);
		_WrapToolBar(pData, size.cx, dwMode, szSeparators);
	}
	else
	{
		CSize sizeMax, sizeMin, sizeMid;

		// Wrap ToolBar vertically
		_WrapToolBar(pData, 0, dwMode, szSeparators);
		sizeMin = _CalcSize(pData, szSeparators);

		// Wrap ToolBar horizontally
		_WrapToolBar(pData, 32767, dwMode, szSeparators);
		sizeMax = _CalcSize(pData, szSeparators);

		while (sizeMin.cx < sizeMax.cx)
		{
			sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
			_WrapToolBar(pData, sizeMid.cx, dwMode, szSeparators);
			sizeMid = _CalcSize(pData, szSeparators);

			if (nLength < sizeMid.cy)
			{
				if (sizeMin == sizeMid)
				{
					_WrapToolBar(pData, sizeMax.cx, dwMode, szSeparators);
					return;
				}
				sizeMin = sizeMid;
			}
			else if (nLength > sizeMid.cy)
				sizeMax = sizeMid;
			else
				return;
		}
	}
}
void CXTPControls::_CenterControlsInRow(XTPBUTTONINFO* pData, int nFirst, int nLast, int nHeight, BOOL bVert, CSize sizeResult, CRect rcBorder)
{
	for (int i = nFirst; i < nLast; i++)
	{
		XTPBUTTONINFO* pControl = &pData[i];
		CRect rc = pControl->rcControl;

		if (bVert)
		{
			pControl->rcRow.SetRect(rc.right - nHeight, rcBorder.top, rc.right, sizeResult.cy + rcBorder.top);
			if (!(m_pParent->GetFlags() & xtpFlagSmartLayout)) pControl->rcControl.OffsetRect(-(nHeight - rc.Width())/2, 0);
		}
		else
		{
			pControl->rcRow.SetRect(rcBorder.left, rc.top, sizeResult.cx + rcBorder.left, rc.top + nHeight);

			if (!(m_pParent->GetFlags() & xtpFlagSmartLayout)) pControl->rcControl.OffsetRect(0, (nHeight - rc.Height())/2);
		}
	}

}

void CXTPControls::_AdjustBorders(XTPBUTTONINFO* pData, CSize& sizeResult, DWORD dwMode, CRect rcBorder)
{
	if (dwMode & LM_COMMIT)
	{
		BOOL bVert = dwMode & LM_VERTDOCK;
		int nFirstInRow = 0;
		int nHeight = 0;
		int nBarHeight = 0;

		for (int i = 0; i < GetCount(); i++)
		{
			XTPBUTTONINFO* pControl = &pData[i];

			if (!pControl->bVisible || pControl->bHide)
				continue;

			pControl->rcControl.OffsetRect(rcBorder.left, rcBorder.top);
			if (bVert) pControl->rcControl.OffsetRect(sizeResult.cx, 0);

			if (pControl->bWrap)
			{
				_CenterControlsInRow(pData, nFirstInRow, i, nHeight, bVert, sizeResult, rcBorder);
				nFirstInRow = i;
				nHeight = 0;
			}

			nBarHeight = bVert ? sizeResult.cx : sizeResult.cy;
			nHeight = max(nHeight, bVert ? pControl->rcControl.Width() : pControl->rcControl.Height());
		}
		_CenterControlsInRow(pData, nFirstInRow, GetCount(), nFirstInRow == 0 ? nBarHeight : nHeight, bVert, sizeResult, rcBorder);
	}

	sizeResult += CSize(rcBorder.left + rcBorder.right, rcBorder.top + rcBorder.bottom);
}

void CXTPControls::_MoveRightAlligned(XTPBUTTONINFO* pData, CSize sizeResult, CRect rcBorder, DWORD dwMode)
{
	BOOL bVert = dwMode & LM_VERTDOCK;
	int nRight = bVert ? sizeResult.cy - rcBorder.bottom : sizeResult.cx - rcBorder.right;

	int i;
	for (i = GetCount() - 1; i >= 0; i--)
	{
		XTPBUTTONINFO* pControl = &pData[i];

		if (!pControl->bVisible || pControl->bHide)
			continue;

		if (!(GetAt(i)->m_dwFlags & xtpFlagRightAlign))
			break;

		CRect rc = pControl->rcControl;
		if (!bVert)
		{
			if (nRight - rc.Width() < rcBorder.right)
				break;

			pControl->rcControl.SetRect(nRight - rc.Width(), rc.top, nRight, rc.bottom);
			nRight -= rc.Width();
		}
		else
		{
			if (nRight - rc.Height() < rcBorder.top)
				break;

			pControl->rcControl.SetRect(rc.left, nRight - rc.Height(), rc.right, nRight);
			nRight -= rc.Height();
		}
		if (pControl->bWrap)
			break;
	}

	int nStretchAvail = -1;
	int nStretchedCount = 0;
	BOOL bSkipRightAligned = FALSE;

	nRight = bVert ? sizeResult.cy - rcBorder.bottom : sizeResult.cx - rcBorder.right;
	int nLastInRow = GetCount() - 1;

	for (i = GetCount() - 1; i >= 0; i--)
	{
		XTPBUTTONINFO* pControl = &pData[i];

		if (pControl->bVisible && !pControl->bHide)
		{
			if (!bSkipRightAligned && (GetAt(i)->m_dwFlags & xtpFlagRightAlign))
			{
				nRight = pControl->rcControl.left;
				nLastInRow = i - 1;
			}
			else if (GetAt(i)->m_dwFlags & xtpFlagControlStretched)
			{
				nStretchedCount++;
				bSkipRightAligned = TRUE;
			}

			if ((nStretchAvail == -1) && (nLastInRow >= i))
			{
				nStretchAvail = nRight - (!bVert ? pControl->rcControl.right : pControl->rcControl.bottom);
			}
		}

		if ((pControl->bVisible && !pControl->bHide && pControl->bWrap) || (i == 0))
		{
			if (nStretchedCount > 0 && nStretchAvail > 0)
			{
				int nOffset = 0;
				for (int j = i; j <= nLastInRow; j++)
				{
					pControl = &pData[j];

					if (!pControl->bVisible || pControl->bHide)
						continue;

					CRect rc = pControl->rcControl;
					if (!bVert)
						pControl->rcControl.SetRect(nOffset + rc.left, rc.top, nOffset + rc.right, rc.bottom);
					else
						pControl->rcControl.SetRect(rc.left, nOffset + rc.top, rc.right, nOffset + rc.bottom);

					if ((GetAt(j)->m_dwFlags & xtpFlagControlStretched) && (nStretchedCount > 0))
					{
						int nStretchedOffset = nStretchAvail / nStretchedCount;
						if (!bVert)
							pControl->rcControl.right += nStretchedOffset;
						else
							pControl->rcControl.bottom += nStretchedOffset;

						nStretchAvail -= nStretchedOffset;
						nStretchedCount--;
						nOffset += nStretchedOffset;
					}
				}

			}

			nRight = bVert ? sizeResult.cy - rcBorder.bottom : sizeResult.cx - rcBorder.right;
			bSkipRightAligned = TRUE;
			nLastInRow = i - 1;
			nStretchAvail = -1;
			nStretchedCount = 0;
		}
	}
}

int CXTPControls::GetVisibleCount(BOOL bIgnoreWraps) const
{
	DWORD nFlags = bIgnoreWraps ? 0 : xtpHideWrap;
	int nCount = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		if (GetAt(i)->IsVisible(nFlags)) nCount++;
	}
	return nCount;
}

CSize CXTPControls::_ReduceSmartLayoutToolBar(CDC* pDC, XTPBUTTONINFO* pData, int nWidth, DWORD& dwMode)
{
	CSize sizeResult(0, 0);
	BOOL bReduced = FALSE;
	BOOL bVert = dwMode & LM_VERTDOCK;

	do
	{
		bReduced = FALSE;

		if (!bReduced)
		{
			for (int i = GetCount() - 1; i >= 2; i--)
			{
				XTPBUTTONINFO* pControl = &pData[i];
				if (!pControl->bVisible || pControl->bBeginGroup)
					continue;

				int nItems = 0;
				int nButtons[3] = {0, 0, 0};

				for (int j = i; j  >= 0; j--)
				{
					pControl = &pData[j];
					if (!pControl->bVisible)
						continue;

					if ((nItems == 2 || !pControl->bBeginGroup) && GetAt(j)->m_buttonStyle == xtpButtonIconAndCaptionBelow)
					{
						nButtons[nItems] = j;
						nItems += 1;
					}
					else
					{
						break;
					}
					if (nItems == 3)
						break;
				}

				if (nItems == 3)
				{
					GetAt(nButtons[0])->m_buttonStyle = xtpButtonIconAndCaption;
					GetAt(nButtons[1])->m_buttonStyle = xtpButtonIconAndCaption;
					GetAt(nButtons[2])->m_buttonStyle = xtpButtonIconAndCaption;
					bReduced = TRUE;
					break;
				}
			}
		}

		if (!bReduced)
		{
			for (int i = GetCount() - 1; i >= 2; i--)
			{
				XTPBUTTONINFO* pControl = &pData[i];
				if (!pControl->bVisible || pControl->bBeginGroup)
					continue;

				int nItems = 0;
				int nButtons[3] = {0, 0, 0};
				BOOL bFound = FALSE;
				int j;

				for (j = i; j  >= 0; j--)
				{
					pControl = &pData[j];
					if (!pControl->bVisible)
						continue;

					if ((nItems == 2 || !pControl->bBeginGroup))
					{
						nButtons[nItems] = j;
						nItems += 1;
						bFound = bFound || GetAt(j)->m_buttonStyle == xtpButtonIconAndCaptionBelow;
					}
					else
					{
						break;
					}
					if (nItems == 3)
						break;
				}
				i = j;
				if (nItems == 3 && bFound)
				{
					GetAt(nButtons[0])->m_buttonStyle = xtpButtonIconAndCaption;
					GetAt(nButtons[1])->m_buttonStyle = xtpButtonIconAndCaption;
					GetAt(nButtons[2])->m_buttonStyle = xtpButtonIconAndCaption;
					bReduced = TRUE;
					break;
				}
			}
		}

		if (!bReduced)
		{
			for (int i = GetCount() - 1; i >= 2; i--)
			{
				XTPBUTTONINFO* pControl = &pData[i];
				if (!pControl->bVisible || pControl->bBeginGroup)
					continue;

				int nItems = 0;
				int nButtons[3] = {0, 0, 0};
				int nLeft = 0;

				for (int j = i; j  >= 0; j--)
				{
					pControl = &pData[j];
					if (!pControl->bVisible)
						continue;

					int nOffset = (bVert ? pControl->rcControl.top : pControl->rcControl.left);
					if (nItems != 0 && (nOffset != nLeft))
						break;

					if ((nItems == 2 || !pControl->bBeginGroup) && GetAt(j)->m_buttonStyle == xtpButtonIconAndCaption)
					{
						nButtons[nItems] = j;
						nItems += 1;
						nLeft = nOffset;
					}
					else
					{
						break;
					}
					if (nItems == 3)
						break;
				}

				if (nItems == 3)
				{
					GetAt(nButtons[0])->m_buttonStyle = xtpButtonIcon;
					GetAt(nButtons[1])->m_buttonStyle = xtpButtonIcon;
					GetAt(nButtons[2])->m_buttonStyle = xtpButtonIcon;
					bReduced = TRUE;
					break;
				}
			}
		}

		sizeResult = _CalcSmartLayoutToolBar(pDC, pData, dwMode);
		if ((bVert ? sizeResult.cy : sizeResult.cx) < nWidth)
			return sizeResult;
	}
	while (bReduced);

	if (dwMode & LM_HIDEWRAP)
	{
		BOOL bFirst = TRUE;

		for (int i = 0; i < GetCount(); i++)
		{
			XTPBUTTONINFO* pControl = &pData[i];
			if (!pControl->bVisible)
				continue;

			if (!bFirst && ((bVert ? pControl->rcControl.bottom : pControl->rcControl.right) > nWidth))
			{
				GetAt(i)->m_buttonStyle = xtpButtonIcon;
				pControl->bHide = TRUE;
			}
			bFirst = FALSE;
		}
		return _CalcSmartLayoutToolBar(pDC, pData, dwMode);
	}

	return sizeResult;

}

CSize CXTPControls::_CalcSmartLayoutToolBar(CDC* pDC, XTPBUTTONINFO* pData, DWORD& dwMode)
{
	BOOL bFirst = TRUE;
	BOOL bVert = dwMode & LM_VERTDOCK;

	CSize szLargeIcon = m_pParent->GetLargeIconSize(TRUE);
	int nResultHeight = szLargeIcon.cy + 7 + m_pParent->GetPaintManager()->GetSplitDropDownHeight();
	CSize sizeResult(0, nResultHeight);


	int dyPrev = 0, dxPrev = 0, x = 0;

	for (int i = 0; i < GetCount(); i++)
	{
		XTPBUTTONINFO* pControl = &pData[i];
		if (!pControl->bVisible || pControl->bHide)
			continue;

		BOOL bFullRow = GetAt(i)->m_buttonStyle == xtpButtonIconAndCaptionBelow;
		pControl->EnsureButtonSize(pDC);
		CSize szControl = pControl->szControl;

		int dx = bVert ? szControl.cy : szControl.cx;
		int dy = !bVert ? szControl.cy : szControl.cx;

		BOOL bBeginGroup = pControl->bBeginGroup && !bFirst;
		if (bBeginGroup)
			x += 6;

		if (!bFirst && !bBeginGroup && !bFullRow && dyPrev + dy <= nResultHeight)
		{
			if (bVert)
				pControl->rcControl.SetRect(-dyPrev - dy, x, -dyPrev,  x + dx);
			else
				pControl->rcControl.SetRect(x, dyPrev, x + dx, dyPrev + dy);

			dxPrev = max(dxPrev, dx);
		}
		else
		{
			x += dxPrev;
			if (bVert)
				pControl->rcControl.SetRect(-dy, x, 0, x + dx);
			else
				pControl->rcControl.SetRect(x, 0, x + dx, dy);

			sizeResult.cy = max(dy, sizeResult.cy);
			dxPrev = dx;
		}
		sizeResult.cx = max(x + dx, sizeResult.cx);

		bFirst = FALSE;
		dyPrev = bVert ? -pControl->rcControl.left : pControl->rcControl.bottom;
	}

	return bVert ? CSize(sizeResult.cy, sizeResult.cx) : sizeResult;

}

CSize CXTPControls::_WrapSmartLayoutToolBar(CDC* pDC, XTPBUTTONINFO* pData, int nWidth, DWORD& dwMode)
{
	BOOL bVert = dwMode & LM_VERTDOCK;
	CSize szLargeIcon = m_pParent->GetLargeIconSize(FALSE);

	for (int i = 0; i < GetCount(); i++)
	{
		XTPBUTTONINFO* pControl = &pData[i];
		if (!pControl->bVisible)
			continue;

		BOOL bDrawImage = GetAt(i)->GetIconId() > 0 ?
			GetAt(i)->GetImageManager()->IsPrimaryImageExists(GetAt(i)->GetIconId(), szLargeIcon.cx) != NULL: FALSE;

		pControl->bHide = FALSE;
		pControl->bWrap = FALSE;
		GetAt(i)->m_buttonStyle = bDrawImage? xtpButtonIconAndCaptionBelow : xtpButtonIconAndCaption;
	}
	CSize sizeResult = _CalcSmartLayoutToolBar(pDC, pData, dwMode);

	if ((bVert ? sizeResult.cy : sizeResult.cx) > nWidth)
	{
		sizeResult = _ReduceSmartLayoutToolBar(pDC, pData, nWidth, dwMode);
		dwMode |= LM_STRETCH;
	}

	return sizeResult;
}

CSize CXTPControls::CalcDynamicSize(CDC* pDC, int nLength, DWORD dwMode, const CRect& rcBorder, int nWidth)
{
	CSize sizeResult(0, 0);
	CSize szSeparators(m_pParent->GetPaintManager()->DrawCommandBarSeparator(pDC, m_pParent, NULL, FALSE));
	int i;

	for (i = GetCount() - 1; i >= 0; i--)
	{
		GetAt(i)->OnCalcDynamicSize(dwMode);
	}

	if (GetVisibleCount() > 0)
	{

		XTPBUTTONINFO* pData = new XTPBUTTONINFO[GetCount()];
		for (int j = 0; j < GetCount(); j++)
			pData[j].Attach(pDC, GetAt(j));

		if (dwMode & LM_MRUWIDTH)
		{
			nLength = m_pParent->m_nMRUWidth;
		}

		if ((m_pParent->GetFlags() & xtpFlagSmartLayout))
		{
			if (dwMode & LM_VERTDOCK)
			{
				sizeResult = _WrapSmartLayoutToolBar(pDC, pData, nLength - rcBorder.top - rcBorder.bottom, dwMode);
			}
			else
			{
				sizeResult = _WrapSmartLayoutToolBar(pDC, pData, nLength - rcBorder.right - rcBorder.left, dwMode);
			}

		}
		else
		{

			if (dwMode & LM_POPUP)
			{
				_WrapToolBar(pData, m_pParent->m_nMRUWidth, dwMode, szSeparators);
				CSize sz = _CalcSize(pData, szSeparators);

				if ((sz.cy > sz.cx * 3) && (m_pParent->m_nMRUWidth <= 0))
				{
					_SizePopupToolBar(pData, dwMode, szSeparators);
				}
				else
				{
					_WrapToolBar(pData, sz.cx , dwMode, szSeparators);
				}

			}
			else if (dwMode & LM_HORZDOCK)
			{
				_WrapToolBar(pData, nLength - rcBorder.right - rcBorder.left, dwMode, szSeparators);
			}
			else if (dwMode & LM_VERTDOCK)
			{
				_WrapToolBar(pData, nLength - rcBorder.top - rcBorder.bottom, dwMode, szSeparators);
			}
			else
			{
				_SizeFloatableBar(pData, nLength - ((dwMode & LM_LENGTHY) ? rcBorder.top + rcBorder.bottom : rcBorder.right + rcBorder.left), dwMode, szSeparators);
			}


			sizeResult = _CalcSize(pData, szSeparators, dwMode & LM_VERTDOCK);
		}

		if (nWidth > 0)
		{
			if (dwMode & LM_HORZDOCK)
				sizeResult.cy = max(nWidth - rcBorder.top - rcBorder.bottom, sizeResult.cy);
			else
				sizeResult.cx = max(nWidth - rcBorder.right - rcBorder.left, sizeResult.cx);
		}

		if (dwMode & LM_STRETCH)
		{
			if (dwMode & LM_VERTDOCK)
				sizeResult.cy = max(nLength - rcBorder.top - rcBorder.bottom, sizeResult.cy);
			else
				sizeResult.cx = max(nLength - rcBorder.right - rcBorder.left, sizeResult.cx);

		}
		_AdjustBorders(pData, sizeResult, dwMode, rcBorder);

		if (dwMode & LM_COMMIT)
		{
			if (!(m_pParent->GetFlags() & xtpFlagSmartLayout))
				_MoveRightAlligned(pData, sizeResult, rcBorder, dwMode);
			for (i = 0; i < GetCount(); i++) pData[i].Detach();
		}
		delete[] pData;
	}
	else
	{
		sizeResult = CSize (23, 22) + CSize(rcBorder.left + rcBorder.right, rcBorder.top + rcBorder.bottom);
		if (dwMode & LM_HORZDOCK) sizeResult.cy = max(nWidth, sizeResult.cy);
		else sizeResult.cx = max(nWidth, sizeResult.cx);
	}

	return sizeResult;


}

void CXTPControls::_MakeSameWidth(int nStart, int nLast, int nWidth)
{
	for (int i = nStart; i < nLast; i++)
	{
		CXTPControl* pControl = GetAt(i);
		if (!pControl || pControl->GetParent() != m_pParent || !pControl->IsVisible())
			continue;

		CRect rc = pControl->GetRect();
		pControl->SetRect(CRect(rc.left, rc.top, rc.left + nWidth, rc.bottom));
	}
}

CSize CXTPControls::CalcPopupSize(CDC* pDC, int nLength, const CRect& rcBorder)
{
	RECT rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);

	int i;

	CSize szSeparators(m_pParent->GetPaintManager()->DrawCommandBarSeparator(NULL, m_pParent, NULL, FALSE));


	CRect rcControl;
	int yPos = rcBorder.top, nWidthColumn = 0, xPos = rcBorder.left, nFirstInColumn = 0, nHeight = 0;

	m_pParent->m_bMultiLine = FALSE;
	int nCount = GetCount();

	BOOL bFirst = TRUE;
	for (i = 0; i < nCount; i++)
	{
		CXTPControl* pControl = GetAt(i);
		if (!pControl || pControl->GetParent() != m_pParent)
			continue;

		pControl->SetHideFlag(xtpHideScroll, FALSE);
		if (!pControl->IsVisible())
			continue;

		CXTPFontDC fontControl(pDC, m_pParent->GetPaintManager()->GetCommandBarFont(m_pParent, pControl->IsItemDefault()));
		CSize szControl = pControl->GetSize(pDC);

		pControl->SetWrap(FALSE);

		if ((pControl->GetFlags() & xtpFlagWrapRow && !bFirst) || (nLength > 0 && szControl.cy + yPos > nLength - rcBorder.bottom))
		{
			_MakeSameWidth(nFirstInColumn, i, nWidthColumn);

			nHeight = max(yPos - rcBorder.top, nHeight);
			yPos = rcBorder.top;
			xPos += nWidthColumn + (pControl->GetBeginGroup() ? szSeparators.cx : 0);
			pControl->SetWrap(TRUE);
			nFirstInColumn = i;
			nWidthColumn = 0;
			m_pParent->m_bMultiLine = TRUE;
		}
		else if (pControl->GetBeginGroup() && !bFirst)
		{
			yPos += szSeparators.cy;
		}

		bFirst = FALSE;


		if (CXTPPopupBar::m_dMaxWidthDivisor > 0 && szControl.cx > (CXTPPopupBar::m_dMaxWidthDivisor * rcWork.right))
			szControl.cx = int(CXTPPopupBar::m_dMaxWidthDivisor * rcWork.right);

		pControl->SetRect(CRect(xPos, yPos, xPos + szControl.cx, yPos + szControl.cy));
		yPos += szControl.cy;

		if (nWidthColumn < szControl.cx) nWidthColumn = szControl.cx;
	}
	_MakeSameWidth(nFirstInColumn, GetCount(), nWidthColumn);


	int nWidth = xPos - rcBorder.left + nWidthColumn;
	nHeight = max(yPos - rcBorder.top, nHeight);

	for (i = 0; i < nCount; i++)
	{
		CXTPControl* pControl = GetAt(i);
		pControl->SetRowRect(CRect(pControl->GetRect().left, rcBorder.top, pControl->GetRect().right, rcBorder.top + nHeight));
	}

	nWidth = max(24, nWidth);
	nHeight = (nHeight == 0 ? 22 : nHeight);

	return CSize(nWidth + rcBorder.left + rcBorder.right, nHeight + rcBorder.bottom + rcBorder.top);
}



CXTPControls* CXTPControls::Duplicate(BOOL bRecursive)
{
	CXTPControls* pControls = (CXTPControls*)GetRuntimeClass()->CreateObject();
	for (int i = 0; i < GetCount(); i++)
	{
		if (!GetAt(i)->m_bTemporary)
			pControls->AddClone(GetAt(i), -1, bRecursive);
	}

	if (m_pOriginalControls)
	{
		pControls->SetOriginalControls((CXTPOriginalControls*)m_pOriginalControls->Duplicate(bRecursive));
	}

	return pControls;
}


void CXTPControls::ClearOriginalControls()
{
	CMDTARGET_RELEASE(m_pOriginalControls);
}


void CXTPControls::CreateOriginalControls()
{
	ClearOriginalControls();

	CXTPOriginalControls* pControls = new CXTPOriginalControls();
	for (int i = 0; i < GetCount(); i++)
	{
		if (!GetAt(i)->m_bTemporary)
			pControls->AddClone(GetAt(i), -1, TRUE);
	}

	SetOriginalControls(pControls);
}

void CXTPControls::SetOriginalControls(CXTPOriginalControls* pControls)
{
	ClearOriginalControls();
	m_pOriginalControls = pControls;
}

void CXTPControls::MoveBefore(CXTPControl* pControl, int nBefore)
{
	ASSERT(pControl);
	ASSERT(pControl->GetControls() == this);

	if (pControl->GetIndex() < nBefore) nBefore--;

	m_arrControls.RemoveAt(pControl->GetIndex());
	m_arrControls.InsertAt(nBefore, pControl);
	RefreshIndexes();
}

BOOL CXTPControls::Compare(const CXTPControls* pOther)
{
	int j = 0, i = 0;
	while (i < GetCount() || j < pOther->GetCount())
	{
		while (i < GetCount() && GetAt(i)->m_bTemporary)
		{
			i++;
		}
		while (j < pOther->GetCount() && pOther->GetAt(j)->m_bTemporary)
		{
			j++;
		}

		if (i < GetCount() && j < pOther->GetCount())
		{
			if (!GetAt(i)->Compare(pOther->GetAt(j)))
				return FALSE;
		}
		else
		{
			if (i < GetCount() || j < pOther->GetCount())
				return FALSE;
		}

		i++;
		j++;
	}

	return TRUE;
}

BOOL CXTPControls::IsChanged() const
{
	if (!m_pOriginalControls)
		return TRUE;

	return !m_pOriginalControls->Compare(this);
}

