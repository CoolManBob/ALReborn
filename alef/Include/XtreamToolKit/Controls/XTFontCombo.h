// XTFontCombo.h interface for the CXTFontCombo class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTFONTCOMBO_H__)
#define __XTFONTCOMBO_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



// ----------------------------------------------------------------------
// Summary:
//     List used for enumerating CXTLogFont structures.
// Remarks:
//     CList definition used by the CXTFontEnum class to maintain
//     a list of CXTLogFont objects representing each font displayed
//     for a CXTFontListBox control.
// See Also:
//     CXTFontEnum::GetFontList
// ----------------------------------------------------------------------
typedef CList<CXTLogFont, CXTLogFont&> CXTFontList;

//===========================================================================
// Summary:
//     CXTFontEnum is a stand alone singleton font enumeration class. It is
//     used to enumerate a list of fonts found installed for the operating
//     system.
// Remarks:
//     CXTFontEnum is a singleton class, which means it can only be
//     instantiated a single time. The constructor is private, so the only
//     way to access members of this class is to use the objects Get() method.
//     To retrieve a list of available fonts for your operating system, you
//     would make the following call:
//
// Example:
//     The following example demonstrates the use of the CXTFontEnum class.
// <code>
// CXTFontEnum::Get().GetFontList()
// </code>
//===========================================================================
class _XTP_EXT_CLASS CXTFontEnum
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFontEnum object. CXTFontEnum is a singleton
	//     class, to instantiate an object, use the static method Get().
	//-----------------------------------------------------------------------
	CXTFontEnum();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTFontEnum object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTFontEnum();


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This static member function will return a reference to the one
	//     and only CXTFontEnum object. You can use this function to access
	//     data members for the CXTFontEnum class.
	// Example:
	//     The following example demonstrates the use of Get.
	// <code>
	// CXTFontEnum::Get().GetFontList()
	// </code>
	// Returns:
	//     A reference to a CXTFontEnum object.
	//-----------------------------------------------------------------------
	static CXTFontEnum& AFX_CDECL Get();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine the existence of the font
	//     specified by 'strFaceName'.
	// Parameters:
	//     strFaceName - Reference to a NULL terminated string that represents
	//                   the font name.
	// Returns:
	//     true if the font exists, otherwise returns false.
	//-----------------------------------------------------------------------
	bool DoesFontExist(CString& strFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get a pointer to the font specified
	//     by 'strFaceName'.
	// Parameters:
	//     strFaceName - A NULL terminated string that represents the font
	//                   name.
	// Returns:
	//     A pointer to the CXTLogFont structure for the specified item,
	//     or NULL if no font was found.
	//-----------------------------------------------------------------------
	CXTLogFont* GetLogFont(const CString& strFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used by the callback function to retrieve the
	//     current width for the longest font name in the list.
	// Returns:
	//     An integer value that represents the width for the longest font
	//     in the list.
	//-----------------------------------------------------------------------
	int GetMaxWidth();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to get a reference to the font list.
	// Returns:
	//     A reference to the CXTFontList used by this class.
	//-----------------------------------------------------------------------
	CXTFontList& GetFontList();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTFontEnum class to initialize
	//     the font list. You can also call this member function to reinitialize
	//     the font enumeration. For example, if you changed printers and you want
	//     to enumerate printer fonts, or you wanted to use a different character
	//     set.
	// Parameters:
	//     pDC      - Points to a valid device context, if NULL, the screen
	//                device context is used.
	//     nCharSet - Represents the character set to enumerate.
	//-----------------------------------------------------------------------
	void Init(CDC* pDC = NULL, BYTE nCharSet = DEFAULT_CHARSET);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     The EnumFontFamExProc function is an application defined-callback
	//     function used with the EnumFontFamiliesEx function. It is used to process
	//     the fonts and is called once for each enumerated font. The FONTENUMPROC
	//     type defines a pointer to this callback function. EnumFontFamExProc
	//     is a placeholder for the application defined-function name.
	// Parameters:
	//     pelf       - Pointer to an ENUMLOGFONTEX structure that contains
	//                  information about the logical attributes of the font.
	//     lpntm      - Pointer to a structure that contains information
	//                  about the physical attributes of a font. The function
	//                  uses the NEWTEXTMETRICEX structure for TrueType fonts;
	//                  and the TEXTMETRIC structure for other fonts.
	//     dwFontType - Specifies the type of the font. This parameter can be a combination
	//              of the values in the Remarks section.
	//     lParam - Specifies the application-defined data passed by the EnumFontFamiliesEx
	//     function.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:<p/>
	//          * <b>DEVICE_FONTTYPE</b> The font is a device font.
	//          * <b>RASTER_FONTTYPE</b> The font is a raster font.
	//          * <b>TRUETYPE_FONTTYPE</b> The font is a TrueType font.
	// Returns:
	//     The return value must be a nonzero value to continue enumeration.
	//     To stop enumeration, the return value must be zero.
	//-----------------------------------------------------------------------
	static BOOL CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* lpntm, DWORD dwFontType, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the font enumeration callback to
	//     add a font to the font list.
	// Parameters:
	//     pLF    - Points to a valid LOGFONT structure.
	//     dwType - Specifies the type of the font. This parameter can be a combination
	//              of the values in the Remarks section.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:<p/>
	//          * <b>DEVICE_FONTTYPE</b> The font is a device font.
	//          * <b>RASTER_FONTTYPE</b> The font is a raster font.
	//          * <b>TRUETYPE_FONTTYPE</b> The font is a TrueType font.
	// Returns:
	//     true if successful, otherwise returns false.
	//-----------------------------------------------------------------------
	bool AddFont(const LOGFONT* pLF, DWORD dwType);

protected:
	CXTFontList m_listFonts; // List of fonts found during enumeration
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CXTFontList& CXTFontEnum::GetFontList() {
	return m_listFonts;
}

// -------------------------------------------------------------------
// Summary:
//     Enumeration used to determine font display.
// Remarks:
//     XTFontStyle type defines the constants used by the CXTFontListBox
//     class to determine how the fonts will be displayed in the CXTFontListBox
//     control.
// See Also:
//     CXTFontListBox, CXTFontListBox::SetListStyle
//
// <KEYWORDS xtFontGUI, xtFontSample, xtFontBoth>
// -------------------------------------------------------------------
enum XTFontStyle
{
	xtFontUnknown   = 0x00, // Display type not defined.
	xtFontGUI       = 0x01, // Display font name with GUI font style.
	xtFontSample    = 0x02, // Display font name with its own font style.
	xtFontBoth      = 0x03, // Display font name with GUI font style, then a sample display to the right.
};

//===========================================================================
// Summary:
//     CXTFontListBox is a CXTListBox derived class. It is used to create
//     a font selection list box.
// Remarks:
//     You can choose to display the font name
//     with the GUI font style, display the font name with its own font style,
//     or display the font name with the default GUI font style and a sample
//     display to the right.
//===========================================================================
class _XTP_EXT_CLASS CXTFontListBox : public CXTListBox
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFontListBox object
	//-----------------------------------------------------------------------
	CXTFontListBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTFontListBox object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTFontListBox();

public:

	// ------------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the font for the currently selected
	//     item.
	// Parameters:
	//     lf -           Reference to an CXTLogFont structure.
	//     strFaceName -  A reference to a valid CString object to receive the
	//                    logfont face name.
	// Returns:
	//     true if successful, otherwise returns false.
	// ------------------------------------------------------------------------
	virtual bool GetSelFont(CXTLogFont& lf);
	virtual bool GetSelFont(CString& strFaceName); //<combine CXTFontListBox::GetSelFont@CXTLogFont&>

	// -------------------------------------------------------------------
	// Summary:
	//     Call this member function to select the font for the list box.
	// Parameters:
	//     lf -           Reference to an CXTLogFont structure.
	//     strFaceName -  A NULL terminated string that represents the logfont
	//                    face name.
	// Returns:
	//     true if successful, otherwise returns false.
	// -------------------------------------------------------------------
	virtual bool SetSelFont(CXTLogFont& lf);
	virtual bool SetSelFont(const CString& strFaceName); //<combine CXTFontListBox::SetSelFont@CXTLogFont&>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the font display style for the font
	//     list box. There are three styles to choose from that include displaying
	//     the font in the default GUI font, displaying the font in its own font
	//     style, or displaying both the font name in the default GUI font and
	//     a sample to the right.
	// Parameters:
	//     eStyle - Specifies the style for the font list box. See XTFontStyle for
	//              a list of available styles.
	// See Also: XTFontStyle
	//-----------------------------------------------------------------------
	void SetListStyle(DWORD dwStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to initialize the font list box and populate it
	//     with a list of available fonts.  This method should be called directly
	//     after creating or sub-classing the control.
	// Parameters:
	//     bAutoFont - True to enable automatic font initialization.
	//-----------------------------------------------------------------------
	virtual void Initialize(bool bAutoFont = true);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTFontListBox)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTFontListBox)
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:

	DWORD       m_dwStyle;     // Display style indicating how to render the font list.
	CString     m_csSymbol;    // String displayed for the symbol characters.
	CImageList  m_ilFontType;  // True type font image list.

};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFontListBox::SetListStyle(DWORD dwStyle) {
	m_dwStyle = dwStyle;
}

