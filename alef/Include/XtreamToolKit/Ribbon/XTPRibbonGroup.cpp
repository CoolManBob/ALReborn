// XTPRibbonGroup.cpp: implementation of the CXTPRibbonGroup class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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

#include "Common/XTPPropExchange.h"
#include "Common/XTPImageManager.h"

#include "CommandBars/XTPControl.h"
#include "CommandBars/XTPControls.h"
#include "CommandBars/XTPControlPopup.h"
#include "CommandBars/XTPControlButton.h"
#include "CommandBars/XTPPopupBar.h"
#include "CommandBars/XTPToolBar.h"
#include "CommandBars/XTPCommandBars.h"

#include "XTPRibbonGroup.h"
#include "XTPRibbonGroups.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonTheme.h"
#include "XTPRibbonQuickAccessControls.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CXTPRibbonGroup, CCmdTarget)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CXTPRibbonGroupPopupToolBar : public CXTPPopupToolBar
{
public:
	CXTPRibbonGroupPopupToolBar(CXTPRibbonGroup* pRibbonGroup)
	{
		m_pRibbonBar = pRibbonGroup->GetRibbonBar();
		m_pRibbonGroup = (CXTPRibbonGroup*)pRibbonGroup->GetRuntimeClass()->CreateObject();
		m_pRibbonGroup->m_pParent = this;

		m_pRibbonGroup->OnGroupAdded();
		m_pRibbonGroup->Copy(pRibbonGroup);

		int nHeight = m_pRibbonBar->GetRibbonPaintManager()->GetGroupCaptionHeight();
		m_rcBorders.SetRect(2, 2, 3, nHeight + 1);

		m_bTrackOnHover = FALSE;
		SetFlags(xtpFlagSmartLayout);

		EnableAnimation(m_pRibbonBar->IsAnimationEnabled());
	}

	~CXTPRibbonGroupPopupToolBar()
	{
		m_pRibbonGroup->OnGroupRemoved();
		m_pRibbonGroup->InternalRelease();
	}

	CSize CalcDynamicLayout(int, DWORD /*nMode*/)
	{
		m_bDynamicLayout = TRUE;

		CClientDC dc(this);
		CXTPFontDC font(&dc, GetPaintManager()->GetCommandBarFont(this));

		UpdateShortcuts();

		int nGroupHeight = m_pRibbonBar->CalcGroupsHeight();

		m_pRibbonGroup->OnBeforeCalcSize(&dc);
		int nWidth = m_pRibbonGroup->OnCalcDynamicSize(&dc);
		m_pRibbonGroup->OnAdjustBorders(nWidth, CRect(0, 0, 0, 0));
		m_pRibbonGroup->OnAfterCalcSize();

		m_rcTearOffGripper.SetRectEmpty();

		m_bDynamicLayout = FALSE;

		return CSize(nWidth + m_rcBorders.left + m_rcBorders.right, nGroupHeight - 7);
	}

	void DrawCommandBar(CDC* pDC, CRect rcClipBox)
	{
		m_pRibbonBar->GetRibbonPaintManager()->FillGroupPopupToolBar(pDC, m_pRibbonGroup, this);

		m_pRibbonGroup->Draw(pDC, rcClipBox);
	}

	CSize GetIconSize() const
	{
		return m_pRibbonBar->GetIconSize();
	}
	CSize GetButtonSize() const
	{
		return m_pRibbonBar->GetButtonSize();
	}
public:
	CXTPRibbonGroup* m_pRibbonGroup;
	CXTPRibbonBar* m_pRibbonBar;
};

class CXTPRibbonGroupOption : public CXTPControlButton
{
	void Draw(CDC* pDC)
	{
		m_pRibbonGroup->GetRibbonBar()->GetRibbonPaintManager()->DrawControlGroupOption(pDC, this, TRUE);
	}
};

