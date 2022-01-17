#ifdef _AREA_KOREA_
#pragma once

//각 나라별.. 게임별 버전을 만들어 놨으면 좋았을텐데...
#include <windows.h>
#include <stdio.h>

class CAuHangAuth
{
public:
	//Factory쓸까 했는뎅 Mode가 두개라..
	enum eHangAuthType
	{
		eHangAuthPatcher	 = 0,
		eHangAuthClient		 = 1,
		eHangAuthMax
	};

	static const char* szNonExecuteFile;
	static const char* szNonExecuteCommand;
	static const DWORD s_dwTickMax;

public:
	CAuHangAuth();
	~CAuHangAuth();

	bool	Initialize( bool bExcute, char* szCmdLine, eHangAuthType eType = eHangAuthPatcher );
	void	Destory();
	void	Refresh();
	int		UpdateGamestring( char* szGameString );
	char*	ErrorCode( int nCode );

	const bool  IsExcute()				{	return m_bExcute;		}
	const bool  IsRealService();
	
	const char* GetMemberID()			{	return m_szMemberID;	}
	const int   GetMemberNumber()		{	return m_nMemberNumber;	}
	char*       GetGameString()			{	return m_szGameString;	}
	char*       GetMyAuthString()		{	return m_szAuthString;	}

private:
	char*	GetSecondParam( char* szCmdLine, int nLength );

	eHangAuthType	m_eType;
	bool			m_bExcute;
	char			m_szErrorCode[512];

	int				m_nMemberNumber;
	char			m_szMemberID[64];
	char*			m_szAuthString;
	char*			m_szGameString;

	DWORD			m_dwTick;
};

extern CAuHangAuth g_cHanAuthKor;
#endif