// ApmEventListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ApmEventManagerDlg.h"
#include "ApmEventListDlg.h"
#include "ApMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ApmEventListDlg dialog

extern CHAR *				aszEventFunctionName[APDEVENT_MAX_FUNCTION];
extern ApmEventDialog *		g_apcsEventDialog[APDEVENT_MAX_FUNCTION];

ApmEventListDlg::ApmEventListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ApmEventListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ApmEventListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ApmEventListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ApmEventListDlg)
	DDX_Control(pDX, IDC_EVENT_ADD_SELECT, m_csEventSelect);
	DDX_Control(pDX, IDC_EVENT_LIST, m_csEventList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ApmEventListDlg, CDialog)
	//{{AFX_MSG_MAP(ApmEventListDlg)
	ON_BN_CLICKED(IDC_EVENT_ADD, OnEventAdd)
	ON_BN_CLICKED(IDC_EVENT_REMOVE, OnEventRemove)
	ON_BN_CLICKED(IDC_EVENT_EDIT, OnEventEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ApmEventListDlg message handlers

void ApmEventListDlg::OnEventAdd() 
{
	INT32		lIndex;
	ApdEvent *	pstEvent;

	lIndex = m_csEventSelect.GetCurSel();
	if (lIndex == LB_ERR)
		return;

	lIndex = (INT32)m_csEventSelect.GetItemData(lIndex);

	pstEvent = m_pcsApmEventManager->AddEvent(m_pstEventAD, (ApdEventFunction) lIndex, m_pcsBase, FALSE);
	if (!pstEvent)
		return;

	if (g_apcsEventDialog[lIndex])
	{
		if (!g_apcsEventDialog[lIndex]->Open(pstEvent))
		{
			m_pcsApmEventManager->RemoveEvent(m_pcsApmEventManager->GetEvent(m_pstEventAD, (ApdEventFunction) lIndex));
			return;
		}
	}

	OnInitDialog();
}

void ApmEventListDlg::OnEventRemove() 
{
	INT32	lIndex;

	lIndex = m_csEventList.GetCurSel();
	if (lIndex == LB_ERR)
		return;

	lIndex = (INT32)m_csEventList.GetItemData(lIndex);

	m_pcsApmEventManager->RemoveEvent( &m_pstEventAD->m_astEvent[ lIndex ] );

	OnInitDialog();
}

void ApmEventListDlg::OnEventEdit() 
{
	INT32					lIndex;

	lIndex = m_csEventList.GetCurSel();
	if (lIndex == LB_ERR)
		return;

	lIndex = (INT32)m_csEventList.GetItemData(lIndex);

	if (g_apcsEventDialog[m_pstEventAD->m_astEvent[lIndex].m_eFunction])
	{
		g_apcsEventDialog[m_pstEventAD->m_astEvent[lIndex].m_eFunction]->Open( &m_pstEventAD->m_astEvent[ lIndex ] );
	}
}

BOOL ApmEventListDlg::Create()
{
	BOOL	bRet = CDialog::Create(IDD, NULL);

	return bRet;
}

void ApmEventListDlg::InitData(ApBase *pcsBase, ApdEventAttachData *pstEventAD, ApmEventManager *pcsApmEventManager)
{
	m_pcsApmEventManager = pcsApmEventManager;
	m_pcsBase = pcsBase;
	m_pstEventAD = pstEventAD;
}

BOOL ApmEventListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	INT32					lIndex;
	INT32					lListIndex;

	m_csEventList.ResetContent();
	m_csEventSelect.ResetContent();

	for (lIndex = 1; lIndex < APDEVENT_MAX_FUNCTION; ++lIndex)
	{
		if (!m_pcsApmEventManager->IsRegistered((ApdEventFunction) lIndex))
			continue;

		if (!aszEventFunctionName[lIndex])
			continue;

		lListIndex = m_csEventSelect.AddString(aszEventFunctionName[lIndex]);
		if (lListIndex == LB_ERR)
		{
			AfxMessageBox("Failed to EventSelect.AddString() !!!");
			return FALSE;
		}

		m_csEventSelect.SetItemData(lListIndex, lIndex);
	}

	for (lIndex = 0; lIndex < m_pstEventAD->m_unFunction; ++lIndex)
	{
		if (m_pstEventAD->m_astEvent[lIndex].m_eFunction == APDEVENT_FUNCTION_NONE || m_pstEventAD->m_astEvent[lIndex].m_eFunction >= APDEVENT_MAX_FUNCTION)
			continue;

		lListIndex = m_csEventList.AddString(aszEventFunctionName[m_pstEventAD->m_astEvent[lIndex].m_eFunction]);
		if (lListIndex == LB_ERR)
		{
			AfxMessageBox("Failed to EventList.AddString() !!!");
			return FALSE;
		}

		m_csEventList.SetItemData(lListIndex, lIndex);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
