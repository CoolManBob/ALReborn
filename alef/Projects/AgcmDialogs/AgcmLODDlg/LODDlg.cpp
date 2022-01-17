// LODDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LODDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLODDlg dialog


CLODDlg::CLODDlg(AgcdPreLOD *pstPreLOD, AgcdDefaultLODInfo *pstInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CLODDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLODDlg)
	m_bBillboard = FALSE;
	m_bTransformGeom = FALSE;
	m_bUseAtomicIndex = FALSE;
	m_lLODIndex = -1;
	m_bYAxis = FALSE;
	m_nDistanceType = -1;
	m_unMaxDistance = 0;
	//}}AFX_DATA_INIT

	AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.CopyPreLOD(&m_csAgcdPreLOD, pstPreLOD);

	m_pstLODInfo				= pstInfo;
}


void CLODDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLODDlg)
	DDX_Control(pDX, IDC_LIST_LOD_DLG, m_csLODDisplay);
	DDX_Check(pDX, IDC_CHECK_BILLBOARD, m_bBillboard);
	DDX_Check(pDX, IDC_CHECK_TRANSFORM_GEOMETRY, m_bTransformGeom);
	DDX_Check(pDX, IDC_CHECK_USE_ATOMIC_INDEX, m_bUseAtomicIndex);
	DDX_CBIndex(pDX, IDC_COMBO_LODDLG_INDEX, m_lLODIndex);
	DDX_Check(pDX, IDC_CHECK_YAXIS, m_bYAxis);
	DDX_CBIndex(pDX, IDC_COMBO_DISTANCE_TYPE, m_nDistanceType);
	DDX_Text(pDX, IDC_EDIT_MAX_DISTANCE, m_unMaxDistance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLODDlg, CDialog)
	//{{AFX_MSG_MAP(CLODDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_LODDLG_INDEX, OnSelchangeComboLoddlgIndex)
	ON_LBN_SELCHANGE(IDC_LIST_LOD_DLG, OnSelchangeListLodDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_BILLBOARD, OnCheckBillboard)
	ON_BN_CLICKED(IDC_CHECK_TRANSFORM_GEOMETRY, OnCheckTransformGeometry)
	ON_BN_CLICKED(IDC_CHECK_YAXIS, OnCheckYaxis)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_PRE_LOD, OnButtonApplyPreLod)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_DISTANCE, OnButtonChangeDistance)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_LODDATA, OnButtonChangeLoddata)
	ON_BN_CLICKED(IDC_BUTTON_INSERT_LODDATA, OnButtonInsertLoddata)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_LODDATA, OnButtonRemoveLoddata)
	ON_BN_CLICKED(IDC_BUTTON_INSERT_DUMMY, OnButtonInsertDummy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLODDlg message handlers

void CLODDlg::OnSelchangeComboLoddlgIndex() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lPreLODIndex);
	if(pcsData)
		pcsData->m_bUseAtomicIndex = m_bUseAtomicIndex;
/*
	m_pstLODInfo->m_aulMaxDistance[m_lPreLODIndex]	= m_unMaxDistance;*/
	m_unMaxDistance									= m_pstLODInfo->m_aulMaxDistanceRatio[m_lLODIndex];
	UpdateData(FALSE);

	m_lPreLODIndex = m_lLODIndex;

	UpdateLODDisplay();
}

BOOL CLODDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (!InitializeLODIndex())
		return FALSE;

	if (!InitializeDistanceType())
		return FALSE;

	UpdateLODDisplay();
	EnableCheckButton(FALSE);

	GetDlgItem(IDC_COMBO_DISTANCE_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CHANGE_DISTANCE)->EnableWindow(FALSE);

	m_unMaxDistance	= m_pstLODInfo->m_aulMaxDistanceRatio[m_lLODIndex];
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID CLODDlg::ReCalcLODDisplayHorizon()
{
	CClientDC dc(this);
	
	CFont* pOldFont= (CFont*)dc.SelectObject(GetFont());

	int nMaxWidth = 0;
	CString szText;

	for(int i = 0; i < m_csLODDisplay.GetCount(); i ++)
	{
		m_csLODDisplay.GetText(i, szText);
		CSize sizText = dc.GetTextExtent(szText);
		
		if(sizText.cx > nMaxWidth)
		{
			nMaxWidth = sizText.cx;
		}
	}

	m_csLODDisplay.SetHorizontalExtent(nMaxWidth);
	dc.SelectObject(pOldFont);
}

