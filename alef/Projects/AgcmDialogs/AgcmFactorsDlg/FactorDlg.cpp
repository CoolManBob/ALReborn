// FactorDlg.cpp : implementation file
//

#include "stdafx.h"
/*#include "../resource.h"
#include "AgcmFactorsDlg.h"
#include "FactorDlg.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFactorDlg dialog


CFactorDlg::CFactorDlg(AgpdFactor *pcsFactor, eAgpdFactorsType eFactorsType, INT32 lSubType, CWnd* pParent )
	: CDialog(CFactorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFactorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsFactor			= pcsFactor;
	m_eAgpdFactorsType	= eFactorsType;
	m_lSubType			= lSubType;

	memset(m_paEventButton, 0, sizeof(CEventButton *) * __FACTOR_DLG_MAX_EVENT_BUTTON__);
}


void CFactorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFactorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFactorDlg, CDialog)
	//{{AFX_MSG_MAP(CFactorDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFactorDlg message handlers

BOOL CFactorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AgpmFactors *pcsAgpmFactors = AgcmFactorsDlg::GetInstance()->GetAgpmFactors();
	if(!pcsAgpmFactors)
		return FALSE;

	const CHAR *pszFactorName = NULL;

	INT32 lIndex	= -1;
	INT32 lNum		= AGPD_FACTORS_MAX_TYPE;

	AgpdFactorDetail *pstDetail = NULL;
	if(m_eAgpdFactorsType != AGPD_FACTORS_TYPE_NONE)
	{
		pstDetail = pcsAgpmFactors->GetFactorDetail((INT32)(m_eAgpdFactorsType));
		if(!pstDetail)
			return FALSE;

		lIndex	= (INT32)(m_eAgpdFactorsType);
		lNum	= pstDetail->lNumber;

		if(m_lSubType > -1)
		{
			if(	(pstDetail->eFactorType == AGPD_FACTORS_TYPE_RESULT) ||
				(pstDetail->eFactorType == AGPD_FACTORS_TYPE_NONE)		)
				return FALSE;
			
			AgpdFactorDetail *pstSubDetail = pcsAgpmFactors->GetFactorDetail((INT32)(pstDetail->eFactorType));
			if(!pstSubDetail)
				return FALSE;

			lIndex	= (INT32)(pstDetail->eFactorType);
			lNum	= pstSubDetail->lNumber;
		}
	}

	const INT32 lXTemp	= 35;
	const INT32 lYTemp1	= 25;
	const INT32 lYTemp2	= 10;
	const INT32 lHeight	= 20;
	const INT32 lWidth	= 120;

	CHAR	szTemp[256];
	INT32	lButtonIndex = 0;
	for(INT16 nCount = 0; nCount < lNum; ++nCount)
	{
		if(lIndex == -1)
		{
			pszFactorName = pcsAgpmFactors->GetFactorTypeName(nCount);
			if(!pszFactorName)
				continue;

			strcpy(szTemp, pszFactorName);
		}
		else
		{
			pszFactorName = pcsAgpmFactors->GetFactorININame(lIndex, nCount);
			if(!pszFactorName)
				continue;

			if((pstDetail) && (pstDetail->eFactorType != AGPD_FACTORS_TYPE_NONE) && (pstDetail->eFactorType != AGPD_FACTORS_TYPE_RESULT) && (m_lSubType == -1))
			{
				strcpy(szTemp, pszFactorName);
			}
			else
			{
				INT32 lTempValue;
				if(m_lSubType > -1)
				{
					//if(!pcsAgpmFactors->GetValue(m_pcsFactor, &lTempValue, (eAgpdFactorsType)(lIndex), m_lSubType, nCount))
					if(!pcsAgpmFactors->GetValue(m_pcsFactor, &lTempValue, m_eAgpdFactorsType, m_lSubType, nCount))
						lTempValue = 0;
				}
				else
				{
					//if(!pcsAgpmFactors->GetValue(m_pcsFactor, &lTempValue, (eAgpdFactorsType)(lIndex), nCount))
					if(!pcsAgpmFactors->GetValue(m_pcsFactor, &lTempValue, m_eAgpdFactorsType, nCount))
						lTempValue = 0;
				}

				sprintf(szTemp, "%s(%d)", pszFactorName, lTempValue);
			}
		}

		RECT rtRect = {	(lXTemp),
						((lYTemp1) + (lHeight * lButtonIndex) + (lYTemp2 * (lButtonIndex + 1))),
						(lXTemp + lWidth),
						((lYTemp1) + (lHeight * (lButtonIndex + 1)) + (lYTemp2 * (lButtonIndex + 1)))	};

		m_alTempSubType[lButtonIndex] = nCount;

		m_paEventButton[lButtonIndex] = new CEventButton(	this, rtRect, szTemp, TRUE,
															PressEventButtonCB,
															(PVOID)(this),
															(PVOID)(&m_eAgpdFactorsType),
															(PVOID)(&m_lSubType),
															(PVOID)(m_alTempSubType + lButtonIndex)	);

		if(++lButtonIndex >= __FACTOR_DLG_MAX_EVENT_BUTTON__)
		{
			ReleaseEventButton();
			return FALSE;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID CFactorDlg::ReleaseEventButton()
{
	for(INT16 nCount = 0; nCount < __FACTOR_DLG_MAX_EVENT_BUTTON__; ++nCount)
	{
		if(m_paEventButton[nCount])
		{
			delete m_paEventButton[nCount];
			m_paEventButton[nCount] = NULL;
		}
	}
}

void CFactorDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	ReleaseEventButton();
}

BOOL CFactorDlg::PressEventButtonCB(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3)
{
	CFactorDlg			*pThis			= (CFactorDlg *)(pClass);
	INT32				lFactorType		= *((INT32 *)(pData1));
	INT32				lSubType		= *((INT32 *)(pData2));
	INT32				lValueIndex		= *((INT32 *)(pData3));

	AgpmFactors			*pcsAgpmFactors	= AgcmFactorsDlg::GetInstance()->GetAgpmFactors();
	if(!pcsAgpmFactors)
		return FALSE;

	AgpdFactor *pcsFactor = pThis->GetFactor();
	if(!pcsFactor)
		return FALSE;

	if(lFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		if(	(lValueIndex == AGPD_FACTORS_TYPE_NONE) ||
			(lValueIndex == AGPD_FACTORS_TYPE_RESULT) ||
			(lValueIndex == AGPD_FACTORS_TYPE_OWNER) ||
			(lValueIndex == AGPD_FACTORS_MAX_TYPE)		)
			return FALSE;

		CFactorDlg dlg(pcsFactor, (eAgpdFactorsType)(lValueIndex));
		dlg.DoModal();
	}
	else
	{
		AgpdFactorDetail *pstDetail = pcsAgpmFactors->GetFactorDetail(lFactorType);
		if(!pstDetail)
			return FALSE;

		if(pstDetail->eFactorType == AGPD_FACTORS_TYPE_RESULT)
			return FALSE;

		if((lSubType > -1) && (pstDetail->eFactorType != AGPD_FACTORS_TYPE_NONE))
		{
			pstDetail = pcsAgpmFactors->GetFactorDetail(pstDetail->eFactorType);
		}

		if(pstDetail->eFactorType == AGPD_FACTORS_TYPE_NONE)
		{
			INT32 lValue;

			if(!pcsAgpmFactors->GetValue(	pcsFactor, &lValue, (eAgpdFactorsType)(lFactorType),
											(	(lSubType > -1) ? (lSubType) : (lValueIndex)	),
											(	(lSubType > -1) ? (lValueIndex) : (-1)	)			)
				)
			{
				lValue = 0;
			}

			CInputDlg dlg(&lValue);
			if(dlg.DoModal() == IDOK)
			{
				pcsAgpmFactors->SetFactor(pcsFactor, NULL, lFactorType);
				if(	!pcsAgpmFactors->SetValue(	pcsFactor, lValue, (eAgpdFactorsType)(lFactorType),
												(	(lSubType > -1) ? (lSubType) : (lValueIndex)	),
												(	(lSubType > -1) ? (lValueIndex) : (-1)	)			)
					)
				{
					return FALSE;
				}

				CHAR szTempText[256];
				pThis->GetTextEventButton(szTempText, lValueIndex);

				INT16 nLen = strlen(szTempText);
				for(INT16 nCount = 0; nCount < nLen; ++nCount)
				{
					if(szTempText[nCount] == '(')
					{
						szTempText[nCount] = '\0';
						break;
					}
				}
				sprintf(szTempText, "%s(%d)", szTempText, lValue);

				pThis->SetTextEventButton(szTempText, lValueIndex);
			}
		}
		else
		{
			CFactorDlg dlg(pcsFactor, (eAgpdFactorsType)(lFactorType), lValueIndex);
			dlg.DoModal();
		}
	}

	return TRUE;
}

VOID CFactorDlg::GetTextEventButton(CHAR *pszText, INT32 lIndex)
{
	m_paEventButton[lIndex]->GetWindowText(pszText, 256);
}

VOID CFactorDlg::SetTextEventButton(CHAR *szText, INT32 lIndex)
{
	m_paEventButton[lIndex]->SetWindowText(szText);
}*/