// XTPDockState.cpp : implementation of the CXTPDockState class.
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

#include "Common/XTPResourceManager.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPPropExchange.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"

#ifdef _XTP_INCLUDE_GRAPHICLIBRARY
#include "GraphicLibrary/zlib/zlib.h"
#endif

#include "XTPShortcutManager.h"
#include "XTPDockState.h"
#include "XTPCommandBars.h"
#include "XTPDockBar.h"
#include "XTPDockContext.h"
#include "XTPControls.h"
#include "XTPControl.h"
#include "XTPControlPopup.h"
#include "XTPControlButton.h"
#include "XTPControlComboBox.h"
#include "XTPControlEdit.h"
#include "XTPPopupbar.h"
#include "XTPMenuBar.h"
#include "XTPToolBar.h"
#include "XTPDialogBar.h"
#include "XTPPaintManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR _xtpVisible[] = _T("Visible");
const TCHAR _xtpBarSection[] = _T("%s-Bar%d");
const TCHAR _xtpDockBarSection[] = _T("%s-DockBar%d");
const TCHAR _xtpSummarySection[] = _T("%s-Summary");
const TCHAR _xtpXPos[] = _T("XPos");
const TCHAR _xtpYPos[] = _T("YPos");
const TCHAR _xtpMRUWidth[] = _T("MRUWidth");
const TCHAR _xtpMRUDockID[] = _T("MRUDockID");
const TCHAR _xtpMRUDockLeftPos[] = _T("MRUDockLeftPos");
const TCHAR _xtpMRUDockRightPos[] = _T("MRUDockRightPos");
const TCHAR _xtpMRUDockTopPos[] = _T("MRUDockTopPos");
const TCHAR _xtpMRUDockBottomPos[] = _T("MRUDockBottomPos");
const TCHAR _xtpMRUFloatXPos[] = _T("MRUFloatXPos");
const TCHAR _xtpMRUFloatYPos[] = _T("MRUFloatYPos");

const TCHAR _xtpBarID[] = _T("BarID");
const TCHAR _xtpFloating[] = _T("Floating");
const TCHAR _xtpBars[] = _T("Bars");
const TCHAR _xtpScreenCX[] = _T("ScreenCX");
const TCHAR _xtpScreenCY[] = _T("ScreenCY");
const TCHAR _xtpBar[] = _T("Bar#%d");

const TCHAR _xtpCommandBarControlsSection[] = _T("%s-Controls");
const TCHAR _xtpCommandBarControls[] = _T("Controls");
const TCHAR _xtpCommandBarLoadFromFile[] = _T("LoadFromFile");
const TCHAR _xtpDialogBar[] = _T("DialogBar");


class CXTPToolBar::CToolBarInfo
{
public:
	CToolBarInfo();
	~CToolBarInfo();

	BOOL SaveState(LPCTSTR lpszProfileName, int nIndex);
	BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, CXTPDockState* pDockState);

	void DoPropExchange(CXTPPropExchange* pPX, CXTPDockState* pDockState);


// Attributes
	UINT m_nBarID;      // ID of this bar
	BOOL m_bVisible;    // visibility of this bar
	BOOL m_bFloating;   // whether floating or not
	CPoint m_pointPos;  // topleft point of window
	UINT m_nMRUWidth;   // MRUWidth for Dynamic Toolbars
	XTPBarPosition m_uMRUDockPosition;  // most recent docked dockbar
	CRect m_rectMRUDockPos; // most recent docked position
	DWORD m_dwMRUFloatStyle; // most recent floating orientation
	CPoint m_ptMRUFloatPos; // most recent floating position
	CXTPToolBar* m_pBar;    // bar which this refers to (transient)

	BOOL m_bDialogBar;
	CSize m_szDialogBarDockingClient;
	CSize m_szDialogBarFloatingClient;
};

CXTPToolBar::CToolBarInfo::CToolBarInfo()
{
	m_nBarID = 0;
	m_bVisible = m_bFloating = FALSE;
	m_pBar = NULL;
	m_pointPos.x = m_pointPos.y = -1;
	m_nMRUWidth = 32767;

	m_uMRUDockPosition = xtpBarTop;
	m_rectMRUDockPos.SetRectEmpty();
	m_dwMRUFloatStyle = 0;
	m_ptMRUFloatPos = CPoint(0, 0);

	m_bDialogBar = FALSE;
	m_szDialogBarFloatingClient = m_szDialogBarDockingClient = CSize(200, 200);
}

CXTPToolBar::CToolBarInfo::~CToolBarInfo()
{
}

void CXTPToolBar::CToolBarInfo::DoPropExchange(CXTPPropExchange* pPX, CXTPDockState* pDockState)
{
	ASSERT(pDockState != NULL);
	if (!pDockState)
		return;

	PX_Long(pPX, _T("BarId"), (long&)m_nBarID, 0);
	PX_Bool(pPX, _T("Visible"), m_bVisible, TRUE);
	PX_Bool(pPX, _T("Floating"), m_bFloating, FALSE);
	PX_Long(pPX, _T("MRUWidth"), (long&)m_nMRUWidth);
	PX_Enum(pPX, _T("MRUDockPosition"), m_uMRUDockPosition, xtpBarTop);
	PX_Point(pPX, _T("PointPos"), m_pointPos, CPoint(0, 0));
	PX_Rect(pPX, _T("MRUDockPos"), m_rectMRUDockPos, CRect(0, 0, 0, 0));
	PX_Point(pPX, _T("MRUFloatPos"), m_ptMRUFloatPos, CPoint(0, 0));

	if (pDockState->GetVersion() > _XTP_SCHEMA_87)
	{
		PX_Bool(pPX, _T("DialogBar"), m_bDialogBar, FALSE);

		if (m_bDialogBar)
		{
			PX_Size(pPX, _T("DialogBarDockingClientSize"), m_szDialogBarDockingClient, CSize(0, 0));
			PX_Size(pPX, _T("DialogBarFloatingClientSize"), m_szDialogBarFloatingClient, CSize(0, 0));
		}
	}
}

BOOL CXTPToolBar::CToolBarInfo::SaveState(LPCTSTR lpszProfileName, int nIndex)
{
	TCHAR szSection[256];
	wsprintf(szSection, _xtpBarSection, lpszProfileName, nIndex);

	// delete the section
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(szSection, NULL, NULL);

	pApp->WriteProfileInt(szSection, _xtpBarID, m_nBarID);
	if (!m_bVisible)
		pApp->WriteProfileInt(szSection, _xtpVisible, m_bVisible);
	if (m_bFloating)
	{
		pApp->WriteProfileInt(szSection, _xtpFloating, m_bFloating);
	}
	if (m_pointPos.x != -1)
		pApp->WriteProfileInt(szSection, _xtpXPos, m_pointPos.x);
	if (m_pointPos.y != -1)
		pApp->WriteProfileInt(szSection, _xtpYPos, m_pointPos.y);
	if (m_nMRUWidth != 32767)
		pApp->WriteProfileInt(szSection, _xtpMRUWidth, m_nMRUWidth);

	pApp->WriteProfileInt(szSection, _xtpMRUDockID, m_uMRUDockPosition);
	pApp->WriteProfileInt(szSection, _xtpMRUDockLeftPos, m_rectMRUDockPos.left);
	pApp->WriteProfileInt(szSection, _xtpMRUDockTopPos, m_rectMRUDockPos.top);
	pApp->WriteProfileInt(szSection, _xtpMRUDockRightPos, m_rectMRUDockPos.right);
	pApp->WriteProfileInt(szSection, _xtpMRUDockBottomPos, m_rectMRUDockPos.bottom);
	pApp->WriteProfileInt(szSection, _xtpMRUFloatXPos, m_ptMRUFloatPos.x);
	pApp->WriteProfileInt(szSection, _xtpMRUFloatYPos, m_ptMRUFloatPos.y);

	pApp->WriteProfileInt(szSection, _xtpDialogBar, m_bDialogBar);
	if (m_bDialogBar)
	{
		pApp->WriteProfileInt(szSection, _T("DialogBarDockingClientCx"), m_szDialogBarDockingClient.cx);
		pApp->WriteProfileInt(szSection, _T("DialogBarDockingClientCy"), m_szDialogBarDockingClient.cy);
		pApp->WriteProfileInt(szSection, _T("DialogBarFloatingClientCx"), m_szDialogBarFloatingClient.cx);
		pApp->WriteProfileInt(szSection, _T("DialogBarFloatingClientCy"), m_szDialogBarFloatingClient.cy);
	}

	//m_pBar->m_pControls->SaveState(szSection);

	return TRUE;

}
BOOL CXTPToolBar::CToolBarInfo::LoadState(LPCTSTR lpszProfileName, int nIndex, CXTPDockState* pDockState)
{
	ASSERT(pDockState != NULL);
	if (!pDockState)
		return FALSE;

	CWinApp* pApp = AfxGetApp();
	TCHAR szSection[256];
	wsprintf(szSection, _xtpBarSection, lpszProfileName, nIndex);

	m_nBarID = pApp->GetProfileInt(szSection, _xtpBarID, 0);
	m_bVisible = (BOOL) pApp->GetProfileInt(szSection, _xtpVisible, TRUE);
	m_bFloating = (BOOL) pApp->GetProfileInt(szSection, _xtpFloating, FALSE);
	m_pointPos = CPoint(
		pApp->GetProfileInt(szSection, _xtpXPos, -1),
		pApp->GetProfileInt(szSection, _xtpYPos, -1));

	pDockState->ScalePoint(m_pointPos);

	m_nMRUWidth = pApp->GetProfileInt(szSection, _xtpMRUWidth, 32767);

	m_uMRUDockPosition = (XTPBarPosition)pApp->GetProfileInt(szSection, _xtpMRUDockID, 0);

	m_rectMRUDockPos = CRect(
		pApp->GetProfileInt(szSection, _xtpMRUDockLeftPos, 0),
		pApp->GetProfileInt(szSection, _xtpMRUDockTopPos, 0),
		pApp->GetProfileInt(szSection, _xtpMRUDockRightPos, 0),
		pApp->GetProfileInt(szSection, _xtpMRUDockBottomPos, 0));
	pDockState->ScaleRectPos(m_rectMRUDockPos);

	m_ptMRUFloatPos = CPoint(
		pApp->GetProfileInt(szSection, _xtpMRUFloatXPos, 0),
		pApp->GetProfileInt(szSection, _xtpMRUFloatYPos, 0));
	pDockState->ScalePoint(m_ptMRUFloatPos);

	m_bDialogBar = pApp->GetProfileInt(szSection, _xtpDialogBar, FALSE);
	if (m_bDialogBar)
	{
		m_szDialogBarDockingClient.cx = pApp->GetProfileInt(szSection, _T("DialogBarDockingClientCx"), m_szDialogBarDockingClient.cx);
		m_szDialogBarDockingClient.cy = pApp->GetProfileInt(szSection, _T("DialogBarDockingClientCy"), m_szDialogBarDockingClient.cy);
		m_szDialogBarFloatingClient.cx = pApp->GetProfileInt(szSection, _T("DialogBarFloatingClientCx"), m_szDialogBarFloatingClient.cx);
		m_szDialogBarFloatingClient.cy = pApp->GetProfileInt(szSection, _T("DialogBarFloatingClientCy"), m_szDialogBarFloatingClient.cy);
	}


	return TRUE;
}


