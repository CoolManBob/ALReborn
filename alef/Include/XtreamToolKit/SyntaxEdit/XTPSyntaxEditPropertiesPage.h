// XTPSyntaxEditPropertiesPage.h : header file
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
#if !defined(__XTPSYNTAXEDITPROPERTIESPAGE_H__)
#define __XTPSYNTAXEDITPROPERTIESPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditPropertiesDlg;
class CXTPSyntaxEditView;

//===========================================================================
// Summary:
//      CXTPSyntaxEditTipWnd implements a tooltip window for
//      CXTPSyntaxEditTipListBox class.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditTipWnd : public CWnd
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTipWnd();

	//-----------------------------------------------------------------------
	// Summary: Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditTipWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//      Create a tooltip window.
	// Parameters:
	//      pListBox - A pointer to CListBox to attach.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: ShowTip
	//-----------------------------------------------------------------------
	virtual BOOL Create(CListBox* pListBox);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to determine is attached list box has
	//      owner draw flags (LBS_OWNERDRAWFIXED or LBS_OWNERDRAWVARIABLE).
	// Returns:
	//      TRUE if attached list box is owner drawn, FALSE otherwise.
	// See Also: CListBox, LBS_OWNERDRAWFIXED, LBS_OWNERDRAWVARIABLE
	//-----------------------------------------------------------------------
	virtual BOOL IsOwnerDrawn();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to show tooltip for specified list box item.
	// Parameters:
	//      iIndex - A list box item index.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: HideTip
	//-----------------------------------------------------------------------
	virtual BOOL ShowTip(int iIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to hide tooltip.
	// Returns:
	//      TRUE if tooltip was visible and method hide it, FALSE if tooltip
	//      was not visible.
	// See Also: ShowTip
	//-----------------------------------------------------------------------
	virtual BOOL HideTip();

	//-----------------------------------------------------------------------
	// Summary: Get text color.
	// Returns: A tooltip text color.
	// See Also: GetBackColor
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get background color.
	// Returns: A tooltip background color.
	// See Also: GetTextColor
	//-----------------------------------------------------------------------
	virtual COLORREF GetBackColor() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Register the window class if it has not already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     TRUE if the window class was successfully registered.  FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method used to draw tooltip for owner drawn list box.
	// Parameters:
	//      pDC     - A pointer to device context.
	//      rClient - Tooltip rect (in client coordinates of this window).
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: DrawTip
	//-----------------------------------------------------------------------
	virtual BOOL OwnerDrawTip(CDC* pDC, CRect rClient);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method used to draw tooltip.
	// Parameters:
	//      pDC     - A pointer to device context.
	//      rClient - Tooltip rect (in client coordinates of this window).
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: OwnerDrawTip
	//-----------------------------------------------------------------------
	virtual BOOL DrawTip(CDC* pDC, CRect rClient);

	//-----------------------------------------------------------------------
	// Summary:
	//      Used to get rectangle for a specified item.
	// Parameters:
	//      iItem - [in] A list box item index.
	//      rItem - [out] Item rectangle.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL CalcItemRect(int iItem, CRect& rItem);

	//{{AFX_VIRTUAL(CXTPSyntaxEditTipWnd)
	//}}AFX_VIRTUAL

	//{{AFX_CODEJOCK_PRIVATE
	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_MSG(CXTPSyntaxEditTipWnd)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int         m_iIndex;       // Current item index.
	CRect       m_rWindow;      // Store window rect.
	CPoint      m_ptCursor;     // Store cursor position.
	CListBox*   m_pListBox;     // Store pointer to attached list box.

private:
	void SetTipTimer();
	void KillTipTimer();
	UINT        m_uIDEvent1;
	UINT        m_uIDEvent2;
};


