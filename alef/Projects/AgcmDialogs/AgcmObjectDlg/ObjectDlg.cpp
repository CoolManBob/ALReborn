// ObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmObjectDlg.h"
#include "ObjectDlg.h"
#include "AgcmFileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectDlg dialog


CObjectDlg::CObjectDlg(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate, CWnd* pParent )
	: CDialog(CObjectDlg::IDD, pParent)
{
	ASSERT( AgcmObjectDlg::GetInstance() );

	//{{AFX_DATA_INIT(CObjectDlg)
	m_strBaseClump			= _T("");
	m_strCategory			= _T(pcsAgcdObjectTemplate->m_szCategory);
	m_strCollisionClump		= _T(pcsAgcdObjectTemplate->m_szCollisionDFFName);
	m_strTemplateName		= _T(pcsApdObjectTemplate->m_szName);
	m_strInitAnimName		= _T("");
	m_fAnimSpeed			= 0;
	m_lGroupIndex			= -1;
	m_strPickClump			= _T(pcsAgcdObjectTemplate->m_szPickDFFName);
	m_fBSphereCenterX		= pcsAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_stBSphere.center.x;
	m_fBSphereCenterY		= pcsAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_stBSphere.center.y;
	m_fBSphereCenterZ		= pcsAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_stBSphere.center.z;
	m_fBSphereRadius		= pcsAgcdObjectTemplate->m_stGroup.m_pstList->m_csData.m_stBSphere.radius;
	//}}AFX_DATA_INIT

	m_pcsAgcmObject			= AgcmObjectDlg::GetInstance()->GetAgcmObject();
	m_pcsAgcmObjectList		= m_pcsAgcmObject->GetObjectList();

	m_pcsApdObjectTemplate	= pcsApdObjectTemplate;
	m_pcsAgcdObjectTemplate	= pcsAgcdObjectTemplate;
	m_lPreGroupIndex		= -1;

	AgcmObjectDlg::GetInstance()->GetAgcmObject()->CopyObjectTemplateGroup(m_pcsApdObjectTemplate->m_lID, &m_stGroup, &pcsAgcdObjectTemplate->m_stGroup);
}

void CObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectDlg)
	DDX_Text(pDX, IDC_EDIT_BASE_CLUMP, m_strBaseClump);
	DDX_Text(pDX, IDC_EDIT_CATEGORY, m_strCategory);
	DDX_Text(pDX, IDC_EDIT_COLLISION_CLUMP, m_strCollisionClump);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strTemplateName);
	DDX_Text(pDX, IDC_EDIT_INIT_ANIM, m_strInitAnimName);
	DDX_Text(pDX, IDC_EDIT_INIT_ANIM_SPEED, m_fAnimSpeed);
	DDX_Text(pDX, IDC_EDIT_PICK_CLUMP, m_strPickClump);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_X, m_fBSphereCenterX);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_Y, m_fBSphereCenterY);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_CENTER_Z, m_fBSphereCenterZ);
	DDX_Text(pDX, IDC_EDIT_B_SPHERE_RADIUS, m_fBSphereRadius);
	DDX_CBIndex(pDX, IDC_COMBO_GROUP_INDEX, m_lGroupIndex);
	DDX_Control(pDX, IDC_COMBO_RIDABLE_MATERIAL_TYPE, m_ctrRidableMaterialType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET_BASE_CLUMP, OnButtonSetBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_CATEGORY, OnButtonSetCategory)
	ON_BN_CLICKED(IDC_BUTTON_SET_COLLISION_CLUMP, OnButtonSetCollisionClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_INIT_ANIM, OnButtonSetInitAnim)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_BASE_CLUMP, OnButtonRemoveBaseClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_COLLISION_CLUMP, OnButtonRemoveCollisionClump)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_INIT_ANIM, OnButtonRemoveInitAnim)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_ADD, OnButtonGroupAdd)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_REMOVE, OnButtonGroupRemove)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_PICK_CLUMP, OnButtonRemovePickClump)
	ON_BN_CLICKED(IDC_BUTTON_SET_PICK_CLUMP, OnButtonSetPickClump)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_B_SPHERE, OnButtonGenerateBSphere)
	ON_CBN_SELCHANGE(IDC_COMBO_GROUP_INDEX, OnSelchangeComboGroupIndex)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDlg message handlers

