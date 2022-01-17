// XTPSyntaxEditView.h : header file
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
#if !defined(__XTPSYNTAXEDITSYNTAXEDITVIEW_H__)
#define __XTPSYNTAXEDITSYNTAXEDITVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Summary: This class represents a View portion of the Edit Control. It
//          extends functionality provided by CView class from MFC's document-view
//          model implementation. CXTPSyntaxEditView class works together with
//          CXTPSyntaxEditDoc and provides facilities for it to be displayed on
//          a screen or printed on a printer.
// See Also: CXTPSyntaxEditDoc
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditView : public CView
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPSyntaxEditDoc;
	DECLARE_DYNCREATE(CXTPSyntaxEditView)
	//}}AFX_CODEJOCK_PRIVATE

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      Protected object constructor. Used by dynamic creation.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditView();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditView() object, handles cleanup and
	//          de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditView();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the font of the edit control pointer.
	// Parameters:
	//      pLogfont   - [in] The LOGFONT pointer to be set.
	//      bUpdateReg - [in] Set TRUE to save font in registry.
	//-----------------------------------------------------------------------
	void SetFontIndirect(LOGFONT *pLogFont, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable/Disable the line numbering.
	// Parameters:
	//      bEnable : [in] The LOGFONT pointer to be set.
	//-----------------------------------------------------------------------
	void SetLineNumbers(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the if horizontal scrollbar is enabled.
	// Returns:
	//      TRUE if horizontal scrollbar is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetHorzScrollBar();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the if vertical scrollbar is enabled.
	// Returns:
	//      TRUE if vertical scrollbar is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetVertScrollBar();

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable/disable scroll bar.
	// Parameters:
	//      bHorz         - [in] TRUE if horizontal scroll bar need to be enabled/disabled.
	//      bVert         - [in] TRUE if vertical scroll bar need to be enabled/disabled.
	//      bUpdateReg    - [in] Set TRUE to save these options in registry.
	//      bRecalcLayout - [in] Set TRUE to recalculate layout.
	// Returns:
	//      TRUE if scrollbars were updated, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SetScrollBars(BOOL bHorz, BOOL bVert, BOOL bUpdateReg=FALSE, BOOL bRecalcLayout=TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable/disables gutter.
	// Parameters:
	//      bEnable : [in] Pass TRUE/FALSE if gutter is to be enabled/disbaled.
	//-----------------------------------------------------------------------
	void SetSelMargin(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable/disables auto indent.
	// Parameters:
	//      bEnable : [in] Pass TRUE/FALSE if auto indent is to be enabled/disbaled.
	//-----------------------------------------------------------------------
	void SetAutoIndent(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable/disables syntax colorization.
	// Parameters:
	//      bEnable : [in] Pass TRUE/FALSE if syntax color is to be enabled/disbaled.
	//-----------------------------------------------------------------------
	void SetSyntaxColor(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//      Updates all active sibling views.
	//-----------------------------------------------------------------------
	void UpdateAllViews();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the view ready for redraw.
	//-----------------------------------------------------------------------
	void SetDirty();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the smart edit control pointer for this editor.
	// Returns:
	//      Edit control pointer.
	//-----------------------------------------------------------------------
	virtual CXTPSyntaxEditCtrl& GetEditCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set external smart edit control for the view.
	// Parameters:
	//      pControl - A pointer to smart edit control object.
	// Remarks:
	//      Default edit control window is destroyed.
	//      InternalRelease will be called for external control object in the
	//      view object destructor.
	// See Also:
	//      GetEditCtrl
	//-----------------------------------------------------------------------
	virtual void SetEditCtrl(CXTPSyntaxEditCtrl* pControl);

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
	//      Use this function to get associated configuration manager.
	// Returns:
	//      Pointer to the associated configuration manager.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditConfigurationManager* GetLexConfigurationManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Refreshes the whole document, recalculates the scrollbar.
	//-----------------------------------------------------------------------
	void Refresh();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the current top row.
	// Parameters:
	//      iRow : [in] Row to set as top row.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SetTopRow(int iRow);

	//-----------------------------------------------------------------------
	// Summary:
	//      Return the topmost row.
	// Returns:
	//      The top row integer identifier.
	//-----------------------------------------------------------------------
	int GetTopRow();

	//-----------------------------------------------------------------------
	// Summary:
	//      Updates the sibling views while in split mode.
	// Parameters:
	//      nFlags : [in] Specifies the view update flags.
	//-----------------------------------------------------------------------
	void UpdateScrollPos(DWORD dwUpdate = XTP_EDIT_UPDATE_ALL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Replaces all occurrences of szFindText to szReplaceText.
	// Parameters:
	//      szFindText : [in] Pointer to text to find.
	//      szReplaceText: [in] Pointer to text which will replace.
	//      bMatchWholeWord: [in] Boolean flag to match whole word.
	//      bMatchCase: [in] Boolean flag to match case.
	// Returns:
	//      The number of founded matches.
	//-----------------------------------------------------------------------
	int ReplaceAll(LPCTSTR szFindText, LPCTSTR szReplaceText, BOOL bMatchWholeWord, BOOL bMatchCase);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns Document.
	// Returns:
	//      Pointer to CXTPSyntaxEditDoc.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditDoc* GetDocument();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the edit buffer manager pointer.
	// Returns:
	//      Buffer manager pointer.
	// See also:
	//      class CXTPSyntaxEditBufferManager
	//-----------------------------------------------------------------------
	CXTPSyntaxEditBufferManager * GetDataManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns pointer to the associated lexical parser.
	// Returns:
	//      Pointer to a XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser object.
	//-----------------------------------------------------------------------
	XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser* GetLexParser();

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines is initial update was made.
	// Returns:
	//      TRUE if initial update was made; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsInitialUpdateWasCalled();

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPSyntaxEditView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDropFiles(HDROP hDropInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint=0L, CObject* pHint=NULL);
	//}}AFX_VIRTUAL
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_EDITCHANGING
	//      message which sent before editing applied.
	// Parameters:
	//      pNMHDR  - [in] Pointer to NMHDR structure.
	//      pResult - [out] Pointer to a LRESULT value. Set as TRUE to cancel editing
	//                or FALSE to allow editing.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: OnEditChanged, XTP_EDIT_NM_EDITCHANGING, XTP_EDIT_NM_EDITCHANGED
	//-----------------------------------------------------------------------
	virtual BOOL OnEditChanging(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_EDITCHANGED
	//       message which sent when editing applied.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NMHDR_EDITCHANGED structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_EDITCHANGING, XTP_EDIT_NM_EDITCHANGED,
	//           XTP_EDIT_NMHDR_EDITCHANGED
	//-----------------------------------------------------------------------
	virtual BOOL OnEditChanged(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_SETDOCMODIFIED
	//      message which sent when document (edit control) modified state changed.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NMHDR_DOCMODIFIED structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_SETDOCMODIFIED, XTP_EDIT_NMHDR_DOCMODIFIED
	//-----------------------------------------------------------------------
	virtual BOOL OnSetDocModified(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_DRAWBOOKMARK
	//      message which sent before draw bookmark.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_SENMBOOKMARK structure.
	//      pResult - [out] Pointer to a LRESULT value. Set as TRUE to cancel
	//                default processing (bookmark drawing),
	//                or FALSE to allow default processing.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_DRAWBOOKMARK, XTP_EDIT_SENMBOOKMARK
	//-----------------------------------------------------------------------
	virtual BOOL OnDrawBookmark(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_ROWCOLCHANGED
	//      message which sent current row or col changed.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NMHDR_ROWCOLCHANGED  structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_ROWCOLCHANGED, XTP_EDIT_NMHDR_ROWCOLCHANGED
	//-----------------------------------------------------------------------
	virtual BOOL OnRowColChanged(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_UPDATESCROLLPOS
	//      message which sent scroll bar position changed.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NM_UPDATESCROLLPOS structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_UPDATESCROLLPOS, XTP_EDIT_NM_UPDATESCROLLPOS
	//-----------------------------------------------------------------------
	virtual BOOL OnUpdateScrollPos(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_INSERTKEY
	//      message which sent when 'Ins' (VK_INSERT) key down (insert mode changed).
	// Parameters:
	//      pNMHDR  - [in] Pointer to NMHDR structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_INSERTKEY
	//-----------------------------------------------------------------------
	virtual BOOL OnInsertKey(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_SELINIT
	//      message which sent when selection reseted.
	// Parameters:
	//      pNMHDR  - [in] Pointer to NMHDR structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_SELINIT
	//-----------------------------------------------------------------------
	virtual BOOL OnSelInit(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_STARTOLEDRAG
	//      message which sent when OLE dragging started.
	// Parameters:
	//      pNMHDR  - [in] Pointer to NMHDR structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_STARTOLEDRAG
	//-----------------------------------------------------------------------
	virtual BOOL OnStartOleDrag(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_MARGINCLICKED
	//      message which sent when left mouse button clicked on bookmarks area.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NMHDR_MARGINCLICKED structure.
	//      pResult - [out] Pointer to a LRESULT value. Set as TRUE to cancel
	//                default processing (breakpoint set/reset),
	//                or FALSE to allow default processing.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_MARGINCLICKED, XTP_EDIT_NMHDR_MARGINCLICKED
	//-----------------------------------------------------------------------
	virtual BOOL OnMarginClicked(NMHDR* pNMHDR, LRESULT* pResult);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is a default handler for XTP_EDIT_NM_PARSEEVENT
	//      message which reflect parser event.
	// Parameters:
	//      pNMHDR  - [in] Pointer to XTP_EDIT_NMHDR_PARSEEVENT structure.
	//      pResult - unused for this handler.
	// Returns:
	//      TRUE is message handled, FALSE otherwise.
	// See Also: XTP_EDIT_NM_PARSEEVENT, XTP_EDIT_NMHDR_PARSEEVENT
	//-----------------------------------------------------------------------
	virtual BOOL OnParseEvent(NMHDR* pNMHDR, LRESULT* pResult);

protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//-----------------------------------------------------------------------
	// Summary: Overwrite this method to customize the read-only file handling
	// Returns: TRUE if read-only file can be changed, FALSE otherwise
	//-----------------------------------------------------------------------
	virtual BOOL CanChangeReadonlyFile();

		//-----------------------------------------------------------------------
	// Summary:
	//      Use this method to read options from the registry.
	// Returns:
	//      TRUE if performs successfully, FALSE otherwise.
	// See Also:
	//      CXTPSyntaxEditCtrl::GetRegValues
	//-----------------------------------------------------------------------
	BOOL GetRegValues();

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_MSG(CXTPSyntaxEditView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
	afx_msg void OnEditRepeat();
	afx_msg void OnUpdateEditRepeat(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

private:
	//-----------------------------------------------------------------------
	// Summary:
	//      Performs specific update for all sibling views.
	// Parameters:
	//      pNMHDR_EC :     [in] Pointer to the parameters header.
	// Remarks:
	//      This method process events from parser, updates the editing results
	//      or selection for all sibling views. It is called when an user edits
	//      text or switches between views or parser is running.
	//-----------------------------------------------------------------------
	void UpdateSiblings(XTP_EDIT_NMHDR_EDITCHANGED* pNMHDR_EC = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Called when a sibling view is needed to be updated.
	// Parameters:
	//      pSender : [in] The source to be updated from
	//      dwUpdate      : [in] Type of update. Allowed values are:
	//                      XTP_EDIT_UPDATE_HORZ, XTP_EDIT_UPDATE_VERT, XTP_EDIT_UPDATE_DIAG.
	// See also
	//  class CXTPSyntaxEditView
	//-----------------------------------------------------------------------
	void UpdateSiblingScrollPos(CXTPSyntaxEditView* pSender, DWORD dwUpdate);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the view pointer of a certain pane of the splitter
	// Parameters:
	//      nRow : [in] Row of the splitter frame.
	//      nCol : [in] Column of the splitter frame.
	// Returns:
	//      Pointer to CXTPSyntaxEditView.
	// See also:
	//      class CXTPSyntaxEditView
	//-----------------------------------------------------------------------
	CXTPSyntaxEditView* GetSplitterView(int nRow, int nCol);

	//-----------------------------------------------------------------------
	// Summary:
	//      Starts OLE drag
	//-----------------------------------------------------------------------
	void StartOleDrag();

	LOGFONT m_lfPrevFont; // Temporarily stores editor font
	int m_nPrevTopRow;  // Temporarily stores previous top row
	BOOL m_bScrollBars; // Whether to create control with own scrollbars, or
	                    // let to manage scrolling for parent window

protected:
	int m_nParserThreadPriority_WhenActive;     // Parser priority (active state).
	int m_nParserThreadPriority_WhenInactive;   // Parser priority (inactive state).

	BOOL m_bHorzScrollBar;
	BOOL m_bVertScrollBar;
	BOOL m_bSetPageSize;            // TRUE if printing is started, FALSE otherwise
	BOOL m_bDraggingOver;           // TRUE if dragover is going on, FALSE otherwise
	BOOL m_bDraggingStartedHere;    // TRUE if dragging is started in this view, FALSE otherwise

	//-----------------------------------------------------------------------

	static CXTPSyntaxEditView *ms_pTargetView;  // Target view pointer to be filled in by drag-drop routine
	static BOOL ms_bDroppedHere;        // TRUE if text is dropped in this view, FALSE otherwise
	static POINT ms_ptDropPos;          // Stores drop mouse position.
	static DWORD_PTR ms_dwSignature;    // A signature used during drag-drop operation

	BOOL m_bOleDragging;                // TRUE if OLE dragging is enabled, FALSE otherwise
	BOOL m_bFilesDragging;              // TRUE when dragging files, FALSE otherwise.
	COleDropTarget m_dropTarget;        // OLE drop target

	CSize m_szPage;                     // Page size for printing or preview
	int m_iTopRow;                      // Top row for display

	CWnd* m_pParentWnd;
	CXTPSyntaxEditCtrl  m_wndEditCtrl;  // Default Edit control instance
	CXTPSyntaxEditCtrl* m_pEditCtrl;    // Edit control instance

	CXTPSyntaxEditFindReplaceDlg m_dlgFindReplace;

	BOOL m_bInitialUpdateWasCalled;

};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPSyntaxEditBufferManager* CXTPSyntaxEditView::GetEditBuffer() {
	return GetEditCtrl().GetEditBuffer();
}
AFX_INLINE CXTPSyntaxEditConfigurationManager* CXTPSyntaxEditView::GetLexConfigurationManager() {
	return GetEditCtrl().GetLexConfigurationManager();
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITSYNTAXEDITVIEW_H__)
