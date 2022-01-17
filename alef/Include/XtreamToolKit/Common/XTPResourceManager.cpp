// XTPResourceManager.cpp: implementation of the CXTPResourceManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#include "XTPVC80Helpers.h"
#include "XTPVC50Helpers.h"
#include "XTPResourceManager.h"
#include "XTPMacros.h"
#include "XTPPropExchange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const WORD cwXTPResManDefLangID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

CProcessLocal<CXTPResourceManager> CXTPResourceManager::m_managerInstance;


CXTPResourceManager* AFX_CDECL XTPResourceManager()
{
	return CXTPResourceManager::m_managerInstance.GetData();
}


BOOL CXTPResourceManager::SetResourceManager(CXTPResourceManager* pResourceManager)
{
	if (pResourceManager == NULL)
		return FALSE;

	if (!pResourceManager->m_bValid)
	{
		delete pResourceManager;
		return FALSE;

	}

	if (m_managerInstance.m_pObject != NULL)
		delete m_managerInstance.m_pObject;

	m_managerInstance.m_pObject = pResourceManager;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
// CXTPResourceManager


HINSTANCE CXTPResourceManager::m_hModXTResource = NULL;

CXTPResourceManager::CManageState::CManageState()
{
	m_bSet = false;
	Redo();
}

CXTPResourceManager::CManageState::~CManageState()
{
	Undo();
	m_hModOldResource = NULL;
}

void CXTPResourceManager::CManageState::Undo()
{
	if (!m_bSet)
		return;

	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	pModuleState->m_hCurrentResourceHandle = m_hModOldResource;

	m_bSet = false;
}

void CXTPResourceManager::CManageState::Redo()
{
	if (XTPResourceManager()->GetResourceHandle() == NULL)
	{
		m_bSet = false;
		return;
	}

	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	m_hModOldResource = pModuleState->m_hCurrentResourceHandle;

	pModuleState->m_hCurrentResourceHandle = XTPResourceManager()->GetResourceHandle();

	m_bSet = true;
}


CXTPResourceManager::CXTPResourceManager()
{
	m_bValid = TRUE;
	m_hResourceFile = 0;

	m_wResourcesLangID = cwXTPResManDefLangID;
}

CXTPResourceManager::~CXTPResourceManager()
{
	Close();
}

void CXTPResourceManager::Close()
{
	if (m_hResourceFile != 0)
	{
		FreeLibrary(m_hResourceFile);
		m_hResourceFile = 0;
	}
}

HMODULE CXTPResourceManager::GetResourceHandle() const
{
	return m_hModXTResource;
}

void CXTPResourceManager::SetResourceHandle(HMODULE hModRes)
{
	m_hModXTResource = hModRes;
	_UpdateResourcesLangID();
}

void CXTPResourceManager::SetResourceFile(const CString& strResourceFile)
{
	CString strExtension = strResourceFile.GetLength() > 3 ? strResourceFile.Right(3) : _T("");
	strExtension.MakeLower();

	if (strExtension == _T("xml"))
	{
		SetResourceManager(new CXTPResourceManagerXML(strResourceFile));
	}
	else
	{
		CXTPResourceManager* pResourceManager = new CXTPResourceManager;

		pResourceManager->m_hResourceFile = strResourceFile.IsEmpty() ? 0 : LoadLibrary(strResourceFile);

		if (pResourceManager->m_hResourceFile)
		{
			SetResourceHandle(pResourceManager->m_hResourceFile);
		}
		else
		{
			SetResourceHandle(AfxGetInstanceHandle());
		}

		pResourceManager->_UpdateResourcesLangID();

		SetResourceManager(pResourceManager);
	}
}

void CXTPResourceManager::_UpdateResourcesLangID()
{
	HMODULE hResModule = GetResourceHandle();
	WORD wLangID = GetResourceLanguage(hResModule);
	ASSERT(wLangID);

	m_wResourcesLangID = wLangID ? wLangID : cwXTPResManDefLangID;
}

WORD CXTPResourceManager::GetResourceLanguage(HMODULE hResModule)
{
	WORD wLangID = 0;
	::EnumResourceTypes(hResModule, &CXTPResourceManager::EnumResTypeProc, (LONG_PTR)&wLangID);
	return wLangID;
}


BOOL CXTPResourceManager::EnumResTypeProc(HMODULE hModule, LPTSTR lpszType, LONG_PTR lParam)
{
	WORD* pwLangID = (WORD*)lParam;
	ASSERT(pwLangID);
	if (!pwLangID)
		return FALSE;

	::EnumResourceNames(hModule, lpszType, &CXTPResourceManager::EnumResNameProc, lParam);

	if(*pwLangID == LANG_NEUTRAL)
		return TRUE; // continue if neutral

	return *pwLangID == cwXTPResManDefLangID; // continue if default
}

BOOL CXTPResourceManager::EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName,
										  LONG_PTR lParam)
{
	WORD* pwLangID = (WORD*)lParam;
	ASSERT(pwLangID);
	if (!pwLangID)
		return FALSE;

	::EnumResourceLanguages(hModule, lpszType, lpszName,
							&CXTPResourceManager::EnumResLangProc, lParam);

	if(*pwLangID == LANG_NEUTRAL)
		return TRUE; // continue if neutral

	return *pwLangID == cwXTPResManDefLangID; // continue if default
}

