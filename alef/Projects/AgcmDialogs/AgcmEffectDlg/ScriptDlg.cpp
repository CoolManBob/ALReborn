// ScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeffectdlg.h"
#include "ScriptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg dialog


CScriptDlg::CScriptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScriptDlg)
	m_strScript = _T("");
	//}}AFX_DATA_INIT

	m_pstAgcdUseEffectSet	= NULL;
	m_lEffectIndex			= -1;
}


void CScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScriptDlg)
	DDX_Text(pDX, IDC_EDIT_SCRIPT, m_strScript);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScriptDlg, CDialog)
	//{{AFX_MSG_MAP(CScriptDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptDlg message handlers
void CScriptDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

VOID CScriptDlg::InputString(CHAR *szString)
{
	CHAR szTemp[256];
	sprintf(szTemp, "\r\n%s", szString);

	m_strScript += szTemp;
	UpdateData(FALSE);
}

void CScriptDlg::OnOK() 
{
	// TODO: Add extra validation here
	AgcmEffectDlg::GetInstance()->CloseEffectScript();

//	CDialog::OnOK();
}

BOOL CScriptDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

/*BOOL CScriptDlg::AddCustData(CHAR *pszDestCustData, CHAR *pszAddCustData)
{
	if (!AgcmEffectDlg::GetInstance())
		return FALSE;

	if (!AgcmEffectDlg::GetInstance()->GetAgcmEventEffect())
		return FALSE;

	AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->AddCustData(pszDestCustData, pszAddCustData);

	return TRUE;
}*/

BOOL CScriptDlg::AddCustData(AgcdUseEffectSetData *pcsData, CHAR *pszAddCustData)
{
	if (!AgcmEffectDlg::GetInstance())
		return FALSE;

	if (!AgcmEffectDlg::GetInstance()->GetAgcmEventEffect())
		return FALSE;

	CHAR	szTempCustData[256];
	if (pcsData->m_pszCustData)
		strcpy(szTempCustData, pcsData->m_pszCustData);
	else
		memset(szTempCustData, 0, 256);

	AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->AddCustData(szTempCustData, pszAddCustData);

	ReallocCopyString( &pcsData->m_pszCustData, szTempCustData );

	return TRUE;
}

void CScriptDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if ((!m_pstAgcdUseEffectSet) || (m_lEffectIndex < 0))
		return;

//	AgcdUseEffectSetData	*pstData	= m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex];
	AgcdUseEffectSetData	*pcsData =
		AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, m_lEffectIndex);
	if (!pcsData)
		return;

	AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->Delete(pcsData);

	CEdit	*pcsEdit = (CEdit *)(GetDlgItem(IDC_EDIT_SCRIPT));
	if (!pcsEdit)
		return;

