// AgcmEventNPCDialogSelectTemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeventnpcdialogdlg.h"
#include "AgcmEventNPCDialogSelectTemplateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEventNPCDialogSelectTemplateDlg dialog


AgcmEventNPCDialogSelectTemplateDlg::AgcmEventNPCDialogSelectTemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEventNPCDialogSelectTemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEventNPCDialogSelectTemplateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmEventNPCDialogSelectTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEventNPCDialogSelectTemplateDlg)
	DDX_Control(pDX, IDC_DialogList, m_cDialogList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEventNPCDialogSelectTemplateDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEventNPCDialogSelectTemplateDlg)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_DialogSelect, OnDialogSelect)
	ON_BN_CLICKED(IDC_DialogView, OnDialogView)
	ON_BN_CLICKED(IDC_DialogReset, OnDialogReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEventNPCDialogSelectTemplateDlg message handlers

void AgcmEventNPCDialogSelectTemplateDlg::InitData( ApmObject *pcsApmObject, AgpmEventNPCDialog *pcsAgpmEventNPCDialog, AgpdEventNPCDialogData *pcsEventData )
{
	m_pcsApmObject = pcsApmObject;
	m_pcsAgpmEventNPCDialog = pcsAgpmEventNPCDialog;
	m_pcsNPCDialogData = pcsEventData;
}

BOOL AgcmEventNPCDialogSelectTemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ApAdmin			*pcsNPCDialogAdmin;
	AgpdEventNPCDialogTemplate	**ppcsTemplate = NULL;

	INT32			lIndex = 0;
	INT32			lDialogIndex;
	char			strBuffer[1024];

	pcsNPCDialogAdmin = m_pcsAgpmEventNPCDialog->GetNPCTemplate();

	// 등록된 모든 NPCTrade Template에 대해서...
	for( ppcsTemplate = (AgpdEventNPCDialogTemplate **) pcsNPCDialogAdmin->GetObjectSequence(&lIndex); ppcsTemplate; ppcsTemplate = (AgpdEventNPCDialogTemplate **) pcsNPCDialogAdmin->GetObjectSequence(&lIndex))
	{
		if( (*ppcsTemplate) != NULL )
		{
			if( (*ppcsTemplate)->m_iDialogTextNum >= 1 )
			{
				sprintf( strBuffer, "%d, %s", (*ppcsTemplate)->m_lNPCDialogTID, (*ppcsTemplate)->m_ppstrDialogText[0] );

				lDialogIndex = m_cDialogList.AddString( strBuffer );
				m_cDialogList.SetItemData( lDialogIndex, (*ppcsTemplate)->m_lNPCDialogTID );
			}
			else
			{
				sprintf( strBuffer, "%d, %s", (*ppcsTemplate)->m_lNPCDialogTID, "지정된 텍스트 없음" );

				lDialogIndex = m_cDialogList.AddString( strBuffer );
				m_cDialogList.SetItemData( lDialogIndex, (*ppcsTemplate)->m_lNPCDialogTID );
			}
		}
	}

	for( int iCounter=0; iCounter<m_cDialogList.GetCount(); iCounter++ )
	{
		if( m_cDialogList.GetItemData( iCounter ) == m_pcsNPCDialogData->m_lNPCDialogTextID )
		{
			m_cDialogList.SetCurSel( iCounter );
			break;
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmEventNPCDialogSelectTemplateDlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	OnOK();	
}

void AgcmEventNPCDialogSelectTemplateDlg::OnDialogSelect() 
{
	// TODO: Add your control notification handler code here
	if( m_pcsNPCDialogData != NULL )
	{
		int				lIndex;

		lIndex = m_cDialogList.GetCurSel();

		if( lIndex != LB_ERR )
		{
			int				lTID;

			lTID = m_cDialogList.GetItemData(lIndex );
			m_pcsNPCDialogData->m_lNPCDialogTextID = lTID;
		}
	}
}

void AgcmEventNPCDialogSelectTemplateDlg::OnDialogView() 
{
	// TODO: Add your control notification handler code here
	int				lIndex;
	int				lTID;

	lIndex = m_cDialogList.GetCurSel();

	if( lIndex != LB_ERR )
	{
		AgpdEventNPCDialogTemplate		*pcsTemplate;

		lTID = m_cDialogList.GetItemData( lIndex );

		pcsTemplate = m_pcsAgpmEventNPCDialog->GetNPCDialogTemplate( lTID );

		if( pcsTemplate )
		{
			char			strBuffer[9999];

			memset( strBuffer, 0, sizeof(strBuffer) );

			for( int iCounter=0; iCounter<pcsTemplate->m_iDialogTextNum; iCounter++ )
			{
				strcat( strBuffer, pcsTemplate->m_ppstrDialogText[iCounter] );
				strcat( strBuffer, "\n" );
			}

			MessageBox( strBuffer );
		}
	}
}

void AgcmEventNPCDialogSelectTemplateDlg::OnDialogReset() 
{
	// TODO: Add your control notification handler code here
	if( m_pcsNPCDialogData != NULL )
	{
		m_pcsNPCDialogData->m_lNPCDialogTextID = 0;
	}
}
