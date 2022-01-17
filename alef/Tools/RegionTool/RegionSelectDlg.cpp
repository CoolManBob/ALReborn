// RegionSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionSelectDlg.h"

ApmMap g_csApmMap;

// Region Template.ini를 로딩하기 위한 전역변수

// CRegionSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRegionSelectDlg, CDialog)

CRegionSelectDlg::CRegionSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegionSelectDlg::IDD, pParent)
{
	m_nSelectedItem = 0;
	m_strSelectParentIndex = "";
}

CRegionSelectDlg::~CRegionSelectDlg()
{
	
}

void CRegionSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGIONSELECT_LIST, m_ctrRegionSelectList);
}


BEGIN_MESSAGE_MAP(CRegionSelectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRegionSelectDlg::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REGIONSELECT_LIST, &CRegionSelectDlg::OnLvnItemchangedRegionselectList)
END_MESSAGE_MAP()


// CRegionSelectDlg 메시지 처리기입니다.

BOOL CRegionSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrRegionSelectList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CRect	rect;
	m_ctrRegionSelectList.GetClientRect(&rect);
	m_ctrRegionSelectList.InsertColumn(0, _T("Index"), LVCFMT_LEFT, 50);
	m_ctrRegionSelectList.InsertColumn(1, _T("Region Name"), LVCFMT_LEFT, 125);

	g_csApmMap.LoadTemplate( "Ini\\" REGIONTEMPLATEFILE );

	if(!UpdateList())
		return FALSE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// List를 보여주는 것 
BOOL CRegionSelectDlg::UpdateList()
{
	AuNode< ApmMap::RegionTemplate > * pNode = g_csApmMap.m_listTemplate.GetHeadNode();
	ApmMap::RegionTemplate * pTemplateInList;

	CString	str;
	int     nIndex = 0;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		str.Format("%03d", pTemplateInList->nIndex);

		m_ctrRegionSelectList.InsertItem(nIndex, str, 4);
		m_ctrRegionSelectList.SetItemText(nIndex, 1, pTemplateInList->pStrName);

		pNode = pNode->GetNextNode();

		nIndex++;
	}

	return TRUE;
}


void CRegionSelectDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	OnOK();
}




void CRegionSelectDlg::OnLvnItemchangedRegionselectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelectedItem = pNMLV->iItem;

	m_strSelectParentIndex = m_ctrRegionSelectList.GetItemText(m_nSelectedItem,0); 

	*pResult = 0;
}





