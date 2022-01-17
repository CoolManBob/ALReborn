// RKRepository.cpp
// 2003.08.05 steeple

#include "stdafx.h"
#include "RKInclude.h"

CRKRepository::CRKRepository()
{
	Init();
}

CRKRepository::~CRKRepository()
{
}

void CRKRepository::Init()
{
	m_szInfoFileName = "";

	SetDefaultIndex(0);
	SetNowIndex(0);
	SetSize(0);
}

BOOL CRKRepository::LoadInfoFile()
{
	if(m_szInfoFileName.GetLength() == 0)
		return FALSE;

	BOOL bResult = FALSE;

	m_csInfoParser.SetFileName(m_szInfoFileName);
	if(m_csInfoParser.Open())
	{
		int i = 0;
		CString* pszTmp = NULL;
		while(m_csInfoParser.NewLine())
		{
			// 이름
			pszTmp = m_csInfoParser.GetToken(0);
			if(pszTmp)
				m_stData[i].szName = (LPCTSTR)(*pszTmp);

			// 경로
			pszTmp = m_csInfoParser.GetToken(1);
			if(pszTmp)
				m_stData[i].szPath = (LPCTSTR)(*pszTmp);

			// Working Folder
			pszTmp = m_csInfoParser.GetToken(2);
			if(pszTmp)
				m_stData[i].szWorkingFolder = (LPCTSTR)(*pszTmp);

			// 디펄트 선택인지..
			pszTmp = m_csInfoParser.GetToken(3);
			if(pszTmp)
			{
				if(pszTmp->GetAt(0) == '1')
				{
					SetDefaultIndex(i);
					SetNowIndex(i);
				}
			}
			
			i++;
		}

		m_csInfoParser.Close();
		SetSize(i);

		if(i > 0) bResult = TRUE;
	}

	return bResult;
}

void CRKRepository::SetDefaultIndex(INT16 nIndex)
{
	if(nIndex >= 0 && nIndex < MAX_REPOSITORY)
		m_nDefaultIndex = nIndex;
}

void CRKRepository::SetNowIndex(INT16 nIndex)
{
	if(nIndex >= 0 && nIndex < MAX_REPOSITORY)
		m_nNowIndex = nIndex;
}

void CRKRepository::SetSize(INT16 nSize)
{
	if(nSize >= 0 && nSize < MAX_REPOSITORY)
		m_nSize = nSize;
}

INT16 CRKRepository::GetRepositoryList(CList<stRepositoryData, stRepositoryData>& csList)
{
	if(m_nSize <= 0) return 0;

	for(int i = 0; i < m_nSize; i++)
	{
		stRepositoryData stRepositoryData;
		stRepositoryData = m_stData[i];
		csList.AddTail(stRepositoryData);
	}

	return m_nSize;
}

stRepositoryData* CRKRepository::GetRepositoryData(INT16 nIndex)
{
	if(nIndex >= 0 && nIndex < m_nSize)
		return &m_stData[nIndex];
	else
		return NULL;
}

stRepositoryData* CRKRepository::GetRepositoryData(CString& szName)
{
	if(m_nSize == 0) return NULL;

	stRepositoryData* pstRepositoryData = NULL;
	for(int i = 0; i < m_nSize; i++)
	{
		if(m_stData[i].szName.CompareNoCase((LPCTSTR)szName) == 0)
		{
			pstRepositoryData = &m_stData[i];
			break;
		}
	}

	return pstRepositoryData;
}

BOOL CRKRepository::NewRepository()
{
	// 현재(2003.08.06) 사용하지 않는다.
	return FALSE;
}

BOOL CRKRepository::AddRepository(CString& szName, CString& szPath, CString& szWorkingFolder)
{
	if(m_nSize >= MAX_REPOSITORY)
		return FALSE;

	if(szName.IsEmpty())
		return FALSE;

	if(m_szInfoFileName.GetLength() == 0)
		return FALSE;

	// 이미 있는 지 확인
	if(GetRepositoryData(szName) != NULL)
		return FALSE;

	BOOL bResult = FALSE;

	m_csWriter.SetFileName(m_szInfoFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteRepository(szName, szPath, szWorkingFolder, 0);	// 기본적으로는 Default Select 를 주지 않는다.
		m_csWriter.Close();
	}

	if(bResult)
		AddRepositoryArray(szName, szPath, szWorkingFolder);

	return bResult;
}