void CXTPDockState::ScalePoint(CPoint& pt)
{
	if (m_bScaling)
	{
		pt.x = MulDiv(pt.x, m_sizeDevice.cx, m_sizeLogical.cx);
		pt.y = MulDiv(pt.y, m_sizeDevice.cy, m_sizeLogical.cy);
	}

	CRect rectClip = XTPMultiMonitor()->GetWorkArea(pt);
	rectClip.right -= GetSystemMetrics(SM_CXICON);
	rectClip.bottom -= GetSystemMetrics(SM_CYICON);

	if (pt.x > rectClip.right)
		pt.x = rectClip.right;
	if (pt.y > rectClip.bottom)
		pt.y = rectClip.bottom;
}

void CXTPDockState::ScaleRectPos(CRect& rect)
{
	CPoint pt;

	if (m_bScaling)
	{
		pt = rect.TopLeft();

		pt.x = MulDiv(pt.x, m_sizeDevice.cx, m_sizeLogical.cx) - rect.left;
		pt.y = MulDiv(pt.y, m_sizeDevice.cy, m_sizeLogical.cy) - rect.top;
		rect.OffsetRect(pt);
	}
	pt.x = pt.y = 0;

	CRect rectClip = XTPMultiMonitor()->GetWorkArea(rect);

	if (rect.left > rectClip.right)
		pt.x = rectClip.right - rect.left;
	if (rect.top > rectClip.bottom)
		pt.y = rectClip.bottom - rect.top;

	if (!((pt.x == 0) && (pt.y == 0)))
		rect.OffsetRect(pt);
}

CXTPDockState::CXTPDockState()
{
	m_dwVersion = _XTP_SCHEMA_CURRENT;

	m_bScaling = FALSE;

	m_sizeDevice = CSize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

CXTPDockState::~CXTPDockState()
{
	Clear();
}

void CXTPDockState::Clear()
{
	for (int i = 0; i < m_arrBarInfo.GetSize(); i++)
		delete m_arrBarInfo[i];
	m_arrBarInfo.RemoveAll();

}

void CXTPDockState::SaveState(LPCTSTR lpszProfileName)
{
	int nIndex = 0;
	for (int i = 0; i < m_arrBarInfo.GetSize(); i++)
	{
		TOOLBARINFO* pInfo = m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		if (!pInfo)
			continue;
		if (pInfo->SaveState(lpszProfileName, nIndex))
			nIndex++;
	}


	CWinApp* pApp = AfxGetApp();
	TCHAR szSection[256];
	wsprintf(szSection, _xtpSummarySection, lpszProfileName);
	pApp->WriteProfileInt(szSection, _xtpBars, nIndex);

	CSize size = GetScreenSize();
	pApp->WriteProfileInt(szSection, _xtpScreenCX, size.cx);
	pApp->WriteProfileInt(szSection, _xtpScreenCY, size.cy);
}

void CXTPDockState::LoadState(LPCTSTR lpszProfileName)
{
	CWinApp* pApp = AfxGetApp();
	TCHAR szSection[256];
	wsprintf(szSection, _xtpSummarySection, lpszProfileName);
	int nBars = pApp->GetProfileInt(szSection, _xtpBars, 0);

	CSize size;
	size.cx = pApp->GetProfileInt(szSection, _xtpScreenCX, 0);
	size.cy = pApp->GetProfileInt(szSection, _xtpScreenCY, 0);
	SetScreenSize(size);

	for (int i = 0; i < nBars; i++)
	{
		TOOLBARINFO* pInfo = new TOOLBARINFO;
		m_arrBarInfo.Add(pInfo);
		pInfo->LoadState(lpszProfileName, i, this);
	}
}

void CXTPDockState::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPPropExchangeSection secDockState(pPX->GetSection(_T("DockState")));
	// read/write version info
	if (pPX->IsStoring())
	{
		PX_DWord(&secDockState, _T("Version"), m_dwVersion, 0);

		CSize sz = GetScreenSize();
		PX_Size(&secDockState, _T("ScreenSize"), sz, CSize(0, 0));

		WORD nCount = (WORD)m_arrBarInfo.GetSize();
		PX_UShort(&secDockState, _T("Count"), nCount, 0);


		for (int i = 0; i < m_arrBarInfo.GetSize(); i++)
		{
			CString strSection;
			strSection.Format(_T("BarInfo%i"), i);

			CXTPPropExchangeSection secBarInfo(secDockState->GetSection(strSection));
			m_arrBarInfo[i]->DoPropExchange(&secBarInfo, this);
		}
	}
	else
	{
		Clear(); //empty out dockstate

		PX_DWord(&secDockState, _T("Version"), m_dwVersion, 0);

		CSize size;
		PX_Size(&secDockState, _T("ScreenSize"), size, CSize(0, 0));
		SetScreenSize(size);

		WORD nOldSize = 0;
		PX_UShort(&secDockState, _T("Count"), nOldSize, 0);

		m_arrBarInfo.SetSize(nOldSize);
		for (int i = 0; i < nOldSize; i++)
		{
			m_arrBarInfo[i] = new TOOLBARINFO;

			CString strSection;
			strSection.Format(_T("BarInfo%i"), i);

			CXTPPropExchangeSection secBarInfo(secDockState->GetSection(strSection));

			m_arrBarInfo[i]->DoPropExchange(&secBarInfo, this);
		}
		m_dwVersion = _XTP_SCHEMA_CURRENT;
	}
}

CSize CXTPDockState::GetScreenSize()
{
	return m_sizeDevice;
}

void CXTPDockState::SetScreenSize(CSize& size)
{
	m_sizeLogical = size;
	m_bScaling = (size != m_sizeDevice && size.cx != 0 && size.cy != 0);
}

void CXTPToolBar::GetBarInfo(CToolBarInfo* pInfo)
{
// get state info
	pInfo->m_nBarID = m_nBarID;
	pInfo->m_pBar = this;
	pInfo->m_bVisible = IsVisible(); // handles delayed showing and hiding

	pInfo->m_nMRUWidth = m_nMRUWidth;

	CXTPWindowRect rect(this);
	if (m_pDockBar != NULL) m_pDockBar->ScreenToClient(&rect);

	pInfo->m_pointPos = m_hWnd ? rect.TopLeft() : m_pDockContext->m_ptMRUFloatPos;

	pInfo->m_bFloating = m_barPosition == xtpBarFloating;

	pInfo->m_uMRUDockPosition = m_pDockContext->m_uMRUDockPosition;
	pInfo->m_rectMRUDockPos = m_pDockContext->m_rectMRUDockPos;
	pInfo->m_ptMRUFloatPos = m_pDockContext->m_ptMRUFloatPos;

	pInfo->m_bDialogBar = IsDialogBar();
	pInfo->m_szDialogBarDockingClient = CSize(0, 0);
	pInfo->m_szDialogBarFloatingClient = CSize(0, 0);

	if (pInfo->m_bDialogBar)
	{
		pInfo->m_szDialogBarDockingClient = ((CXTPDialogBar*)this)->m_szDockingClient;
		pInfo->m_szDialogBarFloatingClient = ((CXTPDialogBar*)this)->m_szFloatingClient;
	}
}

