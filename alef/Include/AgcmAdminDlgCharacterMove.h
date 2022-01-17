// AgcmAdminDlgCharacterMove.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 11. 25.

class __declspec(dllexport) AgcmAdminDlgCharacterMove : public AgcmAdminDlgBase
{
protected:
	// Callback Function Pointer
	ADMIN_CB m_pfCBMove;

	CHAR m_szSelfName[AGPACHARACTER_MAX_ID_STRING+1];

	CHAR m_szName[AGPACHARACTER_MAX_ID_STRING+1];
	float m_fX, m_fY, m_fZ;

	BOOL m_bSelfMove;

public:
	AgcmAdminDlgCharacterMove();
	virtual ~AgcmAdminDlgCharacterMove();

	// Callback Function Registration
	BOOL SetCBMove(ADMIN_CB pfCBMove);

	BOOL SetSelfName(CHAR* szSelfName);
	BOOL SetName(CHAR* szName);
	BOOL SetPosition(float fX, float fY, float fZ);
	BOOL SetSelfMove(BOOL bFlag);

	// Process
	BOOL ProcessSelfMove(float fX, float fY, float fZ);
	BOOL ProcessSelfMove(CHAR* szTargetName);

	BOOL ShowData(HWND hDlg = NULL);

	// Dialog Proc
	static LRESULT CALLBACK MoveDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	
protected:
	// Initialize
	LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
	LRESULT OnActivate(HWND hDlg, WPARAM wParam, LPARAM lParam);
	
	LRESULT OnSelfCheckBox(HWND hDlg);
	LRESULT OnGoBtn(HWND hDlg);
};
