// ModelTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ModelTool.h"
#include "ResourceForm.h"
#include "ResourceTree.h"
#include "AgcdEffGlobal.h"
#include  <dbghelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int MAX_TID = 0x000FFFFF;

char* g_pNationalCodeStr[nMAX_NATIONAL_CODES] =
{
	"KOREA",
	"CHINA",
	"WESTERN",
	"JAPAN"
};

/////////////////////////////////////////////////////////////////////////////
// CModelToolApp

BEGIN_MESSAGE_MAP(CModelToolApp, CWinApp)
	//{{AFX_MSG_MAP(CModelToolApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelToolApp construction
CStringDlg* CModelToolApp::m_pcsStingDlg = NULL;

static LPTOP_LEVEL_EXCEPTION_FILTER _originalExceptionHandler = NULL;
LONG __stdcall xCrashExceptionHandler(EXCEPTION_POINTERS* pExPtrs)
{
	HANDLE hFile = CreateFile( "modeltool.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
	if( !hFile )
	{
		MessageBox( NULL , "덤프파일 생성에 실패했습니다. 그냥 종료합니다." , "모델툴" , MB_OK );
		PostQuitMessage( 0 );
		return 0;
	}

	//그동안 작업을 저장하자...

	MessageBox( NULL , "크래시 발생.. 덤프 남기는동안 OK 누르시고 잠시 딴짓하세요" , "모델툴" , MB_OK );

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId			= GetCurrentThreadId();
	eInfo.ExceptionPointers	= pExPtrs;
	eInfo.ClientPointers	= FALSE;

	// win98 cannot
	MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, pExPtrs ? &eInfo : NULL, NULL, NULL );
	CloseHandle(hFile);

	MessageBox( NULL , "덤프 생성 완료했습니다 리포트 해주세요." , "모델툴" , MB_OK );

	PostQuitMessage( 0 );
	return 0;
}

static CModelToolApp *g_pcsApp = NULL;
CModelToolApp *CModelToolApp::GetInstance()
{
	return g_pcsApp;
}

#define VERSION	"2008.01.14"
CModelToolApp::CModelToolApp()
{
	g_pcsApp = this;

	_originalExceptionHandler = SetUnhandledExceptionFilter( xCrashExceptionHandler );

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	InitializeMember();
}

CModelToolApp::~CModelToolApp()
{
	if( _originalExceptionHandler )
	{
		SetUnhandledExceptionFilter( _originalExceptionHandler );
		_originalExceptionHandler = NULL;
	}
}

