// CreateObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "CreateObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void NewAgcdPreCharacterTemplate( AgcdPreCharacterTemplate* pcsPreData )
{
	if( pcsPreData == NULL ) return;

	if( pcsPreData->m_pszLabel == NULL )
		pcsPreData->m_pszLabel		= new CHAR[256];

	if( pcsPreData->m_pszDADFFName == NULL ) 
		pcsPreData->m_pszDADFFName	= new CHAR[256];

	if( pcsPreData->m_pszDFFName == NULL )
		pcsPreData->m_pszDFFName	= new CHAR[256];

	if( pcsPreData->m_pszPickDFFName == NULL )
		pcsPreData->m_pszPickDFFName= new CHAR[256];

	ZeroMemory( pcsPreData->m_pszLabel,			256 );
	ZeroMemory( pcsPreData->m_pszDADFFName,		256 );
	ZeroMemory( pcsPreData->m_pszDFFName,		256 );
	ZeroMemory( pcsPreData->m_pszPickDFFName,	256 );
}

void NewAgcdPreItemTemplate( AgcdPreItemTemplate* pAgcdPreItemTemplate )
{
	if( pAgcdPreItemTemplate == NULL ) return;

	if( pAgcdPreItemTemplate->m_pszLabel == NULL )
		pAgcdPreItemTemplate->m_pszLabel		= new CHAR[256];

	if( pAgcdPreItemTemplate->m_pszBaseDFFName == NULL )
		pAgcdPreItemTemplate->m_pszBaseDFFName	= new CHAR[256];

	if( pAgcdPreItemTemplate->m_pszSecondDFFName == NULL )
		pAgcdPreItemTemplate->m_pszSecondDFFName= new CHAR[256];

	if( pAgcdPreItemTemplate->m_pszFieldDFFName == NULL )
		pAgcdPreItemTemplate->m_pszFieldDFFName	= new CHAR[256];

	if( pAgcdPreItemTemplate->m_pszPickDFFName == NULL )
		pAgcdPreItemTemplate->m_pszPickDFFName	= new CHAR[256];

	ZeroMemory( pAgcdPreItemTemplate->m_pszLabel,			256 );
	ZeroMemory( pAgcdPreItemTemplate->m_pszBaseDFFName,		256 );
	ZeroMemory( pAgcdPreItemTemplate->m_pszSecondDFFName,	256 );
	ZeroMemory( pAgcdPreItemTemplate->m_pszFieldDFFName,	256 );
	ZeroMemory( pAgcdPreItemTemplate->m_pszPickDFFName,		256 );
}

/////////////////////////////////////////////////////////////////////////////
// CCreateObjectDlg dialog

CCreateObjectDlg::CCreateObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateObjectDlg::IDD, pParent)
{
	InitializeMember();

	//{{AFX_DATA_INIT(CCreateObjectDlg)
	m_strCOName					= _T("");
	m_strID						= _T("");
	m_bMakeTemporaryTemplate	= FALSE;
	//}}AFX_DATA_INIT
}


void CCreateObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateObjectDlg)
	DDX_Control(pDX, IDC_BUTTON_CO_PROPERTY, m_csCOProperty);
	DDX_Control(pDX, IDC_COMBO_CO_TYPE, m_csCOType);
	DDX_Text(pDX, IDC_EDIT_CO_NAME, m_strCOName);
	DDX_Text(pDX, IDC_EDIT_CO_ID, m_strID);
	DDX_Check(pDX, IDC_CHECK_MAKE_TEMPORARY_TEMPLATE, m_bMakeTemporaryTemplate);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CCreateObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateObjectDlg)
	ON_BN_CLICKED(IDC_BUTTON_CO_MORE, OnButtonCoMore)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_CO_TYPE, &CCreateObjectDlg::OnCbnSelchangeComboCoType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateObjectDlg message handlers

