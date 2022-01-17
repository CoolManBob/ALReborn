// SetFace.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"
#include "SetFace.h"

#include "AgcmSetFaceDlg.h"
#include ".\setface.h"


// CSetFace 대화 상자입니다.

IMPLEMENT_DYNCREATE(CSetFace, CDialog)

CSetFace::CSetFace(CWnd* pParent /*=NULL*/)
	: CDialog(CSetFace::IDD, pParent)
{
	m_pstClump	= NULL;
	m_pstFaceAtomic	= NULL;
}

CSetFace::~CSetFace()
{
}

void CSetFace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FACE_ATOMIC, m_csFaceAtomic);
}

BOOL CSetFace::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BEGIN_MESSAGE_MAP(CSetFace, CDialog)
	ON_CBN_SELCHANGE(IDC_FACE_ATOMIC, OnCbnSelchangeFaceAtomic)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()



// CSetFace 메시지 처리기입니다.

BOOL	CSetFace::SetFaceAtomic(RpAtomic *pstAtomic)
{
	if (m_pstFaceAtomic != pstAtomic)
	{
		m_pstFaceAtomic	= pstAtomic;
		m_pstClump	= NULL;

		if (m_pstFaceAtomic)
		{
			INT32		lListIndex;
			CHAR		szAtomic[16];
			RpAtomic *	pstClumpAtomics;

			m_pstClump	= RpAtomicGetClump(m_pstFaceAtomic);

			m_csFaceAtomic.ResetContent();
			pstClumpAtomics = m_pstClump->atomicList;
			do
			{
				sprintf(szAtomic, "%d", pstClumpAtomics->id);

				lListIndex	= m_csFaceAtomic.AddString(szAtomic);
				m_csFaceAtomic.SetItemData(lListIndex, pstClumpAtomics->id);

				if (pstAtomic == pstClumpAtomics)
					m_csFaceAtomic.SelectString(-1, szAtomic);

				pstClumpAtomics = pstClumpAtomics->next;
			} while (pstClumpAtomics != m_pstClump->atomicList);
		}
	}

	return TRUE;
}

void CSetFace::OnCbnSelchangeFaceAtomic()
{
	if(AgcmSetFaceDlg::GetInstance() && m_pstClump)
	{
		INT32	lIndex;

		lIndex = (INT32) m_csFaceAtomic.GetItemData(m_csFaceAtomic.GetCurSel());
		if (lIndex != CB_ERR)
		{
			RpAtomic *	pstAtomic = m_pstClump->atomicList;

			do
			{
				if (pstAtomic->id == lIndex)
				{
					if (AgcmSetFaceDlg::GetInstance()->m_fnCBChangeAtomic)
						AgcmSetFaceDlg::GetInstance()->m_fnCBChangeAtomic(pstAtomic, AgcmSetFaceDlg::GetInstance()->m_pvClass, NULL);
					break;
				}

				pstAtomic	= pstAtomic->next;
			} while (pstAtomic != m_pstClump->atomicList);
		}
	}
}

void CSetFace::OnOk()
{
	if (AgcmSetFaceDlg::GetInstance()->m_fnCBApply)
	{
		AgcmSetFaceDlg::GetInstance()->m_fnCBApply(m_pstFaceAtomic, AgcmSetFaceDlg::GetInstance()->m_pvClass, NULL);
	}

	if (AgcmSetFaceDlg::GetInstance())
		AgcmSetFaceDlg::GetInstance()->CloseMainFaceDlg();
}

void CSetFace::OnCancel()
{
	if (AgcmSetFaceDlg::GetInstance())
		AgcmSetFaceDlg::GetInstance()->CloseMainFaceDlg();
}
