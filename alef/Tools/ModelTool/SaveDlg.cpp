// SaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "SaveDlg.h"
#include ".\savedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg dialog

CSaveDlg::CSaveDlg(CSaveDlgSet *pcsSet, CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveDlg)
	m_bCharacterTemplate	= pcsSet->m_bCharacerTemplate;
	m_bItemTemplate			= pcsSet->m_bItemTemplate;
	m_bObjectTemplate		= pcsSet->m_bObjectTemplate;
	m_bLODCharacter			= pcsSet->m_bLODCharacter;
	m_bLODItem				= pcsSet->m_bLODItem;
	m_bLODObject			= pcsSet->m_bLODObject;
	m_bSkillTemplate		= pcsSet->m_bSkillTemplate;
	m_bCharGenHeight		= pcsSet->m_bCharGenHeight;
	m_bCharGenHitRange		= pcsSet->m_bCharGenHitRange;
	m_bObjGenBSphere		= pcsSet->m_bObjGenBSphere;
	m_bItemGenBSphere		= pcsSet->m_bItemGenBSphere;
	m_bCharGenBSphere		= pcsSet->m_bCharGenBSphere;
	m_bGenLODBoundary		= pcsSet->m_bLODGenBoundary;
	m_bCheckLODLevel		= pcsSet->m_bLODCheckLevel;
	m_bSaveCharGeomData		= pcsSet->m_bCharGeometryData;
	m_bSaveItemGeomData		= pcsSet->m_bItemGeometryData;
	m_bSaveObjtGeomData		= pcsSet->m_bObjGeometryData;
	m_bSaveCharOctreeData	= pcsSet->m_bCharOcTreeData;
	m_bSaveItemOctreeData	= pcsSet->m_bItemOcTreeData;
	m_bSaveObjtOctreeData	= pcsSet->m_bObjtOcTreeData;
	m_bOldTypeSave			= pcsSet->m_bOldTypeSave;
	//}}AFX_DATA_INIT

	m_bSaveRideList			= pcsSet->m_bRideList;
	m_pcsSaveDlgSet = pcsSet;	
}


void CSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveDlg)
	DDX_Check(pDX, IDC_CHECK_SAVE_CHAR_TEMPLATE, m_bCharacterTemplate);
	DDX_Check(pDX, IDC_CHECK_SAVE_ITEM_TEMPLATE, m_bItemTemplate);
	DDX_Check(pDX, IDC_CHECK_SAVE_OBJT_TEMPLATE, m_bObjectTemplate);
	DDX_Check(pDX, IDC_CHECK_SAVE_LOD_CHARACTER, m_bLODCharacter);
	DDX_Check(pDX, IDC_CHECK_SAVE_LOD_ITEM, m_bLODItem);
	DDX_Check(pDX, IDC_CHECK_SAVE_LOD_OBJECT, m_bLODObject);
	DDX_Check(pDX, IDC_CHECK_SAVE_SKILL_TEMPLATE, m_bSkillTemplate);
	DDX_Check(pDX, IDC_CHECK_CHAR_GEN_HEIGHT, m_bCharGenHeight);
	DDX_Check(pDX, IDC_CHECK_CHAR_GEN_HIT_RANGE, m_bCharGenHitRange);
	DDX_Check(pDX, IDC_CHECK_OBJ_GEN_BSPHERE, m_bObjGenBSphere);
	DDX_Check(pDX, IDC_CHECK_ITEM_GEN_BSPHERE, m_bItemGenBSphere);
	DDX_Check(pDX, IDC_CHECK_CHAR_GEN_BSPHERE, m_bCharGenBSphere);
	DDX_Check(pDX, IDC_CHECK_GEN_LOD_BOUNDARY, m_bGenLODBoundary);
	DDX_Check(pDX, IDC_CHECK_CHECK_LEVEL, m_bCheckLODLevel);
	DDX_Check(pDX, IDC_CHECK_SAVE_CHAR_GEOM_DATA, m_bSaveCharGeomData);
	DDX_Check(pDX, IDC_CHECK_SAVE_ITEM_GEOM_DATA, m_bSaveItemGeomData);
	DDX_Check(pDX, IDC_CHECK_SAVE_OBJT_GEOM_DATA, m_bSaveObjtGeomData);
	DDX_Check(pDX, IDC_CHECK_SAVE_CHAR_OCTREE_DATA, m_bSaveCharOctreeData);
	DDX_Check(pDX, IDC_CHECK_SAVE_ITEM_OCTREE_DATA, m_bSaveItemOctreeData);
	DDX_Check(pDX, IDC_CHECK_SAVE_OBJT_OCTREE_DATA, m_bSaveObjtOctreeData);
	DDX_Check(pDX, IDC_OLDTYPESAVE, m_bOldTypeSave);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_SAVE_RIDE_LIST, m_bSaveRideList);

	//@{ 2006/05/03 burumal
	DDX_Control(pDX, IDC_NCODE_COMBO, m_comboNationalCode);

	if ( CModelToolApp::GetInstance() ) 
	{
		UINT nCurSel = m_comboNationalCode.GetCurSel();
		CModelToolApp::GetInstance()->SetNationalCode(nCurSel);		
	}
	//@}
	DDX_Control(pDX, IDC_SAVE_LIST, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CSaveDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_ALL, OnButtonSaveCheckAll)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_LOD, OnButtonSaveCheckLod)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_TEMPLATE, OnButtonSaveCheckTemplate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_CHARACTER, OnButtonSaveCheckCharacter)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_Item, OnBUTTONSAVECHECKItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_OBJECT, OnButtonSaveCheckObject)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_SKILL, OnButtonSaveCheckSkill)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_BSPHERE, OnButtonSaveCheckBsphere)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_GEOMETRY, OnButtonSaveCheckGeometry)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_CHECK_OCTREE, OnButtonSaveCheckOctree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveDlg message handlers

