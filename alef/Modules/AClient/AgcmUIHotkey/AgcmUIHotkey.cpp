// FILE		: AgcmUIHotkey.cpp
// AUTHOR	: burumal
// DATE		: 2006/06/29

#include "AgcmUIHotkey.h"
#include "AgcmUIHelp.h"
#include "AgcmCharacter.h"

#define UI_HOTKEY_SECTION_CONFIG			"CONFIG"
#define UI_HOTKEY_KEY_VERSION				"VERSION"

#define UI_HOTKEY_SECTION_CUSTOM_HOTKEY		"CustomHotkey"
#define UI_HOTKEY_KEY_DEFAULT_CODE			"DefaultCode"
#define UI_HOTKEY_KEY_DEFAULT_TYPE			"DefaultType"
#define UI_HOTKEY_KEY_CUSTOM_CODE			"CustomCode"
#define UI_HOTKEY_KEY_CUSTOM_TYPE			"CustomType"

#define UI_HOTKEY_HELP_ITEMSTRING_TAG		"[_hk_"

UINT g_anScanCodToVirtualTable[256] =
{
	NULL,		VK_ESCAPE,	'1',		'2',		'3',			// 5
	'4',		'5',		'6',		'7',		'8',			// 10
	'9',		'0',		'-',		'=',		VK_BACK,		// 15
	VK_TAB,		'Q',		'W',		'E',		'R',			// 20
	'T',		'Y',		'U',		'I',		'O',			// 25
	'P',		'[',		']',		VK_RETURN,	NULL,			// 30
	'A',		'S',		'D',		'F',		'G',			// 35
	'H',		'J',		'K',		'L',		';',			// 40
	'\'',		'~',		NULL,		'#',		'Z',			// 45
	'X',		'C',		'V',		'B',		'N',			// 50
	'M',		',',		'.',		'/',		NULL,			// 55
	VK_MULTIPLY,NULL,		VK_SPACE,	VK_CAPITAL,	VK_F1,			// 60
	VK_F2,		VK_F3,		VK_F4,		VK_F5,		VK_F6,			// 65
	VK_F7,		VK_F8,		VK_F9,		VK_F10,		VK_NUMLOCK,		// 70
	NULL,		VK_NUMPAD7,	VK_NUMPAD8,	VK_NUMPAD9,	VK_SUBTRACT,	// 75
	VK_NUMPAD4,	VK_NUMPAD5,	VK_NUMPAD6,	VK_ADD,VK_NUMPAD1,			// 80
	VK_NUMPAD2,	VK_NUMPAD3,	VK_NUMPAD0,	VK_DELETE,	NULL,			// 85
	NULL,		'\\',		VK_F11,		VK_F12,		NULL,			// 90
	NULL, NULL, NULL, NULL, NULL,									// 95
	NULL, NULL, NULL, NULL, NULL,									// 100 
	NULL, NULL, NULL, NULL, NULL,									// 105 
	NULL, NULL, NULL, NULL, NULL,									// 110 
	NULL, NULL, NULL, NULL, NULL,									// 115 
	NULL, NULL, NULL, NULL, NULL,									// 120
	NULL, NULL, NULL, NULL, NULL,									// 125 
	NULL, NULL, NULL, NULL, NULL,									// 130 
	NULL, NULL, NULL, NULL, NULL,									// 135 
	NULL, NULL, NULL, NULL, NULL,									// 140 
	NULL, NULL, NULL, NULL, NULL,									// 145 
	NULL, NULL, NULL, NULL, NULL,									// 150 
	NULL, NULL, NULL, NULL, NULL,									// 155 
	NULL, VK_RETURN, NULL, NULL, NULL,								// 160
	NULL, NULL, NULL, NULL, NULL,									// 165 
	NULL, NULL, NULL, NULL, NULL,									// 170 
	NULL, NULL, NULL, NULL, NULL,									// 175
	NULL, NULL, NULL, NULL, NULL,									// 180
	NULL, VK_DIVIDE, NULL, NULL, NULL,								// 185
	NULL, NULL, NULL, NULL, NULL,									// 190
	NULL, NULL, NULL, NULL, NULL,									// 195
	NULL, NULL, VK_NUMLOCK, NULL, VK_HOME,							// 200
	VK_UP,		VK_PRIOR,	NULL,		VK_LEFT,	NULL,			// 205
	VK_RIGHT,	NULL,		VK_END,		VK_DOWN,	VK_NEXT,		// 210
	VK_INSERT,	VK_DELETE,	NULL,		NULL,		NULL,			// 215 
	NULL, NULL, NULL, NULL, NULL,									// 220 
	NULL, NULL, NULL, NULL, NULL,									// 225
	NULL, NULL, NULL, NULL, NULL,									// 230
	NULL, NULL, NULL, NULL, NULL,									// 235
	NULL, NULL, NULL, NULL, NULL,									// 240
	NULL, NULL, NULL, NULL, NULL,									// 245
	NULL, NULL, NULL, NULL, NULL,									// 250
	NULL, NULL, NULL, NULL, NULL,									// 255
	NULL															// 256
};

AgcmUIHotkey::AgcmUIHotkey()
{
	SetModuleName("AgcmUIHotkey");

	m_bRegistered				= FALSE;

	m_bIsDecrypted				= TRUE;
	
	m_pUserDataItemIndex		= NULL;
	
	m_pCurrentControl			= NULL;
	
	m_nHotkeyCount				= 0;	
	
	m_bDefaultPackReady			= FALSE;
	
	m_pAgcmUIManager2			= NULL;

	m_nEventUIOpen				= 0;
	m_nEventHotkeyOverwrited	= 0;

	ZeroMemory(m_pIniFile, sizeof(m_pIniFile));
	
	ZeroMemory(m_pDefaultPack,		sizeof(AgcdUIHotkey*) * UI_HOTKEY_MAX_COUNT);	
	ZeroMemory(m_pCurrentPack,		sizeof(AgcdUIHotkey*) * UI_HOTKEY_MAX_COUNT);
	ZeroMemory(m_pCustomPack,		sizeof(AgcdUIHotkey*) * UI_HOTKEY_MAX_COUNT);	
	ZeroMemory(m_pUIMgrHotkeyPtr,	sizeof(AgcdUIHotkey*) * UI_HOTKEY_MAX_COUNT);
}

