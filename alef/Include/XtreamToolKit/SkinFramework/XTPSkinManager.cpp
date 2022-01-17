// XTPSkinManager.cpp: implementation of the CXTPSkinManager class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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

#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPSystemHelpers.h"

#include "XTPSkinManager.h"
#include "XTPSkinImage.h"
#include "XTPSkinManagerResource.h"
#include "XTPSkinObject.h"
#include "XTPSkinObjectFrame.h"
#include "XTPSkinManagerSchema.h"
#include "XTPSkinManagerApiHook.h"
#include "XTPSkinManagerModuleList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EVENT_OBJECT_REORDER 0x8004

CXTPSkinManagerMetrics::CXTPSkinManagerMetrics(CXTPSkinManager* pManager)
{
	m_pManager = pManager;

	m_brushDialog = NULL;
	m_brushTabControl = NULL;

	ZeroMemory(m_brTheme, sizeof(m_brTheme));
	memset(m_clrTheme, -1, sizeof(m_clrTheme));
}

CXTPSkinManagerMetrics::~CXTPSkinManagerMetrics()
{
	if (m_brushDialog)
	{
		DeleteObject(m_brushDialog);
	}
	if (m_brushTabControl)
	{
		DeleteObject(m_brushTabControl);
	}

	for (int iColorId = 0; iColorId < XTP_SKINMETRICS_COLORTABLESIZE; iColorId++)
	{
		if (m_brTheme[iColorId])
			DeleteObject(m_brTheme[iColorId]);
	}
}

BOOL CXTPSkinManagerMetrics::IsMetricObject(HGDIOBJ hObject) const
{
	if (hObject == 0)
		return FALSE;

	if (GetObjectType(hObject) != OBJ_BRUSH)
		return FALSE;

	for (int i = 0; i < XTP_SKINMETRICS_COLORTABLESIZE; i++)
	{
		if (hObject == m_brTheme[i])
			return TRUE;
	}
	return FALSE;
}

void CXTPSkinManagerMetrics::DestroyMetrics()
{
	m_fntCaption.DeleteObject();
	m_fntSmCaption.DeleteObject();
	m_fntMenu.DeleteObject();

	for (int iColorId = 0; iColorId < XTP_SKINMETRICS_COLORTABLESIZE; iColorId++)
	{
		HBRUSH hBrush = m_brTheme[iColorId];
		m_brTheme[iColorId] = NULL;

		if (hBrush)
			DeleteObject(hBrush);
	}

	if (m_brushDialog)
	{
		DeleteObject(m_brushDialog);
		m_brushDialog = NULL;
	}

	if (m_brushTabControl)
	{
		DeleteObject(m_brushTabControl);
		m_brushTabControl = NULL;
	}

}