void CObjectDlg::OnButtonSetBaseClump() 
{
	// TODO: Add your control notification handler code here
	AgcmFileListDlg dlg;
	CHAR* pszTemp = dlg.OpenFileList( AgcmObjectDlg::GetInstance()->GetClumpPathName1(), AgcmObjectDlg::GetInstance()->GetClumpPathName2(), NULL );
	if( pszTemp )
	{
		UpdateData(TRUE);

		AgcdObjectTemplateGroupData* pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lGroupIndex);
		if( !pcsData )		return;

		ReallocCopyString( &pcsData->m_pszDFFName, pszTemp );

		m_strBaseClump = pszTemp;
		UpdateData(FALSE);
	}
}

void CObjectDlg::OnButtonSetCategory() 
{
	CHAR szTemp[256];
	if( AgcmObjectDlg::GetInstance()->OpenObjectCategory( szTemp ) )
	{
		m_strCategory = szTemp;
		UpdateData(FALSE);
	}
}

void CObjectDlg::OnButtonSetCollisionClump() 
{
	AgcmFileListDlg dlg;
	CHAR* pszTemp = dlg.OpenFileList( AgcmObjectDlg::GetInstance()->GetClumpPathName1(), AgcmObjectDlg::GetInstance()->GetClumpPathName2(), NULL );
	if( pszTemp )
	{
		m_strCollisionClump = pszTemp;
		UpdateData( FALSE );
	}
}

void CObjectDlg::OnOK()
{
	UpdateData(TRUE);

	AgcdObjectTemplateGroupData* pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lGroupIndex);
	if ( !pcsData )		return;

	UpdateGroupData();

	pcsData->m_fAnimSpeed	= m_fAnimSpeed;
	AgcdObjectTemplateGroup* pstDestGroup = &m_pcsAgcdObjectTemplate->m_stGroup;

	AgcdObjectTemplateGroupData	*pcsDestData;
	INT32						lMax					= (pstDestGroup->m_lNum > m_stGroup.m_lNum) ? (pstDestGroup->m_lNum) : (m_stGroup.m_lNum);
	for (INT32 lIndex = 0; lIndex < lMax; ++lIndex)
	{
		pcsData		= m_pcsAgcmObjectList->GetObjectTemplateGroup( &m_stGroup, lIndex );
		pcsDestData	= m_pcsAgcmObjectList->GetObjectTemplateGroup( pstDestGroup, lIndex );

		if (!pcsDestData)
			pcsDestData = m_pcsAgcmObjectList->AddObjectTemplateGroup(pstDestGroup);
		if (!pcsDestData)
			return;

		if (pcsData)
			m_pcsAgcmObject->CopyObjectTemplateGroupData(m_pcsApdObjectTemplate->m_lID, pcsDestData, pcsData);
		else
			m_pcsAgcmObjectList->RemoveObjectTemplateGroup(pstDestGroup, lIndex);
	}

	strcpy(m_pcsApdObjectTemplate->m_szName, m_strTemplateName);
	strcpy(m_pcsAgcdObjectTemplate->m_szCategory, m_strCategory);
	strcpy(m_pcsAgcdObjectTemplate->m_szCollisionDFFName, m_strCollisionClump);
	strcpy(m_pcsAgcdObjectTemplate->m_szPickDFFName, m_strPickClump);

	m_pcsAgcdObjectTemplate->m_eRidableMaterialType = (eApmMapMaterial)m_ctrRidableMaterialType.GetCurSel();

	CDialog::OnOK();
}

void CObjectDlg::OnButtonSetInitAnim() 
{
	AgcmFileListDlg dlg;
	CHAR* pszTemp = dlg.OpenFileList( AgcmObjectDlg::GetInstance()->GetAnimationPathName1(), AgcmObjectDlg::GetInstance()->GetAnimationPathName2(), NULL );
	if( pszTemp )
	{
		UpdateData(TRUE);

		AgcdObjectTemplateGroupData* pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup( &m_stGroup, m_lGroupIndex );
		if( !pcsData )		return;

		AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;
		csRemoveAllAnimationParams.m_ppSrcAnimation	= &pcsData->m_pcsAnimation;
		csRemoveAllAnimationParams.m_bObject		= TRUE;
		AgcmObjectDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);
		if( !csRemoveAllAnimationParams.m_bRemovedAnimation )
			return;

		AAD_AddAnimationParams			csAddAnimationParams;
		csAddAnimationParams.m_ppSrcAnimation	= &pcsData->m_pcsAnimation;
		csAddAnimationParams.m_pszSrcAnimation	= pszTemp;
		csAddAnimationParams.m_bObject			= TRUE;
		AgcmObjectDlg::GetInstance()->AddAnimation(&csAddAnimationParams);
		if( !csAddAnimationParams.m_pDestData )		return;
			
		AAD_ReadRtAnimParams			csReadRtAnimParams;
		csReadRtAnimParams.m_pcsSrcAnimData	= csAddAnimationParams.m_pDestData;
		csReadRtAnimParams.m_bObject		= TRUE;

		AgcmObjectDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);
		if( !csReadRtAnimParams.m_bRead )			return;
		
		m_strInitAnimName = pszTemp;

		UpdateData(FALSE);
	}
}

