#pragma once

class CPatchClientRegistry
{
public:
	CPatchClientRegistry();
	~CPatchClientRegistry();

	bool	InitRegistry();
	bool	FinishPatch( int iVersion, int iCheckCode = 0);

	bool	BuildRegistry( char *pstrSubKey, bool bReset = false );
	bool	BuildPathRegistry( char *pstrSubKey );

	char*	GetIP()			{	return m_strPatchServerIP;	}
	int		GetPort()		{	return m_iPatchServerPort;	}
	int		GetVersion()	{	return m_iVersion;			}
	void	SetVersion( int iVersion )	{	m_iVersion = iVersion;	}
	int		GetPatchCheckCode()			{	return m_iPatchCheckCode;	}
	void	SetPatchCheckCode( int iPatchCheckCode )	{	m_iPatchCheckCode = iPatchCheckCode;	}
	char*	GetRegKeyBase()				{	return m_strRegKeyBase;		}
	void	SetRegKeyBase( char *pstrRegkey );

	bool	SetPatchInfo( char *pstrIP, int iPort );
	bool	SetLoginInfo( char *pstrIP, int iPort );
	
	BOOL	GetUseDriverUpdate();
	void	SetUseDriverUpdate( BOOL bUse );

	BOOL	GetProfileDword( DWORD& dwValue,  LPCTSTR cszSection, LPCTSTR cszEntry, DWORD dwDefault = 0 );
	BOOL	SetProfileDword( LPCTSTR cszSection, LPCTSTR cszEntry, DWORD dwValue );

	DWORD	GetSelfPatchFlag()				{	return m_iSelfPatchFlag;	}
	void	SetSelfPatchFlag( DWORD dwVal )	{	m_iSelfPatchFlag = dwVal ? 1 : 0;	}

	int		GetDXVersionFromReg();

private:
	bool	CleanRegistry( char *pstrSubKey );
	void	RestoreRegistry();
	void	BackupRegistry();
	bool	GetRegistryInfo( char *pstrSubKey );

private:
	char	m_strRegKeyBase[256];

	char	m_strPatchServerIP[256];
	char	m_strLoginServerIP[256];


	int		m_iPatchServerPort;
	int		m_iLoginServerPort;
	int		m_iVersion;
	int		m_iPatchCheckCode;	

	BOOL	m_bUseDriverUpdate;

	BOOL	m_bVersionFromBackup;

	int		m_iSelfPatchFlag;
	char	m_szRegistryPath[256];
};