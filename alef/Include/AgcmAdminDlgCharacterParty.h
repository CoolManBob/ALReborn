// AgcmAdminDlgCharacterParty.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 05.

// Character Party Dialog Class
// For Character Property Page In Character Property Sheet
class __declspec(dllexport) AgcmAdminDlgCharacterParty : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfSearchParty;
	ADMIN_CB m_pfReset;

	AuList<stAgpdAdminCharPartyMember*> m_listMember;

public:
	AgcmAdminDlgCharacterParty();
	virtual ~AgcmAdminDlgCharacterParty();

	// Callback Function Setting
	BOOL SetCBSearchParty(ADMIN_CB pfCallback);
	BOOL SetCBReset(ADMIN_CB pfCallback);

	// Operation
	BOOL SetMember(stAgpdAdminCharPartyMember* pstMember);
	BOOL SetMember(stAgpdAdminCharDataSub* pstMemberSub);
	BOOL SetMember(PVOID pList);

	stAgpdAdminCharPartyMember* GetMember(INT32 lCID);

	BOOL ShowData(HWND hDlg = NULL);
	BOOL ShowBasicData(HWND hDlg = NULL);

	BOOL ClearMemberList();
	BOOL ClearMemberListView(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK CharacterPartyDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	// Initialize PC Basic Info List-View
	LRESULT InitBasicInfoLV(HWND hDlg);

	// Initialize Party List-View
	LRESULT InitPartyLV(HWND hDlg);

	// Button
	LRESULT OnRefreshBtn(HWND hDlg);
	LRESULT OnResetBtn(HWND hDlg);
};