void UpdateID( CString& strID, DWORD dwID )
{
	char szText[128];
	sprintf( szText, "%d", dwID );
	strID = szText;
}

BOOL CCreateObjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_csCOType.InsertString(COD_TYPE_NAME_CHARACTER_INDEX,	COD_TYPE_NAME_CHARACTER);
	m_csCOType.InsertString(COD_TYPE_NAME_ITEM_INDEX,		COD_TYPE_NAME_ITEM);
	m_csCOType.InsertString(COD_TYPE_NAME_OBJECT_INDEX,		COD_TYPE_NAME_OBJECT);
	m_csCOType.InsertString(COD_TYPE_NAME_SKILL_INDEX,		COD_TYPE_NAME_SKILL);
	m_csCOType.SetCurSel(COD_TYPE_NAME_CHARACTER_INDEX);

	UpdateID( m_strID, CModelToolApp::GetInstance()->GenerateCTID( 0 ) );
	UpdateData(FALSE);
	return TRUE;
}

void CCreateObjectDlg::OnOK()
{
	UpdateData(TRUE);

	if(m_strCOName == "")
	{
		MessageBox("Name을 설정하세요!", "Error");
		return;
	}

	DWORD dwID = atoi( m_strID.GetString() );

	switch(m_csCOType.GetCurSel())
	{
	case COD_TYPE_NAME_CHARACTER_INDEX:
		{
			AgpdCharacterTemplate *pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *)(m_pvAgpdCharacterTemplate);
			AgcdCharacterTemplate *pcsAgcdCharacterTemplate = (AgcdCharacterTemplate *)(m_pvAgcdCharacterTemplate);

			if( pcsAgcdCharacterTemplate->m_pcsPreData == NULL ) {
				pcsAgcdCharacterTemplate->m_pcsPreData = new AgcdPreCharacterTemplate;
				NewAgcdPreCharacterTemplate( pcsAgcdCharacterTemplate->m_pcsPreData );
			}

			if (!SetBase(pcsAgpdCharacterTemplate->m_szTName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName))
				return;

			if( CModelToolApp::GetInstance()->AddCharacterTemplate( pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate, dwID ) )
			{
				AgcdPreLOD* pPreLOD = CModelToolApp::GetInstance()->GetEngine()->GetAgcmPreLODManagerModule()->GetCharacterPreLOD( pcsAgpdCharacterTemplate );

				MessageBox("캐릭터를 추가하였습니다.", "Add");
			}
			else
			{
				MessageBox("캐릭터를 추가할 수 없습니다!", "Error");
				return;				
			}
		}
		break;

	case COD_TYPE_NAME_ITEM_INDEX:
		{
			AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(m_pvAgpdItemTemplate);
			AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(m_pvAgcdItemTemplate);

			if( pcsAgcdItemTemplate->m_pcsPreData == NULL ) {
				pcsAgcdItemTemplate->m_pcsPreData = new AgcdPreItemTemplate;
				NewAgcdPreItemTemplate( pcsAgcdItemTemplate->m_pcsPreData );
			}

			if (!SetBase(pcsAgpdItemTemplate->m_szName, pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName))
				return;

			if(!CModelToolApp::GetInstance()->AddItemTemplate(pcsAgpdItemTemplate, pcsAgcdItemTemplate, dwID))
			{
				MessageBox("아이템을 추가할 수 없습니다!", "Error");
				return;
			}
			else
			{
				//AgcdPreLOD* pstAgcdPreLOD = CModelToolApp::GetInstance()->GetEngine()->GetAgcmPreLODManagerModule()->GetItemPreLOD( pcsAgpdItemTemplate );
				//AgcdPreLODData* pcsData = CModelToolApp::GetInstance()->GetEngine()->GetAgcmPreLODManagerModule()->m_csPreLODAdmin.GetPreLODData( pstAgcdPreLOD, 0 );
				//strcpy( pcsData->m_aszData[1], "DUMMY" );

				MessageBox("아이템을 추가하였습니다.", "Add");
			}
		}
		break;

	case COD_TYPE_NAME_OBJECT_INDEX:
		{
			ApdObjectTemplate *pcsAgpdObjectTemplate = (ApdObjectTemplate *)(m_pvApdObjectTemplate);
			AgcdObjectTemplate *pcsAgcdObjectTemplate = (AgcdObjectTemplate *)(m_pvAgcdObjectTemplate);

			if(!SetBase(pcsAgpdObjectTemplate->m_szName, NULL))
				return;

			if(!strcmp(pcsAgcdObjectTemplate->m_szCategory, ""))
			{
				MessageBox("Category를 설정하세요!", "Error");
				return;
			}

			if(!CModelToolApp::GetInstance()->AddObjectTemplate(pcsAgpdObjectTemplate, pcsAgcdObjectTemplate, dwID))
			{
				MessageBox("오브젝트를 추가할 수 없습니다!", "Error");
				return;
			}
			else
			{
				MessageBox("오브젝트를 추가하였습니다.", "Add");
			}
		}
		break;

	case COD_TYPE_NAME_SKILL_INDEX:
		{
			AgpdSkillTemplate *pcsAgpdSkillTemplate = (AgpdSkillTemplate *)(m_pvAgpdSkillTemplate);
			AgcdSkillTemplate *pcsAgcdSkillTemplate = (AgcdSkillTemplate *)(m_pvAgcdSkillTemplate);

			if(!SetBase(pcsAgpdSkillTemplate->m_szName, NULL))
				return;

			if(!CModelToolApp::GetInstance()->AddSkillTemplate(pcsAgpdSkillTemplate, pcsAgcdSkillTemplate, dwID))
			{
				MessageBox("스킬을 추가할 수 없습니다!", "Error");
				return;
			}
			else
			{
				MessageBox("스킬을 추가하였습니다.", "Add");
			}
		}
		break;

	default:
		{ // error
		}
		break;
	}

	CDialog::OnOK();
}

