// AgcmAITemplateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAIDlg.h"
#include "AgcmAIDialog.h"
#include "AgcmAITemplateDialog.h"

extern AgcmAIDialog *	g_pcsAIDialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAITemplateDialog dialog


AgcmAITemplateDialog::AgcmAITemplateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAITemplateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAITemplateDialog)
	m_lAITID = 0;
	m_szAITName = _T("");
	//}}AFX_DATA_INIT

	m_pstTemplate = NULL;
}


void AgcmAITemplateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAITemplateDialog)
	DDX_Control(pDX, IDC_AI_TEMPLATES, m_csAITemplates);
	DDX_Text(pDX, IDC_AI_TEMPLATE_ID, m_lAITID);
	DDX_Text(pDX, IDC_AI_TEMPLATE_NAME, m_szAITName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAITemplateDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAITemplateDialog)
	ON_BN_CLICKED(IDC_AI_TEMPLATE_ADD, OnAITemplateAdd)
	ON_BN_CLICKED(IDC_AI_TEMPLATE_EDIT, OnAITemplateEdit)
	ON_BN_CLICKED(IDC_AI_TEMPLATE_DELETE, OnAITemplateDelete)
	ON_BN_CLICKED(IDC_AI_TEMPLATE_SELECT, OnAITemplateSelect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AI_TEMPLATES, OnItemchangedAITemplates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAITemplateDialog message handlers

VOID AgcmAITemplateDialog::SetModule(AgpmAI *pcsAgpmAI, AgcmAIDlg *pcsAgcmAIDlg)
{
	m_pcsAgpmAI = pcsAgpmAI;
	m_pcsAgcmAIDlg = pcsAgcmAIDlg;
}

BOOL AgcmAITemplateDialog::Init()
{
	return TRUE;
}

BOOL AgcmAITemplateDialog::InitData(BOOL bSelect)
{
	m_bSelect = bSelect;

	return TRUE;
}

void AgcmAITemplateDialog::OnAITemplateAdd() 
{
	UpdateData();

	AgpdAITemplate *	pstTemplate;

	pstTemplate = m_pcsAgpmAI->AddAITemplate(m_lAITID);
	if (!pstTemplate)
	{
		AfxMessageBox("Failed to add AI Template !!!");
		return;
	}

	strncpy(pstTemplate->m_szName, m_szAITName, AGPDAI_TEMPLATE_NAME_LENGTH);
	pstTemplate->m_szName[AGPDAI_TEMPLATE_NAME_LENGTH - 1] = '\0';

	if (!m_pcsAgcmAIDlg->OpenTemplate(pstTemplate))
	{
		AfxMessageBox("Failed to open AI Dialog !!!");
		return;
	}

	InitData(m_bSelect);
}

void AgcmAITemplateDialog::OnAITemplateEdit() 
{
	if (m_pstTemplate)
	{
		if (!m_pcsAgcmAIDlg->OpenTemplate(m_pstTemplate))
		{
			AfxMessageBox("Failed to open AI Dialog !!!");
			return;
		}
	}
}

void AgcmAITemplateDialog::OnAITemplateDelete() 
{
	if (m_pstTemplate)
	{
		m_pcsAgpmAI->RemoveAITemplate(m_pstTemplate->m_lID);
	}

	InitData(m_bSelect);
}

void AgcmAITemplateDialog::OnOK() 
{
	CDialog::OnOK();
}

BOOL AgcmAITemplateDialog::Create() 
{
	return CDialog::Create(IDD, NULL);
}

/*
BOOL AgcmAITemplateDialog::OpenTemplate(AgpdAITemplate *pstTemplate)
{
	if (!pstTemplate)
		return FALSE;

	if (!g_pcsAIDialog)
	{
		g_pcsAIDialog = new AgcmAIDialog;
		g_pcsAIDialog->Create();
		g_pcsAIDialog->Init();
		g_pcsAIDialog->SetModule(m_pcsAgpmAI, m_pcsAgcmAIDlg);
	}

	if (!g_pcsAIDialog->InitData(pstTemplate))
		return FALSE;

	g_pcsAIDialog->ShowWindow(SW_SHOW);

	return TRUE;
}
*/

void AgcmAITemplateDialog::OnAITemplateSelect() 
{
	if (m_pstTemplate)
		m_pcsAgcmAIDlg->EnumCallback(0, m_pstTemplate, NULL);

	OnOK();
}

void AgcmAITemplateDialog::OnItemchangedAITemplates(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	INT32					lListIndex;
	POSITION				pos = m_csAITemplates.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		lListIndex = m_csAITemplates.GetNextSelectedItem(pos);
		m_pstTemplate = (AgpdAITemplate *) m_csAITemplates.GetItemData(lListIndex);
		if (!m_pstTemplate)
			return;

		m_lAITID = m_pstTemplate->m_lID;
		m_szAITName = m_pstTemplate->m_szName;

		UpdateData(FALSE);
	}
}

BOOL AgcmAITemplateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	INT32				lListIndex;
	INT32				lIndex = 0;
	INT32				lIndex2;
	AgpdAITemplate *	pstTemplate;
	CHAR				szTemp[100];

	m_csAITemplates.InsertColumn(1, "ID", LVCFMT_LEFT, 100);
	m_csAITemplates.InsertColumn(2, "Name", LVCFMT_LEFT, 360);

	m_csAITemplates.DeleteAllItems();

	if (!m_bSelect)
	{
		GetWindow(IDC_AI_TEMPLATE_SELECT)->EnableWindow(TRUE);
	}

	for (lIndex2 = 0, pstTemplate = m_pcsAgpmAI->GetTemplateSequence(&lIndex); pstTemplate; pstTemplate = m_pcsAgpmAI->GetTemplateSequence(&lIndex), ++lIndex2)
	{
		sprintf(szTemp, "%d", pstTemplate->m_lID);
		lListIndex = m_csAITemplates.InsertItem(lIndex2, szTemp);
		if (lListIndex == -1)
			return FALSE;

		m_csAITemplates.SetItemData(lListIndex, (INT32) pstTemplate);

		m_csAITemplates.SetItemText(lListIndex, 1, pstTemplate->m_szName);
	}
	
	return TRUE;
}
