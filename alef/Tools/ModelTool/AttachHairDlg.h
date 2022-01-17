#pragma once
#include "afxwin.h"


// CAttachHairDlg dialog

class CAttachHairDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttachHairDlg)

public:
	void		ClearDefaultHair();
	void		SetDefaultHair();

private:
	void		AddDefaultHair( const char* pFileName );

public:
	CAttachHairDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttachHairDlg();

// Dialog Data
	enum { IDD = IDD_ATTACH_HAIR };
	CListBox m_ctlHairList;

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnDblclkAttachHairList();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};