void CXTPSkinManagerMetrics::RefreshMetrics()
{
	static const COLORREF defColors[] =
	{
		RGB(192, 192, 192),
		RGB( 58, 110, 165),
		RGB(  0,   0, 128),
		RGB(128, 128, 128),
		RGB(192, 192, 192),
		RGB(255, 255, 255),
		RGB(  0,   0,   0),
		RGB(  0,   0,   0),
		RGB(  0,   0,   0),
		RGB(255, 255, 255),
		RGB(192, 192, 192),
		RGB(192, 192, 192),
		RGB(128, 128, 128),
		RGB(  0,   0, 128),
		RGB(255, 255, 255),
		RGB(192, 192, 192),
		RGB(128, 128, 128),
		RGB(128, 128, 128),
		RGB(  0,   0,   0),
		RGB(192, 192, 192),
		RGB(255, 255, 255),
		RGB(  0,   0,   0),
		RGB(223, 223, 223),
		RGB(  0,   0,   0),
		RGB(255, 255, 225),
		RGB(180, 180, 180),
		RGB(  0,   0, 255),
		RGB( 16, 132, 208),
		RGB(181, 181, 181)
	};
	CXTPSkinManagerSchema* pSchema = m_pManager->GetSchema();
	if (!pSchema)
	{
		for (int iColorId = 0; iColorId < XTP_SKINMETRICS_COLORTABLESIZE; iColorId++)
		{
			m_clrTheme[iColorId] = GetSysColor(iColorId);
		}
		return;
	}
	UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));

	m_fntCaption.DeleteObject();
	m_fntCaption.CreateFontIndirect(&ncm.lfCaptionFont);

	m_fntSmCaption.DeleteObject();
	m_fntSmCaption.CreateFontIndirect(&ncm.lfSmCaptionFont);

	m_fntMenu.DeleteObject();
	m_fntMenu.CreateFontIndirect(&ncm.lfMenuFont);

	m_nBorderSize = 1;
	SystemParametersInfo(SPI_GETBORDER, 0, &m_nBorderSize, FALSE);

	m_cxBorder = GetSystemMetrics(SM_CXBORDER);
	m_cyBorder = GetSystemMetrics(SM_CYBORDER);
	m_cxEdge = GetSystemMetrics(SM_CXEDGE);
	m_cyEdge = GetSystemMetrics(SM_CYEDGE);

	m_cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
	m_cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	m_cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	m_cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	BOOL bApplyMetrics = m_pManager->GetApplyOptions() & xtpSkinApplyMetrics;

	m_cyOsCaption = m_cyCaption = GetSystemMetrics(SM_CYCAPTION);
	m_cyOsSmallCaption = m_cySmallCaption = GetSystemMetrics(SM_CYSMCAPTION);

	if (bApplyMetrics)
	{
		pSchema->GetIntProperty(nSysMetrics, 0, 0, TMT_CAPTIONBARHEIGHT, m_cyCaption);
		m_cyCaption++;

		pSchema->GetIntProperty(nSysMetrics, 0, 0, TMT_SMCAPTIONBARHEIGHT, m_cySmallCaption);
		m_cySmallCaption++;

		int nScrollbarWidth = 0, nScrollbarHeight = 0;
		if (SUCCEEDED(pSchema->GetIntProperty(nSysMetrics, 0, 0, TMT_SCROLLBARWIDTH, nScrollbarWidth)) && nScrollbarWidth > 0)
		{
			m_cxHScroll = m_cxVScroll = nScrollbarWidth;
		}
		if (SUCCEEDED(pSchema->GetIntProperty(nSysMetrics, 0, 0, TMT_SCROLLBARHEIGHT, nScrollbarHeight)) && nScrollbarHeight > 0)
		{
			m_cyHScroll = m_cyVScroll = nScrollbarHeight;
		}

		LOGFONT lfCaption, lfSmCaption;
		if (SUCCEEDED(pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_CAPTIONFONT, lfCaption)))
		{
			m_fntCaption.DeleteObject();
			m_fntCaption.CreateFontIndirect(&lfCaption);
		}

		if (SUCCEEDED(pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_SMALLCAPTIONFONT, lfSmCaption)))
		{
			m_fntSmCaption.DeleteObject();
			m_fntSmCaption.CreateFontIndirect(&lfSmCaption);
		}
	}

	for (int iColorId = 0; iColorId < XTP_SKINMETRICS_COLORTABLESIZE; iColorId++)
	{
		COLORREF clrOld = m_clrTheme[iColorId];
		m_clrTheme[iColorId] = (COLORREF)-1;

		COLORREF clrVal;

		if (FAILED(pSchema->GetColorProperty(nSysMetrics, 0, 0, iColorId + TMT_FIRSTCOLOR, clrVal)))
		{
			if (iColorId < _countof(defColors))
				clrVal = defColors[iColorId];
			else
				clrVal = GetSysColor(iColorId);

			m_pManager->ApplyColorFilter(clrVal);
		}

		m_clrTheme[iColorId] = clrVal;

		if ((clrOld != clrVal) || (m_brTheme[iColorId] == NULL))
		{
			HBRUSH hBrush = m_brTheme[iColorId];
			m_brTheme[iColorId] = NULL;

			if (hBrush)
				DeleteObject(hBrush);

			m_brTheme[iColorId] = CreateSolidBrush(clrVal);
		}
	}

	m_clrEdgeHighLight = m_clrTheme[COLOR_BTNHIGHLIGHT];
	m_clrEdgeShadow = m_clrTheme[COLOR_BTNSHADOW];
	m_clrEdgeLight = m_clrTheme[COLOR_3DLIGHT];
	m_clrEdgeDkShadow = m_clrTheme[COLOR_3DDKSHADOW];


	if (m_brushDialog)
	{
		DeleteObject(m_brushDialog);
	}
	m_brushDialog = ::CreateSolidBrush(m_clrTheme[COLOR_3DFACE]);

	if (m_brushTabControl)
	{
		DeleteObject(m_brushTabControl);
		m_brushTabControl = NULL;
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManager

class XTP_SKINAMANGER_THREAD_STATE : public CNoTrackObject
{
public:
	XTP_SKINAMANGER_THREAD_STATE();
	virtual ~XTP_SKINAMANGER_THREAD_STATE();

public:
	HHOOK m_hHookOldCbtFilter;
};

XTP_SKINAMANGER_THREAD_STATE::XTP_SKINAMANGER_THREAD_STATE()
{
	m_hHookOldCbtFilter = 0;
}

XTP_SKINAMANGER_THREAD_STATE::~XTP_SKINAMANGER_THREAD_STATE()
{
	if (m_hHookOldCbtFilter)
	{
		UnhookWindowsHookEx(m_hHookOldCbtFilter);
	}
}



THREAD_LOCAL(XTP_SKINAMANGER_THREAD_STATE, _xtpSkinManagerThreadState)


CXTPSkinManager::CXTPSkinManager()
{
	m_pSchema = NULL;
	m_pMetrics = new CXTPSkinManagerMetrics(this);
	m_bEnabled = FALSE;

	m_pResourceFile = new CXTPSkinManagerResourceFile();
	m_pResourceFile->m_pManager = this;

	m_pClassMap = new CXTPSkinObjectClassMap();
	m_pClassMap->AddStandardClasses();

	m_mapObjects.InitHashTable(199, FALSE);

	m_bAutoApplyWindows = TRUE;
	m_bAutoApplyThreads = TRUE;
	m_dwApplyOptions = xtpSkinApplyFrame | xtpSkinApplyColors | xtpSkinApplyMetrics;

	m_dwComCtrl = XTPSystemVersion()->GetComCtlVersion();

	m_bWin9x = XTPSystemVersion()->IsWin9x();

	m_hWinEventHook = NULL;
	m_pSetWinEventHook = NULL;
	m_pUnhookWinEvent = NULL;

	CXTPSkinManagerApiHook* pApiHook = CXTPSkinManagerApiHook::GetInstance();
	pApiHook;

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pSetWinEventHook = (LPFNSETWINEVENTHOOK)GetProcAddress(hLib, "SetWinEventHook");
		m_pUnhookWinEvent = (LPFNUNHOOKWINEVENT)GetProcAddress(hLib, "UnhookWinEvent");

		if (m_pSetWinEventHook && m_pUnhookWinEvent)
		{
			m_hWinEventHook = m_pSetWinEventHook(EVENT_OBJECT_REORDER, EVENT_OBJECT_REORDER, NULL,
				&CXTPSkinManager::WinEventProc, GetCurrentProcessId(), 0, 0);
		}
	}
	EnableCurrentThread();
}

