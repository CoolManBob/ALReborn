// CharacterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmCharacterDlg.h"
#include "AgcmFileListDlg.h"
#include "CharacterDlg.h"
#include "CharCustomFlagsDlg.h"
#include ".\characterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharacterDlg dialog


CCharacterDlg::CCharacterDlg(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate, CWnd* pParent )
	: CDialog(CCharacterDlg::IDD, pParent)	
	, m_unRiderHeight(0)
	, m_fDepth(0)
{
	INT32 lHitRange = 0;

	if(AgcmCharacterDlg::GetInstance())
	{
		AgcmCharacterDlg::GetInstance()->GetAgpmFactors()->GetValue(&pcsAgpdCharacterTemplate->m_csFactor, &lHitRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE);
	}

	//{{AFX_DATA_INIT(CCharacterDlg)
	m_strArmourClump	=
		pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName ?
		_T(pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName) :
		_T("");

	m_strBaseClump		=
		pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName ?
		_T(pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName) :
		_T("");

	m_strTemplateName	= _T(pcsAgpdCharacterTemplate->m_szTName);
	m_unHeight			= pcsAgcdCharacterTemplate->m_lHeight;
	m_fDepth			= pcsAgcdCharacterTemplate->m_fDepth;
	m_unRiderHeight		= pcsAgcdCharacterTemplate->m_lRiderHeight;
	m_unHitRange		= lHitRange	;

	m_strPickAtomic		=
		pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName ?
		_T(pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName) :
		_T("");

	m_fBSphereCenterX	= pcsAgcdCharacterTemplate->m_stBSphere.center.x;
	m_fBSphereCenterY	= pcsAgcdCharacterTemplate->m_stBSphere.center.y;
	m_fBSphereCenterZ	= pcsAgcdCharacterTemplate->m_stBSphere.center.z;
	m_fBSphereRadius	= pcsAgcdCharacterTemplate->m_stBSphere.radius;
	m_fScale			= pcsAgcdCharacterTemplate->m_fScale;
	
	//@{ 2006/02/20 burumal
	//m_lNodeIndex		= pcsAgcdCharacterTemplate->m_lPickingNodeIndex;

	UINT32 uIdx;
	for ( uIdx = 0; uIdx < D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP; uIdx++ )
		m_lNodeIndex[uIdx] = pcsAgcdCharacterTemplate->m_lPickingNodeIndex[uIdx];
	//@}

	//@{ 2006/03/24 burumal
	m_bTagging			= pcsAgcdCharacterTemplate->m_bTagging;
	//@}

	//@{ 2006/08/17 burumal
	m_fSiegeWarfareCollisionBoxWidth		= pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth;
	m_fSiegeWarfareCollisionBoxHeight		= pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight;
	m_fSiegeWarfareCollisionSphereRadius	= pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius;
	m_fSiegeWarfareCollisionOffsetX			= pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX;
	m_fSiegeWarfareCollisionOffsetZ			= pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ;
	//@}

	//@{ 2006/08/22 burumal
	m_lLookAtNode		= pcsAgcdCharacterTemplate->m_lLookAtNode;
	//@}

	//@{ 2006/08/28 burumal
	m_bUseBending		= pcsAgcdCharacterTemplate->m_bUseBending;
	m_fBendingFactor	= pcsAgcdCharacterTemplate->m_fBendingFactor;
	//@}

	//@{ 2006/09/08 burumal
	m_bSelfDestructionAttackType = pcsAgpdCharacterTemplate->m_bSelfDestructionAttackType;
	//@}

	//@{ 2006/12/15 burumal
	m_bNonPickingType = pcsAgcdCharacterTemplate->m_bNonPickingType;
	//@}

	//}}AFX_DATA_INIT

	m_pcsAgpdCharacterTemplate	= pcsAgpdCharacterTemplate;
	m_pcsAgcdCharacterTemplate	= pcsAgcdCharacterTemplate;
}


void CCharacterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ARMOUR_CLUMP, m_strArmourClump);
	DDX_Text(pDX, IDC_EDIT_BASE_CLUMP, m_strBaseClump);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strTemplateName);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_HEIGHT, m_unHeight);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_HIT_RANGE, m_unHitRange);
	DDX_Text(pDX, IDC_EDIT_PICK_ATOMIC, m_strPickAtomic);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_X, m_fBSphereCenterX);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_Y, m_fBSphereCenterY);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_Z, m_fBSphereCenterZ);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_RADIUS, m_fBSphereRadius);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_SCALE, m_fScale);
	DDV_MinMaxFloat(pDX, m_fScale, 0.1f, 100.f);
	
	//@{ 2006/02/21 burumal
	// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP 정의된 수와 일치시키는것을 잊지말것
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX1, m_lNodeIndex[0]);
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX2, m_lNodeIndex[1]);
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX3, m_lNodeIndex[2]);
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX4, m_lNodeIndex[3]);
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX5, m_lNodeIndex[4]);
	DDX_Text(pDX, IDC_EDIT_PICKING_NODE_INDEX6, m_lNodeIndex[5]);
	//@}

	//@{ 2006/03/24 burumal	
	DDX_Check(pDX, IDC_TAGGING, m_bTagging);
	//@}

	//@{ 2006/08/17 burumal	
	if ( m_fSiegeWarfareCollisionBoxWidth < 0.0f )
		m_fSiegeWarfareCollisionBoxWidth = -m_fSiegeWarfareCollisionBoxWidth;
	DDX_Text(pDX, IDC_EDIT_SW_BOX_WIDTH, m_fSiegeWarfareCollisionBoxWidth);	
	
	if ( m_fSiegeWarfareCollisionBoxHeight < 0.0f )
		m_fSiegeWarfareCollisionBoxHeight = -m_fSiegeWarfareCollisionBoxHeight;
	DDX_Text(pDX, IDC_EDIT_SW_BOX_HEIGHT, m_fSiegeWarfareCollisionBoxHeight);
	
	if ( m_fSiegeWarfareCollisionSphereRadius < 0.0f )
		m_fSiegeWarfareCollisionSphereRadius = -m_fSiegeWarfareCollisionSphereRadius;
	DDX_Text(pDX, IDC_EDIT_SW_SPHERE_RADIUS, m_fSiegeWarfareCollisionSphereRadius);
	
	DDX_Text(pDX, IDC_EDIT_SW_COLLOBJ_OFFSET_X, m_fSiegeWarfareCollisionOffsetX);	
	DDX_Text(pDX, IDC_EDIT_SW_COLLOBJ_OFFSET_Z, m_fSiegeWarfareCollisionOffsetZ);
	//@}

	//@{ 2006/08/22 burumal
	DDX_Text(pDX, IDC_LOOKAT_NODE, m_lLookAtNode);
	//@}
	
	DDX_Text(pDX, IDC_EDIT_RIDER_HEIGHT, m_unRiderHeight);
	DDX_Text(pDX, IDC_EDIT_CHARACTER_DEPTH, m_fDepth);

	//@{ 2006/08/28 burumal
	DDX_Check(pDX, IDC_USE_BENDING, m_bUseBending);

	DDX_Text(pDX, IDC_BENDING_FACTOR, m_fBendingFactor);
	DDV_MinMaxFloat(pDX, m_fBendingFactor, 0.0f, 2.0f);
	//@}

	//@{ 2006/09/08 burumal
	DDX_Check(pDX, IDC_CHECK_SELF_DESTRUCTION_ATTACK_TYPE, m_bSelfDestructionAttackType);
	//@}

	//@{ 2006/12/15 burumal
	DDX_Check(pDX, IDC_CHECK_NON_PICKING_TYPE, m_bNonPickingType);
	//@}
}


BEGIN_MESSAGE_MAP(CCharacterDlg, CDialog)
	//{{AFX_MSG_MAP(CCharacterDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET_ARMOUR_CLUMP, OnButtonSetArmourClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_BASE_CLUMP, OnButtonSetBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_CHARACTER_HEIGHT, OnButtonSetCharacterHeight)
	ON_BN_CLICKED(IDC_BUTTON_SET_CHARACTER_HIT_RANGE, OnButtonSetCharacterHitRange)
	ON_BN_CLICKED(IDC_BUTTON_SET_PICK_ATOMIC, OnButtonSetPickAtomic)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_ARMOUR_CLUMP, OnButtonRemoveArmourClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_BASE_CLUMP, OnButtonRemoveBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PICK_ATOMIC, OnButtonRemovePickAtomic)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_B_SPHERE, OnButtonGenerateBSphere)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM_FLAGS, OnButtonCustomFlags)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SET_RIDER_HEIGHT, OnBnClickedButtonSetRiderHeight)
	ON_BN_CLICKED(IDC_BUTTON_SET_CHARACTER_DEPTH, OnBnClickedButtonSetCharacterDepth)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterDlg message handlers

