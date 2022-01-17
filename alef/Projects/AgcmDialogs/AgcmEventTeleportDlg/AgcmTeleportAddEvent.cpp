// AgcmTeleportAddEvent.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventteleportdlg.h"
#include "AgcmTeleportAddEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportAddEvent dialog


AgcmTeleportAddEvent::AgcmTeleportAddEvent(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmTeleportAddEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmTeleportAddEvent)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmTeleportAddEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmTeleportAddEvent)
//	DDX_Control(pDX, IDC_LIST_TELEPORT_GROUP, m_csListGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmTeleportAddEvent, CDialog)
	//{{AFX_MSG_MAP(AgcmTeleportAddEvent)
//	ON_BN_CLICKED(IDC_BUTTON_ADD_GROUP, OnButtonAddGroup)
//	ON_BN_CLICKED(IDC_BUTTON_REMOVE_GROUP, OnButtonRemoveGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportAddEvent message handlers

void AgcmTeleportAddEvent::OnButtonAddGroup() 
{
	// TODO: Add your control notification handler code here
	
	CHAR	*szGroupName	= NULL;

	AgcmTeleportGroupList	csGroupList;

	csGroupList.InitData(m_pcsAgpmEventTeleport, &szGroupName);

	csGroupList.DoModal();

	if (szGroupName)
	{
		AgpdTeleportGroup	*pcsGroup	= m_pcsAgpmEventTeleport->GetTeleportGroup(szGroupName);
		if (!pcsGroup)
			return;

		/*
		// add group info into event
		m_pcsAgpmEventTeleport->AddGroupToEvent(szGroupName, &m_csTempEvent);

		INT32	lAddIndex	= m_csListGroup.AddString(szGroupName);
		if (lAddIndex != LB_ERR)
			m_csListGroup.SetItemDataPtr(lAddIndex, pcsGroup);
			*/
	}
}

void AgcmTeleportAddEvent::OnButtonRemoveGroup() 
{
	// TODO: Add your control notification handler code here

	INT32				lIndex = m_csListGroup.GetCurSel();

	if (lIndex == LB_ERR)
		return;

	/*
	AgpdTeleportGroup *pcsGroup = (AgpdTeleportGroup *) m_csListGroup.GetItemDataPtr(lIndex);
	if (pcsGroup)
	{
		m_pcsAgpmEventTeleport->RemoveGroupFromEvent(pcsGroup->m_szGroupName, &m_csTempEvent);
	}

	m_csListGroup.DeleteString(lIndex);
	*/
	
}

void AgcmTeleportAddEvent::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	GlobalFree(m_csTempEvent.m_pvData);

	m_csListGroup.ResetContent();

	CDialog::OnCancel();
}

void AgcmTeleportAddEvent::OnOK() 
{
	// TODO: Add extra validation here

	CopyMemory(m_pcsEvent->m_pvData, m_csTempEvent.m_pvData, sizeof(AgpdTeleportAttach));

	GlobalFree(m_csTempEvent.m_pvData);

	m_csListGroup.ResetContent();
	
	CDialog::OnOK();
}

BOOL AgcmTeleportAddEvent::InitData(AgpmEventTeleport *pcsAgpmEventTeleport, ApdEvent *pcsEvent)
{
	if (!pcsAgpmEventTeleport || !pcsEvent)
		return FALSE;

	m_pcsAgpmEventTeleport		= pcsAgpmEventTeleport;
	m_pcsEvent					= pcsEvent;

	if (!pcsEvent->m_pvData)
		return FALSE;

	m_csTempEvent.m_pcsSource	= pcsEvent->m_pcsSource;
	m_csTempEvent.m_eFunction	= pcsEvent->m_eFunction;

	m_csTempEvent.m_pvData		= (PVOID) GlobalAlloc(GMEM_FIXED, sizeof(AgpdTeleportAttach));

	CopyMemory(m_csTempEvent.m_pvData, pcsEvent->m_pvData, sizeof(AgpdTeleportAttach));

	return TRUE;
}

BOOL AgcmTeleportAddEvent::OnInitDialog() 
{
   CDialog::OnInitDialog();
   
   // TODO: Add extra initialization here

	if (m_csTempEvent.m_pvData)
	{
		AgpdTeleportAttach			*pcsAttach		= (AgpdTeleportAttach *) m_csTempEvent.m_pvData;

		/*
		for (int i = 0; i < pcsAttach->m_unGroupCount; ++i)
		{
			AgpdTeleportGroup		*pcsGroup		= m_pcsAgpmEventTeleport->GetTeleportGroup(pcsAttach->m_aszGroupName[i]);
			if (!pcsGroup)
				continue;

			INT32	lAddIndex	= m_csListGroup.AddString(pcsGroup->m_szGroupName);
			if (lAddIndex != LB_ERR)
				m_csListGroup.SetItemDataPtr(lAddIndex, pcsGroup);
		}
		*/
	}

   return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