BOOL CXTPResourceManager::EnumResLangProc(HMODULE hModule, LPCTSTR lpszType,
								 LPCTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
	UNREFERENCED_PARAMETER(hModule); UNREFERENCED_PARAMETER(lpszType); UNREFERENCED_PARAMETER(lpszName);

	WORD* pwLangID = (WORD*)lParam;
	ASSERT(pwLangID);

	if (!pwLangID)
		return FALSE; // stop emuneration

	if(wIDLanguage == LANG_NEUTRAL)
		return TRUE; // continue if neutral

	*pwLangID = wIDLanguage;

	return *pwLangID == cwXTPResManDefLangID; // continue if default
}

BOOL CXTPResourceManager::LoadString(CString* pString, UINT nIDResource)
{
	HMODULE hResourceInstanse = GetResourceHandle();

	if (hResourceInstanse && ::FindResource(hResourceInstanse, MAKEINTRESOURCE((nIDResource >> 4)+1), RT_STRING))
	{
		CManageState state;

		if (pString->LoadString(nIDResource))
			return TRUE;
	}

	return pString->LoadString(nIDResource);
}

BOOL CXTPResourceManager::LoadMenu(CMenu* lpMenu, UINT nIDResource)
{
	HMODULE hResourceInstanse = GetResourceHandle();

	if (hResourceInstanse && ::FindResource(hResourceInstanse, MAKEINTRESOURCE(nIDResource), RT_MENU))
	{
		CManageState state;

		if (lpMenu->LoadMenu(nIDResource))
			return TRUE;
	}

	return lpMenu->LoadMenu(nIDResource);
}

BOOL CXTPResourceManager::LoadToolBar(CToolBar* pToolBar, UINT nIDResource)
{
	CManageState state;

	return pToolBar->LoadToolBar(nIDResource);
}

BOOL CXTPResourceManager::LoadBitmap(CBitmap* pBitmap, UINT nIDResource)
{
	HMODULE hResourceInstanse = GetResourceHandle();

	if (hResourceInstanse && ::FindResource(hResourceInstanse, MAKEINTRESOURCE(nIDResource), RT_BITMAP))
	{
		CManageState state;

		if (pBitmap->LoadBitmap(nIDResource))
			return TRUE;

	}

	return pBitmap->LoadBitmap(nIDResource);
}

HCURSOR CXTPResourceManager::LoadCursor(UINT nIDResource)
{
	HMODULE hResourceInstanse = GetResourceHandle();

	if (hResourceInstanse && ::FindResource(hResourceInstanse, MAKEINTRESOURCE(nIDResource), RT_GROUP_CURSOR))
	{
		CManageState state;

		HCURSOR hCursor = AfxGetApp()->LoadCursor(nIDResource);
		if (hCursor)
			return hCursor;
	}

	return AfxGetApp()->LoadCursor(nIDResource);
}

HGLOBAL CXTPResourceManager::LoadDialogTemplate2(LPCTSTR pszTemplate)
{
	HINSTANCE hResourceInstanse = GetResourceHandle();
	HRSRC hResource = FindResource(hResourceInstanse, pszTemplate, RT_DIALOG);

	if (hResource == NULL)
	{
		hResourceInstanse = AfxFindResourceHandle(pszTemplate, RT_DIALOG);
		if (hResourceInstanse == NULL)
			return NULL;

		hResource = FindResource(hResourceInstanse, pszTemplate, RT_DIALOG);
	}

	if (!hResource)
		return NULL;

	HGLOBAL hTemplate = LoadResource(hResourceInstanse, hResource);

	return hTemplate;
}

LPCDLGTEMPLATE CXTPResourceManager::LoadDialogTemplate(UINT nIDResource)
{
	LPCTSTR pszTemplate = MAKEINTRESOURCE(nIDResource);

	HGLOBAL hTemplate = LoadDialogTemplate2(pszTemplate);
	return (LPCDLGTEMPLATE)LockResource(hTemplate);
}

BOOL CXTPResourceManager::LoadHTML(CString* pText, UINT nIDResource)
{
	if (pText == NULL)
		return FALSE;

	HMODULE hResourceInstanse = GetResourceHandle();
	HRSRC hResource = ::FindResource(hResourceInstanse, MAKEINTRESOURCE(nIDResource), RT_HTML);

	if (hResource == NULL)
	{
		hResourceInstanse = AfxFindResourceHandle(MAKEINTRESOURCE(nIDResource), RT_HTML);
		if (hResourceInstanse == NULL)
			return FALSE;

		hResource = ::FindResource(hResourceInstanse, MAKEINTRESOURCE(nIDResource), RT_HTML);
	}

	if (!hResource)
		return FALSE;

	HGLOBAL hMem = ::LoadResource(hResourceInstanse, hResource);
	if (!hMem)
		return FALSE;

	DWORD dwSize = ::SizeofResource(hResourceInstanse, hResource);
	char *pSrc = (char*)::LockResource(hMem);
	if (!pSrc)
		return FALSE;

	const DWORD dwDstSize = (dwSize + 1) * sizeof(TCHAR);
	TCHAR *pDst = pText->GetBuffer(dwDstSize);
	if (pDst == NULL)
		return FALSE;

	::ZeroMemory((BYTE*)pDst, dwDstSize);

#ifdef _UNICODE
	int nLen = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSrc, dwSize,
		pDst, dwDstSize);
#else
	::CopyMemory(pDst, pSrc, dwSize);
	int nLen = dwSize;
#endif

	pText->ReleaseBuffer();

	return nLen > 0;
}

HICON CXTPResourceManager::CreateIconFromResource(HMODULE hModule, LPCTSTR lpszID, CSize iconSize)
{
	// Find the icon directory whose identifier is lpszID.
	HRSRC hResource = ::FindResource(hModule,
		lpszID, RT_GROUP_ICON);

	if (hResource == NULL)
		return NULL;

	// Load and lock the icon directory.
	HGLOBAL hMem = ::LoadResource(hModule, hResource);
	LPVOID lpResource = ::LockResource(hMem);

	// Get the identifier of the icon that is most appropriate
	// for the video display.
	int nID = ::LookupIconIdFromDirectoryEx((PBYTE) lpResource, TRUE,
		iconSize.cx, iconSize.cy, LR_DEFAULTCOLOR);

	// Find the bits for the nID icon.
	hResource = ::FindResource(hModule,
		MAKEINTRESOURCE(nID), MAKEINTRESOURCE(RT_ICON));

	if (hResource == NULL)
		return NULL;

	// Load and lock the icon.
	hMem = ::LoadResource(hModule, hResource);
	lpResource = ::LockResource(hMem);

	// Create a handle to the icon.
	HICON hIcon = ::CreateIconFromResourceEx((PBYTE)lpResource,
		::SizeofResource(hModule, hResource), TRUE, 0x00030000,
		iconSize.cx, iconSize.cy, LR_DEFAULTCOLOR);

	return hIcon;
}

