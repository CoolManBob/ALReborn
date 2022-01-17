// AlefAdminCharMacro.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminCharMacro.h"
#include ".\alefadmincharmacro.h"
#include "AlefAdminManager.h"
#include "AlefAdminNotice.h"
#include "AgcmChatting2.h"


// AlefAdminCharMacro

IMPLEMENT_DYNCREATE(AlefAdminCharMacro, CFormView)

AlefAdminCharMacro::AlefAdminCharMacro()
	: CFormView(AlefAdminCharMacro::IDD)
{
}

AlefAdminCharMacro::~AlefAdminCharMacro()
{
}

void AlefAdminCharMacro::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_L_MACRO, m_csMacroList);
	DDX_Control(pDX, IDC_B_RUN_MACRO, m_csExcuteBtn);
	DDX_Control(pDX, IDC_B_LOAD, m_csLoadBtn);
	DDX_Control(pDX, IDC_B_ADD, m_csAddBtn);
	DDX_Control(pDX, IDC_B_REMOVE, m_csRemoveBtn);
	DDX_Control(pDX, IDC_B_SAVE, m_csSaveBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminCharMacro, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_B_LOAD, OnBnClickedBLoad)
	ON_BN_CLICKED(IDC_B_RUN_MACRO, OnBnClickedBRunMacro)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// AlefAdminCharMacro 진단입니다.

#ifdef _DEBUG
void AlefAdminCharMacro::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminCharMacro::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// AlefAdminCharMacro 메시지 처리기입니다.

void AlefAdminCharMacro::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csExcuteBtn.SetXButtonStyle(dwStyle);
	m_csLoadBtn.SetXButtonStyle(dwStyle);
	m_csAddBtn.SetXButtonStyle(dwStyle);
	m_csRemoveBtn.SetXButtonStyle(dwStyle);
	m_csSaveBtn.SetXButtonStyle(dwStyle);

	// Default 로 Load 해준다.
	LoadMacro(MACRO_DEFAULT_FILENAME);
}

int AlefAdminCharMacro::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminCharMacro::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminCharMacro::OnPaint()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.

	CRect r;
	GetClientRect(&r);
	CXTMemDC memDC(&dc, r);
	
	CFormView::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}





BOOL AlefAdminCharMacro::LoadMacro(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szFileName || strlen(szFileName) == 0)
		return FALSE;

	CString szMacroFileName = szFileName;

	CConfigManager csConfigManager;
	csConfigManager.SetFileName(szMacroFileName);
	csConfigManager.Open();

	CString szKey = _T("");
	CString szValue = _T("");

	CAlefAdminMacro* pcsCurrentMacro = NULL;

	while(csConfigManager.ReadLine())
	{
		szKey = csConfigManager.GetKey();
		szValue = csConfigManager.GetValue();

		// Macro Type
		if(szKey.CompareNoCase(MACRO_TYPE_NAME) == 0)
		{
			CAlefAdminMacro* pcsMacro = new CAlefAdminMacro();
			pcsMacro->m_eMacroType = (eMacroType)atoi((LPCTSTR)szValue);
			pcsCurrentMacro = pcsMacro;

			// vector 에 넣는다.
			m_vectorMacro.push_back(pcsMacro);
		}
		else if(szKey.CompareNoCase(MACRO_FUNCTION_TYPE_NAME) == 0)
		{
			if(pcsCurrentMacro)
			{
				pcsCurrentMacro->m_eFunctionType = (eMacroFunctionType)atoi((LPCTSTR)szValue);
				SetMacroFunction(pcsCurrentMacro);
			}
		}
		else if(szKey.CompareNoCase(MACRO_COMMAND_NAME) == 0)
		{
			if(pcsCurrentMacro)
			{
				pcsCurrentMacro->m_szCommand = new CHAR[szValue.GetLength() + 1];
				memcpy(pcsCurrentMacro->m_szCommand, (LPCTSTR)szValue, szValue.GetLength());
				pcsCurrentMacro->m_szCommand[szValue.GetLength()] = '\0';
			}
		}
		else if(szKey.CompareNoCase(MACRO_DESCRIPTION_NAME) == 0)
		{
			if(pcsCurrentMacro)
			{
				pcsCurrentMacro->m_szDescription = new CHAR[szValue.GetLength() + 1];
				memcpy(pcsCurrentMacro->m_szDescription, (LPCTSTR)szValue, szValue.GetLength());
				pcsCurrentMacro->m_szDescription[szValue.GetLength()] = '\0';
			}
		}
	}

	csConfigManager.Close();

	// 파일 이름 세팅해준다.
	SetDlgItemText(IDC_S_MACRO_FILENAME, szFileName);

	// 뿌린다.
	ShowMacro();

	return TRUE;
}

// Macro Function Type 에 따른 실제 함수를 등록해준다.
BOOL AlefAdminCharMacro::SetMacroFunction(CAlefAdminMacro* pcsMacro)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsMacro)
		return FALSE;

	switch(pcsMacro->m_eFunctionType)
	{
		case MACROFUNCTIONTYPE_SOS:
			pcsMacro->m_pfFuction = AlefAdminCharMacro::MF_SOS;
			break;

		case MACROFUNCTIONTYPE_DISCONNECT:
			pcsMacro->m_pfFuction = AlefAdminCharMacro::MF_Disconnect;
			break;

		case MACROFUNCTIONTYPE_ALL_SKILL_LEARN:
			pcsMacro->m_pfFuction = AlefAdminCharMacro::MF_AllSkillLearn;
			break;

		case MACROFUNCTIONTYPE_ALL_SKILL_UPGRADE:
			pcsMacro->m_pfFuction = AlefAdminCharMacro::MF_AllSkillUpgrade;
			break;
	}

	return TRUE;
}

