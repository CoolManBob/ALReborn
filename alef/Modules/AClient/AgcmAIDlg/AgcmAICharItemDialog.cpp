// AgcmAICharItemDialog.cpp : implementation file
//

#include "stdafx.h"
#include "agcmaidlg.h"
#include "AgcmAICharItemDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharItemDialog dialog


AgcmAICharItemDialog::AgcmAICharItemDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAICharItemDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAICharItemDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmAICharItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAICharItemDialog)
	DDX_Control(pDX, IDC_AI_ITEM_TEMPLATE, m_csItemTemplate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAICharItemDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAICharItemDialog)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharItemDialog message handlers

VOID AgcmAICharItemDialog::SetModule(AgpmItem *pcsAgpmItem)
{
	m_pcsAgpmItem = pcsAgpmItem;
}

int AgcmAICharItemDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void AgcmAICharItemDialog::OnOK() 
{
	INT32	lIndex;

	lIndex = m_csItemTemplate.GetCurSel();
	if (lIndex == CB_ERR)
	{
		m_pcsItemTemplate = NULL;
	}
	else
	{
		m_pcsItemTemplate = (AgpdItemTemplate *) m_csItemTemplate.GetItemDataPtr(lIndex);
	}
	
	CDialog::OnOK();
}

BOOL AgcmAICharItemDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex = 0;
	INT32				lListIndex;
	AgpdItemTemplate *	pcsItemTemplate;

	if (m_pcsAgpmItem)
	{
		for (pcsItemTemplate = m_pcsAgpmItem->GetTemplateSequence(&lIndex); pcsItemTemplate; pcsItemTemplate = m_pcsAgpmItem->GetTemplateSequence(&lIndex))
		{
			lListIndex = m_csItemTemplate.AddString(pcsItemTemplate->m_szName);
			if (lListIndex != CB_ERR)
			{
				m_csItemTemplate.SetItemDataPtr(lListIndex, pcsItemTemplate);
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
