// XTPDockingPaneLayout.cpp : implementation of the CXTPDockingPaneLayout class.
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
#include "Common/XTPVC80Helpers.h"
#include "TabManager/XTPTabManager.h"
#include "Common/XTPPropExchange.h"

#include "XTPDockingPaneLayout.h"
#include "XTPDockingPaneBase.h"
#include "XTPDockingPaneSplitterContainer.h"
#include "XTPDockingPaneTabbedContainer.h"
#include "XTPDockingPane.h"
#include "XTPDockingPaneMiniWnd.h"
#include "XTPDockingPaneManager.h"
#include "XTPDockingPaneAutoHidePanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AFX_STATIC_DATA const TCHAR _xtRootSection[] = _T("DockingPaneLayouts");
AFX_STATIC_DATA const TCHAR _xtPaneSection[] = _T("Pane-%d");
AFX_STATIC_DATA const TCHAR _xtPanes[] = _T("Panes");
AFX_STATIC_DATA const TCHAR _xtSummary[] = _T("Summary");

CXTPDockingPaneLayout::CXTPDockingPaneLayout(CXTPDockingPaneManager* pManager)
	: m_pManager(pManager)
{
	m_bUserLayout = TRUE;

	memset(m_wndPanels, 0, sizeof(m_wndPanels));
	memset(m_wndMargins, 0, sizeof(m_wndMargins));

	m_pClient = m_pManager->OnCreatePane(xtpPaneTypeClient, this);
	m_pTopContainer = (CXTPDockingPaneSplitterContainer*)m_pManager->OnCreatePane(xtpPaneTypeSplitterContainer, this);
	m_pTopContainer->Init(m_pClient, TRUE, m_pManager->GetSite());

}

CXTPDockingPaneLayout::~CXTPDockingPaneLayout()
{
	Free();
}

BOOL CXTPDockingPaneLayout::IsValid() const
{
	return (m_pTopContainer != NULL) && (m_pClient != NULL)
		&& (m_pClient->GetType() == xtpPaneTypeClient) && (m_pManager != NULL);
}

void CXTPDockingPaneLayout::Free()
{
	m_pTopContainer = 0;
	m_pClient = 0;

	while (!m_lstStack.IsEmpty())
	{
		CXTPDockingPaneBase* pPane = m_lstStack.RemoveTail();
		pPane->DeletePane();
	}

	m_lstPanes.RemoveAll();

	memset(m_wndPanels, 0, sizeof(m_wndPanels));

	for (int i = 0; i < 4; i++)
	{
		SAFE_DELETE(m_wndMargins[i]);
	}
}

BOOL CXTPDockingPaneLayout::_Load(CXTPPropExchange* pPX)
{
	if (!pPX->OnBeforeExchange())
		return FALSE;

	ASSERT(pPX->IsLoading());

	Free();

	CXTPPropExchangeSection secSummary(pPX->GetSection(_xtSummary));

	int nCount = 0;
	PX_Int(&secSummary, _xtPanes, nCount);

	if (nCount < 2)
		return FALSE;

	secSummary->ExchangeSchema();


	TCHAR szSection[256];
	CXTPPaneIndexToPaneMap map;

	for (int i = 1; i <= nCount; i++)
	{
		wsprintf(szSection, _xtPaneSection, i);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));

		int nType = -1;
		PX_Int(&secPane, _T("Type"), nType, -1);

		CXTPDockingPaneBase* pPane = m_pManager->OnCreatePane((XTPDockingPaneType)nType, this);

		if (!pPane)
		{
			return FALSE;
		}
		pPane->m_nIndex = i;
		map.SetAt(i, pPane);
	}

	ASSERT(map.GetCount() == m_lstStack.GetCount());

	POSITION pos = m_lstStack.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = m_lstStack.GetNext(pos);

		wsprintf(szSection, _xtPaneSection, pPane->m_nIndex);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));
		secPane->m_dwData = (DWORD_PTR)&map;

		if (!pPane->DoPropExchange(&secPane))
		{
			return FALSE;
		}
	}

	int nIndex = 0;
	if (!PX_Int(&secSummary, _T("TopContainer"), nIndex))
		return FALSE;

	m_pTopContainer = (CXTPDockingPaneSplitterContainer*)map[nIndex];
	if (!m_pTopContainer)
		return FALSE;

	if (!PX_Int(&secSummary, _T("Client"), nIndex))
		return FALSE;

	m_pClient = (CXTPDockingPaneBase*)map[nIndex];
	if (!m_pClient || m_pClient->GetType() != xtpPaneTypeClient)
		return FALSE;

	return TRUE;

}

