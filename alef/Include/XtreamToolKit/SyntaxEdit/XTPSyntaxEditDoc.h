// XTPSyntaxEditDoc.h : header file
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
#if !defined(__XTPSYNTAXEDITSYNTAXEDITDOC_H__)
#define __XTPSYNTAXEDITSYNTAXEDITDOC_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditBufferManager;

//===========================================================================
// Summary: This class represents a Document portion of the Edit control. It
//          extends functionality provided by CDocument class from MFC's
//          document-view model implementation. CXTPSyntaxEditDoc class works
//          together with CXTPSyntaxEditView and provides storage facilities for
//          it.
// See Also: CXTPSyntaxEditView
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditDoc : public CDocument
{
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      Protected object constructor. Used by dynamic creation
	//-----------------------------------------------------------------------
	CXTPSyntaxEditDoc();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditDoc object, handles cleanup and
	//      de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditDoc();

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPSyntaxEditDoc)
	//}}AFX_CODEJOCK_PRIVATE

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPSyntaxEditDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void OnChangedViewList();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL
	//}}AFX_CODEJOCK_PRIVATE

// Implementation
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      ReOpens already opened file
	// Returns:
	//      TRUE if reopened successfully, FALSE otherwise
	//-----------------------------------------------------------------------
	BOOL ReOpen();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the buffer manager pointer.
	// Parameters:
	//      pBuffer : [in] The buffer manager pointer to set.
	// See also:
	//      class CXTPSyntaxEditBufferManager
	//-----------------------------------------------------------------------
	void SetDataManager(CXTPSyntaxEditBufferManager *pBuffer);

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
	//      Set the INI file path and load the configuration.
	// Parameters:
	//      szPath : [in] A valid path for searching the .ini files.
	// Remarks:
	//      Configuration will be reloaded.
	// Returns:
	//      TRUE if path is found, FALSE otherwise
	// See also:
	//      CXTPSyntaxEditCtrl::GetConfigFile().
	//-----------------------------------------------------------------------
	void SetConfigFile(LPCTSTR szPath);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if auto reload of external files has been enabled.
	// Returns:
	//      TRUE if auto reload is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetAutoReload() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Enables or disables automatic reloading of external files.
	// Parameters:
	//      bAutoReload : [in] TRUE to enable automatic reload of external
	//                             files, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL SetAutoReload(BOOL bAutoReload, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method checks if current file externally modified and prompt
	//      user to reload it. Works only when AutoReload option enabled.
	// Parameters:
	//      lpszPathName - A full file name.
	// Returns:
	//      TRUE if file modified and reloaded, FALSE otherwise.
	// See Also:
	//      GetAutoReload, SetAutoReload
	//-----------------------------------------------------------------------
	BOOL CheckFileModified(LPCTSTR lpszPathName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Is current file read only.
	// Returns:
	//      TRUE if current file read only, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsReadonly();

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this function to get a first view for thye document object.
	// Returns:
	//     A pinter to CXTPSyntaxEditView object.
	// See Also:
	//     CDocument::GetFirstViewPosition, CDocument::GetNextView
	//-----------------------------------------------------------------------
	CXTPSyntaxEditView* GetFirstView();

#ifdef _DEBUG
	//{{AFX_CODEJOCK_PRIVATE
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	//}}AFX_CODEJOCK_PRIVATE
#endif

	// Generated message map functions
protected:
	virtual BOOL SaveFileData(LPCTSTR lpszPathName);

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_MSG(CXTPSyntaxEditDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPSyntaxEditBufferManagerPtr m_ptrDataMan;    // The data manager pointer

	BOOL m_bAutoReload;         // TRUE if auto-reload of externally modified files is to be enabled.
	BOOL m_bOpened;             // TRUE if opened, FALSE otherwise
	BOOL m_bNewFile;            // TRUE if a new file is created or a file is opened
	BOOL m_bDocCreated;         // TRUE if document is created
	BOOL m_bReloading;          // TRUE if we are in progress of reloading document

	int m_iTopRow;              // Identifier of top row.
	int m_iHScrollPos;          // Scrolling position.

	WIN32_FIND_DATA m_curFileData;
};

//---------------------------------------------------------------------------
// Summary:
//      View update action. Defines in which way the view should be updated.
//---------------------------------------------------------------------------
enum XTPSyntaxEditUpdateViewHint
{
	xtpEditHintUnknown,         // Updating action is unknown.
	xtpEditHintRefreshView,     // The view should be refreshed.
	xtpEditHintInitView,        // The view should be initialized.
	xtpEditHintSetFont,         // The view should update the display font.
	xtpEditHintSetLineNum,      // The view should update line number display.
	xtpEditHintSetSelMargin,    // The view should update the selection margin display.
	xtpEditHintSetAutoIndent,   // The view should update the auto-indent setting.
	xtpEditHintSetSyntaxColor,  // The view should update the syntax color setting.
	xtpEditHintUpdateScrollPos, // The view should update scrollbars positions.
	xtpEditHintLast,            // The last update code.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITSYNTAXEDITDOC_H__)
