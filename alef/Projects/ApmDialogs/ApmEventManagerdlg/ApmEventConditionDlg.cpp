// ApmEventConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ApmEventManagerDlg.h"
#include "ApmEventConditionDlg.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ApmEventConditionDlg dialog


ApmEventConditionDlg::ApmEventConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ApmEventConditionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ApmEventConditionDlg)
	m_fAreaSphereRadius = 0.0f;
	m_fAreaFanRadius = 0.0f;
	m_fAreaFanAngle = 0.0f;
	m_fAreaBoxInfX = 0.0f;
	m_fAreaBoxInfY = 0.0f;
	m_fAreaBoxInfZ = 0.0f;
	m_fAreaBoxSupX = 0.0f;
	m_fAreaBoxSupY = 0.0f;
	m_fAreaBoxSupZ = 0.0f;
	m_bTarget = FALSE;
	m_bArea = FALSE;
	m_bEnvironment = FALSE;
	m_bTime = FALSE;
	//}}AFX_DATA_INIT
}


void ApmEventConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ApmEventConditionDlg)
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_Z, m_csAreaBoxSupZ);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_Y, m_csAreaBoxSupY);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_X, m_csAreaBoxSupX);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_Z, m_csAreaBoxInfZ);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_Y, m_csAreaBoxInfY);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_X, m_csAreaBoxInfX);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_FAN_ANGLE, m_csAreaFanAngle);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_FAN_RADIUS, m_csAreaFanRadius);
	DDX_Control(pDX, IDC_EVENT_CONDITION_AREA_SPHERE_RADIUS, m_csAreaSphereRadius);
	DDX_Control(pDX, IDC_EVENT_CONDITION_TARGET_ITEMS, m_csTargetItems);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_SPHERE_RADIUS, m_fAreaSphereRadius);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_FAN_RADIUS, m_fAreaFanRadius);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_FAN_ANGLE, m_fAreaFanAngle);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_X, m_fAreaBoxInfX);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_Y, m_fAreaBoxInfY);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_INF_Z, m_fAreaBoxInfZ);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_X, m_fAreaBoxSupX);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_Y, m_fAreaBoxSupY);
	DDX_Text(pDX, IDC_EVENT_CONDITION_AREA_BOX_SUP_Z, m_fAreaBoxSupZ);
	DDX_Check(pDX, IDC_EVENT_CONDITION_TARGET, m_bTarget);
	DDX_Check(pDX, IDC_EVENT_CONDITION_AREA, m_bArea);
	DDX_Check(pDX, IDC_EVENT_CONDITION_ENVIRONMENT, m_bEnvironment);
	DDX_Check(pDX, IDC_EVENT_CONDITION_TIME, m_bTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ApmEventConditionDlg, CDialog)
	//{{AFX_MSG_MAP(ApmEventConditionDlg)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_TARGET, OnTarget)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_AREA, OnArea)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_ENVIRONMENT, OnEnvironment)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_TIME, OnTime)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_TARGET_FACTOR, OnEventConditionTargetFactor)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_TARGET_ITEM_ADD, OnEventConditionTargetItemAdd)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_TARGET_ITEM_REMOVE, OnEventConditionTargetItemRemove)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_AREA_SPHERE, OnEventConditionAreaSphere)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_AREA_FAN, OnEventConditionAreaFan)
	ON_BN_CLICKED(IDC_EVENT_CONDITION_AREA_BOX, OnEventConditionAreaBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ApmEventConditionDlg message handlers

BOOL ApmEventConditionDlg::Create() 
{
	return CDialog::Create(IDD, NULL);
}

void ApmEventConditionDlg::InitData(ApdEvent *pstEvent, ApmEventManager *pcsApmEventManager, AgpmItem *pcsAgpmItem)
{
	m_pstEvent = pstEvent;
	m_pcsApmEventManager = pcsApmEventManager;
	m_pcsAgpmItem = pcsAgpmItem;
}

BOOL ApmEventConditionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	AgpdItemTemplate *	pcsItemTemplate = NULL;

	m_eAreaType = APDEVENT_AREA_SPHERE;

	if (!m_pstEvent->m_pstCondition)
	{
		SetTarget(FALSE);
		SetArea(FALSE);
		SetEnvironment(FALSE);
		SetTime(FALSE);
	}
	else
	{
		if (m_pstEvent->m_pstCondition->m_pstTarget)
		{
			INT32	lListIndex;

			SetTarget(TRUE);

			for (lIndex = 0; m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex] && m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex] != AP_INVALID_IID; ++lIndex)
			{
				if (m_pcsAgpmItem)
					pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex]);

				if (!pcsItemTemplate)
					continue;

				lListIndex = m_csTargetItems.AddString(pcsItemTemplate->m_szName);
				if (lListIndex != LB_ERR)
				{
					m_csTargetItems.SetItemData(lListIndex, m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex]);
				}
			}
		}
		else
		{
			SetTarget(FALSE);
		}

		if (m_pstEvent->m_pstCondition->m_pstArea)
		{
			SetArea(TRUE);

			m_eAreaType = m_pstEvent->m_pstCondition->m_pstArea->m_eType;
			switch (m_eAreaType)
			{
			case APDEVENT_AREA_SPHERE:
				m_fAreaSphereRadius = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius / 100.0f;

				break;

			case APDEVENT_AREA_FAN:
				m_fAreaFanRadius = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius / 100.0f;
				m_fAreaFanAngle = 2.0f * (FLOAT) asin(m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2 / 2.0f);

				break;

			case APDEVENT_AREA_BOX:
				m_fAreaBoxInfX = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.x / 100.0f;
				m_fAreaBoxInfY = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.y / 100.0f;
				m_fAreaBoxInfZ = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.z / 100.0f;

				m_fAreaBoxSupX = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.x / 100.0f;
				m_fAreaBoxSupY = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.y / 100.0f;
				m_fAreaBoxSupZ = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.z / 100.0f;

				break;
			}
		}
		else
		{
			SetArea(FALSE);
		}

		if (m_pstEvent->m_pstCondition->m_pstEnvironment)
		{
			SetEnvironment(TRUE);
		}
		else
		{
			SetEnvironment(FALSE);
		}

		if (m_pstEvent->m_pstCondition->m_pstTime)
		{
			SetTime(TRUE);
		}
		else
		{
			SetTime(FALSE);
		}
	}

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ApmEventConditionDlg::SetTarget(BOOL bEnable)
{
	m_bTarget = bEnable;

	m_csTargetItems.EnableWindow(bEnable);
}