HICON CXTPResourceManager::LoadIcon(LPCTSTR lpszID, CSize iconSize)
{
	HICON hIcon = NULL;

	HMODULE hResource = GetResourceHandle();
	if (hResource && ::FindResource(hResource, lpszID, RT_ICON))
	{
		CManageState state;

		hIcon = (HICON)::LoadImage(hResource, lpszID,
			IMAGE_ICON, iconSize.cx, iconSize.cy, 0);

		if (hIcon != NULL)
			return hIcon;
	}

	hResource = AfxFindResourceHandle(lpszID, RT_GROUP_ICON);
	if (hResource)
	{
		hIcon = (HICON)::LoadImage(hResource, lpszID,
			IMAGE_ICON, iconSize.cx, iconSize.cy, 0);

		if (hIcon != NULL)
			return hIcon;
	}

	return NULL;
}

HICON CXTPResourceManager::LoadIcon(int nID, CSize iconSize)
{
	return LoadIcon(MAKEINTRESOURCE(nID), iconSize);
}

int CXTPResourceManager::ShowMessageBox (UINT nIDPrompt, UINT nType)
{
	CString strPrompt;
	VERIFY(LoadString(&strPrompt, nIDPrompt));

	return AfxMessageBox(strPrompt, nType);
}


#ifndef  _XTP_EXCLUDE_XML


//////////////////////////////////////////////////////////////////////////
// CXTPResourceManagerXML

CXTPResourceManagerXML::CXTPResourceManagerXML(LPCTSTR strFileName)
{
	m_pResourceRoot = new CXTPPropExchangeXMLNode(TRUE, 0, _T("resource"));

	m_bValid = FALSE;

	if (!m_pResourceRoot->LoadFromFile(strFileName))
		return;

	if (!m_pResourceRoot->OnBeforeExchange())
		return;

	m_bValid = TRUE;

	_UpdateResourcesLangID();
}

CXTPResourceManagerXML::CXTPResourceManagerXML(CXTPPropExchangeXMLNode* pResourceRoot)
{
	m_bValid = FALSE;

	m_pResourceRoot = pResourceRoot;

	if (m_pResourceRoot == NULL)
		return;

	if (!m_pResourceRoot->OnBeforeExchange())
		return;

	m_bValid = TRUE;

	_UpdateResourcesLangID();
}


CXTPResourceManagerXML::~CXTPResourceManagerXML()
{
	Close();
}

void CXTPResourceManagerXML::Close()
{
	CXTPResourceManager::Close();

	CMDTARGET_RELEASE(m_pResourceRoot);

	POSITION pos = m_mapDialogs.GetStartPosition();
	while (pos)
	{
		DLGTEMPLATE* pDlgTemplate;
		UINT nIDResource;
		m_mapDialogs.GetNextAssoc(pos, nIDResource, pDlgTemplate);
		free (pDlgTemplate);
	}
	m_mapDialogs.RemoveAll();

	m_bValid = FALSE;
}

void CXTPResourceManagerXML::_UpdateResourcesLangID()
{
	long wResourcesLangID = cwXTPResManDefLangID;
	PX_Long(m_pResourceRoot, _T("LANGID"), wResourcesLangID);
	m_wResourcesLangID = (WORD)wResourcesLangID;
}

BOOL CXTPResourceManagerXML::LoadString(CString* pString, UINT nIDResource)
{
	if (!m_bValid || m_pResourceRoot == NULL)
		return CXTPResourceManager::LoadString(pString, nIDResource);

	CString strPattern;
	strPattern.Format(_T("string[@id = \"%i\"]"), nIDResource);

	if (!m_pResourceRoot->IsSectionExists(strPattern))
		return CXTPResourceManager::LoadString(pString, nIDResource);


	CXTPPropExchangeSection secString(m_pResourceRoot->GetSection(strPattern));
	PX_String(&secString, _T("value"), *pString);

	return TRUE;
}