VOID CCreateObjectDlg::InitializeMember()
{
	m_pvAgpdCharacterTemplate	= NULL;
	m_pvAgcdCharacterTemplate	= NULL;

	m_pvAgpdItemTemplate		= NULL;
	m_pvAgcdItemTemplate		= NULL;

	m_pvApdObjectTemplate		= NULL;
	m_pvAgcdObjectTemplate		= NULL;

	m_pvAgpdSkillTemplate		= NULL;
	m_pvAgcdSkillTemplate		= NULL;

	if(CModelToolApp::GetInstance())
	{
		m_pvAgpdCharacterTemplate	= malloc(sizeof(AgpdCharacterTemplate));
		m_pvAgcdCharacterTemplate	= malloc(sizeof(AgcdCharacterTemplate));

		m_pvAgpdItemTemplate		= malloc(CModelToolApp::GetInstance()->GetEngine()->GetAgpmItemModule()->GetTemplateDataSize());
		m_pvAgcdItemTemplate		= malloc(sizeof(AgcdItemTemplate));

		m_pvApdObjectTemplate		= malloc(sizeof(ApdObjectTemplate));
		m_pvAgcdObjectTemplate		= malloc(sizeof(AgcdObjectTemplate));

		m_pvAgpdSkillTemplate		= malloc(sizeof(AgpdSkillTemplate));
		m_pvAgcdSkillTemplate		= malloc(sizeof(AgcdSkillTemplate));

		memset(m_pvAgpdCharacterTemplate,	0,	sizeof(AgpdCharacterTemplate));
		memset(m_pvAgcdCharacterTemplate,	0,	sizeof(AgcdCharacterTemplate));

		memset(m_pvAgpdItemTemplate,		0,	sizeof(AgpdItemTemplate));
		memset(m_pvAgcdItemTemplate,		0,	sizeof(AgcdItemTemplate));

		memset(m_pvApdObjectTemplate,		0,	sizeof(ApdObjectTemplate));
		memset(m_pvAgcdObjectTemplate,		0,	sizeof(AgcdObjectTemplate));

		memset(m_pvAgpdSkillTemplate,		0,	sizeof(AgpdSkillTemplate));
		memset(m_pvAgcdSkillTemplate,		0,	sizeof(AgcdSkillTemplate));
	}
}

