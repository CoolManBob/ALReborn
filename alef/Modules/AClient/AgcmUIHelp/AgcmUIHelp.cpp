#include "AgcmUIHelp.h"
#include "AgcmUIConsole.h"

#define	AGCM_TIP_NAME_ITEM		"Item"
#define	AGCM_TIP_NAME_DEPTH		"Depth"
#define	AGCM_TIP_NAME_CONTENT	"Content"

AgcmUIHelp::AgcmUIHelp()
{
	SetModuleName("AgcmUIHelp")					;

	m_pAgcmUIManager2		=		NULL		;

	m_pszHelpItem			=		NULL		;
	m_pszHelpContent		=		NULL		;
	m_plHelpDepth			=		NULL		;

	m_lCurrentItem			=		0			;

	m_pstUDItem				=		NULL		;
	m_pstUDItemDepth		=		NULL		;
	m_pstUDItemCurrent		=		NULL		;

	m_bEditMode				=		FALSE		;

	m_pcsContentControl		=		NULL		;

	//@{ 2006/07/04 burumal
	m_bDecryptionFlag		=		FALSE		;
	m_bDecryption			=		FALSE		;
	//@}
}

AgcmUIHelp::~AgcmUIHelp()
{

}

BOOL	AgcmUIHelp::OnInit()
{	
	AS_REGISTER_TYPE_BEGIN(AgcmUIHelp, AgcmUIHelp);
		AS_REGISTER_METHOD1(void, SetEditMode, bool);
		AS_REGISTER_METHOD1(void, WriteHelp, string);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

BOOL	AgcmUIHelp::OnAddModule()
{
	// Get Module
	m_pAgcmUIManager2		=	(AgcmUIManager2		*)GetModule("AgcmUIManager2")		;

	ASSERT( NULL != m_pAgcmUIManager2		);

	m_pstUDItem				= m_pAgcmUIManager2->AddUserData("Help_Item", NULL, sizeof(CHAR *), 0, AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pstUDItem)
		return FALSE;

	m_pstUDItemDepth		= m_pAgcmUIManager2->AddUserData("Help_Depth", NULL, sizeof(INT32), 0, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDItemDepth)
		return FALSE;

	m_pstUDItemCurrent		= m_pAgcmUIManager2->AddUserData("Help_CurrentItem", &m_lCurrentItem, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDItemCurrent)
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "Help_Content", 0, CBDisplayContent, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, "Help_ChangeItem", CBChangeItem, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgcmUIHelp::OnDestroy()
{
	return TRUE;
}

BOOL	AgcmUIHelp::ReadHelp(CHAR *szFile, BOOL bDecryption)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;

	//@{ 2006/07/04 burumal
	if ( m_bDecryptionFlag == FALSE )
		m_bDecryption = bDecryption;
	//@}

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmUITips::ReadTips() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	m_lNumHelp	= csStream.GetRow() - 1;

	m_pszHelpItem	= new CHAR * [m_lNumHelp];
	if (!m_pszHelpItem)
	{
		OutputDebugString("AgcmUITips::ReadHelp() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}
	memset(m_pszHelpItem, 0, sizeof(CHAR *) * m_lNumHelp);

	m_pszHelpContent	= new CHAR * [m_lNumHelp];
	if (!m_pszHelpContent)
	{
		OutputDebugString("AgcmUITips::ReadHelp() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}
	memset(m_pszHelpContent, 0, sizeof(CHAR *) * m_lNumHelp);

	m_plHelpDepth	= new INT32 [m_lNumHelp];
	if (!m_plHelpDepth)
	{
		OutputDebugString("AgcmUITips::ReadHelp() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}
	memset(m_plHelpDepth, 0, sizeof(INT32) * m_lNumHelp);

	for(lRow = 1; lRow <= m_lNumHelp; ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!stricmp(pszData, AGCM_TIP_NAME_ITEM))
			{
				pszData = csStream.GetData(lColumn, lRow);

				if (pszData)
				{
					m_pszHelpItem[lRow - 1] = new CHAR [strlen(pszData) + 1];
					strcpy(m_pszHelpItem[lRow - 1], pszData);
				}
				else
					m_pszHelpItem[lRow - 1] = NULL;
			}
			else if (!stricmp(pszData, AGCM_TIP_NAME_DEPTH))
			{
				m_plHelpDepth[lRow - 1] = csStream.GetDataToInt(lColumn, lRow);
			}
			else if (!stricmp(pszData, AGCM_TIP_NAME_CONTENT))
			{
				pszData = csStream.GetData(lColumn, lRow);

				if (pszData)
				{
					m_pszHelpContent[lRow - 1] = new CHAR [strlen(pszData) + 1];
					strcpy(m_pszHelpContent[lRow - 1], pszData);
				}
			}
		}
	}

	csStream.CloseFile();

	m_pstUDItem->m_stUserData.m_pvData		= m_pszHelpItem;
	m_pstUDItem->m_stUserData.m_lCount		= m_lNumHelp;

	m_pstUDItemDepth->m_stUserData.m_pvData	= m_plHelpDepth;
	m_pstUDItemDepth->m_stUserData.m_lCount	= m_lNumHelp;

	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItem);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItemDepth);

	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItemCurrent);

	return TRUE;
}
//@{ 2006/07/04 burumal

VOID	AgcmUIHelp::RefreshUI()
{
	if ( !m_pAgcmUIManager2 )
		return;

	if ( !m_pstUDItem || !m_pstUDItemDepth )
		return;

	m_pstUDItem->m_stUserData.m_pvData		= m_pszHelpItem;
	m_pstUDItem->m_stUserData.m_lCount		= m_lNumHelp;
	
	m_pstUDItemDepth->m_stUserData.m_pvData	= m_plHelpDepth;
	m_pstUDItemDepth->m_stUserData.m_lCount	= m_lNumHelp;

	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItem);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItemDepth);

	m_pAgcmUIManager2->SetUserDataRefresh(m_pstUDItemCurrent);
}