// The main difference between the .Net versions and the normal versions of _Save is that in the .Net
// versions all the save is done is done in the exact same order of the load.

void CXTPDockingPaneLayout::_Save(CXTPPropExchange* pPX)
{
	ASSERT(!pPX->IsLoading());

	pPX->EmptySection();

	_FreeEmptyPanes();

	CXTPPropExchangeSection secSummary(pPX->GetSection(_xtSummary));

	int nCount = (int)m_lstStack.GetCount();
	PX_Int(&secSummary, _xtPanes, nCount);

	secSummary->ExchangeSchema();


	TCHAR szSection[256];

	// Assign an unique value to each pane
	int nIndex = 1;  // important

	POSITION pos = m_lstPanes.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = m_lstPanes.GetNext(pos);
		pPane->m_nIndex = nIndex++;

		wsprintf(szSection, _xtPaneSection, pPane->m_nIndex);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));
		int nType = pPane->m_type;
		PX_Int(&secPane, _T("Type"), nType);
	}

	pos = m_lstStack.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = m_lstStack.GetNext(pos);
		if (pPane->GetType() == xtpPaneTypeDockingPane) continue;
		pPane->m_nIndex = nIndex++;

		wsprintf(szSection, _xtPaneSection, pPane->m_nIndex);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));
		int nType = pPane->m_type;
		PX_Int(&secPane, _T("Type"), nType);
	}

	pos = m_lstPanes.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = m_lstPanes.GetNext(pos);
		wsprintf(szSection, _xtPaneSection, pPane->m_nIndex);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));
		pPane->DoPropExchange(&secPane);
	}

	// Save all panes
	pos = m_lstStack.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = m_lstStack.GetNext(pos);
		if (pPane->GetType() == xtpPaneTypeDockingPane) continue;
		wsprintf(szSection, _xtPaneSection, pPane->m_nIndex);

		CXTPPropExchangeSection secPane(pPX->GetSection(szSection));
		pPane->DoPropExchange(&secPane);
	}

	PX_Int(&secSummary, _T("TopContainer"), m_pTopContainer->m_nIndex);
	PX_Int(&secSummary, _T("Client"), m_pClient->m_nIndex);

}


BOOL CXTPDockingPaneLayout::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsLoading())
		return _Load(pPX);

	_Save(pPX);
	return TRUE;
}

BOOL CXTPDockingPaneLayout::Load(LPCTSTR strSection)
{
	CXTPPropExchangeSection pxRoot(TRUE, _xtRootSection);
	CXTPPropExchangeSection pxSection(pxRoot->GetSection(strSection));

	return DoPropExchange(&pxSection);
}

void CXTPDockingPaneLayout::Save(LPCTSTR strSection)
{
	CXTPPropExchangeSection pxRoot(FALSE, _xtRootSection);
	CXTPPropExchangeSection pxSection(pxRoot->GetSection(strSection));

	DoPropExchange(&pxSection);
}

void CXTPDockingPaneLayout::Serialize(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}


#ifndef _XTP_EXCLUDE_XML

BOOL CXTPDockingPaneLayout::LoadFromNode(CXTPDOMNodePtr xmlNode, LPCTSTR strSection)
{
	if (xmlNode == 0)
		return FALSE;

	CXTPPropExchangeXMLNode px(TRUE, xmlNode, strSection);
	return DoPropExchange(&px);
}

