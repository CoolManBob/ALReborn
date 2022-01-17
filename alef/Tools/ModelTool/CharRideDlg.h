#pragma once
#include "treectrlex.h"

class CResourceTree;
// CCharRideDlg dialog

class CCharRideDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharRideDlg)

private:
	CResourceTree*	m_pcsTreeCharacter;

	void InsertChildTree( HTREEITEM hSourceParent, HTREEITEM hDestParent );

public:
	CCharRideDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCharRideDlg();

// Dialog Data
	enum { IDD = IDD_CHAR_RIDE_DLG };
	CTreeCtrlEx m_TreeRide;

public:
	BOOL Create(CWnd* pParentWnd = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnClose();
	afx_msg void OnNMDblclkCharRideTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDeatchRide();

	DECLARE_MESSAGE_MAP()
};
