// AgcmAdminDlgCharacterItem.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 01.

// Character Info Dialog Class
// For Character Property Page In Character Property Sheet
class __declspec(dllexport) AgcmAdminDlgCharacterItem : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfSearchItem;

	// Member Data
	stAgpdAdminCharItem m_stLastSearchItem;
	AuList<stAgpdAdminItemData*> m_listItem;

public:
	AgcmAdminDlgCharacterItem();
	virtual ~AgcmAdminDlgCharacterItem();

	// Callback Function Setting
	BOOL SetCBSearchItem(ADMIN_CB pfCallback);

	// Operation
	BOOL SetLastSearchItem(stAgpdAdminCharItem* pstItem);
	stAgpdAdminCharItem* GetLastSearchItem();

	INT16 GetItemPosByCBIndex(INT16 lCBIndex);

	BOOL SetItem(INT16 lType, stAgpdAdminItemData* pstItem);
	stAgpdAdminItemData* GetItemBySlot(AgpmItemPart eItemPart);

	BOOL ShowData(HWND hDlg = NULL);
	BOOL ShowBasicData(HWND hDlg = NULL);

	BOOL ClearItemList();	// 멤버 리스트를 비운다.
	BOOL ClearItemListView(HWND hDlg = NULL);	// 화면출력을 비운다.

	// Dialog Proc
	static LRESULT CALLBACK CharacterItemDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	// Initialize PC Basic Info List-View
	LRESULT InitBasicInfoLV(HWND hDlg);

	// Initialize Item Info List-View
	LRESULT InitItemInfoLV(HWND hDlg);

	LRESULT InitItemPosCB(HWND hDlg);

	// Position
	LRESULT OnItemPosChange(HWND hDlg);

	// Refresh
	LRESULT OnRefreshBtn(HWND hDlg);
};
