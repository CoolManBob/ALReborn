// ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ExportDlg.h"
#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog


CExportDlg::CExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportDlg::IDD, pParent)
	, m_bMinimapExport(TRUE)
	, m_bServerDataExport( TRUE )
{
	//{{AFX_DATA_INIT(CExportDlg)
	m_bCompactData	= TRUE;
	m_bMapDetail	= TRUE;
	m_bObject		= TRUE;
	m_bMapRough		= TRUE;
	m_bTile			= TRUE;
	//}}AFX_DATA_INIT

	m_nCameraType	= g_MyEngine.SC_MINIMAP;
}


void CExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDlg)
	DDX_Check(pDX, IDC_COMPACTEXPORT, m_bCompactData);
	DDX_Check(pDX, IDC_DETAILEXPORT, m_bMapDetail);
	DDX_Check(pDX, IDC_OBJECT_EXPORT, m_bObject);
	DDX_Check(pDX, IDC_ROUGHEXPORT, m_bMapRough);
	DDX_Check(pDX, IDC_TILEEXPORT, m_bTile);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_MINIMAP_EXPORT, m_bMinimapExport);
	DDX_Check(pDX, IDC_SERVERDATA_EXPORT, m_bServerDataExport);
}


BEGIN_MESSAGE_MAP(CExportDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDlg message handlers
