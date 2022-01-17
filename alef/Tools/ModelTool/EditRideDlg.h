#pragma once


// CEditRideDlg dialog

class CEditRideDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditRideDlg)

public:
	CEditRideDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditRideDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_RIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