//===========================================================================
// Summary:
//      This class implements a list box with tooltip.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditTipListBox : public CListBox
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTipListBox();

	//-----------------------------------------------------------------------
	// Summary:
	//      This method used to determine a list box item for a specified point.
	// Parameters:
	//      pPoint - A pointer to POINT object or NULL. If this parameter
	//               NULL - current mouse coordinates are used.
	// Returns:
	//      Item index under specified point or LB_ERR.
	// See Also: HitTest@CPoint@BOOL
	//-----------------------------------------------------------------------
	int HitTest(LPPOINT pPoint = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      This method used to determine a list box item for a specified point.
	// Parameters:
	//      point     - A CPoint object.
	//      bIsClient - Set as TRUE if point contains client coordinates or
	//                  set as FALSE if screen.
	// Returns:
	//      Item index under specified point or LB_ERR.
	// See Also: HitTest@LPPOINT
	//-----------------------------------------------------------------------
	int HitTest(CPoint point, BOOL bIsClient = FALSE) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to show tooltip for specified point
	//      if necessary.
	// Parameters:
	//      point     - A CPoint object.
	//      bIsClient - Set as TRUE if point contains client coordinates or
	//                  set as FALSE if screen.
	// Returns:
	//      Item index if tooltip showed, otherwise LB_ERR.
	//-----------------------------------------------------------------------
	int ShowTip(CPoint point, BOOL bIsClient = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to determine is selection changed
	//      by the user.
	// Returns:
	//      TRUE selection changed, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SelChanged() const;

protected:
	//{{AFX_VIRTUAL(CXTPSyntaxEditTipListBox)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	DWORD                   m_dwIdx;        // Selection indexes: LOWORD - previous selection index, HIWORD - active selection index.
	CXTPSyntaxEditTipWnd    m_wndInfoTip;   // A tooltip window.
};


//===========================================================================
// Summary:
//      This class implements a combo box with tooltip.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditTipComboBox : public CComboBox
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to get an attached list box.
	// Returns:
	//      A CXTPSyntaxEditTipListBox object reference.
	// See Also: CXTPSyntaxEditTipListBox
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTipListBox& GetListBox();

protected:
	//{{AFX_MSG(CXTPSyntaxEditTipComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CXTPSyntaxEditTipListBox m_wndListBox;
};

AFX_INLINE CXTPSyntaxEditTipListBox& CXTPSyntaxEditTipComboBox::GetListBox() {
	return m_wndListBox;
}

//===========================================================================
// Summary:
//     This class implements a property page for common edit options.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditPropertiesPageEdit : public CPropertyPage
{
	DECLARE_DYNCREATE(CXTPSyntaxEditPropertiesPageEdit)

public:
	// -------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pEditView  - Pointer to CXTPSyntaxEditView object.
	// -------------------------------------------------------------------
	CXTPSyntaxEditPropertiesPageEdit(CXTPSyntaxEditView* pEditView=NULL);

	// -------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// -------------------------------------------------------------------
	virtual ~CXTPSyntaxEditPropertiesPageEdit();

	//{{AFX_DATA(CXTPSyntaxEditPropertiesPageEdit)
	enum { IDD = XTP_IDD_EDIT_PAGEEDITOR };
	BOOL    m_bAutoReload;
	BOOL    m_bHorzScrollBar;
	BOOL    m_bVertScrollBar;
	BOOL    m_bSyntaxColor;
	BOOL    m_bAutoIndent;
	BOOL    m_bSelMargin;
	BOOL    m_bLineNumbers;
	int     m_nCaretStyle;
	int     m_nTabType;
	int     m_nTabSize;
	CButton m_btnRadioSpaces;
	CButton m_btnRadioTab;
	CButton m_btnRadioCaretThin;
	CButton m_btnRadioCaretThick;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CXTPSyntaxEditPropertiesPageEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL

protected:

	// -------------------------------------------------------------------
	// Summary:
	//     Use this method to load controls state (options) from the registry.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: WriteRegistryValues
	// -------------------------------------------------------------------
	BOOL ReadRegistryValues();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this method to save controls state (options) it the registry.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: ReadRegistryValues
	// -------------------------------------------------------------------
	BOOL WriteRegistryValues();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this method to set or reset modified flag.
	// Parameters:
	//      bChanged - TRUE if options where modified, FALSE otherwise.
	// See Also: CPropertyPage::SetModified
	// -------------------------------------------------------------------
	void SetModified(BOOL bChanged = TRUE);

	//{{AFX_MSG(CXTPSyntaxEditPropertiesPageEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnChkAutoReload();
	afx_msg void OnChkHorzScrollBar();
	afx_msg void OnChkVertScrollBar();
	afx_msg void OnChkSyntaxColor();
	afx_msg void OnChkAutoIndent();
	afx_msg void OnChkSelMargin();
	afx_msg void OnChkLineNumbers();
	afx_msg void OnChangeTabsSize();
	afx_msg void OnTabsSpaces();
	afx_msg void OnTabsTab();
	afx_msg void OnCaretThin();
	afx_msg void OnCaretThick();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	BOOL                m_bModified;    // Store modified flag.
	CXTPSyntaxEditView* m_pEditView;    // Store pointer to CXTPSyntaxEditView object.
};