AgcmUIHotkey::~AgcmUIHotkey()
{	
#if defined(__SORT_UI_HOTKEY_LIST__)

	INT32 nIdx;
	for ( nIdx = 0; nIdx < UI_HOTKEY_MAX_COUNT; nIdx++ )
	{
		if ( m_vecSortedHotkeyItemTable[nIdx] )
			delete m_vecSortedHotkeyItemTable[nIdx];
	}

	m_vecSortedHotkeyItemTable.clear();

#endif // __SORT_UI_HOTKEY_LIST__
}

BOOL AgcmUIHotkey::OnAddModule()
{
	m_pAgcmUIManager2 =	(AgcmUIManager2*) GetModule("AgcmUIManager2");
	ASSERT( NULL != m_pAgcmUIManager2 );
	if ( !m_pAgcmUIManager2 )
		return FALSE;

	m_hKeyboardLayout = ::GetKeyboardLayout(0);

	return TRUE;
}

BOOL AgcmUIHotkey::OnInit()
{	
	ASSERT(m_pAgcmUIManager2);

	INT32 nIdx;
	for ( nIdx = 0; nIdx < UI_HOTKEY_MAX_COUNT; nIdx++ )
		m_nItemIndexTable[nIdx] = nIdx;

	m_pUserDataItemIndex = m_pAgcmUIManager2->AddUserData("HotKey_ItemIndex", m_nItemIndexTable, sizeof(INT32), UI_HOTKEY_MAX_COUNT, AGCDUI_USERDATA_TYPE_INT32);
	if ( !m_pUserDataItemIndex )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddDisplay(this, "HotKey_DispItemDesc", 0, CBDisplayHotkeyDesc, AGCDUI_USERDATA_TYPE_INT32) )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddDisplay(this, "HotKey_DispItemCode", 0, CBDisplayHotkeyCode, AGCDUI_USERDATA_TYPE_INT32) )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction(this, "HotKey_FuncInputCode", CBFunctionHotkeyInputCode, 0) )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction(this, "HotKey_FuncOnOk", CBFunctionHotkeyOnOk, 0) )
		return FALSE;
	
	if ( !m_pAgcmUIManager2->AddFunction(this, "HotKey_FuncOnCancle", CBFunctionHotkeyOnCancle, 0) )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction(this, "HotKey_FuncOnDefault", CBFunctionHotkeyOnDefault, 0) )
		return FALSE;

	if ( !m_pAgcmUIManager2->AddFunction(this, "HotKey_FuncOnUIOpen", CBFunctionHotkeyOnUIOpen, 0) )
		return FALSE;
	
	m_nEventUIOpen = m_pAgcmUIManager2->AddEvent("Hotkey_EventOpenUI");
	if ( m_nEventUIOpen < 0 )
		return FALSE;

	m_nEventHotkeyOverwrited = m_pAgcmUIManager2->AddEvent("Hotkey_EventDispOverwriteMsg");
	if ( m_nEventHotkeyOverwrited < 0 )
		return FALSE;
	
	RefreshUserData(this);

	return TRUE;
}

BOOL AgcmUIHotkey::OnDestroy()
{
	INT32 nIdx;

	ASSERT(m_pAgcmUIManager2);
	if ( m_pAgcmUIManager2 == NULL )
		return FALSE;

	for ( nIdx = 0; nIdx < UI_HOTKEY_MAX_COUNT; nIdx++ )
	{
		if ( m_pDefaultPack[nIdx] )
		{
			m_pAgcmUIManager2->RemoveHotkey(m_pDefaultPack[nIdx]);
			m_pDefaultPack[nIdx] = NULL;
		}
		
		if ( m_pCurrentPack[nIdx] )
		{
			m_pAgcmUIManager2->RemoveHotkey(m_pCurrentPack[nIdx]);
			m_pCurrentPack[nIdx] = NULL;
		}

		if ( m_pCustomPack[nIdx] )
		{
			m_pAgcmUIManager2->RemoveHotkey(m_pCustomPack[nIdx]);
			m_pCustomPack[nIdx] = NULL;
		}

		m_pUIMgrHotkeyPtr[nIdx] = NULL;
	}

	// 갯수를 다시 복구해준다(UserData 삭제시 이상이 없도록...)
	m_pUserDataItemIndex->m_stUserData.m_lCount = UI_HOTKEY_MAX_COUNT;

	return TRUE;
}