struct CXTPResourceManagerXML::MENUITEMTEMPLATEINFO : public MENUITEMTEMPLATE
{
	BSTR lpszCaption;

public:
	MENUITEMTEMPLATEINFO()
	{
		mtOption = 0;
		lpszCaption = NULL;
	}
	~MENUITEMTEMPLATEINFO()
	{
		if (lpszCaption)
		{
			SysFreeString(lpszCaption);
		}
	}
	UINT GetLength() const
	{
		return sizeof(mtOption) + sizeof(WCHAR) * ((int)wcslen(lpszCaption) + 1) +
			(mtOption & MF_POPUP ? 0 : sizeof(mtID));
	}
};

struct CXTPResourceManagerXML::MENUTEMPLATEINFO
{
	CArray<MENUITEMTEMPLATEINFO*, MENUITEMTEMPLATEINFO*> aItems;

public:
	~MENUTEMPLATEINFO()
	{
		for (int i = 0; i < (int)aItems.GetSize(); i++)
		{
			delete aItems[i];
		}
	}
	UINT GetLength() const
	{
		int nLength = sizeof(MENUITEMTEMPLATEHEADER);

		for (int i = 0; i < aItems.GetSize(); i++)
		{
			nLength += aItems[i]->GetLength();
		}

		return nLength;
	}
};

/////////////////////////////////////////////////////////////////////////////
// Extended dialog templates (new in Win95)

#pragma pack(push, 1)

struct CXTPResourceManagerXML::DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
};

struct CXTPResourceManagerXML::DLGITEMTEMPLATEEX
{
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	DWORD id;
};

#pragma pack(pop)


struct CXTPResourceManagerXML::DLGITEMTEMPLATEINFO : public DLGITEMTEMPLATEEX
{
	BSTR lpszCaption;
	CString strClassName;
	int nCaptionID;

public:
	DLGITEMTEMPLATEINFO()
	{
		lpszCaption = NULL;
		nCaptionID = 0;
	}
	~DLGITEMTEMPLATEINFO()
	{
		if (lpszCaption)
		{
			SysFreeString(lpszCaption);
		}
	}
	ULONG AlignDWord(ULONG uLong) const
	{
		return ((uLong + 3) & ~3);
	}

	UINT GetLength() const
	{
		return AlignDWord(sizeof(DLGITEMTEMPLATEEX)
			+ ((nCaptionID != 0 ? 1 : (int)wcslen(lpszCaption)) * sizeof(WORD)) +  sizeof(WORD)
			+ strClassName.GetLength() * sizeof(WORD) +  sizeof(WORD)
			+ 0
			+ sizeof(WORD)
		);
	}
};

struct CXTPResourceManagerXML::DLGTEMPLATEINFO : public DLGTEMPLATEEX
{
	BSTR lpszCaption;
	CString strFaceName;
	int nPointSize;

	CArray<DLGITEMTEMPLATEINFO*, DLGITEMTEMPLATEINFO*> aItems;

public:
	DLGTEMPLATEINFO()
	{
		lpszCaption = NULL;
	}
	~DLGTEMPLATEINFO()
	{
		if (lpszCaption)
		{
			SysFreeString(lpszCaption);
		}
		for (int i = 0; i < (int)aItems.GetSize(); i++)
		{
			delete aItems[i];
		}
	}

	ULONG AlignDWord(ULONG uLong) const
	{
		return ((uLong + 3) & ~3);
	}

	UINT GetHeaderLength() const
	{
		return AlignDWord(sizeof(DLGTEMPLATEEX)
			+ sizeof(WORD)
			+ sizeof(WORD)
			+ ((int)wcslen(lpszCaption) * sizeof(WORD) +  sizeof(WORD))
			+ (style & DS_SETFONT ? strFaceName.GetLength() * sizeof(WORD) + sizeof(WORD)
			+ sizeof(WORD) + sizeof(WORD) + sizeof(BYTE) + sizeof(BYTE)
			: 0)
		);
	}

