#if !defined(AFX_EVENTQUESTDLG_H__AC57AF38_E4ED_4515_8E1A_537D87DAA90F__INCLUDED_)
#define AFX_EVENTQUESTDLG_H__AC57AF38_E4ED_4515_8E1A_537D87DAA90F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventQuestDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventQuestDlg dialog
class AgpmQuest;
class CEventQuestDlg : public CDialog
{
public:
	AgpmQuest	*m_pcsAgpmQuest;
	ApdEvent	*m_pcsEvent;
	
	static CEventQuestDlg& GetInstance();

// Construction
public:
	CEventQuestDlg(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CEventQuestDlg)
	enum { IDD = IDD_EVENT_QUEST_DIALOG };
	CListBox	m_ctrlQuestGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventQuestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventQuestDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTQUESTDLG_H__AC57AF38_E4ED_4515_8E1A_537D87DAA90F__INCLUDED_)
