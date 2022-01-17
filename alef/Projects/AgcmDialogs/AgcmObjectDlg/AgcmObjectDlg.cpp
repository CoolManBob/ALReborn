#include "stdafx.h"
#include "../resource.h"
#include "AgcmObjectDlg.h"
#include "ObjectListDlg.h"
#include "ObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmObjectDlg *pcsInstAgcmObjectDlg = NULL;

AgcmObjectDlg *AgcmObjectDlg::GetInstance()
{
	return pcsInstAgcmObjectDlg;
}

AgcmObjectDlg::AgcmObjectDlg()
{
	SetModuleName("AgcmObjectDlg");

	pcsInstAgcmObjectDlg	= this;

	m_pcsApmObject			= NULL;
}

AgcmObjectDlg::~AgcmObjectDlg()
{
}

BOOL AgcmObjectDlg::OnAddModule()
{
	m_pcsApmObject		= (ApmObject *)(GetModule("ApmObject"));
	m_pcsAgcmObject		= (AgcmObject *)(GetModule("AgcmObject"));
	m_pcsAgcmObjectList	= m_pcsAgcmObject->GetObjectList();

	if( !m_pcsApmObject || 
		!m_pcsAgcmObject || 
		!m_pcsAgcmObjectList )
		return FALSE;

	return TRUE;
}

VOID AgcmObjectDlg::SetClumpPathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2)
{
	sprintf(m_szFindClumpPathName1, "%s*.%s", szPath, szExt1);
	sprintf(m_szFindClumpPathName2, "%s*.%s", szPath, szExt2);
}

VOID AgcmObjectDlg::SetClumpFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2)
{
	strcpy(m_szFindClumpPathName1, szFindPathName1);
	strcpy(m_szFindClumpPathName2, szFindPathName2);
}

VOID AgcmObjectDlg::SetAnimationFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2)
{
	strcpy(m_szFindAnimationPathName1, szFindPathName1);
	strcpy(m_szFindAnimationPathName2, szFindPathName2);
}

CHAR *AgcmObjectDlg::GetAnimationPathName1()
{
	return m_szFindAnimationPathName1;
}

CHAR *AgcmObjectDlg::GetAnimationPathName2()
{
	return m_szFindAnimationPathName2;
}

CHAR *AgcmObjectDlg::GetClumpPathName1()
{
	return m_szFindClumpPathName1;
}

CHAR *AgcmObjectDlg::GetClumpPathName2()
{
	return m_szFindClumpPathName2;
}

VOID AgcmObjectDlg::SetObjectCategoryPathName(CHAR *szPathName)
{
	strcpy(m_szObjectCategoryPathName, szPathName);
}

CHAR *AgcmObjectDlg::GetObjectCategoryPathName()
{
	return m_szObjectCategoryPathName;
}

