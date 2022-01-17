// AgcmLODDistanceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmloddlg.h"
#include "AgcmLODDistanceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmLODDistanceDlg dialog


AgcmLODDistanceDlg::AgcmLODDistanceDlg(UINT32 *pulDistance, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmLODDistanceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmLODDistanceDlg)
	m_dwDistance0 = pulDistance[0];
	m_dwDistance1 = pulDistance[1];
	m_dwDistance2 = pulDistance[2];
	m_dwDistance3 = pulDistance[3];
	m_dwDistance4 = pulDistance[4];
	//}}AFX_DATA_INIT

	m_pulDistance = pulDistance;
}


void AgcmLODDistanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmLODDistanceDlg)
	DDX_Text(pDX, IDC_EDIT_DISTANCE_0, m_dwDistance0);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_1, m_dwDistance1);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_2, m_dwDistance2);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_3, m_dwDistance3);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_4, m_dwDistance4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmLODDistanceDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmLODDistanceDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmLODDistanceDlg message handlers

void AgcmLODDistanceDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	m_pulDistance[0] = m_dwDistance0;
	m_pulDistance[1] = m_dwDistance1;
	m_pulDistance[2] = m_dwDistance2;
	m_pulDistance[3] = m_dwDistance3;
	m_pulDistance[4] = m_dwDistance4;
	
	CDialog::OnOK();
}
