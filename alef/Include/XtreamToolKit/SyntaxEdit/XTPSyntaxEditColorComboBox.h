// XTPSyntaxEditColorComboBox.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITCOLORCOMBOBOX_H__)
#define __XTPSYNTAXEDITCOLORCOMBOBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Summary:
//      This class allow edit colors using combobox control.
//      It display color name and draw rectangle for each item using item color.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditColorComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CXTPSyntaxEditColorComboBox)

public:
	// -------------------------------------------------------------------
	// Summary:
	//     Standard object constructor.
	// -------------------------------------------------------------------
	CXTPSyntaxEditColorComboBox();

	// -------------------------------------------------------------------
	// Summary:
	//     Standard object destructor.
	// -------------------------------------------------------------------
	virtual ~CXTPSyntaxEditColorComboBox();

public:

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to get selected color.
	// Returns:
	//     Currently selected color or COLORREF_NULL if no selection.
	// -------------------------------------------------------------------
	COLORREF GetSelColor();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to set selected color.
	// Parameters:
	//     crColor - A color value.
	// Returns:
	//     newly selected color item index.
	// -------------------------------------------------------------------
	int SetSelColor(COLORREF crColor);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to remove color.
	// Parameters:
	//     crColor - A color value.
	// Returns:
	//     Deleted color index or CB_ERR.
	// -------------------------------------------------------------------
	int DeleteColor(COLORREF crColor);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to find color index.
	// Parameters:
	//     crColor - A color value.
	// Returns:
	//     Color index or CB_ERR.
	// -------------------------------------------------------------------
	int FindColor(COLORREF crColor);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to set user color value (and name).
	// Parameters:
	//     crColor      - A color value.
	//     lpszUserText - A user color name or NULL to use default.
	// Returns:
	//     Color index or CB_ERR.
	// -------------------------------------------------------------------
	int SetUserColor(COLORREF crColor, LPCTSTR lpszUserText=NULL);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to get user color value.
	// Returns:
	//     A user color value.
	// -------------------------------------------------------------------
	COLORREF GetUserColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to set auto color value.
	// Parameters:
	//     crColor      - A color value.
	//     lpszAutoText - A auto color name or NULL to use default.
	// Returns:
	//     Color index or CB_ERR.
	// -------------------------------------------------------------------
	int SetAutoColor(COLORREF crColor, LPCTSTR lpszAutoText=NULL);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to get auto color value.
	// Returns:
	//     A auto color value.
	// -------------------------------------------------------------------
	COLORREF GetAutoColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to select a user color.
	// Returns:
	//     Selected color index or CB_ERR.
	// -------------------------------------------------------------------
	int SelectUserColor();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to add default set of colors.
	// Returns:
	//     true if successful, false otherwise.
	// -------------------------------------------------------------------
	virtual bool Init();

	//{{AFX_VIRTUAL(CXTPSyntaxEditColorComboBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCIS);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member to add new color.
	// Parameters:
	//     crColor - A color value.
	//     nID     - A resource ID of a color name string.
	// Returns:
	//     New color index or CB_ERR.
	// -------------------------------------------------------------------
	virtual int AddColor(COLORREF crColor, UINT nID);

	// -------------------------------------------------------------------
	// Summary:
	//     Send WM_COMMAND message to owner window.
	// Remarks:
	//      Default implementation send followin notifuications:
	//      CBN_SELENDCANCEL, CBN_SELENDOK
	// Parameters:
	//     nCode - Notifuication code
	// -------------------------------------------------------------------
	virtual void NotifyOwner(UINT nCode);

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to get list box selected item (works
	//      for dropped state only).
	// Returns:
	//      Currently selected item index or LB_ERR.
	// -------------------------------------------------------------------
	virtual int GetLBCurSel() const;

	//{{AFX_MSG(CXTPSyntaxEditColorComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCloseUp();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int      m_iPrevSel;
	bool     m_bPreInit;
	COLORREF m_crAuto;
	COLORREF m_crUser;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTPSyntaxEditColorComboBox::GetUserColor() const {
	return m_crUser;
}
AFX_INLINE COLORREF CXTPSyntaxEditColorComboBox::GetAutoColor() const {
	return m_crAuto;
}

//===========================================================================
// Summary:
//      Use this function to exchange currently selected color for
//      CXTPSyntaxEditColorComboBox control.
// Parameters:
//     pDX   - A pointer to CDataExchange object.
//     nIDC  - A control ID.
//     value - A reference to color value.
//===========================================================================
_XTP_EXT_CLASS void AFXAPI DDX_CBSyntaxColor(CDataExchange* pDX, int nIDC, COLORREF& value);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITCOLORCOMBOBOX_H__)
