// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "OptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionDialog dialog


COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
	, m_bShowObjectBoundingSphere(FALSE)
	, m_bShowObjectCollisionAtomic(FALSE)
	, m_bShowObjectPickingAtomic(FALSE)
	, m_bShowObjectRealPolygon(FALSE)
	, m_bShowOctreeInfo(FALSE)
{
	//{{AFX_DATA_INIT(COptionDialog)
	m_bUsePolygonLock = FALSE;
	m_bUseBrushPolygon = FALSE;
	m_bCameraBlocking = FALSE;
	m_bShadow2 = FALSE;
	//}}AFX_DATA_INIT
}


void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDialog)
	DDX_Check(pDX, IDC_USEPOLYGONLOCK, m_bUsePolygonLock);
	DDX_Check(pDX, IDC_USEBRUSHPOLYGON, m_bUseBrushPolygon);
	DDX_Check(pDX, IDC_CAMERABLOCKING, m_bCameraBlocking);
	DDX_Check(pDX, IDC_SHADOW2, m_bShadow2);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_OBJECTBOUNDINGSPHERE, m_bShowObjectBoundingSphere);
	DDX_Check(pDX, IDC_SHOWOBJECTCOLLISIONATOMIC, m_bShowObjectCollisionAtomic);
	DDX_Check(pDX, IDC_SHOWOBJECTPICKING, m_bShowObjectPickingAtomic);
	DDX_Check(pDX, IDC_SHOWOBJECTREALPOLYGONINFO, m_bShowObjectRealPolygon);
	DDX_Check(pDX, IDC_OCTREEINFO, m_bShowOctreeInfo);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	//{{AFX_MSG_MAP(COptionDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionDialog message handlers