void ApmEventConditionDlg::SetArea(BOOL bEnable)
{
	UpdateData();

	m_bArea = bEnable;

	m_csAreaSphereRadius.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_SPHERE));

	m_csAreaFanRadius.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_FAN));
	m_csAreaFanAngle.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_FAN));

	m_csAreaBoxInfX.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));
	m_csAreaBoxInfY.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));
	m_csAreaBoxInfZ.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));

	m_csAreaBoxSupX.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));
	m_csAreaBoxSupY.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));
	m_csAreaBoxSupZ.EnableWindow(bEnable && (m_eAreaType == APDEVENT_AREA_BOX));
}

void ApmEventConditionDlg::SetEnvironment(BOOL bEnable)
{
	m_bEnvironment = bEnable;
}

void ApmEventConditionDlg::SetTime(BOOL bEnable)
{
	m_bTime = bEnable;
}

void ApmEventConditionDlg::OnTarget() 
{
	UpdateData();

	SetTarget(m_bTarget);
}

void ApmEventConditionDlg::OnArea() 
{
	UpdateData();

	SetArea(m_bArea);
}

void ApmEventConditionDlg::OnEnvironment() 
{
	UpdateData();

	SetEnvironment(m_bEnvironment);
}

void ApmEventConditionDlg::OnTime() 
{
	UpdateData();

	SetTime(m_bTime);
}

void ApmEventConditionDlg::OnEventConditionTargetFactor() 
{
	// TODO: Add your control notification handler code here
	
}

void ApmEventConditionDlg::OnEventConditionTargetItemAdd() 
{
	// TODO: Add your control notification handler code here
	
}

void ApmEventConditionDlg::OnEventConditionTargetItemRemove() 
{
	// TODO: Add your control notification handler code here
	
}

void ApmEventConditionDlg::OnEventConditionAreaSphere() 
{
	m_eAreaType = APDEVENT_AREA_SPHERE;

	UpdateData();

	SetArea(m_bArea);
}

void ApmEventConditionDlg::OnEventConditionAreaFan() 
{
	m_eAreaType = APDEVENT_AREA_FAN;

	UpdateData();

	SetArea(m_bArea);
}

void ApmEventConditionDlg::OnEventConditionAreaBox() 
{
	m_eAreaType = APDEVENT_AREA_BOX;

	UpdateData();

	SetArea(m_bArea);
}

void ApmEventConditionDlg::OnOK() 
{
	INT32	lIndex;

	UpdateData();

	if (m_bTarget)
	{
		m_pcsApmEventManager->SetCondition(m_pstEvent, APDEVENT_COND_TARGET);

		for (lIndex = 0; lIndex < m_csTargetItems.GetCount(); ++lIndex)
		{
			m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex] = (INT32)m_csTargetItems.GetItemData(lIndex);
		}
		for (; lIndex < APDEVENT_TARGET_NUMBER; ++lIndex)
		{
			m_pstEvent->m_pstCondition->m_pstTarget->m_alItemTID[lIndex] = 0;
		}
	}
	else if (m_pstEvent->m_pstCondition && m_pstEvent->m_pstCondition->m_pstTarget)
	{
		delete m_pstEvent->m_pstCondition->m_pstTarget;
	}

	if (m_bArea)
	{
		m_pcsApmEventManager->SetCondition(m_pstEvent, APDEVENT_COND_AREA);

		m_pstEvent->m_pstCondition->m_pstArea->m_eType = m_eAreaType;
		switch (m_eAreaType)
		{
		case APDEVENT_AREA_SPHERE:
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius = m_fAreaSphereRadius * 100.0f;

			break;

		case APDEVENT_AREA_FAN:
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius = m_fAreaFanRadius * 100.0f;
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_f2BySinThetaDiv2 = 2.0f * (FLOAT) sin(m_fAreaFanAngle / 2.0f);

			break;

		case APDEVENT_AREA_BOX:
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.x = m_fAreaBoxInfX * 100.0f;
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.y = m_fAreaBoxInfY * 100.0f;
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.inf.z = m_fAreaBoxInfZ * 100.0f;

			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.x = m_fAreaBoxSupX * 100.0f;
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.y = m_fAreaBoxSupY * 100.0f;
			m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stBox.sup.z = m_fAreaBoxSupZ * 100.0f;

			break;
		}
	}
	else if (m_pstEvent->m_pstCondition && m_pstEvent->m_pstCondition->m_pstArea)
	{
		delete m_pstEvent->m_pstCondition->m_pstArea;
	}

	CDialog::OnOK();
}
