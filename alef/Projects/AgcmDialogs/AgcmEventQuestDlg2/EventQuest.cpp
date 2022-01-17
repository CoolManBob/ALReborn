// EventQuest.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"		// main symbols
#include "AgcmEventQuestDlg2.h"
#include "EventQuest.h"
#include "AgpmEventQuest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventQuest dialog


CEventQuest::CEventQuest(CWnd* pParent /*=NULL*/)
	: CDialog(CEventQuest::IDD, pParent)
	, m_bIsObjectID(false)
	, m_strObjectID(_T(""))
{
	//{{AFX_DATA_INIT(CEventQuest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEventQuest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventQuest)
	DDX_Control(pDX, IDC_QUEST_LIST, m_ctrlQuestGroup);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_OBJECT_ID, m_strObjectID);
}


BEGIN_MESSAGE_MAP(CEventQuest, CDialog)
	//{{AFX_MSG_MAP(CEventQuest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventQuest message handlers

BOOL CEventQuest::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString strMsg;
	AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)m_pcsEvent->m_pvData;
	strMsg.Format("현재 그룹 ID : %d", pcsAttachData->lQuestGroupID);

	SetWindowText(strMsg);
	
	INT32 lIndex = 0;
	INT32 lCount = 0;
	AgpdQuestGroup** ppQuestGroup = NULL;

	for (ppQuestGroup = (AgpdQuestGroup**)m_pcsAgpmQuest->m_csQuestGroup.GetObjectSequence(&lIndex); ppQuestGroup;
			ppQuestGroup = (AgpdQuestGroup**)m_pcsAgpmQuest->m_csQuestGroup.GetObjectSequence(&lIndex), ++lCount)
	{
		INT32 lIdx = m_ctrlQuestGroup.InsertString(lCount, (*ppQuestGroup)->m_szName);
		m_ctrlQuestGroup.SetItemData(lIdx, (*ppQuestGroup)->m_lID);
	}

	if ( ppQuestGroup && m_pcsEvent->m_pcsSource->m_lID == (*ppQuestGroup)->m_lID )
	{
		CButton * pButton = ( CButton * ) GetDlgItem( IDC_OJBECT_ID );
		if( pButton )
		{
			pButton->SetCheck(TRUE);
		}		
	}

	// 그룹 ID 에디트 박스 설정..
	m_strObjectID.Format( "%d" , m_pcsEvent->m_pcsSource->m_lID );
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventQuest::OnOK() 
{
	UpdateData(TRUE);
	AgpdQuestEventAttachData *pcsAttachData = (AgpdQuestEventAttachData*)m_pcsEvent->m_pvData;

	CButton * pButton = ( CButton * ) GetDlgItem( IDC_OJBECT_ID );
	if( pButton )
	{
		m_bIsObjectID = ( bool )pButton->GetCheck();
	}

	if (m_bIsObjectID)
	{
		pcsAttachData->lQuestGroupID = m_pcsEvent->m_pcsSource->m_lID;
	}
	else
	{
		INT32 lCurrentIndex = m_ctrlQuestGroup.GetCurSel();
		if (-1 != lCurrentIndex)
		{
			pcsAttachData->lQuestGroupID = m_ctrlQuestGroup.GetItemData(lCurrentIndex);
		}
	}
	
	CDialog::OnOK();
}