VOID Rw_Test()
{
/*	RwV3d inf, sup;
	inf.x = -100.f;
	inf.y = -100.f;
	inf.z = -100.f;
	sup.x = 100.f;
	sup.y = 100.f;
	sup.z = 100.f;

	ACC_CREATE_BOX_CLUMP("TT_TT.DFF", &inf, &sup);

	RwV3d center;
	center.x = 0.0f;
	center.y = 0.0f;
	center.z = 0.0f;
	ACC_CREATE_CYLINER_CLUMP("TT_TT.DFF", &center, 100.0f, 50.0f, 10);

	RwV3d center;
	center.x = 0.0f;
	center.y = 0.0f;
	center.z = 0.0f;
	ACC_CREATE_SPHERE_CLUMP("TT_TT.DFF", &center, 100.0f, 4);*/
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CModelToolApp object

CModelToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CModelToolApp initialization

static AgcWindow g_cWindow;
BOOL CModelToolApp::InitInstance()
{
	AfxEnableControlContainer();

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | /*_CRTDBG_CHECK_ALWAYS_DF | */_CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	m_rtInitWindow = CRect(0, 0, (980), (645 + 25));

	CModelToolDlg* pcsDlg = new CModelToolDlg();
	pcsDlg->Create(NULL);
	pcsDlg->ShowWindow(SW_HIDE);
	pcsDlg->MoveWindow(m_rtInitWindow);
	m_pMainWnd = pcsDlg;

	pcsDlg->m_rtToolDlg = m_rtInitWindow;

	Rw_Test();

	FirstInitMember();

	if (!InitStringDlg())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED InitStringDlg()", "ERROR", MB_ICONERROR | MB_OK);
		OutputDebugString("!\n!\nERROR <CModelToolApp::InitInstance() - StartUpDlg를 초기화할 수 없습니다>\n!\n!\n");
		return FALSE;
	}

	LoadToolData();
	EnableSound(m_csRenderOption.m_bEnableSound);

	if (!InitializeTitle())
	{
		ReleaseStringDlg();
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED InitializeTitle()", "ERROR", MB_ICONERROR | MB_OK);
		OutputDebugString("!\n!\nERROR <CModelToolApp::InitInstance() - 타이틀을 초기화할 수 없습니다>\n!\n!\n");
		return FALSE;
	}

	CHAR	szDebug[AMT_MAX_STR], szTemp[AMT_MAX_STR];
	if (!InitializeModule(szDebug))
	{
		ReleaseStringDlg();
		sprintf(szTemp, "Tool을 종료합니다!\n%s", szDebug);
		CModelToolDlg::GetInstance()->MessageBox(szTemp, "ERROR", MB_ICONERROR | MB_OK);
		OutputDebugString("!\n!\nERROR <CModelToolApp::InitInstance() - 모듈을 초기화할 수 없습니다>\n!\n!\n");
		return FALSE;
	}

	m_csAgcEngine.SetFullUIModule( &g_cWindow );

	if (!LoadResourceData())
	{
		ReleaseStringDlg();
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadResourceData()", "ERROR", MB_ICONERROR | MB_OK);
		OutputDebugString("!\n!\nERROR <CModelToolApp::InitInstance() - 리소스를 읽어드릴 수 없습니다>\n!\n!\n");
		return FALSE;
	}

	ReleaseStringDlg();

	m_cUtil.Initialize();
	m_cRwUtil.Initialize( GetRenderWare()->GetWorld(), GetRenderWare()->GetCamera() );

	CWnd* pWnd = AfxGetMainWnd();
	if( pWnd )
	{
		char szText[256];
		pWnd->GetWindowText( szText, 256 );
		char szTitle[512];
		string strInfo;
		{
			#ifdef _DEBUG
			strInfo += " Debug Build";
			#else
			strInfo += " Release Build";
			#endif

			// 날자 삽입..
			char strFilename[ 1024 ];
			GetModuleFileName( NULL , strFilename , 1024 );
			CFileStatus	fs;
			if( CFile::GetStatus( strFilename , fs ) )
			{
				CString strDate;
				strDate.Format( " [%04d.%02d.%02d]" , fs.m_mtime.GetYear() , fs.m_mtime.GetMonth() , fs.m_mtime.GetDay() );
				strInfo += (LPCTSTR)strDate;
			}
		}

		sprintf( szTitle, "%s %s", szText, strInfo.c_str() );
		pWnd->SetWindowText( szTitle );
	}

	/*

	2008. 06. 09. pm 15:49 by 성일

	Resource Tree Width 값이 저장되지 않는 문제 해결을 위해 다음과 같이 수정합니다.

	1. AMT_Initialize.cpp 의 CModelToolApp::LoadToolData() 에서 dwWidth 로 읽어들이던 값을
	   m_csRenderOption.m_dwResourceTreeWidth 로 직접 로드.
	2. AMT_ToolFunction.cpp 의 CModelToolApp::F_SetRenderOption() 함수에서 
	   m_csRenderOption.m_dwResourceTreeWidth 의 값을 계산해서 초기화 시키던 부분을 주석처리
	3. CModelToolApp::F_SetRenderOption() 에서 Resource Form 의 크기를 조정하던 구문을
	   CModelToolApp::InitInstance() 에서 초기화 과정이 종료된 후 한번 실행해주도록 수정

	*/
	CModelToolDlg::GetInstance()->ResizeResourceForm( m_csRenderOption.m_dwResourceTreeWidth );

	pcsDlg->MoveWindow(m_rtInitWindow);
	pcsDlg->ShowWindow(SW_SHOW);

	//GetEngine()->SetWorldCameraNearClipPlane( 60.f );

	return TRUE;
}