//	memset(pcsData, 0, sizeof(AgcdUseEffectSetData));
//	pcsData->m_fScale = 1.0f;

	CHAR	szLine[256];
	CHAR	szTemp[256];
	FLOAT	fTemp;
	
	INT32	lRt;
	INT32	lLineCount = pcsEdit->GetLineCount();
	ZeroMemory( szLine, sizeof(szLine) );
	// 첫번째 라인부터 돌면서 값을 저장한다.
	for (INT32 lCount = 1; lCount < lLineCount; ++lCount)
	{
//		memset(szLine, 0, 256);

		lRt = pcsEdit->GetLine(lCount, szLine, sizeof(szLine));
		if (!szLine[0])
			continue;

		szLine[lRt] = '\0';

		INT32 lIndex;

		// 비교할 버퍼를 설정한다.
		for (lIndex = 0; lIndex < 256; ++lIndex)
		{
			szTemp[lIndex] = szLine[lIndex];

			if (szLine[lIndex] == '=')
			{
				// 이러면 안되지! -_-;;
				if (++lIndex == 256)
				{
					MessageBox("ERROR!!!", "ERROR!!!");
					return; // error
				}

				szTemp[lIndex] = '\0';
				break;
			}
		}

		// 이러면 안되지! -_-;;
		if (lIndex == 256)
		{
			MessageBox("ERROR!!!", "ERROR!!!");
			return; // error
		}

		// 데이터가 없으므로 다음으로 넘긴다.
		if (szLine[lIndex] == '\0')
			continue;

		if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_SOUND_NAME))
		{
			pcsData->m_pszSoundName	= new CHAR[strlen(szLine + lIndex) + 1];
			strcpy(pcsData->m_pszSoundName, szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_EID_NAME))
		{
			pcsData->m_ulEID = atoi(szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_OFFSETX_NAME))
		{
			sscanf(szLine + lIndex, "%f", &fTemp);

//			if (fTemp != 0.0f)
			{
				if (!pcsData->m_pv3dOffset)
				{
					pcsData->m_pv3dOffset		= new RwV3d();
					pcsData->m_pv3dOffset->y	= 0.0f;
					pcsData->m_pv3dOffset->z	= 0.0f;
				}

//				pcsData->m_v3dOffset.x = fTemp;
				pcsData->m_pv3dOffset->x	= fTemp;
			}
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_OFFSETY_NAME))
		{
			sscanf(szLine + lIndex, "%f", &fTemp);
			
//			if (fTemp != 0.0f)
			{
				if (!pcsData->m_pv3dOffset)
				{
					pcsData->m_pv3dOffset		= new RwV3d();
					pcsData->m_pv3dOffset->x	= 0.0f;
					pcsData->m_pv3dOffset->z	= 0.0f;
				}

//				pcsData->m_v3dOffset.x = fTemp;
				pcsData->m_pv3dOffset->y	= fTemp;
			}
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_OFFSETZ_NAME))
		{
			sscanf(szLine + lIndex, "%f", &fTemp);

//			if (fTemp != 0.0f)
			{
				if (!pcsData->m_pv3dOffset)
				{
					pcsData->m_pv3dOffset	= new RwV3d();
					pcsData->m_pv3dOffset->x	= 0.0f;
					pcsData->m_pv3dOffset->y	= 0.0f;
				}

//				pcsData->m_v3dOffset.x = fTemp;
				pcsData->m_pv3dOffset->z	= fTemp;
			}

			if (	(pcsData->m_pv3dOffset) &&
					(pcsData->m_pv3dOffset->x == 0.0f) &&
					(pcsData->m_pv3dOffset->y == 0.0f) &&
					(pcsData->m_pv3dOffset->z == 0.0f)		)
			{
				delete pcsData->m_pv3dOffset;
				pcsData->m_pv3dOffset = NULL;
			}
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_SCALE_NAME))
		{
			sscanf(szLine + lIndex, "%f", &fTemp);
			pcsData->m_fScale = fTemp;
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_NODE_NAME))
		{
			pcsData->m_lParentNodeID = atoi(szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_GAP_NAME))
		{
			pcsData->m_ulStartGap = atoi(szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_CONDITION_NAME))
		{
			pcsData->m_ulConditionFlags = atoi(szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_CUSTOM_FLAGS))
		{
			pcsData->m_ulCustomFlags = atoi(szLine + lIndex);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_RIGHT))
		{
			if (!pcsData->m_pcsRotation)
				pcsData->m_pcsRotation	= new AgcdUseEffectSetDataRotation();

			sscanf(szLine + lIndex, "%f:%f:%f",
				&pcsData->m_pcsRotation->m_stRight.x,
				&pcsData->m_pcsRotation->m_stRight.y,
				&pcsData->m_pcsRotation->m_stRight.z	);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_UP))
		{
			if (!pcsData->m_pcsRotation)
				pcsData->m_pcsRotation	= new AgcdUseEffectSetDataRotation();

			sscanf(szLine + lIndex, "%f:%f:%f",
				&pcsData->m_pcsRotation->m_stUp.x,
				&pcsData->m_pcsRotation->m_stUp.y,
				&pcsData->m_pcsRotation->m_stUp.z		);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_AT))
		{
			if (!pcsData->m_pcsRotation)
				pcsData->m_pcsRotation	= new AgcdUseEffectSetDataRotation();

			sscanf(szLine + lIndex, "%f:%f:%f",
				&pcsData->m_pcsRotation->m_stAt.x,
				&pcsData->m_pcsRotation->m_stAt.y,
				&pcsData->m_pcsRotation->m_stAt.z		);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_ANIM_POINT_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_ANIM_POINT, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_DESTROY_MATCH_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_TARGET_STRUCK_ANIM_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_CUST_DATA_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_ON_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_OFF_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_TARGET_OPTION_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_OPTION, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_CALC_DIST_OPTION_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_LINK_SKILL_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_LINK_SKILL, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_DIRECTION_OPTION_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_CHECK_USER_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_USER, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_ACTION_OBJECT_NAME))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT, atoi(szLine + lIndex));