VOID CCreateObjectDlg::ReleaseMember()
{
	free(m_pvAgpdCharacterTemplate);
	free(m_pvAgcdCharacterTemplate);

	free(m_pvAgpdItemTemplate);
	free(m_pvAgcdItemTemplate);

	AgcdObjectTemplate *pstAgcdObjectTemplate = (AgcdObjectTemplate *)(m_pvAgcdObjectTemplate);
	if(pstAgcdObjectTemplate->m_stGroup.m_pstList)
	{
		if(	(CModelToolApp::GetInstance()) &&
			(CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()) &&
			(CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()->GetObjectList())	)
		{
			CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()->GetObjectList()->RemoveAllObjectTemplateGroup(&pstAgcdObjectTemplate->m_stGroup);
		}
	}

	free(m_pvApdObjectTemplate);
	free(m_pvAgcdObjectTemplate);

	free(m_pvAgpdSkillTemplate);
	free(m_pvAgcdSkillTemplate);
}

BOOL CCreateObjectDlg::SetBase(CHAR *szName, CHAR *szDFFName)
{
	if(!CModelToolApp::GetInstance())
		return FALSE;

	UpdateData( TRUE );

	strcpy( szName, m_strCOName );

	if( szDFFName )
		strcpy( szDFFName, CModelToolApp::GetInstance()->GetStaticModelDffName() );

	return TRUE;
}

