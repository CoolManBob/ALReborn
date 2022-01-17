// UITCustomControlDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITCustomControlDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITCustomControlDialog dialog


UITCustomControlDialog::UITCustomControlDialog(CWnd* pParent /*=NULL*/)
	: CDialog(UITCustomControlDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITCustomControlDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsCControl = NULL;
}


void UITCustomControlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITCustomControlDialog)
	DDX_Control(pDX, IDC_UIT_CUSTOM_CONTROL, m_csCustomControls);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITCustomControlDialog, CDialog)
	//{{AFX_MSG_MAP(UITCustomControlDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITCustomControlDialog message handlers

BOOL UITCustomControlDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUICControl *	pcsCControl;

	for (lIndex = 1; lIndex <= g_pcsAgcmUIManager2->GetCControlCount(); ++lIndex)
	{
		pcsCControl = g_pcsAgcmUIManager2->GetCControl(lIndex);

		lListIndex = m_csCustomControls.AddString(pcsCControl->m_szName);
		if (lListIndex < CB_ERR)
			return FALSE;

		m_csCustomControls.SetItemDataPtr(lListIndex, pcsCControl);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITCustomControlDialog::OnOK() 
{
	m_pcsCControl = (AgcdUICControl *) m_csCustomControls.GetItemDataPtr(m_csCustomControls.GetCurSel());
	if (m_pcsCControl == (PVOID) -1)
		m_pcsCControl = NULL;
	
	CDialog::OnOK();
}