//			AddCustData(pcsData->m_szCustData, szTemp);
			AddCustData(pcsData, szTemp);
		}
		//. 2005. 09. 30 Nonstopdj
		//. cold breath(입김)을 위한 스크립트 추가
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_NATURE_CONDITION))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION, atoi(szLine + lIndex));
			AddCustData(pcsData, szTemp);
		}
		//@{ 2007/01/03 burumal
		else if (!strcmp(szTemp, D_AGCD_EFFECT_CUST_DATA_CASTING_EFFECT))
		{
			sprintf(szTemp, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT, atoi(szLine + lIndex));
			AddCustData(pcsData, szTemp);
		}
		//@}
		else
		{
			MessageBox("구문이 잘못되었습니다!", "ERROR", MB_OK);
		}
	}

	m_pstAgcdUseEffectSet->m_ulConditionFlags	= 0;
	m_pstAgcdUseEffectSet->m_ulCustomFlags		= 0;
//	for (lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	AgcdUseEffectSetList	*pcsCurrent	= m_pstAgcdUseEffectSet->m_pcsHead;
	while (pcsCurrent)
	{
//		if (m_pstAgcdUseEffectSet->m_astData[lCount])
//			m_pstAgcdUseEffectSet->m_ulConditionFlags	|= m_pstAgcdUseEffectSet->m_astData[lCount]->m_ulConditionFlags;

		m_pstAgcdUseEffectSet->m_ulConditionFlags	|= pcsCurrent->m_csData.m_ulConditionFlags;
		m_pstAgcdUseEffectSet->m_ulCustomFlags		|= pcsCurrent->m_csData.m_ulCustomFlags;

		pcsCurrent = pcsCurrent->m_pcsNext;
	}

	if (AgcmEffectDlg::GetInstance())
	{
		AgcmEffectDlg::GetInstance()->_EnumCallback(AGCM_EFFECTDLG_CB_ID_SAVE_SCRIPT, NULL, NULL);
	}
}

VOID CScriptDlg::UpdateScript(CHAR *szHeader, CHAR *szData)
{
	CString		strTemp;
	strTemp = szHeader;
	if (szData[0])
		strTemp += szData;

	InputString((LPSTR)(LPCSTR)(strTemp));
}