	UINT GetLength() const
	{
		int nLength = GetHeaderLength();

		for (int i = 0; i < aItems.GetSize(); i++)
		{
			nLength += aItems[i]->GetLength();
		}

		return nLength;
	}
};

void CXTPResourceManagerXML::AddMenuItems(CXTPPropExchange* pPX, MENUTEMPLATEINFO* pItems)
{
	CXTPPropExchangeEnumeratorPtr enumerator(pPX->GetEnumerator(_T("menuitem")));
	POSITION pos = enumerator->GetPosition();

	while (pos)
	{
		CXTPPropExchangeSection sec(enumerator->GetNext(pos));
		int id = 0;

		MENUITEMTEMPLATEINFO* pItemInfo = new MENUITEMTEMPLATEINFO;
		PX_Int(&sec, _T("id"), id, 0);
		PX_Bstr(&sec, _T("caption"), pItemInfo->lpszCaption, L"");

		pItemInfo->mtID = (WORD)id;

		pItems->aItems.Add(pItemInfo);

		CXTPPropExchangeEnumeratorPtr enumeratorChilds(sec->GetEnumerator(_T("menuitem")));

		if (enumeratorChilds->GetPosition())
		{
			pItemInfo->mtOption |= MF_POPUP;
			AddMenuItems(&sec, pItems);
		}
		else if (id == 0)
		{
			pItemInfo->mtOption |= MF_SEPARATOR;
		}

		if (!pos)
		{
			pItemInfo->mtOption |= MF_END;
		}
	}
}

BOOL CXTPResourceManagerXML::CreateMenu(CMenu& menu, CXTPPropExchange* pPX)
{
	MENUTEMPLATEINFO menuTemplate;

	AddMenuItems(pPX, &menuTemplate);

	if (menuTemplate.aItems.GetSize() == 0)
		return FALSE;

	ASSERT(menuTemplate.aItems[menuTemplate.aItems.GetSize() - 1]->mtOption & MF_END);

	int nLength = menuTemplate.GetLength() + sizeof(MENUITEMTEMPLATE);
	LPVOID lpDlgTemplate = malloc(nLength);
	ZeroMemory(lpDlgTemplate, nLength);

	MENUITEMTEMPLATE* mitem = (MENUITEMTEMPLATE*) ((BYTE*)lpDlgTemplate + sizeof(MENUITEMTEMPLATEHEADER));

	for (int i = 0; i < menuTemplate.aItems.GetSize(); i++)
	{
		MENUITEMTEMPLATEINFO& itemInfo = *menuTemplate.aItems[i];
		mitem->mtOption = itemInfo.mtOption;

		if (itemInfo.mtOption & MF_POPUP)
		{
			MEMCPY_S((BYTE*)mitem + sizeof(mitem->mtOption), itemInfo.lpszCaption, (wcslen(itemInfo.lpszCaption) + 1) * sizeof(WORD));
		}
		else
		{
			mitem->mtID = itemInfo.mtID;
			MEMCPY_S(mitem->mtString, itemInfo.lpszCaption, (wcslen(itemInfo.lpszCaption) + 1) * sizeof(WORD));
		}

		mitem = (MENUITEMTEMPLATE*) ((BYTE*)mitem + itemInfo.GetLength());
	}
	mitem->mtOption = MF_END;

	BOOL bResult = menu.LoadMenuIndirect(lpDlgTemplate);

	free(lpDlgTemplate);

	return bResult;
}

