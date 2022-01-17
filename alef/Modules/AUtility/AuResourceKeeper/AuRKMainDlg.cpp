// AuRKMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuRKMainDlg.h"
#include "AuRKSingleton.h"
#include "AuUsersDlg.h"
#include "RKInclude.h"
#include "AuRK_API.h"

#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuRKMainDlg dialog

static UINT BASED_CODE indicators[] =
{
    ID_INDICATOR_FILE,
    ID_INDICATOR_STATUS
};

BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		if (!AfxGetApp()->PumpMessage()) 
		{
			::PostQuitMessage(0);
			return FALSE;
		}	
	}

	return TRUE;
}
/*
DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
                        LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred,
						DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile,
						HANDLE hDestinationFile, LPVOID lpData)
{
	ProgressParam *pParam = (ProgressParam*)lpData;
	pParam->pBar->SetPos(*pParam->pCurrent + TotalBytesTransferred.LowPart);

	PeekAndPump();
	return PROGRESS_CONTINUE;
}
*/


AuRKMainDlg::AuRKMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuRKMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuRKMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AuRKMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuRKMainDlg)
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
	DDX_Control(pDX, IDC_FILE, m_ctrlFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuRKMainDlg, CDialog)
	//{{AFX_MSG_MAP(AuRKMainDlg)
	ON_NOTIFY(NM_RCLICK, IDC_FILE, OnRclickFile)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_COMMAND(ID_ADD_FOLDER, OnAddFolder)
	ON_NOTIFY(NM_CLICK, IDC_FILE, OnClickFile)
	ON_COMMAND(ID_OPEN_REPOSITORY, OnOpenRepository)
	ON_COMMAND(ID_SET_WORKING_FOLDER, OnSetWorkingFolder)
	ON_COMMAND(ID_CREATE_REPORITORY, OnCreateReporitory)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_SHOW_HISTORY, OnShowHistory)
	ON_COMMAND(ID_CHECK_IN, OnCheckIn)
	ON_COMMAND(ID_CHECK_OUT, OnCheckOut)
	ON_COMMAND(ID_GET_LATEST_VERSION, OnGetLatestVersion)
	ON_COMMAND(ID_UNDO_CHECK_OUT, OnUndoCheckOut)
	ON_COMMAND(ID_ADD_FILE, OnAddFile)
	ON_COMMAND(ID_USER_MANAGER, OnUserManager)
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_COMMAND(ID_LOGOUT, OnLogout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuRKMainDlg message handlers

BOOL AuRKMainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_iRClickIndex = -1;
	m_hRClickItem = NULL;
	m_eLastClickCtrl = CTRL_NULL;

	// 이미지 리스트 생성
	m_imgDrives.Create(IDB_DRIVES, 16, 1, RGB(0, 128, 128));
	// 이미지 리스트와 트리 컨트롤 연결
	m_ctrlTree.SetImageList(&m_imgDrives, TVSIL_NORMAL);

	InitTreeView();
	InitListView();
	InitMainMenu();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AuRKMainDlg::InitTreeView()
{
	// Working Folder가 설정되어 있지 않으면 리스트 뷰와 트리뷰를 사용하지 않는다.
	if (AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) return;

	m_ctrlTree.DeleteAllItems();
	HTREEITEM hItem = m_ctrlTree.InsertItem(RKROOT, ID_OPENFOLDER, ID_OPENFOLDER);
	FindSubFolder(hItem, AuRKSingleton::Instance()->m_strRepoRootPath + PATH(REMOTE_ROOT_PATH));
	m_ctrlTree.Expand(hItem, TVE_EXPAND);	
}
/*
void AuRKMainDlg::ConvertSystemPath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		rPath = AuRKSingleton::Instance()->m_strRepoRootPath + PATH(REMOTE_ROOT_PATH) + PATH(rPath);
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}
*/
void AuRKMainDlg::ConvertLocalPath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		rPath = AuRKSingleton::Instance()->m_strWorkingFolder + PATH(rPath);
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}

// 선택한 Node의 Sub Tree를 구성한다.
void AuRKMainDlg::ExpandFolder(HTREEITEM hItem, LPCTSTR lpPath)
{
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			// Tree View에 폴더 리스트 추가
			FindSubFolder(m_ctrlTree.InsertItem(stRKEntryInfo.szFileName, ID_CLOSEDFOLDER, ID_OPENFOLDER, hItem),
							strPath + PATH(stRKEntryInfo.szFileName));
		}
	}
}