BOOL AlefAdminCharMacro::ClearMacro()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// vector 가 있으면 돌면서 지운다.
	for(INT32 i = 0; i < (INT32)m_vectorMacro.size(); i++)
		delete m_vectorMacro[i];

	m_vectorMacro.clear();

	// 화면 비우기
	m_csMacroList.ResetContent();
	return TRUE;
}

BOOL AlefAdminCharMacro::SaveMacro()
{
	// 현재 저장하는 기능 없음~
	return TRUE;
}

BOOL AlefAdminCharMacro::ShowMacro()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	for(INT32 i = 0; i < (INT32)m_vectorMacro.size(); i++)
	{
		if(m_vectorMacro[i])
			m_csMacroList.AddString(m_vectorMacro[i]->m_szDescription);
	}

	return TRUE;
}

BOOL AlefAdminCharMacro::ExecuteMacro(CAlefAdminMacro* pcsMacro)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsMacro)
		return FALSE;

	switch(pcsMacro->m_eMacroType)
	{
		case MACROTYPE_COMMAND:
			ExecuteMacroCommand(pcsMacro);
			break;

		case MACROTYPE_FUNCTION:
			pcsMacro->m_pfFuction(pcsMacro, this, NULL);	// 첫번째 인자에 Macro 를 넣어준다.
			break;
	}

	return TRUE;
}

BOOL AlefAdminCharMacro::ExecuteMacroCommand(CAlefAdminMacro* pcsMacro)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsMacro || strlen(pcsMacro->m_szCommand) == 0 || (INT32)strlen(pcsMacro->m_szCommand) >= MAX_NOTICE_STRING)
		return FALSE;

	// 명령어 Macro 이므로 만들어준다.
	// 아직 파라미터는 지원하지 않으므로 그냥 command 를 채팅 메시지로 만들어서 보낸다.
	
	CHAR szMsg[255];
	if(pcsMacro->m_szCommand[0] != '/')
		sprintf(szMsg, "/%s ", pcsMacro->m_szCommand);
	else
		sprintf(szMsg, "%s ", pcsMacro->m_szCommand);

	// Parameter 를 붙인다.
	if(pcsMacro->m_szParameter && strlen(pcsMacro->m_szParameter) > 0 &&
		(INT32)(strlen(szMsg) + strlen(pcsMacro->m_szParameter)) < MAX_NOTICE_STRING)
		strcat(szMsg, pcsMacro->m_szParameter);

	AgcmChatting2* pcmChatting2 = ( AgcmChatting2* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmChatting2" );
	pcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_NORMAL,	szMsg, (INT32)strlen(szMsg), NULL);
	return TRUE;
}

BOOL AlefAdminCharMacro::MF_SOS(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	CAlefAdminMacro* pcsMacro = (CAlefAdminMacro*)pData;
	AlefAdminCharMacro* pThis = (AlefAdminCharMacro*)pClass;

	return TRUE;
}

BOOL AlefAdminCharMacro::MF_Disconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	CAlefAdminMacro* pcsMacro = (CAlefAdminMacro*)pData;
	AlefAdminCharMacro* pThis = (AlefAdminCharMacro*)pClass;

	if(strlen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return FALSE;

	// Ban 을 세팅한다.
	stAgpdAdminBan stBan;
	memset(&stBan, 0, sizeof(stBan));
	strcpy(stBan.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	
	// Disconnect 세팅
	stBan.m_lLogout = 1;

	AlefAdminManager::Instance()->GetAdminData()->SendBan(&stBan);
	return TRUE;
}

BOOL AlefAdminCharMacro::MF_AllSkillLearn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	CAlefAdminMacro* pcsMacro = (CAlefAdminMacro*)pData;
	AlefAdminCharMacro* pThis = (AlefAdminCharMacro*)pClass;

	return TRUE;
}

BOOL AlefAdminCharMacro::MF_AllSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	CAlefAdminMacro* pcsMacro = (CAlefAdminMacro*)pData;
	AlefAdminCharMacro* pThis = (AlefAdminCharMacro*)pClass;

	return TRUE;
}

void AlefAdminCharMacro::OnBnClickedBLoad()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CHAR szFilters[]= "Files (*.txt)|*.txt|All Files (*.*)|*.*||";

	CFileDialog cFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

    cFileDlg.GetOFN().Flags &= ~OFN_ENABLESIZING;
	cFileDlg.GetOFN().Flags |= OFN_EXPLORER;
    cFileDlg.GetOFN().lpstrTitle = _T("Load Macro File");

	if(cFileDlg.DoModal() != IDOK)
		return;

	// 비우고
	ClearMacro();

	// 다시 로드
	LoadMacro((LPCTSTR)cFileDlg.GetPathName());
}


void AlefAdminCharMacro::OnBnClickedBRunMacro()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iCurSel = m_csMacroList.GetCurSel();
	if(iCurSel < 0 || iCurSel == CB_ERR)
		return;

	if(iCurSel < (int)m_vectorMacro.size() && m_vectorMacro[iCurSel])
		m_vectorMacro[iCurSel]->Execute();
}

void AlefAdminCharMacro::OnDestroy()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnDestroy();

	ClearMacro();
}