BOOL CModelToolApp::OnIdle(LONG lCount)
{
	AgcdEffGlobal::bGetInst().bSetCurrTime(GetTickCount());

	// TODO: Add your specialized code here and/or call the base class
	if( CModelToolDlg::GetInstance() )
	{
		m_csAgcEngine.GetAgcmRenderModule()->UpdateDistanceForTool();
		CModelToolDlg::GetInstance()->UpdateAll();
		UpdateApp();
		RenderScene();

		AcuFrameMemory::Clear();

		return m_bActiveWindows;
	}

	return CWinApp::OnIdle(lCount);
}

VOID CModelToolApp::Fin()
{
	SaveToolData();

	{
		m_csAgcEngine.GetAgcmSound()->OnDestroy();
		// 강제로 종료시켜버림..
		::ExitProcess( 0 );
	}
	ReleaseAllObject();
	RemoveTargetCharacter();
	m_csAgcEngine.Destroy();

	CModelToolDlg *pcsDlg = CModelToolDlg::GetInstance();
	if(pcsDlg)
	{
		pcsDlg->ReleaseAll();
		pcsDlg->ShowWindow(SW_HIDE);
		pcsDlg->DestroyWindow();

		AcuFrameMemory::Release();

		delete pcsDlg;
		pcsDlg = NULL;
	}
}

CRenderWare	*CModelToolApp::GetRenderWare()
{
	return CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare();
}

BOOL CModelToolApp::CursorInRenderView()
{
	if(!CModelToolDlg::GetInstance())
		return FALSE;

	if(!CModelToolDlg::GetInstance()->GetRenderForm())
		return FALSE;

	CRenderView *pcsRenderView = CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderView();
	if(!pcsRenderView)
		return FALSE;

	RECT rtRenderView;
	::GetWindowRect(pcsRenderView->m_hWnd, &rtRenderView);

	POINT ptCursor;
	::GetCursorPos(&ptCursor);

	return ::PtInRect(&rtRenderView, ptCursor);
}

RwUtilClumpArray *CModelToolApp::GetRenderClump()
{
	memset(&m_stClumpArray, 0, sizeof(RwUtilClumpArray));

	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_STATIC_MODEL:
		{
			if(m_pcsStaticModel)
			{
				m_stClumpArray.array[0] = m_pcsStaticModel;
				m_stClumpArray.num		= 1;
			}
		}
		break;

	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if(pcsAgpdCharacter)
			{
				AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
				if((pcsAgcdCharacter) && (pcsAgcdCharacter->m_pClump))
				{
					m_stClumpArray.array[0] = pcsAgcdCharacter->m_pClump;
					m_stClumpArray.num		= 1;
				}
			}
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItem *pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(m_lID);
			if(pcsAgpdItem)
			{
				AgcdItem *pcsAgcdItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(pcsAgpdItem);
				if((pcsAgcdItem) && (pcsAgcdItem->m_pstClump))
				{
					m_stClumpArray.array[0]	= pcsAgcdItem->m_pstClump;
					m_stClumpArray.num		= 1;
				}
			}
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if(pcsApdObject)
			{
				AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
				if(pcsAgcdObject)
				{
					INT32				lCount		= 0;
					AgcdObjectGroupList *pstList	= pcsAgcdObject->m_stGroup.m_pstList;
					while(pstList)
					{
						if(pstList->m_csData.m_pstClump)
						{
							m_stClumpArray.array[lCount] = pstList->m_csData.m_pstClump;
							++lCount;
						}

						pstList = pstList->m_pstNext;
					}

					m_stClumpArray.num = lCount;
				}
			}
		}
		break;

	default:
		return NULL;
	}

	return &m_stClumpArray;
}


INT32 CModelToolApp::GenerateCID()
{
	AgpdCharacter	*pcsAgpdCharacter;
	INT32			lCount = 1;
	for (	pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(lCount);
			pcsAgpdCharacter;
			pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(++lCount)	);

	return lCount;
}

INT32 CModelToolApp::GenerateIID()
{
	AgpdItem	*pcsAgpdItem;
	INT32		lCount = 1;
	for (	pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(lCount);
			pcsAgpdItem;
			pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(++lCount)	);

	return lCount;
}