void CCharacterDlg::OnButtonSetArmourClump() 
{
	// TODO: Add your control notification handler code here
	if(AgcmCharacterDlg::GetInstance())
	{
		CHAR			*pszTemp;
		AgcmFileListDlg dlg;

		pszTemp = dlg.OpenFileList(	AgcmCharacterDlg::GetInstance()->GetFindPathName1(),
									AgcmCharacterDlg::GetInstance()->GetFindPathName2(),
									NULL													);
		if(pszTemp)
		{
			m_strArmourClump = pszTemp;
		}

		UpdateData(FALSE);
	}
}

void CCharacterDlg::OnButtonSetBaseClump() 
{
	// TODO: Add your control notification handler code here
	if(AgcmCharacterDlg::GetInstance())
	{
		CHAR			*pszTemp;
		AgcmFileListDlg dlg;

		pszTemp = dlg.OpenFileList(	AgcmCharacterDlg::GetInstance()->GetFindPathName1(),
									AgcmCharacterDlg::GetInstance()->GetFindPathName2(),
									NULL													);
		if(pszTemp)
		{
			m_strBaseClump = pszTemp;
		}

		UpdateData(FALSE);
	}
}
//. 2005. 10. 07 Nonstopdj
//. Depth값 Auto버튼으로 지정하기
void CCharacterDlg::OnBnClickedButtonSetCharacterDepth()
{
	if (AgcmCharacterDlg::GetInstance())
	{
		float fDepth = 0.0f;

		if (AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterData(AGCMCHARACTERDLG_CB_ID_SET_DEPTH, (PVOID)(&fDepth)))
		{
			m_fDepth = fDepth;
			UpdateData(FALSE);
		}
	}
}


void CCharacterDlg::OnButtonSetCharacterHeight() 
{
	if (AgcmCharacterDlg::GetInstance())
	{
		INT32 lHeight = 0;

//		if(AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterHeight(&lHeight))
		if (AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterData(AGCMCHARACTERDLG_CB_ID_SET_HEIGHT, (PVOID)(&lHeight)))
		{
			m_unHeight = lHeight;
			UpdateData(FALSE);
		}
	}
}

void CCharacterDlg::OnBnClickedButtonSetRiderHeight()
{
	if (AgcmCharacterDlg::GetInstance())
	{
		INT32 lHeight = 0;

		//		if(AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterHeight(&lHeight))
		if (AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterData(AGCMCHARACTERDLG_CB_ID_SET_HEIGHT, (PVOID)(&lHeight)))
		{
			m_unRiderHeight = lHeight;
			UpdateData(FALSE);
		}
	}
}

void CCharacterDlg::OnOK()
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	strcpy(m_pcsAgpdCharacterTemplate->m_szTName,		(LPSTR)(LPCSTR)(m_strTemplateName));	
	ReallocCopyString( &m_pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName, (char*)m_strBaseClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName, (char*)m_strArmourClump.GetBuffer() );
	ReallocCopyString( &m_pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName, (char*)m_strPickAtomic.GetBuffer() );

	m_pcsAgcdCharacterTemplate->m_lHeight					= m_unHeight;
	m_pcsAgcdCharacterTemplate->m_fDepth					= m_fDepth;
	m_pcsAgcdCharacterTemplate->m_lRiderHeight				= m_unRiderHeight;
	m_pcsAgcdCharacterTemplate->m_fScale					= m_fScale;
	m_pcsAgcdCharacterTemplate->m_stBSphere.center.x		= m_fBSphereCenterX;
	m_pcsAgcdCharacterTemplate->m_stBSphere.center.y		= m_fBSphereCenterY;
	m_pcsAgcdCharacterTemplate->m_stBSphere.center.z		= m_fBSphereCenterZ;
	m_pcsAgcdCharacterTemplate->m_stBSphere.radius			= m_fBSphereRadius;		
	m_pcsAgcdCharacterTemplate->m_bTagging					= m_bTagging;
	m_pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth		= m_fSiegeWarfareCollisionBoxWidth;
	m_pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight	= m_fSiegeWarfareCollisionBoxHeight;
	m_pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius	= m_fSiegeWarfareCollisionSphereRadius;
	m_pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX	= m_fSiegeWarfareCollisionOffsetX;
	m_pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ	= m_fSiegeWarfareCollisionOffsetZ;	
	m_pcsAgcdCharacterTemplate->m_lLookAtNode				= m_lLookAtNode; // template 값이 즉시 바뀐다.
	m_pcsAgcdCharacterTemplate->m_bUseBending				= m_bUseBending;
	m_pcsAgcdCharacterTemplate->m_fBendingFactor			= m_fBendingFactor;
	m_pcsAgpdCharacterTemplate->m_bSelfDestructionAttackType= m_bSelfDestructionAttackType;
	m_pcsAgcdCharacterTemplate->m_bNonPickingType			= m_bNonPickingType;

	for ( UINT32 uIdx = 0; uIdx < D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP; uIdx++ )
		m_pcsAgcdCharacterTemplate->m_lPickingNodeIndex[uIdx] = m_lNodeIndex[uIdx];

	if( !AgcmCharacterDlg::GetInstance()->GetAgpmFactors()->SetFactor(&m_pcsAgpdCharacterTemplate->m_csFactor, NULL, AGPD_FACTORS_TYPE_ATTACK) )
		MessageBox("Factor를 설정할 수 없습니다!", "ERROR");
	if( !AgcmCharacterDlg::GetInstance()->GetAgpmFactors()->SetValue(&m_pcsAgpdCharacterTemplate->m_csFactor, (INT32)(m_unHitRange), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE) )
		MessageBox("Factor를 설정할 수 없습니다!", "ERROR");
	
	CDialog::OnOK();
}

