// AgcmTeleportTarget.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmEventTeleportDlg.h"
#include "AgcmTeleportGroup.h"
#include "AgcmTeleportTarget.h"
//#include "AgcmTeleportTargetList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportTarget dialog


AgcmTeleportTarget::AgcmTeleportTarget(AgpmEventTeleport *pcsAgpmEventTeleport, CWnd* pParent /*=NULL*/) : CDialog(AgcmTeleportTarget::IDD, pParent),
 m_pcsAgpmEventTeleport(pcsAgpmEventTeleport),
 m_bUseTypeHuman(FALSE),
 m_bUseTypeOrc(FALSE),
 m_bUseTypeMoonElf(FALSE)
{
	//{{AFX_DATA_INIT(AgcmTeleportTarget)
	m_fX		= 0.0f;
	m_fY		= 0.0f;
	m_fZ		= 0.0f;
	m_ulIndex	= 0;
	m_nBaseType	= -1;
	m_lID		= 0;
	m_fRadiusMin	= 0.0f;
	m_fRadiusMax	= 0.0f;
	m_bTypePos		= FALSE;
	m_bTypeIndex	= FALSE;
	m_bTypeBase		= FALSE;
	m_strDescription= _T("");
	m_strPointName	= _T("");
	m_pcsPoint		= NULL;
	m_bSiegeWarOnly	= FALSE;
	//}}AFX_DATA_INIT
}

void AgcmTeleportTarget::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmTeleportTarget)
	DDX_Control(pDX, IDC_TELEPORT_POINT_TARGET, m_csPointTarget);
	DDX_Control(pDX, IDC_TELEPORT_POINT_GROUP, m_csPointGroup);
	DDX_Control(pDX, IDC_TELEPORT_GROUP_LIST, m_csGroupList);
	DDX_Text(pDX, IDC_TARGET_POS_X, m_fX);
	DDX_Text(pDX, IDC_TARGET_POS_Y, m_fY);
	DDX_Text(pDX, IDC_TARGET_POS_Z, m_fZ);
	DDX_CBIndex(pDX, IDC_TARGET_BASE_TYPE, m_nBaseType);
	DDX_Text(pDX, IDC_TARGET_BASE_ID, m_lID);
	DDX_Text(pDX, IDC_TARGET_RADIUS_MIN, m_fRadiusMin);
	DDX_Text(pDX, IDC_TARGET_RADIUS_MAX, m_fRadiusMax);
	DDX_Check(pDX, IDC_TELEPORT_TYPE_POS, m_bTypePos);
	DDX_Check(pDX, IDC_TELEPORT_TYPE_BASE, m_bTypeBase);
	DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_EDIT_POINT_NAME, m_strPointName);
	DDX_Text(pDX, IDC_CHECK_SIEGEWAR , m_bSiegeWarOnly);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_USE_HUMAN, m_bUseTypeHuman);
	DDX_Check(pDX, IDC_CHECK_USE_ORC, m_bUseTypeOrc);
	DDX_Check(pDX, IDC_CHECK_USE_MOONELF, m_bUseTypeMoonElf);
}


