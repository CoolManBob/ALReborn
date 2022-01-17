// AgcmUIManager2Stream.cpp: streaming implementation of the AgcmUIManager2 class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmUIManager2.h"

#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

#include "AuStringConv.h"
#include "AgcmEventNPCDialog.h"
#include "AuStrTable.h"


#define AGCMUIMANAGER2_NAME_EVENTMAP			"EventMap"
#define AGCMUIMANAGER2_NAME_EVENT_NAME			"EventName"

#define AGCMUIMANAGER2_NAME_HOTKEY				"Hotkey"
#define AGCMUIMANAGER2_NAME_HOTKEY_FLAG			"HotkeyFlag"
#define AGCMUIMANAGER2_NAME_HOTKEY_DESC			"HotkeyDesc"

#define AGCMUIMANAGER2_NAME_POSITION			"Pos"
#define AGCMUIMANAGER2_NAME_POSITION_UI			"UIPos"			// Mode가 추가된 UI의 Position
#define AGCMUIMANAGER2_NAME_SIZE				"Size"
#define AGCMUIMANAGER2_NAME_COLOR				"Color"
#define AGCMUIMANAGER2_NAME_WIN_PROPERTY		"WinProperty"
#define AGCMUIMANAGER2_NAME_TEXTURE				"Texture"
#define AGCMUIMANAGER2_NAME_STATUS				"Status"
#define AGCMUIMANAGER2_NAME_STATUS2				"Status2"		// Status에 Color가 추가되면서 이전버전과의 호환성을 위해서 임시
#define AGCMUIMANAGER2_NAME_DEFAULT_IDS			"DefaultIDs"
#define AGCMUIMANAGER2_NAME_HANDLER				"Handler"
#define AGCMUIMANAGER2_NAME_ANIMATION			"Anim"
#define AGCMUIMANAGER2_NAME_ANIMATION_DEFAULT	"AnimDefault"

#define AGCMUIMANAGER2_NAME_UI_MAIN				"MainUI"
#define AGCMUIMANAGER2_NAME_UI_PARENT			"Parent"
#define AGCMUIMANAGER2_NAME_UI_INITSTATUS		"InitStatus"
#define AGCMUIMANAGER2_NAME_UI_COORDSYSTEM		"CSystem"
#define AGCMUIMANAGER2_NAME_UI_TYPE				"UIType"
#define AGCMUIMANAGER2_NAME_UI_MODE_FLAG		"ModeFlag"
#define AGCMUIMANAGER2_NAME_UI_FOCUS_CB			"Focus"

#define AGCMUIMANAGER2_NAME_CONTROL_NAME		"CtrlName"
#define AGCMUIMANAGER2_NAME_CONTROL_TYPE		"CtrlType"

#define AGCMUIMANAGER2_NAME_CONTROL_MESSAGE		"CtrlMessage"
#define AGCMUIMANAGER2_NAME_CONTROL_DISPLAY		"CtrlDisplay"
#define AGCMUIMANAGER2_NAME_CONTROL_USERDATA	"CtrlUserData"
#define AGCMUIMANAGER2_NAME_CONTROL_USERDATA2	"CtrlUserData2"
#define AGCMUIMANAGER2_NAME_CONTROL_PARENT_UD	"CtrlParentUD"
#define AGCMUIMANAGER2_NAME_CONTROL_PARENT		"CtrlParent"
#define AGCMUIMANAGER2_NAME_CONTROL_FONT		"CtrlFont"
#define AGCMUIMANAGER2_NAME_CONTROL_SHOW		"CtrlShow"
#define AGCMUIMANAGER2_NAME_CONTROL_TOOLTIP		"CtrlTooltip"

#define AGCMUIMANAGER2_NAME_ACTION				"Action"
#define AGCMUIMANAGER2_NAME_ACTION_POS			"ActPos"
#define AGCMUIMANAGER2_NAME_ACTION_ATTACH		"ActAttach"
#define AGCMUIMANAGER2_NAME_ARGUMENT			"Argument"
#define AGCMUIMANAGER2_NAME_ARGUMENT2			"Argument2"

#define AGCMUIMANAGER2_NAME_EDIT				"Edit"
#define AGCMUIMANAGER2_NAME_BUTTON				"Button"
#define AGCMUIMANAGER2_NAME_BUTTON2				"Button2"
#define AGCMUIMANAGER2_NAME_GRID				"Grid"
#define AGCMUIMANAGER2_NAME_GRID2				"Grid2"
#define AGCMUIMANAGER2_NAME_BAR					"Bar"
#define AGCMUIMANAGER2_NAME_BAR2				"Bar2"
#define AGCMUIMANAGER2_NAME_LIST				"List"
#define AGCMUIMANAGER2_NAME_TREE				"Tree"
#define AGCMUIMANAGER2_NAME_SKILLTREE			"SkillTree"
#define AGCMUIMANAGER2_NAME_SCROLL				"Scroll"
#define AGCMUIMANAGER2_NAME_COMBO				"Combo"
#define AGCMUIMANAGER2_NAME_CLOCK				"Clock"

#define AGCMUIMANAGER2_NAME_CURSOR				"Cursor"

#define AGCMUIMANAGER2_NAME_BUTTON_OK			"UI_Name_OK"
#define AGCMUIMANAGER2_NAME_BUTTON_CANCEL		"UI_Name_Cancel"