void CXTPToolBar::SetBarInfo(CToolBarInfo* pInfo)
{
	UINT nFlags = (pInfo->m_bFloating ? 0 : SWP_NOSIZE) | SWP_NOACTIVATE | SWP_NOZORDER;

	if (!pInfo->m_bFloating && GetDockBar() == NULL)
		nFlags |= SWP_NOMOVE;

	m_nMRUWidth = pInfo->m_nMRUWidth;

	if (pInfo->m_bDialogBar && IsDialogBar())
	{
		((CXTPDialogBar*)this)->m_szDockingClient = pInfo->m_szDialogBarDockingClient;
		((CXTPDialogBar*)this)->m_szFloatingClient = pInfo->m_szDialogBarFloatingClient;
	}

	ASSERT(m_pDockContext != NULL);
	// You need to call EnableDocking before calling LoadBarState
	m_pDockContext->m_uMRUDockPosition = pInfo->m_uMRUDockPosition;
	m_pDockContext->m_rectMRUDockPos = pInfo->m_rectMRUDockPos;
	m_pDockContext->m_ptMRUFloatPos = pInfo->m_ptMRUFloatPos;

	// move and show/hide the window
	SetWindowPos(NULL, pInfo->m_pointPos.x, pInfo->m_pointPos.y, 0, 0,
		nFlags | (pInfo->m_bVisible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

	m_bVisible = pInfo->m_bVisible;

	if (pInfo->m_bFloating) OnRecalcLayout();
}

CString CXTPCommandBars::GetIsolatedFileName(const CString& strPrifileName)
{
	if (!XTPSystemVersion()->IsWin2KOrGreater())
		return _T("");

	LPITEMIDLIST pidlAppData = NULL;
	IMalloc* pMalloc = NULL;
	CString strAppDataFolder;

	HRESULT hr = SHGetMalloc(&pMalloc);
	if (SUCCEEDED(hr))
	{
		if (SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidlAppData) == NOERROR)
		{
			TCHAR   szPath[MAX_PATH];

			if (SHGetPathFromIDList(pidlAppData, szPath))
			{
				strAppDataFolder = szPath;
			}
		}

		pMalloc->Free(pidlAppData);
		pMalloc->Release();
	}

	if (strAppDataFolder.IsEmpty())
		return _T("");

	strAppDataFolder += CString(_T("\\")) + AfxGetApp()->m_pszAppName;
	CreateDirectory(strAppDataFolder, NULL);

	if (!DIRECTORYEXISTS_S(strAppDataFolder))
		return _T("");

	// Set customization path as "\Documents and Settings\user\Application Data\AppName\PrifileName"
	return strAppDataFolder + _T("\\") + strPrifileName;
}

BOOL CXTPCommandBars::GetControlsFileName(CString& strFileName, LPCTSTR lpszProfileName)
{
	if (!m_strControlsFileName.IsEmpty())
	{
		strFileName = m_strControlsFileName;
		return TRUE;
	}

	CString strProfileName(lpszProfileName);
	REPLACE_S(strProfileName, _T("\\"), _T("_"));

	strFileName = GetIsolatedFileName(strProfileName);
	if (!strFileName.IsEmpty())
		return TRUE;

	TCHAR szBuff[_MAX_PATH];


	VERIFY(::GetModuleFileName(AfxGetApp()->m_hInstance, szBuff, _MAX_PATH));
	strFileName.Format(_T("%s-%s"), szBuff, (LPCTSTR)strProfileName);

	return TRUE;
}

#define ZIP_HEADER ('Z' + ('I' << 8) + ('P' << 16) + ('!' << 24))

class CXTPZipMemFile : public CMemFile
{
	struct ZIPBUFFERHEADER
	{
		DWORD    dwType;
		DWORD    dwSize;
	};

	struct ZIPBUFFERINFO
	{
		ZIPBUFFERHEADER bh;
		BYTE     bBuffer[1];
	};
public:
	CXTPZipMemFile()
	{
	}
	CXTPZipMemFile(BYTE* lpBuffer, UINT nBufferSize)
	{
		AttachCompressedBuffer(lpBuffer, nBufferSize);
	}
	void AttachCompressedBuffer(BYTE* lpBuffer, UINT nBufferSize, BOOL bAutoDelete = FALSE)
	{
#ifdef ZLIB_H
		ZIPBUFFERINFO* pbi = (ZIPBUFFERINFO*)lpBuffer;

		if (nBufferSize < sizeof(ZIPBUFFERINFO) ||
			pbi->bh.dwType != ZIP_HEADER)
		{
			Attach(lpBuffer, nBufferSize);
			m_bAutoDelete = bAutoDelete;
			return;
		}

		DWORD dwDestCount = pbi->bh.dwSize;

		BYTE* lpBufferDest = (BYTE*)malloc(dwDestCount);
		uncompress(lpBufferDest, &dwDestCount, (LPBYTE)&pbi->bBuffer, nBufferSize - sizeof(ZIPBUFFERHEADER));

		ASSERT(dwDestCount == pbi->bh.dwSize);

		Attach(lpBufferDest, dwDestCount);
		m_bAutoDelete = TRUE;

		if (bAutoDelete)
		{
			free(lpBuffer);
		}
#else
		Attach(lpBuffer, nBufferSize);
		m_bAutoDelete = bAutoDelete;
#endif
	}

	BOOL OpenCompressedFile(LPCTSTR lpszFileName)
	{
		if (!CFile::Open(lpszFileName, CFile::modeRead))
			return FALSE;

		UINT nSize = (UINT)CFile::GetLength();
		LPBYTE lpBuffer = (LPBYTE)malloc(nSize);
		CFile::Read(lpBuffer, nSize);

		CFile::Close();

		AttachCompressedBuffer(lpBuffer, nSize, TRUE);

		return TRUE;
	}

	~CXTPZipMemFile()
	{
	}

public:
	void DetachCompressedBuffer(BYTE*& lpBuffer, DWORD& dwCount)
	{
		dwCount = (DWORD)GetPosition();
		lpBuffer = Detach();

#ifdef ZLIB_H
		if (dwCount < 20)
			return;

		ZIPBUFFERINFO* pbi = (ZIPBUFFERINFO*)malloc(dwCount + sizeof(ZIPBUFFERHEADER));
		DWORD dwDestCount = dwCount;

		if (compress((LPBYTE)&pbi->bBuffer, &dwDestCount, lpBuffer, dwCount) != Z_OK)
		{
			free(pbi);
			return;
		}

		pbi->bh.dwType = ZIP_HEADER;
		pbi->bh.dwSize = dwCount;

		free(lpBuffer);
		dwCount = dwDestCount + sizeof(ZIPBUFFERHEADER);
		lpBuffer = (LPBYTE)pbi;
#endif
	}
};

void CXTPCommandBars::SaveBarState(LPCTSTR lpszProfileName, BOOL bOnlyCustomized /*= TRUE*/)
{
	CXTPDockState state;
	GetDockState(state);
	state.SaveState(lpszProfileName);
	SaveDockBarsState(lpszProfileName);

	XTP_COMMANDBARS_PROPEXCHANGE_PARAM param;
	param.pCommandBars = this;
	param.bSaveOnlyCustomized = bOnlyCustomized;

	if (m_bCustomizeAvail)
	{
		CXTPCommandBarList lstCommandBars(this);
		GenerateCommandBarList(&lstCommandBars, &param);

		CXTPZipMemFile memFile;
		CArchive ar (&memFile, CArchive::store);

		TCHAR szSection[256];
		wsprintf(szSection, _xtpCommandBarControlsSection, lpszProfileName);

		CXTPPropExchangeArchive px(ar);

		px.m_dwData = (DWORD_PTR)&param;

		if (SaveCommandBarList(&px, &lstCommandBars))
		{
			ar.Flush();

			DWORD dwCount = 0;
			BYTE* pControls = 0;

			memFile.DetachCompressedBuffer(pControls, dwCount);

			CString strFileName;
			BOOL bSaveToFile = dwCount > 16000;

			if (bSaveToFile)
			{
				bSaveToFile = GetControlsFileName(strFileName, lpszProfileName);
			}

			if (bSaveToFile)
			{
				::DeleteFile((LPCTSTR)strFileName);
			}

			if (bSaveToFile)
			{
				try
				{
					CFile file;
					if (file.Open(strFileName, CFile::modeWrite | CFile::modeCreate))
					{
						file.Write(pControls, dwCount);
						AfxGetApp()->WriteProfileString(szSection, _xtpCommandBarControls, strFileName);
						AfxGetApp()->WriteProfileInt(szSection, _xtpCommandBarLoadFromFile, TRUE);
						file.Close();
						SetFileAttributes(strFileName, FILE_ATTRIBUTE_HIDDEN);
					}
				}
				catch (CFileException* pEx)
				{
					TRACE(_T("CXTPCommandBars::SaveBarState. File exception\r\n"));
					pEx->Delete ();
				}
			}
			else
			{
				AfxGetApp()->WriteProfileBinary(szSection, _xtpCommandBarControls, pControls, dwCount);
				AfxGetApp()->WriteProfileInt(szSection, _xtpCommandBarLoadFromFile, FALSE);
			}


			ar.Abort();
			memFile.Close();
			free(pControls);
		}
		else
		{
			AfxGetApp()->WriteProfileString(szSection, NULL, NULL);
		}
	}

	if (GetMenuBar() && GetMenuBar()->m_bDirtyMenu)
	{
		GetMenuBar()->SwitchActiveMenu();
	}
	m_pOptions->bDirtyState = FALSE;
}

void CXTPCommandBars::_LoadControlsPart(CFile& file, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	CArchive ar (&file, CArchive::load);

	CXTPPropExchangeArchive px(ar);
	px.m_dwData = (DWORD_PTR)pParam;

	CXTPCommandBarList lstCommandBars(this);
	if (LoadCommandBarList(&px, &lstCommandBars))
	{
		RestoreCommandBarList(&lstCommandBars, pParam->bLoadSilent);
	}

	ar.Close();
}

void CXTPCommandBars::LoadBarState(LPCTSTR lpszProfileName, BOOL bSilent)
{

	XTP_COMMANDBARS_PROPEXCHANGE_PARAM paramT;
	paramT.pCommandBars = this;
	paramT.bLoadSilent = bSilent;

	if (m_bCustomizeAvail)
	{
		TCHAR szSection[256];
		wsprintf(szSection, _xtpCommandBarControlsSection, lpszProfileName);

		if (AfxGetApp()->GetProfileInt(szSection, _xtpCommandBarLoadFromFile, FALSE))
		{
			CString strFileName = AfxGetApp()->GetProfileString(szSection, _xtpCommandBarControls);
			CXTPZipMemFile file;
			if (file.OpenCompressedFile(strFileName))
			{
				_LoadControlsPart(file, &paramT);
			}

		}
		else
		{
			BYTE* pControls = NULL;
			DWORD dwCount = 0;

			CXTPPropExchangeSection sec(TRUE, szSection);

			// PX_Blob support 16000 characters for ini file (MFC - 4096)
			if (PX_Blob(&sec, _xtpCommandBarControls, pControls, dwCount))
			{
				CXTPZipMemFile memFile(pControls, dwCount);

				_LoadControlsPart(memFile, &paramT);

				memFile.Close();
			}

			if (pControls) free(pControls);
		}
	}


	CXTPDockState state;
	state.LoadState(lpszProfileName);

	LoadDockBarsState(lpszProfileName);
	SetDockState(state);

	RecalcFrameLayout(TRUE);
	m_pOptions->bDirtyState = FALSE;

}

void CXTPCommandBars::SaveDockBarsState(LPCTSTR lpszProfileName)
{
	CWinApp* pApp = AfxGetApp();
	TCHAR szSection[256];

	for (int nIndex = 0; nIndex < 4; nIndex++)
	{
		wsprintf(szSection, _xtpDockBarSection, lpszProfileName, nIndex);
		pApp->WriteProfileString(szSection, NULL, NULL);

		CXTPDockBar* pDock = m_pDocks[nIndex];
		int nSize = (int)pDock->m_arrBars.GetSize();
		if (nSize > 1)
		{
			int nCount = 0;
			DWORD dwIDPrev = (DWORD)-1;
			for (int i = 0; i < nSize; i++)
			{
				CXTPToolBar* pBar = (CXTPToolBar*)pDock->m_arrBars[i];
				DWORD dwID = pBar ? pBar->m_nBarID : 0;
				if (dwIDPrev == 0 && dwID == 0)
					continue;
				dwIDPrev = dwID;

				TCHAR buf[16];
				wsprintf(buf, _xtpBar, nCount++);
				pApp->WriteProfileInt(szSection, buf, (int)dwID);
			}
			pApp->WriteProfileInt(szSection, _xtpBars, nCount);
		}
	}


	CControlBar* pBar = m_pFrame && m_pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)) ? ((CFrameWnd*)m_pFrame)->GetControlBar(AFX_IDW_STATUS_BAR) : 0;

	if (pBar != NULL && pBar->GetSafeHwnd() && !pBar->IsVisible())
	{
		pApp->WriteProfileInt(lpszProfileName, _T("StatusBar"), FALSE);
	}
	else
	{
		pApp->WriteProfileString(lpszProfileName, _T("StatusBar"), NULL);
	}
}