void CObjectDlg::OnButtonRemoveBaseClump() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	AgcdObjectTemplateGroupData* pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lGroupIndex);
	if( !pcsData )		return;

	if (pcsData->m_pszDFFName)
	{
		delete [] pcsData->m_pszDFFName;
		pcsData->m_pszDFFName	= NULL;
	}

	m_strBaseClump	= "";

	UpdateData(FALSE);
}

void CObjectDlg::OnButtonRemoveCollisionClump() 
{
	m_strCollisionClump	= "";
	UpdateData(FALSE);
}

void CObjectDlg::OnButtonRemoveInitAnim() 
{
	UpdateData(TRUE);

	AgcdObjectTemplateGroupData* pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lGroupIndex);
	if( !pcsData )		return;

	AAD_RemoveAllAnimationParams	csRemoveAllAnimationParams;
	csRemoveAllAnimationParams.m_ppSrcAnimation	= &pcsData->m_pcsAnimation;
	csRemoveAllAnimationParams.m_bObject		= TRUE;
	AgcmObjectDlg::GetInstance()->RemoveAllAnimation(&csRemoveAllAnimationParams);

	if (!csRemoveAllAnimationParams.m_bRemovedAnimation)
		return;

	m_strInitAnimName	= "";

	UpdateData(FALSE);
}

void CObjectDlg::OnButtonGroupAdd() 
{
	AgcdObjectTemplateGroupData* pstGroupData = m_pcsAgcmObjectList->AddObjectTemplateGroup(&m_stGroup);
	if( !pstGroupData )		return;

	InitializeGroupIndex(TRUE);
	UpdateGroupData();
}

void CObjectDlg::OnButtonGroupRemove() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if( !m_pcsAgcmObjectList->RemoveObjectTemplateGroup(&m_stGroup, m_lGroupIndex) )
		return;

	InitializeGroupIndex();
	UpdateGroupData();
}

void CObjectDlg::OnSelchangeComboGroupIndex() 
{
	// TODO: Add your control notification handler code here
	UpdateGroupData();
}

VOID CObjectDlg::InitializeGroupIndex(BOOL	bSetMax)
{
	CComboBox* pstComboBox = (CComboBox *)(GetDlgItem(IDC_COMBO_GROUP_INDEX));
	if( !pstComboBox )		return;

	pstComboBox->ResetContent();

	AgcdObjectTemplateGroupList* pstList = m_stGroup.m_pstList;
	if (!pstList)
	{
		if (!AgcmObjectDlg::GetInstance()->GetObjectList()->AddObjectTemplateGroup(&m_stGroup))
			return;
	}

	for (INT32 lIndex = 0;; ++lIndex)
	{
		AgcdObjectTemplateGroupData* pstData = AgcmObjectDlg::GetInstance()->GetObjectList()->GetObjectTemplateGroup(&m_stGroup, lIndex);
		if ( !pstData )		break;
		
		CHAR szTemp[256];
		sprintf(szTemp, "%d", lIndex);
		pstComboBox->InsertString(lIndex, szTemp);
	}
	
	if (pstComboBox->GetCount() > 0)
		pstComboBox->SetCurSel( bSetMax ? pstComboBox->GetCount() - 1 : 0 );
}