void CSaveDlg::OnButtonSaveCheckAll()
{
	m_bCharacterTemplate	= TRUE;
	m_bItemTemplate			= TRUE;
	m_bObjectTemplate		= TRUE;
	m_bSkillTemplate		= TRUE;

	m_bLODCharacter			= TRUE;
	m_bLODItem				= TRUE;
	m_bLODObject			= TRUE;

	m_bCharGenHeight		= TRUE;
	m_bCharGenHitRange		= TRUE;

	m_bCharGenBSphere		= TRUE;
	m_bItemGenBSphere		= TRUE;
	m_bObjGenBSphere		= TRUE;

	m_bSaveCharGeomData		= TRUE;
	m_bSaveItemGeomData		= TRUE;
	m_bSaveObjtGeomData		= TRUE;

	m_bSaveCharOctreeData	= TRUE;
	m_bSaveItemOctreeData	= TRUE;
	m_bSaveObjtOctreeData	= TRUE;
	m_bSaveRideList			= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnButtonSaveCheckLod()
{
	UpdateData(TRUE);

	m_bLODCharacter			= TRUE;
	m_bLODItem				= TRUE;
	m_bLODObject			= TRUE;
	m_bGenLODBoundary		= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnButtonSaveCheckTemplate()
{
	UpdateData(TRUE);

	m_bCharacterTemplate	= TRUE;
	m_bItemTemplate			= TRUE;
	m_bObjectTemplate		= TRUE;
	m_bSkillTemplate		= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnOK()
{
	UpdateData(TRUE);

	m_pcsSaveDlgSet->m_bCharacerTemplate	= m_bCharacterTemplate;
	m_pcsSaveDlgSet->m_bItemTemplate		= m_bItemTemplate;
	m_pcsSaveDlgSet->m_bObjectTemplate		= m_bObjectTemplate;
	m_pcsSaveDlgSet->m_bSkillTemplate		= m_bSkillTemplate;

	m_pcsSaveDlgSet->m_bLODCharacter		= m_bLODCharacter;
	m_pcsSaveDlgSet->m_bLODItem				= m_bLODItem;
	m_pcsSaveDlgSet->m_bLODObject			= m_bLODObject;

	m_pcsSaveDlgSet->m_bLODGenBoundary		= m_bGenLODBoundary;
	m_pcsSaveDlgSet->m_bLODCheckLevel		= m_bCheckLODLevel;

	m_pcsSaveDlgSet->m_bCharGenHeight		= m_bCharGenHeight;
	m_pcsSaveDlgSet->m_bCharGenHitRange		= m_bCharGenHitRange;

	m_pcsSaveDlgSet->m_bCharGenBSphere		= m_bCharGenBSphere;
	m_pcsSaveDlgSet->m_bItemGenBSphere		= m_bItemGenBSphere;
	m_pcsSaveDlgSet->m_bObjGenBSphere		= m_bObjGenBSphere;

	m_pcsSaveDlgSet->m_bCharGeometryData	= m_bSaveCharGeomData;
	m_pcsSaveDlgSet->m_bItemGeometryData	= m_bSaveItemGeomData;
	m_pcsSaveDlgSet->m_bObjGeometryData		= m_bSaveObjtGeomData;

	m_pcsSaveDlgSet->m_bCharOcTreeData		= m_bSaveCharOctreeData;
	m_pcsSaveDlgSet->m_bItemOcTreeData		= m_bSaveItemOctreeData;
	m_pcsSaveDlgSet->m_bObjtOcTreeData		= m_bSaveObjtOctreeData;

	m_pcsSaveDlgSet->m_bOldTypeSave			= m_bOldTypeSave;
	
	m_pcsSaveDlgSet->m_bRideList			= m_bSaveRideList;

	// 리스트 컨트롤에서 세이브할 템플릿을 뽑아낸다.
	{
		int nSeq = 0;
		while( m_ListCtrl.GetItemData( nSeq ) )
		{
			if( m_ListCtrl.GetItemCheckedState( nSeq , COLUMN_SAVE ) )
			{
				ApBase * pBase = ( ApBase * ) m_ListCtrl.GetItemData( nSeq );
				switch( pBase->m_eType )
				{
				case APBASE_TYPE_CHARACTER_TEMPLATE	:
					{
						AgpdCharacterTemplate * pcsTemplate = (AgpdCharacterTemplate*) pBase;
						m_pcsSaveDlgSet->vecCharacterTemplate.push_back( pcsTemplate );
					}
					break;
				case APBASE_TYPE_OBJECT_TEMPLATE	:
					{
						ApdObjectTemplate * pcsTemplate = (ApdObjectTemplate*) pBase;
						m_pcsSaveDlgSet->vecObjectTemplate.push_back( pcsTemplate );
					}
					break;
				case APBASE_TYPE_ITEM_TEMPLATE		:
					{
						AgpdItemTemplate * pcsTemplate = (AgpdItemTemplate*) pBase;
						m_pcsSaveDlgSet->vecItemTemplate.push_back( pcsTemplate );
					}
					break;
				}
			}
			nSeq ++;
		}
	}

	CDialog::OnOK();
}

void CSaveDlg::OnButtonSaveCheckCharacter()
{
	UpdateData(TRUE);

	m_bCharacterTemplate	= TRUE;
	m_bLODCharacter			= TRUE;
	m_bCharGenHeight		= TRUE;
	m_bCharGenHitRange		= TRUE;
	m_bCharGenBSphere		= TRUE;
	m_bSaveCharGeomData		= TRUE;
	m_bSaveCharOctreeData	= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnBUTTONSAVECHECKItem()
{
	UpdateData(TRUE);

	m_bItemTemplate			= TRUE;
	m_bLODItem				= TRUE;
	m_bItemGenBSphere		= TRUE;
	m_bSaveItemGeomData		= TRUE;
	m_bSaveItemOctreeData	= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnButtonSaveCheckObject()
{
	UpdateData(TRUE);

	m_bObjectTemplate		= TRUE;
	m_bLODObject			= TRUE;
	m_bObjGenBSphere		= TRUE;
	m_bSaveObjtGeomData		= TRUE;
	m_bSaveObjtOctreeData	= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnButtonSaveCheckSkill()
{
	UpdateData(TRUE);

	m_bSkillTemplate		= TRUE;

	UpdateData(FALSE);
}

void CSaveDlg::OnButtonSaveCheckBsphere()
{
	m_bCharGenBSphere		= TRUE;
	m_bItemGenBSphere		= TRUE;
	m_bObjGenBSphere		= TRUE;
}

void CSaveDlg::OnButtonSaveCheckGeometry()
{
	m_bSaveCharOctreeData	= TRUE;
	m_bSaveItemOctreeData	= TRUE;
	m_bSaveObjtOctreeData	= TRUE;
}

void CSaveDlg::OnButtonSaveCheckOctree()
{
	m_bSaveCharOctreeData	= TRUE;
	m_bSaveItemOctreeData	= TRUE;
	m_bSaveObjtOctreeData	= TRUE;
}

BOOL CSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//@{ 2006/05/03 burumal	
	for ( int nIdx = 0; nIdx < nMAX_NATIONAL_CODES; nIdx++ )
		m_comboNationalCode.InsertString(nIdx, g_pNationalCodeStr[nIdx]);
	
	UINT uNationalCode = (CModelToolApp::GetInstance()) ? CModelToolApp::GetInstance()->GetNationalCode() : 0;
	if ( uNationalCode >= nMAX_NATIONAL_CODES )	
		uNationalCode = 0;

	m_comboNationalCode.SetCurSel(uNationalCode);
	//@}


	// List Control 초기화
	DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ListCtrl.SetExtendedStyle(dwExStyle);

	InitListControl();
	ListingUnsavedData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CSaveDlg::InitListControl()
{
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);

	int	total_cx = 0;
	LV_COLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "Type";
	lvcolumn.iSubItem = COLUMN_TYPE;
	lvcolumn.cx = 45;

	m_ListCtrl.InsertColumn(COLUMN_TYPE, &lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "TID";
	lvcolumn.iSubItem = COLUMN_TID;
	lvcolumn.cx = 64;

	m_ListCtrl.InsertColumn(COLUMN_TID, &lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "Name";
	lvcolumn.iSubItem = COLUMN_NAME;
	lvcolumn.cx = 231;

	m_ListCtrl.InsertColumn(COLUMN_NAME, &lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "Save";
	lvcolumn.iSubItem = COLUMN_SAVE;
	lvcolumn.cx = 49;

	m_ListCtrl.InsertColumn(COLUMN_SAVE, &lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;
	lvcolumn.pszText = "State";
	lvcolumn.iSubItem = COLUMN_STATE;
	lvcolumn.cx = 87;

	m_ListCtrl.InsertColumn(COLUMN_STATE, &lvcolumn);
}

void	CSaveDlg::ListingUnsavedData()
{
	CModelToolApp *pApp = CModelToolApp::GetInstance();

	int	nSeq = 0;

	ForEachCharacterTemplate	csForEachCharacter	( *pApp->GetEngine() , m_ListCtrl , nSeq );
	// ForEachItemTemplate			csForEachItem		( *pApp->GetEngine() , m_ListCtrl , nSeq );
	ForEachObjectTemplate		csForEachObject		( *pApp->GetEngine() , m_ListCtrl , nSeq );

	csForEachCharacter();
	// csForEachItem();
	csForEachObject();
}

template< typename T >
BOOL ForEachTemplate< T >::_DoForEach( T * pcsTemplate )
{
	CModelToolApp *pApp = CModelToolApp::GetInstance();
	AgcEngineChild::stSaveDataInfo	* pSaveData = m_csModuleManager.GetAttachedData( pcsTemplate );

	int	nItem = _nSequence;

	if( AgcEngineChild::SF_NOT_CHANGED == pSaveData->GetState() ) return TRUE;

	// COLUMN_TYPE
	_csCtrl.InsertItem( _nSequence , GetTypeName() , GetTypeColor() , RGB( 255 , 255 ,255 ) );

	// COLUMN_TID
	CString	str;
	str.Format( "%06d" , pcsTemplate->GetID() );
	_csCtrl.SetItemText( _nSequence , CSaveDlg::COLUMN_TID , str );

	// COLUMN_NAME
	_csCtrl.SetItemText( _nSequence , CSaveDlg::COLUMN_NAME , pcsTemplate->GetName() );

	// COLUMN_SAVE
	_csCtrl.SetCheckbox( _nSequence , CSaveDlg::COLUMN_SAVE , TRUE );

	// COLUMN_STATE
	switch( pSaveData->GetState() )
	{
	case	AgcEngineChild::SF_CHANGED		:
		_csCtrl.SetItemText( _nSequence , CSaveDlg::COLUMN_STATE , "Changed" , RGB( 255 , 128 , 128 ) , RGB( 255 , 255 ,255 ) );
		break;
	case	AgcEngineChild::SF_SAVED		:
		_csCtrl.SetItemText( _nSequence , CSaveDlg::COLUMN_STATE , "Saved" , RGB( 128 , 128 , 255 ) , RGB( 255 , 255 ,255 ) );
		break;
	}	
	
	// 포인터를 기록해둠.
	_csCtrl.SetItemData( _nSequence , PtrToInt( pcsTemplate ) );

	_nSequence++;
	return TRUE;
}