class CXTPRibbonGroupControlPopup : public CXTPControlPopup
{
	DECLARE_XTP_CONTROL(CXTPRibbonGroupControlPopup)

public:
	CXTPRibbonGroupControlPopup(CXTPRibbonGroup* pGroup = 0)
	{
		m_pRibbonGroup = pGroup;
		m_pPopupGroup = pGroup;
		SetFlags(xtpFlagManualUpdate);
		m_controlType = xtpControlButtonPopup;
		m_pCommandBar = CXTPPopupBar::CreatePopupBar(0);
	}

	virtual CSize GetSize (CDC* pDC)
	{
		return ((CXTPRibbonTheme*)GetParent()->GetPaintManager())->DrawControlGroupPopup(pDC, this, FALSE);
	}

	void Draw(CDC* pDC)
	{
		((CXTPRibbonTheme*)GetParent()->GetPaintManager())->DrawControlGroupPopup(pDC, this, TRUE);
	}

	BOOL IsTransparent() const
	{
		return TRUE;
	}

	virtual void Copy(CXTPControl* pControl, BOOL bRecursive /* = FALSE */)
	{
		CXTPControlPopup::Copy(pControl, bRecursive);

		m_pPopupGroup = ((CXTPRibbonGroupControlPopup*)pControl)->m_pPopupGroup;
	}

	void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
	{
		CXTPRibbonBar* pBar = (CXTPRibbonBar*)pCommandBarList->GetCommandBars()->GetMenuBar();
		ASSERT(pBar);
		ASSERT(pBar->IsRibbonBar());

		if (!pBar || !pBar->IsRibbonBar())
			return;

		CXTPRibbonGroup* pGroup = pBar->FindGroup(GetID());
		ASSERT(pGroup);
		if (!pGroup)
			return;

		m_pPopupGroup = pGroup;
	}

	void GenerateCommandBarList(DWORD& /*nID*/, CXTPCommandBarList* /*pCommandBarList*/, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* /*pParam*/)
	{
	}

	virtual void OnLButtonUp(CPoint /*point*/)
	{

	}
	virtual BOOL IsVisible(DWORD dwSkipFlags = 0) const
	{
		return CXTPControlPopup::IsVisible(dwSkipFlags) &&
			(!m_pPopupGroup || m_pPopupGroup->m_bVisible);
	}

	BOOL OnSetPopup(BOOL bPopup);

protected:
	CXTPRibbonGroup* m_pPopupGroup;
};

IMPLEMENT_XTP_CONTROL(CXTPRibbonGroupControlPopup, CXTPControlPopup)


BOOL CXTPRibbonBar::IsAllowQuickAccessControl(CXTPControl* pControl)
{
	if (!pControl)
		return FALSE;

	if (!m_bAllowQuickAccessDuplicates)
	{
		CXTPControl* pControlQuickAccess = m_pQuickAccessControls->FindDuplicate(pControl);
		if (pControlQuickAccess)
		{
			return FALSE;
		}
	}

//  if (pControl->GetType() == xtpControlGallery)
//      return FALSE;

	if (pControl->IsKindOf(RUNTIME_CLASS(CXTPRibbonGroupControlPopup)))
	{
		return pControl->GetID() > 0;
	}

	if ((pControl->GetFlags() & xtpFlagNoMovable) == 0)
		return TRUE;

	return FALSE;
}


BOOL CXTPRibbonGroupControlPopup::OnSetPopup(BOOL bPopup)
{
	m_bPopup = bPopup;
	if (bPopup)
	{
		CMDTARGET_RELEASE(m_pCommandBar);

		CXTPCommandBars* pCommandBars = m_pParent->GetCommandBars();
		ASSERT(pCommandBars);

		if (!m_pPopupGroup)
		{
			m_pCommandBar = new CXTPPopupToolBar();
			m_pCommandBar->SetCommandBars(pCommandBars);
			m_pCommandBar->Popup(this, IsKeyboardSelected(GetSelected()));
		}
		else
		{

			m_pCommandBar = new CXTPRibbonGroupPopupToolBar(m_pPopupGroup);
			m_pCommandBar->SetCommandBars(pCommandBars);

			m_pCommandBar->EnableCustomization(FALSE);
			m_pCommandBar->Popup(this, IsKeyboardSelected(GetSelected()));
		}
	}
	else
	{
		ASSERT(m_pCommandBar);
		m_pCommandBar->SetTrackingMode(FALSE);

	}
	RedrawParent();
	return TRUE;
}