BOOL AgcmUIHotkey::CBDisplayHotkeyDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDisplay, INT32* plValue, AgcdUIControl* pcsSourceControl)
{
	if ( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 )
		return FALSE;
	
	AgcmUIHotkey*	pThis	= (AgcmUIHotkey*) pClass;
	INT32			lIndex	= *((INT32*) pData);

	if ( lIndex < 0 || lIndex >= pThis->m_nHotkeyCount )
		return FALSE;

	if ( pThis->m_pCurrentPack[lIndex] == NULL )
		return TRUE;

	if ( pThis->m_pCurrentPack[lIndex]->m_szDescription )
	{
		sprintf(szDisplay, "%s", pThis->m_pCurrentPack[lIndex]->m_szDescription);
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIHotkey::CBDisplayHotkeyCode(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDisplay, INT32* plValue, AgcdUIControl* pcsSourceControl)
{
	if ( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 )
		return FALSE;

	AgcmUIHotkey*	pThis	= (AgcmUIHotkey*) pClass;
	INT32			lIndex	= *((INT32*) pData);

	if ( lIndex < 0 || lIndex >= pThis->m_nHotkeyCount )
		return FALSE;

	if ( pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_EDIT )
	{
		if ( pThis->m_pCurrentPack[lIndex] == NULL )
			return TRUE;

		AcUIEdit* pEdit = (AcUIEdit*) pcsSourceControl->m_pcsBase;
		if ( !pEdit )
			return FALSE;

		if ( pEdit->m_bForHotkey == FALSE )
			return TRUE;

		UpdateLastModified(pThis);
		
		pEdit->SetCallbackHotkeyOverwriteCheck(NULL, NULL);

		if ( pThis->m_pCurrentPack[lIndex] )
			pEdit->SetHotkey(pThis->m_pCurrentPack[lIndex]->m_eType, pThis->m_pCurrentPack[lIndex]->m_cKeyCode);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL AgcmUIHotkey::CBFunctionHotkeyInputCode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl)
{
	AgcmUIHotkey* pThis = (AgcmUIHotkey*) pClass;

	UpdateLastModified(pThis);

	if ( !pThis || !pcsSourceControl )
		return FALSE;

	if ( pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT )
		return FALSE;

	if ( (pThis->m_pCurrentControl != NULL) && (pThis->m_pCurrentControl == pcsSourceControl) )
		return FALSE;
	
	AcUIEdit* pEdit = (AcUIEdit*) pcsSourceControl->m_pcsBase;
	if ( !pEdit )
		return FALSE;

	pEdit->SetCallbackHotkeyOverwriteCheck(CBEditControlHotkeyOverwriteCheck, pThis);

	pThis->m_pCurrentControl = pcsSourceControl;	
	
	// OnKeyDown 메세지가 들어올수 있게 설정함
	g_pEngine->SetCharCheckIn();
		
	return TRUE;
}

BOOL AgcmUIHotkey::ReadDefaultHotkeyPack(AgcmUIManager2* pUIMgr)
{
	INT32 nIdx, nType;

	if ( !pUIMgr )
		return FALSE;
	
	AgcdUIHotkey* pHotkey;

	ASSERT(m_pAgcmUIManager2);

	for ( nIdx = 0; nIdx < UI_HOTKEY_MAX_COUNT; nIdx++ )
	{
		for ( nType = AGCDUI_HOTKEY_TYPE_NORMAL; nType < AGCDUI_MAX_HOTKEY_TYPE; nType++ )
		{
			pHotkey = pUIMgr->GetHotkey((AgcdUIHotkeyType) nType, nIdx);			
			if ( !pHotkey )
				continue;
			
			if ( pHotkey->m_bCustomizable )
			{
				m_pDefaultPack[m_nHotkeyCount] = new AgcdUIHotkey;
				ASSERT(m_pDefaultPack[m_nHotkeyCount]);
				if ( m_pDefaultPack[m_nHotkeyCount] == NULL )
					break;
				
				m_pDefaultPack[m_nHotkeyCount]->m_bCustomizable = pHotkey->m_bCustomizable;
				m_pDefaultPack[m_nHotkeyCount]->m_cKeyCode		= pHotkey->m_cKeyCode;
				m_pDefaultPack[m_nHotkeyCount]->m_eType			= pHotkey->m_eType;				
				m_pDefaultPack[m_nHotkeyCount]->m_listAction	= pHotkey->m_listAction;
				m_pDefaultPack[m_nHotkeyCount]->m_szDescription	= NULL;
				
				// Desc 는 DefaultPack 에만 포함시킨다
				if ( pHotkey->m_szDescription )
				{
					INT32 nStrLen = strlen(pHotkey->m_szDescription) + 1;
					m_pDefaultPack[m_nHotkeyCount]->m_szDescription = new char[nStrLen];
					ASSERT(m_pDefaultPack[m_nHotkeyCount]->m_szDescription);
					memcpy(m_pDefaultPack[m_nHotkeyCount]->m_szDescription, pHotkey->m_szDescription, nStrLen);
				}

				// 포인터를 저장해둔다
				m_pUIMgrHotkeyPtr[m_nHotkeyCount] = pHotkey;

				m_nHotkeyCount++;
			}
		}
	}

	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		m_pCurrentPack[nIdx] = new AgcdUIHotkey;
		ASSERT(m_pCurrentPack[nIdx]);		
		m_pAgcmUIManager2->CopyHotkey(m_pCurrentPack[nIdx], m_pDefaultPack[nIdx]);

		m_pCustomPack[nIdx] = new AgcdUIHotkey;
		ASSERT(m_pCustomPack[nIdx]);		
		m_pAgcmUIManager2->CopyHotkey(m_pCustomPack[nIdx], m_pDefaultPack[nIdx]);
	}

	m_bDefaultPackReady = TRUE;
	
	return TRUE;
}

BOOL AgcmUIHotkey::SetIniInfo(CHAR* pIniName, BOOL bDecryption)
{
	// UserData 및 Function 등록
	if ( DoRegister() == FALSE )
		return FALSE;

	if ( !pIniName )
		return FALSE;

	memcpy(m_pIniFile, pIniName, strlen(pIniName) + 1);
	
	// Decrypt 정보는 실행시 결정되어지는것 이므로 Write 작업을 위해 저장해둔다
	m_bIsDecrypted = bDecryption;
	
	if ( ReadCustomHotkeyIniFile() == FALSE )
	{
		ReMappingAllHotkeys(this, &(m_pDefaultPack[0]));
		WriteCustomHotkeyIniFile();
	}
	else
	{		
		ReMappingAllHotkeys(this, &(m_pCustomPack[0]));
	}

	ApplyHelpHotkeyContext(this);
	ApplyPlayerMovementKeys(this);

	m_bRegistered = TRUE;

	return m_bRegistered;
}

BOOL AgcmUIHotkey::ReadCustomHotkeyIniFile()
{
	if ( !m_pIniFile[0] )
		return FALSE;

	if ( m_nHotkeyCount <= 0 || m_nHotkeyCount > 256 )
		return FALSE;
	
	AuIniManagerA	cIniMgr;
	cIniMgr.SetPath(m_pIniFile);	
	
	if ( cIniMgr.ReadFile(0, m_bIsDecrypted) == FALSE )
		return FALSE;

	INT32 nNumSection = cIniMgr.GetNumSection();

	if ( nNumSection != (m_nHotkeyCount + 1) ) // +1 은 version section 때문임
	{
		// 잘못된 파일은 지워버린다
		::DeleteFile(m_pIniFile);
		return FALSE;
	}

	int nVersion;
	nVersion = cIniMgr.GetValueI(UI_HOTKEY_SECTION_CONFIG, UI_HOTKEY_KEY_VERSION);
	if ( nVersion == 0 || nVersion != UI_HOTKEY_INI_VERSION )
	{
		// 잘못된 파일은 지워버린다
		::DeleteFile(m_pIniFile);
		return FALSE;
	}

	INT32 nIdx;
	char pSection[64];

	AgcdUIHotkeyType eDefType, eCusType;
	INT32 nDefCode, nCusCode;
	
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		sprintf(pSection, "%s%d", UI_HOTKEY_SECTION_CUSTOM_HOTKEY, nIdx);
		
		eDefType = (AgcdUIHotkeyType) cIniMgr.GetValueI(pSection, UI_HOTKEY_KEY_DEFAULT_TYPE);
		nDefCode = cIniMgr.GetValueI(pSection, UI_HOTKEY_KEY_DEFAULT_CODE);

		eCusType = (AgcdUIHotkeyType) cIniMgr.GetValueI(pSection, UI_HOTKEY_KEY_CUSTOM_TYPE);
		nCusCode = cIniMgr.GetValueI(pSection, UI_HOTKEY_KEY_CUSTOM_CODE);		
		
		ASSERT(m_pAgcmUIManager2);
		if ( m_pAgcmUIManager2 == NULL )
			return FALSE;

		if ( (eDefType == m_pDefaultPack[nIdx]->m_eType) && (nDefCode == m_pDefaultPack[nIdx]->m_cKeyCode) )
		{
			m_pCurrentPack[nIdx]->m_eType = eCusType;
			m_pCurrentPack[nIdx]->m_cKeyCode = nCusCode;

			m_pCustomPack[nIdx]->m_eType = eCusType;
			m_pCustomPack[nIdx]->m_cKeyCode = nCusCode;
		}
	}

	return TRUE;
}

BOOL AgcmUIHotkey::WriteCustomHotkeyIniFile()
{
	if ( !m_pIniFile[0] )
		return FALSE;
	
	AuIniManagerA cIniMgr;
	cIniMgr.SetPath(m_pIniFile);

	INT32 nIdx;
	char pSection[64];

	cIniMgr.SetValueI(UI_HOTKEY_SECTION_CONFIG, UI_HOTKEY_KEY_VERSION, (int) UI_HOTKEY_INI_VERSION);
	
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{	
		AgcdUIHotkey* pDefHotkey = m_pDefaultPack[nIdx];
		ASSERT(pDefHotkey);

		AgcdUIHotkey* pCurHotkey = m_pCurrentPack[nIdx];
		ASSERT(pCurHotkey);

		sprintf(pSection, "%s%d", UI_HOTKEY_SECTION_CUSTOM_HOTKEY, nIdx);
		cIniMgr.SetValueI(pSection, UI_HOTKEY_KEY_DEFAULT_TYPE, (int) pDefHotkey->m_eType);
		cIniMgr.SetValueI(pSection, UI_HOTKEY_KEY_DEFAULT_CODE, (int) pDefHotkey->m_cKeyCode);
		cIniMgr.SetValueI(pSection, UI_HOTKEY_KEY_CUSTOM_TYPE, (int) pCurHotkey->m_eType);
		cIniMgr.SetValueI(pSection, UI_HOTKEY_KEY_CUSTOM_CODE, (int) pCurHotkey->m_cKeyCode);
	}

	// ini 파일 새롭게 생성
	::DeleteFile(UI_HOTKEY_INI_FILE);
	cIniMgr.WriteFile(0, m_bIsDecrypted);
	
	return TRUE;
}

BOOL AgcmUIHotkey::DoRegister()
{	
	if ( ReadDefaultHotkeyPack(m_pAgcmUIManager2) == FALSE )
		return FALSE;
	
	// Item 크기에 맞게 UserData 재설정	
	m_pUserDataItemIndex->m_stUserData.m_lCount = m_nHotkeyCount;	
	
	RefreshUserData(this);

	return TRUE;
}

VOID AgcmUIHotkey::UpdateLastModified(AgcmUIHotkey* pThis)
{	
	if ( !pThis )
		return;

	if ( pThis->m_pCurrentControl == NULL )
		return;

	AcUIEdit* pEdit = (AcUIEdit*) pThis->m_pCurrentControl->m_pcsBase;
	if ( !pEdit )
		return;
	
	INT32 lNewCode = -1;
	AgcdUIHotkeyType eNewType = AGCDUI_MAX_HOTKEY_TYPE;

	if ( pEdit->GetHotkey(&lNewCode, &eNewType) )
	{
		INT32 lIndex = pThis->m_pCurrentControl->m_lUserDataIndex;
		
		if ( lIndex < pThis->m_nHotkeyCount && lIndex >= 0 )
			lIndex = pThis->m_nItemIndexTable[lIndex];

		if ( lNewCode != pThis->m_pCurrentPack[lIndex]->m_cKeyCode || eNewType != pThis->m_pCurrentPack[lIndex]->m_eType )
		{
			pThis->m_pCurrentPack[lIndex]->m_eType = eNewType;
			pThis->m_pCurrentPack[lIndex]->m_cKeyCode = lNewCode;
			
			RefreshUserData(pThis);
		}
	}

	pThis->m_pCurrentControl = NULL;
}

BOOL AgcmUIHotkey::CBFunctionHotkeyOnOk(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl)
{
	AgcmUIHotkey*	pThis	= (AgcmUIHotkey*) pClass;
	UpdateLastModified(pThis);
	ASSERT(pClass);	
	
	if ( !ReMappingAllHotkeys(pThis, &(pThis->m_pCurrentPack[0])) )
		return FALSE;

	if ( !pThis->WriteCustomHotkeyIniFile() )
		return FALSE;

	ApplyHelpHotkeyContext(pThis);
	ApplyPlayerMovementKeys(pThis);

	ASSERT(pThis->m_pAgcmUIManager2);
	if ( pThis->m_pAgcmUIManager2 )
		pThis->m_pAgcmUIManager2->SetUIMgrProcessHotkeys();

	return TRUE;
}

BOOL AgcmUIHotkey::CBFunctionHotkeyOnDefault(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl)
{	
	AgcmUIHotkey*	pThis	= (AgcmUIHotkey*) pClass;
	UpdateLastModified(pThis);	
	ASSERT(pClass);

	if ( !pThis->m_pAgcmUIManager2 )
		return FALSE;

	INT32 nIdx;
	
	for ( nIdx = 0; nIdx < UI_HOTKEY_MAX_COUNT; nIdx++ )
		pThis->m_nItemIndexTable[nIdx] = nIdx;

	for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
	{
		pThis->m_pCurrentPack[nIdx]->m_eType	= pThis->m_pDefaultPack[nIdx]->m_eType;
		pThis->m_pCurrentPack[nIdx]->m_cKeyCode = pThis->m_pDefaultPack[nIdx]->m_cKeyCode;

		pThis->m_pCustomPack[nIdx]->m_eType		= pThis->m_pDefaultPack[nIdx]->m_eType;
		pThis->m_pCustomPack[nIdx]->m_cKeyCode	= pThis->m_pDefaultPack[nIdx]->m_cKeyCode;
	}
	
	RefreshUserData(pThis);
	
	return TRUE;
}

BOOL AgcmUIHotkey::CBFunctionHotkeyOnCancle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl)
{	
	AgcmUIHotkey*	pThis	= (AgcmUIHotkey*) pClass;
	UpdateLastModified(pThis);
	ASSERT(pClass);

	if ( !pThis->m_pAgcmUIManager2 )
		return FALSE;

	INT32 nIdx;
	for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
	{
		pThis->m_pCurrentPack[nIdx]->m_eType = pThis->m_pCustomPack[nIdx]->m_eType;
		pThis->m_pCurrentPack[nIdx]->m_cKeyCode = pThis->m_pCustomPack[nIdx]->m_cKeyCode;
	}

	RefreshUserData(pThis);

	return TRUE;
}