BEGIN_MESSAGE_MAP(AgcmTeleportTarget, CDialog)
	//{{AFX_MSG_MAP(AgcmTeleportTarget)
	ON_BN_CLICKED(IDC_TELEPORT_TYPE_BASE, OnTeleportTypeBase)
	ON_BN_CLICKED(IDC_TELEPORT_TYPE_POS, OnTeleportTypePos)
	ON_BN_CLICKED(IDC_TELEPORT_ADD_GROUP, OnTeleportAddGroup)
	ON_BN_CLICKED(IDC_TELEPORT_REMOVE_GROUP, OnTeleportRemoveGroup)
	ON_BN_CLICKED(IDC_TELEPORT_ADD_TARGET, OnTeleportAddTarget)
	ON_BN_CLICKED(IDC_TELEPORT_REMOVE_TARGET, OnTeleportRemoveTarget)
	ON_BN_CLICKED(IDC_TELEPORT_GROUP_ADD, OnTeleportGroupAdd)
	ON_BN_CLICKED(IDC_TELEPORT_GROUP_EDIT, OnTeleportGroupEdit)
	ON_BN_CLICKED(IDC_TELEPORT_GROUP_DELETE, OnTeleportGroupDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmTeleportTarget message handlers

void	AgcmTeleportTarget::OnOK() 
{
	UpdateData(TRUE);

	if (!m_pcsPoint)
	{
		m_pcsPoint = m_pcsAgpmEventTeleport->AddTeleportPoint((LPSTR) (LPCTSTR) m_strPointName, (LPSTR) (LPCTSTR) m_strDescription);
		if (!m_pcsPoint)
		{
			ASSERT(!"Can't Add Teleport Point !!!");
			return;
		}
	}
	else
	{
		if (strcmp(m_pcsPoint->m_szPointName, m_strPointName))
			m_pcsAgpmEventTeleport->ChangeTeleportPointName(m_pcsPoint, (LPSTR) (LPCTSTR) m_strPointName);

		strcpy(m_pcsPoint->m_szDescription, (LPSTR)(LPCTSTR) m_strDescription);
	}

	m_pcsPoint->m_fRadiusMin = m_fRadiusMin * 100.0f; // 단위 미터 환산~ 즐~..
	m_pcsPoint->m_fRadiusMax = m_fRadiusMax * 100.0f;

	if (m_bSiegeWarOnly)
		m_pcsPoint->m_eSpecialType	= AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR;

	if (m_bTypePos)
	{
		m_pcsPoint->m_eType = AGPDTELEPORT_TARGET_TYPE_POS;

		m_pcsPoint->m_uniTarget.m_stPos.x = m_fX;
		m_pcsPoint->m_uniTarget.m_stPos.y = m_fY;
		m_pcsPoint->m_uniTarget.m_stPos.z = m_fZ;
	}
	else if (m_bTypeBase)
	{
		m_pcsPoint->m_eType = AGPDTELEPORT_TARGET_TYPE_BASE;

		switch (m_nBaseType)
		{
		case 0:
			m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType = APBASE_TYPE_OBJECT;
			break;

		case 1:
			m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType = APBASE_TYPE_CHARACTER;
			break;

		case 2:
			m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType = APBASE_TYPE_ITEM;
			break;
		}

		m_pcsPoint->m_uniTarget.m_stBase.m_lID = m_lID;
	}

	m_pcsAgpmEventTeleport->DeleteAllGroupInPoint(m_pcsPoint);

	AgpdTeleportGroup *	pcsGroup;
	for (INT32 lIndex = 0; lIndex < m_csPointGroup.GetCount(); ++lIndex)
	{
		pcsGroup = (AgpdTeleportGroup *) m_csPointGroup.GetItemDataPtr(lIndex);
		if (!pcsGroup || pcsGroup == (PVOID) -1)
		{
			AfxMessageBox("Error Getting Point Group !!!");
			return;
		}

		m_pcsAgpmEventTeleport->AddPointToGroup(pcsGroup, m_pcsPoint);
	}

	for (INT32 lIndex = 0; lIndex < m_csPointTarget.GetCount(); ++lIndex)
	{
		pcsGroup = (AgpdTeleportGroup *) m_csPointTarget.GetItemDataPtr(lIndex);
		if (!pcsGroup || pcsGroup == (PVOID) -1)
		{
			AfxMessageBox("Error Getting Point Group !!!");
			return;
		}

		m_pcsAgpmEventTeleport->AddTargetGroupToPoint(pcsGroup, m_pcsPoint);
	}

	if (m_pstEvent)
	{
		AgpdTeleportAttach* pstAttach = (AgpdTeleportAttach *) m_pstEvent->m_pvData;

		pstAttach->m_lPointID = m_pcsPoint->m_lID;
		pstAttach->m_pcsPoint = m_pcsPoint;

		m_pcsPoint->m_bAttachEvent = TRUE;
	}

	m_pcsPoint->m_byUseType = 0;
	if( m_bUseTypeHuman )	m_pcsPoint->SetUseType( AgpdTeleportPoint::eTeleportUseHuman );
	if( m_bUseTypeOrc )		m_pcsPoint->SetUseType( AgpdTeleportPoint::eTeleportUseOrc );
	if( m_bUseTypeMoonElf )	m_pcsPoint->SetUseType( AgpdTeleportPoint::eTeleportUseMoonElf );
		
	CDialog::OnOK();
}

BOOL	AgcmTeleportTarget::InitData(ApdEvent *pstEvent, AgpdTeleportPoint *pcsPoint)
{
	m_pstEvent = pstEvent;
	m_pcsPoint = pcsPoint;

	return TRUE;
}

BOOL	AgcmTeleportTarget::InitDialogData()
{
	INT32				lIndex;
	INT32				lListIndex;
	AgpdTeleportGroup *	pcsGroup;

	m_csGroupList.ResetContent();
	m_csPointGroup.ResetContent();
	m_csPointTarget.ResetContent();

	lIndex = 0;
	for (pcsGroup = m_pcsAgpmEventTeleport->GetSequenceGroup(&lIndex); pcsGroup; pcsGroup = m_pcsAgpmEventTeleport->GetSequenceGroup(&lIndex))
	{
		lListIndex = m_csGroupList.AddString(pcsGroup->m_szGroupName);
		if (lListIndex == LB_ERR)
			return FALSE;

		m_csGroupList.SetItemDataPtr(lListIndex, pcsGroup);
	}

	if (m_pstEvent)
	{
		m_bTypeBase = TRUE;
		if (m_pstEvent->m_pcsSource)
		{
			if (m_pstEvent->m_pcsSource->m_eType == APBASE_TYPE_OBJECT)
				m_nBaseType = 0;
			else if  (m_pstEvent->m_pcsSource->m_eType == APBASE_TYPE_CHARACTER)
				m_nBaseType = 1;
			else if  (m_pstEvent->m_pcsSource->m_eType == APBASE_TYPE_ITEM)
				m_nBaseType = 2;

			m_lID = m_pstEvent->m_pcsSource->m_lID;
		}
	}

	if (!m_pcsPoint)
	{
		UpdateData(FALSE);

		return TRUE;
	}

	m_strPointName		= m_pcsPoint->m_szPointName;
	m_strDescription	= m_pcsPoint->m_szDescription;

	m_fRadiusMin = m_pcsPoint->m_fRadiusMin / 100.0f; //단위 미터환산~w
	m_fRadiusMax = m_pcsPoint->m_fRadiusMax / 100.0f;

	m_bTypeIndex = m_bTypePos = m_bTypeBase = FALSE;

	if (m_pcsPoint->m_eSpecialType == AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR)
		m_bSiegeWarOnly	= TRUE;
	else
		m_bSiegeWarOnly	= FALSE;

	if (m_pcsPoint->m_eType == AGPDTELEPORT_TARGET_TYPE_POS)
	{
		m_bTypePos = TRUE;
		m_fX = m_pcsPoint->m_uniTarget.m_stPos.x;
		m_fY = m_pcsPoint->m_uniTarget.m_stPos.y;
		m_fZ = m_pcsPoint->m_uniTarget.m_stPos.z;
	}
	else if (m_pcsPoint->m_eType == AGPDTELEPORT_TARGET_TYPE_BASE)
	{
		m_bTypeBase = TRUE;
		if (m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType == APBASE_TYPE_OBJECT)
			m_nBaseType = 0;
		else if (m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType == APBASE_TYPE_CHARACTER)
			m_nBaseType = 1;
		else if (m_pcsPoint->m_uniTarget.m_stBase.m_eBaseType == APBASE_TYPE_ITEM)
			m_nBaseType = 2;

		m_lID = m_pcsPoint->m_uniTarget.m_stBase.m_lID;
	}

	lIndex = 0;
	for (pcsGroup = m_pcsAgpmEventTeleport->GetSequenceGroupInPoint(m_pcsPoint, &lIndex); pcsGroup; pcsGroup = m_pcsAgpmEventTeleport->GetSequenceGroupInPoint(m_pcsPoint, &lIndex))
	{
		lListIndex = m_csPointGroup.AddString(pcsGroup->m_szGroupName);
		if (lListIndex == LB_ERR)
			return FALSE;

		m_csPointGroup.SetItemDataPtr(lListIndex, pcsGroup);
	}

	lIndex = 0;
	for (pcsGroup = m_pcsAgpmEventTeleport->GetSequenceTargetGroup(m_pcsPoint, &lIndex); pcsGroup; pcsGroup = m_pcsAgpmEventTeleport->GetSequenceTargetGroup(m_pcsPoint, &lIndex))
	{
		lListIndex = m_csPointTarget.AddString(pcsGroup->m_szGroupName);
		if (lListIndex == LB_ERR)
			return FALSE;

		m_csPointTarget.SetItemDataPtr(lListIndex, pcsGroup);
	}

	m_bUseTypeHuman		= m_pcsPoint->IsUseHuman();
	m_bUseTypeOrc		= m_pcsPoint->IsUseOrc();
	m_bUseTypeMoonElf	= m_pcsPoint->IsUseMoonElf();

	UpdateData(FALSE);

	return TRUE;
}

void AgcmTeleportTarget::OnTeleportTypeBase() 
{
	UpdateData(TRUE);

	m_bTypeIndex = FALSE;
	m_bTypePos = FALSE;

	UpdateData(FALSE);
}

void AgcmTeleportTarget::OnTeleportTypePos() 
{
	UpdateData(TRUE);

	m_bTypeIndex = FALSE;
	m_bTypeBase = FALSE;

	UpdateData(FALSE);
}

BOOL AgcmTeleportTarget::Create(CWnd* pParent) 
{
	return CDialog::Create(IDD, pParent);
}

void AgcmTeleportTarget::OnTeleportAddGroup() 
{
	INT32				lListIndex;
	AgpdTeleportGroup *	pcsGroup;

	pcsGroup = (AgpdTeleportGroup *) m_csGroupList.GetItemDataPtr(m_csGroupList.GetCurSel());
	if (!pcsGroup || pcsGroup == (PVOID) -1)
		return;

	if (m_csPointGroup.FindStringExact(-1, pcsGroup->m_szGroupName) != LB_ERR)
		return;

	lListIndex = m_csPointGroup.AddString(pcsGroup->m_szGroupName);
	if (lListIndex == LB_ERR)
		return;

	m_csPointGroup.SetItemDataPtr(lListIndex, pcsGroup);
}

void AgcmTeleportTarget::OnTeleportRemoveGroup() 
{
	AgpdTeleportGroup*	pcsGroup = (AgpdTeleportGroup *) m_csPointGroup.GetItemDataPtr(m_csPointGroup.GetCurSel());
	if (!pcsGroup || pcsGroup == (PVOID) -1)
		return;

	m_csPointGroup.DeleteString(m_csPointGroup.FindStringExact(-1, pcsGroup->m_szGroupName));
}

void AgcmTeleportTarget::OnTeleportAddTarget() 
{
	INT32				lListIndex;
	;

	AgpdTeleportGroup*	pcsGroup = (AgpdTeleportGroup *) m_csGroupList.GetItemDataPtr(m_csGroupList.GetCurSel());
	if ( !pcsGroup || pcsGroup == (PVOID) -1 )
		return;

	if (m_csPointTarget.FindStringExact(-1, pcsGroup->m_szGroupName) != LB_ERR)
		return;

	lListIndex = m_csPointTarget.AddString(pcsGroup->m_szGroupName);
	if (lListIndex == LB_ERR)
		return;

	m_csPointTarget.SetItemDataPtr(lListIndex, pcsGroup);
}

void AgcmTeleportTarget::OnTeleportRemoveTarget() 
{
	AgpdTeleportGroup *	pcsGroup;

	pcsGroup = (AgpdTeleportGroup *) m_csPointTarget.GetItemDataPtr(m_csPointTarget.GetCurSel());
	if (!pcsGroup || pcsGroup == (PVOID) -1)
		return;

	m_csPointTarget.DeleteString(m_csPointTarget.FindStringExact(-1, pcsGroup->m_szGroupName));
}

void AgcmTeleportTarget::OnTeleportGroupAdd() 
{
	AgcmTeleportGroup	dlgGroup;
	dlgGroup.InitData(m_pcsAgpmEventTeleport, NULL);

	UpdateData();

	if (dlgGroup.DoModal() != IDOK)
	{
	}

	InitDialogData();
}

void AgcmTeleportTarget::OnTeleportGroupEdit() 
{
	AgcmTeleportGroup	dlgGroup;
	AgpdTeleportGroup *	pcsGroup;

	pcsGroup = (AgpdTeleportGroup *) m_csGroupList.GetItemDataPtr(m_csGroupList.GetCurSel());
	if (!pcsGroup || pcsGroup == (PVOID) -1)
		return;

	dlgGroup.InitData(m_pcsAgpmEventTeleport, pcsGroup);

	UpdateData();

	dlgGroup.DoModal();

	InitDialogData();
}

void AgcmTeleportTarget::OnTeleportGroupDelete() 
{
	AgpdTeleportGroup *	pcsGroup;

	pcsGroup = (AgpdTeleportGroup *) m_csGroupList.GetItemDataPtr(m_csGroupList.GetCurSel());
	if (!pcsGroup || pcsGroup == (PVOID) -1)
		return;

	UpdateData();

	m_pcsAgpmEventTeleport->RemoveTeleportGroup(pcsGroup);

	InitDialogData();
}

BOOL AgcmTeleportTarget::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return InitDialogData();  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
