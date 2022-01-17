#if !defined(AFX_EVENTBUTTON_H__79B5B16F_8E75_401F_B5C2_BD9D7BF13CB8__INCLUDED_)
#define AFX_EVENTBUTTON_H__79B5B16F_8E75_401F_B5C2_BD9D7BF13CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventButton.h : header file
//

typedef BOOL (*EventButtonCallback)(PVOID pvData);

#define	EVENT_BUTTON_PARAMS_DATA_NUM				5

class CEventButtonParams
{
public:
	CEventButtonParams()
	{
		memset(this, 0, sizeof(CEventButtonParams));
	}

	PVOID		m_pvClass;
	PVOID		m_pavData[EVENT_BUTTON_PARAMS_DATA_NUM];
};

/////////////////////////////////////////////////////////////////////////////
// CEventButton window

class CEventButton : public CButton
{
// Construction
public:
	CEventButton(CWnd *pcsParent, RECT &stInitRect,	LPCSTR lpszCaption, BOOL bFlat = FALSE, EventButtonCallback pfCallback = NULL, CEventButtonParams *pData = NULL)
	{
		m_pfCallback	= pfCallback;

		if (pData)
			memcpy(&m_csData, pData, sizeof(CEventButtonParams));

		DWORD dwFlag = (WS_CHILD | WS_VISIBLE);
		if(bFlat)
			dwFlag |= BS_FLAT;

		Create(lpszCaption, dwFlag, stInitRect, pcsParent, NULL);
	}
protected:
	EventButtonCallback		m_pfCallback;
	CEventButtonParams		m_csData;

// Attributes
public:
	VOID					_ShowButton();
	VOID					_HideButton();
	VOID					_SetWindowText(LPCTSTR lpszString);

	EventButtonCallback		GetCallback()							{return m_pfCallback;}
	CEventButtonParams		*GetParams()							{return &m_csData;}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEventButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEventButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTBUTTON_H__79B5B16F_8E75_401F_B5C2_BD9D7BF13CB8__INCLUDED_)