BOOL AgcmUIHotkey::CBEditControlHotkeyOverwriteCheck(PVOID pClass, PVOID pHotkeyType, PVOID pHotkeyCode, PVOID pEditCtrl)
{
	if ( !pClass || !pHotkeyType || !pHotkeyCode || !pEditCtrl )
		return FALSE;

	AgcmUIHotkey* pThis = (AgcmUIHotkey*) pClass;	
	if ( !pThis )
		return FALSE;

	AgcdUIHotkeyType eType = *((AgcdUIHotkeyType*) pHotkeyType);
	INT32 nKeyCode = *((INT32*) pHotkeyCode);

	// 자기자신인지 체크
	AcUIEdit* pEdit = (AcUIEdit*) pEditCtrl;
	if ( pEdit->m_eHotkeyType == eType && pEdit->m_lHotkey == nKeyCode )
		return FALSE;
	
	INT32 nIdx;
	for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
	{
		if ( (eType == pThis->m_pCurrentPack[nIdx]->m_eType) && (nKeyCode == pThis->m_pCurrentPack[nIdx]->m_cKeyCode) )
		{
			pEdit->SetCallbackHotkeyOverwriteCheck(NULL, NULL);
			pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_nEventHotkeyOverwrited);
			return TRUE;
		}		
	}

	return FALSE;
}