void CXTPSkinManager::EnableCurrentThread()
{
	DWORD dwThread = ::GetCurrentThreadId();

	XTP_SKINAMANGER_THREAD_STATE* pThreadState = _xtpSkinManagerThreadState.GetData();
	if (pThreadState)
	{
		if (pThreadState->m_hHookOldCbtFilter == 0)
		{
			pThreadState->m_hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT, CbtFilterHook, NULL, dwThread);
		}
	}
}


CXTPSkinManager::~CXTPSkinManager()
{
	FreeSkinData();
	RemoveColorFilters();

	POSITION pos = m_mapClasses.GetStartPosition();
	CString strClassName;
	CXTPSkinManagerClass* pClass;
	while (pos != NULL)
	{
		m_mapClasses.GetNextAssoc( pos, strClassName, (void*&)pClass);
		delete pClass;
	}
	m_mapClasses.RemoveAll();

	SAFE_DELETE(m_pSchema);
	SAFE_DELETE(m_pMetrics);
	SAFE_DELETE(m_pResourceFile);
	SAFE_DELETE(m_pClassMap);

	RemoveAll();

	CXTPSkinManagerApiHook* pApiHook = CXTPSkinManagerApiHook::GetInstance();
	pApiHook->FinalizeHookManagement();

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (m_hWinEventHook && hLib && m_pUnhookWinEvent)
	{
		m_pUnhookWinEvent(m_hWinEventHook);
	}
}

