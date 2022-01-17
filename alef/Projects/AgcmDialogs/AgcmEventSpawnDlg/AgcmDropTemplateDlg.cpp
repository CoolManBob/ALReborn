// AgcmDropTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventspawndlg.h"
#include "AgcmDropTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmDropTemplateDlg dialog


AgcmDropTemplateDlg::AgcmDropTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmDropTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmDropTemplateDlg)
	//}}AFX_DATA_INIT
}


void AgcmDropTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmDropTemplateDlg)
	DDX_Control(pDX, IDC_Edit_DropRate, m_cDropRateEdit);
	DDX_Control(pDX, IDC_List_DropInfo, m_cDropInfoList);
	DDX_Control(pDX, IDC_Static_DropInfo, m_cDropInfoStaic);
	DDX_Control(pDX, IDC_Combo_DropCategory, m_cDropTemplateCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmDropTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmDropTemplateDlg)
	ON_CBN_SELCHANGE(IDC_Combo_DropCategory, OnSelchangeComboDropCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmDropTemplateDlg message handlers

void AgcmDropTemplateDlg::SetDlgData( AgpmItem *pcsAgpmItem, AgpmDropItem *pcsAgpmDropItem, AgpdDropItemTemplate *pcsAgpdDropItemTemplate )
{
	m_pcsAgpmItem = pcsAgpmItem;
	m_pcsAgpmDropItem = pcsAgpmDropItem;
	m_pcsAgpdDropItemTemplate = pcsAgpdDropItemTemplate;
}

void AgcmDropTemplateDlg::SetDropItemTemplate( AgpdDropItemTemplate *pcsAgpdDropItemTemplate )
{
	m_pcsAgpdDropItemTemplate = pcsAgpdDropItemTemplate;
}

void AgcmDropTemplateDlg::CleanDropItemListRow( int iRow )
{
	m_cDropInfoList.SetItemText( iRow, 0, "N/A" );
	m_cDropInfoList.SetItemText( iRow, 1, "N/A" );
}

void AgcmDropTemplateDlg::ShowCategory( eAgpmDropItemCategory eCategory )
{
	AgpdItemTemplate	*pcsItemTemplate;
	char				strBuffer[80];
	int					lComboCounter;

	m_cDropInfoList.DeleteAllItems();

	m_cDropInfoList.DeleteColumn(4);
	m_cDropInfoList.DeleteColumn(3);
	m_cDropInfoList.DeleteColumn(2);
	m_cDropInfoList.DeleteColumn(1);
	m_cDropInfoList.DeleteColumn(0);

	print_compact_format( strBuffer, "%f", m_pcsAgpdDropItemTemplate->m_cDropCategory[eCategory].m_fDropProbable );
	m_cDropRateEdit.SetWindowText(strBuffer);

	for( lComboCounter=0; lComboCounter<m_cDropTemplateCombo.GetCount(); lComboCounter++ )
	{
		if( m_cDropTemplateCombo.GetItemData(lComboCounter) == eCategory )
		{
			m_cDropTemplateCombo.SetCurSel( lComboCounter );
			break;
		}
	}

	if( eCategory == AGPMDROPITEM_EQUIP )
	{
		AgpdDropItemEquipTemplate			*pcsAgpdDropItemEquipTemplate;

		pcsAgpdDropItemEquipTemplate = m_pcsAgpmDropItem->m_aEquipItemTemplate.GetDropItemEquipTemplate(m_pcsAgpdDropItemTemplate->m_lEquip);

		if( pcsAgpdDropItemEquipTemplate )
		{
			int				lRow;

			m_cDropInfoList.InsertColumn( 1, "종족", LVCFMT_LEFT, 50 );
			m_cDropInfoList.InsertColumn( 2, "클래스", LVCFMT_LEFT, 50 );
			m_cDropInfoList.InsertColumn( 3, "아이템이름", LVCFMT_LEFT, 150 );
			m_cDropInfoList.InsertColumn( 4, "드랍확률", LVCFMT_LEFT, 100 );

			for( int lClass=0; lClass<AGPMDROPITEM_EQUIP_MAX_RACE; lClass++ )
			{
				if( pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_lRace != 0 )
				{
					for( int lCounter=0; lCounter<pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_lItemCount; lCounter++ )
					{
						lRow = lCounter+lClass*pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_lItemCount;

						m_cDropInfoList.InsertItem(lRow, "N/A" );

						if( pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_alTID[lCounter] != 0 )
						{
							pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_alTID[lCounter] );

							if( pcsItemTemplate )
							{
								m_cDropInfoList.SetItemText( lRow, 2, pcsItemTemplate->m_szName );

								sprintf( strBuffer, "%d", pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_lRace );
								m_cDropInfoList.SetItemText( lRow, 0, strBuffer );

								sprintf( strBuffer, "%d", pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_lClass );
								m_cDropInfoList.SetItemText( lRow, 1, strBuffer );

								sprintf( strBuffer, "%d", pcsAgpdDropItemEquipTemplate->m_csAgpdDropEquipData[lClass].m_alScalar[lCounter] );
								m_cDropInfoList.SetItemText( lRow, 3, strBuffer );
							}
						}
					}
				}
			}
		}
	}
	else
	{
		m_cDropInfoList.InsertColumn( 1, "아이템이름", LVCFMT_LEFT, 200 );
		m_cDropInfoList.InsertColumn( 2, "드랍확률", LVCFMT_LEFT, 100 );

		for( int lCounter=0; lCounter<m_pcsAgpdDropItemTemplate->m_cDropCategory[eCategory].m_lItemCount; lCounter++ )
		{
			m_cDropInfoList.InsertItem(lCounter, "N/A" );

			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( m_pcsAgpdDropItemTemplate->m_cDropCategory[eCategory].m_alTID[lCounter] );

			if( pcsItemTemplate )
			{
				char			strBuffer[80];

				//ItemName
				m_cDropInfoList.SetItemText( lCounter, 0, pcsItemTemplate->m_szName );
				m_cDropInfoList.SetItemData( lCounter, pcsItemTemplate->m_lID );

				//DropRate
				sprintf( strBuffer, "%d", m_pcsAgpdDropItemTemplate->m_cDropCategory[eCategory].m_alScalar[lCounter] );
				m_cDropInfoList.SetItemText( lCounter, 1, strBuffer );
			}
		}
	}
}

BOOL AgcmDropTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	INT32				lIndex;

	//Category Combo초기화
	m_cDropTemplateCombo.ResetContent();

	lIndex = m_cDropTemplateCombo.AddString( "장비형" );
	m_cDropTemplateCombo.SetItemData( lIndex, AGPMDROPITEM_EQUIP);

	lIndex = m_cDropTemplateCombo.AddString( "소비형" );
	m_cDropTemplateCombo.SetItemData( lIndex, AGPMDROPITEM_CONSUME);

	lIndex = m_cDropTemplateCombo.AddString( "귀중품" );
	m_cDropTemplateCombo.SetItemData( lIndex, AGPMDROPITEM_VALUABLE);

	lIndex = m_cDropTemplateCombo.AddString( "겔드" );
	m_cDropTemplateCombo.SetItemData( lIndex, AGPMDROPITEM_GHELLD);

	lIndex = m_cDropTemplateCombo.AddString( "생산재" );
	m_cDropTemplateCombo.SetItemData( lIndex, AGPMDROPITEM_PRODUCT);

	//ListCtrl 초기화
	ShowCategory( AGPMDROPITEM_EQUIP );

	//ListBox 초기화
	AgpaItemTemplate		*pcsAgpaItemTemplate;
	AgpdItemTemplate		**ppcsItemTemplate;

	char			strBuffer[255];
	int				iIndex;

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
			int				iIndex;

			sprintf( strBuffer, "%s, ID:%d", (*ppcsItemTemplate)->m_szName, (*ppcsItemTemplate)->m_lID );

			iIndex = m_cDropItemListCombo.AddString( strBuffer );
			m_cDropItemListCombo.SetItemData( iIndex, (*ppcsItemTemplate)->m_lID );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmDropTemplateDlg::OnSelchangeComboDropCategory() 
{
	// TODO: Add your control notification handler code here
	int				lIndex;

	lIndex = m_cDropTemplateCombo.GetCurSel();

	if( lIndex != CB_ERR )
	{
		ShowCategory( (eAgpmDropItemCategory)m_cDropTemplateCombo.GetItemData(lIndex) );
	}
}
