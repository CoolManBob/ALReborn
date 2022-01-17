// PositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "PositionDlg.h"


// CPositionDlg dialog

IMPLEMENT_DYNAMIC(CPositionDlg, CDialog)

CPositionDlg::CPositionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPositionDlg::IDD, pParent)
	, m_fX(0)
	, m_fY(0)
	, m_fZ(0)
	, m_fDegreeX(0)
	, m_fDegreeY(0)
{

}

CPositionDlg::~CPositionDlg()
{
}

void CPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_POSITION_X, m_fX);
	DDX_Text(pDX, IDC_POSITION_Y, m_fY);
	DDX_Text(pDX, IDC_POSITION_Z, m_fZ);
	DDX_Text(pDX, IDC_ROTATE_DEGREEX, m_fDegreeX);
	DDX_Text(pDX, IDC_ROTATE_DEGREEY, m_fDegreeY);
}

BEGIN_MESSAGE_MAP(CPositionDlg, CDialog)
END_MESSAGE_MAP()


// CPositionDlg message handlers
