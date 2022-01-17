// EditFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "EditFaceDlg.h"
#include "OpenFilesDlg.h"

// CEditFaceDlg dialog

IMPLEMENT_DYNAMIC(CEditFaceDlg, CDialog)
CEditFaceDlg::CEditFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditFaceDlg::IDD, pParent)
{
}

CEditFaceDlg::~CEditFaceDlg()
{
}

void CEditFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FACE_LISTBOX, m_ctlFaceList);
}

BEGIN_MESSAGE_MAP(CEditFaceDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_EDIT_FACE_ADD, OnBnClickedEditFaceAdd)
	ON_BN_CLICKED(IDC_EDIT_FACE_DELETE, OnBnClickedEditFaceDelete)
	ON_BN_CLICKED(IDC_EDIT_FACE_DELETE_ALL, OnBnClickedEditFaceDeleteAll)
END_MESSAGE_MAP()

BOOL CEditFaceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for( int i = 0; i < (int)m_vstrDefaultFace.size(); i++ )
	{
		m_ctlFaceList.AddString( m_vstrDefaultFace[i].c_str() );
	}

	m_vstrDefaultFace.clear();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEditFaceDlg::OnBnClickedOk()
{
	char szBuffer[256];

	m_vstrDefaultFace.clear();
	int nNum = m_ctlFaceList.GetCount();
	for( int i = 0; i < nNum; i++ )
	{
		m_ctlFaceList.GetText( i, szBuffer );
		m_vstrDefaultFace.push_back( szBuffer );
	}

//	sprintf(szBuffer, "H_Kni_M_6th_face_b.rws");
//	m_vstrDefaultFace.push_back( szBuffer );

	OnOK();
}

void CEditFaceDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CEditFaceDlg::OnBnClickedEditFaceAdd()
{
	COpenFilesDlg OpenFileDlg;
	OpenFileDlg.SetRootName( "FACE" );
	OpenFileDlg.SetDirectory( "\\CHARACTER\\DefaultHead\\Face\\" );
	if( OpenFileDlg.DoModal() != IDOK )
		return;

	char szBuffer[256];
	for( int n = 0; n < OpenFileDlg.GetFileNum(); n++ ) 
	{
		bool bAdd = true;
		int nNum = m_ctlFaceList.GetCount();
		for( int i = 0; i < nNum; i++ )
		{
			m_ctlFaceList.GetText( i, szBuffer );
			if( strcmp( OpenFileDlg.GetFileName( n ), szBuffer ) == 0 ) {
				bAdd = false;
				break;
			}
		}

		if( bAdd ) {
			m_ctlFaceList.AddString( OpenFileDlg.GetFileName( n ) );
		}
	}
}

void CEditFaceDlg::OnBnClickedEditFaceDelete()
{
	int nSelect = m_ctlFaceList.GetCurSel();
	if( nSelect == -1 )
		return;

	m_ctlFaceList.DeleteString( nSelect );
}

void CEditFaceDlg::OnBnClickedEditFaceDeleteAll()
{
	m_vstrDefaultFace.clear();
	m_ctlFaceList.ResetContent();
}

void CEditFaceDlg::AddDefaultFace( const char* pFileName )
{
	m_vstrDefaultFace.push_back( pFileName );
}

int CEditFaceDlg::GetDefaultFaceNum() const
{
	return (int)m_vstrDefaultFace.size();
}

const char* CEditFaceDlg::GetDefaultFaceNth( int nIdx )
{
	return m_vstrDefaultFace[nIdx].c_str();
}
