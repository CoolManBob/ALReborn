// AgcmAdminDlgCharacterData.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 01.

// Character Info Dialog Class
// For Character Property Page In Character Property Sheet
class __declspec(dllexport) AgcmAdminDlgCharacterData : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfEditCharacter;

	// Member Data
	stAgpdAdminCharData m_stCharData;

public:
	AgcmAdminDlgCharacterData();
	virtual ~AgcmAdminDlgCharacterData();

	// Callback Function Setting
	BOOL SetCBEditCharacter(ADMIN_CB pfCallback);

	// Operation
	BOOL SetCharData(BOOL bResult, stAgpdAdminCharData* pstCharData);	// Sub 도 포함해서 한다.
	BOOL SetCharDataSub(BOOL bResult, stAgpdAdminCharDataSub* pstCharDataSub);	// Sub 만 한다.
	BOOL ShowData(HWND hDlg = NULL);

	void ClearAllInfoField();
	void ShowEditField(UINT iMode, UINT iFieldName, UINT iFieldData);

	// Dialog Proc
	static LRESULT CALLBACK CharacterDataDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);

	LRESULT OnEditEnterBtn();
	LRESULT OnEditCancelBtn();
};