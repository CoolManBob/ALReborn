// FILE		: AgcmUIHotkey.cpp
// AUTHOR	: burumal
// DATE		: 2006/06/29

#ifndef __AGCM_UI_HOTKEY__
#define __AGCM_UI_HOTKEY__

#include "AgcmUIManager2.h"

//#define __SORT_UI_HOTKEY_LIST__

#define UI_HOTKEY_MAX_COUNT			256

#define UI_HOTKEY_INI_VERSION		0x3		// default키가 변경되었거나 추가된 hotkey등이 생겼을때 버젼을 올려줘야한다

#define UI_HOTKEY_INI_FILE			"Ini/UI_Hotkey.ini"

#define UI_HOTKEY_MOVEMENT_FORWARD	17
#define UI_HOTKEY_MOVEMENT_LEFT		30
#define UI_HOTKEY_MOVEMENT_RIGHT	32
#define UI_HOTKEY_MOVEMENT_BACK		31

class AgcmUIHotkey : public AgcModule
{
public :

	AgcmUIHotkey();
	~AgcmUIHotkey();
	
	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();
	
	BOOL			SetIniInfo(CHAR* pIniName, BOOL bDecryption);

	BOOL			IsRegistered() { return m_bRegistered; }

	AgcdUIHotkey*	GetDefaultHotkey(AgcdUIHotkeyType eType, INT32 nCode);		
	AgcdUIHotkey*	GetDefaultHotkeyByIndex(INT32 nIdx);
	INT32			GetDefaultHotkeyIndex(AgcdUIHotkeyType eType, INT32 nCode);

	AgcdUIHotkey*	GetCurrentHotkey(AgcdUIHotkeyType eType, INT32 nCode);
	AgcdUIHotkey*	GetCurrentHotkeyByIndex(INT32 nIdx);
	INT32			GetCurrentHotkeyIndex(AgcdUIHotkeyType eType, INT32 nCode);

	AgcdUIHotkey*	FindCurrentHotkeyByDefault(AgcdUIHotkeyType eDefaultType, INT32 nDefaultCode);
	
	// Callback Funcs...
	static	BOOL	CBEditControlHotkeyOverwriteCheck(PVOID pClass, PVOID pHotkeyType, PVOID pHotkeyCode, PVOID pEditCtrl);

	static	BOOL	CBDisplayHotkeyDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDisplay, INT32* plValue, AgcdUIControl* pcsSourceControl);
	static	BOOL	CBDisplayHotkeyCode(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDisplay, INT32* plValue, AgcdUIControl* pcsSourceControl);

	static	BOOL	CBFunctionHotkeyInputCode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static	BOOL	CBFunctionHotkeyOnOk(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static	BOOL	CBFunctionHotkeyOnCancle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static	BOOL	CBFunctionHotkeyOnDefault(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static	BOOL	CBFunctionHotkeyOnUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);

	// Static Helper Funcs...
	static UINT		ScanCodeToVirtual(INT32 nScanCode);

	static BOOL		GetAsyncHotkeyState(AgcdUIHotkeyType eType, INT16 nVirtualKeyCode);
	static BOOL		GetAsyncHotkeyState(AgcdUIHotkey* pHotkey);


protected :

	struct UI_HOTKEY_ITEM
	{
		CHAR* szDesc;
		INT32 nIndex;

		UI_HOTKEY_ITEM(INT32 nIdx = 0)
		{
			nIndex = nIdx;
			szDesc = NULL;			
		}
	};

	BOOL			DoRegister();

	BOOL			ReadCustomHotkeyIniFile();
	BOOL			WriteCustomHotkeyIniFile();
	
	BOOL			ReadDefaultHotkeyPack(AgcmUIManager2* pUIMgr);

	static VOID		RefreshUserData(AgcmUIHotkey* pThis);

	static VOID		ApplyHelpHotkeyContext(AgcmUIHotkey* pThis);

	static VOID		UpdateLastModified(AgcmUIHotkey* pThis);
		
	static BOOL		ReMappingAllHotkeys(AgcmUIHotkey* pThis, AgcdUIHotkey** ppSrcPack);

	static VOID		ResetToDefaultMapping(AgcmUIHotkey* pThis);

	static VOID		ApplyPlayerMovementKeys(AgcmUIHotkey* pThis);

#if defined(__SORT_UI_HOTKEY_LIST__)
	static bool		SortCompareFunc( const UI_HOTKEY_ITEM* pLeft, const UI_HOTKEY_ITEM* pRight );
#endif // __SORT_UI_HOTKEY_LIST__


public :

	BOOL					m_bRegistered;

	HKL						m_hKeyboardLayout;

	AgcmUIManager2*			m_pAgcmUIManager2;

	BOOL					m_bIsDecrypted;

	AgcdUIUserData*			m_pUserDataItemIndex;
	
	AgcdUIControl*			m_pCurrentControl;
	
	INT32					m_nHotkeyCount;
	
	BOOL					m_bDefaultPackReady;

	INT32					m_nEventHotkeyOverwrited;
	INT32					m_nEventUIOpen;

	CHAR					m_pIniFile[256];

	// 디폴트 핫키들
	AgcdUIHotkey*			m_pDefaultPack[UI_HOTKEY_MAX_COUNT];

	// 커스텀 핫키들
	AgcdUIHotkey*			m_pCustomPack[UI_HOTKEY_MAX_COUNT];

	// 현재 수정중인 핫키들
	AgcdUIHotkey*			m_pCurrentPack[UI_HOTKEY_MAX_COUNT];
	
	AgcdUIHotkey*			m_pUIMgrHotkeyPtr[UI_HOTKEY_MAX_COUNT];

	INT32					m_nItemIndexTable[UI_HOTKEY_MAX_COUNT];	

#if defined(__SORT_UI_HOTKEY_LIST__)
	std::vector<UI_HOTKEY_ITEM*> m_vecSortedHotkeyItemTable;	
#endif // __SORT_UI_HOTKEY_LIST__
};

#endif // __AGCM_UI_HOTKEY__