#pragma once

#include "ApmEventManager.h"
#include "AgcmUIManager2.h"
#include "AgcmEventManager.h"
#include "AcUIBase.h"
#include "AgcmEventGacha.h"

class AgcmUIEventGacha;

class AcUIGacha : public AcUIBase
{
public:
	AgcmUIEventGacha	*m_pcsAgcmUIEventGacha;
public:
	struct	SlotMachine
	{
		INT32		nTID		;
		RwTexture	*pTexture	;
		string		strName		;

		SlotMachine():nTID( 0 ) , pTexture( NULL ) {}
	};

	vector< SlotMachine >	m_vecSlot;
	SlotMachine	* m_pCurrentSlot;

	enum RollingMode
	{
		RM_DISPLAY,    // 디스플레이중.. 그냥 느린속도로 계속 돌아감
		RM_ROLLING ,    // 롤링중으로 실 계산이 이루어짐.
		RM_RESULT        // 결과가 나온경우
	};
	RollingMode    m_eRollingMode; 

    INT32	m_nPosition		;
    INT32	m_nMaxPosition	;
	UINT32	m_uPrevTick		;

	UINT32	m_uRollingStartTime;
	INT32	m_nResultPosition;

	INT32	m_nResultStartOffset;

	INT32	m_nResultTID;

	AgcuPathWork *	m_pSplineData;

	BOOL	LoadGachaSetting( const char * pFileName );
	BOOL	SetSlotItems( vector< INT32 > * pVector );
    BOOL    StartRoll( INT32 nTIDResult );

	BOOL	IsRollingNow() { return m_eRollingMode == RM_ROLLING ? TRUE : FALSE; }

public:
	AcUIGacha();
	virtual ~AcUIGacha();

	virtual	void OnWindowRender	()	;
	virtual BOOL OnInit			()	;

	virtual BOOL OnPostInit		()	;

	virtual BOOL OnIdle			( UINT32 ulClockCount	);

	virtual void OnCloseUI();
};

class AgcmUIEventGacha : public AgcModule
{
public:
	ApmEventManager		*m_pcsApmEventManager	;
	AgcmUIManager2		*m_pcsAgcmUIManager2	;
	AgcmEventGacha		*m_pcsAgcmEventGacha	;

	INT32	m_lEventGachaOpen	;
	INT32	m_lEventGachaError	;
	INT32	m_lEventGachaRoll	;
	INT32	m_lEventGachaClose	;

	AcUIGacha	m_cUIGacha	;
	string		m_stringName;	// 디스플레이되는 텍스트.

	AgpmEventGacha::GachaInfo	m_stGachaInfo;

	AgcdUIUserData* m_pcsUDItemName	;		
	AgcdUIUserData* m_pcsUDNotice	;	

	AuPOS		m_posGachaOpenPosition;
	BOOL		m_bOpenGachaUI;

public:
	void	UpdateControlNotice();	// 공지 내용 업데이트
	void	UpdateControlItem();	// 아이템 이름 업데이트

	BOOL				AddEvent();

	enum Const
	{
		GACHA_MESSAGE_LENGTH = 64
	};

	static const int	AGCMUIEVENTGACHA_CLOSE_UI_DISTANCE			= 150;


	void	ShowErrorReport( AgpmEventGacha::ERROR_CODE eErrorCode );

	AgcmUIEventGacha();
	virtual ~AgcmUIEventGacha();

	BOOL			OnAddModule();
	BOOL			OnRollEnd();	// 가챠 롤이 다 끝나고 결과가 나온경우

	virtual void OnLuaInitialize	( AuLua * pLua			);

	static BOOL		CBOpenGachaUI(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSelfCharacterUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	 	CBGachaRoll	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	 	CBGachaClose	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayGachaMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayGachaItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl);
};
