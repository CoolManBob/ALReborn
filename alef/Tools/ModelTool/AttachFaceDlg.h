#pragma once
#include "afxwin.h"


class CAttachFaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttachFaceDlg)
public:
	BOOL		Create(CWnd* pParentWnd = NULL);
	void		ClearDefaultFace();
	void		SetDefaultFace();

private:
	void		AddDefaultFace( const char* pFileName );

public:
	CAttachFaceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAttachFaceDlg();

// Dialog Data
	enum { IDD = IDD_ATTACH_FACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOk();
	afx_msg void OnCancel();
	CListBox m_ctlFaceList;
	afx_msg void OnLbnDblclkAttackFaceList();
	afx_msg void OnClose();
//	afx_msg void OnBnClickedPreview();
//	afx_msg void OnBnClickedLeftTurn();
//	afx_msg void OnBnClickedRightTurn();
	afx_msg void OnBnClickedViewHelmet();
};