VOID CObjectDlg::UpdateGroupData()
{
	UpdateData(TRUE);

	if (m_lGroupIndex < 0)
	{
		EnableGroupControl(FALSE);
		return;
	}

	EnableGroupControl(TRUE);
	
	AgcdObjectTemplateGroupData* pcsData;

	if (m_lPreGroupIndex > -1)
	{
		pcsData	= m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lPreGroupIndex);
		if( pcsData )
		{
			pcsData->m_fAnimSpeed			= m_fAnimSpeed;
			pcsData->m_stBSphere.center.x	= m_fBSphereCenterX;
			pcsData->m_stBSphere.center.y	= m_fBSphereCenterY;
			pcsData->m_stBSphere.center.z	= m_fBSphereCenterZ;
			pcsData->m_stBSphere.radius		= m_fBSphereRadius;
		}
	}

	pcsData = m_pcsAgcmObjectList->GetObjectTemplateGroup(&m_stGroup, m_lGroupIndex);
	if( !pcsData )		return;
		
	m_strBaseClump = _T(pcsData->m_pszDFFName) ? _T(pcsData->m_pszDFFName) : _T("");
		
	if( pcsData->m_pcsAnimation &&
		pcsData->m_pcsAnimation->m_pcsHead &&
		pcsData->m_pcsAnimation->m_pcsHead->m_pszRtAnimName )
	{
		m_strInitAnimName	= pcsData->m_pcsAnimation->m_pcsHead->m_pszRtAnimName;
	}
	else
	{
		m_strInitAnimName = "";
	}

	m_fAnimSpeed		= pcsData->m_fAnimSpeed;
	m_fBSphereCenterX	= pcsData->m_stBSphere.center.x;
	m_fBSphereCenterY	= pcsData->m_stBSphere.center.y;
	m_fBSphereCenterZ	= pcsData->m_stBSphere.center.z;
	m_fBSphereRadius	= pcsData->m_stBSphere.radius;

	UpdateData(FALSE);

	m_lPreGroupIndex = m_lGroupIndex;
}

VOID CObjectDlg::EnableGroupControl(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_SET_BASE_CLUMP)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_REMOVE_BASE_CLUMP)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_SET_INIT_ANIM)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_REMOVE_INIT_ANIM)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_INIT_ANIM_SPEED)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_GROUP_INDEX)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_GROUP_REMOVE)->EnableWindow(bEnable);
}

void CObjectDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL CObjectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	InitializeGroupIndex();
	UpdateGroupData();

	m_ctrRidableMaterialType.AddString("SOIL");
	m_ctrRidableMaterialType.AddString("SWAMP");
	m_ctrRidableMaterialType.AddString("GRASS");
	m_ctrRidableMaterialType.AddString("SAND");
	m_ctrRidableMaterialType.AddString("LEAF");
	m_ctrRidableMaterialType.AddString("SNOW");
	m_ctrRidableMaterialType.AddString("WATER");
	m_ctrRidableMaterialType.AddString("STONE");
	m_ctrRidableMaterialType.AddString("WOOD");
	m_ctrRidableMaterialType.AddString("METAL");
	m_ctrRidableMaterialType.AddString("BONE");

	m_ctrRidableMaterialType.SetCurSel( m_pcsAgcdObjectTemplate->m_eRidableMaterialType );

	return TRUE;
}

void CObjectDlg::OnButtonRemovePickClump() 
{
	// TODO: Add your control notification handler code here
	m_strPickClump	= "";
	UpdateData(FALSE);
}

void CObjectDlg::OnButtonSetPickClump() 
{
	// TODO: Add your control notification handler code here
	AgcmFileListDlg dlg;
	CHAR* pszTemp = dlg.OpenFileList( AgcmObjectDlg::GetInstance()->GetClumpPathName1(), AgcmObjectDlg::GetInstance()->GetClumpPathName2(), NULL );
	if( pszTemp )
	{
		UpdateData(TRUE);

		strcpy( m_pcsAgcdObjectTemplate->m_szPickDFFName, pszTemp );
		m_strPickClump = pszTemp;

		UpdateData(FALSE);
	}
}

void CObjectDlg::OnButtonGenerateBSphere() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if( m_lGroupIndex < 0 )
		return;

	RwSphere	stBSphere;
	if (!AgcmObjectDlg::GetInstance()->EnumCallbackSetObjectData(AGCMOBJECTDLG_CB_ID_SET_BSPHERE, (PVOID)(&stBSphere), (PVOID)(m_lGroupIndex)))
		return;

	m_fBSphereCenterX	= stBSphere.center.x;
	m_fBSphereCenterY	= stBSphere.center.y;
	m_fBSphereCenterZ	= stBSphere.center.z;
	m_fBSphereRadius	= stBSphere.radius;

	UpdateData(FALSE);
}