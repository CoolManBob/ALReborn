// XTThemeManager.cpp: implementation of the CXTThemeManager class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#include "XTThemeManager.h"
#include "Common/XTPDrawHelpers.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CProcessLocal<CXTThemeManager> CXTThemeManager::m_sThemeManager;


//////////////////////////////////////////////////////////////////////////
// CXTThemeManagerStyle

CXTThemeManagerStyle::CXTThemeManagerStyle()
: m_enumTheme(xtThemeDefault)
, m_pFactory(NULL)
{

}

CXTThemeManagerStyle::~CXTThemeManagerStyle()
{

}

void CXTThemeManagerStyle::RefreshMetrics()
{

}

BOOL CXTThemeManagerStyle::IsOfficeTheme()
{
	return m_enumTheme != xtThemeDefault;
}

BOOL CXTThemeManagerStyle::TakeSnapShot(CWnd* pWndOwner)
{
	CWnd *pWndParent = pWndOwner->GetParent();
	if (::IsWindow(pWndParent->GetSafeHwnd()))
	{
		if (m_bmpSnapShot.GetSafeHandle() != NULL)
			m_bmpSnapShot.DeleteObject();

		//convert our coordinates to our parent coordinates.
		CXTPWindowRect rc(pWndOwner);
		pWndParent->ScreenToClient(&rc);

		//copy what's on the parents background at this point
		CDC *pDC = pWndParent->GetDC();

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		m_bmpSnapShot.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		CXTPBitmapDC bitmapDC(&memDC, &m_bmpSnapShot);
		memDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);

		pWndParent->ReleaseDC(pDC);

		return TRUE;
	}

	return FALSE;
}

