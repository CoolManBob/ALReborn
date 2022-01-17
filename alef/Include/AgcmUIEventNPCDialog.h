#ifndef	__AGCM_UI_EVENT_NPC_DIALOG_H__
#define	__AGCM_UI_EVENT_NPC_DIALOG_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIEventNPCDialogD" )
#else
#pragma comment ( lib , "AgcmUIEventNPCDialog" )
#endif
#endif

#include <time.h>
#include <sys/timeb.h>

#include "ApmEventManager.h"
#include "AgpmItem.h"
#include "AgpmEventNPCDialog.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
//#include "AgcmUIManager.h"
#include "AgcmEventManager.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmUIManager2.h"

#define		AGCMUI_EVENT_MAX_NPCDIALOG		10

#define		AGCMUIEVENTNPCDIALOG_CLOSE_UI_DISTANCE		150

// 마고자 (2005-05-17 오후 6:55:14) : 
// 앞에건 NPC의 CID , 뒤에건 이벤트 인덱스
#define		AGCMUIEVENTNPCDIALOG_SOUNDFILEFORMAT	"Sound\\Effect\\NP%03d%d.wav"

// NPC SOUND TYPE
enum	NS_TYPE 
{
	NS_WELCOME	= 0,
	NS_FAREWELL	= 1,
	NS_QUEST	= 2,
	NS_RESERVED1= 3,
	NS_RESERVED2= 4,
	NS_RESERVED3= 5,
	NS_RESERVED4= 6,
	NS_RESERVED5= 7,
	NS_RESERVED6= 8,
	NS_RESERVED7= 9
};

class AgcdUIEventNPCDialogButtonInfo
{
public:
	char				m_strDisplayString[80];
	ApdEventFunction	m_eFunctionType;
	bool				m_bExit;

	AgcdUIEventNPCDialogButtonInfo()
	{
		Reset();
	}
	void Reset()
	{
		memset( m_strDisplayString, 0, sizeof(m_strDisplayString) );
		m_eFunctionType = APDEVENT_FUNCTION_NONE;
		m_bExit = false;
	}
};

class AgcmUIEventNPCDialogButton
{
public:
	AgcdUIEventNPCDialogButtonInfo	m_csButton[AGCMUI_EVENT_MAX_NPCDIALOG];

	void Clear();
	INT32 SetData(ApdEventFunction eFunctionType, char *pstrDisplay, bool bExit = false );
	AgcdUIEventNPCDialogButtonInfo *GetData( INT32 lIndex );
};

class AgcmUIEventNPCDialog : public AgcModule {
private:
	ApmEventManager		*m_pcsApmEventManager;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmEventNPCDialog	*m_pcsAgpmEventNPCDialog;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmItem			*m_pcsAgcmItem;
	AgcmEventManager	*m_pcsAgcmEventManager;
	AgcmEventNPCDialog	*m_pcsAgcmEventNPCDialog;
	AgcmUIManager2		*m_pcsAgcmUIManager2;

	INT32				m_lNPCDailogTextID;

	//버튼처리를 위한 내부 변수.
	ApBase				*m_pcsApBase;
	ApBase				*m_pcsGenerator;

	//UserData
	AgcdUIEventNPCDialogButtonInfo		*m_apcsDialog[AGCMUI_EVENT_MAX_NPCDIALOG];
	AgcmUIEventNPCDialogButton			m_csDialogButtons;

	AgcdUIUserData		*m_pstDialogUD;

	// event ids
	INT32				m_lEventOpenNPCDialogUI;
	INT32				m_lEventCloseNPCDialogUI;

	BOOL				AddEvent();
	BOOL				AddFunction();
	BOOL				AddDisplay();
	BOOL				AddUserData();

	AuPOS				m_stEventPos;
	BOOL				m_bIsNPCDialogUIOpen;

	// 마고자 (2005-05-17 오후 7:02:19) : 
	// 사운드 출력.
	BOOL				m_bNoNeedCloseSound;
	INT32				m_lLastOpenDialogNPCID;
public:
	BOOL				IsNoNeedNPCCloseSound() {return m_bNoNeedCloseSound;}
	void				PlayNPCSound( NS_TYPE eType , ApBase * pcsOwnerBase = NULL );
	AuPOS&				GetNPCPos( void ) { return m_stEventPos; }

	void				CloseNPCDialog();
	//버튼 출력 관련 정보.

public:
	AgcmUIEventNPCDialog();
	virtual ~AgcmUIEventNPCDialog();

	BOOL				OnAddModule();

	static BOOL			CBAriseNPCDialogUI(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBNPCDialogDisplayDialog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBDisplayNPCDialog(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL			CBNPCDailogSelectButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGCMUIEVENTDialog_H__