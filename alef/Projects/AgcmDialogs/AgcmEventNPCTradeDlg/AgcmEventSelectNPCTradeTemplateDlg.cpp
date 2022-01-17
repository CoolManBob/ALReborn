// AgcmEventSelectNPCTradeTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventnpctradedlg.h"
#include "AgcmEventSelectNPCTradeTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSelectNPCTradeTemplateDlg dialog


AgcmEventSelectNPCTradeTemplateDlg::AgcmEventSelectNPCTradeTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEventSelectNPCTradeTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEventSelectNPCTradeTemplateDlg)
	//}}AFX_DATA_INIT
}


void AgcmEventSelectNPCTradeTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEventSelectNPCTradeTemplateDlg)
	DDX_Control(pDX, IDC_NPCTradeTemplateCombo, m_cNPCTradeTemplateCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEventSelectNPCTradeTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEventSelectNPCTradeTemplateDlg)
	ON_BN_CLICKED(IDC_NPCTradeTemplateSelectButton, OnNPCTradeTemplateSelectButton)
	ON_BN_CLICKED(IDC_NPCTradeTemplateOKButton, OnNPCTradeTemplateOKButton)
	ON_BN_CLICKED(IDC_NPCTradeTemplateCancelButton, OnNPCTradeTemplateCancelButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSelectNPCTradeTemplateDlg message handlers

void AgcmEventSelectNPCTradeTemplateDlg::InitData( ApmObject *pcsApmObject, AgpmCharacter *pcsAgpmCharacter, AgpmEventNPCTrade *pcsAgpmEventNPCTrade, AgpdEventNPCTradeData *pcsEventData )
{
	m_pcsApmObject = pcsApmObject;
	m_pcsAgpmCharacter = pcsAgpmCharacter;
	m_pcsAgpmEventNPCTrade = pcsAgpmEventNPCTrade;
	m_pcsNPCDataData = pcsEventData;
}

BOOL AgcmEventSelectNPCTradeTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if( m_pcsAgpmEventNPCTrade != NULL )
	{
		//Combo박스에 넣어준다.
		ApAdmin				*pcsAdmin;

		pcsAdmin = m_pcsAgpmEventNPCTrade->GetTemplate();

		if( pcsAdmin != NULL )
		{
			AgpdEventNPCTradeTemplate	**ppcsTemplate;

			INT32			iIndex = 0;

			m_cNPCTradeTemplateCombo.AddString( "___N/A___" );

			while( 1 ) 
			{
				ppcsTemplate = (AgpdEventNPCTradeTemplate **)pcsAdmin->GetObjectSequence( &iIndex );

				if( ppcsTemplate == NULL )
				{
					break;
				}
				else
				{
					AgpdCharacterTemplate			*pcsTemplate;

					INT32			lComboIndex;

					pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( (*ppcsTemplate)->m_lNPCTID );

					if( pcsTemplate )
					{
						lComboIndex = m_cNPCTradeTemplateCombo.AddString( pcsTemplate->m_szTName );

						if( lComboIndex != CB_ERR )
						{
							m_cNPCTradeTemplateCombo.SetItemData( lComboIndex, pcsTemplate->m_lID );
						}
					}

/*					ApdObjectTemplate		*pcsApdObjectTemplate;

					INT32			lComboIndex;

					pcsApdObjectTemplate  = m_pcsApmObject->GetObjectTemplate( (*ppcsTemplate)->m_lNPCTID );

					if( pcsApdObjectTemplate != NULL )
					{
						lComboIndex = m_cNPCTradeTemplateCombo.AddString( pcsApdObjectTemplate->m_szName );

						if( lComboIndex != CB_ERR )
						{
							m_cNPCTradeTemplateCombo.SetItemData( lComboIndex, pcsApdObjectTemplate->m_lID );
						}
					}*/
				}
			}

			if( m_pcsNPCDataData->m_lNPCTradeTemplateID )
			{
				ApdObjectTemplate		*pcsApdObjectTemplate;
				INT32			lResult;

				pcsApdObjectTemplate = m_pcsApmObject->GetObjectTemplate( m_pcsNPCDataData->m_lNPCTradeTemplateID );

				if( pcsApdObjectTemplate != NULL )
				{
					lResult = m_cNPCTradeTemplateCombo.FindStringExact( -1, pcsApdObjectTemplate->m_szName );

					m_cNPCTradeTemplateCombo.SetCurSel( lResult );
				}
			}
			else
			{
				m_cNPCTradeTemplateCombo.SetCurSel( 0 );
			}
		}

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmEventSelectNPCTradeTemplateDlg::OnNPCTradeTemplateSelectButton() 
{
	// TODO: Add your control notification handler code here
	INT32			lCurSel;

	lCurSel = m_cNPCTradeTemplateCombo.GetCurSel();

	if( lCurSel != CB_ERR )
	{
		INT32			lCurSel;
		
		lCurSel = m_cNPCTradeTemplateCombo.GetCurSel();

		if( lCurSel != CB_ERR )
		{
			INT32			lData;

			lData = m_cNPCTradeTemplateCombo.GetItemData( lCurSel );

			m_pcsNPCDataData->m_lNPCTradeTemplateID = lData;
		}
	}
}

void AgcmEventSelectNPCTradeTemplateDlg::OnNPCTradeTemplateCancelButton() 
{
	// TODO: Add your control notification handler code here
	m_pcsNPCDataData->m_lNPCTradeTemplateID = 0;
	
	m_cNPCTradeTemplateCombo.SetCurSel( 0 );
}

void AgcmEventSelectNPCTradeTemplateDlg::OnNPCTradeTemplateOKButton() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL AgcmEventSelectNPCTradeTemplateDlg::PreTranslateMessage(MSG* pMsg) 
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
