// KeyReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "KeyReferenceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyReferenceDlg dialog


CKeyReferenceDlg::CKeyReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyReferenceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyReferenceDlg)
	m_strKeyRef = _T("");
	//}}AFX_DATA_INIT
}


void CKeyReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyReferenceDlg)
	DDX_Text(pDX, IDC_REF, m_strKeyRef);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyReferenceDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyReferenceDlg message handlers

BOOL CKeyReferenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char	filename[ 1024 ];
	char	buffer[ 1024 ];
	char	*ptext;
	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , "readme.txt" );
	FILE *pFile = fopen( filename , "rt" );
	if( pFile )
	{
		do
		{
			ptext = fgets( buffer , 1024 , pFile );
			m_strKeyRef += ptext;
		} while ( ptext );

		fclose( pFile );
	}

	m_strKeyRef.Replace( "\n" , "\r\n" );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