BOOL CXTPSkinManager::IsColorFilterExists() const
{
	return m_arrFilters.GetSize() > 0;
}

void CXTPSkinManager::RemoveColorFilters()
{
	for (int i = 0; i < (int)m_arrFilters.GetSize(); i++)
	{
		delete m_arrFilters[i];
	}
	m_arrFilters.RemoveAll();
}

void CXTPSkinManager::AddColorFilter(CXTPSkinManagerColorFilter* pFilter)
{
	m_arrFilters.Add(pFilter);
}

void CXTPSkinManager::RedrawAllControls()
{
	if (!m_bEnabled)
		return;

	POSITION pos = m_mapClasses.GetStartPosition();
	CString strClassName;
	CXTPSkinManagerClass* pClass;
	while (pos != NULL)
	{
		m_mapClasses.GetNextAssoc( pos, strClassName, (void*&)pClass);
		pClass->GetImages()->RemoveAll();
	}

	m_pMetrics->RefreshMetrics();
	XTPColorManager()->RefreshColors();

	OnSkinChanged(TRUE, TRUE);
}

void CXTPSkinManager::ApplyColorFilter(COLORREF& clr)
{
	for (int i = 0; i < (int)m_arrFilters.GetSize(); i++)
	{
		m_arrFilters[i]->ApplyColorFilter(clr);
	}
}

BOOL CXTPSkinManager::LoadSkin(LPCTSTR lpszResourcePath, LPCTSTR lpszIniFileName)
{
	SAFE_DELETE(m_pSchema);

	CXTPSkinManagerApiHook* pApiHook = CXTPSkinManagerApiHook::GetInstance();
	pApiHook->FinalizeHookManagement();

	BOOL bEnabled = m_bEnabled;

	if (XTPColorManager()->IsLowResolution() ||
		!CXTPSkinManagerModuleList::IsEnumeratorExists() ||
		!ReadSkinData(lpszResourcePath, lpszIniFileName))
	{
		m_bEnabled = FALSE;

		SAFE_DELETE(m_pSchema);

		m_pMetrics->RefreshMetrics();
		XTPColorManager()->RefreshColors();

		OnSkinChanged(bEnabled, m_bEnabled);

		return FALSE;
	}

	m_bEnabled = TRUE;

	pApiHook->InitializeHookManagement();

	m_pMetrics->RefreshMetrics();
	m_pSchema->RefreshMetrcis();
	XTPColorManager()->RefreshColors();

	OnSkinChanged(bEnabled, m_bEnabled);

	return TRUE;
}

CXTPSkinManagerClass* CXTPSkinManager::FromHandle(HTHEME hTheme)
{
	POSITION pos = m_mapClasses.GetStartPosition();
	CString strClassName;
	CXTPSkinManagerClass* pClass;

	while (pos != NULL)
	{
		m_mapClasses.GetNextAssoc( pos, strClassName, (void*&)pClass);
		if (pClass == (CXTPSkinManagerClass*)hTheme)
			return pClass;
	}

	return NULL;
}


void CXTPSkinManager::SetApplyOptions(DWORD dwOptions)
{
	m_dwApplyOptions = dwOptions;

	if (!m_bEnabled)
		return;

	CXTPSkinManagerApiHook* pApiHook = CXTPSkinManagerApiHook::GetInstance();

	pApiHook->FinalizeHookManagement();
	pApiHook->InitializeHookManagement();

	m_pMetrics->RefreshMetrics();
	m_pSchema->RefreshMetrcis();
	XTPColorManager()->RefreshColors();

	OnSkinChanged(m_bEnabled, m_bEnabled);
}


void CXTPSkinManager::SetResourceFile(CXTPSkinManagerResourceFile* pResourceFile)
{
	ASSERT(pResourceFile);

	SAFE_DELETE(m_pResourceFile);

	m_pResourceFile = pResourceFile;
	m_pResourceFile->m_pManager = this;
}