INT32 CModelToolApp::GenerateOID()
{
	ApdObject	*pcsApdObject;
	INT32		lCount = 1;
	for(pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(lCount);
		pcsApdObject;
		pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(++lCount) );

	return lCount;
}

INT32 CModelToolApp::GenerateCTID(DWORD dwID)
{
	if( dwID )
	{
		AgpdCharacterTemplate* pTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(dwID);
		return pTemplate ? 0 : dwID;
	}

	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;
	INT32					lCount = 1;
	for(pcsAgpdCharacterTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(lCount);
		pcsAgpdCharacterTemplate;
		pcsAgpdCharacterTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(++lCount) );

	return lCount;
}

INT32 CModelToolApp::GenerateITID(DWORD dwID)
{
	if( dwID )
	{
		AgpdItemTemplate* pTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(dwID);
		return pTemplate ? 0 : dwID;
	}

	AgpdItemTemplate	*pcsAgpdItemTemplate;
	INT32				lCount = 1;
	for (	pcsAgpdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(lCount);
			pcsAgpdItemTemplate;
			pcsAgpdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(++lCount)	);

	return lCount;
}

INT32 CModelToolApp::GenerateOTID(DWORD dwID)
{
/*
	ApdObjectTemplate	*pcsApdObjectTemplate;
	INT32					lCount = (bMakeTempTemplate) ? (AGPD_TEMPORARY_TEMPLATE_INDEX) : (1);
	for(	pcsApdObjectTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(lCount);
			pcsApdObjectTemplate;
			pcsApdObjectTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(++lCount)	);
*/
	if( dwID )
	{
		ApdObjectTemplate* pTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(dwID);
		return pTemplate ? 0 : dwID;
	}

	int nTID = 0;
	ApdObjectTemplate	*pcsApdObjectTemplate = NULL;
	srand( (unsigned)time(NULL) );
	while( true )
	{
		nTID = rand()%MAX_TID;
		pcsApdObjectTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate( nTID );

		if( pcsApdObjectTemplate == NULL )
			break;
	}
	return nTID;
}

INT32 CModelToolApp::GenerateSTID(DWORD dwID)
{
	if( dwID )
	{
		AgpdSkillTemplate* pTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(dwID);
		return pTemplate ? 0 : dwID;
	}

	AgpdSkillTemplate*	pcsAgpdSkillTemplate;
	INT32				lCount = 1;
	for (	pcsAgpdSkillTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(lCount);
			pcsAgpdSkillTemplate;
			pcsAgpdSkillTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(++lCount)	);

	return lCount;
}

BOOL CModelToolApp::OpenBrowseForFolder(CHAR *szGetSelectedDirectory)
{
	if(!CModelToolDlg::GetInstance())
		return FALSE;

	CHAR szTemp[256];

	BROWSEINFO info;
	info.hwndOwner			= CModelToolDlg::GetInstance()->m_hWnd;
	info.pidlRoot			= NULL;
	info.pszDisplayName		= szTemp;
	info.lpszTitle			= "디렉토리를 선택하세요";
	info.ulFlags			= BIF_RETURNONLYFSDIRS;
//	info.lpfn				= BrowseCallbackProc;
	info.lpfn				= NULL;
	info.lParam				= 0;

	LPITEMIDLIST pidlBrowse = SHBrowseForFolder(&info);
	if(!pidlBrowse)
		return FALSE;
	
	SHGetPathFromIDList(pidlBrowse, szGetSelectedDirectory);

	return TRUE;
}

BOOL CModelToolApp::OpenObjectCategory(CHAR *szDest)
{
	return TRUE;
	//return m_csAgcEngine.GetAgcmObjectDlgModule()->OpenObjectCategory(szDest);
}

BOOL CModelToolApp::OpenObjectTemplate(CHAR *szDest)
{
	return m_csAgcEngine.GetAgcmObjectDlgModule()->OpenObjectTemplateList(szDest);
}

