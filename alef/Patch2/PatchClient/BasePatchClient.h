#pragma once

#include "AuThread.h"
#include "AuMD5Encrypt.h"
#include "ClientNetworkLib.h"
#include "PatchClientLib.h"
#include "TestHtmlView.h"
#include "KbcButton.h"
#include "CustomProgressBar.h"
#include "PatchClientOptionDlg.h"
#include "TransparentListBox.h"
#include "NewStatic.h"
#include "mmsystem.h"
#include "LangControl.h"


#define MESSAGE_TITLE  L"Archlord"

#define		LISTSCROLL_POSTION_X			263
#define		LISTSCROLL_MIN					310
#define		LISTSCROLL_MAX					420
#define		LISTSCROLL_SIZE					110
#define		JAPAN_COOKIE_TIMER				100
#define		CHINA_TIMER						1
#define		KOREA_TIMER						100

#define		PING_THREAD						0
#define		PATCH_THREAD					1

#define		PATCH_AREA_LIST_REFRESH_DELAY	1000
#define		LENGTH_PATCH_CODE_STRING		32



#ifdef _KOR
	//#define	REG_KEY_NAME_ARCHLORD	"Software\\Webzen\\Archlord_KR_Alpha"
#define	REG_KEY_NAME_ARCHLORD	"Software\\Webzen\\Archlord_KR"
#endif

#ifdef _ENG
	#define	REG_KEY_NAME_ARCHLORD	"Software\\Webzen\\Archlord"
#endif

#ifdef _JPN
	#define	REG_KEY_NAME_ARCHLORD	"Software\\Archlord"
#endif

#ifdef _CHN
	#define	REG_KEY_NAME_ARCHLORD	"Software\\Archlord"
#endif

#ifdef _TIW
#define	REG_KEY_NAME_ARCHLORD	"Software\\Webzen\\Archlord_TW"
#endif

#define		REG_KEY_NAME_DIRECTX	"SOFTWARE\\Microsoft\\DirectX"


class BasePatchClient
	:	public	AuThread
{
public:
	BasePatchClient( CDialog* pDlg );
	virtual ~BasePatchClient(void);

	virtual BOOL			OnInitDialog				( VOID )					PURE;
	virtual VOID			OnPaint						( VOID )					PURE;
	virtual VOID			DoDataExchange				( CDataExchange* pDX )		PURE;
	
	VOID					Destroy						( VOID );

	virtual VOID			OnTimer						( UINT nIDEvent )			{	}
	virtual VOID			OnBnClickedMenu1			( VOID )					{	}
	virtual VOID			OnBnClickedMenu2			( VOID )					{	}
	virtual VOID			OnBnClickedMenu3			( VOID )					{	}
	virtual VOID			OnBnClickedMenu4			( VOID )					{	}
	virtual VOID			OnBnClickedMenu5			( VOID )					{	}

	virtual VOID			OnBnClickedServer1			( VOID )					{	}
	virtual VOID			OnBnClickedServer2			( VOID )					{	}
	virtual VOID			OnBnClickedServer3			( VOID )					{	}

	virtual VOID			Run							( VOID );
	virtual VOID			OnStartGame					( VOID )					{ m_cPatchClientLib.StartGame( NULL ); }
	virtual	BOOL			OnReceiveClientFileCRC		( void* pPacket )			{ return TRUE; }


	VOID					PatchThreadStart			( VOID );
	VOID					PingThreadStart				( VOID );
	VOID					SetStartupMode				( AgcmOptionStartup eMode )	{	m_cPatchOptionFile.SetStartupMode( eMode ); }
	VOID					SetArea						( const char*		str )	{	m_cPatchClientLib.SetArea( (char*)str );	}

	VOID					SetForceExit				( BOOL bForceExit )			{	m_bForceExit		=	bForceExit;			}
	BOOL					GetForceExit				( VOID )					{	return m_bForceExit;						}

	CPatchOptionFile*		GetPatchOptionFile			( VOID )					{	return &m_cPatchOptionFile;					}
	CPatchClientLib*		GetPatchClientLib			( VOID )					{	return &m_cPatchClientLib;					}

	BOOL					SendRequestClientFileCRC	( void )					{	return m_cPatchClientLib.SendRequestClientFileCRC();	}
	BOOL					IsUpdatePatchClient			( void )					{	return m_cPatchClientLib.IsPatchClient();	}
	BOOL					IsExistFile					( char* pFileName );

	virtual bool			Connect						( void )
	{
		char* pstrIP	= m_cPatchClientLib.m_cPatchReg.GetIP();
		int	iPort		= m_cPatchClientLib.m_cPatchReg.GetPort();
		bool bConnect	= m_cPatchClientLib.initConnect( 0x0202, false, pstrIP, iPort );
		return bConnect;
	}

	void					DisConnect					( void )
	{
		m_cPatchClientLib.cleanupWinsock();
	}

	bool					Listen						( bool bIsLogin )
	{
		bool bResult = m_cPatchClientLib.ExecuteIOLoop( bIsLogin );
		return bResult;
	}

	HBITMAP					LoadBitmapResource			( const char*	strFileName );

protected:
	
	VOID					ResetScrollPos				( VOID );

	virtual VOID			StartPatch			( VOID );
	VOID					StartPing			( VOID );	

	// variable	-------------------------------------------------------------
	CCustomProgressCtrl				m_cCustomProgressRecvFile;
	CCustomProgressCtrl				m_cCustomProgressRecvBlock;

	CTestHtmlView					*m_pWeb;
	CTestHtmlView					*m_pWeb2;
	CTestHtmlView					*m_pWebTitle;
	CTestHtmlView					*m_pWebAdvertisement;
	CTestHtmlView					*m_pWebInside[	3	];
	CDialog							*m_pDlg;
	CStatic							m_staticBackColor;
	CStatic							m_staticMainBack;

	CPatchOptionFile				m_cPatchOptionFile;
	CPatchClientLib					m_cPatchClientLib;
	
	CKbcButton						m_cKbcExitButton;
	CKbcButton						m_cKbcRegisterButton;
	CKbcButton						m_cKbcOptionButton;
	CKbcButton						m_cKbcHomepageButton;
	CKbcButton						m_cKbcStartgameButton;

	INT								m_nCurrentServerListCategory;

	CFont							m_cDrawFont;
	CFont							m_cDrawTitle;
	CFont							m_cDrawFontInfo;
	CFont							m_cDrawProgressMsg;
	CFont							m_cDrawHyperLink;

	CPoint							m_cOldPoint;		
	BOOL							m_bShowPingTime;
	BOOL							m_bForceExit;

	CBitmap							m_cBMPBackground;
	CNewStatic						m_StaticStatus;
	CNewStatic						m_StaticDetailInfo;

	HBITMAP							m_hbmBackBuffer;
	HBITMAP							m_hbmOld;
	HDC								m_hdcBack;

	CBitmap							m_cBmpBackgroundEx;		



	INT								m_nOperationThread;			//	0 - Ping Thread		1 - Patch Thread

};

extern INT	GetIntFromStringTable(	INT nResourceID );