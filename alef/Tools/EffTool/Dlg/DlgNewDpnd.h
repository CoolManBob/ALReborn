#pragma once

#include "DlgList.h"

//------------------------------- CDlgList_BaseDpnd ------------------------
class CDlgNewDpnd;
typedef void (*CBNotify)(CDlgNewDpnd* parentDlg, const AgcdEffBase* pSelectedBase, BOOL bParent);
class CDlgList_BaseDpnd : public CDlgList
{
public:
	CDlgList_BaseDpnd(LPEFFSET pEffSet, BOOL bParent=TRUE, CWnd* pParent = NULL);

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgList_Base)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	INT			bGetIndex() const								{	return m_nSelectIndex;	}
	void		bSetCB(CDlgNewDpnd* parentDlg, CBNotify fptrCB)	{	m_parentDlg = parentDlg; m_fptrNotify = fptrCB;	}
	void		bSetParent(BOOL bParent=TRUE);

private:	
	INT			vInsert(INT nIndex, LPEFFBASE pEffBase);
	INT			vUpload();
	INT			vInitColum();

private:
	INT			m_nColumn;

	LPEFFSET	m_pEffSet;
	BOOL		m_bParent;

	INT			m_nSelectIndex;

	CDlgNewDpnd* m_parentDlg;
	CBNotify	m_fptrNotify;
};

//------------------------------- CDlgNewDpnd ------------------------
class CDlgNewDpnd : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewDpnd)

public:
	CDlgNewDpnd(LPEFFSET pEffSet, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewDpnd();

	//AFX_DATA
	enum { IDD = IDD_DLG_NEWDEPENDANCY };
	CStatic				m_staticParent;
	CStatic				m_staticChild;
	CString				m_strState;
	int					m_nNodeIndex;

protected:
	// Overrides
	virtual void CDlgNewDpnd::DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	static void CB_SelChanged(CDlgNewDpnd* pThis, const AgcdEffBase* pSelectedBase, BOOL bParent);

public:
	AgcdEffSet::BaseDependancyVec	m_vecDependancy;

private:
	LPEFFSET			m_pEffSet;
	CDlgList_BaseDpnd	m_dlgParent;
	CDlgList_BaseDpnd	m_dlgChild;
};