void CXTPCommandBars::LoadDockBarsState(LPCTSTR lpszProfileName)
{
	CWinApp* pApp = AfxGetApp();
	TCHAR szSection[256];
	for (int nIndex = 0; nIndex < 4; nIndex++)
	{
		wsprintf(szSection, _xtpDockBarSection, lpszProfileName, nIndex);

		CXTPDockBar* pDock = m_pDocks[nIndex];

		int nBars = pApp->GetProfileInt(szSection, _xtpBars, 0);
		if (nBars > 1)
		{
			for (int i = 0; i < nBars; i++)
			{
				TCHAR buf[16];
				wsprintf(buf, _xtpBar, i);
				DWORD dwID = pApp->GetProfileInt(szSection, buf, 0);

				CXTPToolBar* pBar = GetToolBar(dwID);
				if (pBar)
				{
					if (pBar->m_hWnd && pBar->GetParent() != pDock)
						pBar->SetParent(pDock);
					if (pBar->m_pDockBar != NULL)
						pBar->m_pDockBar->RemoveCommandBar(pBar);
					pBar->m_pDockBar = pDock;
					pBar->m_barPosition = pDock->GetPosition();
				}
				pDock->m_arrBars.InsertAt(i, pBar);

			}
		}
	}

	CControlBar* pBar = m_pFrame && m_pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)) ? ((CFrameWnd*)m_pFrame)->GetControlBar(AFX_IDW_STATUS_BAR) : 0;
	if (pBar != NULL && !pApp->GetProfileInt(lpszProfileName, _T("StatusBar"), TRUE))
	{
		((CFrameWnd*)m_pFrame)->ShowControlBar(pBar, FALSE, FALSE);
	}
}

