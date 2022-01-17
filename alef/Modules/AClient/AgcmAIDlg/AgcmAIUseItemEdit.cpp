// AgcmAIUseItemEdit.cpp : implementation file
//

#include "stdafx.h"
#include "agcmaidlg.h"
#include "AgcmAIUseItemEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseItemEdit dialog


AgcmAIUseItemEdit::AgcmAIUseItemEdit(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAIUseItemEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAIUseItemEdit)
	//}}AFX_DATA_INIT
}


void AgcmAIUseItemEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAIUseItemEdit)
	DDX_Control(pDX, IDC_AI_USE_ITEM_HP_CON_EDIT, m_cHPConEdit);
	DDX_Control(pDX, IDC_AI_USE_ITEM_RATE_EDIT, m_cRateEdit);
	DDX_Control(pDX, IDC_AI_USE_ITEM_LIST, m_ItemList);
	DDX_Control(pDX, IDC_AI_ITEM_USE_COMBO, m_cItemCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAIUseItemEdit, CDialog)
	//{{AFX_MSG_MAP(AgcmAIUseItemEdit)
	ON_BN_CLICKED(IDC_AI_USE_ITEM_OK_BUTTON, OnAiUseItemOkButton)
	ON_BN_CLICKED(IDC_AI_USE_ITEM_CANCEL_BUTTON, OnAiUseItemCancelButton)
	ON_BN_CLICKED(IDC_AI_USE_ITEM_ADD_BUTTON, OnAiUseItemAddButton)
	ON_BN_CLICKED(IDC_AI_USE_ITEM_REMOVE_BUTTON, OnAiUseItemRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAIUseItemEdit message handlers

BOOL AgcmAIUseItemEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ItemList.InsertColumn( 0, "TID", LVCFMT_LEFT, 50, 0 );
	m_ItemList.InsertColumn( 1, "Name", LVCFMT_LEFT, 150, 0 );
	m_ItemList.InsertColumn( 2, "Rate", LVCFMT_LEFT, 50, 1 );

	AgpaItemTemplate		*pcsAgpaItemTemplate;
	AgpdItemTemplate		**ppcsItemTemplate;
	AgpdItemTemplate		*pcsItemTemplate;
	
	char			strUseItemData[80];
	int				iIndex;
	int				iCounter;

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
			m_cItemCombo.AddString( (*ppcsItemTemplate)->m_szName );
		}
	}

	itoa( m_pstTemplate->m_stAI.m_csAIUseItem.m_lHP, strUseItemData, 10 );
	m_cHPConEdit.SetWindowText( strUseItemData );

	for( iCounter=0; iCounter<AGPDAI_MAX_USABLE_ITEM_COUNT; iCounter++ )
	{
		if( (m_pstTemplate->m_stAI.m_csAIUseItem.m_alTID[iCounter] != 0) && (m_pstTemplate->m_stAI.m_csAIUseItem.m_alRate[iCounter] != 0) )
		{
			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( m_pstTemplate->m_stAI.m_csAIUseItem.m_alTID[iCounter] );

			if( pcsItemTemplate != NULL )
			{
				itoa( m_pstTemplate->m_stAI.m_csAIUseItem.m_alTID[iCounter], strUseItemData, 10 );
				m_ItemList.InsertItem( 0, strUseItemData );

				m_ItemList.SetItemText( 0, 1, pcsItemTemplate->m_szName );

				itoa( m_pstTemplate->m_stAI.m_csAIUseItem.m_alRate[iCounter], strUseItemData, 10 );
				m_ItemList.SetItemText( 0, 2, strUseItemData );
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AgcmAIUseItemEdit::AddItemToComboBox( char *pstrItemName, long lTID )
{
	m_cItemCombo.AddString( pstrItemName );

	return TRUE;
}

void AgcmAIUseItemEdit::OnAiUseItemOkButton() 
{
	// TODO: Add your control notification handler code here
	char			strHP[80];

	m_cHPConEdit.GetWindowText( strHP, sizeof(strHP) );

	if( strlen( strHP ) != 0 )
	{
		char		strTID[80];
		char		strRate[80];

		int			lCounter;
		int			lCurrentIndex;
		int			lNextIndex;

		lCurrentIndex = 0;

		m_pstTemplate->m_stAI.m_csAIUseItem.m_lHP = atoi( strHP );

		while( 1 )
		{
			lNextIndex = m_ItemList.GetNextItem( lCurrentIndex, LVNI_ALL );

			if( lCurrentIndex == -1 )
				break;

			m_ItemList.GetItemText( lCurrentIndex, 0, strTID, sizeof(strTID) );
//			m_ItemList.GetItemText( lCurrentIndex, 1, strName, sizeof(strName) );
			m_ItemList.GetItemText( lCurrentIndex, 2, strRate, sizeof(strRate) );

			for( lCounter=0; lCounter<AGPDAI_MAX_USABLE_ITEM_COUNT; lCounter++ )
			{
				if( m_pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lCounter] == 0 )
				{
					break;
				}
			}

			m_pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lCounter] = atoi( strTID );
			m_pstTemplate->m_stAI.m_csAIUseItem.m_alRate[lCounter] = atoi(strRate );

			lCurrentIndex = lNextIndex;
		}
	}

	OnOK();
}

void AgcmAIUseItemEdit::OnAiUseItemCancelButton() 
{
	// TODO: Add your control notification handler code here
	OnCancel();	
}

void AgcmAIUseItemEdit::OnAiUseItemAddButton() 
{
	// TODO: Add your control notification handler code here
	//현재 선택된 아이템을 얻어낸다.
	char		strItemName[255];
	char		strRate[80];
	int			iSelCur;

	iSelCur = m_cItemCombo.GetCurSel();

	if( iSelCur != CB_ERR )
	{
		if( m_cItemCombo.GetLBText( iSelCur, strItemName ) != CB_ERR )
		{
			if( strlen( strItemName ) )
			{
				m_cRateEdit.GetWindowText( strRate, sizeof( strRate) );

				if( strlen(strRate) )
				{
					AgpdItemTemplate		*pcsItemTempate;
					char				strTID[80];

					pcsItemTempate = m_pcsAgpmItem->GetItemTemplate( strItemName );
					sprintf( strTID, "%d", pcsItemTempate->m_lID );

					m_ItemList.InsertItem( 0, strTID );
					m_ItemList.SetItemText( 0, 1, strItemName );
					m_ItemList.SetItemText( 0, 2, strRate );
				}
			}
		}
	}
}

void AgcmAIUseItemEdit::OnAiUseItemRemoveButton() 
{
	// TODO: Add your control notification handler code here
	int			iSelect;

	iSelect = m_ItemList.GetSelectionMark();
	m_ItemList.DeleteItem( iSelect );
	
}
