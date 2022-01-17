#pragma once

#ifdef _TIW

#include "BasePatchClient.h"

#include "MoreButton.h"
#include "AcArchlordInfo.h"


#define		ARCHLORD_ADVERTISEMENT									_T("")
#define		ARCHLORD_NOTICE											_T("")

#define		ARCHLORD_INSIDE01										_T("")
#define		ARCHLORD_INSIDE02										_T("")
#define		ARCHLORD_INSIDE03										_T("")


class TaiwanPatchClient
	:	public BasePatchClient
{
public :
	unsigned long													m_ulClientFileCRC;
	char															m_strPatchCodeString[ LENGTH_PATCH_CODE_STRING ];

public:
	TaiwanPatchClient( CDialog* pDlg );
	virtual ~TaiwanPatchClient(void);

	virtual BOOL			OnInitDialog							( VOID );
	virtual VOID			OnPaint									( VOID );
	virtual VOID			OnTimer									( UINT nIDEvent );
	virtual VOID			DoDataExchange							( CDataExchange* pDX );

	virtual VOID			OnStartGame								( VOID );
	virtual	BOOL			OnReceiveClientFileCRC					( void* pPacket );

protected:
	BOOL					DoStartGame								( void );
	BOOL					DoCloseDialog							( void );

private :
	BOOL					_GetClientFileCRCFromPatchServer		( void );
	BOOL					_CheckClientFileCRC						( char* pFileName, unsigned long ulCRC );
	BOOL					_MakePatchCodeString					( void );
	BOOL					_IsUpdatePatchClientFile				( char* pFileName );
	virtual bool			Connect									( void );

	AcArchlordInfo			m_ArchlordInfo;
};

#endif