BOOL AgcmUIHotkey::CBFunctionHotkeyOnUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl)
{
	AgcmUIHotkey* pThis = (AgcmUIHotkey*) pClass;	
	if ( !pThis )
		return FALSE;

	UpdateLastModified(pThis);

	if ( pThis->ReadCustomHotkeyIniFile() == FALSE )
	{
		ReMappingAllHotkeys(pThis, &(pThis->m_pDefaultPack[0]));
		pThis->WriteCustomHotkeyIniFile();
	}
	else
	{		
		ReMappingAllHotkeys(pThis, &(pThis->m_pCustomPack[0]));
	}

	ApplyHelpHotkeyContext(pThis);
	ApplyPlayerMovementKeys(pThis);

	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_nEventUIOpen);

	g_pEngine->ReleaseMeActiveEdit(NULL);

	return TRUE;
}

BOOL AgcmUIHotkey::ReMappingAllHotkeys(AgcmUIHotkey* pThis, AgcdUIHotkey** ppSrcPack)
{
	if ( !pThis || !ppSrcPack )
		return FALSE;

	if ( !pThis->m_pAgcmUIManager2 )
		return FALSE;
	
	INT32 nCode;
	AgcdUIHotkeyType eType;

	ASSERT(pThis->m_pAgcmUIManager2);

	INT32 nIdx;

	// 기존 단축키 모두 삭제
	for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
	{
		eType = pThis->m_pUIMgrHotkeyPtr[nIdx]->m_eType;
		nCode = pThis->m_pUIMgrHotkeyPtr[nIdx]->m_cKeyCode;

		pThis->m_pAgcmUIManager2->RemoveHotkey(eType, nCode);
	}

	// 재등록
	for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
	{
		ASSERT(ppSrcPack[nIdx]);
		if ( ppSrcPack[nIdx] == NULL )
			continue;

		eType = ppSrcPack[nIdx]->m_eType;
		nCode = ppSrcPack[nIdx]->m_cKeyCode;

		// UIMgr에 새로운 hotkey 등록
		AgcdUIHotkey* pNewHotkey = pThis->m_pAgcmUIManager2->AddHotkey(eType, nCode);
		ASSERT(pNewHotkey);

		pThis->m_pAgcmUIManager2->CopyHotkey(pNewHotkey, ppSrcPack[nIdx]);		

		// CurrentPack도 새롭게 업데이트
		if ( pThis->m_pCurrentPack[nIdx] != ppSrcPack[nIdx] )
			pThis->m_pAgcmUIManager2->CopyHotkey(pThis->m_pCurrentPack[nIdx], ppSrcPack[nIdx]);

		// CustomPack도 새롭게 업데이트		
		if ( pThis->m_pCustomPack[nIdx] != ppSrcPack[nIdx] )
			pThis->m_pAgcmUIManager2->CopyHotkey(pThis->m_pCustomPack[nIdx], ppSrcPack[nIdx]);

		// 포인터를 저장해둔다
		pThis->m_pUIMgrHotkeyPtr[nIdx] = pNewHotkey;
	}

	RefreshUserData(pThis);
	
	return TRUE;
}

