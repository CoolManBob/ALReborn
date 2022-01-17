// AgcmTeleportGroup.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventteleportdlg.h"
#include "AgcmTeleportGroup.h"
#include "AgcmTeleportTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroup dialog


AgcmTeleportGroup::AgcmTeleportGroup(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmTeleportGroup::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmTeleportGroup)
	m_strDescription = _T("");
	m_strGroupName = _T("");
	//}}AFX_DATA_INIT
}


void AgcmTeleportGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmTeleportGroup)
	DDX_Control(pDX, IDC_LIST_WHOLE_TELEPORT_POINT, m_csListWholePoint);
	DDX_Control(pDX, IDC_LIST_GROUP_POINT, m_csListGroupPoint);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_EDIT_GROUP_NAME, m_strGroupName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmTeleportGroup, CDialog)
	//{{AFX_MSG_MAP(AgcmTeleportGroup)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_POINT, OnButtonCreatePoint)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_POINT, OnButtonDeletePoint)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_POINT, OnButtonEditPoint)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO_GROUP, OnButtonMovetoGroup)
	ON_BN_CLICKED(IDC_BUTTON_MOVETO_POINT, OnButtonMovetoPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportGroup message handlers

BOOL AgcmTeleportGroup::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;
	AgpdTeleportPoint *	pcsPoint;

	lIndex = 0;
	for (pcsPoint = m_pcsAgpmEventTeleport->GetSequencePoint(&lIndex); pcsPoint; pcsPoint = m_pcsAgpmEventTeleport->GetSequencePoint(&lIndex))
	{
		lListIndex = m_csListWholePoint.AddString(pcsPoint->m_szPointName);
		if (lListIndex == LB_ERR)
			return FALSE;

		m_csListWholePoint.SetItemDataPtr(lListIndex, pcsPoint);
	}

	return InitDialogData();   // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AgcmTeleportGroup::InitDialogData()
{
	INT32				lIndex;
	INT32				lListIndex;
	AgpdTeleportPoint *	pcsPoint;

	m_csListGroupPoint.ResetContent();

	if (m_pcsGroup)
	{
		lIndex = 0;
		for (pcsPoint = m_pcsAgpmEventTeleport->GetSequencePointInGroup(m_pcsGroup, &lIndex); pcsPoint; pcsPoint = m_pcsAgpmEventTeleport->GetSequencePointInGroup(m_pcsGroup, &lIndex))
		{
			lListIndex = m_csListGroupPoint.AddString(pcsPoint->m_szPointName);
			if (lListIndex == LB_ERR)
				return FALSE;

			m_csListGroupPoint.SetItemDataPtr(lListIndex, pcsPoint);

			m_csListWholePoint.DeleteString(m_csListWholePoint.FindStringExact(-1, pcsPoint->m_szPointName));
		}
	}

	UpdateData(FALSE);

	return TRUE;
}

BOOL AgcmTeleportGroup::InitData(AgpmEventTeleport *pcsAgpmEventTeleport, AgpdTeleportGroup *pcsGroup)
{
	if (!pcsAgpmEventTeleport)
		return FALSE;

	m_pcsAgpmEventTeleport	= pcsAgpmEventTeleport;
	m_pcsGroup				= pcsGroup;

	if (m_pcsGroup)
	{
		m_strGroupName			= pcsGroup->m_szGroupName;
		m_strDescription		= pcsGroup->m_szDescription;
	}

	return TRUE;
}

void AgcmTeleportGroup::OnButtonCreatePoint() 
{
	AgcmTeleportTarget	dlgTarget(m_pcsAgpmEventTeleport);

	dlgTarget.InitData(NULL, NULL);

	UpdateData();

	dlgTarget.DoModal();

	InitDialogData();
}

void AgcmTeleportGroup::OnButtonDeletePoint() 
{
	INT32				lIndex = m_csListWholePoint.GetCurSel();
	AgpdTeleportPoint*	pcsPoint;

	if (lIndex == LB_ERR)
		return;

	pcsPoint = (AgpdTeleportPoint *) m_csListWholePoint.GetItemDataPtr(lIndex);

	if (pcsPoint &&	m_pcsAgpmEventTeleport->RemoveTeleportPoint(pcsPoint, TRUE))
		m_csListWholePoint.DeleteString(lIndex);

	return;
}

void AgcmTeleportGroup::OnButtonEditPoint() 
{
	INT32				lIndex = m_csListWholePoint.GetCurSel();
	AgpdTeleportPoint*	pcsPoint;
	AgcmTeleportTarget	dlgTarget(m_pcsAgpmEventTeleport);

	if (lIndex == LB_ERR)
		return;

	pcsPoint = (AgpdTeleportPoint *) m_csListWholePoint.GetItemDataPtr(lIndex);
	if (!pcsPoint)
		return;

	dlgTarget.InitData(NULL, pcsPoint);

	UpdateData();

	dlgTarget.DoModal();

	InitDialogData();
}

void AgcmTeleportGroup::OnButtonMovetoGroup() 
{
	INT32				lIndex		= m_csListWholePoint.GetCurSel();
	if (lIndex == LB_ERR)
		return;

	AgpdTeleportPoint *pcsPoint = (AgpdTeleportPoint *) m_csListWholePoint.GetItemDataPtr(lIndex);
	if (!pcsPoint)
		return;

	INT32				lAddIndex	= m_csListGroupPoint.AddString(pcsPoint->m_szPointName);
	if (lAddIndex == LB_ERR)
		return;

	if (m_csListGroupPoint.SetItemDataPtr(lAddIndex, pcsPoint) == LB_ERR)
		return;

	m_csListWholePoint.DeleteString(lIndex);

}

void AgcmTeleportGroup::OnButtonMovetoPoint() 
{
	INT32				lIndex		= m_csListGroupPoint.GetCurSel();
	if (lIndex == LB_ERR)
		return;

	AgpdTeleportPoint *pcsPoint = (AgpdTeleportPoint *) m_csListGroupPoint.GetItemDataPtr(lIndex);
	if (!pcsPoint)
		return;

	INT32				lAddIndex	= m_csListWholePoint.AddString(pcsPoint->m_szPointName);
	if (lAddIndex == LB_ERR)
		return;

	if (m_csListWholePoint.SetItemDataPtr(lAddIndex, pcsPoint) == LB_ERR)
		return;

	m_csListGroupPoint.DeleteString(lIndex);

}

void AgcmTeleportGroup::OnCancel() 
{
	m_csListGroupPoint.ResetContent();
	m_csListWholePoint.ResetContent();

	CDialog::OnCancel();
}

void AgcmTeleportGroup::OnOK() 
{
	AgpdTeleportPoint *	pcsPoint;

	UpdateData(TRUE);

	if (!m_pcsGroup)
	{
		m_pcsGroup = m_pcsAgpmEventTeleport->AddTeleportGroup((LPSTR)(LPCTSTR) m_strGroupName, (LPSTR)(LPCTSTR) m_strDescription);
		if (!m_pcsGroup)
			return;
	}
	else
	{
		if (strcmp(m_strGroupName, m_pcsGroup->m_szGroupName))
			m_pcsAgpmEventTeleport->ChangeTeleportGroupName(m_pcsGroup, (LPSTR) (LPCTSTR) m_strGroupName);

		strcpy(m_pcsGroup->m_szDescription, m_strDescription);

		m_pcsAgpmEventTeleport->DeleteAllPointInGroup(m_pcsGroup);
	}

	for (int i = 0; i < m_csListGroupPoint.GetCount(); ++i)
	{
		pcsPoint = (AgpdTeleportPoint *) m_csListGroupPoint.GetItemDataPtr(i);
		if (!pcsPoint || pcsPoint == (PVOID) -1)
		{
			AfxMessageBox("Error Getting Group List !!!");
			return;
		}

		m_pcsAgpmEventTeleport->AddPointToGroup(m_pcsGroup, pcsPoint);
	}

	CDialog::OnOK();
}