void CXTPCommandBars::DoPropExchange(CXTPPropExchange* pPX, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	XTP_COMMANDBARS_PROPEXCHANGE_PARAM paramT;
	if (pParam == NULL)
		pParam = &paramT;

	pParam->pCommandBars = this;

	pPX->m_dwData = (DWORD_PTR)pParam;

	CXTPDockState state;

	pPX->ExchangeSchemaSafe();

	if (pPX->IsStoring())
	{
		CXTPToolBar* pDesignerBar = 0;

		if (pParam->bSerializeActions)
		{
			CXTPPropExchangeSection secActions(pPX->GetSection(_T("Actions")));
			PX_Bool(&secActions, _T("Enabled"), m_bEnableActions, FALSE);

			m_pActions->DoPropExchange(&secActions);
		}

		if (pParam->bSerializeDesignerControls && m_pDesignerControls)
		{
			pDesignerBar = (CXTPToolBar*)Add(_T("Controls"), xtpBarFloating);
			if (pDesignerBar->m_pControls != NULL)
			{
				pDesignerBar->m_pControls->InternalRelease();
			}
			pDesignerBar->m_pControls = m_pDesignerControls;
			m_pDesignerControls->InternalAddRef();

			pDesignerBar->SetBarID(0xFFEEFF);
			pDesignerBar->SetVisible(FALSE);

		}

		if (pParam->bSerializeControls)
		{
			CXTPCommandBarList lstCommandBars(this);
			GenerateCommandBarList(&lstCommandBars, pParam);

			VERIFY(SaveCommandBarList(pPX, &lstCommandBars));
		}
		if (pDesignerBar)
		{
			Remove(pDesignerBar);
		}

		if (pParam->bSerializeLayout)
		{
			GetDockState(state);
			CXTPPropExchangeSection secLayout(pPX->GetSection(_T("Layout")));
			state.DoPropExchange(&secLayout);
			SerializeDockBarsState(&secLayout);
		}

		if (GetMenuBar() && GetMenuBar()->m_bDirtyMenu)
		{
			GetMenuBar()->SwitchActiveMenu();
		}
	}
	else
	{
		if (pParam->bSerializeActions && pPX->GetSchema() > _XTP_SCHEMA_100)
		{
			CXTPPropExchangeSection secActions(pPX->GetSection(_T("Actions")));
			PX_Bool(&secActions, _T("Enabled"), m_bEnableActions, FALSE);

			m_pActions->DoPropExchange(&secActions);
		}

		if (pParam->bSerializeControls)
		{
			CXTPCommandBarList lstCommandBars(this);
			if (LoadCommandBarList(pPX, &lstCommandBars))
			{
				RestoreCommandBarList(&lstCommandBars, pParam->bLoadSilent);

				CXTPMenuBar* pMenuBar = GetMenuBar();
				if (pMenuBar)
				{
					pMenuBar->SetupHook();
					pMenuBar->RefreshMenu();
				}
			}
			else
			{
				return;
			}
		}

		if (pParam->bSerializeLayout)
		{
			CXTPPropExchangeSection secLayout(pPX->GetSection(_T("Layout")));
			state.DoPropExchange(&secLayout);
			SerializeDockBarsState(&secLayout);
			SetDockState(state);
		}

		CXTPToolBar* pCommandBar = GetToolBar(0xFFEEFF);

		if (pCommandBar)
		{
			if (pParam->bSerializeDesignerControls)
			{

				if (m_pDesignerControls)
					m_pDesignerControls->InternalRelease();

				m_pDesignerControls = pCommandBar->GetControls();
				pCommandBar->m_pControls = NULL;

				Remove(pCommandBar);

				m_pDesignerControls->SetParent(NULL);
				m_pDesignerControls->Remove(m_pDesignerControls->FindControl(xtpControlError, XTP_ID_TOOLBAR_EXPAND, TRUE, FALSE));
				m_pDesignerControls->Remove(m_pDesignerControls->FindControl(xtpControlError, XTP_ID_TOOLBAR_HIDE, TRUE, FALSE));
				m_pDesignerControls->SetCommandBars(this);
			}
			else
			{
				Remove(pCommandBar);
			}
		}

		RecalcFrameLayout(TRUE);
	}

	if (pParam->bSerializeImages)
	{
		CXTPPropExchangeSection secImages(pPX->GetSection(_T("Images")));
		GetImageManager()->DoPropExchange(&secImages);
	}

	if (pParam->bSerializeOptions)
	{
		CXTPPropExchangeSection secShortcuts(pPX->GetSection(_T("Shortcuts")));
		m_pShortcutManager->DoPropExchange(&secShortcuts);

		CXTPPropExchangeSection secOptions(pPX->GetSection(_T("Options")));

		XTPPaintTheme theme = GetPaintManager()->BaseTheme();
		PX_Enum(&secOptions, _T("Theme"), theme, xtpThemeOffice2000);
		if (pPX->IsLoading()) SetPaintManager(CXTPPaintManager::CreateTheme(theme));

		m_pOptions->DoPropExchange(&secOptions);
		PX_Bool(&secOptions, _T("Customizable"), m_bCustomizeAvail, TRUE);


	}

	if (pParam->bSerializeDesignerControls && pPX->IsLoading())
	{
		m_pShortcutManager->CreateOriginalAccelTable();

		if (m_bCustomizeAvail)
		{
			for (int i = 0; i < GetCount(); i++)
			{
				CXTPToolBar* pToolBar = GetAt(i);
				pToolBar->GetControls()->CreateOriginalControls();
			}
		}
	}


	m_pOptions->bDirtyState = FALSE;

}

void CXTPCommandBars::SerializeBarState(CArchive& ar, BOOL bSerializeControls, BOOL bOnlyCustomized, BOOL bSilent)
{
	CXTPPropExchangeArchive px(ar);

	XTP_COMMANDBARS_PROPEXCHANGE_PARAM paramT;
	paramT.bSaveOnlyCustomized = bOnlyCustomized;
	paramT.bSerializeControls = bSerializeControls;
	paramT.bLoadSilent = bSilent;

	DoPropExchange(&px, &paramT);
}


void CXTPCommandBars::SerializeDockBarsState(CXTPPropExchange* pPX)
{
	CXTPPropExchangeSection secDockBars(pPX->GetSection(_T("DockBars")));

	for (int nIndex = 0; nIndex < 4; nIndex++)
	{
		CXTPDockBar* pDock = m_pDocks[nIndex];

		CString strSection;
		strSection.Format(_T("DockBar%i"), nIndex);
		CXTPPropExchangeSection secDockBar(secDockBars->GetSection(strSection));

		if (pPX->IsStoring())
		{

			int nSize = (int)pDock->m_arrBars.GetSize();
			DWORD nCount = 0;
			DWORD dwIDPrev = (DWORD)-1;
			int i;

			for (i = 0; i < nSize; i++)
			{
				CXTPToolBar* pBar = (CXTPToolBar*)pDock->m_arrBars[i];
				DWORD dwID = pBar ? pBar->m_nBarID : 0;
				if (dwIDPrev == 0 && dwID == 0) continue;
				dwIDPrev = dwID;
				nCount++;
			}
			if (nCount > 1)
			{
				PX_DWord(&secDockBar, _T("Count"), nCount, 0);
				int j = 0;

				dwIDPrev = (DWORD)-1;
				for (i = 0; i < nSize; i++)
				{
					CXTPToolBar* pBar = (CXTPToolBar*)pDock->m_arrBars[i];
					DWORD dwID = pBar ? pBar->m_nBarID : 0;
					if (dwIDPrev == 0 && dwID == 0)
						continue;
					dwIDPrev = dwID;

					CString strDockBarSection;
					strDockBarSection.Format(_T("Id%i"), j++);
					PX_DWord(&secDockBar, strDockBarSection, dwID, 0);
				}
			}
			else
			{
				nCount = 0;
				PX_DWord(&secDockBar, _T("Count"), nCount, 0);
			}
		}
		else
		{
			DWORD dwID = 0;
			DWORD nBars = 0;
			PX_DWord(&secDockBar, _T("Count"), nBars, 0);

			if (nBars > 1)
			{
				for (int i = 0; i < (int)nBars; i++)
				{
					CString strDockBarSection;
					strDockBarSection.Format(_T("Id%i"), i);
					PX_DWord(&secDockBar, strDockBarSection, dwID, 0);

					CXTPToolBar* pBar = GetToolBar(dwID);

					if (pBar)
					{
						if (pBar->m_hWnd && pBar->GetParent() != pDock)
							pBar->SetParent(pDock);
						if (pBar->m_pDockBar != NULL)
							pBar->m_pDockBar->RemoveCommandBar(pBar);
						pBar->m_pDockBar = pDock;
						pBar->m_barPosition = pDock->GetPosition();
					}
					pDock->m_arrBars.InsertAt(i, pBar);
				}
			}
		}
	}

}

void CXTPCommandBars::GetDockState(CXTPDockState& state)
{
	state.Clear(); //make sure dockstate is empty
	// get state info for each bar
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);
		ASSERT(pBar != NULL);
		CXTPToolBar::CToolBarInfo* pInfo = new CXTPToolBar::CToolBarInfo;
		pBar->GetBarInfo(pInfo);

		state.m_arrBarInfo.Add(pInfo);
	}
}

void CXTPCommandBars::SetDockState(CXTPDockState& state)
{
	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CXTPToolBar::CToolBarInfo* pInfo = state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);

		pInfo->m_pBar = GetToolBar(pInfo->m_nBarID);

		if (pInfo->m_pBar != NULL)
		{
			if (pInfo->m_bFloating)
			{
				FloatCommandBar(pInfo->m_pBar);
			}
			else
			{
				VERIFY(pInfo->m_pBar->Create(m_pFrame, FALSE));
			}

			pInfo->m_pBar->SetBarInfo(pInfo);
		}
	}
}


void CXTPCommandBars::GenerateCommandBarList(CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	DWORD nID = 0x1000000;
	int i;

	for (i = 0; i < GetCount(); i++)
	{
		CXTPToolBar* pBar = GetAt(i);

		if (pParam->bSaveOnlyCustomized && pBar->IsBuiltIn() && !pBar->IsTemporary())
		{
			if (!pBar->ShouldSerializeBar())
				continue;
		}


		pBar->GenerateCommandBarList(nID, pCommandBarList, pParam);
	}

	for (i = 0; i < m_pContextMenus->GetCount(); i++)
	{
		CXTPCommandBar* pBar = m_pContextMenus->GetAt(i);

		if (pParam->bSaveOnlyCustomized)
		{
			if (!pBar->GetControls()->IsChanged())
				continue;
		}

		pBar->GenerateCommandBarList(nID, pCommandBarList, pParam);
	}
}

void CXTPCommandBar::GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	if (!pCommandBarList->Lookup(this))
	{
		pCommandBarList->Add(this);
		InternalAddRef();


		if (!IsKindOf(RUNTIME_CLASS(CXTPToolBar)))
		{
			if (m_nBarID == 0 || m_nBarID >= 0x1000000)
				m_nBarID = nID++;
		}
		else ASSERT(m_nBarID != 0);

		m_pControls->GenerateCommandBarList(nID, pCommandBarList, pParam);
	}
}