void CXTPSkinManager::FreeSkinData()
{

	POSITION pos = m_mapClasses.GetStartPosition();
	CString strClassName;
	CXTPSkinManagerClass* pClass;
	while (pos != NULL)
	{
		m_mapClasses.GetNextAssoc( pos, strClassName, (void*&)pClass);
		delete pClass;
	}
	m_mapClasses.RemoveAll();

	m_pResourceFile->Close();
}

BOOL CXTPSkinManager::ReadSkinData(LPCTSTR strResourcePath, LPCTSTR strIniFileName)
{
	FreeSkinData();

	if (!m_pResourceFile->Open(strResourcePath, strIniFileName))
		return FALSE;

	m_pSchema = m_pResourceFile->CreateSchema();
	if (!m_pSchema)
		return FALSE;

	if (FAILED(m_pSchema->ReadProperties(m_pResourceFile)))
		return FALSE;

	return TRUE;
}

CXTPSkinManager* CXTPSkinManager::s_pInstance = NULL;

static class CXTPSkinManager::CDestructor
{
public:
	~CDestructor()
	{
		if (s_pInstance != NULL)
			delete s_pInstance;
	}

} __xtpSkinManagerDestructor;

LRESULT CALLBACK CXTPSkinManager::CbtFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
	XTP_SKINAMANGER_THREAD_STATE* pThreadState = _xtpSkinManagerThreadState.GetData();

	LRESULT lResult = CallNextHookEx(pThreadState->m_hHookOldCbtFilter, code,
			wParam, lParam);

	if (code == HCBT_CREATEWND && XTPSkinManager()->m_bAutoApplyWindows)
	{
		ASSERT(lParam != NULL);
		LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
		ASSERT(lpcs != NULL);

		ASSERT(wParam != NULL); // should be non-NULL HWND
		HWND hWnd = (HWND)wParam;

		TCHAR lpszClassName[50];
		GetClassName(hWnd, lpszClassName, 50);

		XTPSkinManager()->SetHook(hWnd, lpszClassName, lpcs, TRUE);
	}

	return lResult;
}

_XTP_EXT_CLASS CXTPSkinManager* AFX_CDECL XTPSkinManager()
{
	if (CXTPSkinManager::s_pInstance == NULL)
		CXTPSkinManager::s_pInstance = new CXTPSkinManager();

	return CXTPSkinManager::s_pInstance;
}

void CXTPSkinManager::SetSkinManager(CXTPSkinManager* pSkinManager)
{
	if (s_pInstance != NULL)
		delete s_pInstance;

	s_pInstance = pSkinManager;
}



CXTPSkinManagerClass* CXTPSkinManager::GetSkinClass(CString strClassList)
{
	strClassList.MakeUpper();

	CXTPSkinManagerClass* pClass;

	if (m_mapClasses.Lookup(strClassList, (void *&)pClass))
		return pClass;

	pClass = new CXTPSkinManagerClass(this);
	pClass->m_strClass = strClassList;
	pClass->m_nClassCode =  m_pSchema->GetClassCode(strClassList);

	m_mapClasses.SetAt(strClassList, pClass);

	return pClass;
}

void CXTPSkinManager::GetCurrentThemeName(CString& strThemeFileName, CString& strColorBuff)
{
	if (m_pResourceFile)
	{
		strThemeFileName = m_pResourceFile->GetResourcePath();
		strColorBuff = m_pResourceFile->GetIniFileName();
	}
}


int CXTPSkinManager::GetThemeSysSize(int iSizeId)
{
	int iVal;
	VERIFY(SUCCEEDED(m_pSchema->GetIntProperty(m_pSchema->GetClassCode(_T("SYSMETRICS")), 0, 0, iSizeId, iVal)));
	return iVal;
}

BOOL CXTPSkinManager::GetThemeSysBool(int iBoolId)
{
	BOOL bVal;
	VERIFY(SUCCEEDED(m_pSchema->GetBoolProperty(m_pSchema->GetClassCode(_T("SYSMETRICS")), 0, 0, iBoolId, bVal)));
	return bVal;
}


COLORREF CXTPSkinManager::GetThemeSysColor(int iColorId)
{
	return iColorId >= TMT_FIRSTCOLOR && iColorId <= TMT_LASTCOLOR ?
		m_pMetrics->m_clrTheme[iColorId - TMT_FIRSTCOLOR] : (COLORREF)-1;
}

