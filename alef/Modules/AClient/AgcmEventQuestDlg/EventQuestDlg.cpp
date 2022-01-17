// EventQuestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AgcmEventQuestDlg.h"
#include "EventQuestDlg.h"
#include "AgpdQuestTemplate.h"
#include "AgpmEventQuest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventQuestDlg dialog


CEventQuestDlg::CEventQuestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventQuestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventQuestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEventQuestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventQuestDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventQuestDlg, CDialog)
	//{{AFX_MSG_MAP(CEventQuestDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventQuestDlg message handlers

BOOL CEventQuestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	INT32 lIndex = 0;
	INT32 lCount = 0;
	AgpdQuestGroup** ppQuestGroup = NULL;

	/*
	for (ppQuestGroup = (AgpdQuestGroup**)m_pcsAgpmQuest->m_csQuestGroup.GetObjectSequence(&lIndex); ppQuestGroup;
			ppQuestGroup = (AgpdQuestGroup**)m_pcsAgpmQuest->m_csQuestGroup.GetObjectSequence(&lIndex), ++lCount)
	{
		INT32 lIdx = m_ctrlQuestGroup.InsertString(lCount, (*ppQuestGroup)->m_szName);
		m_ctrlQuestGroup.SetItemData(lIdx, (*ppQuestGroup)->m_lID);
	}
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventQuestDlg::OnOK() 
{
	// TODO: Add extra validation here
	INT32 lCurrentIndex = m_ctrlQuestGroup.GetCurSel();
	if (-1 != lCurrentIndex)
	{
		AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)m_pcsEvent->m_pvData;
		pcsAttachData->lQuestGroupID = m_ctrlQuestGroup.GetItemData(lCurrentIndex);
	}
	CDialog::OnOK();
}

CEventQuestDlg& CEventQuestDlg::GetInstance()
{
	static CEventQuestDlg Dlg;
	return Dlg;
}
