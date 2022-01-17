// RoughMapGenerateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "RoughMapGenerateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoughMapGenerateDlg dialog


CRoughMapGenerateDlg::CRoughMapGenerateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoughMapGenerateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoughMapGenerateDlg)
	m_fOffset				= 0.0f	;
	m_bHeightGenerate		= TRUE	;
	m_bTileGenerate			= TRUE	;
	m_bVertexColorGenerate	= TRUE	;
	m_bApplyAlpha			= FALSE	;
	//}}AFX_DATA_INIT
}


void CRoughMapGenerateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoughMapGenerateDlg)
	DDX_Text(pDX, IDC_OFFSET, m_fOffset);
	DDX_Check(pDX, IDC_HEIGHT_GENERATE, m_bHeightGenerate);
	DDX_Check(pDX, IDC_TILE_GENERATE, m_bTileGenerate);
	DDX_Check(pDX, IDC_VERTEXCOLOR_GENERATE, m_bVertexColorGenerate);
	DDX_Check(pDX, IDC_APPLYALPHA, m_bApplyAlpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoughMapGenerateDlg, CDialog)
	//{{AFX_MSG_MAP(CRoughMapGenerateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoughMapGenerateDlg message handlers
