// XTPSyntaxEditLexColorFileReader.cpp: implementation of the CXTLexColorFileReader class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// common includes
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"

// syntax editor includes
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditUndoManager.h"
#include "XTPSyntaxEditLineMarksManager.h"
#include "XTPSyntaxEditLexPtrs.h"
#include "XTPSyntaxEditLexClassSubObjT.h"
#include "XTPSyntaxEditTextIterator.h"
#include "XTPSyntaxEditLexCfgFileReader.h"
#include "XTPSyntaxEditLexClassSubObjDef.h"
#include "XTPSyntaxEditLexClass.h"
#include "XTPSyntaxEditLexParser.h"
#include "XTPSyntaxEditSectionManager.h"
#include "XTPSyntaxEditLexColorFileReader.h"
#include "XTPSyntaxEditBufferManager.h"
#include "XTPSyntaxEditToolTipCtrl.h"
#include "XTPSyntaxEditAutoCompleteWnd.h"
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditSectionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_EMPTY_STRING        _T("")

namespace XTPSyntaxEditLexAnalyser
{
	extern void AddIfNeed_noCase(CStringArray& rarData, LPCTSTR strNew);
	extern BOOL IsEventSet(HANDLE hEvent);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace XTPSyntaxEditLexAnalyser;

CXTPSyntaxEditColorInfo::CXTPSyntaxEditColorInfo(CXTPSyntaxEditColorTheme* pTheme) :
	m_pTheme(pTheme)
{
}

CXTPSyntaxEditColorInfo::CXTPSyntaxEditColorInfo(const CString& strClassName,
											   CXTPSyntaxEditColorTheme* pTheme) :
	m_strClassName(strClassName), m_pTheme(pTheme)
{
}

CXTPSyntaxEditColorInfo::~CXTPSyntaxEditColorInfo ()
{
}

void CXTPSyntaxEditColorInfo::AddParam(const CString& strName, const CString& strValue)
{
	CString strLowerName(strName);
	strLowerName.MakeLower();
	// save param-value pair
	m_mapParams[strLowerName] = strValue;
}

const CString CXTPSyntaxEditColorInfo::GetParam(const CString& strName,
												 BOOL bDynamic)
{
	CString strLowerName(strName);
	strLowerName.MakeLower();

	CString strValue;
	if (!m_mapParams.Lookup(strLowerName, strValue) && bDynamic)
	{
		// do not search parent for main section parameters
		if (!m_strClassName.CompareNoCase(XTP_EDIT_LEXPARSER_SECTION_MAIN))
		{
			return strValue;
		}
		// try to get value from the parent schema
		CXTPSyntaxEditColorTheme* pParentTheme = m_pTheme ? m_pTheme->GetParentTheme() : NULL;
		if (m_pTheme && pParentTheme)
		{
			CXTPSyntaxEditColorInfo* pParentColorInfo = pParentTheme->GetColorInfo(m_strClassName, m_pTheme->GetFileName());
			if (pParentColorInfo)
			{
				strValue = pParentColorInfo->GetParam(strName);
			}
		}
	}
	return strValue;
}

DWORD CXTPSyntaxEditColorInfo::GetHexParam(const CString& strName, BOOL bDynamic)
{
	// get hex value if exists
	DWORD dwHex = 0;
	SCANF_S(GetParam(strName, bDynamic), _T("%x"), &dwHex);
	return dwHex;
}

POSITION CXTPSyntaxEditColorInfo::GetFirstParamNamePosition()
{
	return m_mapParams.GetStartPosition();
}

const CString CXTPSyntaxEditColorInfo::GetNextParamName(POSITION& pos)
{
	CString strName, strValue;
	m_mapParams.GetNextAssoc(pos, strName, strValue);

	return strName;
}
/////////////////////////////////////////////////////////////////////////////
//

CXTPSyntaxEditColorTheme::CXTPSyntaxEditColorTheme(CXTPSyntaxEditColorThemesManager* pThemesManager) :
	m_pThemesManager(pThemesManager)
{
}

CXTPSyntaxEditColorTheme::~CXTPSyntaxEditColorTheme()
{
	Cleanup();
}

void CXTPSyntaxEditColorTheme::Cleanup()
{
	// cleanup color info map
	POSITION pos = m_mapLexColorInfo.GetStartPosition();
	CString strKey;
	CXTPSyntaxEditColorInfo* pInf = NULL;
	while (pos != NULL)
	{
		m_mapLexColorInfo.GetNextAssoc(pos, strKey, pInf);
		if (pInf)
			delete pInf;
	}
	m_mapLexColorInfo.RemoveAll();
}

void CXTPSyntaxEditColorTheme::Load(const CString& csFileName)
{
	Cleanup();
	m_csFileName = csFileName;

	// get sections list
	CStringArray arSections;
	CXTPSyntaxEditSectionManager().GetSectionNames(arSections, m_csFileName);

	if (arSections.GetSize() == 0)
		return;

	for (int i = 0; i < arSections.GetSize(); ++i)
	{
		CString csSection = arSections[i];
		csSection.MakeLower();

		CXTPSyntaxEditColorInfo* pOld = m_mapLexColorInfo[csSection];
		if (pOld)
		{
			delete pOld;
		}
		m_mapLexColorInfo[csSection] = ParseSection(csSection);
	}
}

BOOL CXTPSyntaxEditColorTheme::WriteCfgFile(CString strParentThemeName,
								CXTPSyntaxEditColorInfoArray* pColorInfoArray_new)
{
	if (!pColorInfoArray_new)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	CString strFileName = GetFileName();
	CString strFileName_new = strFileName + _T(".tmp");

	::SetFileAttributes(strFileName_new, FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(strFileName_new);

	if (!::CopyFile(strFileName, strFileName_new,  FALSE))
	{
		TRACE(_T("ERROR! CXTPSyntaxEditColorTheme::WriteCfgFile() - Cannot copy file '%s' to '%s' \n"), (LPCTSTR)strFileName, (LPCTSTR)strFileName_new);
		return FALSE;
	}

	CMapStringToPtr mapSaved_ClsProp;
	const CString cstrCNDelim = _T("-xtpEdit_ColorThemeClassPropDelimeter_xtpEdit-");
	void* pVoid = NULL;

	//---------------------------------------------------------------------------

	// (0) Update main section - parent schema property
	LPCTSTR pcszString = NULL;
	if (!strParentThemeName.IsEmpty())
	{
		pcszString = strParentThemeName;
	}
	BOOL bRes = ::WritePrivateProfileString(XTP_EDIT_LEXPARSER_SECTION_MAIN, XTP_EDIT_LEXPARSER_PARENT_SCHEMA,
					pcszString, strFileName_new);

	CString strSavedKey = XTP_EDIT_LEXPARSER_SECTION_MAIN + cstrCNDelim + XTP_EDIT_LEXPARSER_PARENT_SCHEMA;
	strSavedKey.MakeLower();
	ASSERT(mapSaved_ClsProp.Lookup(strSavedKey, pVoid) == FALSE);
	mapSaved_ClsProp.SetAt(strSavedKey, NULL);
	BOOL bClassPropRepeated_DbgAssertWas = FALSE;

	if (!bRes)
	{
		return FALSE;
	}

	// (1) Update Existing and add new properties
	int nCount = (int)pColorInfoArray_new->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditColorInfo* pClrInf_new = pColorInfoArray_new->GetAt(i);
		if (!pClrInf_new)
		{
			ASSERT(FALSE);
			continue;
		}
		CString csSection = pClrInf_new->GetClassName();

		POSITION posParam = pClrInf_new->GetFirstParamNamePosition();
		while (posParam)
		{
			CString strPrmName = pClrInf_new->GetNextParamName(posParam);
			CString strPrmValue = pClrInf_new->GetParam(strPrmName,  FALSE);

			if (!strPrmName.IsEmpty() && !strPrmValue.IsEmpty())
			{
				bRes = ::WritePrivateProfileString(csSection, strPrmName,
														strPrmValue, strFileName_new);
				if (!bRes)
				{
					return FALSE;
				}
				strSavedKey = csSection + cstrCNDelim + strPrmName;
				strSavedKey.MakeLower();

				if (mapSaved_ClsProp.Lookup(strSavedKey, pVoid))
				{
					if (!bClassPropRepeated_DbgAssertWas)
					{
						ASSERT(FALSE);
						bClassPropRepeated_DbgAssertWas = TRUE;
					}
					TRACE(_T("WARNING! Color theme class property repeated: %s->%s. \n"), (LPCTSTR)csSection, (LPCTSTR)strPrmName);
				}

				mapSaved_ClsProp.SetAt(strSavedKey, NULL);
			}
		}
	}

	//===========================================================================
	// (2) Remove old properties
	POSITION posClrInfo_old = m_mapLexColorInfo.GetStartPosition();
	CString strClassName;
	CXTPSyntaxEditColorInfo* pClrInf_old = NULL;
	while (posClrInfo_old)
	{
		m_mapLexColorInfo.GetNextAssoc(posClrInfo_old, strClassName, pClrInf_old);

		if (!pClrInf_old)
		{
			ASSERT(FALSE);
			continue;
		}

		CString csSection = pClrInf_old->GetClassName();

		POSITION posParam = pClrInf_old->GetFirstParamNamePosition();
		while (posParam)
		{
			CString strPrmName = pClrInf_old->GetNextParamName(posParam);

			strSavedKey = csSection + cstrCNDelim + strPrmName;
			strSavedKey.MakeLower();

			if (!mapSaved_ClsProp.Lookup(strSavedKey, pVoid))
			{
				VERIFY( ::WritePrivateProfileString(csSection, strPrmName,
													NULL, strFileName_new) );
			}
		}
	}

	//===========================================================================
	CString strFileName_prev = strFileName + _T(".old");
	::SetFileAttributes(strFileName_prev, FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(strFileName_prev);

#ifdef _DEBUG
	//::MoveFile(strFileName, strFileName_prev);
#endif

	::SetFileAttributes(strFileName, FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(strFileName);

	if (!::MoveFile(strFileName_new, strFileName))
	{
		TRACE(_T("ERROR! CXTPSyntaxEditColorTheme::WriteCfgFile() - Cannot rename file '%s' to '%s' \n"), (LPCTSTR)strFileName_new, (LPCTSTR)strFileName);
		return FALSE;
	}

	return TRUE;
}

CXTPSyntaxEditColorInfo* CXTPSyntaxEditColorTheme::ParseSection(const CString& csSection)
{
	CXTPSyntaxEditColorInfo* pColorInfo = new CXTPSyntaxEditColorInfo(csSection, this);

	CXTPSyntaxEditSchemaFileInfoList infoList;
	CXTPSyntaxEditSectionManager().GetSectionKeyList(infoList, m_csFileName, csSection);

	for (POSITION pos = infoList.GetHeadPosition(); pos;)
	{
		XTP_EDIT_SCHEMAFILEINFO& info = infoList.GetNext(pos);
		pColorInfo->AddParam(info.csName, info.csValue);
	}

	return pColorInfo;
}

const CString CXTPSyntaxEditColorTheme::GetParentThemeName()
{
	CXTPSyntaxEditColorInfo* pInfo = NULL;
	if (m_mapLexColorInfo.Lookup(XTP_EDIT_LEXPARSER_SECTION_MAIN, pInfo) && pInfo)
	{
		return pInfo->GetParam(XTP_EDIT_LEXPARSER_PARENT_SCHEMA);
	}
	return _T("");
}

CXTPSyntaxEditColorTheme* CXTPSyntaxEditColorTheme::GetParentTheme()
{
	return m_pThemesManager ? m_pThemesManager->GetTheme(GetParentThemeName()) : NULL;
}

CXTPSyntaxEditColorInfo* CXTPSyntaxEditColorTheme::GetColorInfo(
								const CString& strLexClass,
								const CString& strThemeFilename,
								BOOL bDynamic)
{
	CXTPSyntaxEditColorInfo* pInfo = NULL;
	CString strLexClass_lower = strLexClass;
	strLexClass_lower.MakeLower();
	if (!m_mapLexColorInfo.Lookup(strLexClass_lower, pInfo) && bDynamic)
	{
		// lookup color info on parent themes
		CXTPSyntaxEditColorTheme* pParentTheme = GetParentTheme();
		if (pParentTheme && pParentTheme->GetFileName().CompareNoCase(strThemeFilename))
		{
			pInfo = pParentTheme->GetColorInfo(strLexClass_lower, strThemeFilename, TRUE);
		}
	}
	return pInfo;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditColorThemesManager

CXTPSyntaxEditColorThemesManager::CXTPSyntaxEditColorThemesManager()
{
}

CXTPSyntaxEditColorThemesManager::~CXTPSyntaxEditColorThemesManager()
{
	RemoveAll();
}

void CXTPSyntaxEditColorThemesManager::LoadTheme(const CString& strThemeName,
												const CString& strThemeFilename)
{
	// delete old theme
	CXTPSyntaxEditColorTheme* pTheme = NULL;
	if (m_mapThemes.Lookup(strThemeName, pTheme) && pTheme)
	{
		delete pTheme;
		// delete its name
		for (int i = 0; i < m_arThemeNames.GetSize(); i++)
		{
			if (!m_arThemeNames.GetAt(i).CompareNoCase(strThemeName))
			{
				m_arThemeNames.RemoveAt(i);
				break;
			}
		}
	}

	// create and load new theme
	pTheme = new CXTPSyntaxEditColorTheme(this);
	pTheme->Load(strThemeFilename);

	m_mapThemes[strThemeName] = pTheme;
	m_arThemeNames.Add(strThemeName);

	CString strTFNameLower = strThemeFilename;
	strTFNameLower.MakeLower();
	m_mapFileToTheme[strTFNameLower] = strThemeName;
}

void CXTPSyntaxEditColorThemesManager::AddThemeInfo(const CString& strThemeName,
												   const CString& strThemeFilename)
{
	CString strTFNameLower = strThemeFilename;
	strTFNameLower.MakeLower();

	m_mapFileToTheme[strTFNameLower] = strThemeName;
}

CXTPSyntaxEditColorTheme* CXTPSyntaxEditColorThemesManager::GetTheme(const CString& strThemeName)
{
	CXTPSyntaxEditColorTheme* pTheme = NULL;
	m_mapThemes.Lookup(strThemeName, pTheme);
	return pTheme;
}

CStringArray& CXTPSyntaxEditColorThemesManager::GetThemes()
{
	return m_arThemeNames;
}

CString CXTPSyntaxEditColorThemesManager::ReloadFile(const CString& csFileName,
													int nCfgFlags)
{
	CString strThemeName;
	// iterate all themes
	POSITION pos = m_mapThemes.GetStartPosition();
	CString strKey;
	CXTPSyntaxEditColorTheme* pTheme = NULL;
	while (pos != NULL)
	{
		m_mapThemes.GetNextAssoc(pos, strKey, pTheme);
		if (pTheme && !pTheme->GetFileName().CompareNoCase(csFileName))
		{
			strThemeName = strKey;
			if (nCfgFlags & xtpEditCfgFileRemove)
			{
				delete pTheme;
				m_mapThemes.RemoveKey(strKey);
			}
			else
			{
				pTheme->Load(csFileName);
			}
			return strThemeName;
		}
	}

	//------------------------------------------------------------------------
	if (nCfgFlags & xtpEditCfgFileAdd)
	{
		CString strTFileLower = csFileName;
		strTFileLower.MakeLower();

		if (m_mapFileToTheme.Lookup(strTFileLower, strThemeName))
		{
			LoadTheme(strThemeName, csFileName);
		}
		else
		{
			strThemeName.Empty();
			//ASSERT(FALSE);
		}
	}

	//------------------------------------------------------------------------
	return strThemeName;
}

void CXTPSyntaxEditColorThemesManager::RemoveAll()
{
	m_arThemeNames.RemoveAll();
	m_mapFileToTheme.RemoveAll();

	// cleanup themes map
	POSITION pos = m_mapThemes.GetStartPosition();
	CString strKey;
	CXTPSyntaxEditColorTheme* pTheme = NULL;
	while (pos != NULL)
	{
		m_mapThemes.GetNextAssoc(pos, strKey, pTheme);
		if (pTheme)
			delete pTheme;
	}
	m_mapThemes.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditTextSchemesManager

CXTPSyntaxEditTextSchemesManager::CXTPSyntaxEditTextSchemesManager()
{
}

CXTPSyntaxEditTextSchemesManager::~CXTPSyntaxEditTextSchemesManager()
{
}

void CXTPSyntaxEditTextSchemesManager::LoadTextScheme(XTP_EDIT_SCHEMAFILEINFO& info)
{
	CString strSchemeNameLower(info.csName);
	strSchemeNameLower.MakeLower();

	CSingleLock lockReadFile(&m_csReadFile,  TRUE);

	if (info.uValue != (UINT)-1)
		XTPSyntaxEditLexConfig()->ReadSource(info.uValue);
	else
		XTPSyntaxEditLexConfig()->ReadSource(info.csValue, TRUE);

	CXTPSyntaxEditLexClassInfoArray& arLexClassInfo = XTPSyntaxEditLexConfig()->GetLexClassInfoArray();
	CXTPSyntaxEditTextSchemaPtr ptrTxtSch(new CXTPSyntaxEditTextSchema(info.csName));
	if (ptrTxtSch)
	{
		ptrTxtSch->LoadClassSchema(arLexClassInfo);

		m_mapSchemes[strSchemeNameLower] = ptrTxtSch;

		AddTextSchemeInfo(info);
	}
}

void CXTPSyntaxEditTextSchemesManager::AddTextSchemeInfo(XTP_EDIT_SCHEMAFILEINFO& info)
{
	if (GetSchemaFileName(info.csName).IsEmpty())
		m_listSchemes.AddHead(info);
	XTPSyntaxEditLexAnalyser::AddIfNeed_noCase(m_arSchemeNames, info.csName);
}

CXTPSyntaxEditTextSchemaPtr CXTPSyntaxEditTextSchemesManager::GetSchema(const CString& strSchemeName)
{
	CString strSchemeNameLower(strSchemeName);
	strSchemeNameLower.MakeLower();

	CXTPSyntaxEditTextSchemaPtr ptrScheme;
	m_mapSchemes.Lookup(strSchemeNameLower, ptrScheme);
	return ptrScheme;
}

CXTPSyntaxEditTextSchemaPtr CXTPSyntaxEditTextSchemesManager::FindSchema(const CString& strFileExt)
{
	// iterate all text schemes
	CString strSchemaName;
	CXTPSyntaxEditTextSchemaPtr ptrScheme;

	if (!strFileExt.IsEmpty())
	{
		POSITION pos = m_mapSchemes.GetStartPosition();
		while (pos != NULL)
		{
			m_mapSchemes.GetNextAssoc(pos, strSchemaName, ptrScheme);
			if (ptrScheme && ptrScheme->IsFileExtSupported(strFileExt))
			{
				return ptrScheme;
			}
		}
	}
	return NULL;
}

void CXTPSyntaxEditTextSchemesManager::SetTheme(CXTPSyntaxEditColorTheme* pTheme)
{
	// iterate all text schemes
	CString strSchemaName;
	CXTPSyntaxEditTextSchemaPtr ptrScheme;
	POSITION pos = m_mapSchemes.GetStartPosition();
	while (pos != NULL)
	{
		m_mapSchemes.GetNextAssoc(pos, strSchemaName, ptrScheme);
		if (ptrScheme)
		{
			ptrScheme->ApplyTheme(pTheme);
		}
	}
}

const CString CXTPSyntaxEditTextSchemesManager::GetSchemaName(const CString& csFileName)
{
	XTP_EDIT_SCHEMAFILEINFO info;
	if (m_listSchemes.LookupValue(csFileName, info))
	{
		return info.csName;
	}
	return _T("");
}

const CString CXTPSyntaxEditTextSchemesManager::GetSchemaFileName(const CString& strSchemeName)
{
	XTP_EDIT_SCHEMAFILEINFO info;
	if (m_listSchemes.LookupName(strSchemeName, info))
	{
		return info.csValue;
	}

	return _T("");
}

BOOL CXTPSyntaxEditTextSchemesManager::ReloadFile(const CString& csFileName, int nCfgFlags)
{
	XTP_EDIT_SCHEMAFILEINFO info;
	if (m_listSchemes.LookupValue(csFileName, info))
	{
		CString csSchemaName = info.csName;
		csSchemaName.MakeLower();
		m_mapSchemes.RemoveKey(csSchemaName);

		if ((nCfgFlags & xtpEditCfgFileRemove) == 0)
		{
			LoadTextScheme(info);
		}

		return TRUE;
	}

	return FALSE;
}

void CXTPSyntaxEditTextSchemesManager::RemoveAll()
{
	m_mapSchemes.RemoveAll();
	m_listSchemes.RemoveAll();
	m_arSchemeNames.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditConfigurationManager

CXTPSyntaxEditConfigurationManager::CXTPSyntaxEditConfigurationManager()
{
	m_pConnectMT = new CXTPNotifyConnectionMT();
	m_hReloadThread = NULL;
	m_pBreakReloadEvent = NULL;

	m_strCurrentThemeName = XTP_EDIT_LEXPARSER_DEFTHEME;
	m_FolderMonitor.SetConfigurationManager(this);

}

CXTPSyntaxEditConfigurationManager::~CXTPSyntaxEditConfigurationManager()
{
	Close();

	CMDTARGET_RELEASE(m_pConnectMT);
}

void CXTPSyntaxEditConfigurationManager::Close()
{
	m_FolderMonitor.StopMonitoring();

	if (m_hReloadThread)
	{
		ASSERT(m_pBreakReloadEvent);
		if (m_pBreakReloadEvent)
		{
			m_pBreakReloadEvent->SetEvent();
		}
		DWORD dwThreadRes = ::WaitForSingleObject(m_hReloadThread, 30*1000);

		if (dwThreadRes == WAIT_TIMEOUT)
		{
			::TerminateThread(m_hReloadThread, 0);
			TRACE(_T("ERROR! Configuration Manager reload thread was not ended by normal way. It was terminated. \n"));
		}
	}
	m_hReloadThread = NULL;

	if (m_pBreakReloadEvent)
	{
		delete m_pBreakReloadEvent;
		m_pBreakReloadEvent = NULL;
	}

	m_FolderMonitor.StopMonitoring();

	m_ColorThemeManager.RemoveAll();
	m_TextSchemesManager.RemoveAll();
}

CString CXTPSyntaxEditConfigurationManager::GetConfigFile() const
{
	return m_strMainIniFilename;
}

void CXTPSyntaxEditConfigurationManager::ReloadConfig(const CString& strConfigFilePath)
{
	CSingleLock singleLock(&m_DataLockerCS, TRUE);

	m_strMainIniFilename = strConfigFilePath;
	ReloadConfig();
}

void CXTPSyntaxEditConfigurationManager::ReloadConfig()
{
	CSingleLock singleLock(&m_DataLockerCS, TRUE);

	TRACE(_T("LOAD/reload configuration: %s \n"), (LPCTSTR)m_strMainIniFilename);
	// restart folder monitor
	m_FolderMonitor.StopMonitoring();

	// Remove previous data
	m_ColorThemeManager.RemoveAll();
	m_TextSchemesManager.RemoveAll();

	if (!m_FolderMonitor.AddMonitorFile(m_strMainIniFilename, xtpEditCfgObjMainConfig))
	{
		m_pConnectMT->SendEvent(xtpEditAllConfigWasChanged, 0, 0);

		// start monitoring config files
		m_FolderMonitor.StartMonitoring();

		return;
	}

	CString strMainIniFolder;
	int nFLs = m_strMainIniFilename.ReverseFind(_T('\\'));
	if (nFLs > 0)
	{
		strMainIniFolder = m_strMainIniFilename.Mid(0, nFLs + 1);
	}

	m_FolderMonitor.SetDefaultFolder(strMainIniFolder);

	CXTPSyntaxEditSchemaFileInfoList listThemes;
	CXTPSyntaxEditSectionManager().GetSectionKeyList(listThemes, m_strMainIniFilename, XTP_EDIT_LEXPARSER_SECTION_THEMES);

	for (POSITION posThemes = listThemes.GetHeadPosition(); posThemes;)
	{
		XTP_EDIT_SCHEMAFILEINFO& info = listThemes.GetNext(posThemes);

		if (m_hReloadThread && m_pBreakReloadEvent)
		{
			if (XTPSyntaxEditLexAnalyser::IsEventSet(*m_pBreakReloadEvent))
			{
				TRACE(_T("BREAK config reloading. \n"));
				return;
			}
		}

		TRACE(_T("LOAD Theme: %s - %s\n"), (LPCTSTR)info.csName, (LPCTSTR)info.csValue);

		if (m_FolderMonitor.AddMonitorFile(info.csValue, xtpEditCfgObjThemeMan))
		{
			m_ColorThemeManager.LoadTheme(info.csName, info.csValue);
		}
		else
		{
			m_ColorThemeManager.AddThemeInfo(info.csName, info.csValue);
		}
	}

	// load all schemes config files

	CXTPSyntaxEditSchemaFileInfoList listSchemes;
	CXTPSyntaxEditSectionManager().GetSectionKeyList(listSchemes, m_strMainIniFilename, XTP_EDIT_LEXPARSER_SECTION_SCHEMES);

	for (POSITION posSchemes = listSchemes.GetHeadPosition(); posSchemes;)
	{
		XTP_EDIT_SCHEMAFILEINFO& info = listSchemes.GetNext(posSchemes);

		if (m_hReloadThread && m_pBreakReloadEvent)
		{
			if (XTPSyntaxEditLexAnalyser::IsEventSet(*m_pBreakReloadEvent))
			{
				TRACE(_T("BREAK config reloading. \n"));
				return;
			}
		}

		TRACE(_T("LOAD Scheme: %s - %s\n"), (LPCTSTR)info.csName, (LPCTSTR)info.csValue);

		if (m_FolderMonitor.AddMonitorFile(info.csValue, xtpEditCfgObjSchMan))
		{
			m_TextSchemesManager.LoadTextScheme(info);
		}
		else
		{
			m_TextSchemesManager.AddTextSchemeInfo(info);
		}
	}

	// set default theme
	SetTheme(m_strCurrentThemeName);

	//========================================================================
	if (m_hReloadThread && m_pBreakReloadEvent)
	{
		if (XTPSyntaxEditLexAnalyser::IsEventSet(*m_pBreakReloadEvent))
		{
			TRACE(_T("BREAK config reloading. \n"));
			return;
		}
	}
	//========================================================================

	m_pConnectMT->SendEvent(xtpEditAllConfigWasChanged, 0, 0);

	// start monitoring config files
	m_FolderMonitor.StartMonitoring();
}

void CXTPSyntaxEditConfigurationManager::ReloadFile(CString csFileName,
												   DWORD dwOwnerFlags,
												   int nCfgFlags)
{
	CSingleLock singleLock(&m_DataLockerCS, TRUE);

	CString strItemName;

	// if file is color theme definition
	if ((dwOwnerFlags & xtpEditCfgObjThemeMan) || dwOwnerFlags == 0)
	{
		TRACE(_T("Reload Theme: %s \n"), (LPCTSTR)csFileName);

		strItemName = m_ColorThemeManager.ReloadFile(csFileName, nCfgFlags);
		if (!strItemName.IsEmpty())
		{
			// Re-apply current theme after reloading
			if (!strItemName.CompareNoCase(m_strCurrentThemeName))
			{
				SetTheme(m_strCurrentThemeName);
			}

			CXTPSyntaxEditColorTheme* pTheme = m_ColorThemeManager.GetTheme(strItemName);
			m_pConnectMT->SendEvent(xtpEditThemeWasChanged,
									(WPARAM)(LPCTSTR)strItemName, (LPARAM)pTheme);
			return;
		}
	}

	// if file is lex text schema definition
	if ((dwOwnerFlags & xtpEditCfgObjSchMan) || dwOwnerFlags == 0)
	{
		TRACE(_T("Reload Scheme: %s \n"), (LPCTSTR)csFileName);

		if (m_TextSchemesManager.ReloadFile(csFileName, nCfgFlags))
		{
			CString strSchName = m_TextSchemesManager.GetSchemaName(csFileName);
			CXTPSyntaxEditTextSchemaPtr ptrTxtSch;
			ptrTxtSch = m_TextSchemesManager.GetSchema(strSchName);

			CXTPSyntaxEditColorTheme* pTheme = m_ColorThemeManager.GetTheme(m_strCurrentThemeName);
			if (pTheme && ptrTxtSch)
			{
				ptrTxtSch->ApplyTheme(pTheme);
			}

			m_pConnectMT->SendEvent(xtpEditClassSchWasChanged, (WPARAM)(LPCTSTR)strSchName,
									(LPARAM)(CXTPSyntaxEditTextSchema*)ptrTxtSch);
			return;
		}
	}

	// else - file is the main schema definition
	if ((dwOwnerFlags & xtpEditCfgObjMainConfig) || dwOwnerFlags == 0)
	{
		ReloadConfigAsync();
	}
}



void CXTPSyntaxEditConfigurationManager::SetTheme(const CString& strThemeName,
												 CXTPSyntaxEditTextSchema* pActiveSch)
{
	CSingleLock singleLock(&m_DataLockerCS, TRUE);

	CXTPSyntaxEditColorTheme* pTheme = m_ColorThemeManager.GetTheme(strThemeName);
	//if (!pTheme) {
	//  return;
	//}

	m_strCurrentThemeName = strThemeName;

	m_TextSchemesManager.SetTheme(pTheme);

	if (pActiveSch)
	{
		pActiveSch->ApplyTheme(pTheme);
	}
}

void CXTPSyntaxEditConfigurationManager::ReloadConfigAsync(int nAsyncThreadPriority)
{
	InternalAddRef();

	CSingleLock singleLock(&m_DataLockerCS, TRUE);
	ASSERT(m_pBreakReloadEvent == NULL);
	m_pBreakReloadEvent = new CEvent(FALSE, TRUE);

	CWinThread* pThread = AfxBeginThread(ReloadConfigAsync_Proc, this, nAsyncThreadPriority);
	if (pThread)
	{
		ASSERT(m_hReloadThread == NULL);
		m_hReloadThread = pThread->m_hThread;
	}
	else
	{
		if (m_pBreakReloadEvent)
		{
			delete m_pBreakReloadEvent;
			m_pBreakReloadEvent = NULL;
		}
	}
}

UINT CXTPSyntaxEditConfigurationManager::ReloadConfigAsync_Proc(LPVOID pThis)
{
	CXTPSyntaxEditConfigurationManager* pMan = (CXTPSyntaxEditConfigurationManager*)pThis;
	ASSERT(pMan);
	if (pMan)
	{
		pMan->ReloadConfig();

		pMan->m_hReloadThread = NULL;
		pMan->InternalRelease();
	}
	return 0;
}