//===========================================================================
// Summary:
//     This class implements a property page font options.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditPropertiesPageFont : public CPropertyPage
{
	DECLARE_DYNCREATE(CXTPSyntaxEditPropertiesPageFont)

public:
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pEditView  - Pointer to CXTPSyntaxEditView object.
	CXTPSyntaxEditPropertiesPageFont(CXTPSyntaxEditView* pEditView=NULL);

	// -------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// -------------------------------------------------------------------
	virtual ~CXTPSyntaxEditPropertiesPageFont();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to get editor font.
	// Returns:
	//      A reference to CFont object.
	// See Also: GetSafeLogFont
	// -------------------------------------------------------------------
	CFont& GetEditFont();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to get editor font.
	// Parameters:
	//      lf - A reference to LOGFONT structure.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: GetEditFont
	// -------------------------------------------------------------------
	BOOL GetSafeLogFont(LOGFONT& lf);

	// -------------------------------------------------------------------
	// Summary:
	//      Use this method to change font in specified CFont object.
	//      The previous font deleted.
	// Parameters:
	//      editFont - A reference to CFont object to set new font.
	//      lf       - A new font parameters.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: CFont::CreateFontIndirect
	// -------------------------------------------------------------------
	BOOL CreateSafeFontIndirect(CFont& editFont, const LOGFONT& lf);

	//{{AFX_DATA(CXTPSyntaxEditPropertiesPageFont)
	enum { IDD = XTP_IDD_EDIT_PAGEFONT };
	CXTPSyntaxEditColorComboBox m_wndComboHiliteText;
	CXTPSyntaxEditColorComboBox m_wndComboHiliteBack;
	CXTPSyntaxEditColorComboBox m_wndComboText;
	CXTPSyntaxEditColorComboBox m_wndComboBack;
	CXTPSyntaxEditColorSampleText   m_txtSampleSel;
	CXTPSyntaxEditColorSampleText   m_txtSample;
	CButton     m_btnCustomText;
	CButton     m_btnCustomBack;
	CButton     m_btnCustomHiliteText;
	CButton     m_btnCustomHiliteBack;
	CXTPSyntaxEditTipComboBox   m_wndComboScript;
	CXTPSyntaxEditTipComboBox   m_wndComboStyle;
	CXTPSyntaxEditTipComboBox   m_wndComboSize;
	CXTPSyntaxEditTipComboBox   m_wndComboName;
	BOOL    m_bStrikeOut;
	BOOL    m_bUnderline;
	CString m_csName;
	CString m_csSize;
	CString m_csStyle;
	COLORREF m_crHiliteText;
	COLORREF m_crHiliteBack;
	COLORREF m_crText;
	COLORREF m_crBack;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CXTPSyntaxEditPropertiesPageFont)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL

protected:

	// -------------------------------------------------------------------
	// Summary:
	//     Initialize font name combo box.
	// -------------------------------------------------------------------
	void InitFontCombo();

	// -------------------------------------------------------------------
	// Summary:
	//     Initialize font style combo box.
	// -------------------------------------------------------------------
	void InitStyleCombo();

	// -------------------------------------------------------------------
	// Summary:
	//     Initialize font size combo box.
	// -------------------------------------------------------------------
	void InitSizeCombo();

	// -------------------------------------------------------------------
	// Summary:
	//     Initialize font script combo box.
	// -------------------------------------------------------------------
	void InitScriptCombo();

	// -------------------------------------------------------------------
	// Summary:
	//     Initialize color combo box.
	// -------------------------------------------------------------------
	void InitColorComboxes();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member function to update font for a sample text control.
	// -------------------------------------------------------------------
	void UpdateSampleFont();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member function to update colors for a sample text control.
	// -------------------------------------------------------------------
	void UpdateSampleColors();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this helper function to get currently selected list box item
	//      text and update combo box.
	// Parameters:
	//      comboBox    - [in] A references to combobox object.
	//      csItemText  - [out] A references to CString to store text.
	// Returns:
	//      Selected item index or CB_ERR.
	// See Also: CComboBox::GetLBText
	// -------------------------------------------------------------------
	int GetLBText(CComboBox& comboBox, CString& csItemText);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this method to load controls state (options) from the registry.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: WriteRegistryValues
	// -------------------------------------------------------------------
	BOOL ReadRegistryValues();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this method to save controls state (options) it the registry.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: ReadRegistryValues
	// -------------------------------------------------------------------
	BOOL WriteRegistryValues();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this method to set or reset modified flag.
	// Parameters:
	//      bChanged - TRUE if options where modified, FALSE otherwise.
	// See Also: CPropertyPage::SetModified
	// -------------------------------------------------------------------
	void SetModified(BOOL bChanged = TRUE);

	//{{AFX_MSG(CXTPSyntaxEditPropertiesPageFont)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeComboNames();
	afx_msg void OnSelChangeComboStyles();
	afx_msg void OnSelChangeComboSizes();
	afx_msg void OnChkStrikeOut();
	afx_msg void OnChkUnderline();
	afx_msg void OnSelEndOkScript();
	afx_msg void OnBtnCustomText();
	afx_msg void OnBtnCustomBack();
	afx_msg void OnBtnCustomHiliteText();
	afx_msg void OnBtnCustomtHiliteBack();
	afx_msg void OnSelEndOkHiliteText();
	afx_msg void OnSelEndOkHiliteBack();
	afx_msg void OnSelEndOkText();
	afx_msg void OnSelEndOkBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	const UINT m_uFaceSize;

	BYTE                m_iCharSet;     // Store char set.
	BOOL                m_bModified;    // Store modified flag.
	CXTPSyntaxEditView* m_pEditView;    // Store pointer to CXTPSyntaxEditView object.
	CFont               m_editFont;     // Store editor font.
};