void AuRKMainDlg::InitListView()
{
	// List View의 Header 초기화
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 3; i++)
	{
		lvcolumn.iSubItem = i;
		switch(i)
		{
		case 0:
			lvcolumn.cx = 120;
			lvcolumn.pszText = "Filename";
			break;

		case 1:
			lvcolumn.cx = 120;
			lvcolumn.pszText = "Date-Time";
			break;

		case 2:
			lvcolumn.cx = 62;
			lvcolumn.pszText = "User";
			break;
		}

		m_ctrlFile.InsertColumn(i, &lvcolumn);
	}	

	m_ctrlFile.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

void AuRKMainDlg::InitMainMenu()
{
	m_csMenuChecker.Init();
	m_csMenuChecker.SetMenu(CWnd::GetMenu());

	m_csMenuChecker.SetAdmin(AuRKSingleton::Instance()->m_bAdmin);
	m_csMenuChecker.ReloadAll();
}

void AuRKMainDlg::FindSubFolder(HTREEITEM hItem, LPCTSTR lpPath)
{
	// 하위 폴더가 하나라도 존재하면 더미 노드 생성
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;
	cRKEntryManager.GetEntryList(CString(lpPath) + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();

	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			m_ctrlTree.InsertItem("", ID_CLOSEDFOLDER, ID_OPENFOLDER, hItem);
			break;
		}
	}
}

void AuRKMainDlg::OnRclickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 선택된 아이템이 없으면 종료
	if (!m_ctrlFile.GetFirstSelectedItemPosition()) return; 

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// 클릭 정보
	m_eLastClickCtrl = CTRL_LISTVIEW;
	m_iRClickIndex = pNMListView->iItem;

	// 팝업될 마우스의 좌표를 얻음
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);

	// 팝업
	PopupMenu(ptMouse);

	*pResult = 0;
}

void AuRKMainDlg::PopupMenu(CPoint &rPoint)
{
	CMenu PopupMenu, *pPupupMenu;
	PopupMenu.LoadMenu(IDR_MAIN_POPUP);

	pPupupMenu = PopupMenu.GetSubMenu(0);
	int nCmd = pPupupMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | 
		TPM_RIGHTBUTTON, rPoint.x, rPoint.y, this ); 

	if (nCmd) this->SendMessage(WM_COMMAND, nCmd);
}

void AuRKMainDlg::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// Click된 아이템을 얻어오기
	CPoint ptTree;
	::GetCursorPos(&ptTree);
	m_ctrlTree.ScreenToClient(&ptTree);
	HTREEITEM hRClickItem = m_ctrlTree.HitTest(ptTree);

	if (!hRClickItem) return;

	// 클릭 정보
	m_eLastClickCtrl = CTRL_TREEVIEW;
	m_hRClickItem = hRClickItem;

	// 팝업될 마우스의 좌표를 얻음
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);

	// 팝업
	PopupMenu(ptMouse);

	*pResult = 0;
}

void AuRKMainDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Working Folder가 설정되어 있지 않으면 리스트 뷰와 트리뷰를 사용하지 않는다.
	if (AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_eLastClickCtrl = CTRL_TREEVIEW;
	m_hRClickItem = pNMTreeView->itemNew.hItem;
	CString strPath = GetPathFromNode(pNMTreeView->itemNew.hItem);
//	ConvertSystemPath(strPath);
	ViewFileList(strPath);

	*pResult = 0;
}

// 현재의 Node에서 Root Node까지의 Path를 구성한다.
CString AuRKMainDlg::GetPathFromNode(HTREEITEM hItem)
{
	CString strResult = m_ctrlTree.GetItemText(hItem);

	HTREEITEM hParent;
	// 루트 노드를 만날 때까지 진행...
	while((hParent = m_ctrlTree.GetParentItem(hItem)) != NULL)
	{
		CString str = m_ctrlTree.GetItemText(hParent);
		if(str.Right(1) != "\\")
			str += "\\";
		strResult = str + strResult;
		hItem = hParent;
	}
	return strResult;
}

void AuRKMainDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// 확장되거나 축소된 노드를 얻음
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	// 확장되거나 축소된 노드가 나타내는 디렉토리명을 얻음
	CString str = GetPathFromNode(hItem);
	
	*pResult = FALSE;

	// 노드가 확장되는 경우
	if(pNMTreeView->action == TVE_EXPAND)
	{
		// 앞서 추가했던 더미 노드 제거
		//DeleteFirstChild(hItem);
		DeleteAllChildren(hItem);
		// 진짜 디렉토리 구조를 읽어 표시
		ExpandFolder(hItem, str);
//		*pResult = TRUE;
	}
	else	// 노드가 축소되는 경우
	{
		// 모든 하위 노드를 제거
		DeleteAllChildren(hItem);

		AuRKSingleton::Instance()->ConvertRemotePath(str);
		FindSubFolder(hItem, str);
		// 십자가 모양의 버튼 표시를 위해 더미 노드 추가
	}
}

// 하나의 자식 노드 제거하기
void AuRKMainDlg::DeleteFirstChild(HTREEITEM hParent)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) != NULL)
		m_ctrlTree.DeleteItem(hItem);
}

// 모든 자식 노드 제거하기
void AuRKMainDlg::DeleteAllChildren(HTREEITEM hParent)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) == NULL)
		return;

	do
	{
		HTREEITEM hNextItem = m_ctrlTree.GetNextSiblingItem(hItem);
		m_ctrlTree.DeleteItem(hItem);
		hItem = hNextItem;
	} while (hItem != NULL);
}
/*
BOOL AuRKMainDlg::FindSubFolder(HTREEITEM hItem, LPCTSTR lpPathName)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) != NULL) return;

	do 
	{
		HTREEITEM hNextItem = m_ctrlTree.GetNextSiblingItem(hItem);
	} while (hItem != NULL);
	GetPathFromNode(
}
*/
// m_ctrlFile에 Item을 하나 추가한다.
void AuRKMainDlg::AddListItem(LPCTSTR lpFileName, LPCTSTR lpDate, LPCTSTR lpUser)
{
	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlFile.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)lpFileName;

	m_ctrlFile.InsertItem(&lvItem);

	m_ctrlFile.SetItemText(lvItem.iItem, 1, lpDate);
	m_ctrlFile.SetItemText(lvItem.iItem, 2, lpUser);
}

void AuRKMainDlg::ViewFileList(LPCTSTR lpPath)
{
	m_ctrlFile.DeleteAllItems();
	m_strRefreshPath = lpPath;

	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strPath);

	CString strLocalPath = lpPath;
	ConvertLocalPath(strLocalPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FILE == stRKEntryInfo.szType)
		{
			// List View에 파일 리스트 추가
//			if (!AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName)))
//				stRKEntryInfo.szOwner = "N " + stRKEntryInfo.szOwner;

			CRKEntryManager cFileVersion;
			if ((!cFileVersion.CheckLatestVersion(strPath + PATH(stRKEntryInfo.szFileName), strLocalPath + PATH(stRKEntryInfo.szFileName))) ||
				(!AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName))))
				stRKEntryInfo.szOwner = "O " + stRKEntryInfo.szOwner;

			AddListItem(stRKEntryInfo.szFileName, stRKEntryInfo.szDate, stRKEntryInfo.szOwner);
		}
	}
}

void AuRKMainDlg::OnCheckOut() 
{
	// JNY TODO : 우선은 파일만 체크 인/아웃 지원
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);
		

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

		BOOL bCheckOut = cRKEntryManager.CheckOut(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
			strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), AuRKSingleton::Instance()->m_strUserID);
		if (bCheckOut) 
		{
			m_ctrlFile.SetItemText(m_iRClickIndex, USERNAME, AuRKSingleton::Instance()->m_strUserID);
//			m_ctrlMessage.AddString("Check Out : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
		else
		{
//			m_ctrlMessage.AddString("Check Out Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
	}
}