VOID AgcmUIHotkey::ApplyHelpHotkeyContext(AgcmUIHotkey* pThis)
{	
	if ( !pThis )
		return;

	AgcmUIHelp* pHelpModule = (AgcmUIHelp*) pThis->GetModule("AgcmUIHelp");
	ASSERT( NULL != pHelpModule );
	if ( !pHelpModule )
		return;

	pHelpModule->ReleaseHelp();

	char strFile[_MAX_PATH];
	sprintf(strFile, "Ini\\%sHelp.txt", g_pEngine->m_szLanguage);

	if ( !pHelpModule->ReadHelp(strFile, pHelpModule->IsDecrypted())  )
		return;

	INT32 nHelpNum = pHelpModule->GetHelpNum();
	CHAR** ppHelpItem = pHelpModule->GetHelpItem();
	CHAR** ppHelpContent = pHelpModule->GetHelpContent();
	INT32* pHelpDepth = pHelpModule->GetHelpDepth();

	if ( nHelpNum <= 0 ||
		!ppHelpItem ||
		!ppHelpContent ||
		!pHelpDepth )
	{
		return;
	}

	INT32 nIdx, nLen;
	INT32 nTagLen = strlen(UI_HOTKEY_HELP_ITEMSTRING_TAG);	
	char pHotkeyStr[256];

	for ( nIdx = 0; nIdx < nHelpNum; nIdx++ )
	{
		if ( pHelpDepth[nIdx] == 1 ) // depth 1인것들중
		{
			// Item 처리
			nLen = strlen(ppHelpItem[nIdx]);
			if ( nLen >= nTagLen + 4 ) // len(TAG):숫자:숫자 이므로 len(TAG) + 4가 최소
			{
				if ( strncmp(ppHelpItem[nIdx], UI_HOTKEY_HELP_ITEMSTRING_TAG, nTagLen) == 0 )
				{
					INT32 nCode = -1;
					INT32 nType = -1;
					sscanf(ppHelpItem[nIdx] + nTagLen, ":%d:%d", &nType, &nCode);

					pHotkeyStr[0] = NULL;

					if ( nCode != -1 && nType != -1 )
					{
						INT32 nDefIdx;						
						for ( nDefIdx = 0; nDefIdx < pThis->m_nHotkeyCount; nDefIdx++ )
						{
							if ( (pThis->m_pDefaultPack[nDefIdx]->m_eType == (AgcdUIHotkeyType) nType) &&
								(pThis->m_pDefaultPack[nDefIdx]->m_cKeyCode == nCode) ) 
							{
								nType = pThis->m_pCurrentPack[nDefIdx]->m_eType;
								nCode = pThis->m_pCurrentPack[nDefIdx]->m_cKeyCode;								
								break;
							}
						}
						
						switch ( (AgcdUIHotkeyType) nType )
						{
						case AGCDUI_HOTKEY_TYPE_SHIFT :
							strcpy(pHotkeyStr, "Shift+");
							break;
						case AGCDUI_HOTKEY_TYPE_CONTROL :
							strcpy(pHotkeyStr, "Ctrl+");
							break;
						case AGCDUI_HOTKEY_TYPE_ALT :
							strcpy(pHotkeyStr, "Alt+");
							break;
						default :
							break;
						}
						
						strcat(pHotkeyStr, g_aszHotkeyNameTable[nCode]);
						//strcat(pHotkeyStr, " KEY");

						delete [] ppHelpItem[nIdx];
						ppHelpItem[nIdx] = new CHAR[strlen(pHotkeyStr) + 1];
						ASSERT(ppHelpItem[nIdx]);
						strcpy(ppHelpItem[nIdx], pHotkeyStr);
					}
				}
			}

			// Content 처리
			char* pToken = strstr(ppHelpContent[nIdx], UI_HOTKEY_HELP_ITEMSTRING_TAG);
			
			if ( pToken != NULL ) // 하나라도 들어있으면
			{
				INT32 nNewSize = strlen(ppHelpContent[nIdx]) + 1024;// tag 변환후 추가 길이가 1024 이상은 넘지 않는다고 가정함

				char* pNewContent = new CHAR[nNewSize];
				ZeroMemory(pNewContent, sizeof(CHAR) * nNewSize);

				char* pNewContentCurrent = pNewContent;

				char* pStrPtr = ppHelpContent[nIdx];
				char* pEndPtr = pStrPtr + strlen(pStrPtr);

				int nBlockSize = 0;

				while ( pToken )
				{
					INT32 nCode = -1;
					INT32 nType = -1;					
					sscanf(pToken + nTagLen, ":%d:%d", &nType, &nCode);
					
					pHotkeyStr[0] = NULL;

					if ( nCode != -1 && nType != -1 )
					{
						INT32 nDefIdx;						
						for ( nDefIdx = 0; nDefIdx < pThis->m_nHotkeyCount; nDefIdx++ )
						{
							if ( (pThis->m_pDefaultPack[nDefIdx]->m_eType == (AgcdUIHotkeyType) nType) &&
								(pThis->m_pDefaultPack[nDefIdx]->m_cKeyCode == nCode) ) 
							{
								nType = pThis->m_pCurrentPack[nDefIdx]->m_eType;
								nCode = pThis->m_pCurrentPack[nDefIdx]->m_cKeyCode;								
								break;
							}
						}

						switch ( (AgcdUIHotkeyType) nType )
						{
						case AGCDUI_HOTKEY_TYPE_SHIFT :
							strcpy(pHotkeyStr, "Shift+");
							break;

						case AGCDUI_HOTKEY_TYPE_CONTROL :
							strcpy(pHotkeyStr, "Ctrl+");
							break;

						case AGCDUI_HOTKEY_TYPE_ALT :
							strcpy(pHotkeyStr, "Alt+");
							break;

						default :							
							break;
						}

						strcat(pHotkeyStr, g_aszHotkeyNameTable[nCode]);					
					}

					nBlockSize = (pToken - pStrPtr);
					memcpy(pNewContentCurrent, pStrPtr, nBlockSize);
					pNewContentCurrent += nBlockSize;

					nBlockSize = strlen(pHotkeyStr);
					memcpy(pNewContentCurrent, pHotkeyStr, nBlockSize);
					pNewContentCurrent += nBlockSize;					
					
					pStrPtr = strstr(pStrPtr, "]") + 1;					

					pToken = strstr(pStrPtr, UI_HOTKEY_HELP_ITEMSTRING_TAG);
				}

				memcpy(pNewContentCurrent, pStrPtr, (pEndPtr - pStrPtr));

				delete [] ppHelpContent[nIdx];
				ppHelpContent[nIdx] = pNewContent;
			}
		}
	}

	pHelpModule->RefreshUI();
}

