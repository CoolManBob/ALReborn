#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminManager.h"
#include "AlefAdminTitle.h"
#include "AgpmTitle.h"

IMPLEMENT_DYNCREATE(AlefAdminTitle, CFormView)

AlefAdminTitle::AlefAdminTitle()
	: CFormView(AlefAdminTitle::IDD)
{
}

AlefAdminTitle::~AlefAdminTitle()
{
}

void AlefAdminTitle::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE_ALL_LIST, m_csTitleAllList);
	DDX_Control(pDX, IDC_TITLE_LIST, m_csTitleList);
	DDX_Control(pDX, IDC_TITLE_QUEST_LIST, m_csTitleQuestList);
	DDX_Control(pDX, IDC_TITLE_QUEST_ADD, m_csTitleQuestAddBtn);
	DDX_Control(pDX, IDC_QUEST_VALUE_EDIT, m_csTitleQuestEditBtn);
	DDX_Control(pDX, IDC_QUEST_COMPLETE, m_csTitleQuestCompleteBtn);
	DDX_Control(pDX, IDC_QUEST_DELETE, m_csTitleQuestDeleteBtn);
	DDX_Control(pDX, IDC_TITLE_USE, m_csTitleUseBtn);
	DDX_Control(pDX, IDC_TITLE_DELETE, m_csTitleDeleteBtn);
}

BEGIN_MESSAGE_MAP(AlefAdminTitle, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ACTIVATEAPP()
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_TITLE_QUEST_ADD, OnBnClickedBTitleQuestAdd)
	ON_BN_CLICKED(IDC_QUEST_VALUE_EDIT, OnBnClickedBTitleQuestEdit)
	ON_BN_CLICKED(IDC_QUEST_COMPLETE, OnBnClickedBTitleQuestComplete)
	ON_BN_CLICKED(IDC_QUEST_DELETE, OnBnClickedBTitleQuestDelete)
	ON_BN_CLICKED(IDC_TITLE_USE, OnBnClickedBTitleUse)
	ON_BN_CLICKED(IDC_TITLE_DELETE, OnBnClickedBTitleDelete)
	ON_NOTIFY(NM_CLICK, IDC_TITLE_ALL_LIST, OnNMClickLcTitle)
	ON_NOTIFY(NM_CLICK, IDC_TITLE_LIST, OnNMClickLcHasTitle)
	ON_NOTIFY(NM_CLICK, IDC_TITLE_QUEST_LIST, OnNMClickLcTitleQuest)
END_MESSAGE_MAP()

#ifdef _DEBUG
void AlefAdminTitle::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminTitle::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


void AlefAdminTitle::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	InitTitleAllList();
	InitTitleList();
	InitTitleQuestList();

	ClearTitleContent();
	ClearTitleQuestContent();

	FillTitleAllList();

	SetDlgItemText(IDC_TITLE_QUEST_CHECK_VALUE, "");
}

BOOL AlefAdminTitle::PreTranslateMessage(MSG* msg)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	return CWnd::PreTranslateMessage(msg);
}

int AlefAdminTitle::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminTitle::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminTitle::OnPaint()
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

void AlefAdminTitle::InitTitleAllList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csTitleAllList.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_ID), LVCFMT_LEFT, 60);
	m_csTitleAllList.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_NAME), LVCFMT_LEFT, 180);
	m_csTitleAllList.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_CHECK_TYPE), LVCFMT_LEFT, 180);
	m_csTitleAllList.InsertColumn(3, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_CHECK_VALUE), LVCFMT_LEFT, 60);

	HWND hWndHeader = m_csTitleAllList.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader.SubclassWindow(hWndHeader);

	m_csTitleAllList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
}

void AlefAdminTitle::InitTitleList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csTitleList.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_ID), LVCFMT_LEFT, 60);
	m_csTitleList.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_NAME), LVCFMT_LEFT, 180);
	m_csTitleList.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_USE), LVCFMT_LEFT, 60);

	HWND hWndHeader = m_csTitleList.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader2.SubclassWindow(hWndHeader);

	m_csTitleList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
}

void AlefAdminTitle::InitTitleQuestList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csTitleQuestList.InsertColumn(0, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_ID), LVCFMT_LEFT, 60);
	m_csTitleQuestList.InsertColumn(1, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_NAME), LVCFMT_LEFT, 180);
	m_csTitleQuestList.InsertColumn(2, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_CHECK_TYPE), LVCFMT_LEFT, 180);
	m_csTitleQuestList.InsertColumn(3, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_CHECK_VALUE), LVCFMT_LEFT, 60);
	m_csTitleQuestList.InsertColumn(4, AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_TITLE_CUR_VALUE), LVCFMT_LEFT, 60);

	HWND hWndHeader = m_csTitleQuestList.GetDlgItem(0)->GetSafeHwnd();
	m_flatHeader3.SubclassWindow(hWndHeader);

	m_csTitleQuestList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB);
}