void CXTPMenuBar::GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	if (!pCommandBarList->Lookup(this))
	{
		CXTPToolBar::GenerateCommandBarList(nID, pCommandBarList, pParam);

		POSITION pos = m_pMDIMenus->GetStartPosition();
		while (pos)
		{
			CXTPMenuBarMDIMenuInfo* pInfo;
			m_pMDIMenus->GetNextMenu(pos, pInfo);

			pInfo->m_pControls->GenerateCommandBarList(nID, pCommandBarList, pParam);
		}
	}
}

void CXTPControl::GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	CXTPCommandBar* pCommandBar = GetCommandBar();
	if (pCommandBar)
	{
		pCommandBar->GenerateCommandBarList(nID, pCommandBarList, pParam);
	}
}

void CXTPControls::GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControl* pControl = GetAt(i);

		if (pControl && !pControl->m_bTemporary)
		{
			pControl->GenerateCommandBarList(nID, pCommandBarList, pParam);
		}
	}

	if (m_pOriginalControls && pParam->bSaveOriginalControls)
		m_pOriginalControls->GenerateCommandBarList(nID, pCommandBarList, pParam);
}

BOOL CXTPCommandBars::SaveCommandBarList(CXTPPropExchange* pPX, CXTPCommandBarList* pCommandBarList)
{
	try
	{
		CXTPPropExchangeSection secCommandBars(pPX->GetSection(_T("CommandBars")));

		long nCount = (long)pCommandBarList->GetCount();

		CXTPPropExchangeEnumeratorPtr pEnumerator(secCommandBars->GetEnumerator(_T("CommandBar")));
		POSITION posEnum = pEnumerator->GetPosition(nCount, FALSE);

		for (int i = 0; i < nCount; i++)
		{
			CXTPCommandBar* pCommandBar = pCommandBarList->GetAt(i);
			CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));
			PX_Object(&secItem, pCommandBar, RUNTIME_CLASS(CXTPCommandBar));
		}


	}
	catch (COleException* pEx)
	{
		TRACE(_T("CXTPCommandBars::SaveCommandBarList. OLE exception: %x\r\n"),
			pEx->m_sc);
		pEx->Delete ();
		return FALSE;
	}
	catch (CArchiveException* pEx)
	{
		TRACE(_T("CXTPCommandBars::SaveCommandBarList. Archive exception\r\n"));
		pEx->Delete ();
		return FALSE;
	}
	return TRUE;

}


BOOL CXTPCommandBars::LoadCommandBarList(CXTPPropExchange* pPX, CXTPCommandBarList* pCommandBarList)
{
	try
	{
		CXTPPropExchangeSection secCommandBars(pPX->GetSection(_T("CommandBars")));

		CXTPPropExchangeEnumeratorPtr pEnumerator(secCommandBars->GetEnumerator(_T("CommandBar")));
		POSITION posEnum = pEnumerator->GetPosition(0, FALSE);

		while (posEnum)
		{
			CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));

			CXTPCommandBar* pCommandBar = NULL;
			PX_Object(&secItem, pCommandBar, RUNTIME_CLASS(CXTPCommandBar));

			if (!pCommandBar)
			{
				return FALSE;
			}

			pCommandBar->GetControls()->RefreshIndexes();

			pCommandBarList->Add(pCommandBar);
		}




	}
	catch (COleException* pEx)
	{
		TRACE(_T("CXTPCommandBars::LoadCommandBarList. OLE exception: %x\r\n"),
			pEx->m_sc);
		pEx->Delete ();
		return FALSE;
	}
	catch (CArchiveException* pEx)
	{
		TRACE(_T("CXTPCommandBars::LoadCommandBarList. Archive exception\r\n"));
		pEx->Delete ();
		return FALSE;
	}
	return TRUE;

}

void CXTPToolBar::MergeToolBar(CXTPCommandBar* pCommandBar, BOOL bSilent)
{
	CXTPControls* pLastControls = pCommandBar->GetControls()->GetOriginalControls();

	if (pLastControls && GetControls()->GetOriginalControls()
		&& GetControls()->GetOriginalControls()->Compare(pLastControls) == FALSE)
	{
		//silent restore if not customized
		if (!bSilent && pLastControls->Compare(pCommandBar->GetControls()) == FALSE)
		{
			CString strMessage, strUpdate;
			VERIFY(XTPResourceManager()->LoadString(&strUpdate, XTP_IDS_CONFIRM_UPDATE));
			strMessage.Format(strUpdate, (LPCTSTR)m_strTitle);

			if (AfxMessageBox(strMessage, MB_YESNO | MB_ICONQUESTION) == IDNO)
			{
				CXTPOriginalControls* pControls = GetControls()->GetOriginalControls();
				CMDTARGET_ADDREF(pControls);

				Copy(pCommandBar, FALSE);

				m_pControls->SetOriginalControls(pControls);
			}
		}
	}
	else
	{
		CXTPOriginalControls* pOriginalControls = pCommandBar->m_pControls->m_pOriginalControls ? NULL :
			m_pControls->m_pOriginalControls;
		CMDTARGET_ADDREF(pOriginalControls);

		Copy(pCommandBar, FALSE);

		if (pOriginalControls)
		{
			ASSERT(m_pControls->m_pOriginalControls == NULL);
			m_pControls->SetOriginalControls(pOriginalControls);
		}
	}
}

void CXTPCommandBars::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList, BOOL bSilent)
{
	for (int i = 0; i < pCommandBarList->GetCount(); i++)
	{
		CXTPCommandBar* pCommandBar = pCommandBarList->GetAt(i);
		CXTPToolBar* pToolBar = DYNAMIC_DOWNCAST(CXTPToolBar, pCommandBar);

		if (pToolBar)
		{
			CXTPToolBar* pExistsBar = GetToolBar(pToolBar->GetBarID());

			pCommandBar->RestoreCommandBarList(pCommandBarList);

			if (pExistsBar)
			{
				pExistsBar->MergeToolBar(pCommandBar, bSilent);
			}
			else
			{
				m_arrBars.Add(pToolBar);
				pToolBar->InternalAddRef();
				pToolBar->Create(m_pFrame, FALSE);
				pToolBar->EnableDocking(pToolBar->m_dwFlags);

				OnToolBarAdded(pToolBar);
			}

		}
		else if (pCommandBar->IsPopupBar() && ((CXTPPopupBar*)pCommandBar)->IsContextMenu())
		{
			CXTPCommandBar* pExistsBar = m_pContextMenus->FindCommandBar(pCommandBar->GetBarID());

			pCommandBar->RestoreCommandBarList(pCommandBarList);

			if (pExistsBar)
			{
				CXTPControls* pControls = pCommandBar->GetControls();
				CMDTARGET_ADDREF(pControls)

				CXTPOriginalControls* pOriginalControls = pExistsBar->GetControls()->GetOriginalControls();
				CMDTARGET_ADDREF(pOriginalControls);

				pExistsBar->SetControls(pControls);
				pCommandBar->SetControls(NULL);

				if (pOriginalControls)
				{
					pExistsBar->GetControls()->SetOriginalControls(pOriginalControls);
				}
			}
			else
			{
				m_pContextMenus->Add(pCommandBar);
				pCommandBar->InternalAddRef();

				if (pCommandBar->GetControls()->GetOriginalControls() == NULL)
					pCommandBar->GetControls()->CreateOriginalControls();
			}
		}
	}
}


void CXTPCommandBar::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
{
	m_pControls->RestoreCommandBarList(pCommandBarList);
}

void CXTPMenuBar::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
{
	CXTPToolBar::RestoreCommandBarList(pCommandBarList);

	POSITION pos = m_pMDIMenus->GetStartPosition();
	while (pos)
	{
		CXTPMenuBarMDIMenuInfo* pInfo;
		m_pMDIMenus->GetNextMenu(pos, pInfo);

		pInfo->m_pControls->RestoreCommandBarList(pCommandBarList);
	}
}



void CXTPControl::RestoreCommandBarList(CXTPCommandBarList* /*pCommandBarList*/)
{

}
void CXTPControlPopup::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
{
	if (m_dwCommandBarID != 0)
	{
		CXTPCommandBar* pCommandBar = pCommandBarList->FindCommandBar(m_dwCommandBarID);
		ASSERT(pCommandBar);

		if (pCommandBar)
		{
			SetCommandBar(pCommandBar);
			m_dwCommandBarID = 0;
			pCommandBar->RestoreCommandBarList(pCommandBarList);
		}
	}
}

void CXTPControls::RestoreCommandBarList(CXTPCommandBarList* pCommandBarList)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CXTPControl* pControl = GetAt(i);

		pControl->RestoreCommandBarList(pCommandBarList);
	}

	if (m_pOriginalControls)
		m_pOriginalControls->RestoreCommandBarList(pCommandBarList);
}