VOID	AgcmUIHelp::ReleaseHelp()
{
	INT32					lRow;
	//INT32					lColumn;
	
	for(lRow = 0; lRow < m_lNumHelp; ++lRow)
	{
		if ( m_pszHelpItem[lRow] )
		{
			delete [] m_pszHelpItem[lRow];
			m_pszHelpItem[lRow] = NULL;
		}

		if ( m_pszHelpContent[lRow] )
		{
			delete [] m_pszHelpContent[lRow];
			m_pszHelpContent[lRow] = NULL;
		}
	}

	if ( m_pszHelpItem	)
	{
		delete [] m_pszHelpItem;
		m_pszHelpItem = NULL;
	}
	
	if ( m_pszHelpContent )
	{
		delete [] m_pszHelpContent;
		m_pszHelpContent = NULL;
	}

	if ( m_plHelpDepth )
	{
		delete [] m_plHelpDepth;
		m_plHelpDepth = NULL;
	}
}
//@}

VOID	AgcmUIHelp::WriteHelp(string szFile)
{
	INT32	lIndex;
	FILE *	fp;

	if (szFile.empty())
		return;

	fp = fopen(szFile.c_str(), "wt");
	if (!fp)
		return;

	fprintf(fp, "%s\t%s\t%s\n", AGCM_TIP_NAME_ITEM, AGCM_TIP_NAME_DEPTH, AGCM_TIP_NAME_CONTENT);

	for (lIndex = 0; lIndex < m_lNumHelp; ++lIndex)
	{
		fprintf(fp, "%s\t%d\t%s\n", m_pszHelpItem[lIndex], m_plHelpDepth[lIndex], m_pszHelpContent[lIndex]);
	}

	fclose(fp);

	return;
}

VOID	AgcmUIHelp::SetEditMode(bool bMode)
{
	if (!m_pcsContentControl)
		return;

	if (m_pcsContentControl->m_lType == AcUIBase::TYPE_EDIT)
	{
		((AcUIEdit *) m_pcsContentControl->m_pcsBase)->m_bReadOnly = !bMode;
		((AcUIEdit *) m_pcsContentControl->m_pcsBase)->SetText(NULL);
	}

	m_bEditMode = bMode;
}

BOOL	AgcmUIHelp::CBDisplayContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	AgcmUIHelp *	pThis = (AgcmUIHelp *) pClass;
	CHAR *			szText = "";

	pThis->m_pcsContentControl = pcsSourceControl;

	// 2008.02.01. steeple
	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());

	if (!pThis->m_pszHelpContent)
		return FALSE;

	if (pThis->m_lCurrentItem >= 0)
		szText = pThis->m_pszHelpContent[pThis->m_lCurrentItem];

	if (!szText)
		szText = "";

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_EDIT)
	{
		((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szText);
	}
	else if (szDisplay)
	{
		_tcscpy(szDisplay, szText);
	}

	return TRUE;
}

BOOL	AgcmUIHelp::CBChangeItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIHelp *	pThis = (AgcmUIHelp *) pClass;

	if (pThis->m_pstUDItem)
	{
		INT32	lNewSelectedIndex	= pcsSourceControl->m_lUserDataIndex;

		if (pThis->m_lCurrentItem != lNewSelectedIndex)
		{
			if (pThis->m_lCurrentItem >= 0 &&
				pThis->m_bEditMode &&
				pThis->m_pcsContentControl &&
					pThis->m_pcsContentControl->m_lType == AcUIBase::TYPE_EDIT)
			{
				AcUIEdit *	pcsEdit = (AcUIEdit *) pThis->m_pcsContentControl->m_pcsBase;

				if (pThis->m_pszHelpContent[pThis->m_lCurrentItem])
					delete [] pThis->m_pszHelpContent[pThis->m_lCurrentItem];

				pThis->m_pszHelpContent[pThis->m_lCurrentItem] = new CHAR [_tcslen(pcsEdit->GetText()) + 1];
				strcpy(pThis->m_pszHelpContent[pThis->m_lCurrentItem], pcsEdit->GetText());
			}

			pThis->m_lCurrentItem = lNewSelectedIndex;
			pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDItemCurrent);
		}
	}

	return TRUE;
}