void AlefAdminTitle::ClearTitleContent()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csTitleList.DeleteAllItems();
}

void AlefAdminTitle::ClearTitleQuestContent()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csTitleQuestList.DeleteAllItems();
}

void AlefAdminTitle::FillTitleAllList()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AgpmTitle *pcsAgpmTitle = ( AgpmTitle* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmTitle" );

	if(!pcsAgpmTitle)
		return;

	INT32 lIndex = 0;
	for(vectorTitleStringTemplate::iterator IterTitle = pcsAgpmTitle->m_stAgpaTitleStringTemplate.m_vtTitleStringTemplate.begin(); IterTitle != pcsAgpmTitle->m_stAgpaTitleStringTemplate.m_vtTitleStringTemplate.end(); ++IterTitle)
	{
		AgpdTitleTemplate *pcsTitleTemplate = pcsAgpmTitle->GetTitleTemplate((*IterTitle).m_nTitleTid);

		if(!pcsTitleTemplate)
			continue;

		CHAR szTmp[255] = { 0, };
		sprintf(szTmp, "%d", pcsTitleTemplate->m_nTitleTid);
		m_csTitleAllList.InsertItem(lIndex, szTmp);

		m_csTitleAllList.SetItemText(lIndex, 1, pcsTitleTemplate->m_szTitleName);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pcsTitleTemplate->m_vtTitleCheck.begin()->m_nTitleCheckType);
		m_csTitleAllList.SetItemText(lIndex, 2, szTmp);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pcsTitleTemplate->m_vtTitleCheck.begin()->m_nTitleCheckValue);
		m_csTitleAllList.SetItemText(lIndex, 3, szTmp);

		lIndex++;
	}
}

BOOL AlefAdminTitle::OnReceiveTitleInfo(stAgpdAdminCharTitle* pstTitle)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstTitle)
		return FALSE;

	if(strcmp(pstTitle->m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) != 0)
		return FALSE;

	AgpmTitle *pcsAgpmTitle = ( AgpmTitle* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmTitle" );

	if(!pcsAgpmTitle)
		return FALSE;

	INT32 lTitleID = pstTitle->lTitleTid;
	BOOL bIsTitle = pstTitle->bIsTitle;
	BOOL bUseTitle = pstTitle->bUseTitle;

	INT32 lCheckValue[AGPDTITLE_MAX_TITLE_CHECK] = {0,};

	for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
		lCheckValue[i] = pstTitle->CheckValue[i];

	AgpdTitleTemplate *pcsTitleTemplate = pcsAgpmTitle->GetTitleTemplate(lTitleID);

	if(!pcsTitleTemplate)
		return FALSE;

	if(bIsTitle)
	{
		INT32 lIndex = m_csTitleList.GetItemCount();

		CHAR szTmp[255] = { 0, };

		sprintf(szTmp, "%d", lTitleID);
		m_csTitleList.InsertItem(lIndex, szTmp);

		m_csTitleList.SetItemText(lIndex, 1, pcsTitleTemplate->m_szTitleName);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", bUseTitle);
		m_csTitleList.SetItemText(lIndex, 2, szTmp);
	}
	else
	{
		INT32 lIndex = m_csTitleQuestList.GetItemCount();

		CHAR szTmp[255] = { 0, };

		sprintf(szTmp, "%d", lTitleID);
		m_csTitleQuestList.InsertItem(lIndex, szTmp);

		m_csTitleQuestList.SetItemText(lIndex, 1, pcsTitleTemplate->m_szTitleName);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pcsTitleTemplate->m_vtTitleCheck.begin()->m_nTitleCheckType);
		m_csTitleQuestList.SetItemText(lIndex, 2, szTmp);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", pcsTitleTemplate->m_vtTitleCheck.begin()->m_nTitleCheckValue);
		m_csTitleQuestList.SetItemText(lIndex, 3, szTmp);

		memset(szTmp, 0, sizeof(CHAR) * 255);
		sprintf(szTmp, "%d", lCheckValue[0]);
		m_csTitleQuestList.SetItemText(lIndex, 4, szTmp);
	}

	return TRUE;
}