void CXTPCommandBar::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Enum(pPX, _T("Type"), m_barType, xtpBarTypeNormal);
	PX_Enum(pPX, _T("Position"), m_barPosition, xtpBarTop);
	PX_Long(pPX, _T("BarID"), (long&)m_nBarID, 0);
	PX_Long(pPX, _T("Flags"), (long&)m_dwFlags, 0);
	PX_Long(pPX, _T("Style"), (long&)m_dwStyle, 0);
	PX_String(pPX, _T("Title"), m_strTitle, _T(""));
	PX_Bool(pPX, _T("Customizable"), m_bCustomizable, TRUE);
	PX_Int(pPX, _T("MRUWidth"), m_nMRUWidth, 0);

	if (pPX->GetSchema() > _XTP_SCHEMA_80)
		PX_Size(pPX, _T("IconSize"), m_szIcons, CSize(0, 0));

	if (pPX->IsLoading())
	{
		ASSERT(pPX->m_dwData != 0);
		SetCommandBars(((XTP_COMMANDBARS_PROPEXCHANGE_PARAM*)pPX->m_dwData)->pCommandBars);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_100)
	{
		PX_Size(pPX, _T("ButtonSize"), m_szButtons, CSize(0, 0));
		PX_Bool(pPX, _T("ShowTextBelowIcons"), m_bTextBelow, XTP_BOOL_DEFAULT);

		BOOL bAnimation = IsAnimationEnabled();
		PX_Bool(pPX, _T("EnableAnimation"), bAnimation, FALSE);
		if (pPX->IsLoading())
		{
			EnableAnimation(bAnimation);
		}
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_103)
	{
		PX_Enum(pPX, _T("DefaultButtonStyle"), m_buttonStyle, xtpButtonAutomatic);

	}

	CXTPPropExchangeSection secControls(pPX->GetSection(_T("Controls")));
	m_pControls->DoPropExchange(&secControls);
}


void CXTPToolBar::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPCommandBar::DoPropExchange(pPX);

	PX_Bool(pPX, _T("BuiltIn"), m_bBuiltIn, TRUE);
	PX_Bool(pPX, _T("TearOff"), m_bTearOff, FALSE);

	if (pPX->GetSchema() > _XTP_SCHEMA_87)
		PX_Bool(pPX, _T("Closeable"), m_bCloseable, TRUE);

	if (pPX->GetSchema() > _XTP_SCHEMA_875)
		PX_Bool(pPX, _T("ContextMenuPresent"), m_bContextMenuPresent, TRUE);

	if (pPX->GetSchema() > _XTP_SCHEMA_98)
		PX_Bool(pPX, _T("CustomizeDialogPresent"), m_bCustomizeDialogPresent, TRUE);

	if (pPX->GetSchema() <= _XTP_SCHEMA_98 && pPX->IsLoading() && m_barType == xtpBarTypeMenuBar)
	{
		m_bCloseable = FALSE;
	}
}


void CXTPMenuBar::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsStoring())
	{
		if (m_pMDIMenus->GetCount() > 0)
		{
			SwitchMDIMenu(GetDefaultMenuResource());
			m_bDirtyMenu = TRUE;
		}
	}

	CXTPToolBar::DoPropExchange(pPX);
	PX_Long(pPX, _T("ResourceId"), (long&)m_nIDResource, 0);

	if (pPX->IsStoring())
	{
		//_XTP_SCHEMA_861
		DWORD dwCount = 0;

		POSITION pos = m_pMDIMenus->GetStartPosition();
		while (pos)
		{
			CXTPMenuBarMDIMenuInfo* pInfo;
			m_pMDIMenus->GetNextMenu(pos, pInfo);
			if (pInfo->m_bChanged)
				dwCount++;
		}

		int i = 0;
		pPX->WriteCount(dwCount);

		pos = m_pMDIMenus->GetStartPosition();
		while (pos)
		{
			CXTPMenuBarMDIMenuInfo* pInfo;
			m_pMDIMenus->GetNextMenu(pos, pInfo);
			if (!pInfo->m_bChanged)
				continue;

			long nIDCommand = (long)pInfo->m_nIDResource;

			CString strSection;
			strSection.Format(_T("%s%i"), _T("MDIMenu"), i++);

			CXTPPropExchangeSection sec(pPX->GetSection(strSection));
			PX_Long(&sec, _T("Id"), nIDCommand, 0);
			pInfo->m_pControls->DoPropExchange(&sec);
		}
	}
	else
	{
		if (pPX->GetSchema() > _XTP_SCHEMA_86)
		{
			DWORD nCount = (DWORD)pPX->ReadCount();

			for (DWORD i = 0; i < nCount; i++)
			{
				UINT nIDCommand = 0;

				CString strSection;
				strSection.Format(_T("%s%i"), _T("MDIMenu"), i);

				CXTPPropExchangeSection sec(pPX->GetSection(strSection));
				PX_Long(&sec, _T("Id"), (long&)nIDCommand, 0);

				CXTPMenuBarMDIMenuInfo* pInfo = NULL;
				if (!m_pMDIMenus->Lookup(nIDCommand))
				{
					pInfo = new CXTPMenuBarMDIMenuInfo(GetCommandBars(), nIDCommand);
					m_pMDIMenus->SetAt(nIDCommand, pInfo);

				}
				pInfo->m_pControls->DoPropExchange(&sec);
			}
		}
	}

}


void CXTPPopupBar::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPCommandBar::DoPropExchange(pPX);

	PX_Bool(pPX, _T("TearOffPopup"), m_bTearOffPopup, FALSE);
	PX_String(pPX, _T("TearOffCaption"), m_strTearOffCaption, _T(""));
	PX_Long(pPX, _T("TearOffId"), (long&)m_nTearOffID, 0);
	PX_Int(pPX, _T("TearOffWidth"), m_nTearOffWidth, 0);

	if (pPX->GetSchema() > _XTP_SCHEMA_85)
	{
		PX_Rect(pPX, _T("Borders"), m_rcBorders, CRect(2, 4, 2, 4));
		PX_Bool(pPX, _T("ShowShadow"), m_bShowShadow, TRUE);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_95)
	{
		PX_Bool(pPX, _T("DoubleGripper"), m_bDoubleGripper, FALSE);
	}

	if (pPX->GetSchema() < _XTP_SCHEMA_104)
	{
		SetShowGripper(TRUE);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_103)
	{
		PX_Bool(pPX, _T("ContextMenu"), m_bContextMenu, FALSE);
	}

}

void CXTPControls::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(pPX->m_dwData != 0);
	XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam = (XTP_COMMANDBARS_PROPEXCHANGE_PARAM*)pPX->m_dwData;


	if (pPX->IsStoring())
	{
		long nCount = 0;
		int i;
		for (i = 0; i < m_arrControls.GetSize(); i++)
		{
			CXTPControl* pControl = GetAt(i);
			if (!pControl->m_bTemporary) nCount++;
		}

		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Control")));
		POSITION posEnum = pEnumerator->GetPosition(nCount, FALSE);

		for (i = 0; i < (int)m_arrControls.GetSize(); i++)
		{
			CXTPControl* pControl = GetAt(i);
			if (!pControl->m_bTemporary)
			{
				CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));
				PX_Object(&secItem, pControl, RUNTIME_CLASS(CXTPControl));
			}
		}

		BOOL bOriginalControls = m_pOriginalControls && pParam->bSaveOriginalControls ? TRUE : FALSE;
		PX_Bool(pPX, _T("OriginalControls"), bOriginalControls, FALSE);

		if (bOriginalControls && m_pOriginalControls)
		{
			CXTPPropExchangeSection secControls(pPX->GetSection(_T("OriginalControls")));
			m_pOriginalControls->DoPropExchange(&secControls);
		}

		BOOL bCustomizedControls = FALSE;
		PX_Bool(pPX, _T("CustomizedControls"), bCustomizedControls, FALSE);

	}
	else
	{
		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Control")));
		POSITION posEnum = pEnumerator->GetPosition(0, FALSE);

		m_arrControls.RemoveAll();

		while (posEnum)
		{
			CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));

			CXTPControl* pControl = NULL;
			PX_Object(&secItem, pControl, RUNTIME_CLASS(CXTPControl));

			if (!pControl)
				AfxThrowArchiveException(CArchiveException::badClass);

			m_arrControls.Add(pControl);
		}

		RefreshIndexes();
		if (pPX->GetSchema() > _XTP_SCHEMA_80)
		{
			BOOL bOriginalControls = FALSE;
			PX_Bool(pPX, _T("OriginalControls"), bOriginalControls, FALSE);

			if (bOriginalControls)
			{
				CXTPOriginalControls* pOriginalControls = new CXTPOriginalControls();
				SetOriginalControls(pOriginalControls);

				CXTPPropExchangeSection secControls(pPX->GetSection(_T("OriginalControls")));
				pOriginalControls->DoPropExchange(&secControls);
			}

			BOOL bCustomizedControls = FALSE;
			PX_Bool(pPX, _T("CustomizedControls"), bCustomizedControls, FALSE);

			if (bCustomizedControls)
			{
				// _XTP_SCHEMA_80 compatibility.
				CXTPControls* pCustomizedControls = new CXTPControls;
				CXTPPropExchangeSection secControls(pPX->GetSection(_T("CustomizedControls")));
				pCustomizedControls->DoPropExchange(&secControls);
				pCustomizedControls->InternalRelease();
			}
		}
	}
}


