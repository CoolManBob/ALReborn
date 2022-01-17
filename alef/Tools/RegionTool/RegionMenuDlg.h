#if !defined(AFX_REGIONMENUDLG_H__5F24834D_68A0_45AD_9B18_25A6ABF9DDF3__INCLUDED_)
#define AFX_REGIONMENUDLG_H__5F24834D_68A0_45AD_9B18_25A6ABF9DDF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegionMenuDlg.h : header file
//
#include <vector>

#define WM_REGIONMENU_UPDATE	( WM_USER + 135 )
/////////////////////////////////////////////////////////////////////////////
// CRegionMenuDlg dialog

class CRegionMenuDlg;
class CRTMenuBase : public CWnd
{
public:
	static CRegionMenuDlg * m_pParent;
	static FLOAT	m_fScale;

	virtual	char *	GetMenuName() = 0;
	virtual BOOL Create( CRect * pRect , CWnd * pParent ) { return TRUE; }

	virtual BOOL OnLoadData		(){ return TRUE;}
	virtual BOOL OnSaveData		(){ return TRUE;}
	virtual BOOL OnQuerySaveData(){ return FALSE;}
	virtual	BOOL OnDeleteKeyDown(){ return TRUE;}

	virtual void OnPaintOnMap		( CDC * pDC ){}
	virtual BOOL OnLButtonDblDownMap( CPoint &point , int x , int z ){ return TRUE; }
	virtual BOOL OnLButtonDownMap	( CPoint &point , int x , int z ){ return TRUE; }
	virtual BOOL OnLButtonUpMap		( CPoint &point , int x , int z ){ return TRUE; }
	virtual BOOL OnMouseMoveMap		( CPoint &point , int x , int z ){ return TRUE; }

	virtual void OnActiveRegionMenu		(){}
	virtual void OnDeActiveRegionMenu	(){}
};

class CRegionToolDlg;

class CRegionMenuDlg : public CDialog
{
// Construction
public:
	CRegionMenuDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegionMenuDlg)
	enum { IDD = IDD_REGIONMENU };
	//}}AFX_DATA

	BOOL	Create( CWnd * pParent )	{ return CDialog::Create( IDD , pParent ); }


	struct	MenuInfo
	{
		CRTMenuBase	* pMenu		;
		CButton		* pButton	;

		MenuInfo():pMenu( NULL ),pButton( NULL ){}
	};

	vector< MenuInfo >	m_vectorMenuWnd;
	// CRTMenuBase	* m_pMenuWnd[ REGIONTOOLMODE_MAX ];

	BOOL	AddMenu( CRTMenuBase * pMenu );
	void	SelectMenu( int nOffset );

	INT32	m_nCurrentMenu;
	
	CRegionToolDlg	* m_pMainDlg;

	void	OnDeleteKeyDown();
	void	InvalidateRegionView();
	void	SetCaptureRegionView();
	void	ReleaseCaptureRegionView();

	BOOL	IsAvailableUnSavedData();

	void	SetWindowControlPosition();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionMenuDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CRegionMenuDlg)
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnGoto();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBitmapupdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

extern CString	g_strLastLockOwnerName;

BOOL	_CreateDirectory( char *pFormat , ... );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONMENUDLG_H__5F24834D_68A0_45AD_9B18_25A6ABF9DDF3__INCLUDED_)
