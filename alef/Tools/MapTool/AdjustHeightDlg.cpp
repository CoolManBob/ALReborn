// AdjustHeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "AdjustHeightDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdjustHeightDlg dialog


CAdjustHeightDlg::CAdjustHeightDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustHeightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdjustHeightDlg)
	m_strValue = _T("");
	//}}AFX_DATA_INIT
}


void CAdjustHeightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdjustHeightDlg)
	DDX_Text(pDX, IDC_VALUE, m_strValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdjustHeightDlg, CDialog)
	//{{AFX_MSG_MAP(CAdjustHeightDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdjustHeightDlg message handlers

void CAdjustHeightDlg::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData( TRUE );
	m_strValue.MakeUpper();

	if( m_strValue[0] == 'X' )
	{
		// 배율 곱하기..
		m_nType		= ADJUST_HEIGHT;
		m_fValue	= ( float ) atof( (LPCTSTR) m_strValue + 1 );
	}
	else if( m_strValue[0] == '+' )
	{
		// 수치 대입하기..
		m_nType		= ADD_HEIGHT;
		m_fValue	= ( float ) atof( (LPCTSTR) m_strValue ) * 100.0f;
	}
	else if( m_strValue[0] == '-' )
	{
		// 수치 대입하기..
		m_nType		= ADD_HEIGHT;
		m_fValue	= ( float ) -atof( (LPCTSTR) m_strValue ) * 100.0f;
	}
	else 
	{
		// 수치 대입하기..
		m_nType		= INSERT_HEIGHT;
		m_fValue	= ( float ) atof( (LPCTSTR) m_strValue ) * 100.0f;
	}
	
	CDialog::OnOK();
}
