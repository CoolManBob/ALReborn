#pragma once
#include "afxwin.h"


// CEditHairDlg dialog

class CEditHairDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditHairDlg)
public:
	void AddDefaultHair( const char* pFileName );
	int GetDefaultHairNum() const;
	const char* GetDefaultHairNth( int nIdx );

private:
	std::vector< std::string > m_vstrDefaultHair;

public:
	CEditHairDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditHairDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_HAIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedEditHairAdd();
	afx_msg void OnBnClickedEditHairDelete();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CListBox m_ctlHairList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEditHairDeleteAll();
};
