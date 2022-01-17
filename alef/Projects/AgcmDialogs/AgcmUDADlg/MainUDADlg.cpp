// MainUDADlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmUDADlg.h"
#include "../resource.h"
#include "MainUDADlg.h"
#include ".\mainudadlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainUDADlg dialog


CMainUDADlg::CMainUDADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainUDADlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainUDADlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_plRenderType	= NULL;
	m_pstAtomic		= NULL;
	m_pstClump		= NULL;
}


void CMainUDADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainUDADlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_ATOMIC_INDEX, m_csAtomicIndex);
}


BEGIN_MESSAGE_MAP(CMainUDADlg, CDialog)
	//{{AFX_MSG_MAP(CMainUDADlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_UDA_APPLY, OnButtonUdaApply)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_ATOMIC_INDEX, OnCbnSelchangeComboAtomicIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainUDADlg message handlers

BOOL CMainUDADlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CMainUDADlg::OnOK()
{
	ApplyData();

	if(AgcmUDADlg::GetInstance())
	{
		AgcmUDADlg::GetInstance()->CloseMainUDADlg();
	}

//	CDialog::OnOK();
}

void CMainUDADlg::OnDestroy()
{
//	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

VOID CMainUDADlg::EnableControl(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_UDA_APPLY)->EnableWindow(bEnable);

	GetDlgItem(IDC_COMBO_RENDER_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_BLEND_MODE)->EnableWindow(bEnable);
}

BOOL CMainUDADlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!AgcmUDADlg::GetInstance())
		return FALSE;

	if (!AgcmUDADlg::GetInstance()->GetAgcmRenderModule())
		return FALSE;

	CHAR		*pszTemp;
	INT32		lIndex;
	CComboBox	*pcsComboBox	= (CComboBox *)(GetDlgItem(IDC_COMBO_RENDER_TYPE));
	if (!pcsComboBox)
		return FALSE;

	for (lIndex = 0; lIndex < R_RENDER_TYPE_NUM; ++lIndex)
	{
		pszTemp = AgcmUDADlg::GetInstance()->GetAgcmRenderModule()->GetRenderTypeName(lIndex);

		pcsComboBox->InsertString(lIndex, pszTemp);
	}

	if (lIndex > 0)
		pcsComboBox->SetCurSel(0);

	pcsComboBox					= (CComboBox *)(GetDlgItem(IDC_COMBO_BLEND_MODE));
	if (!pcsComboBox)
		return FALSE;

	for (lIndex = 0; lIndex < R_BELND_MODE_NUM; ++lIndex)
	{
		pszTemp = AgcmUDADlg::GetInstance()->GetAgcmRenderModule()->GetRenderBlendModeName(lIndex);

		pcsComboBox->InsertString(lIndex, pszTemp);
	}

	if (lIndex > 0)
		pcsComboBox->SetCurSel(0);

	return TRUE;
}

VOID CMainUDADlg::SetRenderTypePointer(INT32 *plRenderType, RpAtomic *pstAtomic)
{
	m_pstAtomic				= pstAtomic;
	m_plRenderType			= plRenderType;

	INT32	lRenderType		= (*plRenderType & 0x0000ffff);
	INT32	lBlendMode		= ((*plRenderType & 0xffff0000) >> 16);

	((CComboBox *)(GetDlgItem(IDC_COMBO_RENDER_TYPE)))->SetCurSel(lRenderType);
	((CComboBox *)(GetDlgItem(IDC_COMBO_BLEND_MODE)))->SetCurSel(lBlendMode);

	if (m_pstAtomic)
	{
		m_pstClump = RpAtomicGetClump(m_pstAtomic);
		if (m_pstClump)
		{
			INT32		lIndex;
			INT32		lIndexList;
			CHAR		szTemp[10];

			m_csAtomicIndex.EnableWindow(TRUE);

			m_csAtomicIndex.ResetContent();
			for (lIndex = 0; lIndex < m_pstClump->iLastAtomicID; ++lIndex)
			{
				sprintf(szTemp, "%d", lIndex);
				lIndexList	= m_csAtomicIndex.AddString(szTemp);
				m_csAtomicIndex.SetItemData(lIndexList, lIndex);

				if (lIndex == pstAtomic->id)
					m_csAtomicIndex.SelectString(-1, szTemp);
			}
		}
	}
	else
	{
		m_csAtomicIndex.EnableWindow(FALSE);
		m_pstClump	= NULL;
	}
}

void CMainUDADlg::OnButtonUdaApply() 
{
	ApplyData();
}

VOID CMainUDADlg::ApplyData()
{
	if( !m_plRenderType || !m_pstAtomic )	return;

	INT32	lRenderType = ((CComboBox *)(GetDlgItem(IDC_COMBO_RENDER_TYPE)))->GetCurSel();

//	if ((m_plRenderType) && (lRenderType))
		*(m_plRenderType) = lRenderType;

	if ((lRenderType == R_BLEND_SORT) || (lRenderType == R_BLEND_NSORT))
	{
		INT32	lBlendMode = ((CComboBox *)(GetDlgItem(IDC_COMBO_BLEND_MODE)))->GetCurSel();

		*(m_plRenderType) = (INT32)((lBlendMode << 16) | (lRenderType));
	}

	if (AgcmUDADlg::GetInstance() && AgcmUDADlg::GetInstance()->m_fnCBApply)
	{
		AgcmUDADlg::GetInstance()->m_fnCBApply(m_pstAtomic, AgcmUDADlg::GetInstance()->m_pvClass, NULL);
	}
}

void CMainUDADlg::OnCancel()
{
	if(AgcmUDADlg::GetInstance())
	{
		AgcmUDADlg::GetInstance()->CloseMainUDADlg();
	}
}

void CMainUDADlg::OnCbnSelchangeComboAtomicIndex()
{
	if(AgcmUDADlg::GetInstance() && m_pstClump)
	{
		INT32	lIndex;

		lIndex = m_csAtomicIndex.GetItemData(m_csAtomicIndex.GetCurSel());
		if (lIndex != CB_ERR)
		{
			RpAtomic *	pstAtomic = m_pstClump->atomicList;

			do
			{
				if (pstAtomic->id == lIndex)
				{
					if (AgcmUDADlg::GetInstance()->m_fnCBChangeAtomic)
						AgcmUDADlg::GetInstance()->m_fnCBChangeAtomic(pstAtomic, AgcmUDADlg::GetInstance()->m_pvClass, NULL);
					break;
				}

				pstAtomic	= pstAtomic->next;
			} while (pstAtomic != m_pstClump->atomicList);
		}
	}
}