CXTPRibbonGroup::CXTPRibbonGroup()
{
	m_nIndex = -1;
	m_pGroups = NULL;
	m_pParent = NULL;
	m_pRibbonBar = NULL;
	m_rcGroup.SetRectEmpty();
	m_nId = -1;
	m_nIconId = -1;
	m_bReduced = FALSE;
	m_bVisible = TRUE;
	m_bControlsCentering = FALSE;


	m_pControlGroupOption = new CXTPRibbonGroupOption();
	m_pControlGroupOption->m_pRibbonGroup = this;

	m_pControlGroupPopup = new CXTPRibbonGroupControlPopup(this);

	m_pControlGroupPopup->m_buttonRibbonStyle = xtpButtonIconAndCaptionBelow;

	m_bShowOptionButton = FALSE;

	m_pLayoutInfo = NULL;

	m_nRowCount = 2;
	m_bControlsGrouping = FALSE;
}

CXTPRibbonGroup::~CXTPRibbonGroup()
{
	RemoveAll();

	CMDTARGET_RELEASE(m_pControlGroupOption);
	CMDTARGET_RELEASE(m_pControlGroupPopup);
}

void CXTPRibbonGroup::SetID(int nId)
{
	m_nId = nId;
	m_pControlGroupOption->SetID(m_nId);
	m_pControlGroupPopup->SetID(m_nId);
}

void CXTPRibbonGroup::Copy(CXTPRibbonGroup* pGroup)
{
	m_nId = pGroup->m_nId;
	m_nIconId = pGroup->m_nIconId;
	m_bShowOptionButton = pGroup->m_bShowOptionButton;
	m_strCaption = pGroup->m_strCaption;
	m_pRibbonBar = pGroup->m_pRibbonBar;
	m_pGroups = pGroup->m_pGroups;
	m_bControlsGrouping = pGroup->m_bControlsGrouping;
	m_bControlsCentering = pGroup->m_bControlsCentering;
	m_bVisible = pGroup->m_bVisible;

	int nControlPos = m_pControlGroupOption->GetIndex();

	for (int i = 0; i < pGroup->GetCount(); i++)
	{
		CXTPControl* pControl = pGroup->GetAt(i);

		CXTPControl* pClone = m_pParent->GetControls()->AddClone(pControl, nControlPos + i);
		pClone->m_pRibbonGroup = this;
		pClone->SetHideFlag(xtpHideRibbonTab, FALSE);
		m_arrControls.Add(pClone);
		pClone->InternalAddRef();
	}

	m_pControlGroupOption->Copy(pGroup->m_pControlGroupOption, FALSE);
	m_pControlGroupOption->SetHideFlag(xtpHideRibbonTab, FALSE);

	m_pControlGroupPopup->Copy(pGroup->m_pControlGroupPopup, FALSE);
	m_pControlGroupPopup->SetHideFlag(xtpHideRibbonTab, FALSE);
}

void CXTPRibbonGroup::Remove(CXTPControl* pControl)
{
	for (int nIndex = 0; nIndex < m_arrControls.GetSize(); nIndex++)
	{
		if (m_arrControls.GetAt(nIndex) == pControl)
		{
			RemoveAt(nIndex);
			return;
		}
	}
}

void CXTPRibbonGroup::RemoveAt(int nIndex)
{
	if (nIndex >= 0 && nIndex < GetCount())
	{
		CXTPControl* pControl = m_arrControls.GetAt(nIndex);
		pControl->m_pRibbonGroup = NULL;
		m_arrControls.RemoveAt(nIndex);

		GetRibbonBar()->GetControls()->Remove(pControl);
		pControl->InternalRelease();
	}
}