INT32 CModelToolApp::GetCharacterCurrentAnimDuration()
{
	if(m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return 0;

	AgpdCharacter *pstAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if(!pstAgpdCharacter)
		return 0;

	AgcdCharacter *pstAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
	if(!pstAgcdCharacter)
		return 0;

	return (INT32)(pstAgcdCharacter->m_csAnimation.GetDuration() * 100.0f);
}

VOID CModelToolApp::StopCharacterAnim(BOOL bStop)
{
	if(m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return;

	AgpdCharacter *pstAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if(!pstAgpdCharacter)
		return;

	AgcdCharacter *pstAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
	if(!pstAgcdCharacter)
		return;

	pstAgcdCharacter->m_bStop		= bStop;

	if( pstAgcdCharacter->m_pRide )
		pstAgcdCharacter->m_pRide->m_bStop = bStop;
}

BOOL CModelToolApp::IsPlayCharacterAnim()
{
	if(m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter *pstAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if(!pstAgpdCharacter)
		return FALSE;

	AgcdCharacter *pstAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
	if(!pstAgcdCharacter)
		return FALSE;

	return !(pstAgcdCharacter->m_bStop);
}

BOOL CModelToolApp::SetCharacterAnimTime(INT32 lAnimTime)
{
	AgpdCharacter *pstAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if (!pstAgpdCharacter)
		return FALSE;

	AgcdCharacter *pstAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
	if (!pstAgcdCharacter)
		return FALSE;

	if (!pstAgcdCharacter->m_pInHierarchy)
		return FALSE;

	RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, (RwReal)(lAnimTime) / 100.0f);
	RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pInHierarchy);

	if( pstAgcdCharacter->m_pRide && pstAgcdCharacter->m_pRide->m_pInHierarchy ) {
		RpHAnimHierarchySetCurrentAnimTime( pstAgcdCharacter->m_pRide->m_pInHierarchy, (RwReal)(lAnimTime) / 100.0f );
		RpHAnimHierarchyUpdateMatrices( pstAgcdCharacter->m_pRide->m_pInHierarchy );
	}

	return TRUE;
}

BOOL CModelToolApp::CBSetUDAData(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	return ((CModelToolApp *) pvClass)->SetUDAData((RpAtomic *) pvData);
}

BOOL CModelToolApp::CBApplyUDA(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvData)
		return TRUE;

	CModelToolApp *				pThis		= (CModelToolApp *) pvClass;
	RpAtomic *					pstAtomic	= (RpAtomic *) pvData;
	RpClump *					pstClump	= RpAtomicGetClump(pstAtomic);
	AgcdClumpRenderType *		pcsRenderType	= pThis->GetCurrentRenderType(pstAtomic);

	if (!pcsRenderType)
		return FALSE;

	if (pstClump)
	{
		pThis->m_csAgcEngine.GetAgcmRenderModule()->RemoveClumpFromWorld(pstClump);
		pThis->m_csAgcEngine.GetAgcmRenderModule()->ClumpSetRenderType(pstClump, pcsRenderType);
		pThis->m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(pstClump);
	}

	return TRUE;
}

BOOL CModelToolApp::CBSetFaceAtomic(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	return ((CModelToolApp *) pvClass)->SetFaceAtomic((RpAtomic *) pvData);
}

BOOL CModelToolApp::CBApplyFace(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvData)
		return TRUE;

	CModelToolApp *				pThis		= (CModelToolApp *) pvClass;
	RpAtomic *					pstAtomic	= (RpAtomic *) pvData;
	RpClump *					pstClump	= RpAtomicGetClump(pstAtomic);

	if (pThis->m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter		*pObject		= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->m_lID);
	if (!pObject)
		return NULL;
	AgcdCharacter		*cObject		= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pObject);
	if ((!cObject) || (!cObject->m_pstAgcdCharacterTemplate) || (!cObject->m_pClump))
		return NULL;

	cObject->m_pstAgcdCharacterTemplate->m_lFaceAtomicIndex = pstAtomic->id;

	return TRUE;
}