BOOL AgcmObjectDlg::OpenObjectDlg(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate)
{
	if((!pcsApdObjectTemplate) || (!pcsAgcdObjectTemplate))
		return FALSE;

	CObjectDlg dlg(pcsApdObjectTemplate, pcsAgcdObjectTemplate);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

BOOL AgcmObjectDlg::SetObjectCategory(PVOID pvDlg, INT16 nSetData)
{
	if( !pvDlg )		return FALSE;

	CObjectListDlg *pcsDlg = (CObjectListDlg *)(pvDlg);

	FILE *infile = fopen(m_szObjectCategoryPathName, "rt");
	if( !infile )		return FALSE;

	INT32		lLen;
	INT32		lIndex;
	CHAR		szCategory[256];
	CHAR		szCategory2[256];
	HTREEITEM	hParent[10];

	memset(hParent, 0, sizeof(HTREEITEM) * 10);

	while(!feof(infile))
	{
		if(fscanf(infile, "%d|", &lIndex) < 1)
			break;

		fgets(szCategory, 256, infile);

		lLen	= strlen(szCategory) - 1;
		
		if(szCategory[lLen] == 10)
			szCategory[lLen] = '\0';

		if(lIndex == 0)
		{
			sprintf(szCategory2, "[%s]", szCategory);
			hParent[lIndex] = pcsDlg->_InsertItem(szCategory2, TVI_ROOT, nSetData);
		}
		else
		{
			if(!hParent[lIndex - 1])
			{
				fclose(infile);
				return FALSE;
			}

			sprintf(szCategory2, "[%s]", szCategory);
			hParent[lIndex] = pcsDlg->_InsertItem(szCategory2, hParent[lIndex - 1], nSetData);
		}
	}

	fclose(infile);

	return TRUE;
}

BOOL AgcmObjectDlg::OpenObjectCategoryCB(PVOID pClass, PVOID pCustClass)
{
	AgcmObjectDlg	*pThis	= (AgcmObjectDlg *)(pCustClass);
	
	return pThis->SetObjectCategory(pClass, OBJECGT_LIST_DLG_TREE_ITEM_SELECT);
}

BOOL AgcmObjectDlg::OpenObjectTemplateListCB(PVOID pClass, PVOID pCustClass)
{
	CObjectListDlg	*pDlg	= (CObjectListDlg *)(pClass);
	AgcmObjectDlg	*pThis	= (AgcmObjectDlg *)(pCustClass);

	if(!pThis->SetObjectCategory(pClass, OBJECGT_LIST_DLG_TREE_ITEM_NOT_SELECT))
		return FALSE;

	CHAR				szCategory[AGCM_OBJECT_DLG_MAX_STR];
	AgcdObjectTemplate	*pcsAgcdObjectTemplate;
	INT32				lIndex = 0;
	for(ApdObjectTemplate	*pcsApdObjectTemplate = pThis->m_pcsApmObject->GetObjectTemplateSequence(&lIndex);
		pcsApdObjectTemplate;
		pcsApdObjectTemplate = pThis->m_pcsApmObject->GetObjectTemplateSequence(&lIndex))
	{
			pcsAgcdObjectTemplate = pThis->m_pcsAgcmObject->GetTemplateData(pcsApdObjectTemplate);
			if(!pcsAgcdObjectTemplate)
				return FALSE;

			sprintf(szCategory, "[%s]", pcsAgcdObjectTemplate->m_szCategory);
			HTREEITEM hParent = pDlg->_FindItem(szCategory);
			if(!hParent)
				return FALSE;

			pDlg->_InsertItem(pcsApdObjectTemplate->m_szName, hParent, OBJECGT_LIST_DLG_TREE_ITEM_SELECT);
	}

	pDlg->_EnableButton(FALSE);

	return TRUE;
}

BOOL AgcmObjectDlg::ObjectListDlgEndCB(PVOID pClass, PVOID pCustClass)
{
	CObjectListDlg	*pDlg	= (CObjectListDlg *)(pClass);
	AgcmObjectDlg	*pThis	= (AgcmObjectDlg *)(pCustClass);

	if((!pClass) || (!pCustClass))
		return FALSE;

	return pThis->SaveObjectCategory(pClass);
}

BOOL AgcmObjectDlg::ParseObjectCategory(CHAR *szDest, CHAR *szSrc)
{
	if(!szSrc)
		return FALSE;

	if(szSrc[0] != '[')
		return FALSE;

	INT32	lIndex	= 0;
	INT32	lLen	= strlen(szSrc);
	for(INT32 lCount = 1; lCount < lLen; ++lCount)
	{
		if(szSrc[lCount] == ']')
			lIndex = lCount - 1;
	}

	strncpy(szDest, szSrc + 1, lIndex);
	szDest[lIndex] = '\0';

	return TRUE;
}

BOOL AgcmObjectDlg::SaveObjectCategory(PVOID pvDlg)
{
	CObjectListDlg	*pDlg		= (CObjectListDlg *)(pvDlg);
	CTreeCtrl		*pTree		= pDlg->_GetTreeCtrl();

	if(FILE_ATTRIBUTE_READONLY == (GetFileAttributes(m_szObjectCategoryPathName) & FILE_ATTRIBUTE_READONLY))
	{
		SetFileAttributes(m_szObjectCategoryPathName, (GetFileAttributes(m_szObjectCategoryPathName) & ~FILE_ATTRIBUTE_READONLY));
	}

	FILE* outfile = fopen(m_szObjectCategoryPathName, "wt");
	if(!outfile)
		return FALSE;

	INT32		lIndex	= 0;
	HTREEITEM	hItem	= pTree->GetRootItem();
	HTREEITEM	hTemp;
	CString		str;
	CHAR		szCategory[256];
	while(hItem)
	{
		while(hItem)
		{
			str = pTree->GetItemText(hItem);
			ParseObjectCategory(szCategory, (LPSTR)(LPCSTR)(str));
			fprintf(outfile, "%d|%s\n", lIndex, szCategory);

			hTemp = pTree->GetNextItem(hItem, TVGN_CHILD);
			if(!hTemp)
			{
				break;
			}
			else
			{
				++lIndex;
			}

			hItem = hTemp;
		}

		while(hItem)
		{
			hTemp = pTree->GetNextItem(hItem, TVGN_NEXT);
			if(hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = pTree->GetParentItem(hItem);
			if(!hItem)
			{
				fclose(outfile);
				return TRUE;
			}
			else
			{
				--lIndex;
			}
		}
	}

	fclose(outfile);

	return TRUE;
}

BOOL AgcmObjectDlg::OpenObjectCategory(CHAR *szDest)
{
	CHAR szTemp[AGCM_OBJECT_DLG_MAX_STR];
	strcpy(szTemp, "");

	CObjectListDlg dlg(OpenObjectCategoryCB, ObjectListDlgEndCB, this, szTemp);
	if(dlg.DoModal() == IDOK)
	{
		if(szDest)
			return ParseObjectCategory(szDest, szTemp);
		else
			return TRUE;
	}

	return FALSE;
}

BOOL AgcmObjectDlg::OpenObjectTemplateList(CHAR *szDest)
{
	CHAR szTemp[AGCM_OBJECT_DLG_MAX_STR];
	strcpy(szTemp, "");

	CObjectListDlg dlg(OpenObjectTemplateListCB, NULL, this, szTemp);
	if(dlg.DoModal() == IDOK)
	{
		if(szDest)
			strcpy(szDest, szTemp);
		else
			return TRUE;
	}
	
	return FALSE;
}

BOOL AgcmObjectDlg::SetCallbackGetAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECTDLG_CB_ID_GET_ANIMATION, pfCallback, pClass);
}