void CCharacterDlg::OnButtonSetCharacterHitRange() 
{
	// TODO: Add your control notification handler code here
	if (AgcmCharacterDlg::GetInstance())
	{
		INT32 lHitRange = 0;

//		if(AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterHitRange(&lHitRange))
		if (AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterData(AGCMCHARACTERDLG_CB_ID_SET_HIT_RANGE, (PVOID)(&lHitRange)))
		{
			m_unHitRange = lHitRange;
			UpdateData(FALSE);
		}
	}	
}

void CCharacterDlg::OnButtonSetPickAtomic() 
{
	// TODO: Add your control notification handler code here
	if(AgcmCharacterDlg::GetInstance())
	{
		CHAR			*pszTemp;
		AgcmFileListDlg dlg;

		pszTemp = dlg.OpenFileList(	AgcmCharacterDlg::GetInstance()->GetFindPathName1(),
									AgcmCharacterDlg::GetInstance()->GetFindPathName2(),
									NULL													);
		if(pszTemp)
		{
			m_strPickAtomic = pszTemp;
		}

		UpdateData(FALSE);
	}
}

void CCharacterDlg::OnButtonRemoveArmourClump() 
{
	// TODO: Add your control notification handler code here
	m_strArmourClump	= "";
	UpdateData(FALSE);	
}

void CCharacterDlg::OnButtonRemoveBaseClump() 
{
	// TODO: Add your control notification handler code here
	m_strBaseClump	= "";
	UpdateData(FALSE);
}

void CCharacterDlg::OnButtonRemovePickAtomic() 
{
	// TODO: Add your control notification handler code here
	m_strPickAtomic = "";
	UpdateData(FALSE);
}

// CCharacterDlg::~CCharacterDlg()
// {
// }

void CCharacterDlg::OnButtonGenerateBSphere() 
{
	// TODO: Add your control notification handler code here
	if (AgcmCharacterDlg::GetInstance())
	{
		RwSphere	stBSphere;

		if (AgcmCharacterDlg::GetInstance()->EnumCallbackSetCharacterData(AGCMCHARACTERDLG_CB_ID_SET_BSPHERE, (PVOID)(&stBSphere)))
		{
			m_fBSphereCenterX	= stBSphere.center.x;
			m_fBSphereCenterY	= stBSphere.center.y;
			m_fBSphereCenterZ	= stBSphere.center.z;
			m_fBSphereRadius	= stBSphere.radius;

			UpdateData(FALSE);
		}
	}
}

void CCharacterDlg::OnButtonCustomFlags() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmCharacterDlg::GetInstance())
		return;

	if (!AgcmCharacterDlg::GetInstance()->GetAgcmEventEffect())
		return;

	AgcdUseEffectSet	*pcsAgcdUseEffectSet	= AgcmCharacterDlg::GetInstance()->GetAgcmEventEffect()->GetUseEffectSet(m_pcsAgpdCharacterTemplate);
	if (!pcsAgcdUseEffectSet)
		return;

	CCharCustomFlagsDlg	dlg(&pcsAgcdUseEffectSet->m_ulCustomFlags);
	dlg.DoModal();
}


void CCharacterDlg::OnBnClickedCheck1()
{
}
