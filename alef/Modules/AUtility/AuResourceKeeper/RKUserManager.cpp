// RKUserManager.cpp
// 2003.08.05 steeple

#include "stdafx.h"
#include "RKInclude.h"

char* getRandomSalt(void)
;

CRKUserManager::CRKUserManager()
{
	Init();
}

CRKUserManager::~CRKUserManager()
{
}

void CRKUserManager::Init()
{
	m_szUserName = "";
	m_szPassword = "";
	m_szRemotePath = "";
	m_bAdmin = FALSE;
	SetStatus(STATUS_LOGOUT);
}

BOOL CRKUserManager::Login(CString& szUserName, CString& szPassword)
{
	SetUserName(szUserName);
	SetPassword(szPassword);

	return Login();
}

BOOL CRKUserManager::Login()
{
	if(m_szRemotePath.GetLength() == 0)
		return FALSE;

	BOOL bResult = FALSE;

	CString szRemoteFileName;
	if(m_szRemotePath.GetAt(m_szRemotePath.GetLength() - 1) == '\\')
		szRemoteFileName = m_szRemotePath + CString(USER_FILENAME);
	else
		szRemoteFileName = m_szRemotePath + "\\" + CString(USER_FILENAME);

	m_csParser.SetFileName(szRemoteFileName);
	if(m_csParser.Open())
	{
		CString* pszUserName = NULL;
		CString* pszPassword = NULL;
		while(m_csParser.NewLine())
		{
			pszUserName = m_csParser.GetToken(0);
			if(pszUserName)
			{
				// User 발견!!!
				if(pszUserName->CompareNoCase((LPCTSTR)m_szUserName) == 0)
				{
					// Password 비교
					pszPassword = m_csParser.GetPassword();
					if(pszPassword)
					{
						char szOriginPassword[PASSWD_LENGTH+1];
						strcpy(szOriginPassword, (LPCTSTR)*pszPassword);

						char szSalt[SALT_LENGTH+1];
						memset(szSalt, 0, SALT_LENGTH+1);
						memcpy(szSalt, &szOriginPassword[3], SALT_LENGTH);

						char szInputPassword[PASSWORD_LENGTH+1];
						strcpy(szInputPassword, (LPCTSTR)m_szPassword);

						char szCryptPassword[128];
						strcpy(szCryptPassword, crypt_md5(szInputPassword, szSalt));

						if(strcmp(szCryptPassword, (LPCTSTR)*pszPassword) == 0)
						{
							SetStatus(STATUS_LOGIN);	// 접속한 상태로 바꿔주고..
							bResult = TRUE;

							// 어드민으로 로긴한지 알아낸다.
							if(m_csParser.GetToken(1))
								m_bAdmin = atoi((LPCTSTR)*m_csParser.GetToken(1));

							break;
						}
					}
				}	// User 발견
			}
		}	// while

		m_csParser.Close();
	}

	return bResult;
}

BOOL CRKUserManager::Logout()
{
	BOOL bResult = FALSE;

	if(GetStatus() == STATUS_LOGIN)
	{
		SetStatus(STATUS_LOGOUT);
		bResult = TRUE;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
// Admin
BOOL CRKUserManager::AddUser(CString& szUserName, CString& szPassword, CString& szRemotePath, BOOL bAdmin)
{
	BOOL bResult = FALSE;

	if(szUserName.GetLength() == 0 || szUserName.GetLength() > USERNAME_LENGTH)
		return FALSE;

	if(szPassword.GetLength() == 0 || szPassword.GetLength() > PASSWORD_LENGTH)
		return FALSE;

	if(szRemotePath.GetLength() == 0)
		szRemotePath = m_szRemotePath;

	if(szRemotePath.GetLength() == 0)
		return FALSE;

	CString szRemoteFileName;
	if(szRemotePath.GetAt(szRemotePath.GetLength() - 1) == '\\')
		szRemoteFileName = szRemotePath + CString(USER_FILENAME);
	else
		szRemoteFileName = szRemotePath + "\\" + CString(USER_FILENAME);

	// 암호화
	char szSalt[SALT_LENGTH+1];
	strcpy(szSalt, getRandomSalt());

	char szInputPassword[PASSWORD_LENGTH+1];
	strcpy(szInputPassword, (LPCTSTR)szPassword);

	char szCryptPassword[PASSWD_LENGTH+1];
	strcpy(szCryptPassword, crypt_md5(szInputPassword, szSalt));
	// 훗훗

	m_csWriter.SetFileName(szRemoteFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteUser(szUserName, CString(szCryptPassword), bAdmin);
		m_csWriter.Close();
	}

	return bResult;
}

BOOL CRKUserManager::RemoveUser(CString& szUserName, CString& szRemotePath)
{
	BOOL bResult = FALSE;

	if(szRemotePath.GetLength() == 0)
		szRemotePath = m_szRemotePath;

	if(szRemotePath.GetLength() == 0)
		return FALSE;

	CString szRemoteFileName;
	if(szRemotePath.GetAt(szRemotePath.GetLength() - 1) == '\\')
		szRemoteFileName = szRemotePath + CString(USER_FILENAME);
	else
		szRemoteFileName = szRemotePath + "\\" + CString(USER_FILENAME);

	m_csWriter.SetFileName(szRemoteFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteUser(szUserName, CString(""), FALSE, FALSE);
		m_csWriter.Close();
	}

	return bResult;
}

INT32 CRKUserManager::GetUserList(CList<stUserInfo, stUserInfo>& csList, CString& szRemotePath)
{
	INT32 iResult = 0;

	if(szRemotePath.GetLength() == 0)
		szRemotePath = m_szRemotePath;

	if(szRemotePath.GetLength() == 0)
		return iResult;

	CString szRemoteFileName;
	if(szRemotePath.GetAt(szRemotePath.GetLength() - 1) == '\\')
		szRemoteFileName = szRemotePath + CString(USER_FILENAME);
	else
		szRemoteFileName = szRemotePath + "\\" + CString(USER_FILENAME);

	m_csParser.SetFileName(szRemoteFileName);
	if(m_csParser.Open())
	{
		while(m_csParser.NewLine())
		{
			stUserInfo stUserInfo;
			stUserInfo.szUserName = (LPCTSTR)*m_csParser.GetToken(0);
			stUserInfo.szPassword = (LPCTSTR)*m_csParser.GetPassword();
			stUserInfo.bAdmin = atoi((LPCTSTR)*m_csParser.GetToken(1));
			csList.AddTail(stUserInfo);

			iResult++;
		}
		m_csParser.Close();
	}

	return iResult;
}

BOOL CRKUserManager::SetAdmin(CString& szUserName, BOOL bAdmin, CString& szRemotePath)
{
	if(szUserName.IsEmpty() || szRemotePath.IsEmpty())
		return FALSE;

	CString szRemoteFileName;
	if(szRemotePath.GetAt(szRemotePath.GetLength() - 1) == '\\')
		szRemoteFileName = szRemotePath + CString(USER_FILENAME);
	else
		szRemoteFileName = szRemotePath + "\\" + CString(USER_FILENAME);

	BOOL bResult = FALSE;
	m_csWriter.SetFileName(szRemoteFileName);
	if(m_csWriter.Open())
	{
		bResult = m_csWriter.WriteAdmin(szUserName, bAdmin);
		m_csWriter.Close();
	}

	return bResult;
}