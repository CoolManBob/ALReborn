// XTPDockingPaneBase.cpp : implementation of the CXTPDockingPaneBase class.
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

#include "XTPDockingPaneBase.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneLayout.h"
#include "XTPDockingPanePaintManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneCaptionButtons

BOOL CXTPDockingPaneCaptionButtons::CheckForMouseOver(CPoint pt)
{
	BOOL bResult = FALSE;
	for (int i = 0; i < GetSize(); i++)
	{
		if (GetAt(i)->CheckForMouseOver(pt))
			bResult = TRUE;
	}
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
// CXTPDockingPaneBase


CXTPDockingPaneBase::CXTPDockingPaneBase(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout)
	: m_type(type), m_pLayout(pLayout)
{
	m_pParentContainer = 0;
	m_pDockingSite = GetDockingPaneManager()->GetSite();
	m_szDocking = 0;
	m_rcWindow.SetRectEmpty();

	m_nIndex = 0;
	m_nLength = 0;

	pLayout->Push(this);

	m_pCaptionButtons = NULL;

}

CXTPDockingPaneBase::~CXTPDockingPaneBase()
{
	if (m_pCaptionButtons)
	{
		for (int i = 0; i < m_pCaptionButtons->GetSize(); i++)
			delete m_pCaptionButtons->GetAt(i);

		delete m_pCaptionButtons;
		m_pCaptionButtons = NULL;
	}
}

CXTPDockingPaneCaptionButton* CXTPDockingPaneBase::FindCaptionButton(int nID) const
{
	if (!m_pCaptionButtons)
		return NULL;

	for (int i = 0; i < m_pCaptionButtons->GetSize(); i++)
	{
		if (m_pCaptionButtons->GetAt(i)->GetID() == nID)
			return m_pCaptionButtons->GetAt(i);
	}

	ASSERT(FALSE);
	return NULL;
}

void CXTPDockingPaneBase::OnSizeParent(CWnd* /*pParent*/, CRect rect, LPVOID lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	lpLayout->rect = m_rcWindow = rect;
}

void CXTPDockingPaneBase::RedrawPane()
{
	if (GetPaneHwnd())
		::InvalidateRect(GetPaneHwnd(), 0, FALSE);
}

void CXTPDockingPaneBase::OnFocusChanged()
{
}

BOOL CXTPDockingPaneBase::IsHidden() const
{
	return FALSE;
}

void CXTPDockingPaneBase::GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo) const
{
	ZeroMemory(pMinMaxInfo, sizeof(MINMAXINFO));

	if (m_type == xtpPaneTypeClient && !GetDockingPaneManager()->IsClientHidden())
	{
		pMinMaxInfo->ptMinTrackSize = GetDockingPaneManager()->m_ptMinClientSize;
	}

	pMinMaxInfo->ptMaxTrackSize = CPoint(32000, 32000);
}

POSITION CXTPDockingPaneBase::ContainPane(CXTPDockingPaneBase* pPane) const
{
	return pPane == this ? (POSITION)TRUE : NULL;
}

CRect CXTPDockingPaneBase::GetPaneWindowRect() const
{
	ASSERT(m_pDockingSite);

	if (!m_pDockingSite)
		return m_rcWindow;

	CRect rc(m_rcWindow);
	m_pDockingSite->ClientToScreen(&rc);
	return rc;
}

void CXTPDockingPaneBase::OnChildContainerChanged(CXTPDockingPaneBase* /*pContainer*/)
{
}

void CXTPDockingPaneBase::OnParentContainerChanged(CXTPDockingPaneBase* /*pContainer*/)
{
}

void CXTPDockingPaneBase::InvalidatePane(BOOL bSelectionChanged)
{
	if (m_pParentContainer)
	{
		m_pParentContainer->InvalidatePane(bSelectionChanged);
	}
}


void CXTPDockingPaneBase::RemovePane(CXTPDockingPaneBase* /*pPane*/)
{
}

void CXTPDockingPaneBase::FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList) const
{
	if (GetType() == type)
	{
		pList->AddTail((CXTPDockingPaneBase*)this);
	}
}

BOOL CXTPDockingPaneBase::IsEmpty() const
{
	return GetType() == xtpPaneTypeClient && GetDockingPaneManager()->m_bHideClient;
}

CXTPDockingPaneBase* CXTPDockingPaneBase::Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD /*dwIgnoredOptions*/)
{
	ASSERT(pMap);

	CXTPDockingPaneBase* pPane = (CXTPDockingPaneBase*)GetDockingPaneManager()->OnCreatePane(GetType(), pLayout);

	SAFE_CALLPTR(pMap, SetAt(this, pPane));

	return pPane;
}

CXTPDockingPaneManager* CXTPDockingPaneBase::GetDockingPaneManager() const
{
	ASSERT(m_pLayout);
	if (!m_pLayout)
		return NULL;

	ASSERT(m_pLayout->m_pManager);
	return m_pLayout->m_pManager;
}

CXTPDockingPanePaintManager* CXTPDockingPaneBase::GetPaintManager() const
{
	CXTPDockingPaneManager* pManager = GetDockingPaneManager();
	ASSERT(pManager);
	if (!pManager)
		return NULL;

	ASSERT(pManager->m_pPaintManager);
	return pManager->m_pPaintManager;
}

//////////////////////////////////////////////////////////////////////////

CXTPDockingPaneClientContainer::CXTPDockingPaneClientContainer(CXTPDockingPaneLayout* pLayout)
	: CXTPDockingPaneBase(xtpPaneTypeClient, pLayout)
{
}

void CXTPDockingPaneClientContainer::DeletePane()
{
	InternalRelease();
}

