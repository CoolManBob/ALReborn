/*****************************************************************************
* File : AgcmImportClientData.h
*
* Desc :
*
* 031903(MMDDYY) Bob Jung.
*****************************************************************************/

#ifndef __AGCM_IMPORT_CLIENT_DATA_H__
#define __AGCM_IMPORT_CLIENT_DATA_H__

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AuExcelTxtLib.h"

#include "AgcmProcessInfoDlg.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmImportClientDataD" )
#else
#pragma comment ( lib , "AgcmImportClientData" )
#endif
#endif

#define AGCM_IMPORT_CHAR_TYPE_INI_PATH				".\\INI\\CHARTYPE.INI"
#define AGCM_IMPORT_BUFFER_LEN						256


class AgcmImportClientData : public ApModule
{	
public:
			AgcmImportClientData();
	virtual	~AgcmImportClientData()	{}

	// 필수 함수덜...
	BOOL	OnInit();
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

protected:
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmItem		*m_pcsAgpmItem;

	AuExcelTxtLib	m_csAuExcelTxtLib;

	BOOL				m_bOpenProcessInfoDlg;
	AgcmProcessInfoDlg	m_csAgcmProcessInfoDlg;

public:
	// 초기화시 불러주던가? 안불러주던가? -_-;;
	VOID	SetOpenProcessInfoDlg();

	BOOL	ImportCharacterData(CHAR *szPath);
	BOOL	ImportItemData(CHAR *szPath);

	BOOL	CloseProcessInfoDlg();

protected:
	//BOOL					SetFactor(AgpdFactor *pcsAgpdFactor, INT16 nRow, INT16 nColumn, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);	

	AgpdCharacterTemplate	*SearchCharacterTemplate(CHAR *szTName);
	AgpdItemTemplate		*SearchItemTemplate(CHAR *szTName);

	//BOOL					SetDefaultEquipments(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lKind, INT32 lPart = 0);
	//BOOL					SetDefaultInventoryItems(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lNum, INT32 lIndex);

/*	BOOL					IsEmptyCharacterTID(INT32 lTID);
	INT32					SearchEmptyCharacterTID();	
	BOOL					ParseCharacterID(INT16 nColumn);	
	BOOL					SetCharacterTemplateName();*/

	static BOOL				CBImportCharacterDataErrorReport(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBImportItemDataErrorReport(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // __ACU_IMPORT_CLIENT_DATA_H__

/******************************************************************************
******************************************************************************/