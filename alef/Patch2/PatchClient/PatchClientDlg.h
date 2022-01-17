// PatchClientDlg.h : header file
//

#if !defined(AFX_PATCHCLIENTDLG_H__30CB0681_1D55_4B32_8DCF_A540401285F3__INCLUDED_)
#define AFX_PATCHCLIENTDLG_H__30CB0681_1D55_4B32_8DCF_A540401285F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AuMD5Encrypt.h"
#include "ClientNetworkLib.h"
#include "PatchClientLib.h"
#include "TestHtmlView.h"
#include "KbcButton.h"
#include "CustomProgressBar.h"
#include "PatchClientOptionDlg.h"
#include "TransparentListBox.h"
#include "NewStatic.h"
#include "BasePatchClient.h"
#include "afxwin.h"


unsigned int __stdcall StartPatch( void *pvArg);
unsigned int __stdcall StartPing( void *pvArg);

class CPatchClientDlg : public CDialog
{
public:

	void						PatchThreadStart();
	void						PingThreadStart();
	VOID						SetArea( const char* str )	{	m_pPatchClient->SetArea(str);	}
	HBITMAP						LoadBitmapResource( const char * pFileName );

public:


// Construction
public:
	CPatchClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPatchClientDlg();
	
	void CloseDlg();

	enum { IDD = IDD_PATCHCLIENT_DIALOG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Generated message map functions
	//{{AFX_MSG(CPatchClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEXitButton();
	afx_msg void OnStartGame();
	afx_msg void OnRegisterButton();
	afx_msg void OnOptionButton();
	afx_msg void OnHomepageButton();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		HICON m_hIcon;
public:
	afx_msg void OnBnClickedSeverselect1();
	afx_msg void OnBnClickedSeverselect2();
	afx_msg void OnBnClickedSeverselect3();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedMenubtn1();
	afx_msg void OnBnClickedMenubtn2();
	afx_msg void OnBnClickedMenubtn3();
	afx_msg void OnBnClickedMenubtn4();
	afx_msg void OnBnClickedMenubtn5();

private:
	void	ChinaTimer(UINT nIDEvent);
	void	JapanTimer(UINT nIDEvent);
	void	KoreaTimer(UINT nIDEvent);


	BasePatchClient*			m_pPatchClient;
	afx_msg LRESULT OnNcHitTest(CPoint point);
	
	afx_msg void	OnBnClickedButtonMore();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);

	LRESULT			OnMessageSelfPatch( WPARAM wParam , LPARAM lParam );

public :
	BOOL	OnReceiveClientFileCRC( void* pPacket )		{ return m_pPatchClient->OnReceiveClientFileCRC( pPacket ); }
public:
	afx_msg void OnLbnSelchangeServerlist();
};

#endif 
