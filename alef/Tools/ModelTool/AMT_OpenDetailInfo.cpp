#include "stdafx.h"
#include "ModelTool.h"
#include "DetailInfoDlg.h"

BOOL CModelToolApp::OpenDetailInfo()
{
	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_NONE:
		return TRUE;

	case AMT_OBJECT_TYPE_STATIC_MODEL:
		{
			CDetailInfoDlg dlg(OpenStaticModelInfoCB, this);
			dlg.DoModal();
		}
		break;

	case AMT_OBJECT_TYPE_CHARACTER:
		{
			CDetailInfoDlg dlg(OpenCharacterInfoCB, this);
			dlg.DoModal();
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			CDetailInfoDlg dlg(OpenItemInfoCB, this);
			dlg.DoModal();
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			CDetailInfoDlg dlg(OpenObjectInfoCB, this);
			dlg.DoModal();
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::OpenStaticModelInfoCB(PVOID pClass, PVOID pCustClass)
{
	CDetailInfoDlg	*pDlg	= (CDetailInfoDlg *)(pClass);
	CModelToolApp	*pThis	= (CModelToolApp *)(pCustClass);
	if((!pDlg) || (!pThis))
		return FALSE;

	CHAR szStatus[AMT_MAX_STR];

	// 'c'
	sprintf(szStatus, "<%s>", pThis->GetStaticModelDffName());
	pDlg->_InsertItem("Clump", szStatus);

	return TRUE;
}

BOOL CModelToolApp::OpenCharacterInfoCB(PVOID pClass, PVOID pCustClass)
{
	CDetailInfoDlg	*pDlg	= (CDetailInfoDlg *)(pClass);
	CModelToolApp	*pThis	= (CModelToolApp *)(pCustClass);
	if((!pDlg) || (!pThis))
		return FALSE;

	AgpmCharacter *pcsAgpmCharacter		= pThis->m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter *pcsAgcmCharacter		= pThis->m_csAgcEngine.GetAgcmCharacterModule();
	if((!pcsAgpmCharacter) || (!pcsAgcmCharacter))
		return FALSE;

	AgpdCharacter *pcsAgpdCharacter = pcsAgpmCharacter->GetCharacter(pThis->GetCurrentID());
	if(!pcsAgpdCharacter)
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if(!pcsAgcdCharacter)
		return FALSE;

	AgpdCharacterTemplate *pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetCharacterTemplate(pThis->GetCurrentTID());
	if(!pcsAgpdCharacterTemplate)
		return FALSE;

	AgcdCharacterTemplate *pcsAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pcsAgpdCharacterTemplate);
	if(!pcsAgcdCharacterTemplate)
		return FALSE;

	return TRUE;
}

BOOL CModelToolApp::OpenItemInfoCB(PVOID pClass, PVOID pCustClass)
{
	CDetailInfoDlg	*pDlg	= (CDetailInfoDlg *)(pClass);
	CModelToolApp	*pThis	= (CModelToolApp *)(pCustClass);
	if((!pDlg) || (!pThis))
		return FALSE;

	AgpmItem *pcsAgpmItem		= pThis->m_csAgcEngine.GetAgpmItemModule();
	AgcmItem *pcsAgcmItem		= pThis->m_csAgcEngine.GetAgcmItemModule();
	if((!pcsAgpmItem) || (!pcsAgcmItem))
		return FALSE;

	AgpdItem *pcsAgpdItem = pcsAgpmItem->GetItem(pThis->GetCurrentID());
	if(!pcsAgpdItem)
		return FALSE;

	AgcdItem *pcsAgcdItem = pcsAgcmItem->GetItemData(pcsAgpdItem);
	if(!pcsAgcdItem)
		return FALSE;

	AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate);
	if(!pcsAgpdItemTemplate)
		return FALSE;

	AgcdItemTemplate *pcsAgcdItemTemplate = pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);
	if(!pcsAgcdItemTemplate)
		return FALSE;

	return TRUE;
}

BOOL CModelToolApp::OpenObjectInfoCB(PVOID pClass, PVOID pCustClass)
{
	CDetailInfoDlg	*pDlg	= (CDetailInfoDlg *)(pClass);
	CModelToolApp	*pThis	= (CModelToolApp *)(pCustClass);
	if((!pDlg) || (!pThis))
		return FALSE;

	ApmObject		*pcsApmObjectModule		= pThis->m_csAgcEngine.GetApmObjectModule();
	AgcmObject		*pcsAgcmObjectModule	= pThis->m_csAgcEngine.GetAgcmObjectModule();

	if((!pcsApmObjectModule) || (!pcsAgcmObjectModule))
		return FALSE;

	ApdObject *pcsApdObject = pcsApmObjectModule->GetObject(pThis->GetCurrentID());
	if(!pcsApdObject)
		return FALSE;

	ApdObjectTemplate *pcsApdObjectTemplate = pcsApmObjectModule->GetObjectTemplate(pThis->GetCurrentTID());
	if(!pcsApdObjectTemplate)
		return FALSE;

	AgcdObject *pcsAgcdObject = pcsAgcmObjectModule->GetObjectData(pcsApdObject);
	if(!pcsAgcdObject)
		return FALSE;

	AgcdObjectTemplate *pcsAgcdObjectTemplate = pcsAgcmObjectModule->GetTemplateData(pcsApdObjectTemplate);
	if(!pcsAgcdObjectTemplate)
		return FALSE;

	return TRUE;
}