BOOL CXTPDockingPaneLayout::LoadFromFile(LPCTSTR strFileName, LPCTSTR strSection)
{
	CXTPPropExchangeXMLNode px(TRUE, 0, _xtRootSection);
	if (!px.LoadFromFile(strFileName))
		return FALSE;

	CXTPPropExchangeSection pxSection(px.GetSection(strSection));
	if (!pxSection->OnBeforeExchange())
		return FALSE;

	return DoPropExchange(&pxSection);
}

void CXTPDockingPaneLayout::SaveToNode (CXTPDOMNodePtr xmlNode, LPCTSTR strSection)
{
	if (xmlNode == 0)
		return;

	CXTPPropExchangeXMLNode px(FALSE, xmlNode, strSection);
	DoPropExchange(&px);
}

void CXTPDockingPaneLayout::SaveToFile (LPCTSTR strFileName, LPCTSTR strSection)
{
	CXTPPropExchangeXMLNode px(FALSE, 0, _xtRootSection);

	CXTPPropExchangeSection pxSection(px.GetSection(strSection));
	DoPropExchange(&pxSection);

	px.SaveToFile(strFileName);
}

#endif



BOOL CXTPDockingPaneBase::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Long(pPX, _T("DockingCX"), m_szDocking.cx, 0);
	PX_Long(pPX, _T("DockingCY"), m_szDocking.cy, 0);

	return TRUE;
}

BOOL CXTPDockingPane::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPDockingPaneBase::DoPropExchange(pPX);

	PX_String(pPX, _T("Title"), m_strTitle);
	PX_Int(pPX, _T("ID"), m_nID, 0);

	PX_Int(pPX, _T("IconID"), m_nIconID, -1);
	PX_ULong(pPX, _T("Options"), m_dwOptions, 0);

	int nIndex = 0;
	CXTPPaneIndexToPaneMap* pMap = (CXTPPaneIndexToPaneMap*)pPX->m_dwData;

	if (!pPX->IsLoading())
	{
		XTP_DOCKINGPANE_INFO* pInfo = m_pLayout->FindPane(this);
		ASSERT(pInfo);
		if (pInfo)
		{

			nIndex = pInfo->pDockingHolder ? pInfo->pDockingHolder->m_nIndex : 0;
			PX_Int(pPX, _T("DockingHolder"), nIndex, 0);
			nIndex = pInfo->pFloatingHolder ? pInfo->pFloatingHolder->m_nIndex : 0;
			PX_Int(pPX, _T("FloatingHolder"), nIndex, 0);
			nIndex = pInfo->pLastHolder ? pInfo->pLastHolder->m_nIndex : 0;
			PX_Int(pPX, _T("LastHolder"), nIndex, 0);
		}
	}
	else
	{
		XTP_DOCKINGPANE_INFO info (this);

		PX_Int(pPX, _T("DockingHolder"), nIndex, 0);
		if (nIndex) info.pDockingHolder = (CXTPDockingPaneBase*)(*pMap)[nIndex];
		PX_Int(pPX, _T("FloatingHolder"), nIndex, 0);
		if (nIndex) info.pFloatingHolder = (CXTPDockingPaneBase*)(*pMap)[nIndex];
		PX_Int(pPX, _T("LastHolder"), nIndex, 0);
		if (nIndex) info.pLastHolder = (CXTPDockingPaneBase*)(*pMap)[nIndex];
		m_pLayout->m_lstPanes.AddTail(info);

	}

	PX_Long(pPX, _T("MinTrackX"), m_ptMinTrackSize.x, 0);
	PX_Long(pPX, _T("MinTrackY"), m_ptMinTrackSize.y, 0);
	PX_Long(pPX, _T("MaxTrackX"), m_ptMaxTrackSize.x, 32000);
	PX_Long(pPX, _T("MaxTrackY"), m_ptMaxTrackSize.y, 32000);

	if (pPX->GetSchema() > _XTP_SCHEMA_97)
	{
		PX_DWord(pPX, _T("TabColor"), (DWORD&)m_clrItemTab, COLORREF_NULL);
		PX_Int(pPX, _T("HelpId"), m_nIDHelp, 0);

		DWORD dwData = (DWORD)m_dwData;
		PX_DWord(pPX, _T("Tag"), dwData, 0);
		if (pPX->IsLoading()) m_dwData = dwData;
	}

	return m_nID > 0;
}