//===========================================================================
// Summary:
//     CXTFontCombo is a CXTFlatComboBox derived class. It is used to create
//     a combo box that displays a drop list of available fonts for your system.
//     The fonts are displayed in their various styles.
//===========================================================================
class _XTP_EXT_CLASS CXTFontCombo : public CXTFlatComboBox
{
	DECLARE_DYNAMIC(CXTFontCombo)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFontCombo object
	//-----------------------------------------------------------------------
	CXTFontCombo();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTFontCombo object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTFontCombo();

public:
	// -------------------------------------------------------------------
	// Summary:
	//     Retrieves the selected font.
	// Parameters:
	//     lf -           Reference to an CXTLogFont structure.
	//     strFaceName -  A reference to a valid CString object to receive the
	//                    logfont face name.
	// Remarks:
	//     The first version of GetSelFont will retrieve the selected font
	//     name. The second version will get the CXTLogFont for the currently
	//     selected item.
	// Returns:
	//     true if successful, otherwise returns false.
	// -------------------------------------------------------------------
	virtual bool GetSelFont(CXTLogFont& lf);
	virtual bool GetSelFont(CString& strFaceName); //<combine CXTFontCombo::GetSelFont@CXTLogFont&>

	// ----------------------------------------------------------------------
	// Summary:
	//     Sets the selected font.
	// Parameters:
	//     lf -           Reference to an CXTLogFont object.
	//     strFaceName -  A NULL terminated string that represents the logfont
	//                    face name.
	// Remarks:
	//     The first version of SetSelFont will set the selected font by
	//     using its face name. The second version will set the selected font
	//     by using a CXTLogFont object.
	// Returns:
	//     true if successful, otherwise returns false.
	// ----------------------------------------------------------------------
	virtual bool SetSelFont(CXTLogFont& lf);
	virtual bool SetSelFont(const CString& strFaceName); //<combine CXTFontCombo::SetSelFont@CXTLogFont&>

