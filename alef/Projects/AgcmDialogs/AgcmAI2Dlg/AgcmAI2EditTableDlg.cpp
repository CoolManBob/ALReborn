#include "stdafx.h"
#include "../resource.h"
#include "agcmai2dlg.h"
#include "AgcmAI2EditTableDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


AgcmAI2EditTableDlg::AgcmAI2EditTableDlg(CWnd* pParent /*=NULL*/) : CDialog(AgcmAI2EditTableDlg::IDD, pParent)
{
}

void AgcmAI2EditTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAI2EditTableDlg)
	DDX_Control(pDX, IDC_ProbableEdit, m_cProbableEdit);
	DDX_Control(pDX, IDC_ConditionCheckCombo, m_cConditionCheck);
	DDX_Control(pDX, IDC_OperatorCombo, m_cOperatorCombo);
	DDX_Control(pDX, IDC_ParameterEdit, m_cParameterEdit);
	DDX_Control(pDX, IDC_PercentCombo, m_cPercentCombo);
	DDX_Control(pDX, IDC_TimerEdit, m_cTimerEdit);
	DDX_Control(pDX, IDC_TargetParameterCombo, m_cTargetParameterCombo);
	DDX_Control(pDX, IDC_ComboBox, m_cComboBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AgcmAI2EditTableDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmAI2EditTableDlg)
	ON_BN_CLICKED(IDC_EditTableOKButton, OnEditTableOKButton)
	ON_BN_CLICKED(IDC_EditTableCancelButton, OnEditTableCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAI2EditTableDlg message handlers
void AgcmAI2EditTableDlg::InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2, AgpdAI2Template *pcsAI2Template, eAgcmAI2DialogEditStatus eStatus, void *pvData )
{
	m_pcsAgpmItem = pcsAgpmItem;
	m_pcsAgpmSkill = pcsAgpmSkill;
	m_pcsAgpmAI2 = pcsAgpmAI2;
	m_pcsAgpdAI2Template = pcsAI2Template;
	m_eEditStatus = eStatus;
	m_pvEditTable = pvData;
}

void AgcmAI2EditTableDlg::ClearControl()
{
	m_cParameterEdit.SetWindowText( "" );
	m_cTimerEdit.SetWindowText( "" );
	m_cComboBox.ResetContent();

	m_cConditionCheck.SetCurSel( -1 );
	m_cOperatorCombo.SetCurSel( -1 );
	m_cPercentCombo.SetCurSel( -1 );
	m_cTargetParameterCombo.SetCurSel( -1 );
}

void AgcmAI2EditTableDlg::FillControlByItem( AgpdAI2UseItem *pcsItem )
{
	AgpaItemTemplate		*pcsAgpaItemTemplate;
	AgpdItemTemplate		*pcsItemTemplate;

	pcsAgpaItemTemplate = m_pcsAgpmItem->GetItemTemplateAdmin();

	if( pcsAgpaItemTemplate != NULL )
	{
		pcsItemTemplate = pcsAgpaItemTemplate->GetTemplate( pcsItem->m_lItemID );

		if( pcsItemTemplate != NULL )
		{
			INT32			lIndex;

			lIndex = m_cComboBox.FindStringExact( -1, pcsItemTemplate->m_szName );

			if( lIndex != CB_ERR )
			{
				char			strBuffer[80];
				int				iCounter;
				int				iTempData;

				//아이템을 선택한다.
				m_cComboBox.SetCurSel( lIndex );

				//대상 인자를 결정한다.
				for( iCounter=0;iCounter<m_cTargetParameterCombo.GetCount(); iCounter++ )
				{
					iTempData = m_cTargetParameterCombo.GetItemData(iCounter);

					if( iTempData == pcsItem->m_csTable.m_lTargetParameter )
					{
						m_cTargetParameterCombo.SetCurSel( iCounter );
						break;
					}
				}

				itoa( pcsItem->m_csTable.m_lParameter, strBuffer, 10 );
				m_cParameterEdit.SetWindowText( strBuffer );

				if( pcsItem->m_csTable.m_bPercent == AGPMAI2_BASE_PERCENT )
				{
					m_cPercentCombo.SetCurSel( 0 );
				}

				//연산자를 결정한다.
				for( iCounter=0;iCounter<m_cOperatorCombo.GetCount(); iCounter++ )
				{
					iTempData = m_cOperatorCombo.GetItemData(iCounter);

					if( iTempData == pcsItem->m_csTable.m_lOperator )
					{
						m_cOperatorCombo.SetCurSel( iCounter );
						break;
					}
				}

				//연산 조건을 본다.
				for( iCounter=0;iCounter<m_cConditionCheck.GetCount(); iCounter++ )
				{
					iTempData = m_cConditionCheck.GetItemData(iCounter);

					if( iTempData == pcsItem->m_csTable.m_lConditionCheck )
					{
						m_cConditionCheck.SetCurSel( iCounter );
						break;
					}
				}

				if( pcsItem->m_csTable.m_lConditionCheck == AGPMAI2_CONDITION_TIMER )
				{
					itoa( pcsItem->m_csTable.m_lTimerCount, strBuffer, 10 );
					m_cTimerEdit.SetWindowText( strBuffer );					
				}

				itoa( pcsItem->m_csTable.m_lProbable, strBuffer, 10 );
				m_cProbableEdit.SetWindowText( strBuffer );
			}
		}
	}
}

