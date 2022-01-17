// AgcmAdminDlgBase.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 29.

class __declspec(dllexport) AgcmAdminDlgBase
{
protected:
	HINSTANCE m_hInst;	// Game Window 에서 받음
	HWND m_hWnd;		// Game Window 에서 받음

	HWND m_hDlg;			// Own Dlg
	BOOL m_bInitialized;	// Is Dialog Initialized

	UINT m_iResourceID;	// Dialog Resource ID
	PVOID m_pfDlgProc;	// Dialog Procedure Function Pointer

	PVOID m_pcsCBClass;	// Callback Class
	
	CRITICAL_SECTION m_csLock;	// 동기화

public:
	AgcmAdminDlgBase();
	virtual ~AgcmAdminDlgBase();

	virtual void SetInst(HINSTANCE hInst, HWND hWnd);
	virtual void SetParentHwnd(HWND hWnd);

	virtual UINT GetResourceID();
	virtual DLGPROC GetDlgProc();

	virtual BOOL OpenDlg();
	virtual BOOL CloseDlg(HWND hDlg);
	virtual HWND GetHDlg();
	virtual BOOL ShowData(HWND hDlg = NULL);

	virtual BOOL SetCBClass(PVOID pClass);

	virtual void Lock();
	virtual void Unlock();

protected:
	virtual LRESULT OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnActivate(HWND hDlg, WPARAM wParam, LPARAM lParam);

	virtual void SetInitialized(BOOL bFlag);
	virtual BOOL GetInitialized();

	virtual BOOL SetWindowPosByMousePoint(HWND hDlg = NULL);
	virtual BOOL SetWindowPosByParentWnd(HWND hDlg = NULL);
};
