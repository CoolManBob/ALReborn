// XTPSyntaxEditBufferManager.h
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
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITBUFFERMANAGER_H__)
#define __XTPSYNTAXEDITBUFFERMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma warning(push)
//#pragma warning(disable: 4121)

//===========================================================================
// Summary:
//      This class is the main class for buffer management. One instance
//      of this class is attached with a CXTPSyntaxEditCtrl object. This class
//      contains a list of CBufferIndex class which in turn contains
//      a position in the file (In case of un-modified index) or a CEditBuffer
//      pointer.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditBufferManager : public CXTPCmdTarget
{
	DECLARE_DYNCREATE(CXTPSyntaxEditBufferManager)
	friend class CXTPSyntaxEditCtrl;
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditBufferManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditBufferManager object, handles cleanup
	//      and de-allocation.
	//-----------------------------------------------------------------------
	~CXTPSyntaxEditBufferManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the file has been modified or not.
	// Returns:
	//      TRUE if modified, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsModified();

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the file has been modified or not.
	// Returns:
	//      CodePage to be used for conversion.
	//-----------------------------------------------------------------------
	UINT GetCodePage();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the tab size for the current document.
	// Returns:
	//      Selected tab size.
	//-----------------------------------------------------------------------
	int GetTabSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the tab size between 1-64.
	// Parameters:
	//      iTabSize : [in] The tab size to set.
	//-----------------------------------------------------------------------
	BOOL SetTabSize(int nTabSize, BOOL bUpdateReg=FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the overwrite flag status.
	// Returns:
	//      TRUE if OVR mode is set, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetOverwriteFlag();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets or resets the OVR flag.
	// Parameters:
	//      bOverwrite : [in] Pass TRUE to set OVR flag, FALSE to INS flag.
	//-----------------------------------------------------------------------
	void SetOverwriteFlag(BOOL bOverwrite);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the parser status.
	// Returns:
	//      TRUE if Parser is enabled, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsParserEnabled();

	//-----------------------------------------------------------------------
	// Summary:
	//      Enable or disable parser.
	// Parameters:
	//      bEnable : [in] Pass TRUE to enable parser, FALSE to disable.
	//-----------------------------------------------------------------------
	void EnableParser(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function converts the supplied text to native CRLF format.
	//      It is mainly used in Paste routine where an user can paste DOS
	//      formatted text into UNIX formatted document
	// Parameters:
	//      strText   : [in] The string to convert.
	//      iTypeFrom : [in] The type of the source CRLF.
	// Returns:
	//      TRUE if converted successfully, FALSE otherwise
	//-----------------------------------------------------------------------
	//BOOL ConvertToNativeCRLFText(CString& strText, int iTypeFrom);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the CRLF type of the current document.
	// Returns:
	//      Current CRLF type.
	//-----------------------------------------------------------------------
	int GetCurCRLFType();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the current documents CRLF style.
	// Parameters:
	//      nStyle : [in] The CRLF style to set
	//-----------------------------------------------------------------------
	void SetCRLFStyle(int nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the CRLF of the current document.
	// Returns:
	//      Current CRLF text.
	//-----------------------------------------------------------------------
	CString GetCurCRLF() const;
private:
	//-----------------------------------------------------------------------
	// Summary:
	//      Internal implementation of getting CRLF of the current document.
	// Parameters:
	//      nCRLFStyle : [in] Index of the CRLF style string.
	// Returns:
	//      Current CRLF text.
	//-----------------------------------------------------------------------
	LPCTSTR GetCRLF(int nCRLFStyle = -1) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the supplied text is a CRLF or not
	// Parameters:
	//      szCompText   : [in] The text to be compared.
	//      bFindReverse : [in] Should be match reverse.
	// Returns:
	//      TRUE if the supplied text is a CRLF, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsTextCRLF(LPCTSTR szCompText, BOOL bFindReverse = FALSE);
#ifdef _UNICODE
	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if the supplied text is a CRLF or not of the non-unicode string.
	// Parameters:
	//      szCompText   : [in] The text to be compared.
	//      bFindReverse : [in] Should be match reverse.
	// Returns:
	//      TRUE if the supplied text is a CRLF, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsTextCRLF(LPCSTR szCompText, BOOL bFindReverse = FALSE);
#endif

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the number of rows in the document.
	// Returns:
	//      Number of rows in the document.
	//-----------------------------------------------------------------------
	int GetRowCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns pointer to the associated Undo/Redo manager.
	// Returns:
	//      Pointer to a CXTPSyntaxEditUndoRedoManager object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditUndoRedoManager* GetUndoRedoManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns pointer to the associated line marks manager.
	// Returns:
	//      Pointer to a CXTPSyntaxEditLineMarksManager object.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditLineMarksManager* GetLineMarksManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns pointer to the associated lexical parser.
	// Returns:
	//      Pointer to a XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser object.
	//-----------------------------------------------------------------------
	XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser* GetLexParser();

	//-----------------------------------------------------------------------
	// Summary: Returns pointer to the associated configuration manager.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditConfigurationManager* GetLexConfigurationManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set pointer to the configuration manager.
	// Parameters:
	//      pMan : [in] Pointer to CXTPSyntaxEditConfigurationManager.
	//-----------------------------------------------------------------------
	void SetLexConfigurationManager(CXTPSyntaxEditConfigurationManager* pMan);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns events connection pointer.
	//-----------------------------------------------------------------------
	CXTPNotifyConnection* GetConnection();


	//-----------------------------------------------------------------------
	// Summary:
	//      Cleans up all internal objects.
	//-----------------------------------------------------------------------
	void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//      Serializes actual buffer.
	// Parameters:
	//      ar : [in] The archive to save to / load from.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the file pointer for the buffer.
	// Parameters:
	//      pFile       : [in] The file pointer to set.
	//      pcszFileExt : [in] Number of lines (Not used).
	// See Also:
	//      SetFileExt()
	//-----------------------------------------------------------------------
	void Load(CFile *pFile, LPCTSTR pcszFileExt = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets file extension.
	// Parameters:
	//      strExt  : [in] string of extension.
	// See also:
	//      CString GetFileExt()
	//-----------------------------------------------------------------------
	void SetFileExt(const CString& strExt);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns file extension.
	// Returns:
	//      CString object with file extensions.
	// See also:
	//      void SetFileExt(const CString& strExt);
	//-----------------------------------------------------------------------
	CString GetFileExt();

	//-----------------------------------------------------------------------
	// Summary:
	//      Gets a line of text either from file or buffer.
	// Parameters:
	//      iLine       : [in] The line for which to get the text.
	//      szText      : [out] The text to be filled in.
	//      bAddCRLF    : [in] Pass TRUE to add CRLF (if exists), otherwise .
	//      iCRLFStyle  : [in] Specify the CRLF style.
	//-----------------------------------------------------------------------
	void GetLineText(int iLine, CString& strText, BOOL bAddCRLF = FALSE, int iCRLFStyle = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a line of text either from file or buffer.
	// Parameters:
	//      iLine       : [in] The line for which to get the text.
	//      bAddCRLF    : [in] Pass TRUE to add CRLF (if exists), otherwise .
	//      iCRLFStyle  : [in] Specify the CRLF style.
	// Returns:
	//      CString object with text line.
	//-----------------------------------------------------------------------
	CString GetLineText(int iLine, BOOL bAddCRLF = FALSE, int iCRLFStyle = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a length for line of text either from file or buffer.
	// Parameters:
	//      iLine       : [in] The line for which to get the text.
	//      bAddCRLF    : [in] Pass TRUE to add CRLF (if exsts), otherwise .
	//      iCRLFStyle  : [in] Specify the CRLF style.
	// Returns:
	//      Text line length as integer value.
	//-----------------------------------------------------------------------
	int GetLineTextLength(int iLine, BOOL bAddCRLF = FALSE, int iCRLFStyle = -1);


	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the max allowable length for line of text.
	// Returns:
	//      Text line length as integer value.
	//-----------------------------------------------------------------------
	int GetMaxLineTextLength() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns a length for line of text either from file or buffer.
	// Parameters:
	//      nLineFrom   : [in] Start line identifier.
	//      nLineTo     : [in] End line identifier.
	// Returns:
	//      Text length as integer value.
	//-----------------------------------------------------------------------
	int CalcMaxLineTextLength(int nLineFrom = -1, int nLineTo = -1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieve text from the buffer bounded between
	//      row1/col1 and row2/col2 text coordinates.
	//-----------------------------------------------------------------------
	BOOL GetBuffer(int row1, int col1, int row2, int col2,
					CMemFile& file, BOOL bColumnSelection = FALSE,
					BOOL bForceDOSStyleCRLF = FALSE);


	//-----------------------------------------------------------------------
	// Summary:
	//      Inserts a text.
	// Parameters:
	//      szText           : [in] Text to be inserted.
	//      iRow             : [in] Start row for delete.
	//      iCol             : [in] Start col for delete.
	//      bCanUndo         : [in] Pass TRUE if this can be undone.
	//      pbRowFlagChanged : [out] Filled with TRUE if row flag has been
	//                                  changed.
	// Returns:
	//      A CEditBuffer pointer of the inserted row.
	//-----------------------------------------------------------------------
	BOOL InsertText(LPCTSTR szText, int iRow, int iCol, BOOL bCanUndo = TRUE,
					XTP_EDIT_LINECOL* pFinalLC = NULL);

	BOOL InsertTextBlock(LPCTSTR szText, int nRow, int nCol, BOOL bCanUndo = TRUE,
					XTP_EDIT_LINECOL* pFinalLC = NULL); // <COMBINE InsertText>

	//-----------------------------------------------------------------------
	// Summary:
	//      Deletes a certain range of text
	// Parameters:
	//      iRowFrom         : [in] Start row for delete.
	//      iColFrom         : [in] Start col for delete.
	//      iRowTo           : [in] End row for delete.
	//      iColTo           : [in] End col for delete.
	//      bCanUndo         : [in] Pass TRUE if this can be undone.
	//      bDispCol         : [in] if TRUE iColFrom and iColTo are display cols,
	//                                  otherwise they are string cols.
	//      pbRowFlagChanged : [out] Filled with TRUE if row flag has been changed.
	// Returns:
	//      A CEditBuffer pointer of the modified row.
	//-----------------------------------------------------------------------
	BOOL DeleteText(int iRowFrom, int iColFrom, int iRowTo, int iColTo,
					BOOL bCanUndo = TRUE, BOOL bDispCol = FALSE);

	//--------------------------------------------------------------------
	// Summary:
	//      Get main configuration file name.
	// Remarks:
	//      Return configuration file name previously stored by
	//      SetConfigFile().
	// Returns:
	//      Main configuration file name.
	// See also:
	//      CXTPSyntaxEditBufferManager::SetConfigFile().
	//--------------------------------------------------------------------
	CString GetConfigFile();

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the path to the main configuration file.
	// Parameters:
	//      szPath : [in] A valid path for searching the .ini files.
	// Remarks:
	//      Configuration will be reloaded.
	// Returns:
	//      TRUE if path is found, FALSE otherwise.
	// See also:
	//      CXTPSyntaxEditBufferManager::GetConfigFile().
	//-----------------------------------------------------------------------
	BOOL SetConfigFile(LPCTSTR szPath);

	//-----------------------------------------------------------------------
	// Summary:
	//      Set the path to the main configuration file.
	// Parameters:
	//      strExt : [in] file extension for for schema definition file.
	// Returns:
	//      CXTPSyntaxEditTextSchemaPtr
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextSchemaPtr GetMasterTextSchema(const CString& strExt);

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the case of the specified characters range in the specified row.
	// Parameters:
	//      nRow      : [in] A row to change case at.
	//      nDispFrom : [in] Begin visible position where to change case at.
	//      nDispTo   : [in] End visible position where to change case at.
	//      bUpper    : [in] TRUE when changing selection case to Upper,
	//                       FALSE when changing selection case to Lower.
	//      bCanUndo  : [in] Pass TRUE if this can be undone.
	// Remarks:
	//      Configuration will be reloaded.
	//-----------------------------------------------------------------------
	void ChangeCase(int nRow, int nDispFrom, int nDispTo, BOOL bUpper, BOOL bCanUndo = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the case of the specified characters range in the specified row.
	// Parameters:
	//      nRow      : [in] A row to change case at.
	//      nDispFrom : [in] Begin visible position where to change case at.
	//      nDispTo   : [in] End visible position where to change case at.
	//      bTabify   : [in] TRUE when changing spaces to tabs,
	//                       FALSE when changing tabs to spaces.
	//      bCanUndo  : [in] Pass TRUE if this can be undone.
	// Remarks:
	//      Configuration will be reloaded.
	//-----------------------------------------------------------------------
	void ChangeTabification(int nRow, int nDispFrom, int nDispTo, BOOL bTabify, BOOL bCanUndo = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts column coordinates of text to string position coordinate.
	// Parameters:
	//      nLine   : [in] Text line identifier.
	//      nDispCol: [in] Column coordinate.
	// Returns:
	//      String position identifier corresponding to column identifier.
	// See also:
	//      virtual int StrPosToCol(int nLine, int nStrPos) const;
	//-----------------------------------------------------------------------
	virtual int ColToStrPos(int nLine, int nDispCol) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts string position of text to column coordinates coordinate.
	// Parameters:
	//      nLine   : [in] Text line identifier.
	//      nDispCol: [in] Column coordinate.
	// Returns:
	//      Column identifier corresponding to string position identifier.
	// See also:
	//      virtual int ColToStrPos(int nLine, int nDispCol) const;
	//-----------------------------------------------------------------------
	virtual int StrPosToCol(int nLine, int nStrPos) const;
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Cleans up the buffer and all other allocated memories
	//-----------------------------------------------------------------------
	void CleanUp();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns average data size.
	// Parameters:
	//      nRowStart   : [in] Start row identifier.
	//      nRowEnd     : [in] End row identifier.
	// Returns:
	//      Average data size as integer value.
	// Remarks:
	//      Call this member function to calculate average data size based
	//      on average row length.
	//-----------------------------------------------------------------------
	virtual UINT CalcAveDataSize(int nRowStart, int nRowEnd);

	int m_nAverageLineLen;  // Stores average line length.

	CXTPNotifyConnection*   m_pConnect; // Connection object to send notifications.

private:

	int m_nTabSize;     // Tab size

	//----------------------------------------------------------------------
	// Summary:
	//      Changes the CRLF style to a specified one.
	// Parameters:
	//      szText      : [in] The line text.
	//      iNewStyle   : [in] The new style to be adopted.
	//----------------------------------------------------------------------
	//void ChangeCRLFStyle(LPTSTR szText, int iNewStyle);

	int m_iCRLFStyle;   // CRLF style 0 - DOS, 1 - UNIX, 2 - MAC

	UINT m_nCodePage;                   // CodePage for text conversion
	BOOL m_bUnicodeFileFormat;          // Determine is file format Unicode or ASCII.

	BOOL m_bOverwrite;                  // Overwrite mode status

	CXTPSyntaxEditUndoRedoManager m_mgrUndoRedo;    // Undo / redo manager.

	CXTPSyntaxEditLineMarksManager m_LineMarksManager; // line marks manager

	CXTPSyntaxEditLexParser* m_pLexParser; // Lexical parser
	CXTPSyntaxEditConfigurationManagerPtr m_ptrLexConfigurationManager; // Pointer to the Lexical configuration manager

	static CXTPSyntaxEditConfigurationManagerPtr s_ptrLexConfigurationManager_Default;
	static LONG s_dwLexConfigurationManager_DefaultRefs;

	WIN32_FIND_DATA m_oldFileData;
	WIN32_FIND_DATA m_curFileData;

	CString m_strFileExt;   // Stores file extension

	BOOL m_bIsParserEnabled; // Stores flag of parser state (enabled/disabled)

	DECLARE_XTP_SINK_MT(CXTPSyntaxEditBufferManager, m_LexConfigManSinkMT)

	virtual void OnLexConfigManEventHandler(XTP_NOTIFY_CODE Event,
											WPARAM wParam, LPARAM lParam);

	//------------------------------------------------------------------------
	// Summary:
	//      This is helper class designed to manage a set of strings
	//------------------------------------------------------------------------
	class CXTPSyntaxEditStringsManager
	{
		friend class CXTPSyntaxEditBufferManager;
	public:

		//----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//----------------------------------------------------------------------
		CXTPSyntaxEditStringsManager();

		//----------------------------------------------------------------------
		// Summary:
		//      Default object destructor. Handles cleanup and deallocation
		//----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditStringsManager();

		//----------------------------------------------------------------------
		// Summary:
		//      Returns count of managed strings
		//----------------------------------------------------------------------
		int GetCount() const;

		//----------------------------------------------------------------------
		// Summary:
		//      Returns string by given index.
		// Parameters:
		//      nRow : [in] String identifier.
		// Returns:
		//      CString object.
		//----------------------------------------------------------------------
		CString GetStr(int nRow) const;

		//----------------------------------------------------------------------
		// Summary:
		//      Returns length of string.
		// Parameters:
		//      nRow : [in] String identifier.
		// Returns:
		//      Integer value of length of string.
		//----------------------------------------------------------------------
		int GetStrLen(int nRow) const;

		//----------------------------------------------------------------------
		// Summary:
		//      Sets string at given id.
		// Parameters:
		//      nRow    : [in] String identifier.
		//      strText : [in] Reference to CString object with text to set.
		// Remarks:
		//      If given string id doesn't exist in collection function does
		//      nothing.
		// See also:
		//      void SetAtGrowStr(int nRow, LPCTSTR pcszText);
		//----------------------------------------------------------------------
		void SetAtGrowStr(int nRow, const CString& strText);

		//----------------------------------------------------------------------
		// Summary:
		//      Sets string at given id.
		// Parameters:
		//      nRow    : [in] String identifier.
		//      pcszText: [in] Pointer to null terminated string with text to set.
		// Remarks:
		//      If given string id doesn't exist in collection function does
		//      nothing.
		// See also:
		//      void SetAtGrowStr(int nRow, const CString& strText);
		//----------------------------------------------------------------------
		void SetAtGrowStr(int nRow, LPCTSTR pcszText);

		//----------------------------------------------------------------------
		// Summary:
		//      Add string.
		// Parameters:
		//      nRow    : [in] String identifier.
		//      strText : [in] Reference to CString object with text to set.
		// Remarks:
		//      If given string id exists in collection function sets string to
		//      given id.
		// See also:
		//      void SetAtGrowStr(int nRow, const CString& strText);
		//----------------------------------------------------------------------
		void InsertStr(int nRow, const CString& strText);

		//----------------------------------------------------------------------
		// Summary:
		//      Removes string.
		// Parameters:
		//      nRow    : [in] String identifier.
		// Returns:
		//      TRUE if success; FALSE if given id doesn't exist.
		//----------------------------------------------------------------------
		BOOL RemoveStr(int nRow);

		//----------------------------------------------------------------------
		// Summary:
		//      Removes all strings from collection.
		//----------------------------------------------------------------------
		void RemoveAllStrs();

		//-----------------------------------------------------------------------
		// Summary: Inserts text.
		// Parameters:
		//      nRow - [in] String identifier.
		//      nPos - [in] Start position in the string.
		//      pcszText          - [in] Text to insert.
		//      bGrowArrayIfNeed  - [in] If nRow greater than strings count and this
		//                          parameter TRUE the strings array will be grown;
		//                          if this parameter FALSE function will do nothing
		//                          and return FALSE.
		//      chLeftSpaceFiller - [in] if nPos parameter greater than string length
		//                          the char specified in this parameter will be added
		//                          to string until nPos.
		// Returns:
		//      TRUE if text successfully inserted, FALSE otherwise.
		// See Also: DeleteText
		//-----------------------------------------------------------------------
		BOOL InsertText(int nRow, int nPos, LPCTSTR pcszText,
						BOOL bGrowArrayIfNeed = FALSE,
						TCHAR chLeftSpaceFiller = _T(' ') );
		//-----------------------------------------------------------------------
		// Summary: Deletes text.
		// Parameters:
		//      nRow    - [in] String identifier.
		//      nPos    - [in] Start position in the string.
		//      nCount  - [in] Chars count to remove;
		// Returns:
		//      TRUE if text successfully deleted, FALSE otherwise.
		// See Also: InsertText
		//-----------------------------------------------------------------------
		BOOL DeleteText(int nRow, int nPos, int nCount);

	protected:

		//----------------------------------------------------------------------
		// Summary:
		//      Gets string by given index.
		// Parameters:
		//      nRow : [in] id of string.
		//      bGrowArrayIfNeed : [in] Flag to grow array of strings.Default is TRUE
		// Returns:
		//      CString pointer.
		//----------------------------------------------------------------------
		CString* GetStrData(int nRow, BOOL bGrowArrayIfNeed = TRUE);

		//----------------------------------------------------------------------
		// Summary:
		//      Gets string by given index.
		// Parameters:
		//      nRow : [in] id of string.
		// Returns:
		//      CString pointer.
		//----------------------------------------------------------------------
		CString* GetStrDataC(int nRow) const;

		//--------------------------------------------------------------------
		// Summary:
		//          Internal class, implement collection of strings.
		//--------------------------------------------------------------------
		class CStringPtrArray : public CArray<CString*, CString*>
		{
			typedef CArray<CString*, CString*> TBase;
		public:
			CStringPtrArray();
			virtual ~CStringPtrArray();

			void RemoveAll();
		};

		CStringPtrArray m_arStrings; // String collection
	};

	CXTPSyntaxEditStringsManager    m_Strings; // Stores CXTPSyntaxEditStringsManager

protected:
};
////////////////////////////////////////////////////////////////////////////
AFX_INLINE int CXTPSyntaxEditBufferManager::GetRowCount() const {
	return m_Strings.GetCount() ? m_Strings.GetCount()-1 : 0;
}

AFX_INLINE CXTPSyntaxEditLineMarksManager*
						CXTPSyntaxEditBufferManager::GetLineMarksManager() {
	return &m_LineMarksManager;
}

AFX_INLINE XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser*
								CXTPSyntaxEditBufferManager::GetLexParser() {
	return m_pLexParser;
}

AFX_INLINE CXTPSyntaxEditConfigurationManager* CXTPSyntaxEditBufferManager::GetLexConfigurationManager() {
	return m_ptrLexConfigurationManager;
}

AFX_INLINE CXTPNotifyConnection* CXTPSyntaxEditBufferManager::GetConnection()
{
	return m_pConnect;
}

AFX_INLINE CXTPSyntaxEditUndoRedoManager* CXTPSyntaxEditBufferManager::GetUndoRedoManager() {
	return &m_mgrUndoRedo;
}

AFX_INLINE int CXTPSyntaxEditBufferManager::GetTabSize() const {
	return m_nTabSize;
}

////////////////////////////////////////////////////////////////////////////
//#pragma warning(pop)
////////////////////////////////////////////////////////////////////////////
#endif // !defined(__XTPSYNTAXEDITBUFFERMANAGER_H__)
