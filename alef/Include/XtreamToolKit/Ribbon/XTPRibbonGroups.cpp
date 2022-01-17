// XTPRibbonGroups.cpp: implementation of the CXTPRibbonGroups class.
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

#include "Common/XTPImageManager.h"
#include "CommandBars/XTPControl.h"
#include "CommandBars/XTPControlPopup.h"
#include "CommandBars/XTPControls.h"

#include "XTPRibbonGroup.h"
#include "XTPRibbonGroups.h"
#include "XTPRibbonTab.h"
#include "XTPRibbonBar.h"
#include "XTPRibbonTheme.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPRibbonGroups::CXTPRibbonGroups()
{
	m_pParentTab = NULL;
	m_nGroupsScrollPos = 0;


}

CXTPRibbonGroups::~CXTPRibbonGroups()
{
	RemoveAll();
}

void CXTPRibbonGroups::RemoveAll()
{
	for (int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		CXTPRibbonGroup* pGroup = GetAt(nIndex);
		pGroup->OnGroupRemoved();
		pGroup->InternalRelease();
	}
	m_arrGroups.RemoveAll();
	RefreshIndexes();
}

void CXTPRibbonGroups::RefreshIndexes()
{
	for (int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		CXTPRibbonGroup* pGroup = GetAt(nIndex);
		pGroup->m_nIndex = nIndex;
	}
}

CXTPRibbonBar* CXTPRibbonGroups::GetRibbonBar() const
{
	return m_pParentTab->GetRibbonBar();
}

CXTPRibbonGroup* CXTPRibbonGroups::Add(LPCTSTR lpszCaption, int nId)
{
	return InsertAt(GetCount(), lpszCaption, nId);
}

CXTPRibbonGroup* CXTPRibbonGroups::InsertAt(int nIndex, CXTPRibbonGroup* pGroup)
{
	pGroup->m_pGroups = this;
	pGroup->m_pRibbonBar = m_pParentTab->GetRibbonBar();
	pGroup->m_pParent = m_pParentTab->GetParent();

	m_arrGroups.InsertAt(nIndex, pGroup);

	RefreshIndexes();

	pGroup->OnGroupAdded();

	return pGroup;
}

CXTPRibbonGroup* CXTPRibbonGroups::InsertAt(int nIndex, LPCTSTR lpszCaption, int nId)
{
	CXTPRibbonGroup* pGroup = new CXTPRibbonGroup;
	pGroup->SetID(nId);
	pGroup->SetCaption(lpszCaption);

	InsertAt(nIndex, pGroup);

	pGroup->LoadToolBar(nId);

	return pGroup;
}

void CXTPRibbonGroups::Remove(int nIndex)
{
	if ((nIndex >= 0) && (nIndex < GetCount()))
	{
		CXTPRibbonGroup* pGroup = GetAt(nIndex);
		ASSERT(pGroup);

		pGroup->OnGroupRemoved();
		m_arrGroups.RemoveAt(nIndex);
		pGroup->InternalRelease();
		RefreshIndexes();
	}
}

CXTPRibbonGroup* CXTPRibbonGroups::HitTest(CPoint point) const
{
	for (int nIndex = 0; nIndex < GetCount(); nIndex++)
	{
		CXTPRibbonGroup* pGroup = GetAt(nIndex);
		if (pGroup->IsVisible() && pGroup->GetRect().PtInRect(point))
		{
			return pGroup;
		}
	}

	return NULL;

}

void CXTPRibbonGroups::Copy(CXTPRibbonGroups* pGroups)
{
	RemoveAll();

	for (int nIndex = 0; nIndex < pGroups->GetCount(); nIndex++)
	{
		CXTPRibbonGroup* pGroup = pGroups->GetAt(nIndex);
		CXTPRibbonGroup* pClone = (CXTPRibbonGroup*)pGroup->GetRuntimeClass()->CreateObject();
		InsertAt(nIndex, pClone);

		pClone->Copy(pGroup);
		pClone->m_pGroups = this;
	}
}


