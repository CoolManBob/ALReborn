// ConfigManager.cpp: implementation of the CConfigManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigManager::CConfigManager()
{
	Init();
}

CConfigManager::~CConfigManager()
{
	Close();
}

void CConfigManager::Init()
{
	m_bOpen = FALSE;

	m_szFileName = _T("");
	m_szNowLine = _T("");
}

void CConfigManager::SetFileName(CString& szFileName)
{
	m_szFileName = szFileName;
}

BOOL CConfigManager::Open()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(m_szFileName.GetLength() == 0)
		return FALSE;

	if(m_csFile.Open((LPCTSTR)m_szFileName, CFile::modeRead | CFile::shareDenyRead | CFile::shareDenyWrite))
	{
		m_bOpen = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CConfigManager::Close()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(m_bOpen)
	{
		m_csFile.Close();
		m_bOpen = FALSE;

		m_szNowLine = _T("");
	}

	return TRUE;
}

BOOL CConfigManager::ReadLine()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_bOpen)
		return FALSE;

	if(m_csFile.GetLength() == 0)
		return FALSE;

	if(!m_csFile.ReadString(m_szNowLine))
	{
		m_szNowLine = _T("");
		return FALSE;
	}

	return TRUE;
}

CString CConfigManager::GetKey()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szKey = _T("");

	if(!m_bOpen)
		return szKey;

	if(m_szNowLine.GetLength() == 0)
		return szKey;

	INT32 lIndex = m_szNowLine.Find(CONFIGMANAGER_BRIDGE, 0);
	if(lIndex < 0 || lIndex >= m_szNowLine.GetLength())
		return szKey;

	return m_szNowLine.Left(lIndex);
}

CString CConfigManager::GetValue()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CString szValue = _T("");

	if(!m_bOpen)
		return szValue;

	if(m_szNowLine.GetLength() == 0)
		return szValue;

	INT32 lIndex = m_szNowLine.ReverseFind(CONFIGMANAGER_BRIDGE);
	if(lIndex < 0 || lIndex >= m_szNowLine.GetLength())
		return szValue;
	
	return m_szNowLine.Right(m_szNowLine.GetLength() - lIndex - 1);
}
