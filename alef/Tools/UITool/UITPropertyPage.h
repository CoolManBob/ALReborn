#if !defined(AFX_UITPROPERTYPAGE_H__3050BBAC_9384_4314_9651_3CB3169383F8__INCLUDED_)
#define AFX_UITPROPERTYPAGE_H__3050BBAC_9384_4314_9651_3CB3169383F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITPropertyPage.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITPropertyPage dialog

class UITPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(UITPropertyPage)

// Construction
public:
	virtual BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)	{ return TRUE; }
	virtual BOOL UpdateDataFromDialog()					{ return TRUE; }

	UITPropertyPage();
	~UITPropertyPage();

// Dialog Data
	//{{AFX_DATA(UITPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITPropertyPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITPROPERTYPAGE_H__3050BBAC_9384_4314_9651_3CB3169383F8__INCLUDED_)