void CXTPRibbonGroup::RemoveAll()
{
	for (int nIndex = (int)m_arrControls.GetSize() - 1; nIndex >= 0; nIndex--)
	{
		CXTPControl* pControl = m_arrControls.GetAt(nIndex);
		pControl->m_pRibbonGroup = NULL;
		m_arrControls.RemoveAt(nIndex);

		m_pParent->GetControls()->Remove(pControl);
		pControl->InternalRelease();
	}
}

CXTPRibbonTab* CXTPRibbonGroup::GetParentTab() const
{
	return m_pGroups->GetParentTab();
}

BOOL CXTPRibbonGroup::IsHighlighted() const
{
	return GetRibbonBar()->GetHighlightedGroup() == this;
}

int CXTPRibbonGroup::GetMinimumWidth(CDC* pDC)
{
	CXTPFontDC fnt(pDC, &GetRibbonBar()->GetRibbonPaintManager()->m_fontSmCaption);
	CSize sz = pDC->GetTextExtent(m_strCaption);

	if (m_bShowOptionButton)
		sz.cx += GetRibbonBar()->GetRibbonPaintManager()->GetGroupCaptionHeight();

	return sz.cx + 2;
}

BOOL CXTPRibbonGroup::IsOptionButtonVisible() const
{
	return !m_bReduced && m_bShowOptionButton;
}

void CXTPRibbonGroup::SetRect(CRect rc)
{
	m_rcGroup = rc;

	if (m_bReduced)
	{
		CRect rcEntry(rc.left, rc.top, rc.right, rc.bottom);
		m_pControlGroupPopup->SetHideFlag(xtpHideGeneric, FALSE);
		m_pControlGroupPopup->SetRect(rcEntry);
		m_pControlGroupPopup->SetWrap(TRUE);
	}
	else
	{
		m_pControlGroupPopup->SetHideFlag(xtpHideGeneric, TRUE);
	}

	if (!m_bShowOptionButton || m_bReduced)
	{
		m_pControlGroupOption->SetHideFlag(xtpHideGeneric, TRUE);
	}
	else
	{
		int nHeight = GetRibbonBar()->GetRibbonPaintManager()->GetGroupCaptionHeight();
		m_pControlGroupOption->SetWrap(TRUE);
		m_pControlGroupOption->SetHideFlag(xtpHideGeneric, FALSE);
		m_pControlGroupOption->SetRect(CRect(rc.right - nHeight - 1, rc.bottom - nHeight, rc.right - 3, rc.bottom - 3));
	}
}

void CXTPRibbonGroup::RepositionControls(CDC* pDC)
{
	CXTPRibbonTheme* pPaintManager = GetRibbonBar()->GetRibbonPaintManager();

	int x = m_rcGroup.left + 4;
	int y = m_rcGroup.top + pPaintManager->GetGroupCaptionHeight();

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControl* pControl = GetAt(i);
		if (!pControl->IsVisible())
			continue;

		CSize sz = pControl->GetSize(pDC);
		pControl->SetRect(CRect(x, y, x + sz.cx, y + sz.cy));

		x += sz.cx;
	}
}


void CXTPRibbonGroup::Draw(CDC* pDC, CRect rcClipBox)
{
	GetRibbonBar()->GetRibbonPaintManager()->DrawGroup(pDC, this);

	BOOL bFirst = TRUE;

	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControl* pControl = GetAt(i);
		if (!pControl->IsVisible())
			continue;

		if (!m_bControlsGrouping && pControl->GetBeginGroup() && !bFirst)
		{
			GetRibbonBar()->GetPaintManager()->
				DrawCommandBarSeparator(pDC, GetRibbonBar(), pControl);
		}

		if (CRect().IntersectRect(rcClipBox, pControl->GetRect()))
		{
			pControl->Draw(pDC);
		}

		bFirst = FALSE;
	}

	if (m_pControlGroupPopup->IsVisible())
	{
		m_pControlGroupPopup->Draw(pDC);
	}

	if (m_pControlGroupOption->IsVisible())
	{
		m_pControlGroupOption->Draw(pDC);
	}
}

CXTPControl* CXTPRibbonGroup::Add(XTPControlType controlType, int nId, LPCTSTR lpszParameter, int nBefore, BOOL bTemporary)
{
	if (nBefore < 0 || nBefore >= GetCount())
		nBefore = (int)GetCount();

	int nControlPos = m_pControlGroupOption->GetIndex() - GetCount() + nBefore;

	CXTPControl* pControl = GetRibbonBar()->GetControls()->Add(controlType, nId, lpszParameter, nControlPos, bTemporary);
	ASSERT(pControl);

	m_arrControls.InsertAt(nBefore, pControl);
	pControl->InternalAddRef();

	pControl->m_pRibbonGroup = this;
	pControl->SetHideFlag(xtpHideRibbonTab, !IsVisible());

	return pControl;
}

void CXTPRibbonGroup::SetVisible(BOOL bVisible)
{
	if (m_bVisible != bVisible)
	{
		m_bVisible = bVisible;

		GetRibbonBar()->RebuildControls(GetRibbonBar()->GetSelectedTab());
		GetRibbonBar()->OnRecalcLayout();
	}
}

BOOL CXTPRibbonGroup::IsVisible() const
{
	if (m_pRibbonBar != m_pParent)
		return m_bVisible;

	return GetRibbonBar()->GetSelectedTab() == GetParentTab() && m_bVisible;
}

void CXTPRibbonGroup::OnGroupAdded()
{
	CXTPControl* pGroupOption = GetControlGroupOption();

	m_pParent->GetControls()->Add(pGroupOption);
	pGroupOption->InternalAddRef();

	pGroupOption->SetHideFlag(xtpHideRibbonTab, !IsVisible());


	pGroupOption->SetID(GetID());

	CXTPControl* pGroupPopup = (CXTPControl*)GetControlGroupPopup();
	m_pParent->GetControls()->Add(pGroupPopup);
	pGroupPopup->SetCaption(GetCaption());
	pGroupPopup->SetDescription(NULL);

	pGroupPopup->InternalAddRef();

	pGroupPopup->SetHideFlag(xtpHideRibbonTab, !IsVisible());
}

void CXTPRibbonGroup::OnGroupRemoved()
{
	RemoveAll();

	if (m_pParent == m_pRibbonBar)
	{
		GetControlGroupPopup()->m_pRibbonGroup = NULL;
		m_pParent->GetControls()->Remove(GetControlGroupPopup());

		if (m_pRibbonBar->m_pHighlightedGroup == this)
		{
			m_pRibbonBar->m_pHighlightedGroup = NULL;
		}
	}

	GetControlGroupOption()->m_pRibbonGroup = NULL;
	m_pParent->GetControls()->Remove(GetControlGroupOption());
}


