#if !defined(AFX_EVENTQUEST_H__68B492D5_2A17_429B_88C1_9FBEC728C997__INCLUDED_)
#define AFX_EVENTQUEST_H__68B492D5_2A17_429B_88C1_9FBEC728C997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventQuest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEventQuest dialog
class AgpmQuest;
class CEventQuest : public CDialog
{
public:
	AgpmQuest	*m_pcsAgpmQuest;
	ApdEvent	*m_pcsEvent;	
// Construction
public:
	CEventQuest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEventQuest)
	enum { IDD = IDD_EVENT_QUEST };
	CListBox	m_ctrlQuestGroup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventQuest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEventQuest)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	bool m_bIsObjectID;
	CString m_strObjectID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTQUEST_H__68B492D5_2A17_429B_88C1_9FBEC728C997__INCLUDED_)
