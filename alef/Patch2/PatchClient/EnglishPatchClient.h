#pragma once

#include "BasePatchClient.h"
#include "AcArchlordInfo.h"

#define		GLOBAL_WEB_LEFT_URL										"http://archlord.webzen.com/Launcher/leftPart.aspx"
#define		GLOBAL_WEB_RIGHT_URL									"http://archlord.webzen.com/Launcher/RightPart.aspx"

#define		GLOBAL_WEB_LEFT_URL_TEST								"http://new-archlord.webzen.com/Launcher/leftPart.aspx"
#define		GLOBAL_WEB_RIGHT_URL_TEST								"http://new-archlord.webzen.com/Launcher/RightPart.aspx"



class EnglishPatchClient
	:	public BasePatchClient
{
public :
	unsigned long													m_ulClientFileCRC;
	char															m_strPatchCodeString[ LENGTH_PATCH_CODE_STRING ];

public:
	EnglishPatchClient( CDialog* pDlg );
	virtual ~EnglishPatchClient(void);


	virtual BOOL			OnInitDialog							( VOID );
	virtual VOID			OnPaint									( VOID );
	virtual VOID			DoDataExchange							( CDataExchange* pDX );

	virtual VOID			OnTimer									( UINT nIDEvent );


	virtual bool			Connect									( void );

protected:
	HBITMAP															m_cBMPBackgroundEx;	

public :
	virtual VOID			OnStartGame								( VOID );
	virtual	BOOL			OnReceiveClientFileCRC					( void* pPacket );


	BOOL					DoStartGame								( void );

protected:
	BOOL					DoCloseDialog							( void );

private :
	BOOL					_GetClientFileCRCFromPatchServer		( void );
	BOOL					_CheckClientFileCRC						( char* pFileName, unsigned long ulCRC );
	BOOL					_MakePatchCodeString					( void );
	BOOL					_IsUpdatePatchClientFile				( char* pFileName );


	AcArchlordInfo			m_ArchlordInfo;
};