BOOL CXTPResourceManagerXML::LoadMenu(CMenu* lpMenu, UINT nIDResource)
{
	if (!m_bValid || m_pResourceRoot == NULL)
		return CXTPResourceManager::LoadMenu(lpMenu, nIDResource);

	CString strPattern;
	strPattern.Format(_T("menu[@id = \"%i\"]"), nIDResource);

	if (!m_pResourceRoot->IsSectionExists(strPattern))
		return CXTPResourceManager::LoadMenu(lpMenu, nIDResource);

	CXTPPropExchangeSection secMenu(m_pResourceRoot->GetSection(strPattern));

	if (CreateMenu(*lpMenu, &secMenu))
		return TRUE;

	return CXTPResourceManager::LoadMenu(lpMenu, nIDResource);
}

//////////////////////////////////////////////////////////////////////////
// Utils


LPDLGTEMPLATE CXTPResourceManagerXML::CreateDialogTemplate(DLGTEMPLATEINFO& dlgTemplate)
{
	int nDlgLength = dlgTemplate.GetLength();

	DLGTEMPLATEEX* lpDlgTemplate = (DLGTEMPLATEEX*)malloc(nDlgLength);
	if (lpDlgTemplate == NULL)
		return NULL;

	ZeroMemory(lpDlgTemplate, nDlgLength);
	*lpDlgTemplate = dlgTemplate;

	BYTE* pMain = (BYTE*) lpDlgTemplate + sizeof(DLGTEMPLATEEX) + sizeof(WORD) * 2;

	// Caption
	ASSERT(dlgTemplate.lpszCaption != 0);
	size_t nLength = (wcslen(dlgTemplate.lpszCaption) + 1) * sizeof(WORD);
	MEMCPY_S(pMain, dlgTemplate.lpszCaption, nLength);
	pMain += nLength;

	if (dlgTemplate.style & DS_SETFONT)
	{
		*((WORD *)pMain) = (WORD)dlgTemplate.nPointSize;
		pMain += sizeof(WORD);

		pMain += sizeof(WORD) + sizeof(BYTE) + sizeof(BYTE);

		MBSTOWCS_S((LPWSTR)pMain, dlgTemplate.strFaceName, dlgTemplate.strFaceName.GetLength() + 1);
	}

	pMain = (BYTE*) lpDlgTemplate + dlgTemplate.GetHeaderLength();
	ASSERT(dlgTemplate.cDlgItems == dlgTemplate.aItems.GetSize());


	CArray<DLGITEMTEMPLATEINFO*, DLGITEMTEMPLATEINFO*>& aItems = dlgTemplate.aItems;

	for (int i = 0; i < aItems.GetSize(); i++)
	{
		DLGITEMTEMPLATEINFO& itemInfo = *aItems[i];

		// Copy constant part of the template
		*(DLGITEMTEMPLATEEX*)pMain = itemInfo;
		BYTE* pControlMain = pMain + sizeof(DLGITEMTEMPLATEEX);

		// Class
		MBSTOWCS_S((LPWSTR)pControlMain, itemInfo.strClassName, itemInfo.strClassName.GetLength() + 1);
		pControlMain += (itemInfo.strClassName.GetLength() + 1) * sizeof(WORD);


		// Copy Caption
		if (itemInfo.nCaptionID != 0)
		{
			*((PWORD)pControlMain) = 0xFFFF;
			*((PWORD)pControlMain + 1) = (WORD)itemInfo.nCaptionID;

			pControlMain += sizeof(WORD) * 2;
		}
		else
		{
			ASSERT(itemInfo.lpszCaption != 0);
			nLength = (wcslen(itemInfo.lpszCaption) + 1) * sizeof(WORD);
			MEMCPY_S(pControlMain, itemInfo.lpszCaption, nLength);
			pControlMain += nLength;

		}


		// Init Data length
		*((WORD *)pControlMain) = 0;
		pControlMain += sizeof(WORD);

		pMain += itemInfo.GetLength();
	}

	return (LPDLGTEMPLATE)lpDlgTemplate;
}