void AgcmAI2EditTableDlg::FillControlBySkill( AgpdAI2UseSkill *pcsSkill )
{
	AgpaSkillTemplate		*pcsAgpaSkillTemplate;
	AgpdSkillTemplate		*pcsSkillTemplate;

	pcsAgpaSkillTemplate = &m_pcsAgpmSkill->m_csAdminTemplate;

	if( pcsAgpaSkillTemplate != NULL )
	{
		pcsSkillTemplate = pcsAgpaSkillTemplate->GetSkillTemplate( pcsSkill->m_lSkillID );

		if( pcsSkillTemplate != NULL )
		{
			INT32			lIndex;

			lIndex = m_cComboBox.FindStringExact( -1, pcsSkillTemplate->m_szName );

			if( lIndex != CB_ERR )
			{
				char			strBuffer[80];
				int				iCounter;
				int				iTempData;

				//아이템을 선택한다.
				m_cComboBox.SetCurSel( lIndex );

				//대상 인자를 결정한다.
				for( iCounter=0;iCounter<m_cTargetParameterCombo.GetCount(); iCounter++ )
				{
					iTempData = m_cTargetParameterCombo.GetItemData(iCounter);

					if( iTempData == pcsSkill->m_csTable.m_lTargetParameter )
					{
						m_cTargetParameterCombo.SetCurSel( iCounter );
						break;
					}
				}

				itoa( pcsSkill->m_csTable.m_lParameter, strBuffer, 10 );
				m_cParameterEdit.SetWindowText( strBuffer );

				if( pcsSkill->m_csTable.m_bPercent == AGPMAI2_BASE_PERCENT )
				{
					m_cPercentCombo.SetCurSel( 0 );
				}

				//연산자를 결정한다.
				for( iCounter=0;iCounter<m_cOperatorCombo.GetCount(); iCounter++ )
				{
					iTempData = m_cOperatorCombo.GetItemData(iCounter);

					if( iTempData == pcsSkill->m_csTable.m_lOperator )
					{
						m_cOperatorCombo.SetCurSel( iCounter );
						break;
					}
				}

				//연산 조건을 본다.
				for( iCounter=0;iCounter<m_cConditionCheck.GetCount(); iCounter++ )
				{
					iTempData = m_cConditionCheck.GetItemData(iCounter);

					if( iTempData == pcsSkill->m_csTable.m_lConditionCheck )
					{
						m_cConditionCheck.SetCurSel( iCounter );
						break;
					}
				}

				if( pcsSkill->m_csTable.m_lConditionCheck == AGPMAI2_CONDITION_TIMER )
				{
					itoa( pcsSkill->m_csTable.m_lTimerCount, strBuffer, 10 );
					m_cTimerEdit.SetWindowText( strBuffer );					
				}

				itoa( pcsSkill->m_csTable.m_lProbable, strBuffer, 10 );
				m_cProbableEdit.SetWindowText( strBuffer );
			}
		}
	}
}

void AgcmAI2EditTableDlg::FillComboItem()
{
	AgpaItemTemplate		*pcsAgpaItemTemplate;
	AgpdItemTemplate		**ppcsItemTemplate;
	
	int				iIndex;
	int				iItemData;

	pcsAgpaItemTemplate = m_pcsAgpmItem->GetItemTemplateAdmin();
	iIndex = 0;

	while( 1 ) 
	{
		ppcsItemTemplate = (AgpdItemTemplate **)pcsAgpaItemTemplate->GetObjectSequence( &iIndex );

		if( ppcsItemTemplate == NULL )
		{
			break;
		}
		else
		{
			iItemData = m_cComboBox.AddString( (*ppcsItemTemplate)->m_szName );

			if( iItemData != CB_ERR )
			{
				m_cComboBox.SetItemData( iItemData, (*ppcsItemTemplate)->m_lID );
			}
		}
	}
}