HRESULT CXTPSkinManager::GetThemeSysFont(int iFontId, LOGFONT *plf)
{
	VERIFY(SUCCEEDED(m_pSchema->GetFontProperty(m_pSchema->GetClassCode(_T("SYSMETRICS")), 0, 0, iFontId, *plf)));
	return S_OK;

}

HRESULT CXTPSkinManager::EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags)
{
	CXTPSkinObjectFrame* pObject = (CXTPSkinObjectFrame*)Lookup(hWnd);

	if (!pObject)
		return E_INVALIDARG;

	pObject->m_dwDialogTexture = dwFlags;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

CXTPSkinManagerClass::CXTPSkinManagerClass(CXTPSkinManager* pManager)
{
	m_pImages = new CXTPSkinImages();
	m_pManager = pManager;

	m_mapCachedProperties.InitHashTable(50);
}

CXTPSkinManagerClass::~CXTPSkinManagerClass()
{
	delete m_pImages;
}



BOOL CXTPSkinManagerClass::DrawThemeText(CDC* pDC, int iPartId, int iStateId, const CString& strText, DWORD dwFormat, const RECT *pRect)
{
	pDC->SetBkMode(TRANSPARENT);


	CRect rcCaptionMargins = GetThemeRect(iPartId, iStateId, TMT_CAPTIONMARGINS);
	rcCaptionMargins.top = rcCaptionMargins.bottom = 0;

	CRect rc(pRect);
	rc.DeflateRect(rcCaptionMargins);

	if (GetThemeEnumValue(iPartId, iStateId, TMT_CONTENTALIGNMENT) == CA_CENTER)
	{
		dwFormat |=  DT_CENTER;

	}

	pDC->SetTextColor(GetThemeColor(iPartId, iStateId, TMT_TEXTCOLOR, pDC->GetTextColor()));


	if (GetThemeEnumValue(iPartId, iStateId, TMT_TEXTSHADOWTYPE) == TST_SINGLE)
	{
		CSize sz = GetThemeSize(iPartId, iStateId, TMT_TEXTSHADOWOFFSET);

		if (sz != CSize(0, 0))
		{

			COLORREF clrShadow = GetThemeColor(iPartId, iStateId, TMT_TEXTSHADOWCOLOR);
			CRect rcShadow(rc);
			rcShadow.OffsetRect(sz);

			COLORREF clr = pDC->SetTextColor(clrShadow);
			pDC->DrawText(strText, rcShadow, dwFormat);
			pDC->SetTextColor(clr);
		}
	}

	pDC->DrawText(strText, rc, dwFormat);



	return TRUE;
}

CXTPSkinManagerSchemaProperty* CXTPSkinManagerClass::GetProperty(XTPSkinManagerProperty propType, int iPartId, int iStateId, int iPropId)
{
	UINT nCachedProp = iPropId + ((iPartId + (iStateId << 6 )) << 14);
	CXTPSkinManagerSchemaProperty* pProperty = NULL;

	if (m_mapCachedProperties.Lookup(nCachedProp, pProperty))
		return pProperty;

	pProperty = m_pManager->GetSchema()->GetProperty(m_nClassCode, iPartId, iStateId, iPropId);
	if (!pProperty)
		return 0;

	if (pProperty->propType != propType)
		return 0;

	m_mapCachedProperties.SetAt(nCachedProp , pProperty);

	return pProperty;
}

CString CXTPSkinManagerClass::GetThemeString(int iPartId, int iStateId, int iPropId, LPCTSTR lpszDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_STRING, iPartId, iStateId, iPropId);

	if (!pProperty)
		return lpszDefault;

	return pProperty->lpszVal;
}

CRect CXTPSkinManagerClass::GetThemeRect(int iPartId, int iStateId, int iPropId, CRect rcDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_RECT, iPartId, iStateId, iPropId);

	if (!pProperty)
		return rcDefault;

	return pProperty->rcVal;
}