//

struct CXTPRibbonGroup::CONTROLINFO
{
	CRect rcControl;
	CRect rcRow;
	CSize szControl;
	BOOL bHide;
	BOOL bWrap;
	BOOL bBeginGroup;
	BOOL bWrapGroup;
	XTPButtonStyle buttonStyle;
	CXTPControl* pControl;
	int nTailWidth;

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
		rcRow.SetRectEmpty();

		szControl = pControl->GetSize(pDC);
		rcControl = pControl->GetRect();
		bBeginGroup = pControl->GetBeginGroup();
		bWrapGroup = bBeginGroup || (pControl->GetFlags() & xtpFlagWrapRow);
		buttonStyle = pControl->GetStyle();
		bWrap = bHide = FALSE;
		nTailWidth = 0;
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

struct CXTPRibbonGroup::LAYOUTINFO
{
	CONTROLINFO* pControls;
	int nControlCount;
	int nMinWidth;
	int nWidth;
	BOOL bOldReduced;
};

BOOL CXTPRibbonGroup::OnReduceSize(int nLevel)
{
	if (m_pLayoutInfo == NULL)
		return FALSE;

	if (m_pLayoutInfo->nWidth < m_pLayoutInfo->nMinWidth)
		return FALSE;

	BOOL bReduced = FALSE;

	int nResultHeight = GetRibbonBar()->CalcGroupsHeight() - 2 -
		GetRibbonBar()->GetRibbonPaintManager()->GetGroupCaptionHeight() ;

	int nButtonSize = GetRibbonBar()->GetButtonSize().cy;

	int nControlsPerGroup = int(nResultHeight / nButtonSize);

	int nDataCount = m_pLayoutInfo->nControlCount;
	CONTROLINFO* pData = m_pLayoutInfo->pControls;


	bReduced = FALSE;


	int* pButtons = new int[nControlsPerGroup];

	if (nLevel == 0 && !m_bControlsGrouping)
	{
		if (nControlsPerGroup > 1)
		{
			for (int i = nDataCount - 1; i >= 2; i--)
			{
				if (pData[i].bWrapGroup)
					continue;

				int nItems = 0;

				for (int j = i; j  >= 0; j--)
				{
					CONTROLINFO* pControl = &pData[j];

					if ((nItems == nControlsPerGroup - 1 || !pControl->bWrapGroup) && pControl->pControl->m_buttonRibbonStyle == xtpButtonIconAndCaptionBelow)
					{
						pButtons[nItems] = j;
						nItems += 1;
					}
					else
					{
						break;
					}
					if (nItems == nControlsPerGroup)
						break;
				}

				if (nItems == nControlsPerGroup)
				{
					for (int k = 0; k < nItems; k++)
						pData[pButtons[k]].pControl->m_buttonRibbonStyle = xtpButtonIconAndCaption;
					bReduced = TRUE;
					break;
				}
			}
		}
	}

	if (nLevel == 1 && !m_bControlsGrouping)
	{
		if (nControlsPerGroup > 1)
		{

			for (int i = nDataCount - 1; i >= 2; i--)
			{
				if (pData[i].bWrapGroup)
					continue;

				int nItems = 0;
				BOOL bFound = FALSE;
				int j;

				for (j = i; j  >= 0; j--)
				{
					CONTROLINFO* pControl = &pData[j];

					if ((nItems == nControlsPerGroup - 1 || !pControl->bWrapGroup))
					{
						pButtons[nItems] = j;
						nItems += 1;
						bFound = bFound || pControl->pControl->m_buttonRibbonStyle == xtpButtonIconAndCaptionBelow;
					}
					else
					{
						break;
					}
					if (nItems == nControlsPerGroup)
						break;
				}
				i = j;
				if (nItems == nControlsPerGroup && bFound)
				{
					for (int k = 0; k < nItems; k++)
						pData[pButtons[k]].pControl->m_buttonRibbonStyle = xtpButtonIconAndCaption;

					bReduced = TRUE;
					break;
				}
			}
		}
	}

	if (nLevel == 1 && m_bControlsGrouping && m_nRowCount == 2)
	{
		m_nRowCount = 3;
		bReduced = TRUE;
	}


	if (nLevel == 2 && !m_bControlsGrouping)
	{
		if (nControlsPerGroup > 1)
		{
			for (int i = nDataCount - 1; i >= 2; i--)
			{
				if (pData[i].bWrapGroup)
					continue;

				int nItems = 0;
				int nLeft = 0;

				for (int j = i; j  >= 0; j--)
				{
					CONTROLINFO* pControl = &pData[j];

					int nOffset = pControl->rcControl.left;
					if (nItems != 0 && (nOffset != nLeft))
						break;

					if ((nItems == nControlsPerGroup - 1 || !pControl->bWrapGroup) && pControl->pControl->m_buttonRibbonStyle == xtpButtonIconAndCaption)
					{
						pButtons[nItems] = j;
						nItems += 1;
						nLeft = nOffset;
					}
					else
					{
						break;
					}
					if (nItems == nControlsPerGroup)
						break;
				}

				if (nItems == nControlsPerGroup && GetCount() > nControlsPerGroup)
				{
					for (int k = 0; k < nItems; k++)
						pData[pButtons[k]].pControl->m_buttonRibbonStyle = xtpButtonIcon;
					bReduced = TRUE;
					break;
				}
			}
		}
	}

	if (nLevel == 2 && !bReduced && !m_bControlsGrouping)
	{
		for (int i = nDataCount - 1; i >= 0; i--)
		{
			if (pData[i].pControl->GetType() == xtpControlGallery &&
				pData[i].pControl->m_buttonRibbonStyle != xtpButtonIconAndCaptionBelow)
			{
				pData[i].pControl->m_buttonRibbonStyle = xtpButtonIconAndCaptionBelow;
				bReduced = TRUE;
				break;
			}
		}
	}

	delete[] pButtons;


	if (nLevel == 3)
	{
		if (nDataCount > 1 && !m_bReduced)
		{
			m_bReduced = TRUE;

			for (int j = 0; j < nDataCount; j++)
			{
				CONTROLINFO* pControl = &pData[j];
				pControl->bHide = TRUE;
			}

			bReduced = TRUE;
		}
	}

	return bReduced;
}

void CXTPRibbonGroup::OnBeforeCalcSize(CDC* pDC)
{
	CSize szLargeIcon = GetRibbonBar()->GetLargeIconSize(FALSE);

	m_pLayoutInfo = new LAYOUTINFO;
	m_pLayoutInfo->pControls = NULL;
	m_pLayoutInfo->nMinWidth = GetMinimumWidth(pDC);
	m_pLayoutInfo->bOldReduced = m_bReduced;

	int nVisibleCount = 0;
	int j;

	for (j = 0; j < GetCount(); j++)
	{
		CXTPControl* pControl = GetAt(j);

		if (pControl->IsVisible(xtpHideWrap))
		{
			nVisibleCount++;
		}
	}

	m_pLayoutInfo->nControlCount = nVisibleCount;

	if (nVisibleCount == 0)
		return;

	m_pLayoutInfo->pControls = new CONTROLINFO[nVisibleCount];

	int k = 0;

	for (j = 0; j < GetCount(); j++)
	{
		CXTPControl* pControl = GetAt(j);

		if (pControl->IsVisible(xtpHideWrap))
		{
			pControl->m_buttonRibbonStyle = xtpButtonAutomatic;

			if (!m_bControlsGrouping && pControl->GetStyle() == xtpButtonAutomatic && pControl->GetType() != xtpControlGallery)
			{
				BOOL bDrawImage = pControl->GetIconId() > 0 ?
					pControl->GetImageManager()->IsPrimaryImageExists(pControl->GetIconId(), szLargeIcon.cx) != NULL: FALSE;

				pControl->m_buttonRibbonStyle = bDrawImage? xtpButtonIconAndCaptionBelow : xtpButtonIconAndCaption;
			}

			m_pLayoutInfo->pControls[k].Attach(pDC, pControl);
			k++;
		}
	}


	ASSERT(k == nVisibleCount);

	m_bReduced = FALSE;
	m_nRowCount = 2;
}

void CXTPRibbonGroup::OnAfterCalcSize()
{
	if (!m_pLayoutInfo)
		return;

	for (int i = 0; i < m_pLayoutInfo->nControlCount; i++)
	{
		if (i == 0)
			m_pLayoutInfo->pControls[i].bWrap = TRUE;

		m_pLayoutInfo->pControls[i].Detach();
	}

	if (m_pLayoutInfo->bOldReduced && !m_bReduced)
	{
		GetRibbonBar()->m_bGroupReducedChanged = TRUE;

	}

	delete[] m_pLayoutInfo->pControls;
	delete m_pLayoutInfo;
}

int CXTPRibbonGroup::_WrapSpecialDynamicSize(int nWidth, BOOL bIncludeTail)
{
	int nDataCount = m_pLayoutInfo->nControlCount;
	CONTROLINFO* pData = m_pLayoutInfo->pControls;

	int nWraps = 0, x = 0;

	for (int i = 0; i < nDataCount; i++)
	{
		CONTROLINFO* pControl = &pData[i];

		pControl->bWrap = FALSE;
		CSize szControl = pControl->szControl;
		BOOL bBeginGroup = pControl->bBeginGroup && i > 0;
		BOOL bWrapGroup = pControl->bWrapGroup && i > 0;

		if (bWrapGroup)
		{
			if ((bIncludeTail && (x + pControl->nTailWidth >= nWidth))
				|| (!bIncludeTail && nWraps == 0 && (x + pControl->nTailWidth >= nWidth)))
			{
				x = szControl.cx;
				pControl->bWrap = TRUE;
				nWraps++;
				continue;
			}
		}

		if (bBeginGroup)
			x += 3;

		x += szControl.cx;
	}
	return nWraps;
}

int CXTPRibbonGroup::_GetSizeSpecialDynamicSize()
{
	int nTotalWidth = 0, x = 0, y = 0, nRowHeight = 0;

	int nDataCount = m_pLayoutInfo->nControlCount;
	CONTROLINFO* pData = m_pLayoutInfo->pControls;

	for (int i = 0; i < nDataCount; i++)
	{
		CONTROLINFO* pControl = &pData[i];

		CSize szControl = pControl->szControl;

		if (pControl->bWrap)
		{
			x = 0;
			y += nRowHeight;
		}
		else
		{
			BOOL bBeginGroup = pControl->bBeginGroup && i > 0;

			if (bBeginGroup)
				x += 3;
		}

		pControl->rcControl.SetRect(2 + x, y, 2 + x + szControl.cx, y + szControl.cy);

		x += szControl.cx;
		nRowHeight = max(nRowHeight, szControl.cy);
		nTotalWidth = max(nTotalWidth, x);
	}
	return nTotalWidth + 4;
}

BOOL CXTPRibbonGroup::_FindBestWrapSpecialDynamicSize()
{
	int nMax = _GetSizeSpecialDynamicSize();
	int nMin = 0, nMid = 0;

	int nCurrent = _WrapSpecialDynamicSize(nMin, TRUE);
	int nTarget = 2;
	if (nCurrent < nTarget)
		return FALSE;

	if (nCurrent == nTarget)
		return TRUE;

	while (nMin < nMax)
	{
		nMid = (nMin + nMax) / 2;
		nCurrent = _WrapSpecialDynamicSize(nMid, TRUE);

		if (nCurrent <= nTarget)
		{
			nMax = nMid;
		}
		else
		{
			if (nMin == nMid)
			{
				_WrapSpecialDynamicSize(nMax, TRUE);
				break;
			}
			nMin = nMid;
		}
	}

	return TRUE;
}

int CXTPRibbonGroup::_CalcSpecialDynamicSize(CDC* /*pDC*/)
{
	int nResultHeight = GetRibbonBar()->CalcGroupsHeight() - 9 -
		GetRibbonBar()->GetRibbonPaintManager()->GetGroupCaptionHeight();

	int i;

	int nDataCount = m_pLayoutInfo->nControlCount;
	CONTROLINFO* pData = m_pLayoutInfo->pControls;

	int nTailWidth = 0;
	for (i = nDataCount - 1; i >= 0; i--)
	{
		CONTROLINFO* pControl = &pData[i];

		CSize szControl = pControl->szControl;

		nTailWidth += szControl.cx;
		pControl->nTailWidth = nTailWidth;

		if (pControl->bWrapGroup)
		{
			nTailWidth = 0;
		}
	}

	int nBaseHeight = GetRibbonBar()->GetRibbonPaintManager()->GetEditHeight();

	_WrapSpecialDynamicSize(32000, FALSE);
	int nTotalWidth = _GetSizeSpecialDynamicSize();
	int nRowCount = 3;
	BOOL bAllow3Row = nResultHeight >= nBaseHeight * 3;
	BOOL bAllow2Row = nResultHeight >= nBaseHeight * 2 ;

	if (!bAllow2Row)
	{
		_WrapSpecialDynamicSize(32000, FALSE);
		nRowCount = 1;

	}
	else
	{
		if (!(bAllow3Row && m_nRowCount == 3 && _FindBestWrapSpecialDynamicSize()))
		{
			nRowCount = _WrapSpecialDynamicSize(nTotalWidth / 2, FALSE) + 1;
		}
	}
	nTotalWidth = _GetSizeSpecialDynamicSize();

	int nOffset = (nResultHeight - nBaseHeight * nRowCount) / (nRowCount + 1);
	int nTotalOffset = nOffset;

	for (i = 0; i < nDataCount; i++)
	{
		CONTROLINFO* pControl = &pData[i];

		if (i > 0 && pControl->bWrap)
		{
			nTotalOffset += nOffset;
		}

		pControl->rcControl.OffsetRect(0, nTotalOffset);
	}


	m_pLayoutInfo->nWidth = nTotalWidth;

	return max(m_pLayoutInfo->nWidth, m_pLayoutInfo->nMinWidth);
}

int CXTPRibbonGroup::OnCalcDynamicSize(CDC* pDC)
{
	if (m_bReduced)
	{
		CSize szControl = GetControlGroupPopup()->GetSize(pDC);
		return m_pLayoutInfo->nWidth = szControl.cx;
	}

	if (m_bControlsGrouping)
		return _CalcSpecialDynamicSize(pDC);

	int nResultHeight = GetRibbonBar()->CalcGroupsHeight() - 9 -
		GetRibbonBar()->GetRibbonPaintManager()->GetGroupCaptionHeight() ;

	CSize sizeResult(0, nResultHeight);


	int dyPrev = 0, dxPrev = 0, x = 2;

	int nDataCount = m_pLayoutInfo->nControlCount;
	CONTROLINFO* pData = m_pLayoutInfo->pControls;

	for (int i = 0; i < nDataCount; i++)
	{
		CONTROLINFO* pControl = &pData[i];

		BOOL bFullRow = pControl->pControl->GetStyle() == xtpButtonIconAndCaptionBelow;
		pControl->EnsureButtonSize(pDC);
		CSize szControl = pControl->szControl;

		int dx = szControl.cx;
		int dy = szControl.cy;

		BOOL bBeginGroup = pControl->bBeginGroup && i > 0;
		BOOL bWrapGroup = pControl->bWrapGroup && i > 0;

		if (bBeginGroup)
			x += 6;

		if (i > 0 && !bWrapGroup && !bFullRow && dyPrev + dy <= nResultHeight)
		{
			pControl->rcControl.SetRect(x, dyPrev, x + dx, dyPrev + dy);

			dxPrev = max(dxPrev, dx);
		}
		else
		{
			x += dxPrev;
			pControl->rcControl.SetRect(x, 0, x + dx, dy);
			dxPrev = dx;
		}
		sizeResult.cx = max(x + dx, sizeResult.cx);

		dyPrev = pControl->rcControl.bottom;
	}

	m_pLayoutInfo->nWidth = sizeResult.cx + 2;

	return max(m_pLayoutInfo->nWidth, m_pLayoutInfo->nMinWidth);
}

void CXTPRibbonGroups::_ReduceSmartLayoutToolBar(CDC* pDC, int* pWidth, int nWidth)
{
	int nResult = _CalcSmartLayoutToolBar(pWidth);
	if (nResult <= nWidth)
		return;

	BOOL bReduced = FALSE;

	do
	{
		bReduced = FALSE;

		for (int nLevel = 0; nLevel < 4; nLevel++)
		{
			for (int i = GetCount() - 1; i >= 0; i--)
			{
				CXTPRibbonGroup* pGroup = GetAt(i);
				if (!pGroup->IsVisible())
					continue;

				if (pGroup->OnReduceSize(nLevel))
				{
					pWidth[i] = pGroup->OnCalcDynamicSize(pDC);
					bReduced = TRUE;
					break;
				}
			}

			if (bReduced)
				break;
		}

		if (bReduced)
		{
			nResult = _CalcSmartLayoutToolBar(pWidth);
			if (nResult < nWidth)
				return;
		}
	}
	while (bReduced);
}

int CXTPRibbonGroups::_CalcSmartLayoutToolBar(int* pWidth)
{
	int nWidth = 0;
	for (int i = 0; i < GetCount(); i++)
	{
		nWidth += pWidth[i];

		if (i != GetCount() - 1 && pWidth[i] != 0)
		{
			nWidth += 7;
		}
	}
	return nWidth;
}


void CXTPRibbonGroup::CenterColumn(int nFirstItem, int nLastItem, int nGroupHeight)
{
	int nHeight = 0;
	int i;
	for (i = nFirstItem; i <= nLastItem; i++)
	{
		CONTROLINFO* pControl = &m_pLayoutInfo->pControls[i];

		if (pControl->bHide)
			continue;

		nHeight +=pControl->rcControl.Height();
	}

	if (nHeight >= nGroupHeight)
		return;

	int nItems = nLastItem - nFirstItem + 1;
	int nOffset = (nGroupHeight - nHeight) / (nItems + 1);

	for (i = nFirstItem; i <= nLastItem; i++)
	{
		CONTROLINFO* pControl = &m_pLayoutInfo->pControls[i];

		if (pControl->bHide)
			continue;

		pControl->rcControl.OffsetRect(0, nOffset);
		nOffset += nOffset;
	}
}

void CXTPRibbonGroup::OnAdjustBorders(int nWidth, CRect rcBorder)
{
	CXTPRibbonTheme* pPaintManager = GetRibbonBar()->GetRibbonPaintManager();
	int nGroupCaptionHeigh = pPaintManager->GetGroupCaptionHeight();
	int nGroupHeight = GetRibbonBar()->CalcGroupsHeight();
	int nGroupClientHeight = nGroupHeight - 2 - 3 - 2;

	int i;
	BOOL bFirstItem = TRUE;
	int nFirstItem = 0;
	int nColumn = 0;
	BOOL bCenterItems = m_bControlsCentering;
	if (IsControlsGrouping())
		bCenterItems = FALSE;

	int nControlCount = m_pLayoutInfo ? m_pLayoutInfo->nControlCount : 0;

	for (i = 0; i < nControlCount; i++)
	{
		CONTROLINFO* pControl = &m_pLayoutInfo->pControls[i];

		if (pControl->bHide)
			continue;

		pControl->rcControl.OffsetRect(rcBorder.left + 2, rcBorder.top + 2);
		pControl->rcRow = CRect(rcBorder.left, rcBorder.top,
			rcBorder.left + nWidth, rcBorder.top + nGroupClientHeight - nGroupCaptionHeigh);

		if (bCenterItems && !bFirstItem)
		{
			if (nColumn != pControl->rcControl.left)
			{
				CenterColumn(nFirstItem, i - 1, nGroupClientHeight - nGroupCaptionHeigh - 3);
				nColumn = pControl->rcControl.left;
				nFirstItem = i;
			}
		}
		else
		{
			bFirstItem = FALSE;
			nColumn = pControl->rcControl.left;
		}
	}
	if (bCenterItems && nControlCount > 0) CenterColumn(nFirstItem, m_pLayoutInfo->nControlCount - 1, nGroupClientHeight - nGroupCaptionHeigh - 3);

	int nBeginGroup = rcBorder.left;
	int nEndGroup = nBeginGroup + nWidth + 5;

	SetRect(CRect(nBeginGroup, rcBorder.top, nEndGroup,  rcBorder.top + nGroupClientHeight));
}


void CXTPRibbonGroups::CalcDynamicSize(CDC* pDC, int nLength, DWORD /*dwMode*/, const CRect& rcBorder)
{
	CXTPRibbonScrollableBar* pScrollableBar = GetScrollableBar(m_pParentTab->GetParent());

	int nCount = GetCount();

	if (nCount == 0)
	{
		pScrollableBar->EnableGroupsScroll(FALSE, FALSE);
		return;
	}

	int* pData = new int[nCount];
	int i;

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonGroup* pGroup = GetAt(i);
		if (pGroup->IsVisible())
		{
			pGroup->OnBeforeCalcSize(pDC);
		}
	}

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonGroup* pGroup = GetAt(i);
		pData[i] = pGroup->IsVisible() ? pGroup->OnCalcDynamicSize(pDC) : 0;
	}

	int nGroupsLength = nLength - rcBorder.right - rcBorder.left;

	_ReduceSmartLayoutToolBar(pDC, pData, nGroupsLength);

	CRect rcBorders = rcBorder;

	int nTotalWidth = -7;

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonGroup* pGroup = GetAt(i);
		if (pGroup->IsVisible())
		{
			nTotalWidth += pData[i] + 2 + 5;
		}
	}

	int nScrollPos = pScrollableBar->m_nGroupsScrollPos;

	if (nTotalWidth > nGroupsLength)
	{
		if (nScrollPos > nTotalWidth - nGroupsLength)
		{
			nScrollPos = nTotalWidth - nGroupsLength;
		}
	}
	else
	{
		nScrollPos = 0;
	}
	if (nScrollPos < 0) nScrollPos = 0;

	m_nGroupsScrollPos = nScrollPos;

	pScrollableBar->EnableGroupsScroll(nScrollPos > 0, nTotalWidth - nGroupsLength - nScrollPos > 0);

	rcBorders.left -= nScrollPos;

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonGroup* pGroup = GetAt(i);
		if (pGroup->IsVisible())
		{
			pGroup->OnAdjustBorders(pData[i], rcBorders);
			rcBorders.left += pData[i] + 2 + 5;
		}
	}

	for (i = 0; i < nCount; i++)
	{
		CXTPRibbonGroup* pGroup = GetAt(i);
		if (pGroup->IsVisible())
		{
			pGroup->OnAfterCalcSize();
		}
	}

	delete[] pData;
}