BOOL CXTPDockingPaneSplitterContainer::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPDockingPaneBase::DoPropExchange(pPX);

	PX_Bool(pPX, _T("Horiz"), m_bHoriz, FALSE);

	int nCount = 0;

	if (!pPX->IsLoading())
	{
		nCount = (int)m_lstPanes.GetCount();
		PX_Int(pPX, _xtPanes, nCount);

		int nIndex = 1;
		TCHAR szPane[256];
		POSITION pos = m_lstPanes.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pPane = m_lstPanes.GetNext(pos);
			wsprintf(szPane, _xtPaneSection, nIndex++);

			PX_Int(pPX, szPane, pPane->m_nIndex);
		}
	}
	else
	{
		PX_Int(pPX, _xtPanes, nCount, 0);

		int nIndex = 0;
		TCHAR szPane[256];
		CXTPPaneIndexToPaneMap* pMap = (CXTPPaneIndexToPaneMap*)pPX->m_dwData;

		for (int i = 1; i <= nCount; i++)
		{
			wsprintf(szPane, _xtPaneSection, i);
			PX_Int(pPX, szPane, nIndex, 0);
			ASSERT(nIndex > 0);

			CXTPDockingPaneBase* pPane = NULL;
			ASSERT(pMap->Lookup(nIndex, pPane));

			pPane = (CXTPDockingPaneBase*)((*pMap)[nIndex]);
			if (!pPane) return FALSE;

			_InsertPane(pPane);
		}

		ASSERT(nCount > 0);

	}
	return TRUE;
}

BOOL CXTPDockingPaneAutoHidePanel::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPDockingPaneBase::DoPropExchange(pPX);

	int direction = m_direction;
	PX_Int(pPX, _T("Direction"), direction);

	int nCount = 0;

	if (!pPX->IsLoading())
	{
		nCount = (int)m_lstPanes.GetCount();
		PX_Int(pPX, _xtPanes, nCount);

		int nIndex = 1;
		TCHAR szPane[256];
		POSITION pos = m_lstPanes.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pPane = m_lstPanes.GetNext(pos);
			wsprintf(szPane, _xtPaneSection, nIndex++);
			PX_Int(pPX, szPane, pPane->m_nIndex);
		}
	}
	else
	{
		m_direction = (XTPDockingPaneDirection)direction;

		PX_Int(pPX, _xtPanes, nCount, 0);

		int nIndex = 0;
		TCHAR szPane[256];
		CXTPPaneIndexToPaneMap* pMap = (CXTPPaneIndexToPaneMap*)pPX->m_dwData;

		for (int i = 1; i <= nCount; i++)
		{
			wsprintf(szPane, _xtPaneSection, i);
			PX_Int(pPX, szPane, nIndex, 0);
			ASSERT(nIndex > 0);

			CXTPDockingPaneBase* pPane = NULL;
			ASSERT(pMap->Lookup(nIndex, pPane));

			pPane = (CXTPDockingPaneBase*)((*pMap)[nIndex]);
			if (!pPane) return FALSE;

			_InsertPane(pPane);
		}

		m_pLayout->m_wndPanels[m_direction] = this;
	}

	return TRUE;
}