BOOL			AgcmUIManager2::StreamWriteUI( AgcdUI* pUI, CHAR* szFile, BOOL bEncryption )
{
	if( !pUI )		return FALSE;
	if( !szFile )	return FALSE;
	
	ApModuleStream		csStream;

	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );
	csStream.SetSection(AGCMUIMANAGER2_NAME_EVENTMAP);

	//int nIndex = 0;
	//for( AgcdUIControl* pcsControl = pThis->GetSequenceControl( pUI, &nIndex ); pcsControl; pcsControl = pThis->GetSequenceControl( pUI, &nIndex ) )
	//{
	//	if( !pcsControl )				continue;
	//	if( pcsControl->m_bDynamic)		continue;

	//	if ( !pThis->StreamWriteControl( pcsControl, pStream ) )
	//		return FALSE;
	//}

	csStream.SetSection( pUI->m_szUIName );
	csStream.EnumWriteCallback( AGCMUIMANAGER2_DATA_TYPE_UI, pUI, this );
	csStream.Write( szFile, 0, bEncryption );

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWrite(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex;
	INT32				lIndex2;
	AgcdUI *			pcsUI;
	AgcdUIEventMap *	pstEventMap;
	AgcdUIHotkey *		pstHotkey;
	CHAR				szTemp[128];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	csStream.SetSection(AGCMUIMANAGER2_NAME_EVENTMAP);
	for (lIndex = 0; lIndex < m_lMaxUIEvents; ++lIndex)
	{
		pstEventMap = GetEventMap(lIndex);
		if (pstEventMap && pstEventMap->m_listAction.GetCount())
		{
			if (!csStream.WriteValue(AGCMUIMANAGER2_NAME_EVENT_NAME, pstEventMap->m_szName))
				return FALSE;

			if (!StreamWriteAction(&pstEventMap->m_listAction, &csStream))
				return FALSE;
		}
	}

	for (lIndex = AGCDUI_HOTKEY_TYPE_NORMAL; lIndex < AGCDUI_MAX_HOTKEY_TYPE; ++lIndex)
	{
		for (lIndex2 = 0; lIndex2 < 256; ++lIndex2)
		{
			pstHotkey = GetHotkey((AgcdUIHotkeyType) lIndex, lIndex2);
			if (!pstHotkey)
				continue;

			sprintf(szTemp, "%d:%d", lIndex, lIndex2);
			if (!csStream.WriteValue(AGCMUIMANAGER2_NAME_HOTKEY, szTemp))
				return FALSE;

			sprintf(szTemp, "%d", pstHotkey->m_bCustomizable);
			if (!csStream.WriteValue(AGCMUIMANAGER2_NAME_HOTKEY_FLAG, szTemp))
				return FALSE;

			if (pstHotkey->m_szDescription)
				if (!csStream.WriteValue(AGCMUIMANAGER2_NAME_HOTKEY_DESC, pstHotkey->m_szDescription))
					return FALSE;

			if (!StreamWriteAction(&pstHotkey->m_listAction, &csStream))
				return FALSE;
		}
	}

	// 등록된 모든 Character Template에 대해서...
	lIndex = 0;
	for (pcsUI = GetSequenceUI(&lIndex); pcsUI; pcsUI = GetSequenceUI(&lIndex))
	{
		// Name로 Section을 설정하고
		csStream.SetSection(pcsUI->m_szUIName);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMUIMANAGER2_DATA_TYPE_UI, pcsUI, this))
			return FALSE;
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL			AgcmUIManager2::StreamRead(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				nNumKeys;
	INT32				i;
	const CHAR *		szName;
	AgcdUI *			pcsUI;
	AgcdUIEventMap *	pstEventMap;
	AgcdUIHotkeyType	eHotkeyType;
	AgcdUIHotkey *		pstHotkey;
	CHAR				szError[128];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();

	if (nNumKeys <= 0 || strcmp(csStream.ReadSectionName(0), AGCMUIMANAGER2_NAME_EVENTMAP))
	{
		ASSERT(!"UI File Format is incorrect !!!");
		return FALSE;
	}

	csStream.SetSection(AGCMUIMANAGER2_NAME_EVENTMAP);

	while (csStream.ReadNextValue())
	{
		if (!strcmp(csStream.GetValueName(), AGCMUIMANAGER2_NAME_EVENT_NAME))
		{
			char			*pstrEventName;

			pstrEventName = (char *)csStream.GetValue();

			pstEventMap = GetEventMap( pstrEventName );
			if (!pstEventMap)
			{
				if( !strncmp( pstrEventName , "GAMBLE_OPEN" , 10 )  ||
					!strncmp( pstrEventName , "GAMBLE_CLOSE" , 10 )	)
				{
					// do nothing
					// 국내에서는 겜블이 모듈 등록이 안들어 있어서 에러가 생긴다.
					// 2007/06/25 마고자
				}
				else
				{
					//ASSERT(!"Can't Find Event Map !!!");
				}
			}
			else if (!StreamReadAction(&pstEventMap->m_listAction, &csStream))
				return FALSE;
		}
		else if (!strcmp(csStream.GetValueName(), AGCMUIMANAGER2_NAME_HOTKEY))
		{
			sscanf(csStream.GetValue(), "%d:%d", &eHotkeyType, &i);
			pstHotkey = AddHotkey(eHotkeyType, i);
			while (csStream.ReadNextValue())
			{
				if (!strcmp(csStream.GetValueName(), AGCMUIMANAGER2_NAME_HOTKEY_FLAG))
				{
					pstHotkey->m_bCustomizable = atoi(csStream.GetValue());
				}
				else if (!strcmp(csStream.GetValueName(), AGCMUIMANAGER2_NAME_HOTKEY_DESC))
				{
					SetHotkeyDescription(pstHotkey, (CHAR *) csStream.GetValue());
				}
				else
				{
					csStream.ReadPrevValue();
					break;
				}
			}
			if (pstHotkey && !StreamReadAction(&pstHotkey->m_listAction, &csStream))
			{
				ASSERT(!"Error reading hotkey !!!");
				return FALSE;
			}
		}
	}

	// 각 Section에 대해서...
	for (i = 1; i < nNumKeys; ++i)
	{
		// Section Name은 TID 이다.
		szName = csStream.ReadSectionName(i);

		// Template을 추가한다.
		pcsUI = CreateUI();
		if (!pcsUI)
			return FALSE;

		strncpy(pcsUI->m_szUIName, szName, AGCDUIMANAGER2_MAX_NAME);
		pcsUI->m_szUIName[AGCDUIMANAGER2_MAX_NAME - 1] = '\0';

		if (!AddUI(pcsUI))
		{
			ASSERT(!"AgcmUIManager2::StreamRead() Error Adding UI!!!");

			DestroyUI(pcsUI);

			return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGCMUIMANAGER2_DATA_TYPE_UI, pcsUI, this))
		{
			sprintf(szError, "UI (%s) Reading Error!!!", szName);
			ASSERT(szError);
		}

		pcsUI->m_pcsUIWindow->UpdateChildWindow();
	}

	SetupActions();
	CountUserData();

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgcmUIManager2 *	pThis = (AgcmUIManager2 *) pClass;
	AgcdUI *			pcsUI = (AgcdUI *) pData;
	INT32				lIndex;
	AcUIBase *			pcsBase = pcsUI->m_pcsUIWindow;
	AgcdUIControl *		pcsControl;
	CHAR				szValue[128];

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_TYPE, (INT32) pcsUI->m_eUIType))
		return FALSE;

	sprintf(szValue, "%d:%d:%d:%d", pcsUI->m_bMainUI, pcsUI->m_bAutoClose, pcsUI->m_bEventUI, pcsUI->m_bTransparent);
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_MAIN, szValue))
		return FALSE;

	sprintf(szValue, "%d:%s", pcsUI->m_bUseParentPosition, pcsUI->m_pcsParentUI ? pcsUI->m_pcsParentUI->m_szUIName : "");
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_PARENT, szValue))
		return FALSE;
	
	sprintf(szValue, "%d", pcsUI->m_bInitControlStatus );
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_INITSTATUS, szValue))
		return FALSE;

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_COORDSYSTEM, (INT32) pcsUI->m_eCoordSystem))
		return FALSE;

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_MODE_FLAG, (INT32) pcsUI->m_ulModeFlag))
		return FALSE;

	if (pcsUI->m_pstSetFocusFunction || pcsUI->m_pstKillFocusFunction)
	{
		sprintf(szValue, "%s %s",
				pcsUI->m_pstSetFocusFunction ? pcsUI->m_pstSetFocusFunction->m_szName : ".",
				pcsUI->m_pstKillFocusFunction ? pcsUI->m_pstKillFocusFunction->m_szName : ".");

		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_UI_FOCUS_CB, szValue))
			return FALSE;
	}

	if (!pThis->StreamWriteBase(pcsUI->m_pcsUIWindow, NULL, pStream))
		return FALSE;

	for (lIndex = 0; lIndex < AGCDUI_MAX_MODE; ++lIndex)
	{
		sprintf(szValue, "%d:%d,%d", lIndex, pcsUI->m_alX[lIndex], pcsUI->m_alY[lIndex]);
		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_POSITION_UI, szValue))
			return FALSE;
	}

	lIndex = 0;
	for (pcsControl = pThis->GetSequenceControl(pcsUI, &lIndex); pcsControl; pcsControl = pThis->GetSequenceControl(pcsUI, &lIndex))
	{
		if (pcsControl->m_bDynamic)
			continue;

		if (!pThis->StreamWriteControl(pcsControl, pStream))
			return FALSE;
	}

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteControl(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR				szValue[256];
	INT32				lIndex;
	AcUIBase *			pcsBase = pcsControl->m_pcsBase;
//	AgcdUIControl *		pcsChildControl;
	AgcdUIMessageMap *	pstMessageMap;
	AgcdUIDisplayMap *	pstDisplayMap;

	if (pcsControl->m_pcsParentControl)
		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_PARENT, pcsControl->m_pcsParentControl->m_szName))
			return FALSE;


	sprintf(szValue, "%d:%s",
					 (INT32) pcsControl->m_lType,
					 pcsControl->m_lType == AcUIBase::TYPE_CUSTOM ?
						(pcsControl->m_uoData.m_stCustom.m_pstCustomControl ? pcsControl->m_uoData.m_stCustom.m_pstCustomControl->m_szName : ".")
						: ".");
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_TYPE, szValue))
		return FALSE;

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_NAME, pcsControl->m_szName))
		return FALSE;

	if (!StreamWriteBase(pcsControl->m_pcsBase, pcsControl, pStream))
		return FALSE;

	sprintf(szValue, "%d:%d:%s", pcsControl->m_lUserDataIndex,
							  pcsControl->m_bUseParentUserData,
							  pcsControl->m_pstUserData ? pcsControl->m_pstUserData->m_szName : ".");
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_USERDATA2, szValue))
		return FALSE;

	for (lIndex = 0; lIndex < pcsControl->m_lMessageCount; ++lIndex)
	{		
		pstMessageMap = pcsControl->m_pstMessageMaps + lIndex;
	
		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_MESSAGE, lIndex))
			return FALSE;

		if (!StreamWriteAction(&pstMessageMap->m_listAction, pStream))
			return FALSE;
	}

	pstDisplayMap = &pcsControl->m_stDisplayMap;

	sprintf(szValue, "%d:%d:%f:%d:%d:%d:%d",
					pstDisplayMap->m_stFont.m_lType,
					pstDisplayMap->m_stFont.m_ulColor,
					pstDisplayMap->m_stFont.m_fScale,
					pstDisplayMap->m_stFont.m_bShadow,
					pstDisplayMap->m_stFont.m_eHAlign | (pstDisplayMap->m_stFont.m_eVAlign << 4),
					pstDisplayMap->m_stFont.m_bImageNumber,
					pcsControl->m_pcsBase->m_bStringNumberComma);
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_FONT, szValue))
		return FALSE;

	if (pcsControl->m_pstShowCB || pcsControl->m_bAutoAlign)
	{
		sprintf(szValue, "%d:%s %s",
				pcsControl->m_bAutoAlign,
				pcsControl->m_pstShowCB ? (LPCTSTR) pcsControl->m_pstShowCB->m_szName : ".",
				pcsControl->m_pstShowUD ? (LPCTSTR) pcsControl->m_pstShowUD->m_szName : ".");

		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_SHOW, szValue))
			return FALSE;
	}


	if (pcsControl->m_szTooltip)
		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_TOOLTIP, pcsControl->m_szTooltip))
			return FALSE;

	switch (pstDisplayMap->m_eType)
	{
	case AGCDUI_DISPLAYMAP_TYPE_STATIC:
		{
			sprintf(szValue, "%d:%s", pstDisplayMap->m_eType, pstDisplayMap->m_uoDisplay.m_szStatic);
			if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_DISPLAY, szValue))
				return FALSE;

			break;
		}

	case AGCDUI_DISPLAYMAP_TYPE_VARIABLE:
		{
			AgcdUIUserData *	pstUserData = pstDisplayMap->m_uoDisplay.m_stVariable.m_pstUserData;
			AgcdUIDisplay *		pstDisplay = pstDisplayMap->m_uoDisplay.m_stVariable.m_pstDisplay;

			sprintf(szValue, "%d:%d:%s %s",
					pstDisplayMap->m_eType,
					pstDisplayMap->m_uoDisplay.m_stVariable.m_bControlUserData, 
					pstUserData ? pstUserData->m_szName : ".",
					pstDisplay ? pstDisplay->m_szName : ".");

			if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CONTROL_DISPLAY, szValue))
				return FALSE;

			break;
		}
	}

	switch (pcsControl->m_lType)
	{
	case AcUIBase::TYPE_SCROLL_BUTTON:
	case AcUIBase::TYPE_BUTTON:
		{
			if (!StreamWriteButton(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_SKILL_TREE:
		{
			if (!StreamWriteSkillTree(pcsControl, pStream))
				return FALSE;
		}
	case AcUIBase::TYPE_GRID:
		{
			if (!StreamWriteGrid(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_BAR:
		{
			if (!StreamWriteBar(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_TREE:
		{
			if (!StreamWriteTree(pcsControl, pStream))
				return FALSE;
		}
	case AcUIBase::TYPE_LIST:
		{
			if (!StreamWriteList(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_EDIT:
		{
			if (!StreamWriteEdit(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_SCROLL:
		{
			if (!StreamWriteScroll(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_COMBO:
		{
			if (!StreamWriteCombo(pcsControl, pStream))
				return FALSE;

			break;
		}

	case AcUIBase::TYPE_CLOCK:
		{
			if (!StreamWriteClock(pcsControl, pStream))
				return FALSE;

			break;
		}
	}

	/*
	lIndex = 0;
	for (pcsChildControl = GetSequenceControl(pcsControl, &lIndex); pcsChildControl; pcsChildControl = GetSequenceControl(pcsControl, &lIndex))
	{
		if (!StreamWriteControl(pcsChildControl, pStream))
			return FALSE;
	}
	*/

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteBase(AcUIBase *pcsBase, AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	INT32				lIndex;
	INT32				lImageID;
//	RwTexture *			pstTexture;
	CHAR				szValue[256];
	stStatusInfo *		pstStatus;

	sprintf(szValue, "%d,%d", pcsBase->x, pcsBase->y);
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_POSITION, szValue))
		return FALSE;

	sprintf(szValue, "%d,%d", pcsBase->w, pcsBase->h);
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_SIZE, szValue))
		return FALSE;

	sprintf(szValue, "%d,%d", pcsBase->GetRenderColor(), pcsBase->GetRenderAlpha());
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_COLOR, szValue))
		return FALSE;

	sprintf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d",
			pcsBase->m_Property.bUseInput,
			pcsBase->m_Property.bMovable,
			pcsBase->m_Property.bTopmost,
			pcsBase->m_Property.bModal,
			pcsBase->m_Property.bVisible,
			pcsBase->m_clProperty.bShrink,
			pcsBase->m_bAutoFitString,
			pcsBase->m_bClipImage);
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_WIN_PROPERTY, szValue))
		return FALSE;

	for (lIndex = 0; lIndex < pcsBase->m_csTextureList.GetCount(); ++lIndex)
	{
		CHAR *	szTextureName = pcsBase->m_csTextureList.GetImageName_Index(lIndex, &lImageID);
		if (szTextureName)
		{
			sprintf(szValue, "%d:%s", lImageID, szTextureName);
			if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_TEXTURE, szValue))
				return FALSE;
		}
	}

	for (lIndex = 0; lIndex < ACUIBASE_STATUS_MAX_NUM; ++lIndex)
	{
		pstStatus = pcsBase->GetStatusInfo_Index(lIndex);
		if (pstStatus && pstStatus->m_bSetting)
		{
			sprintf(szValue, "%d:%d,%d,%d,%d:%d:%d:%d:%s %s",
					pstStatus->m_lStatusID,
					pstStatus->m_rectStatusPos.x,
					pstStatus->m_rectStatusPos.y,
					pstStatus->m_rectStatusPos.w,
					pstStatus->m_rectStatusPos.h,
					pstStatus->m_lStatusImageID, 
					pstStatus->m_bVisible,
					pstStatus->m_lStringColor,
					pcsControl ? (pcsControl->m_apstUserData[lIndex] ? pcsControl->m_apstUserData[lIndex]->m_szName : ".") : ".",
					pstStatus->m_szStatusName);

			if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_STATUS2, szValue))
				return FALSE;
		}
	}

	sprintf(szValue, "%d:%d", pcsBase->GetDefaultRenderTexture(), pcsBase->GetDefaultStatus());
	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_DEFAULT_IDS, szValue))
		return FALSE;

	if (pcsBase->m_bExistHandler)
	{
		sprintf(szValue, "%d:%d:%d:%d",
				pcsBase->m_rectHandlerArea.x,
				pcsBase->m_rectHandlerArea.y,
				pcsBase->m_rectHandlerArea.w,
				pcsBase->m_rectHandlerArea.h);

		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_HANDLER, szValue))
			return FALSE;
	}

	AcUIBaseAnimation *	pcsAnim = pcsBase->m_pAnimationData;
	while (pcsAnim)
	{
		sprintf(szValue, "%d:%d:%d:%d",
				pcsAnim->m_lTextureID,
				pcsAnim->m_ulClockCount,
				pcsAnim->m_bRenderString,
				pcsAnim->m_bRenderBaseTexture);

		if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ANIMATION, szValue))
			return FALSE;

		pcsAnim = pcsAnim->m_pNext;
	}

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ANIMATION_DEFAULT, (INT32) pcsBase->m_bAnimationDefault))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteAction(AuList <AgcdUIAction> *plistActions, ApModuleStream *pStream)
{
	INT32				lIndex;
	CHAR				szValue[256];
	AgcdUIAction *		pstAction;

	for (lIndex = 0; lIndex < plistActions->GetCount(); ++lIndex)
	{
		pstAction = &((*plistActions)[lIndex]);

		switch (pstAction->m_eType)
		{
		case AGCDUI_ACTION_UI:
			{
				AgcdUIActionUI *	pstActionUI = &pstAction->m_uoAction.m_stUI;

				sprintf(szValue, "%d:%d:%d:%d:%s",
						pstAction->m_eType,
						pstActionUI->m_eType,
						pstActionUI->m_bGroupAction,
						pstActionUI->m_bSmooth,
						pstActionUI->m_pcsUI ? pstActionUI->m_pcsUI->m_szUIName : "");

				if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION, szValue))
					return FALSE;

				if (pstActionUI->m_eType == AGCDUI_ACTION_UI_MOVE)
				{
					sprintf(szValue, "%d:%d",
						pstActionUI->m_lPosX,
						pstActionUI->m_lPosY);

					if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION_POS, szValue))
						return FALSE;
				}
				else if (pstActionUI->m_pcsAttachUI &&
						 (pstActionUI->m_eType == AGCDUI_ACTION_UI_ATTACH_LEFT ||
						  pstActionUI->m_eType == AGCDUI_ACTION_UI_ATTACH_RIGHT ||
						  pstActionUI->m_eType == AGCDUI_ACTION_UI_ATTACH_TOP ||
						  pstActionUI->m_eType == AGCDUI_ACTION_UI_ATTACH_BOTTOM))
				{
					sprintf(szValue, "%s",
						pstActionUI->m_pcsAttachUI->m_szUIName);

					if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION_ATTACH, szValue))
						return FALSE;
				}

				break;
			}

		case AGCDUI_ACTION_CONTROL:
			{
				AgcdUIActionControl *	pstActionControl = &pstAction->m_uoAction.m_stControl;

				sprintf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%s %s",
						pstAction->m_eType,
						pstActionControl->m_lStatusNum,
						pstActionControl->m_astStatus[0].m_lCurrentStatus,
						pstActionControl->m_astStatus[0].m_lTargetStatus,
						pstActionControl->m_astStatus[1].m_lCurrentStatus,
						pstActionControl->m_astStatus[1].m_lTargetStatus,
						pstActionControl->m_astStatus[2].m_lCurrentStatus,
						pstActionControl->m_astStatus[2].m_lTargetStatus,
						pstActionControl->m_bSaveCurrent,
						pstActionControl->m_pcsUI ? pstActionControl->m_pcsUI->m_szUIName : ".",
						pstActionControl->m_pcsControl ? pstActionControl->m_pcsControl->m_szName : ".");

				if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION, szValue))
					return FALSE;

				break;
			}

		case AGCDUI_ACTION_FUNCTION:
			{
				INT32	lIndex2;

				AgcdUIActionFunction *	pstActionFunction = &pstAction->m_uoAction.m_stFunction;

				sprintf(szValue, "%d:%s",
						pstAction->m_eType,
						pstActionFunction->m_pstFunction ? pstActionFunction->m_pstFunction->m_szName : "");

				if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION, szValue))
					return FALSE;

				for (lIndex2 = 0; lIndex2 < AGCDUIMANAGER2_MAX_ARGS; ++lIndex2)
				{
					if (pstActionFunction->m_astArgs[lIndex2].m_eType == AGCDUI_ARG_TYPE_UI)
					{
						sprintf(szValue, "%d:%d:%s",
								lIndex2,
								pstActionFunction->m_astArgs[lIndex2].m_eType,
								pstActionFunction->m_astArgs[lIndex2].m_pcsUI ? pstActionFunction->m_astArgs[lIndex2].m_pcsUI->m_szUIName : ".");

						if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ARGUMENT2, szValue))
							return FALSE;
					}
					else if (pstActionFunction->m_astArgs[lIndex2].m_eType == AGCDUI_ARG_TYPE_CONTROL)
					{
						sprintf(szValue, "%d:%d:%s %s",
								lIndex2,
								pstActionFunction->m_astArgs[lIndex2].m_eType,
								pstActionFunction->m_astArgs[lIndex2].m_pstControl ? pstActionFunction->m_astArgs[lIndex2].m_pstControl->m_pcsParentUI->m_szUIName : ".",
								pstActionFunction->m_astArgs[lIndex2].m_pstControl ? pstActionFunction->m_astArgs[lIndex2].m_pstControl->m_szName : ".");

						if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ARGUMENT2, szValue))
							return FALSE;
					}
				}

				break;

			}

		case AGCDUI_ACTION_MESSAGE:
			{
				AgcdUIActionMessage * pstActionMessage = &pstAction->m_uoAction.m_stMessage;

				sprintf(szValue, "%d:%d:%d:%s",
							pstAction->m_eType,
							pstActionMessage->m_eMessageType,
							pstActionMessage->m_lColor, 
							pstActionMessage->m_szMessage );

				/*UINT8 lColor[4] = { 255, 255, 255, 255 };			// red, green, blue, alpha

				lColor[3] = (UINT8)((pstActionMessage->m_lColor & 0xff000000) >> 24);
				lColor[0] = (UINT8)((pstActionMessage->m_lColor & 0x00ff0000) >> 16);
				lColor[1] = (UINT8)((pstActionMessage->m_lColor & 0x0000ff00) >> 8);
				lColor[2] = (UINT8)( pstActionMessage->m_lColor & 0x000000ff);

				sprintf(szValue, "%d:%d:%d:%d:%d:%d:%s", 
							&pstAction->m_eType,
							&pstActionMessage->m_eMessageType,
							&lColor[0],								// red
							&lColor[1],								// green 
							&lColor[2],								// blue
							&lColor[3],								// alpha
							&pstActionMessage->m_szMessage);*/

				if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION, szValue))
					return FALSE;

				break;
			}

		case AGCDUI_ACTION_SOUND:
			{
				AgcdUIActionSound *	pstActionSound = &pstAction->m_uoAction.m_stSound;

				sprintf(szValue, "%d:%s",
						pstAction->m_eType,
						pstActionSound->m_szSound);

				if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_ACTION, szValue))
					return FALSE;

				break;
			}
		}
	}

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteButton(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR			szValue[128];
	AcUIButton *	pcsButton = (AcUIButton *) pcsControl->m_pcsBase;

	sprintf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
		pcsButton->GetButtonImage(ACUIBUTTON_MODE_NORMAL),
		pcsButton->GetButtonImage(ACUIBUTTON_MODE_ONMOUSE),
		pcsButton->GetButtonImage(ACUIBUTTON_MODE_CLICK),
		pcsButton->GetButtonImage(ACUIBUTTON_MODE_DISABLE),
		pcsButton->m_stProperty.m_bPushButton,
		pcsButton->m_stProperty.m_bStartOnClickStatus,
		pcsButton->m_lButtonDownStringOffsetX,
		pcsButton->m_lButtonDownStringOffsetY,
		pcsControl->m_uoData.m_stButton.m_eButtonType);

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_BUTTON, szValue))
		return FALSE;

	sprintf(szValue, "%s %s %s %s",
		pcsControl->m_uoData.m_stButton.m_pstCheckBoolean ? (LPCTSTR) pcsControl->m_uoData.m_stButton.m_pstCheckBoolean->m_szName : ".",
		pcsControl->m_uoData.m_stButton.m_pstCheckUserData ? (LPCTSTR) pcsControl->m_uoData.m_stButton.m_pstCheckUserData->m_szName : ".",
		pcsControl->m_uoData.m_stButton.m_pstEnableBoolean ? (LPCTSTR) pcsControl->m_uoData.m_stButton.m_pstEnableBoolean->m_szName : ".",
		pcsControl->m_uoData.m_stButton.m_pstEnableUserData ? (LPCTSTR) pcsControl->m_uoData.m_stButton.m_pstEnableUserData->m_szName : ".");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_BUTTON2, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteSkillTree(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR			szValue[128];
	AcUISkillTree *	pcsSkillTree = (AcUISkillTree *) pcsControl->m_pcsBase;

	sprintf(szValue, "%d:%d:%d:%d",
			pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_1_3),
			pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_2_3),
			pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_3_3),
			pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_1_10));

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_SKILLTREE, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteGrid(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR			szValue[128];
	AcUIGrid *		pcsGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	sprintf(szValue, "%d:%d:%d:%d:%d:%d:%d:%s",
		pcsGrid->GetStartX(), pcsGrid->GetStartY(),
		pcsGrid->GetGapX(), pcsGrid->GetGapY(),
		pcsGrid->GetItemWidth(), pcsGrid->GetItemHeight(),
		pcsControl->m_uoData.m_stGrid.m_bControlUserData,
		pcsControl->m_uoData.m_stGrid.m_pstGrid ? pcsControl->m_uoData.m_stGrid.m_pstGrid->m_szName : "");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_GRID, szValue))
		return FALSE;

	sprintf(szValue, "%d:%d:%d:%d:%d", pcsGrid->GetGridItemMovable(), pcsGrid->GetGridItemMoveItemCopy(), pcsGrid->GetGridItemBottomCountWrite(), pcsGrid->GetReusableDisplayImage(), pcsGrid->GetGridItemDrawImageForeground() );

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_GRID2, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteBar(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR			szValue[128];
	AcUIBar *		pcsBar = (AcUIBar *) pcsControl->m_pcsBase;

	sprintf(szValue, "%d:%d:%d:%d:%s %s %s %s",
		pcsBar->GetEdgeImageID(),
		pcsBar->GetBodyImageID(),
		pcsBar->m_bVertical,
		pcsBar->m_eBarType,
		pcsControl->m_uoData.m_stBar.m_pstMax ? pcsControl->m_uoData.m_stBar.m_pstMax->m_szName : ".",
		pcsControl->m_uoData.m_stBar.m_pstCurrent ? pcsControl->m_uoData.m_stBar.m_pstCurrent->m_szName : ".",
		pcsControl->m_uoData.m_stBar.m_pstDisplayMax ? pcsControl->m_uoData.m_stBar.m_pstDisplayMax->m_szName : ".",
		pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent ? pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent->m_szName : ".");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_BAR, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteList(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	AcUIList *		pcsList = (AcUIList *) pcsControl->m_pcsBase;
	CHAR			szValue[128];

	sprintf(szValue, "%d:%d:%d:%d",
		pcsList->m_lVisibleListItemRow,
		pcsList->m_bStartAtBottom,
		pcsList->m_lItemColumn,
		pcsList->m_lSelectedItemTextureID);

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_LIST, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteTree(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	AcUITree *		pcsTree = (AcUITree *) pcsControl->m_pcsBase;
	CHAR			szValue[128];

	sprintf(szValue, "%d:%s",
		pcsTree->m_lItemDepthMargin,
		pcsControl->m_uoData.m_stTree.m_pstDepthUserData ? pcsControl->m_uoData.m_stTree.m_pstDepthUserData->m_szName : ".");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_TREE, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteEdit(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	AcUIEdit *		pcsEdit = (AcUIEdit *) pcsControl->m_pcsBase;
	CHAR			szValue[128];

	sprintf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
			pcsEdit->m_bPasswordEdit,
			pcsEdit->GetTextMaxLength(),
			pcsEdit->m_bMultiLine,
			pcsEdit->m_bReadOnly,
			pcsEdit->m_bAutoLF,
			pcsEdit->m_bEnableTag,
			pcsEdit->m_bFilterCase,
			pcsEdit->m_bReleaseEditInputEnd,
			pcsEdit->m_bForHotkey);

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_EDIT, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteScroll(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	AcUIScroll *	pcsScroll = (AcUIScroll *) pcsControl->m_pcsBase;
	CHAR			szValue[128];

	sprintf(szValue, "%d:%d:%d:%f:%d:%s %s %s %s",
					pcsScroll->m_bVScroll,
					pcsScroll->m_lMinPosition,
					pcsScroll->m_lMaxPosition,
					pcsScroll->m_fScrollUnit,
					pcsScroll->m_bMoveByUnit,
					pcsControl->m_uoData.m_stScroll.m_pstCurrent ? pcsControl->m_uoData.m_stScroll.m_pstCurrent->m_szName : ".",
					pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent ? pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent->m_szName : ".",
					pcsControl->m_uoData.m_stScroll.m_pstMax ? pcsControl->m_uoData.m_stScroll.m_pstMax->m_szName : ".",
					pcsControl->m_uoData.m_stScroll.m_pstDisplayMax ? pcsControl->m_uoData.m_stScroll.m_pstDisplayMax->m_szName : ".");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_SCROLL, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteCombo(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	AcUICombo *		pcsCombo = (AcUICombo *) pcsControl->m_pcsBase;
	CHAR			szValue[128];

	sprintf(szValue, "%d:%d:%d:%d:%d",
					pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_NORMAL),
					pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_ONMOUSE),
					pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_CLICK),
					pcsCombo->GetOpenLineHeight(),
					pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_BOTTOM));

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_COMBO, szValue))
		return FALSE;

	return TRUE;
}

BOOL			AgcmUIManager2::StreamWriteClock(AgcdUIControl *pcsControl, ApModuleStream *pStream)
{
	CHAR			szValue[128];
	AcUIClock *		pcsClock = (AcUIClock *) pcsControl->m_pcsBase;

	sprintf(szValue, "%d:%d:%s %s %s %s",
		pcsClock->GetAlphaImageID(),
		pcsClock->m_eClockType,
		pcsControl->m_uoData.m_stClock.m_pstMax ? pcsControl->m_uoData.m_stClock.m_pstMax->m_szName : ".",
		pcsControl->m_uoData.m_stClock.m_pstCurrent ? pcsControl->m_uoData.m_stClock.m_pstCurrent->m_szName : ".",
		pcsControl->m_uoData.m_stClock.m_pstDisplayMax ? pcsControl->m_uoData.m_stClock.m_pstDisplayMax->m_szName : ".",
		pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent ? pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent->m_szName : ".");

	if (!pStream->WriteValue(AGCMUIMANAGER2_NAME_CLOCK, szValue))
		return FALSE;

	return TRUE;
}

static void AdjustUIPos( int & x, int & y, int & w, int & h )
{
	RwRaster * raster = RwCameraGetRaster( g_pEngine->m_pCamera );

	if( !raster )
		return;

	float scale = raster->height / AgcmUIManager2::GetModeHeight();

	float halfW = AgcmUIManager2::GetModeWidth() * 0.5f;
	float H = AgcmUIManager2::GetModeHeight();

	float cw = raster->width * 0.5f;

	if( (x < halfW) && (halfW < (x + w)) )
	{
		x = (int)(cw - (halfW - x));
	}
	else if( x < halfW )
	{
		x = (int)(x*(raster->width)/AgcmUIManager2::GetModeWidth());
	}
	else
	{
		x = (int)((raster->width)-(AgcmUIManager2::GetModeWidth()-x));
	}

	float ch = (float)raster->height;

	if( y < H*0.5f )
	{
		y = (int)(y*ch/H);
	}
	else
	{
		y = (int)(ch - (H - y));
	}
}

static void AdjustUIPos( AcUIBase * pcsBase, int &x, int &y )
{
	if( !pcsBase )
		return;

	//-----------------------------------------------------------------------
	// 해상도에 맞춰서 좌표 수정
	RwRaster * raster = RwCameraGetRaster( g_pEngine->m_pCamera );

	if( raster )
	{
		if( pcsBase->w == AgcmUIManager2::GetModeWidth() && pcsBase->h == AgcmUIManager2::GetModeHeight()
			|| ( pcsBase->w == raster->width && pcsBase->h == raster->height ) )
		{
			pcsBase->w = raster->width;
			pcsBase->h = raster->height;
		}
		else
		{
			int w = pcsBase->w;
			int h = pcsBase->h;
			AdjustUIPos( x, y, w, h );
			pcsBase->w = w;
			pcsBase->h = h;
		}
	}
	//-----------------------------------------------------------------------
}

BOOL			AgcmUIManager2::StreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR *		szValueName;
	const CHAR *		szValue;
	ApSafeArray<CHAR, 128>	szTemp;
	ApSafeArray<CHAR, 128>	szTemp2;
	ApSafeArray<CHAR, 128>	szTemp3;
	ApSafeArray<CHAR, 128>	szTemp4;
	INT32				lImageID;
	AgcmUIManager2 *	pThis = (AgcmUIManager2 *) pClass;
	AgcdUI *			pcsUI = (AgcdUI *) pData;
	AcUIBase *			pcsBase = pcsUI->m_pcsUIWindow;
	AgcdUIControl *		pcsControl = NULL;
	AgcdUIAction *		pstAction = NULL;
	AgcdUIMessageMap *	pstMessageMap = NULL;
	AgcdUIControl *		pcsParentControl = NULL;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();
		szValue		= pStream->GetValue();

		szTemp.MemSetAll();
		szTemp2.MemSetAll();
		szTemp3.MemSetAll();
		szTemp4.MemSetAll();

		if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_POSITION))
		{
			INT32	x, y;

			sscanf(szValue, "%d,%d", &x, &y);
			pcsBase->MoveWindow(x, y, pcsBase->w, pcsBase->h);

			if (pcsUI && !pcsControl)
			{
				INT32	lIndex;

				for (lIndex = 0; lIndex < AGCDUI_MAX_MODE; ++lIndex)
				{
					pcsUI->m_alX[lIndex] = x;
					pcsUI->m_alY[lIndex] = y;
				}
			}

			if (pcsControl && pcsControl->m_pcsParentControl)
			{
				if (pcsControl->m_lType == AcUIBase::TYPE_LIST_ITEM && pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_LIST)
				{
					((AcUIList *) pcsControl->m_pcsParentControl->m_pcsBase)->SetListItemWindowMoveInfo(pcsBase->w, pcsBase->h, x, y);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_TREE_ITEM && pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_TREE)
				{
					((AcUITree *) pcsControl->m_pcsParentControl->m_pcsBase)->SetListItemWindowMoveInfo(pcsBase->w, pcsBase->h, x, y);
				}
			}
		}
		if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_POSITION_UI))
		{
			INT32	lMode;
			INT32	x, y;

			sscanf(szValue, "%d:%d,%d", &lMode, &x, &y);

			AdjustUIPos( pcsBase, x, y );

			if (pThis->m_eMode == lMode)
				pcsBase->MoveWindow(x, y, pcsBase->w, pcsBase->h);

			if (pcsUI && !pcsControl)
			{
				pcsUI->m_alX[lMode] = x;
				pcsUI->m_alY[lMode] = y;
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_SIZE))
		{
			INT32	w, h;

			sscanf(szValue, "%d,%d", &w, &h);
			pcsBase->MoveWindow(pcsBase->x, pcsBase->y, w, h);

			/*
			if (pcsUI && !pcsControl)
			{
				pcsUI->m_lX = pcsBase->x;
				pcsUI->m_lY = pcsBase->y;
			}
			*/

			if (pcsControl && pcsControl->m_pcsParentControl)
			{
				if (pcsControl->m_lType == AcUIBase::TYPE_LIST_ITEM && pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_LIST)
				{
					((AcUIList *) pcsControl->m_pcsParentControl->m_pcsBase)->SetListItemWindowMoveInfo(w, h, pcsBase->x, pcsBase->y);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_TREE_ITEM && pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_TREE)
				{
					((AcUIList *) pcsControl->m_pcsParentControl->m_pcsBase)->SetListItemWindowMoveInfo(w, h, pcsBase->x, pcsBase->y);
				}
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_COLOR))
		{
			DWORD		dwColor;
			INT32		lAlpha;
			UINT8		cAlpha;

			sscanf(szValue, "%d,%d", &dwColor, &lAlpha);

			cAlpha = (UINT8) lAlpha;

			pcsBase->SetRenderColor(dwColor);
			pcsBase->SetRenderAlpha(cAlpha);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_WIN_PROPERTY))
		{
			sscanf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d",
				&pcsBase->m_Property.bUseInput,
				&pcsBase->m_Property.bMovable,
				&pcsBase->m_Property.bTopmost,
				&pcsBase->m_Property.bModal,
				&pcsBase->m_Property.bVisible,
				&pcsBase->m_clProperty.bShrink,
				&pcsBase->m_bAutoFitString,
				&pcsBase->m_bClipImage);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_TEXTURE))
		{
			sscanf(szValue, "%d:%s", &lImageID, &szTemp[0]);
			pcsBase->m_csTextureList.AddImage(&szTemp[0], FALSE);
			pcsBase->m_csTextureList.SetImageID_Name(lImageID, &szTemp[0], FALSE);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_STATUS))
		{
			stStatusInfo		stStatus;
			AgcdUIUserData *	pstStatusUserData;

			sscanf(szValue, "%d:%d,%d,%d,%d:%d:%d:%s %s",
					&stStatus.m_lStatusID,
					&stStatus.m_rectStatusPos.x,
					&stStatus.m_rectStatusPos.y,
					&stStatus.m_rectStatusPos.w,
					&stStatus.m_rectStatusPos.h,
					&stStatus.m_lStatusImageID, 
					&stStatus.m_bVisible,
					&szTemp[0],
					stStatus.m_szStatusName);

			bool notAdjust = false;

			if( (pcsBase->pParent && pcsBase->pParent->pParent) 
				|| (pcsControl && !strstr(pcsControl->m_szName, "CTL_BTN_CHARINFO") && !strstr(pcsControl->m_szName, "CTL_BTN_CHARINFO") ) )
				notAdjust = true;

			if( !notAdjust )
				AdjustUIPos( stStatus.m_rectStatusPos.x, stStatus.m_rectStatusPos.y, stStatus.m_rectStatusPos.w, stStatus.m_rectStatusPos.h );

			pcsBase->SetStatusInfo(stStatus.m_rectStatusPos.x,
					stStatus.m_rectStatusPos.y,
					stStatus.m_rectStatusPos.w,
					stStatus.m_rectStatusPos.h,
					stStatus.m_lStatusImageID,
					stStatus.m_szStatusName,
					stStatus.m_lStatusID,
					stStatus.m_bVisible);

			pstStatusUserData = pThis->GetUserData(&szTemp[0]);
			if (pstStatusUserData && pcsControl)
			{
				pcsControl->m_apstUserData[stStatus.m_lStatusID & (ACUIBASE_STATUS_ID_BIT_CONSTANT - 1)] = pstStatusUserData;
			}

		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_STATUS2))
		{
			stStatusInfo		stStatus;
			AgcdUIUserData *	pstStatusUserData;

			sscanf(szValue, "%d:%d,%d,%d,%d:%d:%d:%d:%s %s",
					&stStatus.m_lStatusID,
					&stStatus.m_rectStatusPos.x,
					&stStatus.m_rectStatusPos.y,
					&stStatus.m_rectStatusPos.w,
					&stStatus.m_rectStatusPos.h,
					&stStatus.m_lStatusImageID, 
					&stStatus.m_bVisible,
					&stStatus.m_lStringColor,
					&szTemp[0],
					stStatus.m_szStatusName);

			bool notAdjust = false;

			if( (pcsBase->pParent && pcsBase->pParent->pParent) 
				|| (pcsControl && !strstr(pcsControl->m_szName, "CTL_BTN_CHARINFO") && !strstr(pcsControl->m_szName, "CTL_BTN_CHARINFO") ) )
				notAdjust = true;

			if( !notAdjust )
				AdjustUIPos( stStatus.m_rectStatusPos.x, stStatus.m_rectStatusPos.y, stStatus.m_rectStatusPos.w, stStatus.m_rectStatusPos.h );

			pcsBase->SetStatusInfo(stStatus.m_rectStatusPos.x,
					stStatus.m_rectStatusPos.y,
					stStatus.m_rectStatusPos.w,
					stStatus.m_rectStatusPos.h,
					stStatus.m_lStatusImageID,
					stStatus.m_szStatusName,
					stStatus.m_lStatusID,
					stStatus.m_bVisible,
					stStatus.m_lStringColor);

			pstStatusUserData = pThis->GetUserData(&szTemp[0]);
			if (pstStatusUserData && pcsControl)
			{
				pcsControl->m_apstUserData[stStatus.m_lStatusID & (ACUIBASE_STATUS_ID_BIT_CONSTANT - 1)] = pstStatusUserData;
			}

		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_DEFAULT_IDS))
		{
			INT32	lImageID, lStatusID;

			sscanf(szValue, "%d:%d", &lImageID, &lStatusID);

			if (!lImageID)
				lImageID = -1;

			pcsBase->SetDefaultRenderTexture(lImageID);
			pcsBase->SetDefaultStatus(lStatusID);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_HANDLER))
		{
			if (!pcsBase)
				return FALSE;

			INT32	alHandler[4];

			sscanf(szValue, "%d:%d:%d:%d",
				   alHandler + 0,
				   alHandler + 1,
				   alHandler + 2,
				   alHandler + 3);

			pcsBase->SetHandler(alHandler[0], alHandler[1], alHandler[2], alHandler[3]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ANIMATION_DEFAULT))
		{
			if (!pcsBase)
				return FALSE;

			pcsBase->m_bAnimationDefault = atoi(szValue);
			if (pcsBase->m_bAnimationDefault)
				pcsBase->StartAnimation();
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ANIMATION))
		{
			if (!pcsBase)
				return FALSE;

			INT32	alTemp[4];

			sscanf(szValue, "%d:%d:%d:%d",
					alTemp + 0,
					alTemp + 1,
					alTemp + 2,
					alTemp + 3);

			pcsBase->AddAnimation(alTemp[0], alTemp[1], alTemp[2], alTemp[3]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_PARENT))
		{
			pcsParentControl = pThis->GetControl(pcsUI, (CHAR *) szValue);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_TYPE))
		{
			INT32				lType;
			AgcdUICControl *	pcsCControl = NULL;

			sscanf(szValue, "%d:%s", &lType, &szTemp[0]);

			if (lType == AcUIBase::TYPE_CUSTOM)
				pcsCControl = pThis->GetCControl(&szTemp[0]);

			pcsControl = pThis->AddControl(pcsUI, lType, 0, 0, pcsParentControl ? pcsParentControl->m_pcsBase : NULL, NULL, pcsCControl);

			if (pcsControl)
				pcsBase = pcsControl->m_pcsBase;
			else
			{
				ASSERT(!"Can't AddControl Control!!!");
			}

			if (pcsControl && pcsControl->m_pcsParentControl)
			{
				AcUIBase *	pcsParentBase = pcsParentControl->m_pcsBase;

				if (pcsControl->m_lType == AcUIBase::TYPE_LIST_ITEM && pcsParentControl->m_lType == AcUIBase::TYPE_LIST)
				{
					((AcUIList *) pcsParentBase)->SetListItemWindowMoveInfo(pcsBase->w, pcsBase->h, pcsBase->x, pcsBase->y);

					pcsUI->m_pcsUIWindow->UpdateChildWindow();
					pcsParentControl->m_uoData.m_stList.m_pcsListItemFormat = pcsControl;
					pcsParentControl->m_pcsBase->DeleteChild(pcsControl->m_pcsBase, FALSE, FALSE);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_TREE_ITEM && pcsParentControl->m_lType == AcUIBase::TYPE_TREE)
				{
					((AcUIList *) pcsParentBase)->SetListItemWindowMoveInfo(pcsBase->w, pcsBase->h, pcsBase->x, pcsBase->y);

					pcsUI->m_pcsUIWindow->UpdateChildWindow();
					pcsParentControl->m_uoData.m_stTree.m_pcsListItemFormat = pcsControl;
					pcsParentControl->m_pcsBase->DeleteChild(pcsControl->m_pcsBase, FALSE, FALSE);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_SCROLL_BUTTON)
				{
					((AcUIScroll *) pcsParentBase)->SetScrollButton((AcUIScrollButton *) pcsBase);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_BUTTON && pcsParentControl->m_lType == AcUIBase::TYPE_COMBO)
				{
					((AcUICombo *) pcsParentBase)->SetComboButton((AcUIButton *) pcsBase);
				}
				else if (pcsControl->m_lType == AcUIBase::TYPE_BUTTON && pcsParentControl->m_lType == AcUIBase::TYPE_SCROLL)
				{
					if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_SCROLL_DOWN)
						((AcUIScroll *) pcsParentBase)->SetScrollDownButton((AcUIButton *) pcsBase);
					else if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_SCROLL_UP)
						((AcUIScroll *) pcsParentBase)->SetScrollUpButton((AcUIButton *) pcsBase);
				}
			}

			pcsParentControl = NULL;
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_NAME))
		{
			if (!pcsControl)
				return FALSE;

			strcpy(pcsControl->m_szName, szValue);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_PARENT_UD))
		{
			if (!pcsControl)
				return FALSE;

			pcsControl->m_bUseParentUserData = atoi(szValue);
			if (pcsControl->m_bUseParentUserData && pcsControl->m_pcsParentControl)
			{
				pcsControl->m_pstUserData = pcsControl->m_pcsParentControl->m_pstUserData;
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_USERDATA))
		{
			if (!pcsControl)
				return FALSE;

			if (!pcsControl->m_bUseParentUserData)
				pcsControl->m_pstUserData = pThis->GetUserData((CHAR *) szValue);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_USERDATA2))
		{
			if (!pcsControl)
				return FALSE;

			sscanf(szValue, "%d:%d:%s",
					&pcsControl->m_lUserDataIndex,
					&pcsControl->m_bUseParentUserData,
					&szTemp[0]);

			if (!pcsControl->m_bUseParentUserData)
				pcsControl->m_pstUserData = pThis->GetUserData(&szTemp[0]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_SHOW))
		{
			if (!pcsControl)
				return FALSE;

			sscanf(szValue, "%d:%s %s",
					&pcsControl->m_bAutoAlign,
					&szTemp[0],
					&szTemp2[0]);

			pcsControl->m_pstShowCB = pThis->GetBoolean(&szTemp[0]);
			pcsControl->m_pstShowUD = pThis->GetUserData(&szTemp2[0]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_TOOLTIP))
		{
			if (!pcsControl)
				return FALSE;

			pThis->SetControlTooltip(pcsControl, (CHAR *) szValue);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_MESSAGE))
		{
			INT32	lMessageID;

			if (!pcsControl)
				return FALSE;

			lMessageID = atoi(szValue);
			if (lMessageID >= m_alControlMessages[pcsControl->m_pcsBase->m_nType])
				return FALSE;
			
			pstMessageMap = pcsControl->m_pstMessageMaps + lMessageID;
			
			if (!pThis->StreamReadAction(&pstMessageMap->m_listAction, pStream))
				return FALSE;
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_FONT))
		{
			if (!pcsControl)
				return FALSE;

			AgcdUIDisplayMap *	pstDisplayMap = &pcsControl->m_stDisplayMap;
			INT32				lAlign;

			pstDisplayMap->m_stFont.m_bImageNumber = FALSE;

			sscanf(szValue, "%d:%d:%f:%d:%d:%d:%d",
					&pstDisplayMap->m_stFont.m_lType,
					&pstDisplayMap->m_stFont.m_ulColor,
					&pstDisplayMap->m_stFont.m_fScale,
					&pstDisplayMap->m_stFont.m_bShadow,
					&lAlign,
					&pstDisplayMap->m_stFont.m_bImageNumber,
					&pcsControl->m_pcsBase->m_bStringNumberComma);

			pThis->SetControlDisplayFont(pcsControl,
					pstDisplayMap->m_stFont.m_lType,
					pstDisplayMap->m_stFont.m_ulColor,
					pstDisplayMap->m_stFont.m_fScale,
					pstDisplayMap->m_stFont.m_bShadow,
					(AcUIBaseHAlign) (lAlign & 0x0f),
					(AcUIBaseVAlign) (lAlign >> 4),
					pstDisplayMap->m_stFont.m_bImageNumber);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CONTROL_DISPLAY))
		{
			AgcdUIDisplayMap *	pstDisplayMap;
			INT32				lDisplayType;

			if (!pcsControl)
				return FALSE;

			lDisplayType = atoi(szValue);
			pstDisplayMap = &pcsControl->m_stDisplayMap;
			switch (lDisplayType)
			{
			case AGCDUI_DISPLAYMAP_TYPE_STATIC:
				{
					UINT32	ulIndex;

					for (ulIndex = 0; ulIndex < strlen(szValue); ++ulIndex)
						if (szValue[ulIndex] == ':')
							break;

					if (ulIndex < strlen(szValue))
						pThis->SetControlDisplayMap(pcsControl, (CHAR *) szValue + ulIndex + 1);

					break;
				}

			case AGCDUI_DISPLAYMAP_TYPE_VARIABLE:
				{
					sscanf(szValue, "%d:%d:%s %s",
							&pstDisplayMap->m_eType,
							&pstDisplayMap->m_uoDisplay.m_stVariable.m_bControlUserData,
							&szTemp[0],
							&szTemp2[0]);

					pstDisplayMap->m_uoDisplay.m_stVariable.m_pstUserData = pThis->GetUserData(&szTemp[0]);
					pstDisplayMap->m_uoDisplay.m_stVariable.m_pstDisplay = pThis->GetDisplay(&szTemp2[0]);

					pThis->SetControlDisplayMap(pcsControl,
							pstDisplayMap->m_uoDisplay.m_stVariable.m_pstUserData,
							pstDisplayMap->m_uoDisplay.m_stVariable.m_pstDisplay,
							pstDisplayMap->m_uoDisplay.m_stVariable.m_bControlUserData);

					break;
				}
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ACTION))
		{
			if (!pstMessageMap)
				return FALSE;

			pStream->ReadPrevValue();
			if (!pThis->StreamReadAction(&pstMessageMap->m_listAction, pStream))
				return FALSE;
			/*
			INT32	lActionType;

			pstAction = pstMessageMap->m_astAction + pstMessageMap->m_lCount;

			lActionType = atoi(szValue);
			switch (lActionType)
			{
			case AGCDUI_ACTION_UI:
				{
					AgcdUIActionUI *	pstActionUI = &pstAction->m_uoAction.m_stUI;

					sscanf(szValue, "%d:%d:%s",
							&pstAction->m_eType,
							&pstActionUI->m_eType,
							pstActionUI->m_szUIName);

					pstActionUI->m_pcsUI = pThis->GetUI(pstActionUI->m_szUIName);

					break;
				}

			case AGCDUI_ACTION_CONTROL:
				{
					AgcdUIActionControl *	pstActionControl = &pstAction->m_uoAction.m_stControl;

					sscanf(szValue, "%d:%d:%d:%s %s",
							&pstAction->m_eType,
							&pstActionControl->m_lStatus,
							&pstActionControl->m_bSaveCurrent,
							pstActionControl->m_szUIName,
							pstActionControl->m_szControlName);

					pstActionControl->m_pcsUI = pThis->GetUI(pstActionControl->m_szUIName);
					pstActionControl->m_pcsControl = pThis->GetControl(pstActionControl->m_pcsUI, pstActionControl->m_szControlName);

					break;
				}

			case AGCDUI_ACTION_FUNCTION:
				{
					AgcdUIActionFunction *	pstActionFunction = &pstAction->m_uoAction.m_stFunction;

					sscanf(szValue, "%d:%s",
							&pstAction->m_eType,
							szTemp);

					pstActionFunction->m_pstFunction = pThis->GetFunction(szTemp);

					break;
				}

			default:
				return FALSE;
			}

			++(pstMessageMap->m_lCount);
			*/
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_BUTTON))
		{
			if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_BUTTON && pcsControl->m_lType != AcUIBase::TYPE_SCROLL_BUTTON))
				return FALSE;

			AcUIButton *	pcsButton = (AcUIButton *) pcsControl->m_pcsBase;
			INT32			alImageID[4];

			sscanf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
				alImageID + 0,
				alImageID + 1,
				alImageID + 2,
				alImageID + 3,
				&pcsButton->m_stProperty.m_bPushButton,
				&pcsButton->m_stProperty.m_bStartOnClickStatus,
				&pcsButton->m_lButtonDownStringOffsetX,
				&pcsButton->m_lButtonDownStringOffsetY,
				&pcsControl->m_uoData.m_stButton.m_eButtonType);

			pcsButton->SetButtonImage(alImageID[0], ACUIBUTTON_MODE_NORMAL);
			pcsButton->SetButtonImage(alImageID[1], ACUIBUTTON_MODE_ONMOUSE);
			pcsButton->SetButtonImage(alImageID[2], ACUIBUTTON_MODE_CLICK);
			pcsButton->SetButtonImage(alImageID[3], ACUIBUTTON_MODE_DISABLE);

			if (pcsButton->m_stProperty.m_bStartOnClickStatus)
				pcsButton->SetButtonMode(ACUIBUTTON_MODE_CLICK);

			if (pcsControl->m_pcsParentControl && pcsControl->m_uoData.m_stButton.m_eButtonType != AGCDUI_BUTTON_TYPE_NORMAL)
			{
				if (pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_SCROLL)
				{
					if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_SCROLL_DOWN)
						((AcUIScroll *) pcsControl->m_pcsParentControl->m_pcsBase)->SetScrollDownButton(pcsButton);
					else if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_SCROLL_UP)
						((AcUIScroll *) pcsControl->m_pcsParentControl->m_pcsBase)->SetScrollUpButton(pcsButton);
				}
				else if (pcsControl->m_pcsParentControl->m_lType == AcUIBase::TYPE_TREE_ITEM)
				{
					if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_CLOSE)
						((AcUITreeItem *) pcsControl->m_pcsParentControl->m_pcsBase)->SetTreeButtonClose(pcsButton);
					if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_OPEN)
						((AcUITreeItem *) pcsControl->m_pcsParentControl->m_pcsBase)->SetTreeButtonOpen(pcsButton);
				}
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_BUTTON2))
		{
			if (!pcsControl ||
				(pcsControl->m_lType != AcUIBase::TYPE_BUTTON && pcsControl->m_lType != AcUIBase::TYPE_SCROLL_BUTTON))
				return FALSE;

			sscanf(szValue, "%s %s %s %s",
				&szTemp[0],
				&szTemp2[0],
				&szTemp3[0],
				&szTemp4[0]);

			pcsControl->m_uoData.m_stButton.m_pstCheckBoolean = pThis->GetBoolean(&szTemp[0]);
			pcsControl->m_uoData.m_stButton.m_pstCheckUserData = pThis->GetUserData(&szTemp2[0]);
			pcsControl->m_uoData.m_stButton.m_pstEnableBoolean = pThis->GetBoolean(&szTemp3[0]);
			pcsControl->m_uoData.m_stButton.m_pstEnableUserData = pThis->GetUserData(&szTemp4[0]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_GRID))
		{
			if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
				return FALSE;

			AcUIGrid *		pcsGrid = (AcUIGrid *) pcsControl->m_pcsBase;
			INT32			alValue[7];

			sscanf(szValue, "%d:%d:%d:%d:%d:%d:%d:%s",
				alValue + 0,
				alValue + 1,
				alValue + 2,
				alValue + 3,
				alValue + 4,
				alValue + 5,
				alValue + 6,
				&szTemp[0]);

			pThis->SetControlGrid(pcsControl, pThis->GetUserData(&szTemp[0]), (BOOL) alValue[6]);

			pcsGrid->SetGridItemDrawInfo(alValue[0], alValue[1], alValue[2], alValue[3], alValue[4], alValue[5]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_GRID2))
		{
			if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
				return FALSE;

			AcUIGrid *		pcsGrid = (AcUIGrid *) pcsControl->m_pcsBase;
			BOOL			bMovable			= FALSE;
			BOOL			bMoveItemCopy		= FALSE;
			BOOL			bGridItemWriteCount	= FALSE;
			INT32			lReusableDisplayTextureID	= 0;
			BOOL			bGridItemDrawImageForeground	= FALSE;

			sscanf(szValue, "%d:%d:%d:%d:%d", &bMovable, &bMoveItemCopy, &bGridItemWriteCount, &lReusableDisplayTextureID, &bGridItemDrawImageForeground );

			if (bMovable < 0) bMovable = FALSE;
			if (bMoveItemCopy < 0) bMoveItemCopy = FALSE;
			if (bGridItemWriteCount < 0) bGridItemWriteCount = FALSE;
			if (lReusableDisplayTextureID < 0) lReusableDisplayTextureID = FALSE;
			if (bGridItemDrawImageForeground < 0) bGridItemDrawImageForeground = FALSE;

			pcsGrid->SetGridItemMovable(bMovable)						;
			pcsGrid->SetGridItemMoveItemCopy(bMoveItemCopy)				;
			pcsGrid->SetGridItemBottomCountWrite(bGridItemWriteCount)	;

			pcsGrid->SetReusableDisplayImage(lReusableDisplayTextureID)	;

			pcsGrid->SetGridItemDrawImageForeground(bGridItemDrawImageForeground);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_SKILLTREE))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE)
				return FALSE;

			AcUISkillTree *	pcsSkillTree = (AcUISkillTree *) pcsControl->m_pcsBase;
			INT32			alValue[4];

			sscanf(szValue, "%d:%d:%d:%d",
				alValue + 0,
				alValue + 1,
				alValue + 2,
				alValue + 3);

			pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_1_3, alValue[0]);
			pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_2_3, alValue[1]);
			pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_3_3, alValue[2]);
			pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_1_10, alValue[3]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_BAR))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_BAR)
				return FALSE;

			AcUIBar *		pcsBar = (AcUIBar *) pcsControl->m_pcsBase;
			INT32			alValue[2];

			sscanf(szValue, "%d:%d:%d:%d:%s %s %s %s",
				alValue + 0,
				alValue + 1,
				&pcsBar->m_bVertical,
				&pcsBar->m_eBarType,
				&szTemp[0],
				&szTemp2[0],
				&szTemp3[0],
				&szTemp4[0]);

			pcsBar->SetEdgeImageID(alValue[0]);
			pcsBar->SetBodyImageID(alValue[1]);

			pcsControl->m_uoData.m_stBar.m_pstMax = pThis->GetUserData(&szTemp[0]);
			pcsControl->m_uoData.m_stBar.m_pstCurrent = pThis->GetUserData(&szTemp2[0]);
			pcsControl->m_uoData.m_stBar.m_pstDisplayMax = pThis->GetDisplay(&szTemp3[0]);
			pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent = pThis->GetDisplay(&szTemp4[0]);

			pThis->RefreshControl(pcsControl);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_BAR2))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_BAR)
				return FALSE;

			AcUIBar *		pcsBar = (AcUIBar *) pcsControl->m_pcsBase;
			INT32			alValue[2];

			sscanf(szValue, "%d:%d:%d:%s %s %s %s",
				alValue + 0,
				alValue + 1,
				&pcsBar->m_bVertical,
				&szTemp[0],
				&szTemp2[0],
				&szTemp3[0],
				&szTemp4[0]);

			pcsBar->SetEdgeImageID(alValue[0]);
			pcsBar->SetBodyImageID(alValue[1]);

			pcsControl->m_uoData.m_stBar.m_pstMax = pThis->GetUserData(&szTemp[0]);
			pcsControl->m_uoData.m_stBar.m_pstCurrent = pThis->GetUserData(&szTemp2[0]);
			pcsControl->m_uoData.m_stBar.m_pstDisplayMax = pThis->GetDisplay(&szTemp3[0]);
			pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent = pThis->GetDisplay(&szTemp4[0]);

			pThis->RefreshControl(pcsControl);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_LIST))
		{
			if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_LIST && pcsControl->m_lType != AcUIBase::TYPE_TREE))
				return FALSE;

			AcUIList *	pcsList = (AcUIList *) pcsControl->m_pcsBase;
			INT32		lVisibleItem = 1;
			INT32		lColumn = 1;

			sscanf(szValue, "%d:%d:%d:%d",
				&lVisibleItem,
				&pcsList->m_bStartAtBottom,
				&lColumn,
				&pcsList->m_lSelectedItemTextureID);

			((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowVisibleRow(lVisibleItem);
			((AcUIList *) pcsControl->m_pcsBase)->SetListItemColumn(lColumn);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_TREE))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_TREE)
				return FALSE;

			AcUITree *	pcsTree = (AcUITree *) pcsControl->m_pcsBase;

			sscanf(szValue, "%d:%s",
				&pcsTree->m_lItemDepthMargin,
				&szTemp[0]);

			pcsControl->m_uoData.m_stTree.m_pstDepthUserData = pThis->GetUserData(&szTemp[0]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_EDIT))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_EDIT)
				return FALSE;

			AcUIEdit *	pcsEdit = (AcUIEdit *) pcsControl->m_pcsBase;
			UINT32		ulMaxLength = 0;

			sscanf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d",
					&pcsEdit->m_bPasswordEdit,
					&ulMaxLength,
					&pcsEdit->m_bMultiLine,
					&pcsEdit->m_bReadOnly,
					&pcsEdit->m_bAutoLF,
					&pcsEdit->m_bEnableTag,
					&pcsEdit->m_bFilterCase,
					&pcsEdit->m_bReleaseEditInputEnd,
					&pcsEdit->m_bForHotkey);

			pcsEdit->SetTextMaxLength(ulMaxLength);
		}		
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_SCROLL))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_SCROLL)
				return FALSE;

			AcUIScroll *	pcsScroll = (AcUIScroll *) pcsControl->m_pcsBase;

			sscanf(szValue, "%d:%d:%d:%f:%d:%s %s %s %s",
							&pcsScroll->m_bVScroll,
							&pcsScroll->m_lMinPosition,
							&pcsScroll->m_lMaxPosition,
							&pcsScroll->m_fScrollUnit,
							&pcsScroll->m_bMoveByUnit,
							&szTemp[0],
							&szTemp2[0],
							&szTemp3[0],
							&szTemp4[0]);

			pcsScroll->SetScrollButtonInfo(pcsScroll->m_bVScroll, pcsScroll->m_lMinPosition, pcsScroll->m_lMaxPosition, pcsScroll->m_fScrollUnit, pcsScroll->m_bMoveByUnit);

			pcsControl->m_uoData.m_stScroll.m_pstCurrent = pThis->GetUserData(&szTemp[0]);
			pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent = pThis->GetDisplay(&szTemp2[0]);
			pcsControl->m_uoData.m_stScroll.m_pstMax = pThis->GetUserData(&szTemp3[0]);
			pcsControl->m_uoData.m_stScroll.m_pstDisplayMax = pThis->GetDisplay(&szTemp4[0]);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_COMBO))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_COMBO)
				return FALSE;

			AcUICombo *	pcsCombo = (AcUICombo *) pcsControl->m_pcsBase;
			INT32		alTemp[ACUICOMBO_MAX_TEXTURE] = {0,0,0,0};
			INT32		lHeight = 16;

			sscanf(szValue, "%d:%d:%d:%d:%d",
							alTemp + ACUICOMBO_TEXTURE_NORMAL,
							alTemp + ACUICOMBO_TEXTURE_ONMOUSE,
							alTemp + ACUICOMBO_TEXTURE_CLICK,
							&lHeight,
							alTemp + ACUICOMBO_TEXTURE_BOTTOM);

			pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_NORMAL, alTemp[ACUICOMBO_TEXTURE_NORMAL]);
			pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_ONMOUSE, alTemp[ACUICOMBO_TEXTURE_ONMOUSE]);
			pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_CLICK, alTemp[ACUICOMBO_TEXTURE_CLICK]);
			pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_BOTTOM, alTemp[ACUICOMBO_TEXTURE_BOTTOM]);
			pcsCombo->SetOpenLineHeight(lHeight);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_CLOCK))
		{
			if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_CLOCK)
				return FALSE;

			AcUIClock *		pcsClock = (AcUIClock *) pcsControl->m_pcsBase;
			INT32			lValue;

			sscanf(szValue, "%d:%d:%s %s %s %s",
				&lValue,
				&pcsClock->m_eClockType,
				&szTemp[0],
				&szTemp2[0],
				&szTemp3[0],
				&szTemp4[0]);

			pcsClock->SetAlphaImageID(lValue);

			pcsControl->m_uoData.m_stClock.m_pstMax = pThis->GetUserData(&szTemp[0]);
			pcsControl->m_uoData.m_stClock.m_pstCurrent = pThis->GetUserData(&szTemp2[0]);
			pcsControl->m_uoData.m_stClock.m_pstDisplayMax = pThis->GetDisplay(&szTemp3[0]);
			pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent = pThis->GetDisplay(&szTemp4[0]);

			pThis->RefreshControl(pcsControl);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_PARENT))
		{
			if (!pcsUI)
				return FALSE;

			sscanf(szValue, "%d:%s", &pcsUI->m_bUseParentPosition, pcsUI->m_szParentUIName);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_TYPE))
		{
			if (!pcsUI)
				return FALSE;

			pcsUI->m_eUIType = (AgcdUIType) atoi(szValue);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_MAIN))
		{
			if (!pcsUI)
				return FALSE;

			sscanf(szValue, "%d:%d:%d:%d", &pcsUI->m_bMainUI, &pcsUI->m_bAutoClose, &pcsUI->m_bEventUI, &pcsUI->m_bTransparent);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_INITSTATUS))
		{
			if (!pcsUI)
				return FALSE;

			pcsUI->m_bInitControlStatus = atoi( szValue );
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_COORDSYSTEM))
		{
			if (!pcsUI)
				return FALSE;

			//pcsUI->m_eCoordSystem = AGCDUI_COORDSYSTEM_ABSOLUTE;
			pcsUI->m_eCoordSystem = (AgcdUICoordSystem) atoi( szValue );
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_MODE_FLAG))
		{
			if (!pcsUI)
				return FALSE;

			pcsUI->m_ulModeFlag = atoi( szValue );
			//pcsUI->m_eCoordSystem = (AgcdUICoordSystem) atoi( szValue );
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_UI_FOCUS_CB))
		{
			if (!pcsUI)
				return FALSE;

			sscanf(szValue, "%s %s", &szTemp[0], &szTemp2[0]);

			pcsUI->m_pstSetFocusFunction = pThis->GetFunction(&szTemp[0]);
			pcsUI->m_pstKillFocusFunction = pThis->GetFunction(&szTemp2[0]);
		}
	}

	pThis->RefreshUI(pcsUI, TRUE);

	return TRUE;
}

BOOL			AgcmUIManager2::StreamReadAction(AuList <AgcdUIAction> *plistActions, ApModuleStream *pStream)
{
	const CHAR *		szValueName;
	const CHAR *		szValue;
	ApSafeArray<CHAR, 128>	szTemp;
	AgcdUIControl *		pcsControl = NULL;
	INT32				lActionType;
	AgcdUIAction *		pstAction;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();
		szValue		= pStream->GetValue();

		szTemp.MemSetAll();

		if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ACTION))
		{
			AgcdUIAction	stAction;

			memset(&stAction, 0, sizeof(stAction));
			if (!plistActions->AddTail(stAction))
				return FALSE;

			pstAction = &((*plistActions)[plistActions->GetCount() - 1]);

			lActionType = atoi(szValue);
			switch (lActionType)
			{
			case AGCDUI_ACTION_UI:
				{
					AgcdUIActionUI *	pstActionUI = &pstAction->m_uoAction.m_stUI;

					sscanf(szValue, "%d:%d:%d:%d:%s",
							&pstAction->m_eType,
							&pstActionUI->m_eType,
							&pstActionUI->m_bGroupAction,
							&pstActionUI->m_bSmooth,
							pstActionUI->m_szUIName);

					pstActionUI->m_pcsUI = GetUI(pstActionUI->m_szUIName);

					break;
				}

			case AGCDUI_ACTION_CONTROL:
				{
					AgcdUIActionControl *	pstActionControl = &pstAction->m_uoAction.m_stControl;

					sscanf(szValue, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%s %s",
							&pstAction->m_eType,
							&pstActionControl->m_lStatusNum,
							&pstActionControl->m_astStatus[0].m_lCurrentStatus,
							&pstActionControl->m_astStatus[0].m_lTargetStatus,
							&pstActionControl->m_astStatus[1].m_lCurrentStatus,
							&pstActionControl->m_astStatus[1].m_lTargetStatus,
							&pstActionControl->m_astStatus[2].m_lCurrentStatus,
							&pstActionControl->m_astStatus[2].m_lTargetStatus,
							&pstActionControl->m_bSaveCurrent,
							pstActionControl->m_szUIName,
							pstActionControl->m_szControlName);

					pstActionControl->m_pcsUI = GetUI(pstActionControl->m_szUIName);
					if (pstActionControl->m_pcsUI)
						pstActionControl->m_pcsControl = GetControl(pstActionControl->m_pcsUI, pstActionControl->m_szControlName);

					break;
				}

			case AGCDUI_ACTION_FUNCTION:
				{
					AgcdUIActionFunction *	pstActionFunction = &pstAction->m_uoAction.m_stFunction;

					sscanf(szValue, "%d:%s",
							&pstAction->m_eType,
							&szTemp[0]);

					pstActionFunction->m_pstFunction = GetFunction(&szTemp[0]);

					break;
				}
				
			case AGCDUI_ACTION_MESSAGE:
				{
					AgcdUIActionMessage *	pstActionMessage = &pstAction->m_uoAction.m_stMessage;
					
					//UINT8 lColor[4] = { 255, 255, 255, 255 };	// red, green, blue, alpha
					DWORD	lColor = 0xffffffff;

					/*sscanf(szValue, "%d:%d:%d:%d:%d:%d:%s", 
							&pstAction->m_eType,
							&pstActionMessage->m_eMessageType,
							&lColor[0],								// red
							&lColor[1],								// green 
							&lColor[2],								// blue
							&lColor[3],								// alpha
							&pstActionMessage->m_szMessage			// 길이 문제 없나 ?
							);	*/
					sscanf(szValue, "%d:%d:%d", 
							&pstAction->m_eType, 
							&pstActionMessage->m_eMessageType, 
							&lColor//,
							//&pstActionMessage->m_szMessage
							);
					
					INT32 lStartPosition	=	0	; 
					INT32 lEqualCount		=	0	;
					
					for ( INT32 i = 0 ; i < (INT32) strlen(szValue) ; ++i )
					{
						if ( !strncmp( &szValue[i] , ":" , 1 ) )
						{
							++lEqualCount;

							if ( 3 == lEqualCount )
							{
								lStartPosition = i + 1;

								if ( (INT32) strlen(szValue) > lStartPosition )
								{
									strncpy( pstActionMessage->m_szMessage, &(szValue[lStartPosition]), AGCDUIMANAGER2_MAX_ACTION_MESSAGE );
									pstActionMessage->m_szMessage[AGCDUIMANAGER2_MAX_ACTION_MESSAGE - 1] = 0;
								}
							}
						}
					}
					
					pstActionMessage->m_lColor = lColor;	//= ( ( lColor[3] << 24 ) | ( lColor[0] << 16 ) | ( lColor[1] << 8 ) | lColor[2] )		;
						
					break;
				}

			case AGCDUI_ACTION_SOUND:
				{
					AgcdUIActionSound	*	pstActionSound = &pstAction->m_uoAction.m_stSound;

					sscanf(szValue, "%d:%s",
							&pstAction->m_eType,
							pstActionSound->m_szSound);

					break;
				}
				
			default:
				return FALSE;
			}
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ACTION_POS))
		{
			AgcdUIActionUI *	pstActionUI = &pstAction->m_uoAction.m_stUI;

			sscanf(szValue, "%d:%d",
				   &pstActionUI->m_lPosX,
				   &pstActionUI->m_lPosY);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ACTION_ATTACH))
		{
			AgcdUIActionUI *	pstActionUI = &pstAction->m_uoAction.m_stUI;

			sscanf(szValue, "%s",
				pstActionUI->m_szAttachUIName);
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ARGUMENT))
		{
			INT32	lArgIndex;

			if (lActionType != AGCDUI_ACTION_FUNCTION)
				return FALSE;

			lArgIndex = atoi(szValue);
			if (lArgIndex >= AGCDUIMANAGER2_MAX_ARGS)
				return FALSE;

			sscanf(szValue, "%d:%s %s",
					&lArgIndex,
					pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_szUIName,
					pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_szControlName);

			pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_eType = AGCDUI_ARG_TYPE_CONTROL;
		}
		else if (!strcmp(szValueName, AGCMUIMANAGER2_NAME_ARGUMENT2))
		{
			INT32	lArgIndex;

			if (lActionType != AGCDUI_ACTION_FUNCTION)
				return FALSE;

			lArgIndex = atoi(szValue);
			if (lArgIndex >= AGCDUIMANAGER2_MAX_ARGS)
				return FALSE;

			sscanf(szValue, "%d:%d:%s %s",
					&lArgIndex,
					&pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_eType,
					pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_szUIName,
					pstAction->m_uoAction.m_stFunction.m_astArgs[lArgIndex].m_szControlName);
		}
		else
		{
			pStream->ReadPrevValue();

			return TRUE;
		}
	}

	return TRUE;
}

BOOL AgcmUIManager2::StreamReadUIMessage(CHAR *szFile, BOOL bEncryption)
{
	if( m_mapAdminUIMsg.size() > 0 )
		return TRUE;
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bEncryption))
	{
		OutputDebugString("AgcmUIManager2::StreamReadUIMessage() Error (1) !!!\n");
		return FALSE;
	}

	INT32			lRow			= csExcelTxtLib.GetRow();

	m_aszMessageData	= new CHAR [sizeof(CHAR) * AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA * lRow];

	ZeroMemory(m_aszMessageData, sizeof(CHAR) * AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA * lRow);

	CHAR			*szDelimiter	= csExcelTxtLib.GetData(1, 0);
	if (!szDelimiter)
		return FALSE;

	strcpy(m_szLineDelimiter, szDelimiter);

	CHAR			*szQuotationMark	= csExcelTxtLib.GetData(1, 1);
	if (!szQuotationMark)
		return FALSE;

	m_szQuotationMark	= *szQuotationMark;

	CHAR			*szComma	= csExcelTxtLib.GetData(1, 2);
	if (!szComma)
		return FALSE;

	m_szComma		= *szComma;

	INT32			lCurRow			= 0;
	INT32			lNumMessage		= 0;

	//-----------------------------------------------------------------------
	// MessageID 키 이후부터 인덱스를 먹여 사용되는 것으로 밝혀짐. - 2010.04.29 kdi
	char const * uiMessage = 0;
	while( strcmp( "MessageID", ( uiMessage = csExcelTxtLib.GetData(0, lCurRow++) ) ? uiMessage : "") != 0 )
	{
		if( lCurRow >= lRow ) break;
	}
	//-----------------------------------------------------------------------

	for ( ; ; )
	{
		if (lCurRow >= lRow)
			break;

		CHAR		*szMessageID	= csExcelTxtLib.GetData(0, lCurRow);
		CHAR		*szMessageData	= csExcelTxtLib.GetData(1, lCurRow);

		++lCurRow;

		if (!szMessageID || !szMessageData)
			continue;

		CHAR	*szMessageIndex	= m_aszMessageData + AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA * lNumMessage;

		strncpy_s( szMessageIndex, AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA, szMessageData, _TRUNCATE );

		ChangeCommaAndQutationMark( szMessageIndex, m_szComma, m_szQuotationMark );

		stAdminUIMsgEntry* NewEntry = new stAdminUIMsgEntry;

		NewEntry->m_strIndex	=	szMessageID;
		NewEntry->m_strMsg		=	szMessageIndex;

		m_mapAdminUIMsg.insert( make_pair(NewEntry->m_strIndex, NewEntry) );
		++lNumMessage;
	}

	m_pcsAgcmUIControl->SetButtonName(GetUIMessage(AGCMUIMANAGER2_NAME_BUTTON_OK), GetUIMessage(AGCMUIMANAGER2_NAME_BUTTON_CANCEL));

	AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
	if( pcmEventNPCDialog )
	{
		CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
		if( pExNPC )
		{
			pExNPC->SetMsgBoxButtonName( ClientStr().GetStr( STI_OK ), ClientStr().GetStr( STI_CANCEL ) );
		}
	}

	return TRUE;
}
