// AgcmAdminDlgCharacterSkill.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 05.

// Character Skill Dialog Class (All Skill)
// For Character Property Page In Character Property Sheet
// 여기에는 Mastery 정보가 온다.
class __declspec(dllexport) AgcmAdminDlgCharacterSkill : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfSearchSkill;
	ADMIN_CB m_pfRollback;
	ADMIN_CB m_pfSave;	// 아직 쓸지 안 쓸지 미정

	AuList<stAgpdAdminSkillMastery*> m_listMastery;

public:
	AgcmAdminDlgCharacterSkill();
	virtual ~AgcmAdminDlgCharacterSkill();

	// Callback Function Setting
	BOOL SetCBSearchSkill(ADMIN_CB pfCallback);
	BOOL SetCBRollback(ADMIN_CB pfCallback);
	BOOL SetCBSave(ADMIN_CB pfCallback);

	// Operation

	// Mastery List 를 받는다.
	BOOL SetMasteryList(BOOL bResult, PVOID pList);
	BOOL SetMastery(stAgpdAdminSkillMastery* pstMastery);

	BOOL ShowData(HWND hDlg = NULL);
	BOOL ShowBasicData(HWND hDlg = NULL);

	BOOL ClearMasteryList();
	BOOL ClearMasteryListView(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK CharacterSkillDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	// Initialize PC Basic Info List-View
	LRESULT InitBasicInfoLV(HWND hDlg);

	// Initialize Skill Info List-View
	LRESULT InitSkillInfoLV(HWND hDlg);

	// Initialize ComboBox
	LRESULT InitMasteryCB(HWND hDlg);

	// Combo Box
	LRESULT OnMasteryChangeCB(HWND hDlg);
	
	// Mastery List View
	LRESULT OnMasteryLVDblClk(HWND hMastery, LPNMHDR hdr, LPNMLISTVIEW nlv);

	// Button
	LRESULT OnRefreshBtn(HWND hDlg = NULL);
	LRESULT OnRollbackBtn(HWND hDlg);
	LRESULT OnSaveBtn(HWND hDlg);
};
