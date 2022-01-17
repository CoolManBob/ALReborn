// XTPRibbonTab.cpp: implementation of the CXTPRibbonTab class.
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

#include "XTPRibbonTab.h"
#include "XTPRibbonGroup.h"
#include "XTPRibbonGroups.h"
#include "XTPRibbonBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////
//

CXTPRibbonTabContextHeader::CXTPRibbonTabContextHeader(CXTPRibbonTab* pFirstTab)
{
	ASSERT(pFirstTab);
	m_pLastTab = m_pFirstTab = pFirstTab;
	m_color = pFirstTab->GetContextColor();
	m_strCpation = pFirstTab->GetContextCaption();
	m_rcRect.SetRectEmpty();
	pFirstTab->m_pContextHeader = this;
}

CXTPRibbonTabContextHeaders::CXTPRibbonTabContextHeaders()
{

}

CXTPRibbonTabContextHeaders::~CXTPRibbonTabContextHeaders()
{
	RemoveAll();
}
int CXTPRibbonTabContextHeaders::GetCount() const
{
	return (int)m_arrHeaders.GetSize();
}

CXTPRibbonTabContextHeader* CXTPRibbonTabContextHeaders::GetHeader(int nIndex) const
{
	return m_arrHeaders.GetAt(nIndex);
}

void CXTPRibbonTabContextHeaders::RemoveAll()
{
	for (int i = 0; i < GetCount(); i++)
	{
		delete m_arrHeaders[i];
	}
	m_arrHeaders.RemoveAll();
}

CXTPRibbonTabContextHeader* CXTPRibbonTabContextHeaders::HitTest(CPoint pt) const
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (m_arrHeaders[i]->m_rcRect.PtInRect(pt))
			return m_arrHeaders[i];
	}
	return NULL;
}

void CXTPRibbonTabContextHeaders::Add(CXTPRibbonTabContextHeader* pHeader)
{
	m_arrHeaders.Add(pHeader);
}

IMPLEMENT_DYNCREATE(CXTPRibbonTab, CXTPTabManagerItem)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPRibbonTab::CXTPRibbonTab()
{
	m_nId = 0;

	m_pGroups = new CXTPRibbonGroups;
	m_pGroups->m_pParentTab = this;

	m_nContextColor = xtpRibbonTabContextColorNone;

	m_pContextHeader = NULL;
	m_pRibbonBar = NULL;
	m_pParent = NULL;
}

CXTPRibbonTab::~CXTPRibbonTab()
{
	ASSERT(m_pGroups->GetCount() == 0);

	CMDTARGET_RELEASE(m_pGroups);
}

void CXTPRibbonTab::Copy(CXTPRibbonTab* pRibbonTab)
{
	m_nContextColor = pRibbonTab->m_nContextColor;
	m_strContextCaption= pRibbonTab->m_strContextCaption;
	m_nId = pRibbonTab->m_nId;
	m_pRibbonBar = pRibbonTab->m_pRibbonBar;

	m_pGroups->Copy(pRibbonTab->m_pGroups);
}

void CXTPRibbonTab::OnRemoved()
{
	m_pGroups->RemoveAll();
}

CXTPRibbonBar* CXTPRibbonTab::GetRibbonBar() const
{
	return m_pRibbonBar;
}

CXTPRibbonGroup* CXTPRibbonTab::AddGroup(LPCTSTR lpszCaption)
{
	return GetGroups()->Add(lpszCaption);
}

CXTPRibbonGroup* CXTPRibbonTab::AddGroup(int nID)
{
	CString strCaption;
	strCaption.LoadString(nID);

	if (strCaption.Find('\n') != -1)
	{
		CString strPrompt = strCaption;
		AfxExtractSubString(strCaption, strPrompt, 1);
	}

	return GetGroups()->Add(strCaption, nID);
}

void CXTPRibbonTab::SetVisible(BOOL bVisible)
{
	if (bVisible == IsVisible())
		return;

	CXTPTabManagerItem::SetVisible(bVisible);

	if (!bVisible && IsSelected())
	{
		CXTPTabManagerItem* pItem = GetTabManager()->FindNextFocusable(-1, +1);
		if (pItem)
		{
			pItem->Select();
		}
	}
}

CXTPRibbonGroup* CXTPRibbonTab::FindGroup(int nID) const
{
	for (int i = 0 ; i < m_pGroups->GetCount(); i++)
	{
		if (m_pGroups->GetAt(i)->GetID() == nID)
			return m_pGroups->GetAt(i);
	}
	return NULL;
}

