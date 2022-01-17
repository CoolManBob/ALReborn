// XTPSyntaxEditCtrl.h : header file
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
#if !defined(__XTPSYNTAXEDITSYNTAXEDITCTRL_H__)
#define __XTPSYNTAXEDITSYNTAXEDITCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace XTPSyntaxEditLexAnalyser;

//{{AFX_CODEJOCK_PRIVATE
typedef CMap<int,int,COLORREF,COLORREF> CXTPSyntaxEditRowColorMap;
//}}AFX_CODEJOCK_PRIVATE

// forwards
class CXTPSyntaxEditBufferManager;

//===========================================================================
// Summary: This class is the main CWnd - based editor class. It is a rectangular
//          child window in which the user can enter and edit text.
//
//          The control is also capable of enhance text coloring for a number
//          of specific input file types, e.c. C/C++ source files, HTML files, etc.
//          User can create and customize his own text files schemes - they are
//          independent of source files.
//          The control contains the main configuration file, which can contain
//          references to any specific configuration files. By default, this
//          file is placed in the same folder as the application executable and
//          is called <i>EditControl.ini</i>.
//          The main configuration file contains 2 blocks with references to
//          input file lexicographic schemes in the specific format (see below)
//          and color themes for the editor, which could be applied to the
//          control editing any file type. The format of the main configuration
//          file is the following:
//<code>
//[Schemas]
//FileType1=SchemaFileName1.schclass
//FileType2=SchemaFileName2.schclass
//...
//
//[Themes]
//ThemeName1=ThemeFileName1.ini
//ThemeName2=ThemeFileName2.ini
//...
//</code>
//
// Example:
// See also: <LINK XTPSyntaxEditLexSchemas.h, LexSchemas>
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditCtrl : public CWnd
{
	DECLARE_DYNAMIC(CXTPSyntaxEditCtrl)
	DECLARE_XTP_SINK(CXTPSyntaxEditCtrl, m_Sink)

	friend class CXTPSyntaxEditAutoCompleteWnd;
	friend class CXTPSyntaxEditView;
	friend class CXTPSyntaxEditCommand;
	friend class CXTPSyntaxEditPaintManager;
	friend class CSyntaxEditCtrl;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditCtrl object, handles cleanup and de- allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditCtrl();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if line numbering is enabled or not.
	// Returns:
	//     TRUE if line numbering is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetLineNumbers() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables or disables line numbering.
	// Parameters:
	//     bPrintLineNum: [in] Pass TRUE to enable line numbering.
	//-----------------------------------------------------------------------
	BOOL SetLineNumbers(BOOL bLineNumbers, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Replaces all the tabs with blank spaces.
	// Returns:
	//     TRUE if Untabify done successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL UnTabifySelection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Replaces all the blank spaces with tabs.
	// Returns:
	//     TRUE if Tabify done successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL TabifySelection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Turns selected text to the lowercase.
	// Returns:
	//     TRUE if performs successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL LowercaseSelection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Turns selected text to the uppercase.
	// Returns:
	//     TRUE if performs successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL UppercaseSelection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the current selection.
	// Parameters:
	//     ptStart: [in] Start position of the selection.
	//     ptEnd:   [in] End position of the selection.
	// Returns:
	//     TRUE if valid selection exists, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetCurSel(CPoint& ptStart, CPoint& ptEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the number of rows can be printed per page.
	// Returns:
	//     int value determines the number of rows can be printed in current
	//     settings.
	//-----------------------------------------------------------------------
	int GetRowPerPage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns visibility status for given row.
	// Parameters:
	//     iRow: [in] The document row number to be tested.
	// Remarks:
	//     Call this member function to determine if a row of the document is
	//     visible or not.
	// Returns:
	//     TRUE if the row is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsRowVisible(int iRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets right mouse button for dragging.
	// Parameters:
	//     bRightButtonDrag: [in] Boolean state flag of right button drag mode.
	// Remarks:
	//     Mainly used by <b>CXTPSyntaxEditView</b> for setting right button
	//     drag mode. If parameter is TRUE function sets right button for
	//     dragging, otherwise dragging mode is disabled for right mouse
	//     button.
	// See also:
	//     void CancelRightButtonDrag(), BOOL IsRightButtonDrag()
	//-----------------------------------------------------------------------
	void SetRightButtonDrag(BOOL bRightButtonDrag = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets scroll bars to enable/disable state.
	// Parameters:
	//     bHorz: [in] TRUE enables horizontal scroll bar; otherwise disables.
	//     bVert: [in] TRUE enables vertical scroll bar; otherwise disables.
	// Summary:
	//     Call this member function to  change state for scroll bars. This
	//     function operates on either vertical and horizontal scroll bars
	//     simultaneously. Disabled scroll bar is not visible.
	// Returns:
	//      TRUE if scrollbars were updated, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SetScrollBars(BOOL bHorzSBar, BOOL bVertSBar, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the if horizontal scrollbar is enabled.
	// Returns:
	//      TRUE if horizontal scrollbar is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetHorzScrollBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the if vertical scrollbar is enabled.
	// Returns:
	//      TRUE if vertical scrollbar is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetVertScrollBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets gutter to enable/disable state.
	// Parameters:
	//     bEnabled: [in] Boolean state flag of gutter.
	// Summary:
	//     Call this member function to  change state for gutter. Passing TRUE
	//     you enable gutter. Otherwise gutter is disabled. Disabled gutter is
	//     not visible.
	// See also:
	//     BOOL GetSelMargin()
	//-----------------------------------------------------------------------
	BOOL SetSelMargin(BOOL bSelMargin, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns gutter state.
	// Remarks:
	//     Call this member function to determine current state of gutter.
	//     It may be enabled or disabled. Disabled gutter is not visible and
	//     inactive.
	// Returns:
	//     TRUE if gutter is enabled, FALSE otherwise
	// See also:
	//     void SetSelMargin(BOOL bEnabled = TRUE)
	//-----------------------------------------------------------------------
	BOOL GetSelMargin() const;

	//-----------------------------------------------------------------------
	// Summary:
	//    Sets syntax colorization state.
	// Parameters:
	//     bEnable: [in] Boolean flag determines if colorization is enabled or
	//              disabled.
	// Returns:
	//     Call this member function to change state syntax colorization.
	//     Passing TRUE you enable syntax colorization, otherwise it is
	//     disabled.
	// See also:
	//     BOOL GetSyntaxColor()
	//-----------------------------------------------------------------------
	BOOL SetSyntaxColor(BOOL bSyntaxColor, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Return state of syntax colorization.
	// Summary:
	//     Call  this member function to determine current syntax colorization
	//     state.
	// Returns:
	//     TRUE if syntax colorization is enabled, FALSE otherwise.
	// See also:
	//     void SetSyntaxColor(BOOL bEnable = TRUE)
	//-----------------------------------------------------------------------
	BOOL GetSyntaxColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets auto indentation state.
	// Parameters:
	//     bEnable: [in] boolean flag determines state auto indent.
	// Remarks:
	//     Call this member function change state of auto indentation. Passing
	//     TRUE you enable it, disable otherwise.
	// See also:
	//     BOOL GetAutoIndent()
	//-----------------------------------------------------------------------
	BOOL SetAutoIndent(BOOL bAutoIndent, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns state of auto intend.
	// Remarks:
	//     Call this member function to determine current auto intend state.
	// Returns:
	//     TRUE if syntax auto indent is enabled, FALSE otherwise.
	// See also:
	//     void SetAutoIndent(BOOL bEnable = TRUE)
	//-----------------------------------------------------------------------
	BOOL GetAutoIndent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the tab size.
	// Parameters:
	//     iTabSize: [in] The tab size to set should be between 2 to 10.
	// Remarks:
	//     Call this member function to set tab size. Size is measured in
	//     space character and should be between 2 to 10.
	// See also:
	//     int GetTabSize()
	//-----------------------------------------------------------------------
	BOOL SetTabSize(int nTabSize, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the tab size.
	// Returns:
	//     Actual tab size is number of character.
	// See also:
	//     void SetTabSize(int iTabSize)
	//-----------------------------------------------------------------------
	int GetTabSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the status if tab should be replaced by space.
	// Returns:
	//     Returns TRUE if tab is enabled, FALSE otherwise.
	// See also:
	//     void SetTabWithSpace(BOOL bReplace = TRUE);
	//-----------------------------------------------------------------------
	BOOL GetTabWithSpace() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables replacing tabs with spaces.
	// Parameters:
	//     bEnable: [in] Boolean flag determines enable/disable state.
	// Remarks:
	//     Call this member function to enable/disable replacing tabs with space
	//     Pass TRUE if tab should be replaced with space, FALSE otherwise.
	// See also:
	//     BOOL GetTabWithSpace()
	//-----------------------------------------------------------------------
	BOOL SetTabWithSpace(BOOL nTabWithSpace, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the text color of the specified row in the colors cache.
	// Parameters:
	//      nRow     : [in] A row identifier for changing the foreground color.
	//      clrFront : [in] An actual text color foreground.
	// See also:
	//      GetRowColor(int nRow);
	//-----------------------------------------------------------------------
	void SetRowColor(int nRow, COLORREF clrFront);

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets the text color of the specified row in the colors cache.
	// Parameters:
	//      nRow    : [in] A row identifier for getting the foreground color.
	// Returns:
	//      The actual text color foreground as a COLORREF value.
	// See also:
	//      SetRowColor(int nRow);
	//-----------------------------------------------------------------------
	COLORREF GetRowColor(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the background color of a specified row in the colors cache.
	// Parameters:
	//      nRow    : [in] A row identifier to set the background color.
	//      crBack : [in] A background color
	// See also:
	//      COLORREF GetRowBkColor(int nRow);
	//-----------------------------------------------------------------------
	void SetRowBkColor(int nRow, COLORREF crBack);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the background color of the specified row in the colors cache.
	// Parameters:
	//      nRow     - Specifies the document row.
	// Returns:
	//      COLORREF value of the current background color
	// See also:
	//      void SetRowBkColor(int nRow, COLORREF crBack);
	//-----------------------------------------------------------------------
	COLORREF GetRowBkColor(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the status if white space should be shown or not.
	// Remarks:
	//     Call this member function to determine if currently white space
	//     should be shown or not.
	// Returns:
	//     TRUE if white space should be shown, FALSE otherwise.
	// See also:
	//     void EnableWhiteSpace(BOOL bShow = TRUE)
	//-----------------------------------------------------------------------
	BOOL IsEnabledWhiteSpace();

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables showing white spaces.
	// Parameters:
	//     bShow: [in] Boolean flag determines if white space should be shown
	//            or not
	// Summary:
	//     Call tis member function to enable or disable showing white spaces
	//     facility. Pass TRUE if white space should be shown, FALSE otherwise.
	// See also:
	//     BOOL IsEnabledWhiteSpace()
	//-----------------------------------------------------------------------
	void EnableWhiteSpace(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns state flag of mouse right button for drag or not.
	// Remarks:
	//     Call this member function to determine if mouse right button is
	//     currently enabled for dragging.
	// Returns:
	//     TRUE if right button drag is enabled, FALSE otherwise.
	// See also:
	//     void SetRightButtonDrag(BOOL bRightButtonDrag = TRUE)
	//     void CancelRightButtonDrag();
	//-----------------------------------------------------------------------
	BOOL IsRightButtonDrag();

	//-----------------------------------------------------------------------
	// Summary:
	//     Cancels right button drag.
	// Remarks:
	//     Call this member function to cancel right button drag. It is mainly
	//     by by CXTPSyntaxEditView class.
	// See also:
	//   void SetRightButtonDrag(BOOL bRightButtonDrag = TRUE),
	//   IsRightButtonDrag()
	//   CXTPSyntaxEditView
	//-----------------------------------------------------------------------
	void CancelRightButtonDrag();

	//-----------------------------------------------------------------------
	// Summary:
	//     Displays the default context menu.
	// Remarks:
	//     This member function calls OnContextMenu().
	// See also:
	//     OnContextMenu()
	//-----------------------------------------------------------------------
	void ShowDefaultContextMenu();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets margins for printed page.
	// Parameters:
	//     iTop:    [in] Top margin.
	//     iLeft:   [in] Left margin.
	//     iBottom: [in] Bottom margin.
	//     iRight:  [in] Right margin.
	// Summary:
	//     Call this member function to specify printed page margins. Page
	//     margins are determined by the top, left, right and bottom corners
	//-----------------------------------------------------------------------
	void SetPrintMargins(int iTop, int iLeft, int iBottom, int iRight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Handle the drop operation within the same edit buffer.
	// Parameters:
	//      bCopy: [in] TRUE if drag is to copy, FALSE to specify a move.
	//-----------------------------------------------------------------------
	void HandleDrop(BOOL bCopy);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set the current row and column.
	// Parameters:
	//     iRow: [in] current row index.
	//     iCol: [in] current row column index.
	//-----------------------------------------------------------------------
	void SetCurRowCol(int iRow, int iCol);

	//-----------------------------------------------------------------------
	// Summary: Call this method to move to the row
	// Parameters:
	//     iRow       - [in] row index.
	//     bSelectRow - [in] select row if TRUE.
	//-----------------------------------------------------------------------
	void GoToRow(int iRow, BOOL bSelectRow = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Replaces a selection with the supplied text.
	// Parameters:
	//     szNewText: [in] The text to be pasted into the selected part.
	//     bRedraw:   [in] TRUE if should be redrawn, FALSE otherwise.
	// Returns:
	//     TRUE if replaced, FALSE otherwise
	//-----------------------------------------------------------------------
	BOOL ReplaceSel(LPCTSTR szNewText, BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the selected text.
	// Parameters:
	//      strText: [out] Reference to a CString object where returned text
	//               will be placed.
	// Returns:
	//     Number of character in the returned text.
	//-----------------------------------------------------------------------
	int GetSelectionText(CString& strText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the current selected rectangle area.
	// Parameters:
	//     iRow1: [in] Specifies the start row.
	//     iCol1: [in] Specifies the start column.
	//     iRow2: [in] Specifies the end row.
	//     iCol2: [in] Specifies the end column.
	// See also:
	//     void Unselect()
	//-----------------------------------------------------------------------
	void SetCurSel(int iRow1, int iCol1, int iRow2, int iCol2);

	//-----------------------------------------------------------------------
	// Summary:
	//     Cancels the previously selected rectangle area.
	// See also:
	//     void SetCurSel(int iRow1, int iCol1, int iRow2, int iCol2)
	//-----------------------------------------------------------------------
	void Unselect();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the drop position.
	// Parameters:
	//     iRow: [in] Row of the drop position.
	//     iCol: [in] Col of the drop position.
	// Remarks:
	//     Call this member function to set the drop position. Mainly used by
	//     CXTPSyntaxEditView during OLE drag drop operation.
	// See also:
	//     CXTPSyntaxEditView class
	//-----------------------------------------------------------------------
	void SetDropPos(int iRow, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the current absolute horizontal text position.
	// Summary:
	//     Call this member function to determine the current absolute text
	//     position, mainly used in text insert or delete operations.
	//     Treats tabs as one character.
	// Returns:
	//     Return absolute column position.
	//-----------------------------------------------------------------------
	int GetCurAbsCol();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a handle of selection buffer.
	// Parameters:
	//     nFormat: [in] Determines clipboard format to be returned
	//              (CF_TEXT or CF_UNICODETEXT)
	// Summary:
	//     Call this member function get a handle of a selection buffer in the
	//     global memory in one of the clipboard format of CF_TEXT or
	//     CF_UNICODETEXT. Mainly used in OLE drag drop operation.
	// Returns:
	//     Handle of a globally allocated memory.
	//-----------------------------------------------------------------------
	HGLOBAL GetSelectionBuffer(UINT nFormat);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables or disables OLE drag drop mode.
	// Parameters:
	//     bEnableDrag: [in] Determines state of OLE drag drop.
	// Remarks:
	//     CAll this member function toenablr/disable OLE drag drop. Pass TRUE
	//     to enable OLE drag drop, otherwise pass FALSE.
	//-----------------------------------------------------------------------
	void EnableOleDrag(BOOL bEnableDrag = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Validates and set the row column to a valid one.
	// Parameters:
	//     nRow:    [in]     Document row to be validated.
	//     nCol:    [in out] Col to be validated.
	//     nAbsCol: [in out] Absolute col to be validated.
	//-----------------------------------------------------------------------
	void ValidateCol(const int nRow, int& nCol, int& nAbsCol);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sends notification when current text position was changed.
	// Parameters:
	//     nRow: [in] Row to be notified in document coordinates.
	//     nCol: [in] Column to be notified.
	// Remarks:
	//     Call this member function to send notification to the parent class
	//     about the row col changes.
	//-----------------------------------------------------------------------
	void NotifyCurRowCol(int iRow, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE or FALSE depending on, if the specified row, col lines
	//     on the current selection.
	// Parameters:
	//     iRow :   [in] row for lookup.
	//     iCol :   [in] column for lookup.
	// Returns: TRUE if the specified row,col is in selection
	//-----------------------------------------------------------------------
	BOOL RowColInSel(int iRow, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the row and col (Not validated) for a specific point. Use
	//     CalcValidDispCol for validation.
	// Parameters:
	//          pt :        [in]  The point in client co-ordinate.
	//          pRow :      [out] The out parameter contains the calculated
	//                             document row.
	//          pCol :      [out] The out parameter contains the calculated col.
	//          pDispRow :  [out] The out parameter contains the calculated
	//                            visible row.
	//          pDispCol :  [out] The out parameter contains the calculated
	//                            visible col.
	// Returns:
	//      BOOL : True if point was out of bound and index was corrected
	//-----------------------------------------------------------------------
	BOOL RowColFromPoint(CPoint pt, int *pRow, int *pCol, int *pDispRow = NULL, int *pDispCol = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes tab from the front of a selection.
	// Returns:
	//     BOOL : True if successful, false if not.
	//-----------------------------------------------------------------------
	BOOL DecreaseIndent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Inserts tab in front of a selection.
	// Returns:
	//     BOOL - True if successful, false if not.
	//-----------------------------------------------------------------------
	BOOL IncreaseIndent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes all the lines in a selection or a specific line in the case
	//     if no selection exists.
	// Parameters:
	//     iForceDeleteRow : [in] The document row to delete.
	//-----------------------------------------------------------------------
	void DeleteSelectedLines(int iForceDeleteRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Shows or hides the scroll bar.
	// Parameters:
	//     bShow : [in] Determines if a horizontal scrollbar should be shown.
	//                  Pass TRUE to show FALSE otherwise. Default value is
	//                  TRUE.
	//-----------------------------------------------------------------------
	void ShowHScrollBar(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Copy or move text after dragging.
	// Parameters:
	//     bIsCtrl : [in] TRUE if user pressed ctrl (for copy).
	//-----------------------------------------------------------------------
	void CopyOrMoveText(BOOL bCopy);

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds a certain text with some parameters for find.
	// Parameters:
	//     szText           : [in] Pointer to a text string to be found.
	//     bMatchWholeWord  : [in] Find text matching the whole word.
	//     bMatchCase       : [in] Try to match case.
	//     bSearchDown      : [in] TRUE for searching downward.
	// Returns:
	//     TRUE if found, FALSE if not
	//-----------------------------------------------------------------------
	BOOL Find(LPCTSTR szText, BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bSearchDown,BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Selects a text bounded by given coordinates.
	// Parameters:
	//      nRow1 - Start row of the text.
	//      nCol1 - Start visible column (character) of the text.
	//      nRow2 - End row of the text.
	//      nCol2 - End visible column (character) of the text.
	// Returns:
	//      TRUE if found, FALSE if not.
	//-----------------------------------------------------------------------
	BOOL Select(int nRow1, int nCol1, int nRow2, int nCol2, BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary: Clears all selection
	//-----------------------------------------------------------------------
	void ClearSelection();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this operator to copy control settings from other control.
	// Parameters:
	//      src - A source syntax edit control.
	// Returns:
	//      A reference to this object.
	//-----------------------------------------------------------------------
	const CXTPSyntaxEditCtrl& operator=(const CXTPSyntaxEditCtrl& src);

	//-----------------------------------------------------------------------
	// Summary:
	//      Restores cursor, used mainly while changing font.
	//-----------------------------------------------------------------------
	void RestoreCursor();

	//-----------------------------------------------------------------------
	// Summary:
	//      Turns on/off overwrite mode.
	// Parameters:
	//      bOverwriteMode : [in] TRUE if INS is off, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetOverwriteMode(BOOL bOverwriteMode);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets caret for overwrite mode.
	// Parameters:
	//      bThin : [in] TRUE if thin caret, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SetWideCaret(BOOL bWideCaret, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if thin caret is set for overwrite mode.
	// Returns:
	//      Boolean flag. TRUE if set, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetWideCaret() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Selects the surrounding word of the point specified.
	// Parameters:
	//      point : [in] A point about which a word will be searched.
	//-----------------------------------------------------------------------
	void SelectWord(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the current caret position to the specified row and col.
	// Parameters:
	//          nRow             : [in] The visible row number for the current
	//                                  caret position.
	//          nCol             : [in] The col for the current caret position.
	//          bMakeVisible     : [in] Pass true if the set row col should be
	//                                  forcefully made visible.
	//-----------------------------------------------------------------------
	void SetCurCaretPos(int nRow, int nCol, BOOL bMakeVisible = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a buffer manager which maintains a buffer of a smart edit
	//      control.
	// Returns:
	//      Pointer to the CXTPSyntaxEditBufferManager object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditBufferManager* GetEditBuffer();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to get associated configuration manager.
	// Returns:
	//      Returns pointer to configuration manager.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditConfigurationManager* GetLexConfigurationManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Inserts a string at specified row and col.
	// Parameters:
	//      szText : [in] A pointer to a text string to be inserted.
	//      iRow   : [in] A row identifier where text will be inserted.
	//      iCol   : [in] A column identifier where text will be inserted.
	//      bDeleteSelection : [in] If TRUE selected text will be deleted at insertion.
	//                              Default is TRUE.
	//      bRedraw : [in] If TRUE document will be redrawn.
	//-----------------------------------------------------------------------
	void InsertString(LPCTSTR szText, int iRow, int iCol, BOOL bDeleteSelection = TRUE, BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Inserts a vertical text block at specified row and col.
	// Parameters:
	//      szText : [in] A pointer to a text string to be inserted.
	//      iRow   : [in] A row identifier where text will be inserted.
	//      iCol   : [in] A column identifier where text will be inserted.
	//      bDeleteSelection : [in] If TRUE selected text will be deleted at insertion.
	//                              Default is TRUE.
	//      bRedraw : [in] If TRUE document will be redrawn.
	//-----------------------------------------------------------------------
	void InsertTextBlock(LPCTSTR szText, int iRow, int iCol, BOOL bDeleteSelection = TRUE, BOOL bRedraw = TRUE);

	//--------------------------------------------------------------------
	// Summary:
	//      Returns main configuration file name.
	// Remarks:
	//      Return configuration file name previously stored by SetConfigFile().
	// Returns:
	//      CString object contains main configuration file name.
	// See also:
	//      CXTPSyntaxEditCtrl::SetConfigFile().
	//--------------------------------------------------------------------
	CString GetConfigFile();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a path of the executable file module.
	// Returns:
	//      CString objects with path.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetModulePath();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the path to the main configuration file.
	// Parameters:
	//      szPath : [in] A pointer to a string containing a valid path for
	//                      searching the control's .ini files.
	// Remarks:
	//      Configuration will be reloaded.
	// Returns:
	//      TRUE if path is found, FALSE otherwise
	// See also:
	//      CXTPSyntaxEditCtrl::GetConfigFile().
	//-----------------------------------------------------------------------
	BOOL SetConfigFile(LPCTSTR szPath);

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates and applies the font for the edit control
	// Parameters:
	//      pLogFont    : [in] A pointer to LOGFONT structure.
	//-----------------------------------------------------------------------
	void SetFontIndirect(LPLOGFONT pLogfont, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the current cursor position to the specified row and col.
	// Parameters:
	//      iRow    : [in] A Row to set as the current cursor position document
	//                     coordinates.
	//      iCol    : [in] A Col to set as the current cursor position in
	//                      document coordinates.
	//      bRemainSelected : [in] FALSE determines to unselect the selected text.
	// See also:
	//      int GetCurCol(), int GetCurRow()
	//-----------------------------------------------------------------------
	void SetCurPos(int iRow, int iCol, BOOL bRemainSelected = FALSE, BOOL bForceVisible = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the current column.
	// Returns:
	//      An integer value of current document column identifier.
	//-----------------------------------------------------------------------
	int GetCurCol();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the current document row
	// Returns:
	//      An integer value of current document row identifier.
	//-----------------------------------------------------------------------
	int GetCurRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the number of rows in the current document.
	// Returns:
	//      Number of rows in the document.
	//-----------------------------------------------------------------------
	int GetRowCount();

	//-----------------------------------------------------------------------
	// Summary:
	//      Selects all the text.
	//-----------------------------------------------------------------------
	void SelectAll();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the current page to the given one.
	// Parameters:
	//      iPage : [in] The current page to set.
	//-----------------------------------------------------------------------
	void SetCurrentPage(int iPage);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the page count while printing.
	// Returns:
	//      The actual page count.
	//-----------------------------------------------------------------------
	int CalcPageCount();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the page size for printing.
	// Parameters:
	//      szPage : [in] The size of the page to be printed
	//-----------------------------------------------------------------------
	void SetPageSize(CSize szPage);

	//-----------------------------------------------------------------------
	// Summary:
	//      Actually prints the text.
	// Parameters:
	//      pDC : [in] A pointer to device context to print.
	//-----------------------------------------------------------------------
	void DrawEditControl(CDC *pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns boolean selection existence flag.
	// Returns:
	//      TRUE if selection exist, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsSelectionExist();

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to paste from clipboard to the current
	//      document.
	//-----------------------------------------------------------------------
	void Paste();

	//-----------------------------------------------------------------------
	// Summary:
	//      Copies the selected text to clipboard.
	//-----------------------------------------------------------------------
	void Copy();

	//-----------------------------------------------------------------------
	// Summary:
	//      Copies the selected text to clipboard and deletes it from document.
	//-----------------------------------------------------------------------
	void Cut();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns flag which determines if Redo can be made.
	// Remarks:
	//      Call this member function to determine if redo can be made.
	// Returns:
	//      TRUE if redo can be made, FALSE otherwise.
	// See also:
	//      BOOL Redo(int nActionsCount = 1);
	//-----------------------------------------------------------------------
	BOOL CanRedo();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns flag which determines if Undo can be made.
	// Remarks:
	//      Call this member function to determine if undo can be made.
	// Returns:
	//      TRUE if any undo action pending, FALSE otherwise
	// See also:
	//      BOOL Undo(int nActionsCount = 1);
	//-----------------------------------------------------------------------
	BOOL CanUndo();

	//-----------------------------------------------------------------------
	// Summary:
	//      Re-does the last undone operation.
	// Parameters:
	//      nActionsCount : [in] Specifies the number of actions to be redone
	//                              simultaneously.
	// Returns:
	//      TRUE if success, FALSE otherwise.
	// See also:
	//      BOOL CanRedo();
	//-----------------------------------------------------------------------
	BOOL Redo(int nActionsCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Un-does the current action.
	// Parameters:
	//      nActionsCount : [in] Specifies the number of actions to be undone
	//                          simultaneously.
	// Returns:
	//      TRUE if success, FALSE otherwise.
	// See also:
	//      BOOL CanUndo();
	//-----------------------------------------------------------------------
	BOOL Undo(int nActionsCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the list of text for undo operations.
	// Returns:
	//      A reference to CStringList with strings of text for undo.
	//-----------------------------------------------------------------------
	const CStringList& GetUndoTextList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the list of text for redo operations.
	// Returns:
	//      A reference to CStringList with strings of text for redo.
	//-----------------------------------------------------------------------
	const CStringList& GetRedoTextList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Deletes a character at specified location.
	// Parameters:
	//      iRow : [in] A row identifier in document coordinates.
	//      iCol : [in] A column identifier in document coordinates.
	//      pos  : [in] A position qualifier.
	// Returns:
	//      TRUE if deleted, FALSE otherwise;
	// See also:
	//      XTPSyntaxEditDeletePos enum.
	//-----------------------------------------------------------------------
	BOOL DeleteChar(int iRow, int iCol, XTPSyntaxEditDeletePos pos);

	//-----------------------------------------------------------------------
	// Summary:
	//      Deletes a given range of text.
	// Parameters:
	//      iRowFrom    : [in] Specifies start row in document coordinates.
	//      iColFrom    : [in] Specifies start col in document coordinates.
	//      iRowTo      : [in] Specifies end row in document coordinates.
	//      iColTo      : [in] Specifies end col in document coordinates.
	//      nFlags      : [in] mode flag, possible values : smfRedraw,
	//                          smfForceRedraw, smfTextAsBlock.
	// Returns:
	//      TRUE if deleted, FALSE otherwise.
	// See also:
	//      enum XTPSyntaxEditFlags.
	//-----------------------------------------------------------------------
	BOOL DeleteBuffer(int iRowFrom, int iColFrom, int iRowTo, int iColTo, int nFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//      Deletes the currently selected text.
	// Parameters:
	//      bRedraw : [in] If TRUE document should be redrawn. Default value
	//                      is TRUE.
	// Returns:
	//      TRUE if deleted, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL DeleteSelection(BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the top row of the visible window.
	// Parameters:
	//      iRow : [in] An identifier of the top visible row in document
	//                  coordinates.
	// Returns:
	//      TRUE if set successfully, FALSE otherwise.
	// See also:
	//      int GetTopRow();
	//-----------------------------------------------------------------------
	BOOL SetTopRow(int iRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns top row identifier.
	// Returns:
	//      An integer value of the current top row identifier.
	// See also:
	//      SetTopRow(int iRow);
	//-----------------------------------------------------------------------
	int GetTopRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Recalculates and set the scroll bars depending on the buffer size.
	//-----------------------------------------------------------------------
	void RecalcScrollBars();

	//-----------------------------------------------------------------------
	// Summary: Repositions and resizes control bars in the client area of a
	//          window.
	// Parameters:
	//      nIDFirst     : [in] The ID of the first in a range of control bars
	//                          to reposition and resize.
	//      nIDLast      : [in] The ID of the last in a range of control bars
	//                          to reposition and resize.
	//      nIDLeftOver  : [in] Specifies ID of pane that fills the rest of the
	//                          client area.
	//      nFlag        : [in] Can have one of the following values:
	//                          [ul]
	//                          [li]CWnd::reposDefault - Performs the layout of
	//                              the control bars. lpRectParam is not used and
	//                              can be NULL.[/li]
	//                          [li]CWnd::reposQuery - The layout of the control
	//                              bars is not done; instead lpRectParam is
	//                              initialized with the size of the client area,
	//                              as if the layout had actually been done.[/li]
	//                          [li]CWnd::reposExtra   Adds the values of lpRectParam
	//                              to the client area of nIDLast and also performs
	//                              the layout.[/li]
	//                          [/ul]
	//      lpRectParam  : [in] Points to a RECT structure; the usage of which
	//                          depends on the value of nFlag.
	//      lpRectClient : [in] Points to a RECT structure containing the available
	//                          client area. If NULL, the window's client area will
	//                          be used.
	//      bStretch     : [in] Indicates whether the bar should be stretched to
	//                          the size of the frame.
	// Remarks: The nIDFirst and nIDLast parameters define a range of control-bar
	//          IDs to be repositioned in the client area. The nIDLeftOver parameter
	//          specifies the ID of the child window (normally the view) which
	//          is repositioned and resized to fill the rest of the client area
	//          not filled by control bars.
	//-----------------------------------------------------------------------
	void RepositionBars(UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver, UINT nFlag = CWnd::reposDefault, LPRECT lpRectParam = NULL, LPCRECT lpRectClient = NULL, BOOL bStretch = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Return pointer to the specified CScrollBar object.
	// Parameters:
	//      nBar : [in] Specifies the type of scroll bar. The parameter can take
	//                  one of the following values:
	//                  [ul]
	//                  [li]SB_HORZ - Retrieves the position of the horizontal scroll bar[/li]
	//                  [li]SB_VERT - Retrieves the position of the vertical scroll bar[/li]
	//                  [/ul]
	// Remarks:
	//      Call this member function to obtain a pointer to the specified sibling
	//      scroll bar. This member function does not operate on scroll bars created
	//      when the WS_HSCROLL or WS_VSCROLL bits are set during the creation
	//      of a window. The CWnd implementation of this function simply returns
	//      NULL. Derived classes, such as CView, implement the described
	//      functionality.
	// Returns:
	//      A pointer to sibling scroll-bar control, or NULL if none.
	//-----------------------------------------------------------------------
	virtual CScrollBar* GetScrollBarCtrl( int nBar ) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Enables or disables the scroll bar.
	// Parameters:
	//      nBar    : [in] The scroll-bar identifier.
	//      bEnable : [in] Specifies whether the scroll bar is to be enabled
	//                    or disabled.
	// Remarks:
	//      If the window has a sibling scroll-bar control, that scroll bar
	//      is used; otherwise the window's own scroll bar is used.
	// See also:
	//      void ShowScrollBar( UINT nBar, BOOL bShow = TRUE );
	//-----------------------------------------------------------------------
	void EnableScrollBarCtrl( int nBar, BOOL bEnable = TRUE );

	//-----------------------------------------------------------------------
	// Summary: Shows or hides a scroll bar.
	// Parameters:
	//      nBar : [in] Specifies whether the scroll bar is a control or part
	//                  of a window's non-client area. If it is part of the
	//                  non-client area, nBar also indicates whether the scroll
	//                  bar is positioned horizontally, vertically, or both.
	//                  It must be one of the following:
	//                  [ul]
	//                  [li]SB_BOTH - Specifies the horizontal and vertical
	//                      scroll bars of the window.[/li]
	//                  [li]SB_HORZ - Specifies that the window is a horizontal
	//                      scroll bar.[/li]
	//                  [li]SB_VERT - Specifies that the window is a vertical
	//                      scroll bar.[/li]
	//      nShow: [in] Specifies whether Windows shows or hides the scroll bar.
	//                  If this parameter is TRUE, the scroll bar is shown;
	//                  otherwise the scroll bar is hidden.
	// Remarks:
	//      An application should not call ShowScrollBar to hide a scroll
	//      bar while processing a scroll-bar notification message.
	// See also:
	//          void EnableScrollBarCtrl( int nBar, BOOL bEnable = TRUE );
	//-----------------------------------------------------------------------
	void ShowScrollBar( UINT nBar, BOOL bShow = TRUE );

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets minimum and maximum position values for the given scroll bar.
	// Parameters:
	//      nBar    : [in] Specifies the scroll bar to be set. This parameter
	//                      can be either of the following values:
	//                      [ul]
	//                      [li]SB_HORZ - Sets the range of the horizontal scroll
	//                                      bar of the window.[/li]
	//                      [li]SB_VERT - Sets the range of the vertical scroll
	//                                      bar of the window.[/li]
	//                      [/ul]
	//      nMinPos : [in] Specifies the minimum scrolling position.
	//      nMaxPos : [in] Specifies the maximum scrolling position.
	//      bRedraw : [in] Specifies whether the scroll bar should be redrawn to
	//                      reflect the change. If bRedraw is TRUE, the scroll bar
	//                      is redrawn; if FALSE, the scroll bar is not redrawn.
	// Remarks: It can also be used to hide or show standard scroll bars.
	//
	//          An application should not call this function to hide a scroll
	//          bar while processing a scroll-bar notification message.
	//
	//          If the call to SetScrollRange immediately follows a call to
	//          the SetScrollPos member function, the bRedraw parameter in the
	//          SetScrollPos member function should be 0 to prevent the scroll
	//          bar from being drawn twice.
	//
	//          The default range for a standard scroll bar is 0 through 100.
	//          The default range for a scroll bar control is empty (both the
	//          nMinPos and nMaxPos values are 0). The difference between the
	//          values specified by nMinPos and nMaxPos must not be greater
	//          than INT_MAX.
	//-----------------------------------------------------------------------
	void SetScrollRange( int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE );

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the current position of a scroll box and, if requested, redraws
	//      the scroll bar to reflect the new position of the scroll box
	// Parameters:
	//      nBar    : [in] Specifies the scroll bar to be set. This parameter
	//                      can be either of the following:
	//                      [ul]
	//                      [li]SB_HORZ - Sets the position of the scroll box
	//                                      in the horizontal scroll bar of the
	//                                      window.[/li]
	//                      [li]SB_VERT - Sets the position of the scroll box in
	//                                      the vertical scroll bar of the
	//                                      window.[/li]
	//                      [/ul]
	//      nPos    : [in] Specifies the new position of the scroll box. It must
	//                      be within the scrolling range.
	//      bRedraw : [in] Specifies whether the scroll bar should be repainted
	//                      to reflect the new scroll-box position. If this parameter
	//                      is TRUE, the scroll bar is repainted; if FALSE, the
	//                      scroll bar is not repainted.
	// Remarks:
	//      Setting bRedraw to FALSE is useful whenever the scroll bar will
	//      be redrawn by a subsequent call to another function.
	// Returns:
	//      The previous position of the scroll box.
	//-----------------------------------------------------------------------
	int SetScrollPos( int nBar, int nPos, BOOL bRedraw = TRUE );

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the information that the SCROLLINFO structure maintains about
	//      a scroll bar.
	// Parameters:
	//      nBar         : [in] Specifies the scroll bar to be set. This parameter
	//                          can be either of the following:
	//                          [ul]
	//                          [li]SB_HORZ - Sets the position of the scroll box
	//                             in the horizontal scroll bar of the window.[/li]
	//                          [li]SB_VERT   Sets the position of the scroll box in
	//                             the vertical scroll bar of the window.[/li]
	//                          [/ul]
	//      lpScrollInfo : [in] A pointer to a SCROLLINFO structure.
	//      bRedraw      : [in] Specifies whether the scroll bar should be redrawn
	//                          to reflect the new information. If bRedraw is TRUE,
	//                          the scroll bar is redrawn. If it is FALSE, it is
	//                          not redrawn. The scroll bar is redrawn by default.
	// Remarks:
	//      You must provide the values required by the SCROLLINFO structure
	//      parameters, including the flag values.
	//
	//      The SCROLLINFO structure contains information about a scroll bar,
	//      including the minimum and maximum scrolling positions, the page
	//      size, and the position of the scroll box (the thumb). See the
	//      SCROLLINFO structure topic in the Platform SDK for more information
	//      about changing the structure defaults.
	// Returns:
	//      If successful, the return is TRUE. Otherwise, it is FALSE.
	// See also:
	//     BOOL GetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL )
	//     int GetScrollLimit( int nBar )
	//-----------------------------------------------------------------------
	BOOL SetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE );

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns max scroll position identifier.
	// Parameters:
	//      nBar : [in] Specifies the type of scroll bar. The parameter can take
	//                  one of the following values:
	//                  [ul]
	//                  [li]SB_HORZ - Retrieves the scroll limit of the horizontal
	//                     scroll bar.[/li]
	//                  [li]SB_VERT   Retrieves the scroll limit of the vertical
	//                     scroll bar.[/li]
	//                  [/ul]
	// Remarks:
	//      Call this member function to retrieve the maximum scrolling
	//      position of the scroll bar.
	// Returns:
	//      An  integer value which specifies the maximum position of a scroll
	//      bar if successful; otherwise 0.
	//-----------------------------------------------------------------------
	int GetScrollLimit( int nBar );

	//-----------------------------------------------------------------------
	// Summary: Retrieves the scrollbar information
	// Parameters:
	//      nBar         : [in] Specifies whether the scroll bar is a control
	//                         or part of a window's non-client area. If it is
	//                         part of the non-client area, nBar also indicates
	//                         whether the scroll bar is positioned horizontally,
	//                         vertically, or both. It must be one of the following:
	//                         [ul]
	//                         [li]SB_CTL - Contains the parameters for a scroll
	//                             bar control. The m_hWnd data member must be
	//                             the handle of the scroll bar control.[/li]
	//                         [li]SB_HORZ - Specifies that the window is a
	//                             horizontal scroll bar.[/li]
	//                         [li]SB_VERT   Specifies that the window is a
	//                             vertical scroll bar.
	//      lpScrollInfo : [out] A pointer to a SCROLLINFO structure. See the
	//                         Platform SDK for more information about this
	//                         structure.
	//      nMask        : [in] Specifies the scroll bar parameters to retrieve.
	//                         The default specifies a combination of SIF_PAGE,
	//                         SIF_POS, SIF_TRACKPOS, and SIF_RANGE. See SCROLLINFO
	//                         for more information on the nMask values.
	// Remarks:
	//      Call this member function to retrieve the information that the
	//      SCROLLINFO structure maintains about a scroll bar.
	//      GetScrollInfo enables applications to use 32-bit scroll positions.
	//
	//      The SCROLLINFO structure contains information about a scroll bar,
	//      including the minimum and maximum scrolling positions, the page
	//      size, and the position of the scroll box (the thumb). See the
	//      SCROLLINFO structure topic in the Platform SDK for more information
	//      about changing the structure defaults.
	//
	//      The MFC Windows message handlers that indicate scroll-bar position,
	//      CWnd::OnHScroll and CWnd::OnVScroll, provide only 16 bits of position
	//      data. GetScrollInfo and SetScrollInfo provide 32 bits of scroll-bar
	//      position data. Thus, an application can call GetScrollInfo while
	//      processing either CWnd::OnHScroll or CWnd::OnVScroll to obtain 32-bit
	//      scroll-bar position data.
	// Returns:
	//      If the message retrieved any values, the return is TRUE. Otherwise,
	//      it is FALSE.
	// See also:
	//     BOOL SetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE );
	//-----------------------------------------------------------------------
	BOOL GetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL );

	//-----------------------------------------------------------------------
	// Summary:
	//      Scrolls the specified client area with additional features.
	// Parameters:
	//      dx : [in] Specifies the amount, in device units, of horizontal scrolling.
	//                  This parameter must have a negative value to scroll to the left.
	//      dy : [in] Specifies the amount, in device units, of vertical scrolling.
	//                  This parameter must have a negative value to scroll up.
	//      lpRectScroll : [in] Points to a RECT structure that specifies the portion
	//                          of the client area to be scrolled. If this parameter
	//                          is NULL, the entire client area is scrolled.
	//      lpRectClip : [in] Points to a RECT structure that specifies the clipping
	//                          rectangle to scroll. This structure takes precedence
	//                          over the rectangle pointed to by lpRectScroll. Only
	//                          bits inside this rectangle are scrolled. Bits outside
	//                          this rectangle are not affected even if they are in
	//                          the lpRectScroll rectangle. If this parameter is NULL,
	//                          no clipping is performed on the scroll rectangle.
	//      prgnUpdate : [in] Identifies the region that is modified to hold the
	//                          region invalidated by scrolling. This parameter
	//                          may be NULL.
	//      lpRectUpdate : [out] Points to a RECT structure that will receive the
	//                          boundaries of the rectangle invalidated by scrolling.
	//                          This parameter may be NULL.
	//      flags : [in] Can have one of the following values:
	//                      [ul]
	//                      [li]SW_ERASE   When specified with SW_INVALIDATE,
	//                      erases the newly invalidated region by sending
	//                      a WM_ERASEBKGND message to the window. [/li]
	//                      [li]SW_INVALIDATE Invalidates the region identified
	//                                      by prgnUpdate after scrolling.[/li]
	//                      [li]SW_SCROLLCHILDREN Scrolls all child windows that
	//                      intersect the rectangle pointed to by lpRectScroll
	//                      by the number of pixels specified in dx and dy.
	//                      Windows sends a WM_MOVE message to all child windows
	//                      that intersect lpRectScroll, even if they do not move.
	//                      The caret is repositioned when a child window is
	//                      scrolled and the cursor rectangle intersects the
	//                      scroll rectangle.[/li]
	//                      [/ul]
	//-----------------------------------------------------------------------
	int ScrollWindowEx( int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, CRgn* prgnUpdate, LPRECT lpRectUpdate, UINT flags );

	//-----------------------------------------------------------------------
	// Summary:
	//      Scrolls the specified client area.
	// Parameters:
	//      xAmount : [in] Specifies the amount, in device units, of horizontal
	//                      scrolling. This parameter must be a negative value
	//                      to scroll to the left.
	//      yAmount : [in] Specifies the amount, in device units, of vertical
	//                      scrolling. This parameter must be a negative value
	//                      to scroll up.
	//      lpRect : [in]  Points to a CRect object or RECT structure that
	//                      specifies the portion of the client area to be scrolled.
	//                      If lpRect is NULL, the entire client area is scrolled.
	//                      The caret is repositioned if the cursor rectangle
	//                      intersects the scroll rectangle.
	//      lpClipRect : [in] Points to a CRect object or RECT structure that
	//                          specifies the clipping rectangle to scroll.
	//                          Only bits inside this rectangle are scrolled.
	//                          Bits outside this rectangle are not affected
	//                          even if they are in the lpRect rectangle.
	//                          If lpClipRect is NULL, no clipping is performed
	//                          on the scroll rectangle.
	//-----------------------------------------------------------------------
	void ScrollWindow( int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL );

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves the scroll bar range.
	// Parameters:
	//      nBar : [in] Specifies the scroll bar to examine. The parameter can
	//                  take one of the following values:
	//                  [ul]
	//                  [li]SB_HORZ - Retrieves the position of the horizontal
	//                                  scroll bar.[/li]
	//                  [li]SB_VERT - Retrieves the position of the vertical
	//                                  scroll bar.[/li]
	//                  [/ul]
	//      lpMinPos : [out] Points to the integer variable that is to receive
	//                          the minimum position.
	//      lpMaxPos : [out] Points to the integer variable that is to receive
	//                          the maximum position.
	//-----------------------------------------------------------------------
	void GetScrollRange( int nBar, LPINT lpMinPos, LPINT lpMaxPos ) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves the position of the scroll box.
	// Parameters:
	//      Bar : [in] Specifies the scroll bar to examine. The parameter can
	//                  take one of the following values:
	//                  [ul]
	//                  [li]SB_HORZ - Retrieves the position of the horizontal
	//                                  scroll bar.[/li]
	//                  [li]SB_VERT - Retrieves the position of the vertical
	//                                  scroll bar.[/li]
	//                  [/ul]
	// Returns:
	//      The current position of the scroll box in the scroll bar if it is
	//      successful; otherwise, it is zero.
	//-----------------------------------------------------------------------
	int GetScrollPos(int nBar);

	//-----------------------------------------------------------------------
	// Summary:
	//      Scrolls the window along with the contents for the specified x and
	//      y number of pixels
	// Parameters:
	//      x : [in] Scroll horizontally in characters.
	//      y : [in] Scroll vertically in rows.
	//-----------------------------------------------------------------------
	void Scroll(int x, int y);

	//-----------------------------------------------------------------------
	// Summary:
	//      Scrolls the window to a specific location where the specified row
	//      and col is visible.
	// Parameters:
	//      iRow : [in] Row for lookup in document coordinates.
	//      iCol : [in] Column for lookup in document coordinates.
	//      bRedraw : [in] Pass TRUE if redraw is to be forced and pass FALSE
	//                      to make it determine if redraw is needed.
	//      bRowColNotify : [in] Pass TRUE if row col change is to be made
	//                              FALSE otherwise.
	//-----------------------------------------------------------------------
	void EnsureVisible(int iRow, int iCol, BOOL bRedraw = FALSE, BOOL bRowColNotify = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Inserts a text at specified or current position.
	// Parameters:
	//      szText : [in] A pointer to a text string to be inserted.
	//      iAbsPos : [in] The absolute position to insert. Pass -1 to insert
	//                    at current caret position.
	// Returns:
	//      TRUE if inserted successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL InsertText(LPCTSTR szText, int iAbsPos = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the window
	// Parameters:
	//      pParent     : [in] Parent window (Must not be NULL)
	//      bHorzScroll : [in] TRUE if horizontal scroll bar is needed
	//      bVertScroll : [in] TRUE if vertical scroll bar is needed
	//      bSplit      : [in] TRUE if split bar is implemented
	//      pBuffer     : [in] A pointer to The buffer for the control (Used by splitter)
	//      lpCS        : [in] Specifies the create context of the window.
	// Returns: TRUE if created, FALSE otherwise
	//-----------------------------------------------------------------------
	BOOL Create(CWnd *pParent, BOOL bHorzScroll, BOOL bVertScroll, CXTPSyntaxEditBufferManager *pBuffer = NULL, CCreateContext *lpCS = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets Breakpoint shape at specified row.
	// Parameters:
	//      nRow : [in] A row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void AddRemoveBreakPoint(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets Bookmark shape at specified row.
	// Parameters:
	//      nRow : [in] A row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void AddRemoveBookmark(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Expands/Collapses block at specified row.
	// Parameters:
	//      nRow : [in] A row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void CollapseExpandBlock(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Removes Breakpoint shape at specified row.
	// Parameters:
	//      nRow : [in] A row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void DeleteBreakpoint(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Removes Breakpoint shape at specified row.
	// Parameters:
	//      nRow : [in] A row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void DeleteBookmark(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Moves the current position to the previous bookmark.
	//-----------------------------------------------------------------------
	void PrevBookmark();

	//-----------------------------------------------------------------------
	// Summary:
	//      Moves the current position to the next bookmark.
	//-----------------------------------------------------------------------
	void NextBookmark();

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the current document has breakpoints.
	// Returns:
	//      TRUE if breakpoints were set; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL HasBreakpoints();

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the current document has bookmarks.
	// Returns:
	//      TRUE if bookmarks were set; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL HasBookmarks();

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves a row mark for the specified row.
	// Parameters:
	//      nRow : [in] row identifier in document coordinates.
	//      lmType : [in]  type of line mark.
	//      pParam : [in]  a pointer to a parameter structure.
	// Returns:
	//      TRUE if specified row mark exists at the specified row, FALSE otherwise.
	// See also:
	//      XTP_EDIT_LINEMARKTYPE, XTP_EDIT_LMPARAM
	//-----------------------------------------------------------------------
	BOOL HasRowMark(int nRow, XTP_EDIT_LINEMARKTYPE lmType, XTP_EDIT_LMPARAM* pParam = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Collapses all expanded collapsible nodes.
	// See Also:
	//      void ExpandAll();
	//-----------------------------------------------------------------------
	void CollapseAll();

	//-----------------------------------------------------------------------
	// Summary:
	//      Expands all collapsed nodes.
	// See Also:
	//      void CollapseAll();
	//-----------------------------------------------------------------------
	void ExpandAll();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns an array of available theme names from the configuration
	//      manager.
	// Returns:
	//      A reference to CStringArray object containing available theme names.
	//-----------------------------------------------------------------------
	CStringArray& GetThemes();

	//-----------------------------------------------------------------------
	// Summary:
	//      Applies the theme with the specified name onto the editor.
	// Parameters:
	//      strTheme : [in] theme name.
	//-----------------------------------------------------------------------
	void ApplyTheme(CString strTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current theme name.
	// Returns:
	//      A reference to CString object with active theme name.
	//-----------------------------------------------------------------------
	const CString& GetCurrentTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//      Refreshes colors for the current document.
	// Remarks:
	//      Refreshing colors causes reread configuration files and reparsing
	//      the current document.
	//-----------------------------------------------------------------------
	virtual void RefreshColors();

	//-----------------------------------------------------------------------
	// Summary:
	//      Invalidates required row.
	// Parameters:
	//      nRow : [in] row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void InvalidateRow(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Invalidates given set of rows
	// Parameters:
	//      nRowFrom : [in] first row identifier in document coordinates.
	//      nRowTo : [in]   last row identifier in document coordinates.
	//-----------------------------------------------------------------------
	void InvalidateRows(int nRowFrom = -1, int nRowTo = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns valid row flag.
	// Parameters:
	//      nDispRow : [in] row identifier in document coordinates.
	// Returns:
	//      TRUE if row valid; FALSE otherwise.
	// See also:
	//      void SetRowValid(int nDispRow);
	//-----------------------------------------------------------------------
	BOOL IsRowValid(int nDispRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets valid flag for the row.
	// Parameters:
	//      nDispRow : [in] row identifier in document coordinates.
	// See also:
	//      BOOL IsRowValid(int nDispRow);
	//-----------------------------------------------------------------------
	void SetRowValid(int nDispRow);

	//-----------------------------------------------------------------------
	// Summary:
	//    Call this member function to determine if the edit control has been activated
	//    or deactivated.
	// Returns:
	//    TRUE if the edit control has been activated, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsActive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called the the edit control is being activated
	//     or deactivated.
	// Parameters:
	//     bIsActive - TRUE if the edit control has been activated, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual void SetActive(BOOL bIsActive);

	//-----------------------------------------------------------------------
	// Summary:
	//    Call this member function to determine if the edit control create and use
	//    scroll bars on parent window.
	// Returns:
	//    TRUE if parent window scroll bars are used, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsCreateScrollbarOnParent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//    Call this member function to define where scrollbars are created -
	//    in SyntaxEdit control window or in parent window.
	// Parameters:
	//     nSet - TRUE if parent window scroll bars should be used, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual void SetCreateScrollbarOnParent(BOOL nSet);

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPSyntaxEditCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSyntaxEditCtrl)
	protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDragCopy();
	afx_msg void OnDragMove();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

protected:

	/////////////////////////////////////////////////////////////////
	// Methods
	/////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates rect(s) for Collapsible node icon
	// Parameters:
	//      nRow        : [in] The window row number for which to print
	//      rcNode      : [out] Reference to CRect variable to receive node
	//                          icon rect
	//      prcNodeFull : [out] Pointer to CRect variable to receive full
	//                          node area rect
	//-----------------------------------------------------------------------
	void GetLineNodeRect(int nRow, CRect& rcNode, CRect* prcNodeFull = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates the edit bar length and stores it into m_nEditbarLength
	//      member variable depending upon the gutter length and visibility and
	//      line number length and visibility.
	//-----------------------------------------------------------------------
	void CalculateEditbarLength();

	//-----------------------------------------------------------------------
	// Summary:
	//      Fills and/or calculates number of tabs or blank spaces required.
	// Parameters:
	//      iNewRow      : [in] New row after indentation
	//      bInsertAtEnd : [in] TRUE if CRLF to be inserted at end
	//      iMaxDispCol  : [out] DispCol to start from and to be calculated
	//      iMaxCol      : [out] Absolute col to be calculated
	//      strTextToIns : [in out] Text to be inserted
	//      bOverwrite   : [in] TRUE on overwrite mode, FALSE otherwise
	//-----------------------------------------------------------------------
	void FillTabs(int iNewRow, BOOL bInsertAtEnd, int& iMaxDispCol, int& iMaxCol, CString& strTextToIns, BOOL bOverwrite);

	//-----------------------------------------------------------------------
	// Summary:
	//      Fills and/or calculates number of tabs or blank spaces required.
	// Parameters:
	//      szText              : [in] Text to be inserted
	//      strTextToIns        : [out] Return string with all the indentation
	//      iNewRow             : [out] New row
	//      iNewCol             : [out] New col  (absolute)
	//      iNewDispCol         : [out] New display col
	//      iNewMaxCol          : [out] New max col
	//      iEditRowFrom        : [out] Start row for XTP_EDIT_NM_EDITCHANGED
	//      iEditRowTo          : [out] End row for XTP_EDIT_NM_EDITCHANGED
	//      iChainActionCount   : [out] Chain action count for undo
	//-----------------------------------------------------------------------
	BOOL CreateInsertText(LPTSTR szText, CString& strTextToIns,int& iNewRow,int& iNewCol, int& iNewDispCol, int& iNewMaxCol, int& iEditRowFrom, int& iEditRowTo,int& iChainActionCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Send XTP_EDIT_NM_EDITCHANGED notification with specified parameters.
	// Parameters:
	//      iRowFrom : [in] Start row identifier.
	//      iRowTo   : [in] End row identifier.
	//      nActions : [in] Action mask.
	//-----------------------------------------------------------------------
	void NotifyEditChanged(int iRowFrom, int iRowTo, UINT nActions);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if a document can be modified by sending SM_EDITCHANGING
	//      notification codes to parent.
	// Returns:
	//      TRUE if the document can be edited, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL CanEditDoc();

	//-----------------------------------------------------------------------
	// Summary:
	//      Send any notification to parent.
	// Parameters:
	//      uCode : [in] The notification code to send.
	//-----------------------------------------------------------------------
	LRESULT NotifyParent(UINT uCode);

	//-----------------------------------------------------------------------
	// Summary:
	//      Send any notification to parent that user click in the bookmark area.
	// Parameters:
	//      nRow     : [in] Document row.
	//      nDispRow : [in] Visible row.
	// Returns:
	//      TRUE if action was handled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL NotifyMarginLBtnClick(int nRow, int nDispRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize selection of all the sibling views.
	//-----------------------------------------------------------------------
	void NotifySelInit();

	//-----------------------------------------------------------------------
	// Summary:
	//      Recalculates horizontal scroll bar.
	// Returns:
	//      Effective size of the page.
	//-----------------------------------------------------------------------
	CSize GetEffectivePageSize();

	//-----------------------------------------------------------------------
	// Summary:
	//      Recalculates horizontal scroll bar
	//-----------------------------------------------------------------------
	void RecalcHorzScrollPos();

	//-----------------------------------------------------------------------
	// Summary:
	//      Recalculates vertical scroll bar
	//-----------------------------------------------------------------------
	void RecalcVertScrollPos();

	//-----------------------------------------------------------------------
	// Summary:
	//      Normalizes a selection by making the end points grater than start
	//      points
	//-----------------------------------------------------------------------
	void NormalizeSelection();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initials all the selection variables
	// Parameters:
	//      point : [in] Initializes selection variables
	//-----------------------------------------------------------------------
	void InitiateSelectionVars(CPoint& point);

	//-----------------------------------------------------------------------
	// Summary:
	//      Find the next word position depending on the parameter supplied.
	// Parameters:
	//      nFindWhat : [in] XTP_EDIT_FINDWORD_PREV to find previous and
	//                          XTP_EDIT_FINDWORD_NEXT to find next word.
	//-----------------------------------------------------------------------
	void FindWord(UINT nFindWhat);

	//-----------------------------------------------------------------------
	// Summary:
	//      Try to match text on the supplied parameters.
	// Parameters:
	//      nRow            : [in] Row to match for.
	//      szLineText      : [in] Line text.
	//      szMatchText     : [in] Text to be matched.
	//      nStartPos       : [in] Start position.
	//      bMatchWholeWord : [in] Has to match whole word or not.
	//      bMatchCase      : [in] Case to be matched or not.
	//      bSearchForward  : [in] TRUE if it is to search forward.
	//-----------------------------------------------------------------------
	BOOL MatchText(int nRow, LPCTSTR szLineText, LPCTSTR szMatchText, int nStartPos, BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bSearchForward, BOOL bRedraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the effective parent.
	// Returns:
	//      The parent window in form of CWnd*.
	//-----------------------------------------------------------------------
	CWnd* GetEffectiveParent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculate and returns the display column for another display column
	//      specified in the iCol parameter.
	// Parameters:
	//      szText : [in] The text on which the calculation should be made.
	//      iCol   : [in] The display column for which calculation is needed.
	// Returns:
	//      The display column as integer value.
	//-----------------------------------------------------------------------
	int CalcValidDispCol(LPCTSTR szText, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the absolute column for a display column.
	// Parameters:
	//      szText   : [in] The text for which calculation is needed.
	//      iDispCol : [in] The valid display column.
	// Returns:
	//      The absolute column.
	//-----------------------------------------------------------------------
	int CalcAbsCol(LPCTSTR szText, int iDispCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Expands the character set by putting space in the position of tab.
	// Parameters:
	//      pszChars : [in] The text to be processed.
	//      strBufer : [in] Buffer for text to be created after expansion.
	//      nCurPos  : [in] The current position in the line.
	// Returns:
	//      The length of processed text after expansion.
	//-----------------------------------------------------------------------
	int ExpandChars(CDC* pDC, LPCTSTR pszChars, int nCurPos, CString& strBufer);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws collapsed text sign like [..]
	// Parameters:
	//      pCoDrawBlk  : [in] Pointer to XTP_EDIT_COLLAPSEDBLOCK structure.
	//      iLine       : [in] Line identifier in document coordinates.
	//      iCol        : [in] Column identifier in document coordinates.
	// See also:
	//      struct XTP_EDIT_COLLAPSEDBLOCK
	//-----------------------------------------------------------------------
	void ProcessCollapsedText(CDC* pDC, XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk, int iLine, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      The same functionality as for DrawCollapsedText but performs some
	//      additional checks for input parameters.
	// Parameters:
	//      pCoDrawBlk  : [in] Pointer to XTP_EDIT_COLLAPSEDBLOCK structure.
	//      txtBlk      : [in] Pointer to XTP_EDIT_TEXTBLOCK structure.
	//      iLine       : [in] Line identifier in document coordinates.
	//      iCol        : [in] Column identifier in document coordinates.
	// See also:
	//      struct XTP_EDIT_COLLAPSEDBLOCK, struct XTP_EDIT_TEXTBLOCK
	//-----------------------------------------------------------------------
	BOOL ProcessCollapsedTextEx(CDC* pDC, XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk, const XTP_EDIT_TEXTBLOCK& txtBlk, int iLine, int iCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a collapsed text for the specified collapsed block.
	// Parameters:
	//      pCoDrawBlk      : [in] XTP_EDIT_COLLAPSEDBLOCK structure.
	//      nMaxLinesCount  : [in] Max lines of text. Default is 150.
	// Returns:
	//      CString object with text for collapsed block.
	// See also:
	//      struct XTP_EDIT_COLLAPSEDBLOCK
	//-----------------------------------------------------------------------
	CString GetCollapsedText(XTP_EDIT_COLLAPSEDBLOCK* pCoDrawBlk, int nMaxLinesCount = 150);

	//-----------------------------------------------------------------------
	// Summary:
	//      Finds collapsed block on the specified point if exist.
	// Parameters:
	//      ptMouse : [in] Mouse coordinates where to check for collapsed block.
	// Returns:
	//      Pointer to the XTP_EDIT_COLLAPSEDBLOCK structure with collapsed block
	//      parameters if it found, NULL otherwise.
	// See also:
	//      struct XTP_EDIT_COLLAPSEDBLOCK.
	//-----------------------------------------------------------------------
	XTP_EDIT_COLLAPSEDBLOCK* GetBlockFromPt(const CPoint& ptMouse);

	//-----------------------------------------------------------------------
	// Summary:
	//      Shows tool tip with the text of the collapsed block on the specified
	//      point.
	// Parameters:
	//      ptMouse : [in] Mouse coordinates where to check for tooltip.
	// See also:
	//      CXTPSyntaxEditToolTipCtrl, m_wndToolTip
	//-----------------------------------------------------------------------
	void ShowCollapsedToolTip(const CPoint& ptMouse);

	//-----------------------------------------------------------------------
	// Summary:
	//      Expand collapsed block if double clicked on it.
	// Parameters:
	//      ptMouse : [in] Mouse coordinates where to check for collapsed block.
	// Returns:
	//      A number of row where collapsed block was expanded, 0 if collapsed
	//      block was not expanded.
	// See also:
	//      CXTPSyntaxEditToolTipCtrl
	//-----------------------------------------------------------------------
	int ProcessCollapsedBlockDblClick(const CPoint& ptMouse);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates and returns the display column for the specified
	//      absolute column.
	// Parameters:
	//      szText     : [in] The text for calculation.
	//      iActualCol : [in] The actual (absolute) column position.
	// Returns:
	//      The calculated display column.
	//-----------------------------------------------------------------------
	int CalcDispCol(LPCTSTR szText, int iActualCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Prints a single row.
	// Parameters:
	//      iTopRow   : [in] The topmost row for the visible area
	//      iRow      : [in] The row to be printed
	//      pDC       : [in] The device context for printing
	//      iStartCol : [in] Column to start print from
	// Returns:
	//      TRUE if printed successfully, FALSE otherwise
	//-----------------------------------------------------------------------
	BOOL PrintRow(int iTopRow, int iRow, CDC *pDC, int iStartCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Updates the sibling views while in split mode.
	// Parameters:
	//      nFlags : [in] Specifies the view update flags.
	//-----------------------------------------------------------------------
	void UpdateScrollPos(DWORD dwUpdate = XTP_EDIT_UPDATE_ALL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets or clears the modification flag for the currently opened
	//      document.
	// Parameters:
	//      bModified : [in] TRUE if want to set the document modified.
	//                          Default is TRUE.
	//-----------------------------------------------------------------------
	void SetDocModified(BOOL bModified = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates the maximum width for a text.
	// Parameters:
	//      szText : [in] The text for calculation.
	// Returns:
	//      The maximum width.
	//-----------------------------------------------------------------------
	int CalcMaximumWidth(LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a filename of the default ini configuration file.
	// Returns:
	//      CString objects with filename.
	//-----------------------------------------------------------------------
	CString GetDefaultCfgFilePath();

	//-----------------------------------------------------------------------
	// Summary:
	//      Internal editing event handler.
	// Parameters:
	//      nRow : [in] Row identifier.
	//      nCol : [in] Column identifier.
	//-----------------------------------------------------------------------
	void OnBeforeEditChanged(int nRow, int nCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Internal editing event handler.
	// Parameters:
	//      nRowFrom    : [in] Start row identifier.
	//      nColFrom    : [in] Start column identifier.
	//      nRowTo      : [in] End row identifier.
	//      nColTo      : [in] End column identifier.
	//      eEditAction : [in] Action identifier.
	//-----------------------------------------------------------------------
	void OnEditChanged(int nRowFrom, int nColFrom, int nRowTo, int nColTo, int eEditAction);

	//-----------------------------------------------------------------------
	// Summary:
	//      Internal editing event handler.
	// Parameters:
	//      LCFrom  : [in] Start coordinates.
	//      LCTo    : [in] End coordinates.
	//      eEditAction : [in] Action identifier.
	// See also:
	//      struct XTP_EDIT_LINECOL
	//-----------------------------------------------------------------------
	void OnEditChanged(const XTP_EDIT_LINECOL& LCFrom, const XTP_EDIT_LINECOL& LCTo, int eEditAction);

	//-----------------------------------------------------------------------
	// Summary:
	//      Internal editing event handler.
	// Parameters:
	//      code        : [in] Event code.
	//      wParam      : [in] Event parameter 1.
	//      lParam      : [in] Event parameter 2.
	// See also:
	//      XTP_NOTIFY_CODE
	//-----------------------------------------------------------------------
	void OnParseEvent(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     Send WM_NOTIFY with XTP_EDIT_NM_PARSEEVENT code to parent window.
	// Parameters:
	//      code        : [in] Event code from XTPSyntaxEditOnParseEvent enum.
	//      wParam      : [in] Event parameter 1.
	//      lParam      : [in] Event parameter 2.
	// See Also:
	//      XTPSyntaxEditOnParseEvent
	//-----------------------------------------------------------------------
	BOOL NotifyParseEvent(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//      Internal editing event handler.
	// Parameters:
	//      code        : [in] Event code from XTPSyntaxEditOnCfgChangedEvent enum.
	//      wParam      : [in] Event parameter 1.
	//      lParam      : [in] Event parameter 2.
	// See also:
	//      XTPSyntaxEditOnCfgChangedEvent
	//-----------------------------------------------------------------------
	void OnLexCfgWasChanged(XTP_NOTIFY_CODE code, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current document row number.
	// Returns:
	//      Current document row integer value.
	// See also:
	//      void SetCurrentDocumentRow(int nRow);
	//-----------------------------------------------------------------------
	int GetCurrentDocumentRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set row number as current document row.
	// Parameters:
	//      nRow : [in] row number.
	// See also:
	//      int GetCurrentDocumentRow();
	//-----------------------------------------------------------------------
	void SetCurrentDocumentRow(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current visible row number.
	// Returns:
	//      Integer value of current visible row.
	//-----------------------------------------------------------------------
	int GetCurrentVisibleRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates document row basing in visible row.
	// Returns:
	//      Integer value of document row.
	// See also:
	//  int GetCurrentVisibleRow();
	//-----------------------------------------------------------------------
	int GetDocumentRow(int nVisibleRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates document row basing in global visible row.
	// Returns:
	//      Integer value of document row.
	//-----------------------------------------------------------------------
	int CalculateDocumentRow(int nStartDocumentRow, int nRowDelta);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates visible row basing in document row.
	// Returns:
	//      Integer value of visible row.
	//-----------------------------------------------------------------------
	int GetVisibleRow(int nDocumentRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates global visible row basing in document row.
	// Parameters:
	//      nStartDocumentRow : [in] Start row.
	//      nDocumentRow      : [in] Count of document rows.
	// Returns:
	//      Integer value of global visible row.
	//-----------------------------------------------------------------------
	int CalculateVisibleRow(int nStartDocumentRow, int nDocumentRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns maximum visible rows count.
	// Parameters:
	//      nMaxDocRow : [in] Max document row up to which calculate visible
	//                          rows. If you pass -1 then calculate up to the
	//                          end of the document. -1 is default value.
	// Returns:
	//      Integer value of maximum visible rows count.
	//-----------------------------------------------------------------------
	int GetVisibleRowsCount(int nMaxDocRow = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Moves current row up on a number of rows.
	// Parameters:
	//      nCount : [in] Count of rows.
	// Returns:
	//      Integer value of current visible row.
	//-----------------------------------------------------------------------
	int MoveCurrentVisibleRowUp(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Moves current row down on a number of rows.
	// Parameters:
	//      nCount : [in] Count of rows.
	// Returns:
	//      Integer value of current visible row.
	//-----------------------------------------------------------------------
	int MoveCurrentVisibleRowDown(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Shifts current row up on a number of rows.
	// Parameters:
	//      nCount : [in] Count of rows.
	//      bChangeCaret : [in] Caret movement flag, default is FALSE.
	// Returns:
	//      TRUE if screen is changed; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL ShiftCurrentVisibleRowUp(int nCount, BOOL bChangeCaret = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Shifts current row down on a number of rows.
	// Parameters:
	//      nCount : [in] Count of rows.
	//      bChangeCaret : [in] Caret movement flag, default is FALSE.
	// Returns:
	//      TRUE if screen is changed; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL ShiftCurrentVisibleRowDown(int nCount, BOOL bChangeCaret = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets length of collapsed block.
	// Parameters:
	//      nStartRow : [in] Start row of collapsed block.
	//      rnLen     : [out] Reference to store length value.
	// Returns:
	//      TRUE if success; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetCollapsedBlockLen(int nStartRow, int& rnLen);

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets the type of row node.
	// Parameters:
	//      nRow    : [in] Row identifier.
	//      rowNode : [out] Reference to store row node type.
	// Returns:
	//      TRUE if success; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetRowNodes(int nRow, DWORD& dwType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a text for the specified line.
	// Parameters:
	//      nRow    : [in] Row identifier.
	//      bAddCRLF : [in] Boolean flag how to process end of text lines.
	//                      Default is FALSE.
	//      iCRLFStyle : [in] Style of end of text lines. Default is -1.
	// Returns:
	//      Reference to CString with text.
	// See Also: CXTPSyntaxEditBufferManager::GetLineText
	//-----------------------------------------------------------------------
	const CString& GetLineText(int nRow, BOOL bAddCRLF = FALSE, int iCRLFStyle = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets a text for the specified line.
	// Parameters:
	//      nRow    : [in] Row identifier.
	//      strBuffer : [out] Reference to text buffer to put returned text.
	//      bAddCRLF : [in] Boolean flag how to process end of text lines.
	//                      Default is FALSE.
	//      iCRLFStyle : [in] Style of end of text lines. Default is -1.
	// See Also: CXTPSyntaxEditBufferManager::GetLineText
	//-----------------------------------------------------------------------
	void GetLineText(int nRow, CString& strBuffer, BOOL bAddCRLF = FALSE, int iCRLFStyle = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads data into auto complete list.
	//-----------------------------------------------------------------------
	void SetAutoCompleteList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Refreshes line marks depending on change type.
	// Parameters:
	//      pEditChanged : [in] Pointer to XTP_EDIT_NMHDR_EDITCHANGED structure.
	// See also:
	//      LPXTP_EDIT_NMHDR_EDITCHANGED
	//-----------------------------------------------------------------------
	void RefreshLineMarks(XTP_EDIT_NMHDR_EDITCHANGED* pEditChanged);

	//-----------------------------------------------------------------------
	// Summary:
	//      Redraws line marks.
	//-----------------------------------------------------------------------
	void RedrawLineMarks();

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs Undo/Redo operations and common control properties
	//      adjustment.
	// Parameters:
	//      nActionsCount : [in] A number of actions to undo/redo.
	//      bUndoRedo : [in] TRUE for performing Undo operations, FALSE for
	//                      performing Redo operations.
	// Returns:
	//      TRUE if success, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL DoUndoRedo(int nActionsCount, BOOL bUndoRedo);

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the case of the selection.
	// Parameters:
	//      bUpper : [in] TRUE when changing selection case to Upper,
	//               FALSE when changing selection case to Lower.
	// Returns:
	//     TRUE if performs successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL DoChangeSelectionCase(BOOL bUpper);

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the tabification of the selection.
	// Parameters:
	//      bTabify : [in] TRUE when changing spaces to tabs,
	//               FALSE when changing tabs to spaces.
	// Returns:
	//     TRUE if performs successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL DoChangeSelectionTabify(BOOL bTabify);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to read options from the registry.
	// Returns:
	//      TRUE if performs successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetRegValues();

	/////////////////////////////////////////////////////////////////////////
	// Variables
	/////////////////////////////////////////////////////////////////////////

	BOOL m_bLineNumbers;        // TRUE if line number is to be printed
	BOOL m_bDrawNodes;          // TRUE if Collapsible nodes signs are to be printed
	BOOL m_bScrolling;          // Temporary variable to specify if scrolling by mouse wheel
	BOOL m_bCaseSensitive;      // Case sensitive
	BOOL m_bIsSelectingWord;    // TRUE if user is selecting word by word by pressing ctrl
	BOOL m_bSelectingFromBookmark;  // TRUE if user is selecting from bookmark
	BOOL m_bSelMargin;          // TRUE if selection margin is to be enabled or not
	BOOL m_bVertScrollBar;      // TRUE if horizontal scrollbar is to be enabled
	BOOL m_bHorzScrollBar;      // TRUE if vertical scrollbar is to be enabled
	BOOL m_bEnableWhiteSpace;   // Show white space
	BOOL m_bSyntaxColor;        // Enables or disables syntax colorization
	BOOL m_bEnableOleDrag;      // Enables or disables OLE drag drop
	BOOL m_bAutoIndent;         // Auto indentation is enabled or not
	BOOL m_bIsSmartIndent;      // Smart indentation is enabled or not
	BOOL m_bRightButtonDrag;    // TRUE if user is dragging through right button
	BOOL m_bIsScrollingEndRow;  // Maintains a internal logic to draw the last row which is partly visible
	BOOL m_bTokensLoaded;       // Specifies if tokens are loaded or not
	BOOL m_bDroppable;          // TRUE if the current position is droppable
	BOOL m_bDragging;           // TRUE if in dragging mode
	BOOL m_bIsDragging;         // TRUE if dragging, FALSE otherwise
	BOOL m_bIsOnSelection;      // TRUE if cursor is on selection
	BOOL m_bShiftPressed;       // If shift is pressed
	BOOL m_bPageDirty;          // TRUE if the whole page is dirty
	BOOL m_bSelectionStarted;   // TRUE if selection started
	BOOL m_bSelecting;          // TRUE if currently selecting
	BOOL m_bColumnSelect;       // Column selection mode (vertical block)
	BOOL m_bSelectionExist;     // TRUE if selection exists
	BOOL m_bWideCaret;          // Stored Overwrite Caret Style: Thin or Thick.
	BOOL m_bTabWithSpace;       // Store tabulation width.
	BOOL m_bIsActive;           // TRUE if application is active.
	BOOL m_bCaretCreated;       // TRUE if this window has created the caret
	BOOL m_bFocused;            // States if focused or not
	BOOL m_bCreateScrollbarOnParent; // TRUE if parent window scroll bars are used, otherwise FALSE.


	int m_nTopCalculatedRow;    // Top row of the last range of rows that have scrollbars properly calculated
	int m_nBottomCalculatedRow; // Bottom row of the last range of rows that have scrollbars properly calculated
	int m_nEditbarLength;       // Edit bar length including gutter and line number
	int m_nMarginLength;        // Margin length
	int m_nLineNumLength;       // Line numbers length
	int m_nNodesWidth;          // The width of the nodes signs
	int m_nNumPages;            // Stores the total number of pages
	int m_nTopMargin;           // Top margin in pixels while printing
	int m_nBottomMargin;        // Bottom margin in pixels while printing
	int m_nRightMargin;         // Right margin while printing
	int m_nLeftMargin;          // Left margin while printing
	int m_nBookmarkSelStartRow; // Starting row if selection is started in bookmark area// Starting row if selection is started in bookmark area
	int m_nInsertTabCount;      // Temporarily stores the no of tabs to be inserted for auto indentation
	int m_nInsertSpaceCount;    // Temporarily stores the no of space to be inserted for auto indentation
	int m_nAutoIndentCol;       // Contains the column for auto indentation
	int m_nPrvDirtyRow;         // Previous document row for refreshment
	int m_nCurrentPage;         // The current page
	int m_nDirtyRow;            // The dirty document row, -1 for all
	int m_nTopRow;              // Top row for display
	int m_nCurrentDocumentRow;  // Current row in the document
	int m_nCurrentCol;          // Current absolute column
	int m_nDispCol;             // Current display column
	int m_nMaxCol;              // Maximum displayed column
	int m_nWheelScroll;         // Lines to scroll on mouse wheel
	int m_nAverageLineLen;      // Average length of line
	int m_nCollapsedTextRowsCount;  // The total amount of collapsed blocks in the document,

	DWORD m_dwInsertPos;        // Current insert position
	DWORD m_dwLastRedrawTime;   // Stores last redrawing time

	CSize m_szPage;             // The size of the page

	CPoint m_ptWordSelStarted;  // The exact point where word selection has been started
	CPoint m_ptDropPos;         // Specifies the drop position
	CPoint m_ptEndSel;          // The end point of selection in visible (displayed) coordinates
	CPoint m_ptStartSel;        // The start point of selection in visible (displayed) coordinates
	CPoint m_ptPrevMouse;       // Stored mouse position of the previous tip showing.

	CXTPSyntaxEditBufferManager*    m_pBuffer;    // The buffer manager
	CWnd*                           m_pParentWnd; // Points to parent window.

	TEXTMETRIC m_tm;            // Loaded text metrics for display
	SCROLLINFO m_siHorz;        // Horizontal scroll data

	CString m_strTmpLineTextBuff;       // Temporary text buffer
	CUIntArray  m_arCollapsedTextRows;  // The array of rows with beginning of collapsed blocks.

	CXTPSyntaxEditToolTipCtrl m_wndToolTip;         // Tool tip window for collapsed blocks text.
	CXTPSyntaxEditAutoCompleteWnd m_wndAutoComplete;    // Auto complete popup window.

	CString m_strDefaultCfgFilePath;            // Store configuration file full name.

private:

	CXTPSyntaxEditRowColorMap m_mapRowBkColor;  // The map for row back colors (cache)
	CXTPSyntaxEditRowColorMap m_mapRowColor;        // The map for row colors (cache)

	// registry management
	BOOL SetValueInt(LPCTSTR lpszValue, int nNewValue, int& nRefValue, BOOL bUpdateReg);
	BOOL SetValueBool(LPCTSTR lpszValue, BOOL bNewValue, BOOL& bRefValue, BOOL bUpdateReg);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a pointer to the screen schema cache.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditLexTextBlock* GetOnScreenSch(int nForRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Clears a screen schema cache.
	// Parameters:
	//      nRowFrom : [in] An integer index of a row where to clear cache from.
	//-----------------------------------------------------------------------
	void ClearOnScreenSchCache(int nRowFrom);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates an average line length from start row to end row.
	// Parameters:
	//      nRowStart : [in] An integer index of a row where to calculate from.
	//      nRowEnd : [in] An integer index of a row where to calculate to.
	// Returns:
	//      An average line length.
	//-----------------------------------------------------------------------
	virtual UINT CalcAveDataSize(int nRowStart, int nRowEnd);

	// -----------------------------------------------------------------
	// Summary:
	//     This member function registers the window class if it has not
	//     already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     A boolean value that specifies if the window is successfully
	//     registered.<p/>
	//     TRUE if the window class is successfully registered.<p/>
	//     Otherwise FALSE.
	// -----------------------------------------------------------------
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns an index of a dirty row.
	// Returns:
	//      An integer value of a dirty row.
	//-----------------------------------------------------------------------
	int GetDirtyRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets the dirty row by given row's index.
	// Parameters:
	//      nRow : [in ] An integer index of a  row to be set as the dirty row.
	// Returns:
	//      TRUE if set successfully, FALSE otherwise.
	// Remarks:
	//      Passing  -1 as nRow you set the whole document as dirty. It causes
	//      repainting of the document.
	//-----------------------------------------------------------------------
	BOOL SetDirtyRow(int nRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a pointer to the line marks manager associated with the
	//      current text buffer.
	// Returns:
	//      Pointer to CXTPSyntaxEditLineMarksManager
	//-----------------------------------------------------------------------
	CXTPSyntaxEditLineMarksManager* GetLineMarksManager();

	//===========================================================================
	// CXTPSyntaxEditCtrl::CTextSearchCache
	//===========================================================================
	class CTextSearchCache
	{
	public:
		CTextSearchCache();
		void Update(int nCurrTopRow);

		int nForTopRow;
		CXTPSyntaxEditLexTextBlockPtr ptrTBStart;
	};

	CTextSearchCache m_fcCollapsable;   // Collapsible rows cache
	CTextSearchCache m_fcRowColors;     // Row colors cache

	//===========================================================================
	// CXTPSyntaxEditCtrl::CScreenSearchBlock
	//===========================================================================
	class _XTP_EXT_CLASS CScreenSearchBlock
	{
	public:
		CScreenSearchBlock();
		virtual ~CScreenSearchBlock();
		CScreenSearchBlock(const CScreenSearchBlock& rSrc);

		int nRowStart;
		int nRowEnd;
		DWORD dwLastAccessTime;
		CXTPSyntaxEditLexTextBlockPtr ptrTBFirst;
	};

	typedef CArray<CScreenSearchBlock, CScreenSearchBlock&> CScreenSearchBlockArray;

	//===========================================================================
	// CXTPSyntaxEditCtrl::CScreenSearchCache
	//===========================================================================
	class CScreenSearchCache : public CScreenSearchBlockArray
	{
		typedef CScreenSearchBlockArray Base;
	public:
		DWORD m_dwLastRemoveOldTime;

		CScreenSearchCache();
		virtual ~CScreenSearchCache();

		void RemoveAll();
		void RemoveAt(int nIndex);
		void RemoveOld(int nTimeOut_sec);
	};

	CScreenSearchCache  m_arOnScreenSchCache;   // On-screen schema cache for this control.
	CByteArray          m_arValidDispRows;      // An array with indexes of valid displayed rows.

	//===========================================================================
	// CXTPSyntaxEditCtrl::CAverageVal
	//===========================================================================
	class CAverageVal
	{
	public:
		CAverageVal(int nDataSize = 100);
		void AddValue(UINT uVal);
		UINT GetAverageValue(UINT uDefaultIfNoData = 0);
	protected:
		int m_nDataSize;
		int m_nNextIndex;
		CUIntArray m_arData;
	};

	CAverageVal m_aveRedrawScreenTime;
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTPSyntaxEditCtrl::GetRowCount() {
	return m_pBuffer ? m_pBuffer->GetRowCount() : 0;
}
AFX_INLINE void CXTPSyntaxEditCtrl::InvalidateRow(int nDispRow) {
	InvalidateRows(nDispRow, nDispRow);
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::GetAutoIndent() const {
	return m_bAutoIndent;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::GetSyntaxColor() const {
	return m_bSyntaxColor;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::GetSelMargin() const {
	return m_bSelMargin;
}
AFX_INLINE CXTPSyntaxEditBufferManager* CXTPSyntaxEditCtrl::GetEditBuffer() {
	return m_pBuffer;
}
AFX_INLINE CXTPSyntaxEditConfigurationManager* CXTPSyntaxEditCtrl::GetLexConfigurationManager() {
	return m_pBuffer? m_pBuffer->GetLexConfigurationManager(): NULL;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::GetHorzScrollBar() const {
	return m_bHorzScrollBar;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::GetVertScrollBar() const {
	return m_bVertScrollBar;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::IsActive() const {
	return m_bIsActive;
}
AFX_INLINE BOOL CXTPSyntaxEditCtrl::IsCreateScrollbarOnParent() const {
	return m_bCreateScrollbarOnParent;
}
AFX_INLINE void CXTPSyntaxEditCtrl::SetCreateScrollbarOnParent(BOOL bSet) {
	m_bCreateScrollbarOnParent = bSet;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITSYNTAXEDITCTRL_H__)
