#pragma once

#include "ApModule.h"
#include "AgcmItem.h"

#define AGCM_ITEM_DLG_MAX_STR		256

enum AgcmItemDlgCallbackPoint
{
	AGCMITEMDLG_CB_ID_SET_BSPHERE = 0,
	AGCMITEMDLG_CB_ID_NUM
};

class AFX_EXT_CLASS AgcmItemDlg : public ApModule
{
public:
	AgcmItemDlg();
	virtual ~AgcmItemDlg();

	static AgcmItemDlg	*GetInstance();

	CHAR				*GetClumpPathName1();
	CHAR				*GetClumpPathName2();

	CHAR				*GetTexturePathName1();
	CHAR				*GetTexturePathName2();
	CHAR				*GetTexturePathName3();

	AgpmItem			*GetAgpmItemModule()	{return m_pcsAgpmItem;}

protected:
	BOOL				OnAddModule();

public:
	// 초기화시 SetClumpPathExt나 SetClumpFindPathName를 불러줘야 한다.
	VOID				SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2);
	VOID				SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2);

	// 초기화시 SetTexturePathExt나 SetTextureFindPathName를 불러줘야 한다.
	VOID				SetTexturePathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3);
	VOID				SetTextureFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2, CHAR *szFindPathName3);
	
	BOOL				SetCallbackGenerateBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				EnumCallbackSetItemData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData = NULL);

	// 필요할 때 불러준당...
	BOOL				OpenItemDlg(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, bool bWeaponTypeSelectable = true);
	AgpdItemTemplate	*OpenItemList();

protected:
	CHAR				m_szFindClumpPathName1[AGCM_ITEM_DLG_MAX_STR];
	CHAR				m_szFindClumpPathName2[AGCM_ITEM_DLG_MAX_STR];
	CHAR				m_szFindTexturePathName1[AGCM_ITEM_DLG_MAX_STR];
	CHAR				m_szFindTexturePathName2[AGCM_ITEM_DLG_MAX_STR];
	CHAR				m_szFindTexturePathName3[AGCM_ITEM_DLG_MAX_STR];

	AgpmItem			*m_pcsAgpmItem;

};
