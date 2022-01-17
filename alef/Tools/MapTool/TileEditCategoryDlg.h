#if !defined(AFX_TILEEDITCATEGORYDLG_H__25D7E500_33FC_4A7B_B821_97C778418554__INCLUDED_)
#define AFX_TILEEDITCATEGORYDLG_H__25D7E500_33FC_4A7B_B821_97C778418554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TileEditCategoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTileEditCategoryDlg dialog

class CTileEditCategoryDlg : public CDialog
{
// Construction
public:
	CTileEditCategoryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTileEditCategoryDlg)
	enum { IDD = IDD_TILE_EDITCATEGORY };
	CString	m_strName;
	int		m_nType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTileEditCategoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTileEditCategoryDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILEEDITCATEGORYDLG_H__25D7E500_33FC_4A7B_B821_97C778418554__INCLUDED_)
