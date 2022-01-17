#ifndef		_AGCMUICHATTING_H_
#define		_AGCMUICHATTING_H_

#include "ApModule.h"
#include "AgcmChatting.h"
#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AuIniManager.h"

#define	AGCMUICHATTING_CHAT_WINDOW_X					3
#define	AGCMUICHATTING_CHAT_WINDOW_Y					626

// Ini Key Name 
#define	AGCMUICHATTING_KEY_NAME_CHAT_MAIN			"chatmain"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB0_0			"chattabbutton0_0"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB0_1			"chattabbutton0_1"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB0_2			"chattabbutton0_2"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB0_3			"chattabbutton0_3"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB1_0			"chattabbutton1_0"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB1_1			"chattabbutton1_1"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB1_2			"chattabbutton1_2"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB1_3			"chattabbutton1_3"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB2_0			"chattabbutton2_0"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB2_1			"chattabbutton2_1"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB2_2			"chattabbutton2_2"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB2_3			"chattabbutton2_3"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB3_0			"chattabbutton3_0"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB3_1			"chattabbutton3_1"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB3_2			"chattabbutton3_2"
#define AGCMUICHATTING_KEY_NAME_CHAT_TAB3_3			"chattabbutton3_3"

#define AGCMUICHATTING_CHATTING_WINDOW_HANDLER_X	0
#define AGCMUICHATTING_CHATTING_WINDOW_HANDLER_Y	72
#define AGCMUICHATTING_CHATTING_WINDOW_HANDLER_W	40
#define AGCMUICHATTING_CHATTING_WINDOW_HANDLER_H	24

#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIChattingD" )
#else
#pragma comment ( lib , "AgcmUIChatting" )
#endif

class AgcmUIChatting : public AgcModule
{
private:
	AgcdUIUserData		*m_pcsUserDataMaxScrollPos;
	AgcdUIUserData		*m_pcsUserDataCurrentScrollPos;

	INT32				m_lMaxScrollPos;
	INT32				m_lCurrentScrollPos;

public:
	AgcmUIChatting()	;
	~AgcmUIChatting()	;
	
	BOOL			m_bAddChatWindow	;		// Chatting Window를 붙였는가의 여부 

	virtual BOOL	OnAddModule()	;
	virtual BOOL	OnInit()		;
	virtual BOOL	OnDestroy()		;

	AgcdUI *		m_pcsUI			;

public:
	AgcmChatting*		m_pAgcmChatting		;
	AgcmUIManager2*		m_pAgcmUIManager2	;

public:
	BOOL	UIIniRead( CHAR* szName, BOOL bDecryption )		;
	
	static	BOOL	CBOepnMainUI( PVOID pData, PVOID pClass, PVOID pCustData				)	;
	static  BOOL	CBCloseAllUIWindow( PVOID pData, PVOID pClass, PVOID pCustData			)	;

	static  BOOL	CBInputMessage( PVOID pData, PVOID pClass, PVOID pCustData			)	;

	static BOOL		CBNextChatTab( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBScrollUp( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBScrollDown( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBScrollHome( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBScrollEnd( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBScrollMove( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;

	static BOOL		CBChatModeAll( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBChatModeParty( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;
	static BOOL		CBChatModeGuild( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )	;

	static BOOL		CBDisplayMaxScrollPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplayCurrentScrollPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	BOOL			RefreshScrollPos();
};

#endif		// _AGCMUICHATTING_H_