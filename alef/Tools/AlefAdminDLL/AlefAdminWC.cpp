// AlefAdminWC.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminWC.h"
#include "AlefAdminManager.h"



// AlefAdminWC

IMPLEMENT_DYNCREATE(AlefAdminWC, CFormView)

AlefAdminWC::AlefAdminWC()
	: CFormView(AlefAdminWC::IDD)
{

}

AlefAdminWC::~AlefAdminWC()
{
}

void AlefAdminWC::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_B_WC_REFRESH, m_csRefreshBtn);
	DDX_Control(pDX, IDC_B_WC_DELETE, m_csDeleteBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminWC, CFormView)
	ON_BN_CLICKED(IDC_B_WC_REFRESH, &AlefAdminWC::OnBnClickedBWcRefresh)
	ON_BN_CLICKED(IDC_B_WC_DELETE, &AlefAdminWC::OnBnClickedBWcDelete)
END_MESSAGE_MAP()


// AlefAdminWC 진단입니다.

#ifdef _DEBUG
void AlefAdminWC::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AlefAdminWC::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// AlefAdminWC 메시지 처리기입니다.

void AlefAdminWC::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csRefreshBtn.SetXButtonStyle(dwStyle);
	m_csDeleteBtn.SetXButtonStyle(dwStyle);

	ClearContent();
}

int AlefAdminWC::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminWC::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminWC::OnPaint()
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

void AlefAdminWC::ClearContent()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	SetDlgItemText(IDC_E_WC_CONTENT, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WC_DEFAULT_TEXT));
}

BOOL AlefAdminWC::OnReceiveWCInfo(stAgpdAdminCharDataMoney* pstWantedCriminal)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstWantedCriminal->m_szCharName))
		return FALSE;

	CString szTmp;
	CString szContent;

	INT32 lRank = pstWantedCriminal->m_llBankMoney;

	szTmp.Format("%s : %d", AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ITEM_RANK),
							lRank);
	szContent += szTmp;
	szContent += "\r\n";

	szTmp.Format("%s : %I64d", AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WC_MONEY),
							pstWantedCriminal->m_llInvenMoney);
	szContent += szTmp;
	szContent += "\r\n";

	SetDlgItemText(IDC_E_WC_CONTENT, (LPCTSTR)szContent);

	return TRUE;
}
void AlefAdminWC::OnBnClickedBWcRefresh()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	ClearContent();
	
	// 걍 통째로 검색 -ㅂ-;
	AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_stCharData.m_stBasic.m_szCharName);
}

void AlefAdminWC::OnBnClickedBWcDelete()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(_tcslen(AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) == 0)
		return;

	if(::AfxMessageBox(IDS_WC_DELETE_CONFIRM, MB_YESNO) == IDNO)
		return;

	// 일단 비우고
	ClearContent();

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_WANTED_CRIMINAL;
	stCharEdit.m_lCID = 1;
	_tcscpy(stCharEdit.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);

	AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit);
}
