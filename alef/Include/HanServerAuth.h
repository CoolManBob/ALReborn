#pragma once

class CHanServerAuth
{
public:
	enum
	{
		HAN_SERVER_AREA_KOREA,
		HAN_SERVER_AREA_JAPAN,
	};

	int m_iArea;

public:
	CHanServerAuth( void );

	bool  Init( int iArea = HAN_SERVER_AREA_KOREA );
	int   Auth( char* account, char* authstring, int* piAge = NULL );
	char* GetError( int errorCode );

private:
};

extern CHanServerAuth g_SvrAuth;