AgcdClumpRenderType * CModelToolApp::GetCurrentRenderType(RpAtomic *pstAtomic)
{
	INT32					lIndex = -1;

	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter		*pObject		= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if (!pObject)
				return NULL;
			AgcdCharacter		*cObject		= m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pObject);
			if ((!cObject) || (!cObject->m_pstAgcdCharacterTemplate) || (!cObject->m_pClump))
				return NULL;

			return &cObject->m_pstAgcdCharacterTemplate->m_csClumpRenderType;
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{			
			if (!CheckItemArmour())
				return NULL;

			if (!m_pcsArmourClump1)
				return NULL;			

			AgpdItem			*ppObject		= m_csAgcEngine.GetAgpmItemModule()->GetItem(m_lID);
			if (!ppObject)
				return FALSE;

			AgcdItem			*pcObject		= m_csAgcEngine.GetAgcmItemModule()->GetItemData(ppObject);
			if ((!pcObject) || (!pcObject->m_pstAgcdItemTemplate))
				return NULL;

			return &pcObject->m_pstAgcdItemTemplate->m_csClumpRenderType;
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject						*pObject	= m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if (!pObject)
				return NULL;
			AgcdObject						*cObject	= m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pObject);
			if ((!cObject) || (!cObject->m_pstTemplate))
				return NULL;

			INT32							lAtomicNum;
			INT32							lGroupIndex;
			AgcdObjectGroupList	*			pstList	= cObject->m_stGroup.m_pstList;

			while(pstList)
			{
				if (pstList->m_csData.m_pstClump)
				{
					lIndex = RwUtilClumpFindAtomicIndex(pstList->m_csData.m_pstClump, pstAtomic);
					if (lIndex > -1)
					{
						lAtomicNum	= RpClumpGetNumAtomics(pstList->m_csData.m_pstClump);
						lGroupIndex = pstList->m_csData.m_lIndex;
						break;
					}
				}

				pstList = pstList->m_pstNext;
			}

			if (lIndex > -1)
			{
				AgcdObjectTemplateGroupData *pcsData = m_csAgcEngine.GetAgcmObjectModule()->GetObjectList()->GetObjectTemplateGroup(&cObject->m_pstTemplate->m_stGroup, lGroupIndex);
				if (!pcsData)
					return NULL;

				return &pcsData->m_csClumpRenderType;
			}
		}
		break;
	}

	return NULL;
}

BOOL CModelToolApp::SetUDAData(RpAtomic *pstAtomic)
{
	SetPickedAtomic(pstAtomic);
	SetInitTransform();

	INT32					lIndex = -1;
	AgcdClumpRenderType *	pcsRenderType = NULL;
	RpClump *				pstClump = NULL;

	if (pstAtomic)
	{
		pcsRenderType = GetCurrentRenderType(pstAtomic);

		if( pcsRenderType == NULL )
			return FALSE;

		pstClump = RpAtomicGetClump(pstAtomic);

		if ( pcsRenderType->m_lSetCount != pstClump->iLastAtomicID )
		{
			//@{ 2006/06/16 burumal
			AgcdClumpRenderType* pSavedClumpRenderType = new AgcdClumpRenderType;
			if ( pSavedClumpRenderType )
			{
				if ( pcsRenderType->m_lSetCount > 0 )
				{
					pSavedClumpRenderType->m_lSetCount = pcsRenderType->m_lSetCount;

					pSavedClumpRenderType->m_csRenderType.Alloc				( pcsRenderType->m_lSetCount );
					pSavedClumpRenderType->m_csRenderType.MemsetRenderType	( 0 );
					pSavedClumpRenderType->m_csRenderType.MemsetCustData	( 0 );
				}
			}
			//@}

			pcsRenderType->m_lSetCount		= 0;
			m_csAgcEngine.GetAgcmRenderModule()->GetRenderType()->NewClumpRenderType(pcsRenderType, pstClump->iLastAtomicID);

			//@{ 2006/06/16 burumal
			if ( pSavedClumpRenderType )
			{
				m_csAgcEngine.GetAgcmRenderModule()->GetRenderType()->SearchClumpRenderType(pstClump, pSavedClumpRenderType, pcsRenderType);				
				
				delete pSavedClumpRenderType;
			}
			//@}
		}

		m_csAgcEngine.GetAgcmUDADlgModule()->SetRenderType(&pcsRenderType->m_csRenderType.m_vecRenderType[ pstClump->iLastAtomicID - pstAtomic->id - 1 ] , pstAtomic);
		m_csAgcEngine.GetAgcmUDADlgModule()->EnableMainUDADlgControl(TRUE);
		m_csAgcEngine.GetAgcmUDADlgModule()->SetCallbacks(this, CBSetUDAData, CBApplyUDA);
	}
	else
	{
		m_csAgcEngine.GetAgcmUDADlgModule()->EnableMainUDADlgControl(FALSE);
		m_csAgcEngine.GetAgcmUDADlgModule()->SetCallbacks(this, CBSetUDAData, CBApplyUDA);
	}

	return TRUE;
}