BOOL CXTPRibbonGroup::SetButtons(UINT* pButtons, int nCount)
{
	BOOL bSeparator = FALSE;

	CXTPRibbonBar* pRibbonBar = GetRibbonBar();
	CWnd* pSite = pRibbonBar->GetSite();

	for (int i = 0; i < nCount; i++)
	{
		if (pButtons[i] == 0)
			bSeparator = TRUE;
		else
		{
			XTPControlType controlType = xtpControlButton;
			XTPButtonStyle buttonStyle = xtpButtonAutomatic;
			CXTPControl* pControl = NULL;
			UINT nID = pButtons[i];

			XTP_COMMANDBARS_CREATECONTROL cs;

			if (pSite)
			{
				cs.nID = nID;
				cs.pControl = NULL;
				cs.bToolBar = TRUE;
				cs.pMenu = NULL;
				cs.nIndex = i;
				cs.strCaption = pRibbonBar->GetTitle();
				cs.controlType = controlType;
				cs.pCommandBar = pRibbonBar;
				cs.buttonStyle = buttonStyle;

				if (pSite->SendMessage(WM_XTP_BEFORECREATECONTROL, 0, (LPARAM)&cs) != 0)
				{
					pControl = cs.pControl;
					controlType = cs.controlType;
					buttonStyle = cs.buttonStyle;
					nID = cs.nID;
				}
			}

			int nControlPos = m_pControlGroupOption->GetIndex();

			if (pControl == NULL)
			{
				pControl = pRibbonBar->GetControls()->Add(controlType, nID, NULL, nControlPos);
				if (pControl)
				{
					pControl->SetStyle(buttonStyle);
					if (controlType == xtpControlPopup) pControl->SetIconId(nID);
				}
			}
			else pRibbonBar->GetControls()->Add(pControl, nID, NULL, nControlPos);

			if (bSeparator)
			{
				pControl->SetBeginGroup(TRUE);
				bSeparator = FALSE;
			}

			m_arrControls.InsertAt(GetCount(), pControl);
			pControl->InternalAddRef();

			pControl->m_pRibbonGroup = this;
			pControl->SetHideFlag(xtpHideRibbonTab, !IsVisible());

			if (pSite)
			{
				cs.pControl = pControl;
				pSite->SendMessage(WM_XTP_AFTERCREATECONTROL, 0, (LPARAM)&cs);
			}
		}
	}
	return TRUE;
}

CXTPControl* CXTPRibbonGroup::Add(CXTPControl* pControl, int nId, LPCTSTR lpszParameter, int nBefore, BOOL bTemporary)
{
	if (nBefore < 0 || nBefore >= GetCount())
		nBefore = (int)GetCount();

	int nControlPos = m_pControlGroupOption->GetIndex() - GetCount() + nBefore;

	ASSERT(pControl);
	GetRibbonBar()->GetControls()->Add(pControl, nId, lpszParameter, nControlPos, bTemporary);

	m_arrControls.InsertAt(nBefore, pControl);
	pControl->InternalAddRef();

	pControl->m_pRibbonGroup = this;
	pControl->SetHideFlag(xtpHideRibbonTab, !IsVisible());

	return pControl;
}

CXTPControl* CXTPRibbonGroup::InsertAt(CXTPControl* pControl, int nBefore)
{
	if (nBefore < 0 || nBefore >= GetCount())
		nBefore = (int)GetCount();

	int nControlPos = m_pControlGroupOption->GetIndex() - GetCount() + nBefore;

	ASSERT(pControl);
	GetRibbonBar()->GetControls()->InsertAt(pControl, nControlPos);

	m_arrControls.InsertAt(nBefore, pControl);
	pControl->InternalAddRef();

	pControl->m_pRibbonGroup = this;
	pControl->SetHideFlag(xtpHideRibbonTab, !IsVisible());

	return pControl;
}

BOOL CXTPRibbonGroup::LoadToolBar(UINT nIDResource, BOOL bLoadIcons)
{
	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResource);

	struct CToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		WORD* items()
		{ return (WORD*)(this + 1); }
	};

	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);


	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;

	ASSERT(pData->wVersion == 1);

	int i;
	UINT* pItems = new UINT[pData->wItemCount];

	for (i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];

	BOOL bResult = SetButtons(pItems, pData->wItemCount);

	if (bLoadIcons)
	{
		CXTPImageManager* pImageManager = GetRibbonBar()->GetImageManager();

		if (!pImageManager->SetIcons(nIDResource, pItems,
			pData->wItemCount, CSize(pData->wWidth, pData->wHeight)))
			bResult = FALSE;
	}

	delete[] pItems;

	UnlockResource(hGlobal);
	FreeResource(hGlobal);


	return bResult;
}