void AlefAdminTitle::OnBnClickedBTitleQuestAdd()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_QUEST_ADD;
	stTitleOperation.lTitleTid = m_stSelectedTitle.lTitleTid;
	stTitleOperation.bIsTitle = FALSE;
	stTitleOperation.bUseTitle = FALSE;

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminTitle::OnBnClickedBTitleQuestEdit()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_QUEST_CHECK;
	stTitleOperation.lTitleTid = m_stSelectedTitleQuest.lTitleTid;
	stTitleOperation.bIsTitle = FALSE;
	stTitleOperation.bUseTitle = FALSE;

	char tempValue[10] = {0,};

	GetDlgItemText(IDC_TITLE_QUEST_CHECK_VALUE, tempValue, 9);
	stTitleOperation.CheckValue[0] = atoi(tempValue);

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminTitle::OnBnClickedBTitleQuestComplete()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_QUEST_COMPLETE;
	stTitleOperation.lTitleTid = m_stSelectedTitleQuest.lTitleTid;
	stTitleOperation.bIsTitle = FALSE;
	stTitleOperation.bUseTitle = FALSE;

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminTitle::OnBnClickedBTitleQuestDelete()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_QUEST_DELETE;
	stTitleOperation.lTitleTid = m_stSelectedTitleQuest.lTitleTid;
	stTitleOperation.bIsTitle = FALSE;
	stTitleOperation.bUseTitle = FALSE;

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminTitle::OnBnClickedBTitleUse()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_USE;
	stTitleOperation.lTitleTid = m_stSelectedHasTitle.lTitleTid;
	stTitleOperation.bIsTitle = TRUE;
	stTitleOperation.bUseTitle = TRUE;

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}

void AlefAdminTitle::OnBnClickedBTitleDelete()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
	{
		AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
		return;
	}

	if(AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->IsOnline() == FALSE)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_NOT_EDIT);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	if(m_stSelectedHasTitle.bUseTitle)
	{
		CString szMsg = _T("");
		szMsg.LoadString(IDS_TITLE_USING_STATE);

		::AfxMessageBox((LPCTSTR)szMsg);
		return;
	}

	stAgpdAdminTitleOperation stTitleOperation;
	memset(&stTitleOperation, 0, sizeof(stTitleOperation));

	stTitleOperation.m_cOperation = AGPMADMIN_TITLE_DELETE;
	stTitleOperation.lTitleTid = m_stSelectedHasTitle.lTitleTid;
	stTitleOperation.bIsTitle = TRUE;
	stTitleOperation.bUseTitle = FALSE;

	strcpy(stTitleOperation.m_szCharName, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName);
	strcpy(stTitleOperation.m_szAccName, AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->GetAccName());

	AlefAdminManager::Instance()->GetAdminData()->SendTitleOperation(&stTitleOperation);

	CString szMsg = _T("");
	CString szFormat = _T("");

	szFormat.LoadString(IDS_ITEM_REFRESH);
	szMsg.Format((LPCTSTR)szFormat);
	::AfxMessageBox((LPCTSTR)szMsg);
}




void AlefAdminTitle::OnNMClickLcTitle(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	stAgpdAdminCharTitle tempTitle;
	memset(&tempTitle, 0, sizeof(tempTitle));

	CString szTmp = _T("");

	szTmp = m_csTitleAllList.GetItemText(nlv->iItem, 0);

	INT32 lTitleTid = atoi((LPCTSTR)szTmp);

	tempTitle.lTitleTid = lTitleTid;

	m_stSelectedTitle = tempTitle;
}

void AlefAdminTitle::OnNMClickLcHasTitle(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	stAgpdAdminCharTitle tempTitle;
	memset(&tempTitle, 0, sizeof(tempTitle));

	CString szTmp = _T("");

	szTmp = m_csTitleList.GetItemText(nlv->iItem, 0);

	INT32 lTitleTid = atoi((LPCTSTR)szTmp);

	tempTitle.lTitleTid = lTitleTid;
	tempTitle.bIsTitle = TRUE;

	szTmp = m_csTitleList.GetItemText(nlv->iItem, 2);

	tempTitle.bUseTitle = atoi((LPCTSTR)szTmp);

	m_stSelectedHasTitle = tempTitle;
}

void AlefAdminTitle::OnNMClickLcTitleQuest(NMHDR *pNMHDR, LRESULT *pResult)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	LPNMLISTVIEW nlv = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	stAgpdAdminCharTitle tempTitle;
	memset(&tempTitle, 0, sizeof(tempTitle));

	CString szTmp = _T("");

	szTmp = m_csTitleQuestList.GetItemText(nlv->iItem, 0);

	INT32 lTitleTid = atoi((LPCTSTR)szTmp);

	tempTitle.lTitleTid = lTitleTid;
	tempTitle.bIsTitle = FALSE;

	szTmp = m_csTitleQuestList.GetItemText(nlv->iItem, 4);

	tempTitle.CheckValue[0] = atoi((LPCTSTR)szTmp);

	m_stSelectedTitleQuest = tempTitle;

	SetDlgItemText(IDC_TITLE_QUEST_CHECK_VALUE, (LPCTSTR)szTmp);
}