BOOL AgcmObjectDlg::SetCallbackGenerateBSphere(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECTDLG_CB_ID_SET_BSPHERE, pfCallback, pClass);
}

BOOL AgcmObjectDlg::SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECTDLG_CB_ID_ADD_ANIMATION, pfCallback, pClass);
}

BOOL AgcmObjectDlg::SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECTDLG_CB_ID_REMOVE_ALL_ANIMATION, pfCallback, pClass);
}

BOOL AgcmObjectDlg::SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMOBJECTDLG_CB_ID_READ_RT_ANIM, pfCallback, pClass);
}

BOOL AgcmObjectDlg::EnumCallbackSetObjectData(INT32 lCallbackPoint, PVOID pvData, PVOID pvCustData)
{
	return EnumCallback(lCallbackPoint, pvData, pvCustData);
}

BOOL AgcmObjectDlg::AddAnimation(AAD_AddAnimationParams *pcsParams)
{
	return EnumCallback(AGCMOBJECTDLG_CB_ID_ADD_ANIMATION, (PVOID)(pcsParams), NULL);
}

BOOL AgcmObjectDlg::RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams)
{
	return EnumCallback(AGCMOBJECTDLG_CB_ID_REMOVE_ALL_ANIMATION, (PVOID)(pcsParams), NULL);
}

BOOL AgcmObjectDlg::ReadRtAnim(AAD_ReadRtAnimParams *pcsParams)
{
	return EnumCallback(AGCMOBJECTDLG_CB_ID_READ_RT_ANIM, (PVOID)(pcsParams), NULL);
}