LPDLGTEMPLATE CXTPResourceManagerXML::CreateDialogTemplate(CXTPPropExchange* pPX)
{
	DLGTEMPLATEINFO dlgTemplate;

	CString strPos;
	CRect rc;
	DWORD dwStyle = 0, dwExtendedStyle = 0, id;

	PX_DWord(pPX, _T("style"), dwStyle, 0);
	PX_DWord(pPX, _T("styleex"), dwExtendedStyle, 0);
	PX_Rect(pPX, _T("position"), rc, CRect(0, 0, 0, 0));
	PX_String(pPX, _T("fontface"), dlgTemplate.strFaceName, _T("MS Sans Serif"));
	PX_Int(pPX, _T("fontsize"), dlgTemplate.nPointSize, 8);
	PX_Bstr(pPX, _T("caption"), dlgTemplate.lpszCaption, L"");
	PX_DWord(pPX, _T("id"), id, 0);

	dlgTemplate.x = (short)rc.left;
	dlgTemplate.y = (short)rc.top;
	dlgTemplate.cx = (short)rc.right;
	dlgTemplate.cy = (short)rc.bottom;
	dlgTemplate.style = dwStyle;
	dlgTemplate.exStyle = dwExtendedStyle;
	dlgTemplate.helpID = id;

	dlgTemplate.cDlgItems = 0;
	dlgTemplate.style |= DS_SETFONT;
	dlgTemplate.dlgVer = 1;
	dlgTemplate.signature = 0xFFFF;


	CArray<DLGITEMTEMPLATEINFO*, DLGITEMTEMPLATEINFO*>& aItems = dlgTemplate.aItems;

	CXTPPropExchangeEnumeratorPtr enumerator(pPX->GetEnumerator(_T("control")));
	POSITION pos = enumerator->GetPosition();

	while (pos)
	{
		CXTPPropExchangeSection sec(enumerator->GetNext(pos));

		DLGITEMTEMPLATEINFO* pItemInfo = new DLGITEMTEMPLATEINFO;

		PX_DWord(&sec, _T("style"), dwStyle, WS_CHILD | WS_VISIBLE | WS_GROUP);
		PX_DWord(&sec, _T("styleex"), dwExtendedStyle, 0);
		PX_Rect(&sec, _T("position"), rc, CRect(0, 0, 0, 0));
		PX_Bstr(&sec, _T("caption"), pItemInfo->lpszCaption, L"");
		PX_String(&sec, _T("class"), pItemInfo->strClassName, _T("STATIC"));
		PX_DWord(&sec, _T("id"), id, (DWORD)IDC_STATIC);
		PX_Int(&sec, _T("resource"), pItemInfo->nCaptionID, 0);

		pItemInfo->x = (short)rc.left;
		pItemInfo->y = (short)rc.top;
		pItemInfo->cx = (short)rc.right;
		pItemInfo->cy = (short)rc.bottom;
		pItemInfo->style = dwStyle;
		pItemInfo->exStyle = dwExtendedStyle;
		pItemInfo->helpID = 0;
		pItemInfo->id = id;

		aItems.Add(pItemInfo);
		dlgTemplate.cDlgItems++;
	}


	return CreateDialogTemplate(dlgTemplate);
}

LPCDLGTEMPLATE CXTPResourceManagerXML::LoadDialogTemplate(UINT nIDResource)
{
	if (!m_bValid || m_pResourceRoot == NULL)
		return CXTPResourceManager::LoadDialogTemplate(nIDResource);

	CString strPattern;
	strPattern.Format(_T("dialog[@id = \"%i\"]"), nIDResource);

	DLGTEMPLATE* pTemplate = 0;
	if (m_mapDialogs.Lookup(nIDResource, (DLGTEMPLATE*&)pTemplate))
		return pTemplate;

	if (!m_pResourceRoot->IsSectionExists(strPattern))
		return CXTPResourceManager::LoadDialogTemplate(nIDResource);

	CXTPPropExchangeSection secDialog(m_pResourceRoot->GetSection(strPattern));

	pTemplate = CreateDialogTemplate(&secDialog);

	if (pTemplate)
	{
		m_mapDialogs.SetAt(nIDResource, pTemplate);
		return pTemplate;
	}

	return CXTPResourceManager::LoadDialogTemplate(nIDResource);
}


#endif