BOOL CModelToolApp::SetFaceAtomic(RpAtomic *pstAtomic)
{
	SetPickedAtomic(pstAtomic);
	SetInitTransform();

	INT32					lIndex = -1;

	m_csAgcEngine.GetAgcmSetFaceDlgModule()->SetFaceAtomic(pstAtomic);
	m_csAgcEngine.GetAgcmSetFaceDlgModule()->SetCallbacks(this, CBSetFaceAtomic, CBApplyFace);

	return TRUE;
}

RpAtomic *CModelToolApp::RenderWorldForAllIntersections(RwV2d *pixel)
{
	if (!GetRenderWare()) 
		return NULL;

	if (!GetRenderWare()->GetCamera())
		return NULL;

	RwUtilAtomicIntersectParams intersectParams;
    RwLine						pixelRay;

	RwCameraCalcPixelRay(GetRenderWare()->GetCamera(), &pixelRay, pixel);

	intersectParams.intersection.t.line = pixelRay;
    intersectParams.intersection.type   = rpINTERSECTLINE;
    intersectParams.pickedAtomic        = NULL;
    intersectParams.minDistance         = RwRealMAXVAL;

	GetEngine()->GetAgcmRenderModule()->RenderWorldForAllIntersections(
		&intersectParams.intersection,
		RwUtilWorldForAllAtomicIntersectLineCB,
		(void *)(&intersectParams)										);

	return intersectParams.pickedAtomic;
}

int GenObjectID( const std::vector<int>& vIDs )
{
	if( vIDs.empty() )
		return 1;

	int nLastIdx = vIDs.size()-1;
	return (vIDs[ nLastIdx ]+1);
}

void CModelToolApp::SetNationalCode(UINT uCode)
{ 
	if ( (uCode < nMAX_NATIONAL_CODES) && (m_uNationalCode != uCode) )
	{
		m_uNationalCode = uCode;
		m_bNationalCodeChanged = TRUE;

		ResetAllResourceTreeDNF(m_uNationalCode);
	}
}

VOID CModelToolApp::SetStringDlgTextCB(const CHAR* pMessage, VOID* pParam1)
{
	if( !m_pcsStingDlg )		return;

	CHAR pMsgBuff[256];
	sprintf(pMsgBuff, "%s\n%s", (char*)pParam1, pMessage);
	m_pcsStingDlg->SetString(pMsgBuff);
}

VOID CModelToolApp::WriteDirtyFileListCB(const CHAR* pMessage, VOID* pFileHandle)
{
	CHAR pMsgBuff[256];
	FILE* fpWrite = (FILE*) pFileHandle;

	if ( fpWrite )
	{
		sprintf(pMsgBuff, "%s\n", pMessage);
		fputs(pMsgBuff, fpWrite);
	}
}

VOID CModelToolApp::EnableSound(BOOL bValue)
{
	if ( m_csAgcEngine.GetAgcmSound() )
	{
		if ( bValue )
		{
			m_csAgcEngine.GetAgcmSound()->m_b3DSampleEnable = TRUE;
			m_csAgcEngine.GetAgcmSound()->m_bSampleEnable = TRUE;
		}	
		else
		{
			
			if ( m_csAgcEngine.GetAgcmSound()->m_b3DSampleEnable )
			{
				m_csAgcEngine.GetAgcmSound()->CloseAll3DSample();
				m_csAgcEngine.GetAgcmSound()->m_b3DSampleEnable = FALSE;
			}

			if ( m_csAgcEngine.GetAgcmSound()->m_bSampleEnable )
			{
				m_csAgcEngine.GetAgcmSound()->CloseAllSample();
				m_csAgcEngine.GetAgcmSound()->m_bSampleEnable = FALSE;
			}
		}
	}
}