void AgcmAI2EditTableDlg::FillComboSkill()
{
	AgpaSkillTemplate		*pcsAgpaSkillTemplate;
	AgpdSkillTemplate		**ppcsSkillTemplate;
	
	int				iIndex;
	int				iItemData;

	pcsAgpaSkillTemplate = &m_pcsAgpmSkill->m_csAdminTemplate;
	iIndex = 0;

	while( 1 ) 
	{
		ppcsSkillTemplate = (AgpdSkillTemplate **)pcsAgpaSkillTemplate->GetObjectSequence( &iIndex );

		if( ppcsSkillTemplate == NULL )
		{
			break;
		}
		else
		{
			iItemData = m_cComboBox.AddString( (*ppcsSkillTemplate)->m_szName );

			if( iItemData != CB_ERR )
			{
				m_cComboBox.SetItemData( iItemData, (*ppcsSkillTemplate)->m_lID );
			}
		}
	}
}

BOOL AgcmAI2EditTableDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	INT32			lResultIndex;

	ClearControl();

	//Set Target Parameter
	lResultIndex = m_cTargetParameterCombo.AddString( "HP" );
	m_cTargetParameterCombo.SetItemData( lResultIndex, AGPMAI2_TARGET_PARAMETER_HP );

	lResultIndex = m_cTargetParameterCombo.AddString( "MP" );
	m_cTargetParameterCombo.SetItemData( lResultIndex, AGPMAI2_TARGET_PARAMETER_MP );

	lResultIndex = m_cTargetParameterCombo.AddString( "SP" );
	m_cTargetParameterCombo.SetItemData( lResultIndex, AGPMAI2_TARGET_PARAMETER_SP );

	//Set Percent
	lResultIndex = m_cPercentCombo.AddString( "%" );
	m_cPercentCombo.SetItemData( lResultIndex, AGPMAI2_BASE_PERCENT );

	//Set Operator
	lResultIndex = m_cOperatorCombo.AddString( "= (같다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_EQUAL );

	lResultIndex = m_cOperatorCombo.AddString( "!= (다르다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_NOT_EQUAL );

	lResultIndex = m_cOperatorCombo.AddString( "> (크다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_BIGGER );

	lResultIndex = m_cOperatorCombo.AddString( ">= (크거나 같다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_BIGGER_EQUAL );

	lResultIndex = m_cOperatorCombo.AddString( "< (작다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_LESS );

	lResultIndex = m_cOperatorCombo.AddString( "<= (작거나 같다)" );
	m_cOperatorCombo.SetItemData( lResultIndex, AGPMAI2_OPERATOR_LESS_EQUAL );

	//Set Condition Check
	lResultIndex = m_cConditionCheck.AddString( "타이머(단, 0이면 항상)" );
	m_cConditionCheck.SetItemData( lResultIndex, AGPMAI2_CONDITION_TIMER );

	lResultIndex = m_cConditionCheck.AddString( "한번만" );
	m_cConditionCheck.SetItemData( lResultIndex, AGPMAI2_CONDITION_COUNT );

	if( m_eEditStatus == AGCMAI2DIALOG_EDITDLG_ADDITEM )
	{
		FillComboItem();
	}
	else if( m_eEditStatus == AGCMAI2DIALOG_EDITDLG_UPDATEITEM )
	{
		FillComboItem();

		FillControlByItem( (AgpdAI2UseItem *)m_pvEditTable );
	}
	else if( m_eEditStatus == AGCMAI2DIALOG_EDITDLG_ADDSKILL )
	{
		FillComboSkill();
	}
	else if( m_eEditStatus == AGCMAI2DIALOG_EDITDLG_UPDATESKILL )
	{
		FillComboSkill();
		FillControlBySkill( (AgpdAI2UseSkill *)m_pvEditTable );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAI2EditTableDlg::OnEditTableOKButton() 
{
	// TODO: Add your control notification handler code here
	INT32			lIndex;
	INT32			lTID;
	BOOL			bResult;
	char			strBuffer[80];

	bResult = TRUE;

	lIndex = m_cComboBox.GetCurSel();
	if( lIndex == CB_ERR )
		bResult = FALSE;
	
	lIndex = m_cTargetParameterCombo.GetCurSel();
	if( lIndex == CB_ERR )
		bResult = FALSE;

	m_cParameterEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
	if( strlen(strBuffer) <= 0 )
		bResult = FALSE;

	//퍼센트는 체크하지 않는다.
/*	lIndex = m_cPercentCombo.GetCurSel();
	if( lIndex == CB_ERR )
		bResult = FALSE;*/

	lIndex = m_cOperatorCombo.GetCurSel();
	if( lIndex == CB_ERR )
		bResult = FALSE;

	lIndex = m_cConditionCheck.GetCurSel();
	if( lIndex == CB_ERR )
		bResult = FALSE;

	if( m_cConditionCheck.GetItemData(m_cConditionCheck.GetCurSel()) == AGPMAI2_CONDITION_TIMER )
	{
		m_cTimerEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
		if( strlen(strBuffer) <= 0 )
			bResult = FALSE;
	}

	if( bResult == FALSE )
	{
		MessageBox( "모두 입력하셨는지 확인해주십시요." );
		return;
	}


	lTID = m_cComboBox.GetItemData( m_cComboBox.GetCurSel() );

	if( lTID != 0 )
	{
		//아이템인 경우.
		if( AGCMAI2DIALOG_EDITDLG_ADDITEM || AGCMAI2DIALOG_EDITDLG_UPDATEITEM )
		{
			AgpdAI2UseItem		*pcsUseItem;

			pcsUseItem = (AgpdAI2UseItem *)m_pvEditTable;

			pcsUseItem->m_lItemID = lTID;
			pcsUseItem->m_csTable.m_lTargetParameter = m_cTargetParameterCombo.GetItemData( m_cTargetParameterCombo.GetCurSel() );
			pcsUseItem->m_csTable.m_lOperator = m_cOperatorCombo.GetItemData( m_cOperatorCombo.GetCurSel() );
			pcsUseItem->m_csTable.m_lConditionCheck = m_cConditionCheck.GetItemData( m_cConditionCheck.GetCurSel() );

			if( m_cPercentCombo.GetCurSel() == CB_ERR )
				pcsUseItem->m_csTable.m_bPercent = FALSE;
			else
				pcsUseItem->m_csTable.m_bPercent = m_cPercentCombo.GetItemData( m_cPercentCombo.GetCurSel() );

			m_cParameterEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsUseItem->m_csTable.m_lParameter = atoi(strBuffer);

			if( m_cConditionCheck.GetItemData(m_cConditionCheck.GetCurSel()) == AGPMAI2_CONDITION_TIMER )
			{
				m_cTimerEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
				pcsUseItem->m_csTable.m_lTimerCount = atoi( strBuffer );
			}

			m_cProbableEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsUseItem->m_csTable.m_lProbable = atoi(strBuffer);
		}
		//Skill인경우.
		else
		{
			AgpdAI2UseSkill		*pcsUseSkill;

			pcsUseSkill = (AgpdAI2UseSkill *)m_pvEditTable;

			pcsUseSkill->m_lSkillID = lTID;
			pcsUseSkill->m_csTable.m_lTargetParameter = m_cTargetParameterCombo.GetItemData( m_cTargetParameterCombo.GetCurSel() );
			pcsUseSkill->m_csTable.m_bPercent = m_cPercentCombo.GetItemData( m_cPercentCombo.GetCurSel() );
			pcsUseSkill->m_csTable.m_lOperator = m_cOperatorCombo.GetItemData( m_cOperatorCombo.GetCurSel() );
			pcsUseSkill->m_csTable.m_lConditionCheck = m_cConditionCheck.GetItemData( m_cConditionCheck.GetCurSel() );

			m_cParameterEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsUseSkill->m_csTable.m_lParameter = atoi(strBuffer);

			if( m_cConditionCheck.GetItemData(m_cConditionCheck.GetCurSel()) == AGPMAI2_CONDITION_TIMER )
			{
				m_cTimerEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
				pcsUseSkill->m_csTable.m_lTimerCount = atoi( strBuffer );
			}

			m_cProbableEdit.GetWindowText( strBuffer, sizeof(strBuffer) );
			pcsUseSkill->m_csTable.m_lProbable = atoi(strBuffer);
		}
	}
	else
	{
		MessageBox( "존재하지 않는 TID임." );
	}

	OnOK();	
}

void AgcmAI2EditTableDlg::OnEditTableCancelButton() 
{
	// TODO: Add your control notification handler code here
	OnCancel();	
}

BOOL AgcmAI2EditTableDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
	{
		if( pMsg->wParam == VK_RETURN )
		{
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
