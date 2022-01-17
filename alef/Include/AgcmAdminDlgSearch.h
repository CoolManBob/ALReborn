// AgcmAdminDlgSearch.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 29.

// Search Dialog Class
class __declspec(dllexport) AgcmAdminDlgSearch : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfCBSearch;

	// Member Data
	stAgpdAdminCharData m_stCharData;
	AuList<stAgpdAdminSearchResult*> m_listSearchResult;

	CHAR m_szClickName[AGPACHARACTER_MAX_ID_STRING+1];

public:
	AgcmAdminDlgSearch();
	virtual ~AgcmAdminDlgSearch();

	// Callback Function Regist
	BOOL SetCBSearch(ADMIN_CB pfCBSearch);

	// Operation
	void SetCharData(stAgpdAdminCharData* pstCharData);
	stAgpdAdminCharData* GetCharData();
	BOOL SetResultList(PVOID pList);	// 일단 List 자체는 PVOID 형태로 받아온다.
	BOOL SetResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult);	// 단일 아이템이 들어온다.
	BOOL IsInResultList(stAgpdAdminSearchResult* pstSearchResult);	// 이미 Result List 에 있는 지 확인한다.
	
	BOOL ProcessSearch(CHAR* szName);	// 외부에서 Search 함수를 부르고 싶을 때 사용한다.

	BOOL ShowData(HWND hDlg = NULL);
	BOOL ClearResultList();
	BOOL ClearResultListView(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK SearchDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	// Button
	LRESULT OnSearchBtn(HWND hDlg);
	LRESULT OnAddBtn(HWND hDlg);
	LRESULT OnGoBtn(HWND hDlg);
	LRESULT OnModifyBtn(HWND hDlg);
	LRESULT OnGMBtn(HWND hDlg);

	// Result List-View
	LRESULT OnResultListClk(HWND hResultList, LPNMHDR hdr, LPNMLISTVIEW nlv);
	LRESULT OnResultListDblClk(HWND hResultList, LPNMHDR hdr, LPNMLISTVIEW nlv);

	// Result View Type Setting
	void ResetResultListView(HWND hDlg);
	void InitResultListView2Character(HWND hDlg);
	void InitResultListView2Item(HWND hDlg);
	void InitResultListView2NPC(HWND hDlg);
	void InitResultListView2Monster(HWND hDlg);
};