AgcdUIHotkey* AgcmUIHotkey::GetDefaultHotkey(AgcdUIHotkeyType eType, INT32 nCode)
{
	INT32 nIdx;
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		if ( m_pDefaultPack[nIdx]->m_eType == eType && m_pDefaultPack[nIdx]->m_cKeyCode == nCode )
			return m_pDefaultPack[nIdx];
	}

	return NULL;
}

AgcdUIHotkey* AgcmUIHotkey::GetDefaultHotkeyByIndex(INT32 nIdx)
{
	if ( nIdx < 0 || nIdx >= m_nHotkeyCount )
		return NULL;

	return m_pDefaultPack[nIdx];
}

INT32 AgcmUIHotkey::GetDefaultHotkeyIndex(AgcdUIHotkeyType eType, INT32 nCode)
{
	INT32 nIdx;
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		if ( m_pDefaultPack[nIdx]->m_eType == eType && m_pDefaultPack[nIdx]->m_cKeyCode == nCode )
			return nIdx;
	}

	return -1;
}

AgcdUIHotkey* AgcmUIHotkey::GetCurrentHotkey(AgcdUIHotkeyType eType, INT32 nCode)
{
	INT32 nIdx;
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		if ( m_pCurrentPack[nIdx]->m_eType == eType && m_pCurrentPack[nIdx]->m_cKeyCode == nCode )
			return m_pCurrentPack[nIdx];
	}

	return NULL;
}

AgcdUIHotkey* AgcmUIHotkey::GetCurrentHotkeyByIndex(INT32 nIdx)
{
	if ( nIdx < 0 || nIdx >= m_nHotkeyCount )
		return NULL;

	return m_pCurrentPack[nIdx];
}

INT32 AgcmUIHotkey::GetCurrentHotkeyIndex(AgcdUIHotkeyType eType, INT32 nCode)
{
	INT32 nIdx;
	for ( nIdx = 0; nIdx < m_nHotkeyCount; nIdx++ )
	{
		if ( m_pCurrentPack[nIdx]->m_eType == eType && m_pCurrentPack[nIdx]->m_cKeyCode == nCode )
			return nIdx;
	}

	return -1;
}

VOID AgcmUIHotkey::ApplyPlayerMovementKeys(AgcmUIHotkey* pThis)
{
	if ( !pThis )
		return;

	AgcmCharacter* pCharModule = (AgcmCharacter*) pThis->GetModule("AgcmCharacter");
	ASSERT( NULL != pCharModule );
	if ( !pCharModule )
		return;

	pCharModule->SetMovementKeys();
}