int CXTPSkinManagerClass::GetThemeInt(int iPartId, int iStateId, int iPropId, int nDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_INT, iPartId, iStateId, iPropId);

	if (!pProperty)
		return nDefault;

	return pProperty->iVal;
}
BOOL CXTPSkinManagerClass::GetThemeBool(int iPartId, int iStateId, int iPropId, BOOL bDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_BOOL, iPartId, iStateId, iPropId);

	if (!pProperty)
		return bDefault;

	return pProperty->bVal;
}
COLORREF CXTPSkinManagerClass::GetThemeColor(int iPartId, int iStateId, int iPropId, COLORREF clrDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_COLOR, iPartId, iStateId, iPropId);

	if (!pProperty)
		return clrDefault;

	COLORREF clrVal = pProperty->clrVal;
	m_pManager->ApplyColorFilter(clrVal);

	return clrVal;
}

int CXTPSkinManagerClass::GetThemeEnumValue(int iPartId, int iStateId, int iPropId, int nDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_ENUM, iPartId, iStateId, iPropId);

	if (!pProperty)
		return nDefault;

	return pProperty->iVal;
}

CSize CXTPSkinManagerClass::GetThemeSize(int iPartId, int iStateId, int iPropId, CSize szDefault)
{
	CXTPSkinManagerSchemaProperty* pProperty = GetProperty(XTP_SKINPROPERTY_POSITION, iPartId, iStateId, iPropId);

	if (!pProperty)
		return szDefault;

	return pProperty->szVal;
}


BOOL CXTPSkinManagerClass::DrawThemeBackground(CDC* pDC, int iPartId, int iStateId, const RECT *pRect)
{
	return m_pManager->GetSchema()->DrawThemeBackground(pDC, this, iPartId, iStateId, pRect);
}

BOOL CXTPSkinManagerClass::GetThemePartSize(int iPartId, int iStateId, RECT *pRect, THEMESIZE eSize, SIZE* pSize)
{
	if (GetThemeEnumValue(iPartId, iStateId, TMT_BGTYPE) != BT_IMAGEFILE)
		return FALSE;

	int nImageFile = (GetThemeEnumValue(iPartId, iStateId, TMT_IMAGESELECTTYPE) != IST_NONE) &&
		(GetThemeEnumValue(iPartId, iStateId, TMT_GLYPHTYPE, GT_NONE) == GT_NONE) ?
		TMT_IMAGEFILE1: TMT_IMAGEFILE;

	CString strImageFile = GetThemeString(iPartId, iStateId, nImageFile);
	if (strImageFile.IsEmpty())
	{
		if (nImageFile != TMT_IMAGEFILE1)
			return FALSE;

		strImageFile = GetThemeString(iPartId, iStateId, TMT_IMAGEFILE);
		if (strImageFile.IsEmpty())
			return FALSE;

	}


	CXTPSkinImage* pImage = GetImages()->LoadFile(m_pManager->GetResourceFile(), strImageFile);
	if (!pImage)
	{
		return FALSE;
	}

	int nImageCount = GetThemeInt(iPartId, iStateId, TMT_IMAGECOUNT, 1);
	if (nImageCount < 1)
		nImageCount = 1;

	BOOL bHorizontalImageLayout = GetThemeEnumValue(iPartId, iStateId, TMT_IMAGELAYOUT, IL_HORIZONTAL) == IL_HORIZONTAL;

	CSize sz(pImage->GetWidth(), pImage->GetHeight());
	if (bHorizontalImageLayout) sz.cx /= nImageCount; else sz.cy /= nImageCount;

	if (eSize == TS_TRUE)
	{
		*pSize = sz;
	}
	if (eSize == TS_DRAW)
	{
		if (GetThemeEnumValue(iPartId, iStateId, TMT_SIZINGTYPE, ST_STRETCH) == ST_TRUESIZE)
		{
			*pSize = sz;
			return TRUE;
		}

		if (!pRect)
		{
			pSize->cy = 0;
			pSize->cx = 0;
		}
		else
		{
			pSize->cy = pRect->bottom - pRect->top;
			pSize->cx = pRect->right - pRect->left;
		}

		if (GetThemeBool(iPartId, iStateId, TMT_UNIFORMSIZING, FALSE))
		{
			pSize->cx = MulDiv(pSize->cy, sz.cx, sz.cy);
		}
	}

	return TRUE;
}