void AuRKMainDlg::OnAddFolder() 
{
	// TODO: Add your command handler code here
	if (AuRKSingleton::Instance()->m_bLogin)
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strSelectedPath = strPath;
		strPath = "저장위치 : " + strPath;

		AuRKSingleton::Instance()->ConvertRemotePath(strSelectedPath);

		CString strMsg;

		if (AddFolder(strPath, strSelectedPath, strMsg))
		{
			InitTreeView();
//			if (strMsg.GetLength())
//				m_ctrlTree.InsertItem(strMsg, ID_CLOSEDFOLDER, ID_OPENFOLDER, m_hRClickItem);
/*
			// TRUE이면 추가된 폴더의 이름을 strMsg에 대입해준다.
			// 만약 strMsg가 IsEmpty이면 디렉토리 추가작업을 취소한것으로 간주한다.
			if (!strMsg.IsEmpty())
			{
				CString strTreePath = GetPathFromNode(m_hRClickItem);
				AuRKSingleton::Instance()->ConvertRemotePath(strTreePath);
				FindSubFolder(m_ctrlTree.InsertItem(strMsg, ID_CLOSEDFOLDER, ID_OPENFOLDER, m_hRClickItem), 
								strTreePath + PATH(strMsg));
			}
*/
		}
		else
		{
			// FALSE이면 파일 폴더가 실패한 이유를 알려준다.
			m_ctrlMessage.AddString(strMsg);
		}
	}
}

void AuRKMainDlg::OnClickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 선택된 아이템이 없으면 종료
	if (!m_ctrlFile.GetFirstSelectedItemPosition()) return; 

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_eLastClickCtrl = CTRL_LISTVIEW;
	m_iRClickIndex = pNMListView->iItem;

	*pResult = 0;
}

void AuRKMainDlg::OnCheckIn() 
{
	// JNY TODO : 우선은 파일만 체크 인/아웃 지원
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

		BOOL bCheckIn = cRKEntryManager.CheckIn(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
			strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), AuRKSingleton::Instance()->m_strUserID);
		if (bCheckIn) 
		{
			m_ctrlFile.SetItemText(m_iRClickIndex, USERNAME, "");
//			m_ctrlMessage.AddString("Check In : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
		else
		{
//			m_ctrlMessage.AddString("Check In Failed: " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
	}	
}

void AuRKMainDlg::OnOpenRepository() 
{
	// JNY TODO : Login 기능 구현해야됨
	if (OpenLoginDlg())
	{
		// 성공
	}
}

void AuRKMainDlg::OnSetWorkingFolder() 
{
	// TODO: Add your command handler code here
	if (SetWorkingFolder())
	{
		InitTreeView();
	}
}

void AuRKMainDlg::OnCreateReporitory() 
{
	// TODO: Add your command handler code here
	AuRKSingleton::Instance()->CreateRepository();
}

void AuRKMainDlg::OnRefresh() 
{
	// TODO: Add your command handler code here
	RefreshFileList();
}

void AuRKMainDlg::OnShowHistory() 
{
	// List View만 지원
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		AuRKSingleton::Instance()->ConvertRemotePath(strPath);
		AuRKSingleton::Instance()->ViewHistory(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)));
	}
}

void AuRKMainDlg::OnGetLatestVersion() 
{
	CString strRepoPath, strLocalPath;
	strRepoPath = GetPathFromNode(m_hRClickItem);
	strLocalPath = strRepoPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
	ConvertLocalPath(strLocalPath);

	CProgressDlg dlg;
	// List View일 경우엔 파일 카피 및 List View의 Index 유효성 검사
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		dlg.GetLatestFile(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, FILENAME)));
		if (IDOK == dlg.DoModal())
		{
		}
	}
	// Tree View일 경우엔 폴더 카피 및 
	else if ((CTRL_TREEVIEW == m_eLastClickCtrl) && (m_hRClickItem))
	{
		dlg.GetLatestFolder(strLocalPath);
		if (IDOK == dlg.DoModal())
		{
		}
	}

	RefreshFileList();
}