UINT AgcmUIHotkey::ScanCodeToVirtual(INT32 nScanCode)
{
	UINT vkCode;

	/*
	vkCode = MapVirtualKeyEx(nScanCode, 0, m_hKeyboardLayout);

	if ( vkCode == 0 )
		vkCode = MapVirtualKey(nScanCode, 0);
	*/

	// comment: 위 MapVirtualKey 함수가 제대로 동작하지 않아서 대체해 넣은 코드.

	if ( nScanCode < 0 || nScanCode >= 256 )
		return 0;

	vkCode = g_anScanCodToVirtualTable[nScanCode];
	
	return vkCode;
}


AgcdUIHotkey* AgcmUIHotkey::FindCurrentHotkeyByDefault(AgcdUIHotkeyType eDefaultType, INT32 nDefaultCode)
{
	INT32 nDefIdx;

	nDefIdx = GetDefaultHotkeyIndex(eDefaultType, nDefaultCode);
	if ( nDefIdx < 0 ) 
		return NULL;

	return GetCurrentHotkeyByIndex(nDefIdx);
}

BOOL AgcmUIHotkey::GetAsyncHotkeyState(AgcdUIHotkey* pHotkey)
{	
	if ( !pHotkey )
		return FALSE;

	UINT16 nVkCode = ScanCodeToVirtual(pHotkey->m_cKeyCode);

	return GetAsyncHotkeyState(pHotkey->m_eType, nVkCode);
}

BOOL AgcmUIHotkey::GetAsyncHotkeyState(AgcdUIHotkeyType eType, INT16 nVirtualKeyCode)
{	
	BOOL bIsPressed = TRUE;

	switch ( eType )
	{
	case AGCDUI_HOTKEY_TYPE_SHIFT :
		bIsPressed = g_pEngine->IsShiftDown();
		break;

	case AGCDUI_HOTKEY_TYPE_CONTROL :
		bIsPressed = g_pEngine->IsCtrlDown();
		break;

	case AGCDUI_HOTKEY_TYPE_ALT :
		bIsPressed = g_pEngine->IsAltDown();
		break;
	}

	if ( bIsPressed )
		return (GetAsyncKeyState(nVirtualKeyCode) < 0);

	return FALSE;
}

VOID AgcmUIHotkey::RefreshUserData(AgcmUIHotkey* pThis)
{	
	INT32 nIdx;

	if ( pThis == NULL )
		return;

	//pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pUserDataItemIndex);
	
#if defined(__SORT_UI_HOTKEY_LIST__)

	if ( pThis->m_nHotkeyCount > 0 )
	{
		for ( nIdx = 0; nIdx < pThis->m_vecSortedHotkeyItemTable.size(); nIdx++ )
		{
			if ( pThis->m_vecSortedHotkeyItemTable[nIdx] )
				delete pThis->m_vecSortedHotkeyItemTable[nIdx];
		}

		pThis->m_vecSortedHotkeyItemTable.clear();

		for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
		{
			pThis->m_vecSortedHotkeyItemTable.push_back(new UI_HOTKEY_ITEM(nIdx));
			pThis->m_vecSortedHotkeyItemTable[nIdx]->szDesc = pThis->m_pCurrentPack[nIdx]->m_szDescription;
		}
		
		std::sort(pThis->m_vecSortedHotkeyItemTable.begin(), pThis->m_vecSortedHotkeyItemTable.end(), SortCompareFunc);

		for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
			pThis->m_nItemIndexTable[nIdx] = pThis->m_vecSortedHotkeyItemTable[nIdx]->nIndex;
	}

#else // __SORT_UI_HOTKEY_LIST__

	// w,a,s,d가 연속해서 붙어서 나오도록 forward를 left 앞에 위치시킨다

	AgcdUIHotkey* pHkForward = 
		pThis->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_FORWARD);	// forward 단축키

	AgcdUIHotkey* pHkLeft = 
		pThis->FindCurrentHotkeyByDefault(AGCDUI_HOTKEY_TYPE_NORMAL, UI_HOTKEY_MOVEMENT_LEFT);	// left 단축키

	if ( pHkForward && pHkLeft )
	{
		INT32 nExchange1 = -1;
		INT32 nExchange2 = -1;

		for ( nIdx = 0; nIdx < pThis->m_nHotkeyCount; nIdx++ )
		{
			if ( nExchange1 == -1 && pThis->m_pCurrentPack[nIdx] == pHkForward )
			{
				nExchange1 = nIdx;
			}
			else
			if ( nExchange2 == -1 && pThis->m_pCurrentPack[nIdx] == pHkLeft )
			{
				if ( nIdx > 0 )
					nExchange2 = nIdx - 1;
				else
					break;
			}

			if ( nExchange1 != -1 && nExchange2 != -1 )
				break;
		}

		if ( nExchange1 != -1 && nExchange2 != -1 )
		{
			pThis->m_nItemIndexTable[nExchange1] = nExchange2;
			pThis->m_nItemIndexTable[nExchange2] = nExchange1;
		}
	}
	
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pUserDataItemIndex);

#endif // __SORT_UI_HOTKEY_LIST__		
}

#if defined(__SORT_UI_HOTKEY_LIST__)
bool AgcmUIHotkey::SortCompareFunc( const UI_HOTKEY_ITEM* pLeft, const UI_HOTKEY_ITEM* pRight )
{
	if ( !pLeft || !pRight )
		return false;

	if ( !pLeft->szDesc || !pRight->szDesc )
		return false;

	return (_stricmp(pLeft->szDesc, pRight->szDesc) < 0);
}
#endif // __SORT_UI_HOTKEY_LIST__