	// --------------------------------------------------------------------------
	// Summary:
	//     Sets the font style for the CXTFontCombo control.
	// Parameters:
	//     eStyle -  Specifies the style for the font list box. Styles can be any
	//               one of the values in the Remarks section.
	// Remarks:
	//     Call this member function to set the font display style for the
	//     font list box. There are three styles to choose from that include
	//     displaying the font in the default GUI font, displaying the font
	//     in its own font style, or displaying both the font name in the
	//     default GUI font and a sample to the right.
	//
	//     The style can be any of the following values:
	//
	//     * <b>xtFontGUI</b> Display font name with GUI font style.
	//     * <b>xtFontSample</b> Display font name with its own font style.
	//     * <b>xtFontBoth</b> Display font name with GUI font style,
	//           then a sample display to the right.
	// --------------------------------------------------------------------------
	void SetListStyle(DWORD dwStyle);

	// ------------------------------------------------------------------------
	// Summary:
	//     Initializes the CXTFontCombo control.
	// Parameters:
	//     lpszFaceName -  A NULL terminated string that represents the logfont
	//                     face name.
	// nWidth -        The minimum allowable width of the list box portion of
	//                     the combo box in pixels.
	// bEnable -       TRUE to enable auto completion, otherwise FALSE.
	// Remarks:
	//     Call this member function to initialize the font list box and
	//     populate it with a list of available fonts.
	// ------------------------------------------------------------------------
	virtual void InitControl(LPCTSTR lpszFaceName = NULL, UINT nWidth = 0, BOOL bEnable = TRUE);

protected:

	// ------------------------------------------------------------------------
	// Summary:
	//      Called by the font combo box to send CBN notifications to the owner
	//      window to support keyboard navigation.
	// Parameters:
	//      nCode - CBN notification message to send.
	// ------------------------------------------------------------------------
	virtual void NotifyOwner(UINT nCode);

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTFontCombo)
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTFontCombo)
	afx_msg BOOL OnDropDown();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
//}}AFX_CODEJOCK_PRIVATE

protected:
	DWORD       m_dwStyle;      // Enumerated style indicating how to display the font list.
	CString     m_csSymbol;     // String displayed for symbol characters.
	CImageList  m_ilFontType;   // true type font image list.

private:
	CString     m_csSelected;   // Selected text set when CBN_DROPDOWN is called.
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTFontCombo::SetListStyle(DWORD dwStyle) {
	m_dwStyle = dwStyle;
}

#endif // #if !defined(__XTFONTCOMBO_H__)