void AuRKMainDlg::OnUndoCheckOut() 
{
	BOOL bRefresh = FALSE;
	// List View만 갱신하면 됨
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);
		

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
		
		if (AuRKSingleton::Instance()->m_bAdmin)
		{
			// 어드민일 경우 무조건 Unlock한다.
			stRKEntryInfo stEntryInfo;
			if (cRKEntryManager.GetEntryFileInfo(m_ctrlFile.GetItemText(m_iRClickIndex, 0), &stEntryInfo))
			{
				CRKWriter csWriter;
				csWriter.SetFileName(strRepoPath + PATH(ENTRIES));
				csWriter.Open();
				csWriter.WriteOwner(stEntryInfo.szFileName, atoi(stEntryInfo.szSize), 
									stEntryInfo.szDate, CString(""), atoi(stEntryInfo.szVer));

				bRefresh = TRUE;
			}
		}
		else
		{
			CString strMsg;
			if (cRKEntryManager.UndoCheckOut(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), 
										strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
										AuRKSingleton::Instance()->m_strUserID))
			{
				bRefresh = TRUE;
				strMsg.Format("Unlock : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
				m_ctrlMessage.AddString(strMsg);
			}
			else
			{
				strMsg.Format("Unlock Failed : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
				m_ctrlMessage.AddString(strMsg);
			}
		}
	}

	if (bRefresh) RefreshFileList();
}

void AuRKMainDlg::OnAddFile() 
{
	// TODO: Add your command handler code here
	CFileDialog FileDlg(TRUE, "*.*");
	if (IDOK == FileDlg.DoModal())
	{
		// 경로 얻기
		CString strPathName = AuRKSingleton::Instance()->GetFolderPath(FileDlg.GetPathName());

		// 파일 이름 얻기
		CString strFileName = FileDlg.GetFileName();
		CString strFileName2 = strFileName;

		// Repository 경로 얻기
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		AuRKSingleton::Instance()->AddFile(strPathName + PATH(strFileName2), strRepoPath + PATH(strFileName2));
		RefreshFileList();
	}
}

void AuRKMainDlg::OnUserManager() 
{
	// TODO: Add your command handler code here
	AuUsersDlg UsersDlg;
	UsersDlg.DoModal();
}

void AuRKMainDlg::RefreshFileList()
{
	ViewFileList(m_strRefreshPath);
}

void AuRKMainDlg::OnFileDelete() 
{

	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		// 파일 삭제 시작
		CRKEntryManager cRKEntryManager;
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strRepoPath = strPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		CString strFileName = m_ctrlFile.GetItemText(m_iRClickIndex, 0);

		if (IDNO == MessageBox("파일 : " + strFileName + "\r\n를 정말로 지울까요?", "삭제", MB_YESNO)) return;

		CString strMsg;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
		if (cRKEntryManager.RemoveEntry(strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), 
					AuRKSingleton::Instance()->m_strUserID))
		{
			// Local 파일도 삭제
//			ConvertLocalPath(strPath);
//			EnableReadOnly(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), FALSE);
//			cRKEntryManager.RemoveFile(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)));

			// 파일 삭제 성공
			strMsg.Format("파일 삭제 성공 : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString(strMsg);

			RefreshFileList();
		}
		else
		{
			// 파일 삭제 실패
			strMsg.Format("파일 삭제 실패 : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString(strMsg);
		}

	}
	else if ((CTRL_TREEVIEW == m_eLastClickCtrl) && (m_hRClickItem))
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strRepoPath = strPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		if (IDNO == MessageBox("폴더 : " + strRepoPath + "\r\n를 정말로 지울까요?", "삭제", MB_YESNO)) return;

		CProgressDlg dlg;
		dlg.DeleteFolder(strRepoPath);
		if (IDOK == dlg.DoModal())
		{
			InitTreeView();
		}
	}
}

void AuRKMainDlg::OnLogout()
{
	RKLogout();
	OnCancel();
}

BOOL AuRKMainDlg::AddFolder(LPCTSTR lpTitle, CString &rSelectedPath, CString& rMsg)
{
	// Repository에 등록할 폴더 선택
	CHAR pszPathName[MAX_PATH];

	CString strFolderName;
	if (AuRKSingleton::Instance()->GetFolderNameDlg(lpTitle, pszPathName, MAX_PATH))
	{
		strFolderName = pszPathName;
		int iPos = strFolderName.ReverseFind('\\');
		strFolderName.Delete(0, iPos + 1);
		rMsg = strFolderName;
		BOOL bResult = AuRKSingleton::Instance()->AddFolder(pszPathName, rSelectedPath + PATH(strFolderName));
		return bResult;
	}

	return FALSE;
}