BOOL CXTPDockingPaneTabbedContainer::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPDockingPaneBase::DoPropExchange(pPX);

	int nCount, nSelected;

	if (!pPX->IsLoading())
	{
		nCount = (int)m_lstPanes.GetCount();
		PX_Int(pPX, _xtPanes, nCount);

		nSelected = m_pSelectedPane ? m_pSelectedPane->m_nIndex : 0;
		PX_Int(pPX, _T("Selected"), nSelected, 0);

		POSITION pos = m_lstPanes.GetHeadPosition();
		int nIndex = 1;
		while (pos)
		{
			CXTPDockingPaneBase* pPane = m_lstPanes.GetNext(pos);

			TCHAR szPane[256];
			wsprintf(szPane, _xtPaneSection, nIndex++);

			PX_Int(pPX, szPane, pPane->m_nIndex);
		}

	}
	else
	{
		PX_Int(pPX, _xtPanes, nCount);
		PX_Int(pPX, _T("Selected"), nSelected, 0);

		TCHAR szPane[256];
		CXTPDockingPane* pSelected = NULL;
		int nIndex = 0;
		CXTPPaneIndexToPaneMap* pMap = (CXTPPaneIndexToPaneMap*)pPX->m_dwData;

		for (int i = 1; i <= nCount; i++)
		{
			wsprintf(szPane, _xtPaneSection, i);
			PX_Int(pPX, szPane, nIndex, 0);
			ASSERT(nIndex > 0);

			CXTPDockingPaneBase* pPane = NULL;
			ASSERT(pMap->Lookup(nIndex, pPane));

			pPane = (CXTPDockingPaneBase*)((*pMap)[nIndex]);

			if (!pPane || pPane->GetType() != xtpPaneTypeDockingPane) return FALSE;

			_InsertPane((CXTPDockingPane*)pPane);

			if (nIndex == nSelected) pSelected = (CXTPDockingPane*)pPane;
		}

		if (pSelected)
		{
			SelectPane(pSelected);
		}
	}

	return TRUE;
}

BOOL CXTPDockingPaneMiniWnd::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPDockingPaneBase::DoPropExchange(pPX);

	PX_Long(pPX, _T("WindowRectTopPos"), m_rcWindow.top);
	PX_Long(pPX, _T("WindowRectBottomPos"), m_rcWindow.bottom);
	PX_Long(pPX, _T("WindowRectLeftPos"), m_rcWindow.left);
	PX_Long(pPX, _T("WindowRectRightPos"), m_rcWindow.right);
	PX_Bool(pPX, _T("Collapsed"), m_bCollapsed, FALSE);
	PX_Int(pPX, _T("ExpandedHeight"), m_nExpandedHeight, 0);

	if (!pPX->IsLoading())
	{
		int nTopContainer = m_pTopContainer ? m_pTopContainer->m_nIndex : 0;
		PX_Int(pPX, _T("TopContaner"), nTopContainer);
	}
	else
	{
		int nTopContainer = 0;
		PX_Int(pPX, _T("TopContaner"), nTopContainer);

		if (nTopContainer == 0)
			return FALSE;

		CXTPPaneIndexToPaneMap* pMap = (CXTPPaneIndexToPaneMap*)pPX->m_dwData;

		CXTPDockingPaneBase* pTop = (CXTPDockingPaneBase*)(*pMap)[nTopContainer];
		if (!pTop || pTop->GetType() != xtpPaneTypeSplitterContainer)
			return FALSE;

		m_pTopContainer = (CXTPDockingPaneSplitterContainer*)pTop;
		m_pTopContainer->m_pParentContainer = this;

	}

	return TRUE;
}