void CXTPControlActions::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsStoring())
	{
		long nCount = GetCount();

		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Action")));
		POSITION posEnum = pEnumerator->GetPosition(nCount, FALSE);

		for (int i = 0; i < nCount; i++)
		{
			CXTPControlAction* pAction = GetAt(i);
			CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));

			int nId = pAction->GetID();
			PX_Int(&secItem, _T("Id"), nId);
			pAction->DoPropExchange(&secItem);
		}
	}
	else
	{
		CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Action")));
		POSITION posEnum = pEnumerator->GetPosition(0, FALSE);

		while (posEnum)
		{
			CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));

			int nId = 0;
			PX_Int(&secItem, _T("Id"), nId);

			CXTPControlAction* pAction = FindAction(nId);

			if (!pAction)
			{
				pAction = new CXTPControlAction(this);
				pAction->m_nId = nId;
				Insert(pAction);
			}

			pAction->DoPropExchange(&secItem);
		}
	}
}

void CXTPControlAction::DoPropExchange(CXTPPropExchange* pPX)
{
	PX_Int(pPX, _T("IconId"), m_nIconId, 0);
	PX_Int(pPX, _T("HelpId"), m_nHelpId, 0);
	PX_String(pPX, _T("Caption"), m_strCaption, _T(""));
	PX_String(pPX, _T("ShortcutText"), m_strShortcutText, _T(""));
	PX_String(pPX, _T("TooltipText"), m_strTooltipText, _T(""));
	PX_String(pPX, _T("DescriptionText"), m_strDescriptionText, _T(""));
	PX_String(pPX, _T("Category"), m_strCategory, _T(""));
}



void CXTPControl::DoPropExchange(CXTPPropExchange* pPX)
{
	ASSERT(sizeof(long) == sizeof(DWORD));
	CXTPCommandBars* pCommandBars = ((XTP_COMMANDBARS_PROPEXCHANGE_PARAM*)pPX->m_dwData)->pCommandBars;

	PX_Enum(pPX, _T("Type"), m_controlType, xtpControlButton);
	PX_Int(pPX, _T("Id"), m_nId, -1);

	if (pPX->IsLoading())
	{
		CXTPControlAction* pAction = pCommandBars->GetActions()->FindAction(m_nId);
		if (pAction)
		{
			SetAction(pAction);
		}
	}

	PX_Int(pPX, _T("IconId"), m_nIconId, 0);
	PX_Bool(pPX, _T("BeginGroup"), m_bBeginGroup, FALSE);
	PX_Long(pPX, _T("Tag"), (long&)m_nTag, 0);
	PX_Long(pPX, _T("Flags"), (long&)m_dwFlags, 0);
	PX_Long(pPX, _T("HideFlags"), (long&)m_dwHideFlags, 0);
	PX_String(pPX, _T("Caption"), m_strCaption, _T(""));
	PX_String(pPX, _T("ShortcutText"), m_strShortcutText, _T(""));
	PX_String(pPX, _T("TooltipText"), m_strTooltipText, _T(""));
	PX_String(pPX, _T("DescriptionText"), m_strDescriptionText, _T(""));
	PX_String(pPX, _T("Parameter"), m_strParameter, _T(""));
	PX_Int(pPX, _T("CustomIconId"), m_nCustomIconId, 0);
	PX_String(pPX, _T("CustomCaption"), m_strCustomCaption, _T(""));


	if (pPX->GetSchema() > _XTP_SCHEMA_71)
		PX_String(pPX, _T("Category"), m_strCategory, _T(""));

	if (m_nCustomIconId > 0)
	{
		ASSERT(pCommandBars->IsKindOf(RUNTIME_CLASS(CXTPCommandBars)));

		CXTPPropExchangeSection secCustomIcon(pPX->GetSection(_T("CustomIcon")));
		pCommandBars->GetImageManager()->DoPropExchange(m_nCustomIconId, &secCustomIcon);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_85)
	{
		if (!(pPX->IsStoring() && pPX->IsAllowDefaultValues() && m_mapDocTemplatesExcluded.IsEmpty()))
			PX_Serialize(pPX, _T("DocTemplatesExcluded"), &m_mapDocTemplatesExcluded);

		if (!(pPX->IsStoring() && pPX->IsAllowDefaultValues() && m_mapDocTemplatesAssigned.IsEmpty()))
			PX_Serialize(pPX, _T("DocTemplatesAssigned"), &m_mapDocTemplatesAssigned);

		PX_Bool(pPX, _T("DefaultItem"), m_bDefaultItem, FALSE);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_87)
		PX_Bool(pPX, _T("CloseSubMenuOnClick"), m_bCloseSubMenuOnClick, TRUE);

	if (pPX->GetSchema() > _XTP_SCHEMA_875)
		PX_Enum(pPX, _T("CustomStyle"), m_buttonCustomStyle, xtpButtonUndefined);

	if (pPX->GetSchema() < _XTP_SCHEMA_103 && m_buttonCustomStyle == xtpButtonAutomatic)
		m_buttonCustomStyle = xtpButtonUndefined;

	if (pPX->GetSchema() > _XTP_SCHEMA_98)
		PX_Int(pPX, _T("HelpId"), m_nHelpId, 0);

	if (pPX->GetSchema() > _XTP_SCHEMA_100)
		PX_Enum(pPX, _T("Style"), m_buttonStyle, xtpButtonAutomatic);


	if (pPX->GetSchema() > _XTP_SCHEMA_103)
	{
		PX_Int(pPX, _T("Width"), m_nWidth, 0);
		PX_Int(pPX, _T("Height"), m_nHeight, 0);
		PX_Int(pPX, _T("ExecuteOnPressInterval"), m_nExecuteOnPressInterval, 0);
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_1042)
	{
		PX_String(pPX, _T("ShortcutTextAuto"), m_strShortcutTextAuto, _T(""));
	}
}


void CXTPControlButton::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	if (pPX->GetSchema() < _XTP_SCHEMA_103)
		PX_Enum(pPX, _T("Style"), m_buttonStyle, xtpButtonAutomatic);
}

void CXTPControlPopup::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	if (pPX->IsStoring())
	{
		long dwCommandBarID = m_pCommandBar ? m_pCommandBar->m_nBarID : 0;
		PX_Long(pPX, _T("CommandBarId"), dwCommandBarID, 0);
	}
	else
	{
		PX_Long(pPX, _T("CommandBarId"), (long&)m_dwCommandBarID, 0);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_86 && pPX->GetSchema() < _XTP_SCHEMA_103)
		PX_Enum(pPX, _T("Style"), m_buttonStyle, xtpButtonAutomatic);
}

void CXTPControlComboBox::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControlPopup::DoPropExchange(pPX);

	PX_Bool(pPX, _T("DropDown"), m_bDropDown, TRUE);
	PX_Int(pPX, _T("Width"), m_nWidth, 0);
	PX_Enum(pPX, _T("ComboStyle"), m_comboStyle, xtpComboNormal);

	if (pPX->GetSchema() > _XTP_SCHEMA_95)
	{
		PX_String(pPX, _T("EditHint"), m_strEditHint, _T(""));
		PX_DWord(pPX, _T("ShellAutoCompleteFlags"), m_dwShellAutoCompleteFlags, 0);
		PX_Bool(pPX, _T("AutoComplete"), m_bAutoComplete, FALSE);
		PX_Int(pPX, _T("EditIconId"), m_nEditIconId, 0);
		PX_Int(pPX, _T("DropDownItemCount"), m_nDropDownItemCount, 12);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_98)
	{
		PX_DWord(pPX, _T("EditStyle"), m_dwEditStyle, 0);
	}

	if (pPX->IsLoading())
	{
		SetDropDownListStyle(m_bDropDown);
		EnableShellAutoComplete(m_dwShellAutoCompleteFlags);
	}

}

void CXTPControlEdit::DoPropExchange(CXTPPropExchange* pPX)
{
	CXTPControl::DoPropExchange(pPX);

	if (pPX->GetSchema() > _XTP_SCHEMA_861)
	{
		PX_Bool(pPX, _T("HasLabel"), m_bLabel, FALSE);
		PX_Int(pPX, _T("Width"), m_nWidth, 0);
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_875)
	{
		PX_Bool(pPX, _T("ReadOnly"), m_bReadOnly, FALSE);
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_95)
	{
		PX_String(pPX, _T("EditHint"), m_strEditHint, _T(""));
		PX_DWord(pPX, _T("ShellAutoCompleteFlags"), m_dwShellAutoCompleteFlags, 0);
	}
	if (pPX->GetSchema() > _XTP_SCHEMA_98)
	{
		PX_DWord(pPX, _T("EditStyle"), m_dwEditStyle, 0);
	}

	if (pPX->GetSchema() > _XTP_SCHEMA_103)
	{
		PX_Bool(pPX, _T("ShowSpinButtons"), m_bShowSpinButtons, FALSE);
	}

	if (pPX->IsLoading())
	{
		EnableShellAutoComplete(m_dwShellAutoCompleteFlags);
	}
}
