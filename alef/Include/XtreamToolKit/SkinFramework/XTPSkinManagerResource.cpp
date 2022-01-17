// XTPSkinManagerResource.cpp: implementation of the CXTPSkinManagerResourceFile class.
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

#include "Common/XTPVC50Helpers.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPSkinManagerResource.h"
#include "XTPSkinImage.h"
#include "XTPSkinManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CXTPSkinManagerResourceFile::CXTPSkinManagerResourceFile()
{
	m_lpTextFile = m_lpTextFileEnd = 0;
	m_hModule = 0;
	m_pIniFile = NULL;
	m_pManager = NULL;
}
CXTPSkinManagerResourceFile::~CXTPSkinManagerResourceFile()
{
	Close();
}

void CXTPSkinManagerResourceFile::Close()
{
	SAFE_DELETE(m_pIniFile);

	if (m_hModule)
	{
		FreeLibrary(m_hModule);
		m_hModule = 0;
	}
}

CXTPSkinManagerSchema* CXTPSkinManagerResourceFile::CreateSchema()
{
	TCHAR lpszStyleName[200];
	if (LoadString(m_hModule, 5000, lpszStyleName, 200))
	{
		if (_tcsicmp(lpszStyleName, _T("Office 2007 Style")) == 0)
		{
			return new CXTPSkinManagerSchemaOffice2007(m_pManager);
		}
	}

	return new CXTPSkinManagerSchemaDefault(m_pManager);
}

CString CXTPSkinManagerResourceFile::GetDefaultIniFileName()
{
	HRSRC hRsrc = FindResource(m_hModule, _T("THEMES_INI"), _T("TEXTFILE"));
	if (!hRsrc)
		return _T("");

	HGLOBAL hResData = LoadResource(m_hModule, hRsrc);
	if (!hResData)
		return _T("");

	LPWSTR lpwzThemes = (LPWSTR)LockResource(hResData);

	LPWSTR lpwzFile =  wcsstr(lpwzThemes, L"[File.");
	if (!lpwzFile)
		return _T("");

	LPWSTR pNextLine = wcschr(lpwzFile, L']');
	CString str = CONSTRUCT_S(lpwzFile + 6, (int)(pNextLine - lpwzFile) - 6);


	return str + _T("_ini");
}

BOOL CXTPSkinManagerResourceFile::Open(LPCTSTR lpszResourcePath, LPCTSTR lpszIniFileName)
{
	Close();

	m_strResourcePath = lpszResourcePath;
	m_strIniFileName = lpszIniFileName;

	if (m_strResourcePath.IsEmpty())
		return FALSE;

	CString strFileName = m_strResourcePath + _T('\\') + m_strIniFileName;

	if (!m_strIniFileName.IsEmpty() && FILEEXISTS_S(strFileName))
	{
		m_pIniFile = new CStdioFile;

		if (!m_pIniFile->Open(strFileName, CFile::modeRead))
		{
			SAFE_DELETE(m_pIniFile);
			return FALSE;
		}

		return TRUE;
	}

	m_hModule = LoadLibrary(lpszResourcePath);

	if (!m_hModule)
	{
		m_hModule = LoadLibraryEx(lpszResourcePath, NULL, LOAD_LIBRARY_AS_DATAFILE);
	}
	if (!m_hModule)
	{
		return FALSE;
	}

	if (m_strIniFileName.IsEmpty())
	{
		m_strIniFileName = GetDefaultIniFileName();
	}

	CString strFilePath(m_strIniFileName);

	REPLACE_S(strFilePath, _T('.'), _T('_'));
	REPLACE_S(strFilePath, _T('\\'), _T('_'));

	HRSRC hRsrc = FindResource(m_hModule, strFilePath, _T("TEXTFILE"));
	if (!hRsrc)
		return FALSE;

	HGLOBAL hResData = LoadResource(m_hModule, hRsrc);
	if (!hResData)
		return FALSE;

	m_lpTextFile = (LPWSTR)LockResource(hResData);

	m_lpTextFileEnd = m_lpTextFile + SizeofResource(m_hModule, hRsrc)/2;

	if (m_lpTextFile && m_lpTextFile[0] == 0xFEFF)
		m_lpTextFile++;

	return TRUE;
}

BOOL CXTPSkinManagerResourceFile::ReadString(CString& str)
{
	if (m_pIniFile)
	{
		if (!m_pIniFile->ReadString(str))
		{
			m_pIniFile->Close();
			return FALSE;
		}
		return TRUE;
	}

	ASSERT(m_lpTextFile);

	LPWSTR pNextLine = wcschr(m_lpTextFile, L'\r');

	if (pNextLine == NULL || pNextLine > m_lpTextFileEnd)
		return FALSE;

	str = CONSTRUCT_S(m_lpTextFile, (int)(pNextLine - m_lpTextFile));
	m_lpTextFile = pNextLine + 1;
	if (m_lpTextFile[0] == L'\n') m_lpTextFile = m_lpTextFile + 1;

	return TRUE;
}


CXTPSkinImage* CXTPSkinManagerResourceFile::LoadImage(CString strImageFile)
{
	CXTPSkinImage* pImage = NULL;

	if (m_pIniFile)
	{
		strImageFile = m_strResourcePath + _T('\\') + strImageFile;

		if (!FILEEXISTS_S(strImageFile))
			return NULL;

		pImage = new CXTPSkinImage();

		if (!pImage->LoadFile(strImageFile))
		{
			delete pImage;
			return NULL;
		}
	}
	else
	{
		REPLACE_S(strImageFile, _T('.'), _T('_'));
		REPLACE_S(strImageFile, _T('\\'), _T('_'));

		pImage = new CXTPSkinImage();

		if (!pImage->LoadFile(GetModuleHandle(), strImageFile))
		{
			delete pImage;
			return NULL;
		}
	}

	return pImage;
}
