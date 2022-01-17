
#include "AlefAdminQuest.h"
#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminManager.h"

IMPLEMENT_DYNCREATE(AlefAdminQuest, CFormView)

AlefAdminQuest::AlefAdminQuest()
	: CFormView(AlefAdminQuest::IDD)
{
}

AlefAdminQuest::~AlefAdminQuest()
{
}

void AlefAdminQuest::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_L_QUEST, m_csQuestList);
}

BEGIN_MESSAGE_MAP(AlefAdminQuest, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#ifdef _DEBUG
void AlefAdminQuest::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminQuest::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void AlefAdminQuest::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	ClearContent();
}

int AlefAdminQuest::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL AlefAdminQuest::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminQuest::OnPaint()
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

void AlefAdminQuest::ClearContent()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csQuestList.ResetContent();
}

BOOL AlefAdminQuest::OnReceiveQuestInfo(stAgpdAdminCharQuest* pstQuest)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AgpmQuest* pcsAgpmQuest = ( AgpmQuest* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmQuest" );

	if(!pcsAgpmQuest)
		return FALSE;

	INT32 lQid = pstQuest->lQuestID;
	INT32 lParam1 = pstQuest->lParam1;
	INT32 lParam2 = pstQuest->lParam2;

	AgpdQuestTemplate* pcsAgpdQuestTemplate = pcsAgpmQuest->m_csQuestTemplate.Get(lQid);
	if(!pcsAgpdQuestTemplate)
		return FALSE;

	CString szContent;

	szContent.Format("%s - %s "	, pcsAgpdQuestTemplate->m_QuestInfo.szScenarioName
								, pcsAgpdQuestTemplate->m_QuestInfo.szName);

	m_csQuestList.AddString(szContent);

	int maxWidth = 0;
	CString szText = _T("");

	for(int i = 0; i < m_csQuestList.GetCount(); i++)
	{
		m_csQuestList.GetText(i, szText);

		int w = szText.GetLength();

		if(w > maxWidth)
			maxWidth = w;
	}

	m_csQuestList.SetHorizontalExtent(maxWidth);

	return TRUE;
}