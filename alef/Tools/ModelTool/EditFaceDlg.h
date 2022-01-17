#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CEditFaceDlg dialog

class CEditFaceDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditFaceDlg)

public:
	void AddDefaultFace( const char* pFileName );
	int GetDefaultFaceNum() const;
	const char* GetDefaultFaceNth( int nIdx );

private:
	std::vector< std::string > m_vstrDefaultFace;

public:
	CEditFaceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditFaceDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_FACE };
	CListBox m_ctlFaceList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedEditFaceAdd();
	afx_msg void OnBnClickedEditFaceDelete();
	afx_msg void OnBnClickedEditFaceDeleteAll();

	DECLARE_MESSAGE_MAP()
};
