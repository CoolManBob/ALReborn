// AgcmAdminDlgXT_Move.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAdminDlgXT.h"
#include "AgcmAdminDlgXT_Move.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MESSAGE_BOX_TITLE	"ArchLord Admin - Move"

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Move dialog

const char* MOVE_BUTTON_STRING[] = {"Go!", "Add", "Remove"};
const char MOVE_PLACE_NAME_INPUT[] = "이곳에 장소를 입력하세요";
const char MOVE_PLACE_IN_LIST[] = "이미 같은 이름의 장소가 저장되어 있습니다.";

AgcmAdminDlgXT_Move::AgcmAdminDlgXT_Move(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_Move::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_Move)
	m_szMovePC = _T("");
	m_szMoveTargetPC = _T("");
	m_szMoveX = _T("");
	m_szMoveY = _T("");
	m_szMoveZ = _T("");
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;
	
	m_pfCBMove = NULL;

	m_eMoveMode = AGCMADMINDLGXT_MOVEMODE_NORMAL;
}


void AgcmAdminDlgXT_Move::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_Move)
	DDX_Control(pDX, IDC_CB_MOVE_MOVE_PLACE, m_csMovePlaceCB);
	DDX_Control(pDX, IDC_B_MOVE_MOVE_GO, m_csMoveBtn);
	DDX_Text(pDX, IDC_E_MOVE_MOVE_PC, m_szMovePC);
	DDX_Text(pDX, IDC_E_MOVE_MOVE_TARGET, m_szMoveTargetPC);
	DDX_Text(pDX, IDC_E_MOVE_MOVE_X, m_szMoveX);
	DDX_Text(pDX, IDC_E_MOVE_MOVE_Y, m_szMoveY);
	DDX_Text(pDX, IDC_E_MOVE_MOVE_Z, m_szMoveZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_Move, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_Move)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_B_MOVE_MOVE_GO, OnBMoveMoveGo)
	ON_CBN_SELCHANGE(IDC_CB_MOVE_MOVE_PLACE, OnSelchangeCbMoveMovePlace)
	ON_COMMAND(IDM_MOVE_FILE_CLOSE, OnMoveFileClose)
	ON_COMMAND(IDM_MOVE_EDIT_NORMAL, OnMoveEditNormal)
	ON_COMMAND(IDM_MOVE_EDIT_ADD, OnMoveEditAdd)
	ON_COMMAND(IDM_MOVE_EDIT_REMOVE, OnMoveEditRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Move message handlers

BOOL AgcmAdminDlgXT_Move::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::OpenDlg(INT nShowCmd)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
		CDialog::Create(IDD, m_pParentWnd);

	ShowWindow(nShowCmd);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::CloseDlg()
{
	if(::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::IsInitialized()
{
	return m_bInitialized;
}

BOOL AgcmAdminDlgXT_Move::SetCBMove(ADMIN_CB pfCBMove)
{
	m_pfCBMove = pfCBMove;
	return TRUE;
}

void AgcmAdminDlgXT_Move::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_Move::Unlock()
{
	m_csLock.Unlock();
}

BOOL AgcmAdminDlgXT_Move::SetMovePC(CString& szName)
{
	m_szMovePC = szName;

	return ShowData();
}

BOOL AgcmAdminDlgXT_Move::SetMoveTargetPC(CString& szName)
{
	m_szMoveTargetPC = szName;

	return ShowData();
}

BOOL AgcmAdminDlgXT_Move::SetPlace(CString& szName)
{
	return ShowData();
}

BOOL AgcmAdminDlgXT_Move::SetPosition(float fX, float fY, float fZ)
{
	// MovePlace CB 는 공백으로 바꾼다.
	if(m_bInitialized)
	{
		UpdateData();
		m_csMovePlaceCB.SetCurSel(0);
	}

	m_szMoveX.Format("%.2f", fX);
	m_szMoveY.Format("%.2f", fY);
	m_szMoveZ.Format("%.2f", fZ);

	return ShowData();
}

// 멤버 변수에 할당되어 있는 놈으로 움직인다.
BOOL AgcmAdminDlgXT_Move::ProcessMove()
{
	if(!m_pfCBMove || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return FALSE;

	// 이미 다이얼로그 되어 있으면, 그 값을 얻어온다.
	if(m_bInitialized)
		UpdateData();

	stAgpdAdminCharDataPosition stPosition;
	memset(&stPosition, 0, sizeof(stPosition));

	strcpy(stPosition.m_szName, (LPCTSTR)m_szMovePC);
	strcpy(stPosition.m_szTargetName, (LPCTSTR)m_szMoveTargetPC);

	stPosition.m_fX = atof((LPCTSTR)m_szMoveX);
	stPosition.m_fY = atof((LPCTSTR)m_szMoveY);
	stPosition.m_fZ = atof((LPCTSTR)m_szMoveZ);

	m_pfCBMove(&stPosition, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);
	
	return TRUE;
}

// 외부에서 호출하고 싶을 때 사용한다. - 2004.04.09.
BOOL AgcmAdminDlgXT_Move::ProcessMove(CString& szName, CString& szTargetName, float fX, float fY, float fZ)
{
	if(!m_pfCBMove || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return FALSE;

	stAgpdAdminCharDataPosition stPosition;
	memset(&stPosition, 0, sizeof(stPosition));

	strcpy(stPosition.m_szName, (LPCTSTR)szName);
	strcpy(stPosition.m_szTargetName, (LPCTSTR)szTargetName);

	stPosition.m_fX = fX;
	stPosition.m_fY = fY;
	stPosition.m_fZ = fZ;

	m_pfCBMove(&stPosition, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::MakeMainWindowMovePlaceCB()
{
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->ClearMovePlace();

	CString szTmp;
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->AddMovePlace(szTmp);
	
	list<AgcdAdminMovePlace*>::iterator iterData = m_listMovePlace.begin();
	while(iterData != m_listMovePlace.end())
	{
		if(*iterData)
		{
			szTmp = (*iterData)->m_szPlaceName;
			// Main Window 에도 추가.
			AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->AddMovePlace(szTmp);
		}

		iterData++;
	}
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::AddPlace(CString& szName, float fX, float fY, float fZ)
{
	if(GetMovePlace(szName))
		return FALSE;

	AgcdAdminMovePlace* pcsMovePlace = new AgcdAdminMovePlace;
	memset(pcsMovePlace, 0, sizeof(AgcdAdminMovePlace));

	strcpy(pcsMovePlace->m_szPlaceName, (LPCTSTR)szName);
	pcsMovePlace->m_fX = fX;
	pcsMovePlace->m_fY = fY;
	pcsMovePlace->m_fZ = fZ;

	m_listMovePlace.push_back(pcsMovePlace);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::RemovePlace(CString& szName)
{
	if(szName.GetLength() == 0)
		return FALSE;

	list<AgcdAdminMovePlace*>::iterator iterData = m_listMovePlace.begin();
	while(iterData != m_listMovePlace.end())
	{
		if(*iterData && szName.Compare((*iterData)->m_szPlaceName) == 0)
		{
			delete *iterData;
			m_listMovePlace.erase(iterData);
			break;
		}

		iterData++;
	}

	return TRUE;
}

AgcdAdminMovePlace* AgcmAdminDlgXT_Move::GetMovePlace(CString& szName)
{
	if(szName.GetLength() == 0)
		return NULL;

	list<AgcdAdminMovePlace*>::iterator iterData = m_listMovePlace.begin();
	while(iterData != m_listMovePlace.end())
	{
		if(*iterData && szName.Compare((*iterData)->m_szPlaceName) == 0)
			return *iterData;

		iterData++;
	}

	return NULL;
}

AgcdAdminMovePlace* AgcmAdminDlgXT_Move::GetMovePlace(CHAR* szName)
{
	if(!szName || strlen(szName) == 0)
		return NULL;

	return GetMovePlace(CString(szName));
}

BOOL AgcmAdminDlgXT_Move::ClearMovePlaceList()
{
	if(m_listMovePlace.size() == 0)
		return FALSE;

	list<AgcdAdminMovePlace*>::iterator iterData = m_listMovePlace.begin();
	while(iterData != m_listMovePlace.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listMovePlace.clear();

	return TRUE;
}

list<AgcdAdminMovePlace*>* AgcmAdminDlgXT_Move::GetMovePlaceList()
{
	return &m_listMovePlace;
}

BOOL AgcmAdminDlgXT_Move::ShowData()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData(FALSE);
	
	return TRUE;
}

// Main Window 의 화면도 바꿔줘야 한다.
BOOL AgcmAdminDlgXT_Move::ShowMovePlaceCB()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();

	m_csMovePlaceCB.ResetContent();

	m_csMovePlaceCB.AddString("");	// 일단 공백 하나 넣는다.
	
	list<AgcdAdminMovePlace*>::iterator iterData = m_listMovePlace.begin();
	while(iterData != m_listMovePlace.end())
	{
		if(*iterData)
			m_csMovePlaceCB.AddString((*iterData)->m_szPlaceName);

		iterData++;
	}

	UpdateData(FALSE);

	// Main Window 에도 다시 그려준다.
	MakeMainWindowMovePlaceCB();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Move::ClearMovePlaceCB()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();
	m_csMovePlaceCB.ResetContent();
	UpdateData(FALSE);

	return TRUE;
}





////////////////////////////////////////////////////////////////////////////////
// Dialog

BOOL AgcmAdminDlgXT_Move::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInitialized = TRUE;
	
	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csMoveBtn.SetXButtonStyle(dwStyle);

	ShowMovePlaceCB();

	// Move Mode Check
	OnMoveEditNormal();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAdminDlgXT_Move::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	CloseDlg();
}

void AgcmAdminDlgXT_Move::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	ClearMovePlaceList();
	
	CDialog::PostNcDestroy();
}

void AgcmAdminDlgXT_Move::OnBMoveMoveGo() 
{
	// TODO: Add your control notification handler code here
	switch(m_eMoveMode)
	{
		case AGCMADMINDLGXT_MOVEMODE_NORMAL:
			ProcessMove();
			break;

		case AGCMADMINDLGXT_MOVEMODE_ADD:
		{
			UpdateData();
			if(m_szMoveTargetPC.GetLength() == 0 || m_szMoveX.GetLength() == 0 ||
				m_szMoveY.GetLength() == 0 || m_szMoveZ.GetLength() == 0)
				break;

			if(AddPlace(m_szMoveTargetPC, atof((LPCTSTR)m_szMoveX), atof((LPCTSTR)m_szMoveY), atof((LPCTSTR)m_szMoveZ)) == FALSE)
			{
				MessageBox(MOVE_PLACE_IN_LIST, MESSAGE_BOX_TITLE);
				break;
			}

			// Config 파일 저장한다.
			AgcmAdminDlgXT_Manager::Instance()->GetIniManager()->Save();

			m_szMoveTargetPC = _T("");
			m_szMoveX = _T("");
			m_szMoveY = _T("");
			m_szMoveZ = _T("");

			UpdateData(FALSE);

			// Move Place Combo Box 를 다시 그린다.
			ClearMovePlaceCB();
			ShowMovePlaceCB();

			break;
		}

		case AGCMADMINDLGXT_MOVEMODE_REMOVE:
		{
			UpdateData();
			int iIndex = m_csMovePlaceCB.GetCurSel();
			CString szMovePlaceName;
			m_csMovePlaceCB.GetLBText(iIndex, szMovePlaceName);

			if(szMovePlaceName.GetLength() == 0)
				break;

			RemovePlace(szMovePlaceName);

			// Config 파일 저장
			AgcmAdminDlgXT_Manager::Instance()->GetIniManager()->Save();

			UpdateData();

			m_szMoveTargetPC = _T("");
			m_szMoveX = _T("");
			m_szMoveY = _T("");
			m_szMoveZ = _T("");

			UpdateData(FALSE);

			// Move Place Combo Box 를 다시 그린다.
			ClearMovePlaceCB();
			ShowMovePlaceCB();

			break;
		}
	}	
}

void AgcmAdminDlgXT_Move::OnSelchangeCbMoveMovePlace() 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	UpdateData();

	int iIndex = m_csMovePlaceCB.GetCurSel();
	CString szMovePlaceName;
	m_csMovePlaceCB.GetLBText(iIndex, szMovePlaceName);

	m_szMovePC = AgcmAdminDlgXT_Manager::Instance()->GetSelfAdminInfo()->m_szAdminName;
	m_szMoveX = _T("");
	m_szMoveY = _T("");
	m_szMoveZ = _T("");

	if(szMovePlaceName.GetLength() != 0)
	{
		AgcdAdminMovePlace* pcsMovePlace = GetMovePlace(szMovePlaceName);
		if(pcsMovePlace)
		{
			m_szMoveX.Format("%.2f", pcsMovePlace->m_fX);
			m_szMoveY.Format("%.2f", pcsMovePlace->m_fY);
			m_szMoveZ.Format("%.2f", pcsMovePlace->m_fZ);
		}
	}

	UpdateData(FALSE);
}

void AgcmAdminDlgXT_Move::OnMoveFileClose() 
{
	// TODO: Add your command handler code here
	CloseDlg();
}

void AgcmAdminDlgXT_Move::OnMoveEditNormal() 
{
	// TODO: Add your command handler code here
	m_eMoveMode = AGCMADMINDLGXT_MOVEMODE_NORMAL;
	OnMoveModeChanged();
}

void AgcmAdminDlgXT_Move::OnMoveEditAdd() 
{
	// TODO: Add your command handler code here
	m_eMoveMode = AGCMADMINDLGXT_MOVEMODE_ADD;
	OnMoveModeChanged();
}

void AgcmAdminDlgXT_Move::OnMoveEditRemove() 
{
	// TODO: Add your command handler code here
	m_eMoveMode = AGCMADMINDLGXT_MOVEMODE_REMOVE;
	OnMoveModeChanged();
}

BOOL AgcmAdminDlgXT_Move::OnMoveModeChanged()
{
	UpdateData();

	CMenu* pcsMenu = GetMenu();
	if(!pcsMenu)
		return FALSE;

	// Menu Item Check
	pcsMenu->CheckMenuItem(IDM_MOVE_EDIT_NORMAL, m_eMoveMode == AGCMADMINDLGXT_MOVEMODE_NORMAL ? MF_CHECKED : MF_UNCHECKED);
	pcsMenu->CheckMenuItem(IDM_MOVE_EDIT_ADD, m_eMoveMode == AGCMADMINDLGXT_MOVEMODE_ADD ? MF_CHECKED : MF_UNCHECKED);
	pcsMenu->CheckMenuItem(IDM_MOVE_EDIT_REMOVE, m_eMoveMode == AGCMADMINDLGXT_MOVEMODE_REMOVE ? MF_CHECKED : MF_UNCHECKED);

	// Move Button String Change
	m_csMoveBtn.SetWindowText(MOVE_BUTTON_STRING[m_eMoveMode]);

	// Target String Change
	if(m_eMoveMode == AGCMADMINDLGXT_MOVEMODE_ADD)
		m_szMoveTargetPC = MOVE_PLACE_NAME_INPUT;
	else
		m_szMoveTargetPC = _T("");

	UpdateData(FALSE);

	return TRUE;
}
