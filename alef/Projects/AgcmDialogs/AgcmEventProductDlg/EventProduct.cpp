// EventProduct.cpp : implementation file
//
#include "stdafx.h"
#include "AgcmEventProductDlg.h"
#include "EventProduct.h"
#include "AgpmEventProduct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEventProduct dialog

INT32 CEventProduct::s_lCategory[EVENT_PRODUCT_CATEGORY_MAX] =
	{
		eAGPMPRODUCT_CATEGORY::AGPMPRODUCT_CATEGORY_HUNT,
		eAGPMPRODUCT_CATEGORY::AGPMPRODUCT_CATEGORY_COOK,
		eAGPMPRODUCT_CATEGORY::AGPMPRODUCT_CATEGORY_ALCHEMY
	};



CEventProduct::CEventProduct(CWnd* pParent /*=NULL*/)
	: CDialog(CEventProduct::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventProduct)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEventProduct::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventProduct)
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_Combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventProduct, CDialog)
	//{{AFX_MSG_MAP(CEventProduct)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventProduct message handlers

void CEventProduct::OnOK() 
{
	// TODO: Add extra validation here
	// TODO: Add extra validation here
	AgpdEventProduct *pAgpdEventProduct  = (AgpdEventProduct *) m_pApdEvent->m_pvData;

	int nSel = m_Combo.GetCurSel();
	if (nSel < 0 || nSel > EVENT_PRODUCT_CATEGORY_MAX)
		pAgpdEventProduct->m_eCategory = AGPMPRODUCT_CATEGORY_NONE;
	else
		pAgpdEventProduct->m_eCategory = s_lCategory[nSel];

	CDialog::OnOK();
}

BOOL CEventProduct::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AgpdEventProduct *pAgpdEventProduct  = (AgpdEventProduct *) m_pApdEvent->m_pvData;
	int nSel = -1;

	//for (int i=0; i<EVENT_PRODUCT_CATEGORY_MAX; i++)
	for (int i = 0; i < eAGPMPRODUCT_CATEGORY::AGPMPRODUCT_CATEGORY_MAX; ++i)
		{
		// add category string to combo box
		//m_Combo.InsertString(i, s_szCategory[i]);
		m_Combo.InsertString(i, g_szProductCategory[i]);

		// find category of event embedded
		if (s_lCategory[i] == pAgpdEventProduct->m_eCategory)
			nSel = i;
		}

	m_Combo.SetCurSel(nSel);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