BOOL CRKRepository::RemoveRepository(CString& szName)
{
	if(GetRepositoryData(szName) == NULL)
		return FALSE;

	BOOL bResult = FALSE;

	m_csWriter.SetFileName(m_szInfoFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.RemoveRepository(szName);
		m_csWriter.Close();
	}

	// 파일에서 지웠으면, 배열에서도 지워준다.
	if(bResult)
		RemoveRepositoryArray(szName);

	return bResult;
}

BOOL CRKRepository::SetDefaultRepository(CString& szName)
{
	BOOL bResult = FALSE;

	CString szOldDefault;
	stRepositoryData* pstRepositoryData = GetRepositoryData(m_nDefaultIndex);
	if(pstRepositoryData != NULL)
		szOldDefault = pstRepositoryData->szName;

	m_csWriter.SetFileName(m_szInfoFileName);
	if(szOldDefault.CompareNoCase((LPCTSTR)szName) != 0 && szOldDefault.GetLength() > 0)
	{
		if(m_csWriter.Open())
		{
			m_csWriter.WriteRepositoryDefault(szOldDefault, FALSE);
			m_csWriter.Close();
		}
	}

	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteRepositoryDefault(szName, TRUE);
		m_csWriter.Close();
	}

	if(bResult)
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(m_stData[i].szName.CompareNoCase((LPCTSTR)szName) == 0)
			{
				SetDefaultIndex(i);
				SetNowIndex(i);
				break;
			}
		}
	}

	return bResult;
}

BOOL CRKRepository::AddRepositoryArray(CString& szName, CString& szPath, CString& szWorkingFolder)
{
	if(m_nSize >= MAX_REPOSITORY)
		return FALSE;

	m_stData[m_nSize].szName = szName;
	m_stData[m_nSize].szPath = szPath;
	m_stData[m_nSize].szWorkingFolder = szWorkingFolder;

	m_nSize++;

	return TRUE;
}

BOOL CRKRepository::RemoveRepositoryArray(CString& szName)
{
	BOOL bResult = FALSE;

	for(int i = 0; i < m_nSize; i++)
	{
		if(szName.CompareNoCase((LPCTSTR)m_stData[i].szName) == 0)
		{
			bResult = TRUE;

			if(i + 1 == m_nSize)
			{
				m_stData[i].szName = "";
				m_stData[i].szPath = "";
				m_stData[i].szWorkingFolder = "";

				SetDefaultIndex(0);
				SetNowIndex(0);
			}
			else
			{
				// 하나씩 땡겨준다.
				for(int j = i; j < m_nSize - 1; j++)
				{
					m_stData[j].szName = m_stData[j+1].szName;
					m_stData[j].szPath = m_stData[j+1].szPath;
					m_stData[j].szWorkingFolder = m_stData[j+1].szWorkingFolder;
				}

				if(GetDefaultIndex() == i) SetDefaultIndex(0);
				else if(GetDefaultIndex() > i) SetDefaultIndex(GetDefaultIndex() - 1);

				if(GetNowIndex() == i) SetNowIndex(0);
				else if(GetNowIndex() > i) SetNowIndex(GetNowIndex() - 1);
			}

			m_nSize--;
			break;
		}
	}

	return bResult;
}

BOOL CRKRepository::SetRemotePath(CString& szName, CString& szPath)
{
	if(szPath.GetLength() == 0)
		return FALSE;

	BOOL bResult = FALSE;

	m_csWriter.SetFileName(m_szInfoFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteRemotePath(szName, szPath);
		m_csWriter.Close();
	}

	return bResult;
}

BOOL CRKRepository::SetWorkingFolder(CString& szName, CString& szWorkingFolder)
{
	BOOL bResult = FALSE;

	m_csWriter.SetFileName(m_szInfoFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteWorkingFolder(szName, szWorkingFolder);
		m_csWriter.Close();
	}

	return bResult;
}