BOOL CXTThemeManagerStyle::DrawTransparentBack(CDC* pDC, CWnd* pWndOwner)
{
	if (::GetWindowLong(pWndOwner->GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_TRANSPARENT)
	{
		// Get background.
		if (!TakeSnapShot(pWndOwner))
			return FALSE;

		CXTPClientRect rc(pWndOwner);

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		CXTPBitmapDC bitmapDC(&memDC, &m_bmpSnapShot);
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// CXTThemeManagerStyleFactory

IMPLEMENT_DYNCREATE(CXTThemeManagerStyleFactory, CObject)

CXTThemeManagerStyleFactory::CXTThemeManagerStyleFactory()
: m_pTheme(NULL)
, m_pNextFactory(NULL)
{
	XTThemeManager()->m_factoryList.AddHead(this);
	m_hostList.Construct(offsetof(CXTThemeManagerStyleHost, m_pNextHost));
}

CXTThemeManagerStyleFactory::~CXTThemeManagerStyleFactory()
{
	SAFE_DELETE(m_pTheme);
	XTThemeManager()->m_factoryList.Remove(this);
	m_hostList.RemoveAll();
}

CXTThemeManagerStyle* CXTThemeManagerStyleFactory::CreateTheme(XTThemeStyle /*theme*/)
{
	ASSERT(FALSE);
	return NULL;
}

void CXTThemeManagerStyleFactory::SetTheme(CXTThemeManagerStyle* pTheme)
{
	ASSERT(pTheme);

	SAFE_DELETE(m_pTheme);
	m_pTheme = pTheme;

	m_pTheme->m_pFactory = this;
	m_pTheme->RefreshMetrics();

	for (CXTThemeManagerStyleHost* pHost = m_hostList.GetHead();
			pHost != NULL; pHost = pHost->m_pNextHost)
	{
		pHost->OnThemeChanged();
	}
}

void CXTThemeManagerStyleFactory::SetTheme(XTThemeStyle theme)
{
	SetTheme(CreateTheme(theme));
}

CXTThemeManagerStyle* CXTThemeManagerStyleFactory::GetSafeTheme()
{
	if (m_pTheme == 0)
	{
		m_pTheme = CreateTheme(XTThemeManager()->m_enumTheme);

		m_pTheme->m_pFactory = this;
		m_pTheme->RefreshMetrics();
	}
	return m_pTheme;
}

void CXTThemeManagerStyleFactory::RefreshMetrics()
{
	if (m_pTheme)
	{
		m_pTheme->RefreshMetrics();
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTThemeManagerStyleHost

CXTThemeManagerStyleHost::CXTThemeManagerStyleHost(CRuntimeClass* pThemeFactoryClass)
{
	m_bAutoDeleteCustomFactory = FALSE;

	m_pDefaultFactory = NULL;
	m_pCustomFactory = NULL;
	m_pNextHost = NULL;
	m_pThemeFactoryClass = NULL;

	if (pThemeFactoryClass)
	{
		InitStyleHost(pThemeFactoryClass);
	}
}

void CXTThemeManagerStyleHost::InitStyleHost(CRuntimeClass* pThemeFactoryClass)
{
	ASSERT(pThemeFactoryClass);
	ASSERT(pThemeFactoryClass->IsDerivedFrom(RUNTIME_CLASS(CXTThemeManagerStyleFactory)));

	if (m_pDefaultFactory)
	{
		m_pDefaultFactory->m_hostList.Remove(this);
	}

	m_pThemeFactoryClass = pThemeFactoryClass;

	m_pDefaultFactory = XTThemeManager()->GetDefaultThemeFactory(pThemeFactoryClass);
	m_pDefaultFactory->m_hostList.AddHead(this);
}

CXTThemeManagerStyleHost::~CXTThemeManagerStyleHost()
{
	GetSafeThemeFactory()->m_hostList.Remove(this);

	if (m_bAutoDeleteCustomFactory)
	{
		SAFE_DELETE(m_pCustomFactory);
	}
}

CXTThemeManagerStyleFactory* CXTThemeManagerStyleHost::CreateThemeFactory()
{
	return (CXTThemeManagerStyleFactory*)m_pThemeFactoryClass->CreateObject();
}

CXTThemeManagerStyleFactory* CXTThemeManagerStyleHost::GetSafeThemeFactory() const
{
	if (m_pCustomFactory != NULL)
		return m_pCustomFactory;

	return m_pDefaultFactory;
}


CXTThemeManagerStyle* CXTThemeManagerStyleHost::GetSafeTheme() const
{
	CXTThemeManagerStyleFactory* pFactory = GetSafeThemeFactory();
	ASSERT(pFactory);
	return pFactory->GetSafeTheme();
}

void CXTThemeManagerStyleHost::SetTheme (XTThemeStyle theme)
{
	SetTheme(m_pDefaultFactory->CreateTheme(theme));
}

void CXTThemeManagerStyleHost::SetTheme (CXTThemeManagerStyle* pTheme)
{
	if (m_pCustomFactory == NULL && pTheme == NULL)
		return;

	GetSafeThemeFactory()->m_hostList.Remove(this);

	if (m_bAutoDeleteCustomFactory)
	{
		SAFE_DELETE(m_pCustomFactory);
	}
	if (pTheme != NULL)
	{
		m_pCustomFactory = CreateThemeFactory();
		m_pCustomFactory->SetTheme(pTheme);
		m_bAutoDeleteCustomFactory = TRUE;
	}
	else
	{
		m_pCustomFactory = NULL;
		m_bAutoDeleteCustomFactory = FALSE;
	}

	OnThemeChanged();
	GetSafeThemeFactory()->m_hostList.AddHead(this);
}

void CXTThemeManagerStyleHost::AttachThemeFactory(CXTThemeManagerStyleFactory* pFactory)
{
	GetSafeThemeFactory()->m_hostList.Remove(this);

	if (m_bAutoDeleteCustomFactory)
	{
		SAFE_DELETE(m_pCustomFactory);
	}
	m_pCustomFactory = pFactory;
	m_bAutoDeleteCustomFactory = FALSE;

	OnThemeChanged();
	GetSafeThemeFactory()->m_hostList.AddHead(this);
}

void CXTThemeManagerStyleHost::OnThemeChanged()
{
	// Can be overridden by subclass
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTThemeManager::CXTThemeManager()
{
	m_factoryList.Construct(offsetof(CXTThemeManagerStyleFactory, m_pNextFactory));
	m_enumTheme = xtThemeDefault;
}

CXTThemeManager::~CXTThemeManager()
{
	POSITION pos = m_mapDefaultFactories.GetStartPosition();
	void* pThemeFactoryClass;
	CXTThemeManagerStyleFactory* pFactory = 0;
	while (pos)
	{
		m_mapDefaultFactories.GetNextAssoc(pos, pThemeFactoryClass, (void *&)pFactory);
		SAFE_DELETE(pFactory);
	}

	ASSERT(m_factoryList.m_pHead == 0);
	m_factoryList.RemoveAll();
}

CXTThemeManagerStyleFactory* CXTThemeManager::GetDefaultThemeFactory(CRuntimeClass* pThemeFactoryClass)
{
	ASSERT(pThemeFactoryClass);
	ASSERT(pThemeFactoryClass->IsDerivedFrom(RUNTIME_CLASS(CXTThemeManagerStyleFactory)));

	CXTThemeManagerStyleFactory* pFactory = 0;

	if (!m_mapDefaultFactories.Lookup(pThemeFactoryClass, (void *&)pFactory))
	{
		pFactory = (CXTThemeManagerStyleFactory*)pThemeFactoryClass->CreateObject();
		m_mapDefaultFactories.SetAt(pThemeFactoryClass, pFactory);
	}

	return pFactory;
}

void CXTThemeManager::SetTheme(XTThemeStyle eTheme)
{
	if (m_enumTheme == eTheme)
		return;

	m_enumTheme = eTheme;

	RefreshXtremeColors();

	POSITION pos = m_mapDefaultFactories.GetStartPosition();
	void* pThemeFactoryClass;
	CXTThemeManagerStyleFactory* pFactory = 0;
	while (pos)
	{
		m_mapDefaultFactories.GetNextAssoc(pos, pThemeFactoryClass, (void *&)pFactory);

		pFactory->SetTheme(eTheme);
	}
}

void CXTThemeManager::RefreshMetrics()
{
	RefreshXtremeColors();

	for (CXTThemeManagerStyleFactory* pFactory = m_factoryList.GetHead();
			pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		pFactory->RefreshMetrics();
	}
}

_XTP_EXT_CLASS CXTThemeManager* AFX_CDECL XTThemeManager()
{
	return CXTThemeManager::m_sThemeManager.GetData();
}
