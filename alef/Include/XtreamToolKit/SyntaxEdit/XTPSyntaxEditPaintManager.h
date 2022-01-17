// XTPSyntaxEditPaintManager.h interface for the CXTPSyntaxEditPaintManager class.
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
#if !defined(__XTPSYNTAXEDITPAINTMANAGER_H__)
#define __XTPSYNTAXEDITPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditCtrl;

//===========================================================================
// Summary:
//     Standalone class used by CXTPSyntaxEditCtrl as the paint manager.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditPaintManager
{
	// This constructor is called only when creating static instance
	// Load default values.
	CXTPSyntaxEditPaintManager();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSyntaxEditPaintManager object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes default paint manager bitmaps, should be called after
	//     creating control window.
	//-----------------------------------------------------------------------
	virtual void InitBitmaps();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//      Update internal text font object (if need) and select it to DC.
	// Parameters:
	//      lf  : [in] Reference to font options structure.
	// Returns:
	//      TRUE if the edit control font was updated, otherwise FALSE.
	// See also:
	//      struct LOGFONT
	//-----------------------------------------------------------------------
	BOOL UpdateTextFont(CXTPSyntaxEditCtrl* pEditCtrl, const XTP_EDIT_FONTOPTIONS& lf);

	//-----------------------------------------------------------------------
	// Summary:
	//      Prints the line numbers and returns the column position to start
	//      rest of the drawing.
	// Parameters:
	//      pDC  : [in] Pointer to device context to print to
	//      nRow : [in] The window row number for which to print
	//      nRowActual : [in] The document row number for which to print
	// Returns:
	//      The virtual start column position as integer value.
	//-----------------------------------------------------------------------
	int DrawLineNumber(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, int nRowActual);

	//-----------------------------------------------------------------------
	// Summary:
	//      Prints the line Collapsible node icon
	// Parameters:
	//      pDC  : [in] Device context to print to.
	//      nRow : [in] The window row number for which to print.
	//      nRowActual : [in] The document row number for which to print.
	//-----------------------------------------------------------------------
	void DrawLineNode(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, const DWORD& dwType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws frames around collapsed text blocks.
	// Parameters:
	//      pDC         : [in] Pointer to device context.
	//-----------------------------------------------------------------------
	void DrawCollapsedTextMarks(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws a single line.
	// Parameters:
	//      pDC    : [in] The device context for printing.
	//      szText : [in] The text to print.
	//      iLine  : [in] The line number.
	//-----------------------------------------------------------------------
	void DrawLine(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, const CString& strText, int iLine, int nRow, int iStartCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws editor text line and calculates selection position.
	// Parameters:
	//      txtBlk      : [in] Reference to XTP_EDIT_TEXTBLOCK structure.
	//      nSelStartX  : [in] Start coordinate.
	//      nSelEndX    : [in] End coordinate.
	//      arTxtBlk    : [in] Array of references to XTP_EDIT_TEXTBLOCK structures.
	//      bUseBlk     : [in] Array of references to BOOL.
	// See also:
	//  struct XTP_EDIT_TEXTBLOCK
	//-----------------------------------------------------------------------
	void DrawLineCalcSel(const XTP_EDIT_TEXTBLOCK& txtBlk,
		int nSelStartX, int nSelEndX,
		XTP_EDIT_TEXTBLOCK (&arTxtBlk)[4], BOOL (&bUseBlk)[4] );

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws marks for line.
	// Parameters:
	//      pDC         : [in] Pointer to device context.
	//      nRow        : [in] Window row identifier.
	//      nRowActual  : [in] Document row identifier.
	//-----------------------------------------------------------------------
	void DrawLineMarks(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, int nRow, int nRowActual);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Draws marks for line.
	// Parameters:
	//      pBookmark : [in] Pointer to structure with mark parameters.
	// See also:
	//      LPXTP_EDIT_SENMBOOKMARK
	//-----------------------------------------------------------------------
	void DrawLineMark(CXTPSyntaxEditCtrl* pEditCtrl, XTP_EDIT_SENMBOOKMARK* pBookmark);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws text for single line.
	// Parameters:
	//      pDC         : [in] Pointer to device context.
	//      strText     : [in] Text to print.
	//      nTextLen    : [in] Length of text.
	//      block       : [in] Reference to XTP_EDIT_TEXTBLOCK structure.
	//      ppPrevFont  : [out] Previous font
	//      iLine       : [in] Line number.
	//      iStartCol   : [in] Start column identifier.
	//      iCol        : [in] Column identifier.
	// See also:
	//      struct XTP_EDIT_TEXTBLOCK
	//-----------------------------------------------------------------------
	int DrawLineText(CXTPSyntaxEditCtrl* pEditCtrl, CDC *pDC, const CString& strText, int nTextLen,
		const XTP_EDIT_TEXTBLOCK& block, CFont** ppPrevFont,
		int iLine, int iStartCol, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draw the line numbers border    .
	// Parameters:
	//          pDC         : [in] Pointer to device context to print to
	//          rcLineNum   : [in] Line number rect
	//          clrBorder   : [in] Border color
	//-----------------------------------------------------------------------
	void DrawLineNumberBorder(CDC *pDC, const CRect& rcLineNum, const COLORREF clrBorder);

public:
	//-----------------------------------------------------------------------
	// Summary :
	//      Sets line selection cursor.
	// Parameters:
	//      hCurLine : [in] Handle to the cursor. The cursor must have been
	//                          created by the CreateCursor function or loaded
	//                          by the LoadCursor or LoadImage function.Cursor
	//                          appears in lines selection area.
	// Returns:
	//      Handle of previous cursor.
	//-----------------------------------------------------------------------
	HCURSOR SetLineSelCursor(HCURSOR hCurLine);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets cursor shape for "copy" cursor.
	// Parameters:
	//      hCurCopy : [in] Handle to the cursor. The cursor must have been
	//                      created by the CreateCursor function or loaded
	//                      by the LoadCursor or LoadImage function.
	// Returns:
	//      Handle of previous cursor.
	//-----------------------------------------------------------------------
	HCURSOR SetCopyCursor(HCURSOR hCurCopy);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets cursor shape for "move" cursor.
	// Parameters:
	//      hCurMove : [in] Handle to the cursor. The cursor must have been
	//                      created by the CreateCursor function or loaded
	//                      by the LoadCursor or LoadImage function.
	// Returns:
	//      Handle of previous cursor.
	//-----------------------------------------------------------------------
	HCURSOR SetMoveCursor(HCURSOR hCurMove);

	//-----------------------------------------------------------------------
	// Summary: Get text color.
	// Returns: Text color.
	// See Also: SetTextColor
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get background color.
	// Returns: Background color.
	// See Also: SetBackColor
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get selected text color.
	// Returns: Selected text color.
	// See Also: SetHiliteTextColor
	//-----------------------------------------------------------------------
	COLORREF GetHiliteTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get selected background color.
	// Returns: Selected background color.
	// See Also: SetHiliteBackColor
	//-----------------------------------------------------------------------
	COLORREF GetHiliteBackColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get selected text color when control has no focus.
	// Returns: Selected text color when control has no focus.
	// See Also: SetInactiveHiliteTextColor
	//-----------------------------------------------------------------------
	COLORREF GetInactiveHiliteTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get selected background color when control has no focus.
	// Returns: Selected background color when control has no focus.
	// See Also: SetInactiveHiliteBackColor
	//-----------------------------------------------------------------------
	COLORREF GetInactiveHiliteBackColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get Line Numbers text color.
	// Returns: Line Numbers text color.
	// See Also: SetLineNumberTextColor
	//-----------------------------------------------------------------------
	COLORREF GetLineNumberTextColor() const;

	//-----------------------------------------------------------------------
	// Summary: Get Line Numbers background color.
	// Returns: Line Numbers background color.
	// See Also: SetLineNumberBackColor
	//-----------------------------------------------------------------------
	COLORREF GetLineNumberBackColor() const;

	//-----------------------------------------------------------------------
	// Summary: Set text color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetTextColor
	//-----------------------------------------------------------------------
	BOOL SetTextColor(COLORREF crText, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary: Set background color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetBackColor
	//-----------------------------------------------------------------------
	BOOL SetBackColor(COLORREF crBack, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary: Set selected text color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetHiliteTextColor
	//-----------------------------------------------------------------------
	BOOL SetHiliteTextColor(COLORREF color, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set selected background color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetHiliteBackColor
	//-----------------------------------------------------------------------
	BOOL SetHiliteBackColor(COLORREF color, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set selected text color when control has no focus.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetInactiveHiliteTextColor
	//-----------------------------------------------------------------------
	BOOL SetInactiveHiliteTextColor(COLORREF color, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set selected background color when control has no focus.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetInactiveHiliteBackColor
	//-----------------------------------------------------------------------
	BOOL SetInactiveHiliteBackColor(COLORREF color, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set Line Numbers text color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetLineNumberTextColor
	//-----------------------------------------------------------------------
	BOOL SetLineNumberTextColor(COLORREF color, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set Line Numbers background color.
	// Parameters:
	//      crText      - A color value.
	//      bUpdateReg  - [in] Set TRUE to save color in registry.
	// Returns: TRUE if successful, FALSE otherwise.
	// See Also: GetLineNumberBackColor
	//-----------------------------------------------------------------------
	BOOL SetLineNumberBackColor(COLORREF color, BOOL bUpdateReg=FALSE);

public:
	// fonts
	CFont* GetFont();       // Normal text font.
	CFont* GetFontText();   // Edit text font.
	CFont* GetFontLineNumber(); // Line numbers font.
	CFont* GetFontToolTip();    // ToolTip font.

	BOOL CreateFontIndirect(LPLOGFONT pLogfont, BOOL bUpdateReg=FALSE);
	BOOL SetFont(LPLOGFONT pLogfont);       // Normal text font.
	BOOL SetFontText(LPLOGFONT pLogfont);   // Edit text font.
	BOOL SetFontLineNumber(LPLOGFONT pLogfont); // Line numbers font.
	BOOL SetFontToolTip(LPLOGFONT pLogfont);    // ToolTip font.

	// cursors
	HCURSOR GetCurLine();   // Cursor to show on line selection area
	HCURSOR GetCurMove();   // Cursor for moving text
	HCURSOR GetCurCopy();   // Cursor for copying text
	HCURSOR GetCurNO(); // Standard NO cursor
	HCURSOR GetCurIBeam();  // Standard IBEAM cursor
	HCURSOR GetCurArrow();  // Standard ARROW cursor

protected:
	// fonts
	CFont m_font;       // Default font for normal text.
	CFont m_fontText;   // Current text font
	CFont m_fontLineNumber; // The font for line numbering
	CFont m_fontToolTip;    // The font for tool tip text

	// cursors
	HCURSOR m_hCurLine; // Cursor to show on line selection area
	HCURSOR m_hCurMove; // Cursor for moving text
	HCURSOR m_hCurCopy; // Cursor for copying text
	HCURSOR m_hCurNO;   // Standard NO cursor
	HCURSOR m_hCurIBeam;    // Standard IBEAM cursor
	HCURSOR m_hCurArrow;    // Standard ARROW cursor

	CImageList m_ilBookmark;    // Image list with all bookmark images

	CPen m_penGray;             // Standard common gray pen

	XTP_EDIT_COLORVALUES   m_clrValues;    // Default color

	//-----------------------------------------------------------------------
	// Summary: Get Bookmarks image list.
	// Returns: A pointer to CImageList.
	//-----------------------------------------------------------------------
	virtual CImageList* GetBookmarks();
private:
	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the default instance of the Edit Control paint manager.
	//      It could be used for applying paint settings default for all
	//      control's instances for the process.
	// Returns:
	//      Pointer to the single default instance of the
	//      CXTPSyntaxEditPaintManager class.
	//-----------------------------------------------------------------------
	static CXTPSyntaxEditPaintManager* AFX_CDECL Instance();
	friend CXTPSyntaxEditPaintManager* AFX_CDECL XTPSyntaxEditPaintManager();
};

//===========================================================================
// Summary:
//      Use this function to get CXTPSyntaxEditPaintManager global object.
// Returns:
//      Pointer to CXTPSyntaxEditPaintManager object.
//===========================================================================
AFX_INLINE CXTPSyntaxEditPaintManager* AFX_CDECL XTPSyntaxEditPaintManager() {
	return CXTPSyntaxEditPaintManager::Instance();
}

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetTextColor() const {
	return m_clrValues.crText;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetBackColor() const {
	return m_clrValues.crBack;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetHiliteTextColor() const {
	return m_clrValues.crHiliteText;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetHiliteBackColor() const {
	return m_clrValues.crHiliteBack;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetInactiveHiliteTextColor() const {
	return m_clrValues.crInactiveHiliteText;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetInactiveHiliteBackColor() const {
	return m_clrValues.crInactiveHiliteBack;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetLineNumberTextColor() const {
	return m_clrValues.crLineNumberText;
}
AFX_INLINE COLORREF CXTPSyntaxEditPaintManager::GetLineNumberBackColor() const {
	return m_clrValues.crLineNumberBack;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITPAINTMANAGER_H__)