VOID CScriptDlg::UpdateScriptDlg(AgcdUseEffectSet *pstSet, INT32 lEffectIndex)
{
	if (pstSet)
		m_pstAgcdUseEffectSet	= pstSet;
	if (lEffectIndex != -1)
		m_lEffectIndex			= lEffectIndex;

	if ((!m_pstAgcdUseEffectSet) || (m_lEffectIndex < 0))
		return;

//	AgcdUseEffectSetData	*pstData	= m_pstAgcdUseEffectSet->m_astData[m_lEffectIndex];
	AgcdUseEffectSetData	*pcsData	= AgcmEffectDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->GetData(m_pstAgcdUseEffectSet, m_lEffectIndex);
	if (!pcsData)
		return;

	CHAR	szTemp[256];

	sprintf(szTemp, "[%d]", m_lEffectIndex);
	m_strScript		= szTemp;

	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_SOUND_NAME, (pcsData->m_pszSoundName) ? (pcsData->m_pszSoundName) : (""));

	sprintf(szTemp, "%d", pcsData->m_ulEID);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_EID_NAME, szTemp);

	print_compact_format(szTemp, "%f", (pcsData->m_pv3dOffset) ? (pcsData->m_pv3dOffset->x) : (0.0f));
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_OFFSETX_NAME, szTemp);
	print_compact_format(szTemp, "%f", (pcsData->m_pv3dOffset) ? (pcsData->m_pv3dOffset->y) : (0.0f));
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_OFFSETY_NAME, szTemp);
	print_compact_format(szTemp, "%f", (pcsData->m_pv3dOffset) ? (pcsData->m_pv3dOffset->z) : (0.0f));
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_OFFSETZ_NAME, szTemp);

	print_compact_format(szTemp, "%f", pcsData->m_fScale);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_SCALE_NAME, szTemp);

	sprintf(szTemp, "%d", pcsData->m_lParentNodeID);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_NODE_NAME, szTemp);

	sprintf(szTemp, "%d", pcsData->m_ulStartGap);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_GAP_NAME, szTemp);

	sprintf(szTemp, "%d", pcsData->m_ulConditionFlags);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_CONDITION_NAME, szTemp);

	sprintf(szTemp, "%d", pcsData->m_ulCustomFlags);
	UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_CUSTOM_FLAGS, szTemp);

	if (pcsData->m_pcsRotation)
	{
		print_compact_format(szTemp, "%f:%f:%f",
			pcsData->m_pcsRotation->m_stRight.x,
			pcsData->m_pcsRotation->m_stRight.y,
			pcsData->m_pcsRotation->m_stRight.z		);
		UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_RIGHT, szTemp);

		print_compact_format(szTemp, "%f:%f:%f",
			pcsData->m_pcsRotation->m_stUp.x,
			pcsData->m_pcsRotation->m_stUp.y,
			pcsData->m_pcsRotation->m_stUp.z	);
		UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_UP, szTemp);

		print_compact_format(szTemp, "%f:%f:%f",
			pcsData->m_pcsRotation->m_stAt.x,
			pcsData->m_pcsRotation->m_stAt.y,
			pcsData->m_pcsRotation->m_stAt.z	);
		UpdateScript(D_AGCD_EFFECT_DATA_SCRIPT_ROTATE_AT, szTemp);
	}

	if (AgcmEffectDlg::GetInstance())
	{
		AgcmEventEffect	*pcsAgcmEventEffect = AgcmEffectDlg::GetInstance()->GetAgcmEventEffect();
		if (pcsAgcmEventEffect)
		{
			INT32	alCustIndex[D_AGCD_EFFECT_MAX_CUST_DATA];

			if ((pcsData->m_pszCustData) && (pcsAgcmEventEffect->GetCustDataIndex(pcsData->m_pszCustData, alCustIndex) > 0))
			{
				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ANIM_POINT] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ANIM_POINT]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_ANIM_POINT_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_DESTROY_MATCH_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_TARGET_STRUCK_ANIM_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_EXCEPTION_TYPE_CUST_DATA_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_ON_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_CHECK_TIME_OFF_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_TARGET_OPTION_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_CALC_DIST_OPTION_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_LINK_SKILL] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_LINK_SKILL]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_LINK_SKILL_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_DIRECTION_OPTION_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_USER] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_USER]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_CHECK_USER_NAME, szTemp);
				}

				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_ACTION_OBJECT_NAME, szTemp);
				}
				//. 2005. 09. 30 Nonstopdj
				if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_NATURE_CONDITION, szTemp);
				}

				//@{ 2007/01/03 burumal				
				//if (alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT] > -1)
				{
					sprintf(szTemp, "%d", alCustIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT]);
					UpdateScript(D_AGCD_EFFECT_CUST_DATA_CASTING_EFFECT, szTemp);
				}
				//@}
			}
		}
	}

	m_pstAgcdUseEffectSet->m_ulConditionFlags	= 0;
	m_pstAgcdUseEffectSet->m_ulCustomFlags		= 0;
//	for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
	AgcdUseEffectSetList	*pcsCurrent	= m_pstAgcdUseEffectSet->m_pcsHead;
	while (pcsCurrent)
	{
//		if (m_pstAgcdUseEffectSet->m_astData[lCount])
//			m_pstAgcdUseEffectSet->m_ulConditionFlags	|= m_pstAgcdUseEffectSet->m_astData[lCount]->m_ulConditionFlags;

		m_pstAgcdUseEffectSet->m_ulConditionFlags	|=	pcsCurrent->m_csData.m_ulConditionFlags;
		m_pstAgcdUseEffectSet->m_ulCustomFlags		|=	pcsCurrent->m_csData.m_ulCustomFlags;

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}
}

void CScriptDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_RETURN)
		return;
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CScriptDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreTranslateMessage(pMsg);
}

VOID CScriptDlg::EnableSaveButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(bEnable);
}

void CScriptDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