void CCreateObjectDlg::OnButtonCoMore()
{
	UpdateData(TRUE);

	if(!CModelToolApp::GetInstance())
	{
		MessageBox("초기화실패!", "ERROR!");
		return;
	}

	switch(m_csCOType.GetCurSel())
	{
	case COD_TYPE_NAME_CHARACTER_INDEX:
		{
			AgpdCharacterTemplate *pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *)(m_pvAgpdCharacterTemplate);
			AgcdCharacterTemplate *pcsAgcdCharacterTemplate = (AgcdCharacterTemplate *)(m_pvAgcdCharacterTemplate);

			if( pcsAgcdCharacterTemplate->m_pcsPreData == NULL ) {
				pcsAgcdCharacterTemplate->m_pcsPreData = new AgcdPreCharacterTemplate;
				NewAgcdPreCharacterTemplate( pcsAgcdCharacterTemplate->m_pcsPreData );
			}

			if(!SetBase(pcsAgpdCharacterTemplate->m_szTName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName))
				return;

			if(CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterDlgModule()->OpenCharacterDlg(pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate))
			{
				m_strCOName = pcsAgpdCharacterTemplate->m_szTName;
				UpdateData(FALSE);
			}
		}
		break;

	case COD_TYPE_NAME_ITEM_INDEX:
		{
			AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(m_pvAgpdItemTemplate);
			AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(m_pvAgcdItemTemplate);

			if( pcsAgcdItemTemplate->m_pcsPreData == NULL ) {
				pcsAgcdItemTemplate->m_pcsPreData = new AgcdPreItemTemplate;
				NewAgcdPreItemTemplate( pcsAgcdItemTemplate->m_pcsPreData );
			}

			if (!SetBase(pcsAgpdItemTemplate->m_szName, pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName))
				return;

			if(CModelToolApp::GetInstance()->GetEngine()->GetAgcmItemDlgModule()->OpenItemDlg(pcsAgpdItemTemplate, pcsAgcdItemTemplate))
			{
				m_strCOName = pcsAgpdItemTemplate->m_szName;
				UpdateData(FALSE);
			}
		}
		break;

	case COD_TYPE_NAME_OBJECT_INDEX:
		{
			ApdObjectTemplate *pcsAgpdObjectTemplate = (ApdObjectTemplate *)(m_pvApdObjectTemplate);
			AgcdObjectTemplate *pcsAgcdObjectTemplate = (AgcdObjectTemplate *)(m_pvAgcdObjectTemplate);

			AgcdObjectTemplateGroupData *pcsData;
			if(!pcsAgcdObjectTemplate->m_stGroup.m_pstList)
			{
				if(	(CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()) &&
					(CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()->GetObjectList())	)
				{
					pcsData = CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectModule()->GetObjectList()->AddObjectTemplateGroup(&pcsAgcdObjectTemplate->m_stGroup);
					if(!pcsData)
						return;

					if (!pcsData->m_pszDFFName)
					{
						pcsData->m_pszDFFName =
							new CHAR [strlen(CModelToolApp::GetInstance()->GetStaticModelDffName()) + 1];
					}

					if (!pcsData->m_pszDFFName)
						return;

					//strcpy(pcsData->m_szDFFName, CModelToolApp::GetInstance()->GetStaticModelDffName());
					strcpy(pcsData->m_pszDFFName, CModelToolApp::GetInstance()->GetStaticModelDffName());
				}
				else
					return;
			}
			else
			{
				pcsData = &pcsAgcdObjectTemplate->m_stGroup.m_pstList->m_csData;
			}

			if (!SetBase(pcsAgpdObjectTemplate->m_szName, pcsData->m_pszDFFName))
				return;

			//pcsData->m_afAnimSpeed[E_OBJECT_ANIM_TYPE_NORMAL] = 1.0f;
			pcsData->m_fAnimSpeed	= 1.0f;

			if(CModelToolApp::GetInstance()->GetEngine()->GetAgcmObjectDlgModule()->OpenObjectDlg(pcsAgpdObjectTemplate, pcsAgcdObjectTemplate))
			{
				m_strCOName = pcsAgpdObjectTemplate->m_szName;
				UpdateData(FALSE);
			}
		}
		break;

	case COD_TYPE_NAME_SKILL_INDEX:
		{
			AgpdSkillTemplate *pcsAgpdSkillTemplate = (AgpdSkillTemplate *)(m_pvAgpdSkillTemplate);
			AgcdSkillTemplate *pcsAgcdSkillTemplate = (AgcdSkillTemplate *)(m_pvAgcdSkillTemplate);

			if(!SetBase(pcsAgpdSkillTemplate->m_szName, NULL))
				return;

			if(CModelToolApp::GetInstance()->GetEngine()->GetAgcmSkillDlgModule()->OpenSkillTemplateDlg(pcsAgpdSkillTemplate, pcsAgcdSkillTemplate))
			{
				m_strCOName = pcsAgpdSkillTemplate->m_szName;
				UpdateData(FALSE);
			}
		}
		break;

	default:
		{ // error
		}
		break;
	}
}

void CCreateObjectDlg::OnDestroy() 
{
	ReleaseMember();

	CDialog::OnDestroy();
}

void CCreateObjectDlg::OnCbnSelchangeComboCoType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch( m_csCOType.GetCurSel() )
	{
	case COD_TYPE_NAME_CHARACTER_INDEX:
		UpdateID( m_strID, CModelToolApp::GetInstance()->GenerateCTID(0) );
		break;
	case COD_TYPE_NAME_ITEM_INDEX:
		UpdateID( m_strID, CModelToolApp::GetInstance()->GenerateITID(0) );
		break;
	case COD_TYPE_NAME_OBJECT_INDEX:
		UpdateID( m_strID, CModelToolApp::GetInstance()->GenerateOTID(0) );
		break;
	case COD_TYPE_NAME_SKILL_INDEX:
		UpdateID( m_strID, CModelToolApp::GetInstance()->GenerateSTID(0) );
		break;
	}
	UpdateData(FALSE);
}