//---------------------------------------------------------------------------

AFX_INLINE CFont& CXTPSyntaxEditPropertiesPageFont::GetEditFont() {
	return m_editFont;
}

//===========================================================================
// Summary:
//     This class implements a property page colors options.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditPropertiesPageColor : public CPropertyPage
{
	DECLARE_DYNCREATE(CXTPSyntaxEditPropertiesPageColor)

public:
	// -------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pEditView  - Pointer to CXTPSyntaxEditView object.
	// -------------------------------------------------------------------
	CXTPSyntaxEditPropertiesPageColor(CXTPSyntaxEditView* pEditView=NULL);

	// -------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// -------------------------------------------------------------------
	virtual ~CXTPSyntaxEditPropertiesPageColor();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this method to save controls state (options) it the registry.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// -------------------------------------------------------------------
	BOOL WriteRegistryValues();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this method to set or reset modified flag.
	// Parameters:
	//      bChanged - TRUE if options where modified, FALSE otherwise.
	// See Also: CPropertyPage::SetModified
	// -------------------------------------------------------------------
	void SetModified(BOOL bChanged = TRUE);

	//{{AFX_DATA(CXTPSyntaxEditPropertiesPageColor)
	enum { IDD = XTP_IDD_EDIT_PAGECOLOR };
	CXTPSyntaxEditColorSampleText   m_txtSampleSel;
	CXTPSyntaxEditColorSampleText   m_txtSample;
	CXTPSyntaxEditColorComboBox m_wndComboHiliteText;
	CXTPSyntaxEditColorComboBox m_wndComboHiliteBack;
	CXTPSyntaxEditColorComboBox m_wndComboText;
	CXTPSyntaxEditColorComboBox m_wndComboBack;
	CButton     m_btnBold;
	CButton     m_btnItalic;
	CButton     m_btnUnderline;
	CButton     m_btnCustomText;
	CButton     m_btnCustomBack;
	CButton     m_btnCustomHiliteText;
	CButton     m_btnCustomHiliteBack;
	CXTPSyntaxEditTipListBox    m_lboxName;
	CXTPSyntaxEditTipListBox    m_lboxProp;
	CStatic     m_gboxSampleText;
	BOOL    m_bBold;
	BOOL    m_bItalic;
	BOOL    m_bUnderline;
	COLORREF m_crHiliteText;
	COLORREF m_crHiliteBack;
	COLORREF m_crText;
	COLORREF m_crBack;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CXTPSyntaxEditPropertiesPageColor)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	public:
	virtual BOOL OnApply();
	//}}AFX_VIRTUAL

