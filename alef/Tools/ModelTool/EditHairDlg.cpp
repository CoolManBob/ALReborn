// EditHairDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "EditHairDlg.h"
#include "OpenFilesDlg.h"

IMPLEMENT_DYNAMIC(CEditHairDlg, CDialog)
CEditHairDlg::CEditHairDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditHairDlg::IDD, pParent)
{
}

CEditHairDlg::~CEditHairDlg()
{
}

void CEditHairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HAIR_LISTBOX, m_ctlHairList);
}


BEGIN_MESSAGE_MAP(CEditHairDlg, CDialog)
	ON_BN_CLICKED(IDC_EDIT_HAIR_ADD, OnBnClickedEditHairAdd)
	ON_BN_CLICKED(IDC_EDIT_HAIR_DELETE, OnBnClickedEditHairDelete)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_EDIT_HAIR_DELETE_ALL, OnBnClickedEditHairDeleteAll)
END_MESSAGE_MAP()


BOOL CEditHairDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	for( int i = 0; i < (int)m_vstrDefaultHair.size(); i++ )
	{
		m_ctlHairList.AddString( m_vstrDefaultHair[i].c_str() );
	}

	m_vstrDefaultHair.clear();
	return TRUE;
}

// CEditHairDlg message handlers
void CEditHairDlg::AddDefaultHair( const char* pFileName )
{
	m_vstrDefaultHair.push_back( pFileName );
}

int CEditHairDlg::GetDefaultHairNum() const
{
	return (int)m_vstrDefaultHair.size();
}

const char* CEditHairDlg::GetDefaultHairNth( int nIdx )
{
	return m_vstrDefaultHair[nIdx].c_str();
}

void CEditHairDlg::OnBnClickedEditHairAdd()
{
	COpenFilesDlg OpenFileDlg;
	OpenFileDlg.SetRootName( "Hair" );
	OpenFileDlg.SetDirectory( "\\CHARACTER\\DefaultHead\\Hair\\" );
	if( OpenFileDlg.DoModal() != IDOK )
		return;

	char szBuffer[256];

	for( int n = 0; n < OpenFileDlg.GetFileNum(); n++ ) 
	{
		bool bAdd = true;
		int nNum = m_ctlHairList.GetCount();
		const char * pFileName = OpenFileDlg.GetFileName( n );
		for( int i = 0; i < nNum; i++ )
		{
			m_ctlHairList.GetText( i, szBuffer );
			if( strcmp( pFileName , szBuffer ) == 0 ) {
				bAdd = false;
				break;
			}
		}

		if( bAdd ) {
			m_ctlHairList.AddString( pFileName );
		}
	}
}

void CEditHairDlg::OnBnClickedEditHairDelete()
{
	int nSelect = m_ctlHairList.GetCurSel();
	if( nSelect == -1 )
		return;

	m_ctlHairList.DeleteString( nSelect );
}

void CEditHairDlg::OnBnClickedOk()
{
	char szBuffer[256];

	m_vstrDefaultHair.clear();
	int nNum = m_ctlHairList.GetCount();
	for( int i = 0; i < nNum; i++ )
	{
		m_ctlHairList.GetText( i, szBuffer );
		m_vstrDefaultHair.push_back( szBuffer );
	}
	OnOK();
}

void CEditHairDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CEditHairDlg::OnBnClickedEditHairDeleteAll()
{
	m_vstrDefaultHair.clear();
	m_ctlHairList.ResetContent();
}