VOID CLODDlg::UpdateLODDisplay()
{
	UpdateData(TRUE);

	CHAR			szTemp[256];

	AgcdPreLODData	*pcsData = AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
	{
		m_csLODDisplay.ResetContent();

		for(INT32 lCount = 0; lCount < AGPDLOD_MAX_NUM; ++lCount)
		{
			if(lCount == 0)
			{
				sprintf(szTemp, "[Level %d] = [%s]", lCount, m_pstLODInfo->m_paszDFFInfo[m_lLODIndex]);
				m_csLODDisplay.InsertString(lCount, szTemp);
			}
			else if(strcmp(pcsData->m_aszData[lCount], ""))
			{
				sprintf(szTemp, "[Level %d] = [%s]", lCount, pcsData->m_aszData[lCount]);
				m_csLODDisplay.InsertString(lCount, szTemp);
			}
		}
	}

	ReCalcLODDisplayHorizon();
}

void CLODDlg::OnSelchangeListLodDlg() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	EnableCheckButton(TRUE);

	INT32 lCurSel = m_csLODDisplay.GetCurSel();
	if(lCurSel < 1)
	{
		EnableCheckButton(FALSE);
		return;
	}

	AgcdPreLODData	*pcsData = AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
	{
		if(pcsData->m_stBillboard.isbill[lCurSel] > 0)
		{
			m_bBillboard		= TRUE;
			if(pcsData->m_stBillboard.isbill[lCurSel] == 2)
				m_bYAxis		= TRUE;

			GetDlgItem(IDC_CHECK_YAXIS)->EnableWindow(TRUE);
		}
		else
		{
			m_bBillboard		= FALSE;
			m_bYAxis			= FALSE;

			GetDlgItem(IDC_CHECK_YAXIS)->EnableWindow(FALSE);
		}

		if(pcsData->m_abTransformGeom[lCurSel])
			m_bTransformGeom	= TRUE;
		else
			m_bTransformGeom	= FALSE;

		UpdateData(FALSE);
	}
}

VOID CLODDlg::EnableCheckButton(BOOL bEnable)
{
	GetDlgItem(IDC_CHECK_BILLBOARD)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_YAXIS)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_TRANSFORM_GEOMETRY)->EnableWindow(bEnable);
}

BOOL CLODDlg::InitializeLODIndex()
{
	CHAR		szTemp[256];
	CComboBox	*pcsIndex = (CComboBox *)(GetDlgItem(IDC_COMBO_LODDLG_INDEX));
	if(!pcsIndex)
		return FALSE;

	pcsIndex->ResetContent();

	INT32 lCount;

	for(lCount = 0; lCount < m_csAgcdPreLOD.m_lNum; ++lCount)
	{
		sprintf(szTemp, "%d", lCount);
		pcsIndex->InsertString(lCount, szTemp);
	}

	if(lCount == 0)
		return FALSE;

	pcsIndex->SetCurSel(0);
	m_lPreLODIndex = 0;

	return TRUE;
}

void CLODDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.RemoveAllPreLODData(&m_csAgcdPreLOD);
}

void CLODDlg::OnCheckBillboard() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	INT32			lCurSel		= m_csLODDisplay.GetCurSel();
	if(lCurSel > 0)// && (lCurSel < m_csAgcdPreLOD.m_lNum))
	{
		AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
		if(!pcsData)
			return;

		if(m_bBillboard)
		{
			if(m_bYAxis)
				pcsData->m_stBillboard.isbill[lCurSel] = 2;
			else
				pcsData->m_stBillboard.isbill[lCurSel] = 1;

			GetDlgItem(IDC_CHECK_YAXIS)->EnableWindow(TRUE);
		}
		else
		{
			pcsData->m_stBillboard.isbill[lCurSel] = 0;
			GetDlgItem(IDC_CHECK_YAXIS)->EnableWindow(FALSE);
		}
	}
}

void CLODDlg::OnCheckTransformGeometry() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	INT32			lCurSel		= m_csLODDisplay.GetCurSel();
	if((lCurSel > 0)/* && (lCurSel < m_csAgcdPreLOD.m_lNum)*/)
	{
		AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
		if(!pcsData)
			return;

		pcsData->m_abTransformGeom[lCurSel] = m_bTransformGeom;
	}
}

void CLODDlg::OnCheckYaxis() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	INT32			lCurSel		= m_csLODDisplay.GetCurSel();
	if((lCurSel > 0)/* && (lCurSel < m_csAgcdPreLOD.m_lNum) */&& (m_bBillboard))
	{
		AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
		if(!pcsData)
			return;

		if(m_bYAxis)
		{
			pcsData->m_stBillboard.isbill[lCurSel] = 2;
		}
		else
		{
			pcsData->m_stBillboard.isbill[lCurSel] = 1;
		}
	}
}

void CLODDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	m_pstLODInfo->m_lDistanceType					= m_nDistanceType;

	AgcdPreLODData	*pcsData						= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
		pcsData->m_bUseAtomicIndex					= m_bUseAtomicIndex;

	m_pstLODInfo->m_aulMaxDistanceRatio[m_lLODIndex]		= m_unMaxDistance;

	AgcmLODDlg::GetInstance()->CloseLODDlg(&m_csAgcdPreLOD);

//	CDialog::OnOK();
}

void CLODDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	AgcmLODDlg::GetInstance()->CloseLODDlg();
//	CDialog::OnCancel();
}


void CLODDlg::OnButtonApplyPreLod() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	AgcdPreLODData	*pcsData						= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
		pcsData->m_bUseAtomicIndex					= m_bUseAtomicIndex;

	m_pstLODInfo->m_aulMaxDistanceRatio[m_lLODIndex]		= m_unMaxDistance;

	AgcmLODDlg::GetInstance()->ApplyLODDlg(&m_csAgcdPreLOD);
}

void CLODDlg::OnButtonChangeDistance() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	AgcmLODDlg::GetInstance()->OpenLODDistance(m_pstLODInfo->m_aaulDistance[m_lLODIndex]);
}

void CLODDlg::OnButtonChangeLoddata() 
{
	// TODO: Add your control notification handler code here
	if(m_csLODDisplay.GetCurSel() < 1)
		return;

	UpdateData(TRUE);

	INT32			lCurIndex	= m_csLODDisplay.GetCurSel();
	AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);


	if(AgcmLODDlg::GetInstance()->FindLODDataName(pcsData->m_aszData[lCurIndex]))
		UpdateLODDisplay();
}

void CLODDlg::OnButtonInsertLoddata() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	INT32 lNewLevel = m_csLODDisplay.GetCount();
	if(lNewLevel >= AGPDLOD_MAX_NUM)
	{
		::MessageBox(NULL, "더이상 추가할 수 없습니다.", "FULL", MB_OK);
		return;
	}

	AgcdPreLODData	*pcsData = AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
	{
		if(AgcmLODDlg::GetInstance()->FindLODDataName(pcsData->m_aszData[lNewLevel]))
			UpdateLODDisplay();
	}
}

void CLODDlg::OnButtonRemoveLoddata() 
{
	// TODO: Add your control notification handler code here
	if(m_csLODDisplay.GetCurSel() < 1)
		return;

	UpdateData(TRUE);

	INT32 lCurIndex = m_csLODDisplay.GetCurSel();
	if(lCurIndex > 0)
	{
		INT32			lMax		= m_csLODDisplay.GetCount();
		AgcdPreLODData	*pcsData	= AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);

		for(INT32 lCount = lCurIndex; lCount < lMax; ++lCount)
		{
			m_csLODDisplay.DeleteString(lCurIndex);
			strcpy(pcsData->m_aszData[lCount], "");
		}

		UpdateLODDisplay();
	}
}

BOOL CLODDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CLODDlg::OnButtonInsertDummy() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	INT32 lNewLevel = m_csLODDisplay.GetCount();
	if(lNewLevel >= AGPDLOD_MAX_NUM)
	{
		::MessageBox(NULL, "더이상 추가할 수 없습니다.", "FULL", MB_OK);
		return;
	}

	AgcdPreLODData	*pcsData = AgcmLODDlg::GetInstance()->m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(&m_csAgcdPreLOD, m_lLODIndex);
	if(pcsData)
	{
		strcpy(pcsData->m_aszData[lNewLevel], "DUMMY");
		UpdateLODDisplay();
	}
}

BOOL CLODDlg::InitializeDistanceType()
{
	if ((m_pstLODInfo->m_lDistanceType < 0) || (m_pstLODInfo->m_lDistanceType >= E_AGCM_LOD_DISTANCE_TYPE_NUM))
		return FALSE;

	CComboBox *pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_DISTANCE_TYPE));
	if (!pCombo)
		return FALSE;

	pCombo->ResetContent();

	INT32 lCount;

	for (lCount = 0; lCount < E_AGCM_LOD_DISTANCE_TYPE_NUM; ++lCount)
	{
		pCombo->InsertString(lCount, AgcmLODDlg::GetInstance()->m_pcsAgcmLODManager->GetDistanceTypeName(lCount));
	}

	if (lCount == 0)
		return FALSE;

	pCombo->SetCurSel(m_pstLODInfo->m_lDistanceType);

	return TRUE;
}