protected:

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member function to update font for a sample text control.
	// -------------------------------------------------------------------
	void UpdateSampleColors();

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member function to update font for a sample text control.
	// -------------------------------------------------------------------
	void UpdateFont();

	// -------------------------------------------------------------------
	// Summary:
	//      Initialize dialog controls for specified lex schema.
	// Parameters:
	//      pSchemaInfo - A pointer to XTP_EDIT_SCHEMAFILEINFO object.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// -------------------------------------------------------------------
	BOOL InitSchemaClasses(XTP_EDIT_SCHEMAFILEINFO* pSchemaInfo);

	// -------------------------------------------------------------------
	// Summary:
	//      Initialize dialog controls for specified lex class.
	// Parameters:
	//      infoClass - A reference to XTP_EDIT_LEXCLASSINFO object.
	// -------------------------------------------------------------------
	void InitClassData(const XTP_EDIT_LEXCLASSINFO& infoClass);

	// -------------------------------------------------------------------
	// Summary:
	//     Determine is specified class a top level schema class.
	// Parameters:
	//      infoClass - A reference to XTP_EDIT_LEXCLASSINFO object.
	// Returns:
	//      TRUE if specified class is a top level schema class, FALSE otherwise.
	// -------------------------------------------------------------------
	BOOL IsTopLevelClass(const XTP_EDIT_LEXCLASSINFO& infoClass);

	// -------------------------------------------------------------------
	// Summary:
	//     Update controls state (enabled/disabled).
	// -------------------------------------------------------------------
	void EnableControls();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to get lex property as a comma separated
	//      string.
	// Parameters:
	//      infoProp - A reference to XTP_EDIT_LEXPROPINFO object.
	// Returns:
	//      A comma separated string.
	// See Also: XTP_EDIT_LEXPROPINFO
	// -------------------------------------------------------------------
	CString GetPropValue(const XTP_EDIT_LEXPROPINFO& infoProp) const;

	// -------------------------------------------------------------------
	// Summary:
	//     Set defaults controls state.
	// -------------------------------------------------------------------
	void SetDefaults();

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to determine is specified properties
	//      data exists in the provided array.
	// Parameters:
	//      arrProp      - A properties data array.
	//      lpszPropName - A lex property name.
	// Returns:
	//      Property data index in the array or -1
	// -------------------------------------------------------------------
	int PropExists(CXTPSyntaxEditLexPropInfoArray& arrProp, LPCTSTR lpszPropName);

	// -------------------------------------------------------------------
	// Summary:
	//     Use this member function to update font parameters for currently
	//     selected lex class.
	// Parameters:
	//      lpszPropName - A lex class property name (font attribute).
	//      bValue       - A property value.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: UpdateColorValue
	// -------------------------------------------------------------------
	BOOL UpdateFontValue(BOOL& bValue, LPCTSTR lpszPropName);

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to update colors parameters for currently
	//      selected lex class.
	// Parameters:
	//      combo        - An edit color combo box.
	//      color        - A color value.
	//      lpszPropName - A lex class property name (color attribute).
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also: UpdateFontValue
	// -------------------------------------------------------------------
	BOOL UpdateColorValue(CXTPSyntaxEditColorComboBox& combo, COLORREF& color, LPCTSTR lpszPropName);

	// -------------------------------------------------------------------
	// Summary:
	//      Use this member function to retrieve a display name property
	//      value for a lex class.
	// Parameters:
	//      info - A reference to XTP_EDIT_LEXCLASSINFO object.
	// Returns:
	//      Display name property value for a lex class.
	// -------------------------------------------------------------------
	CString GetDisplayName(const XTP_EDIT_LEXCLASSINFO& info) const;

	//{{AFX_MSG(CXTPSyntaxEditPropertiesPageColor)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnCustomText();
	afx_msg void OnBtnCustomBack();
	afx_msg void OnBtnCustomHiliteText();
	afx_msg void OnBtnCustomtHiliteBack();
	afx_msg void OnChkBold();
	afx_msg void OnChkItalic();
	afx_msg void OnChkUnderline();
	afx_msg void OnSelEndOkHiliteText();
	afx_msg void OnSelEndOkHiliteBack();
	afx_msg void OnSelEndOkText();
	afx_msg void OnSelEndOkBack();
	afx_msg void OnSelChangeSchemaNames();
	afx_msg void OnSelChangeSchemaProp();
	afx_msg void OnDblClickSchema();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL                                    m_bModified;        // Store modified flag.
	CXTPSyntaxEditView*                     m_pEditView;        // Store pointer to CXTPSyntaxEditView object.
	CXTPSyntaxEditConfigurationManagerPtr   m_ptrConfigMgr;     // Store pointer to Configuration Manager.
	CXTPSyntaxEditTextSchemesManager*       m_pTextSchemesMgr;  // Store pointer to Schemes Manager.
	CFont                                   m_editFont;         // Store editor font.
	CMapStringToPtr                         m_mapLexClassInfo;  // Map lex schema name to schema classes array (CXTPSyntaxEditLexClassInfoArray*).
	CXTPSyntaxEditLexClassInfoArray*        m_parLexClassInfo;  // Store currently selected schema classes.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITPROPERTIESPAGE_H__)