void CXTPDockingPaneLayout::Copy(const CXTPDockingPaneLayout* pLayout)
{
	Free();

	CXTPPaneToPaneMap map;

	// Clone them
	POSITION pos = pLayout->m_lstStack.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneBase* pPane = pLayout->m_lstStack.GetNext(pos);
		if (pPane->m_pParentContainer == NULL) // hidden pane, top pane, miniwnd.
		{
			pPane->Clone(this, &map);
		}
	}

	ASSERT(map.GetCount() == pLayout->m_lstStack.GetCount());
	ASSERT(map.Lookup(pLayout->m_pClient, m_pClient));
	ASSERT(map.Lookup(pLayout->m_pTopContainer, (CXTPDockingPaneBase*&)m_pTopContainer));

	m_pClient = map[pLayout->m_pClient];
	m_pTopContainer = (CXTPDockingPaneSplitterContainer*)map[pLayout->m_pTopContainer];

	for (int i = 0; i < 4; i++)
		if (pLayout->m_wndPanels[i]) m_wndPanels[i] = (CXTPDockingPaneAutoHidePanel*)map[pLayout->m_wndPanels[i]];

	// Map Holders
	pos = pLayout->m_lstPanes.GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = pLayout->GetPaneList().GetNext(pos);
		XTP_DOCKINGPANE_INFO infoCopy;
		ASSERT(map.Lookup((CXTPDockingPaneBase*)info.pPane, (CXTPDockingPaneBase*&)infoCopy.pPane));

		infoCopy.pPane = (CXTPDockingPane*)map[(CXTPDockingPaneBase*)info.pPane];

		if (info.pDockingHolder) infoCopy.pDockingHolder = (CXTPDockingPaneBase*)map[info.pDockingHolder];
		if (info.pLastHolder) infoCopy.pLastHolder = (CXTPDockingPaneBase*)map[info.pLastHolder];
		if (info.pFloatingHolder) infoCopy.pFloatingHolder = (CXTPDockingPaneBase*)map[info.pFloatingHolder];

		m_lstPanes.AddTail(infoCopy);
	}
}

void CXTPDockingPaneLayout::_FreeEmptyPanes()
{

	WORD w;
	CMapPtrToWord map;

	POSITION pos = m_lstPanes.GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = m_lstPanes.GetNext(pos);
		map.SetAt(info.pDockingHolder, TRUE);
		map.SetAt(info.pFloatingHolder, TRUE);
		map.SetAt(info.pPane->m_pParentContainer, TRUE);
	}

	BOOL bFound = TRUE;

	while (bFound)
	{
		bFound = FALSE;

		POSITION posRemove = pos = m_lstStack.GetHeadPosition();
		while (pos)
		{
			CXTPDockingPaneBase* pPane = m_lstStack.GetNext(pos);
			CXTPDockingPaneBase* pParentContainer = pPane->m_pParentContainer;

			switch (pPane->GetType())
			{
			case xtpPaneTypeTabbedContainer:
				if (!map.Lookup(pPane, w))
				{

					SAFE_CALLPTR(pParentContainer, RemovePane(pPane));

					m_lstStack.RemoveAt(posRemove);
					pPane->DeletePane();
					bFound = TRUE;
				}
				break;

			case xtpPaneTypeSplitterContainer:
				if (((CXTPDockingPaneSplitterContainer*)pPane)->m_lstPanes.GetCount() == 0)
				{
					SAFE_CALLPTR(pParentContainer, RemovePane(pPane));

					m_lstStack.RemoveAt(posRemove);
					pPane->DeletePane();
					bFound = TRUE;
				}
				else if (pParentContainer && (pParentContainer->GetType() == xtpPaneTypeSplitterContainer)
					&& (((CXTPDockingPaneSplitterContainer*)pPane)->m_lstPanes.GetCount() == 1)
					&& (((CXTPDockingPaneSplitterContainer*)pParentContainer)->m_lstPanes.GetCount() == 1))
				{
					CXTPDockingPaneBase* pChildPane = ((CXTPDockingPaneSplitterContainer*)pPane)->m_lstPanes.RemoveHead();
					((CXTPDockingPaneSplitterContainer*)pParentContainer)->m_lstPanes.RemoveHead();
					((CXTPDockingPaneSplitterContainer*)pParentContainer)->m_lstPanes.AddHead(pChildPane);
					pChildPane->m_pParentContainer = pParentContainer;

					m_lstStack.RemoveAt(posRemove);
					pPane->DeletePane();
					bFound = TRUE;

				}
				break;

			case xtpPaneTypeMiniWnd:
				if (((CXTPDockingPaneMiniWnd*)pPane)->m_pTopContainer == NULL)
				{
					m_lstStack.RemoveAt(posRemove);
					pPane->DeletePane();
					bFound = TRUE;
				}
				break;
			}
			posRemove = pos;
		}
	}

}


XTP_DOCKINGPANE_INFO* CXTPDockingPaneLayout::FindPane(CXTPDockingPane* pPane)
{
	POSITION pos = m_lstPanes.GetHeadPosition();
	while (pos)
	{
		XTP_DOCKINGPANE_INFO& info = m_lstPanes.GetNext(pos);

		if (info.pPane == pPane)
			return &info;
	}
	return 0;
}

XTPDockingPaneDirection CXTPDockingPaneLayout::_GetPaneDirection(const CXTPDockingPaneBase* pPane) const
{
	if (pPane->GetType() == xtpPaneTypeMiniWnd)
		return (XTPDockingPaneDirection)4;

	if (!pPane->m_pParentContainer || pPane->GetType() == xtpPaneTypeClient)
		return (XTPDockingPaneDirection)-1;

	if (pPane->m_pParentContainer->GetType() == xtpPaneTypeAutoHidePanel)
	{
		return ((CXTPDockingPaneAutoHidePanel*)pPane->m_pParentContainer)->m_direction;
	}

	if (pPane->m_pParentContainer->GetType() == xtpPaneTypeSplitterContainer)
	{
		POSITION pos = pPane->m_pParentContainer->ContainPane(m_pClient);
		if (pos)
		{
			CXTPDockingPaneSplitterContainer* pSplitter = (CXTPDockingPaneSplitterContainer*)pPane->m_pParentContainer;

			if (pSplitter->_Before(pPane, pos))
			{
				return  pSplitter->IsHoriz() ? xtpPaneDockLeft : xtpPaneDockTop;

			}
			else
			{
				return  pSplitter->IsHoriz() ? xtpPaneDockRight : xtpPaneDockBottom;
			}
		}
	}
	return _GetPaneDirection(pPane->m_pParentContainer);
}

void CXTPDockingPaneLayout::_AddPanesTo(CXTPDockingPaneTabbedContainer* pContainer, CXTPDockingPaneBaseList& lst, DWORD dwIgnoredOptions)
{
	POSITION pos = lst.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lst.GetNext(pos);

		if ((((CXTPDockingPane*)pPane)->GetOptions() & dwIgnoredOptions) == 0)
		{
			SAFE_CALLPTR(pPane->m_pParentContainer, RemovePane(pPane));
			pContainer->_InsertPane(pPane);
		}
	}
}

BOOL CXTPDockingPaneLayout::_FindTabbedPaneToHide(CXTPDockingPaneAutoHidePanel* pPanel, CXTPDockingPaneBase* pPane)
{
	CXTPDockingPaneBaseList lst;
	pPane->FindPane(xtpPaneTypeDockingPane, &lst);

	if (lst.IsEmpty())
		return FALSE;

	CXTPDockingPane* pPanePrimary = (CXTPDockingPane*)lst.GetHead();
	XTP_DOCKINGPANE_INFO* pInfo = FindPane(pPanePrimary);
	if (!pInfo || pInfo->pDockingHolder == NULL)
		return FALSE;

	POSITION pos = pPanel->m_lstPanes.GetHeadPosition();
	while (pos)
	{
		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)pPanel->m_lstPanes.GetNext(pos);
		CXTPDockingPane* pContainerPane = (CXTPDockingPane*)pContainer->GetFirstPane();
		if (pContainerPane)
		{
			XTP_DOCKINGPANE_INFO* pContanerPaneInfo = FindPane(pContainerPane);

			if (pContanerPaneInfo && pContanerPaneInfo->pDockingHolder == pInfo->pDockingHolder)
			{
				_AddPanesTo(pContainer, lst, xtpPaneNoHideable);
				return TRUE;
			}
		}
	}



	return FALSE;
}

void CXTPDockingPaneLayout::HidePane(CXTPDockingPaneBase* pPane)
{
	ASSERT(!m_bUserLayout);

	XTPDockingPaneDirection direction = _GetPaneDirection(pPane);
	if (direction < 0 || direction >= 4)
		direction = xtpPaneDockLeft;

	if (m_wndPanels[direction] == 0)
	{
		m_wndPanels[direction] = (CXTPDockingPaneAutoHidePanel*)m_pManager->
			OnCreatePane(xtpPaneTypeAutoHidePanel, this);
		m_wndPanels[direction]->m_direction = direction;
	}

	if (_FindTabbedPaneToHide(m_wndPanels[direction], pPane))
		return;

	if (pPane->GetType() == xtpPaneTypeDockingPane)
	{
		pPane->m_pParentContainer->RemovePane(pPane);

		CXTPDockingPaneTabbedContainer* pContainer = (CXTPDockingPaneTabbedContainer*)m_pManager->
			OnCreatePane(xtpPaneTypeTabbedContainer, this);
		pContainer->Init((CXTPDockingPane*)pPane, m_pManager->GetSite());

		pPane = pContainer;

	}
	else if (pPane->GetType() == xtpPaneTypeTabbedContainer)
	{
		pPane = ((CXTPDockingPaneBase*)pPane)->Clone(this, 0, xtpPaneNoHideable);
	}

	m_wndPanels[direction]->_InsertPane(pPane);
}

void CXTPDockingPaneLayout::RepositionMargins(CRect& rect, const CRect& rcClientMargins)
{
	int i;

	if (rcClientMargins.IsRectNull())
	{
		for (i = 0; i < 4; i++)
		{
			SAFE_DELETE(m_wndMargins[i]);
		}
		return;
	}

	for (i = 0; i < 4; i++)
	{
		if (m_wndMargins[i] == 0)
		{
			m_wndMargins[i] = new CXTPDockingPaneSplitterWnd();
			m_wndMargins[i]->Create(m_pManager, i == xtpPaneDockTop || i == xtpPaneDockBottom);
		}
	}

	CRect rcClient(rect);
	rcClient.DeflateRect(rcClientMargins);

	m_wndMargins[xtpPaneDockLeft]->MoveWindow(CRect(rect.left, rect.top, rcClient.left, rect.bottom));
	m_wndMargins[xtpPaneDockRight]->MoveWindow(CRect(rcClient.right , rect.top, rect.right, rect.bottom));
	m_wndMargins[xtpPaneDockTop]->MoveWindow(CRect(rcClient.left, rect.top, rcClient.right, rcClient.top));
	m_wndMargins[xtpPaneDockBottom]->MoveWindow(CRect(rcClient.left, rcClient.bottom, rcClient.right, rect.bottom));

	for (i = 0; i < 4; i++)
	{
		m_wndMargins[i]->Invalidate(FALSE);
	}

	rect = rcClient;
}

void CXTPDockingPaneLayout::OnSizeParent(CWnd* pParent, CRect rect, LPVOID lParam)
{
	AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

	for (int i = 0; i < 4; i++)
	{
		SAFE_CALLPTR(m_wndPanels[i], OnSizeParentEx(pParent, rect, lParam));
	}

	int nMargin = m_pManager->GetClientMargin();
	CRect rcClientMargins(nMargin, nMargin, nMargin, nMargin);

	if (lpLayout->hDWP != NULL)
	{
		RepositionMargins(rect, rcClientMargins);
	}
	else
	{
		rect.DeflateRect(rcClientMargins);
	}

	m_pClient->OnSizeParent(pParent, rect, lParam);
	m_pTopContainer->OnSizeParent(pParent, rect, lParam);
}


void CXTPDockingPaneLayout::DestroyPane(CXTPDockingPane* pPane)
{
	if (pPane == NULL) return;

	SAFE_CALLPTR(pPane->m_pParentContainer, RemovePane(pPane));

	POSITION pos = m_lstPanes.GetHeadPosition();
	while (pos)
	{
		if (m_lstPanes.GetAt(pos) == pPane)
		{
			m_lstPanes.RemoveAt(pos);
			break;
		}
		m_lstPanes.GetNext(pos);
	}

	pos = m_lstStack.GetHeadPosition();
	while (pos)
	{
		if (m_lstStack.GetAt(pos) == pPane)
		{
			m_lstStack.RemoveAt(pos);
			break;
		}
		m_lstStack.GetNext(pos);
	}
	((CXTPDockingPane*)pPane)->InternalRelease();
	_FreeEmptyPanes();

}


