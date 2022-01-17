// XTPSyntaxEditLexParser.h
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
#if !defined(__XTPSYNTAXEDITLEXPARSER_H__)
#define __XTPSYNTAXEDITLEXPARSER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditColorTheme;

////////////////////////////////////////////////////////////////////////////
//{{AFX_CODEJOCK_PRIVATE
typedef CList<XTP_EDIT_TEXTBLOCK, XTP_EDIT_TEXTBLOCK> CXTPSyntaxEditTextBlockList;
typedef CArray<XTP_EDIT_TEXTBLOCK, XTP_EDIT_TEXTBLOCK&> CXTPSyntaxEditTextBlockArray;
//}}AFX_CODEJOCK_PRIVATE

namespace XTPSyntaxEditLexAnalyser
{
	//========================================================================
	// Summary:
	//      This enum describes set of edit actions.
	//========================================================================
	enum XTPSyntaxEditEditAction
	{
		xtpEditActInsert    = 1,    // Edit action insert text.
		xtpEditActDelete    = 2,    // Edit action delete text.
	};

	//========================================================================
	// Summary:
	//      This class is designed to store coordinates of text region.
	// See also:
	//      XTP_EDIT_LINECOL
	//========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditTextRegion
	{
	public:
		XTP_EDIT_LINECOL m_posStart; // start position in document coordinates.
		XTP_EDIT_LINECOL m_posEnd; // end position in document coordinates.

		//--------------------------------------------------------------------
		// Summary:
		//      Clears coordinates.
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Summary:
		//      Sets coordinates.
		// Parameters:
		//      pLCStart    : [in] start position in document coordinates.
		//      pLCEnd      : [in] end position in document coordinates.
		//--------------------------------------------------------------------
		void Set(const XTP_EDIT_LINECOL* pLCStart, const XTP_EDIT_LINECOL* pLCEnd);

	};

	//{{AFX_CODEJOCK_PRIVATE
	typedef CArray<CXTPSyntaxEditTextRegion, const CXTPSyntaxEditTextRegion&> CXTPSyntaxEditInvalidZoneArray;
	//}}AFX_CODEJOCK_PRIVATE

	//===========================================================================
	// Summary:
	//      This class stores tokens definition: a list of tokens and corresponding
	//      lists of start and end tokens separators.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexTokensDef
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexTokensDef(){};
		//-----------------------------------------------------------------------
		// Summary:
		//      Copy object constructor.
		// Parameters:
		//      rSrc : [in] Reference to source CXTPSyntaxEditLexTokensDef object.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexTokensDef(const CXTPSyntaxEditLexTokensDef& rSrc);

		//-----------------------------------------------------------------------
		// Summary:
		//      Object destructor. Handles clean up and deallocations.
		//-----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexTokensDef();

		//-----------------------------------------------------------------------
		// Summary:
		//      Assignment operator for class.
		// Parameters:
		//      rSrc : [in] Reference to source CXTPSyntaxEditLexTokensDef object.
		// Returns:
		//      Reference to CXTPSyntaxEditLexTokensDef object.
		//-----------------------------------------------------------------------
		const CXTPSyntaxEditLexTokensDef& operator=(const CXTPSyntaxEditLexTokensDef& rSrc);

		CStringArray m_arTokens;        // Stores list of tokens.
		CStringArray m_arStartSeps;     // Stores list of start separators.
		CStringArray m_arEndSeps;       // Stores list of end separators.
	};

	//{{AFX_CODEJOCK_PRIVATE
	typedef CArray<CXTPSyntaxEditLexTokensDef, const CXTPSyntaxEditLexTokensDef&> CXTPSyntaxEditLexTokensDefArray;
	//}}AFX_CODEJOCK_PRIVATE

	//========================================================================
	// Summary:
	//      This class is designed to easily manipulate parser options.
	//========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexParserSchemaOptions : public CXTPCmdTarget
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexParserSchemaOptions();

		//-----------------------------------------------------------------------
		// Summary:
		//      Object constructor.
		// Parameters:
		//      rSrc : [in] Reference to source CXTPSyntaxEditLexParserSchemaOptions.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexParserSchemaOptions(const CXTPSyntaxEditLexParserSchemaOptions& rSrc);

		//-----------------------------------------------------------------------
		// Summary:
		//      Overloaded equals operator.
		// Parameters:
		//      rSrc : [in] Reference to source CXTPSyntaxEditLexParserSchemaOptions.
		//-----------------------------------------------------------------------
		const CXTPSyntaxEditLexParserSchemaOptions& operator=(const CXTPSyntaxEditLexParserSchemaOptions& rSrc);

		//global:
		BOOL    m_bFirstParseInSeparateThread;              // {0,1} default=1
		BOOL    m_bEditReparceInSeparateThread;             // {0,1} default=1
		BOOL    m_bConfigChangedReparceInSeparateThread;    // {0,1} default=1
		DWORD   m_dwMaxBackParseOffset;                     // default=100; maximum back buffer size. Some times parser look back for the text from current position.
		DWORD   m_dwEditReparceTimeout_ms;                  // default=500 ms; time out for start reparse after last key was pressed.
		DWORD   m_dwOnScreenSchCacheLifeTime_sec;           // default=180 sec; time out for on screen parsed pices of text. for memory using optimization.
		DWORD   m_dwParserThreadIdleLifeTime_ms;            // default=60 sec; time out for existing of parser thread when parser idle (no parse requests).
	};

//{{AFX_CODEJOCK_PRIVATE
	//=======================================================================
	// Summary:
	//
	//=======================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexClassSchema : public CXTPCmdTarget
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexClassSchema();
		//-----------------------------------------------------------------------
		// Summary:
		//      Object destructor. Handles clean up and deallocations.
		//-----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexClassSchema();

		virtual void AddPreBuildClass(CXTPSyntaxEditLexClass* pClass);

		virtual BOOL Build();

		virtual BOOL Copy(CXTPSyntaxEditLexClassSchema* pDest);

		virtual void Close();

		virtual void RemoveAll();

		virtual CXTPSyntaxEditLexClassPtrArray* GetClasses(BOOL bShortSch);

		virtual CXTPSyntaxEditLexClassPtrArray* GetPreBuildClasses();
		virtual CXTPSyntaxEditLexClass* GetPreBuildClass(const CString& strName);

		virtual CXTPSyntaxEditLexClass* GetNewClass(BOOL bForFile);

		virtual CXTPSyntaxEditLexObj_ActiveTags* GetActiveTagsFor(CXTPSyntaxEditLexClass* pTopClass);

		//virtual CXTPSyntaxEditLexObj_ActiveTags* GetActiveTags(BOOL bShortSch);

	protected:
		virtual BOOL Build_ChildrenFor(BOOL bDynamic, CXTPSyntaxEditLexClass* pCBase,
										CStringArray& rarAdded, int& rnNextClassID,
										int nLevel = 0);

		virtual CXTPSyntaxEditLexClassPtrArray* GetChildrenFor(CXTPSyntaxEditLexClass* pClass, BOOL& rbSelfChild);
		virtual CXTPSyntaxEditLexClassPtrArray* GetDynChildrenFor(CXTPSyntaxEditLexClass* pClass, BOOL& rbSelfChild);

		virtual void GetDynParentsList(CXTPSyntaxEditLexClass* pClass, CStringArray& rarDynParents,
										CStringArray& rarProcessedClasses);

		virtual int CanBeParentDynForChild(CString strParentName, CXTPSyntaxEditLexClass* pCChild);

		virtual BOOL Build_ShortTree();

		virtual BOOL PostBuild_Step(CXTPSyntaxEditLexClassPtrArray* pArClasses);

		virtual BOOL CopyChildrenFor(BOOL bShort, CXTPSyntaxEditLexClass* pCDest,
												CXTPSyntaxEditLexClass* pCSrc, int nLevel);

		CXTPSyntaxEditLexClassPtrArray  m_arPreBuildClassesList;

		CXTPSyntaxEditLexClassPtrArray  m_arClassesTreeShort;
		CXTPSyntaxEditLexClassPtrArray  m_arClassesTreeFull;

		//CXTPSyntaxEditLexObj_ActiveTags   m_arAllActiveTagsShort;
		//CXTPSyntaxEditLexObj_ActiveTags   m_arAllActiveTagsFull;
	};
	//}}AFX_CODEJOCK_PRIVATE

	//===========================================================================
	// Summary:
	//      CXTPSyntaxEditLexTextBlock class represents a pice (block) of text which
	//      corresponds some lex class in a text schema.
	//      It stores start position, end position, pointer to the lex class
	//      object in a class schema and some other information.
	//      Text blocks organized as a bidirectional list and as tree structure.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexTextBlock : public CXTPCmdTarget
	{
		//{{AFX_CODEJOCK_PRIVATE
		friend class CXTPSyntaxEditLexTextSchema;
		friend class CXTPSyntaxEditLexClass;
		friend class CXTPSyntaxEditLexClass_file;
		//}}AFX_CODEJOCK_PRIVATE
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexTextBlock();
		//-----------------------------------------------------------------------
		// Summary:
		//      Object destructor. Handles clean up and deallocations.
		//-----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexTextBlock();

		CXTPSyntaxEditLexClassPtr       m_ptrLexClass;  // Pointer to the LexClass object.
		CXTPSyntaxEditLexTextBlockPtr   m_ptrParent;    // Pointer to the parent block object.

		CXTPSyntaxEditLexTextBlockPtr   m_ptrPrev;      // Pointer to the previous block.
		CXTPSyntaxEditLexTextBlockPtr   m_ptrNext;      // Pointer to the next block.

		XTP_EDIT_LINECOL       m_PosStartLC;   // Text block start position.
		XTP_EDIT_LINECOL       m_PosEndLC;     // Text block start position.

		//--------------------------------------------------------------------
		// Summary:
		//      Get Text block end position.
		// Parameters:
		//      bMaxIfInvalid - [in] If TRUE returns XTP_EDIT_LINECOL::MAXPOS for
		//                      invalid position. If FALSE returns
		//                      XTP_EDIT_LINECOL::MINPOS for invalid position.
		// Returns:
		//      Value of m_PosEndLC if it is valid, otherwise maximum or minimum
		//      value depends on parameter bMaxIfInvalid.
		// See Also:
		//      XTP_EDIT_LINECOL, m_PosEndLC, m_PosStartLC.
		//--------------------------------------------------------------------
		XTP_EDIT_LINECOL    GetPosEndLC(BOOL bMaxIfInvalid = TRUE);

		//--------------------------------------------------------------------
		// Summary:
		//      Get text block start tag end position in the text.
		// Returns:
		//      Text block start tag end position in XLC form.
		// See Also:
		//      XTP_EDIT_LINECOL::GetXLC(), XTP_EDIT_XLC() macros.
		//--------------------------------------------------------------------
		DWORD GetStartTagEndXLC();

		//--------------------------------------------------------------------
		// Summary:
		//      Get text block end tag begin position in the text.
		// Returns:
		//      Text block end tag begin position in XLC form.
		// See Also:
		//      XTP_EDIT_LINECOL::GetXLC(), XTP_EDIT_XLC() macros.
		//--------------------------------------------------------------------
		DWORD GetEndTagBeginXLC();

		//--------------------------------------------------------------------
		// Summary:
		//      Get text block end tag end position in the text.
		// Returns:
		//      Text block end tag end position in XLC form.
		// See Also:
		//      XTP_EDIT_LINECOL::GetXLC(), XTP_EDIT_XLC() macros.
		//--------------------------------------------------------------------
		DWORD GetEndTagEndXLC();

		//--------------------------------------------------------------------
		// Summary:
		//      Release references to other objects.
		// Remarks:
		//      This method is used in text schema destruction process.
		// See Also:
		//      CXTPSyntaxEditLexTextSchema::Close().
		//--------------------------------------------------------------------
		virtual void Close();
		//virtual void Close(BOOL bWithPrev = FALSE, BOOL bWithNext = FALSE);

		//--------------------------------------------------------------------
		// Summary:
		//      This methods compare lex classes names for 2 text blocks.
		// Parameters:
		//      pTB2 - [in] Second text block to compare.
		// Returns:
		//      TRUE if lex classes names for 2 text blocks are equal,
		//      FALSE otherwise.
		//--------------------------------------------------------------------
		virtual BOOL IsEqualLexClasses(CXTPSyntaxEditLexTextBlock* pTB2) const;

		//--------------------------------------------------------------------
		// Summary:
		//      This method determine is the specified text block included
		//      in the current text block.
		// Parameters:
		//      pTB2 - [in] Text block pointer.
		// Returns:
		//      TRUE if the specified text block included in the current text
		//      block, FALSE otherwise.
		//--------------------------------------------------------------------
		virtual BOOL IsInclude(CXTPSyntaxEditLexTextBlock* pTB2) const;

		//--------------------------------------------------------------------
		// Summary:
		//      This method determine is text block look like closed or
		//      partly closed.
		// Returns:
		//      TRUE if some or all references to other objects are NULL,
		//      FALSE otherwise.
		//----------------------------------------------------------------
		virtual BOOL IsLookLikeClosed() const;

		//--------------------------------------------------------------------
		// Summary:
		//      This method set end position to not ended children blocks.
		// Parameters:
		//      pTxtSch - [in] Text schema pointer to send notifications.
		// Returns:
		//      Ended children blocks count.
		//----------------------------------------------------------------
		virtual int EndChildren(CXTPSyntaxEditLexTextSchema* pTxtSch = NULL);

		//--------------------------------------------------------------------
		// Summary:
		//      This method searches previous child block for the specified
		//      child. This block is parent.
		// Parameters:
		//      pChild      - [in] Child block pointer.
		//      bWithAddRef : [in]  If TRUE - InternalAddRef() is called before
		// Returns:
		//      Previous child block pointer or NULL.
		//----------------------------------------------------------------
		virtual CXTPSyntaxEditLexTextBlock* GetPrevChild(CXTPSyntaxEditLexTextBlock* pChild, BOOL bWithAddRef);

#ifdef _DEBUG
		virtual void Dump(CDumpContext& dc) const;
#endif

	protected:
		int m_nStartTagLen;     // Start tag length.
		int m_nEndTagXLCLen;    // End tag length in XLC form.

		CXTPSyntaxEditLexTextBlockPtr   m_ptrLastChild; // for internal use only. not always contains correct value
	};

	//===========================================================================
	// Summary:
	//      CXTPSyntaxEditLexTextSchema class is used to store data which necessary to
	//      parse a text and parsing results. It also contains methods to run
	//      lex classes from a class schema, methods to update text schema
	//      when text is edited, methods to reparse text, to get information
	//      which is necessary to draw colorized text and other helper methods.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexTextSchema : public CXTPCmdTarget
	{
		//{{AFX_CODEJOCK_PRIVATE
		friend class CXTPSyntaxEditLexClass;
		friend class CXTPSyntaxEditLexClass_file;
		friend class CXTPSyntaxEditLexTextBlock;
		friend class CXTPSyntaxEditLexParser;
		//}}AFX_CODEJOCK_PRIVATE
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		// Parameters:
		//      pcszSchName - [in] Schema name.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexTextSchema(LPCTSTR pcszSchName);

		//-----------------------------------------------------------------------
		// Summary:
		//      Object destructor. Handles clean up and deallocations.
		//-----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexTextSchema();

		//-----------------------------------------------------------------------
		// Summary:
		//      Get text schema name.
		// Returns:
		//      Text schema name string.
		//-----------------------------------------------------------------------
		virtual CString GetSchName() const;

		//-----------------------------------------------------------------------
		// Summary:
		//      Destroy all interlocked objects references and free all
		//      stored objects.
		// Remarks:
		//      Call this method to delete schema.
		// See Also:
		//      RemoveAll(), CXTPSyntaxEditLexClassSchema::Close().
		//-----------------------------------------------------------------------
		virtual void Close();

		//-----------------------------------------------------------------------
		// Summary:
		//      Destroy text blocks list.
		// Remarks:
		//      This helper method is called to destroy separately stored text
		//      blocks list (text schema).
		// See Also:
		//      CXTPSyntaxEditLexTextBlock::Close().
		//-----------------------------------------------------------------------
		static void AFX_CDECL Close(CXTPSyntaxEditLexTextBlock* pFirst);

		//-----------------------------------------------------------------------
		// Summary:
		//      Remove all text block from the schema.
		// Remarks:
		//      Schema object is not destroyed, just only blocks are removed.
		// See Also:
		//      RemoveAll(), CXTPSyntaxEditLexClassSchema::Close().
		//-----------------------------------------------------------------------
		virtual void RemoveAll();

		//-----------------------------------------------------------------------
		// Summary:
		//      Create a new text schema object and init its internal
		//      infrastructure (like class schema) from the current schema.
		// Remarks:
		//      Text blocks are not copied. The class schema is copied.
		// Returns:
		//      A new text schema object.
		//-----------------------------------------------------------------------
		virtual CXTPSyntaxEditLexTextSchema* Clone();

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns notification connection.
		// Returns:
		//      CXTPNotifyConnectionPtr pointer.
		//-----------------------------------------------------------------------
		virtual CXTPNotifyConnection* GetConnection();

		//-----------------------------------------------------------------------
		// Summary:
		//      This method run parse for the text.
		// Parameters:
		//      bShort              - [in] If TRUE the short (global blocks only)
		//                            class schema is used, otherwise full
		//                            (global and onscreen blocks) class schema
		//                            is used.
		//      pTxtIter            - [in] Text iterator object.
		//      pLCStart            - [in] Start text position for parse/reparse.
		//      pLCEnd              - [in] End text position for parse/reparse.
		//      bSendProgressEvents - [in] If TRUE the progress parsing events
		//                            will be sent using connection object.
		// Remarks:
		//  Special parameters values:
		//      pLCStart = NULL, pLCEnd = NULL
		//          - parse from the last parsed block to the end of the text
		//
		//      pLCStart = XTP_EDIT_LINECOL::Pos1, pLCEnd = NULL
		//          - parse (reparse) all text from the begin
		// Returns:
		//      Parse result as flag from enum XTPSyntaxEditLexParseResult.
		//-----------------------------------------------------------------------
		virtual int RunParseUpdate(BOOL bShort, CTextIter* pTxtIter,
									const XTP_EDIT_LINECOL* pLCStart = NULL,
									const XTP_EDIT_LINECOL* pLCEnd = NULL,
									BOOL bSendProgressEvents = FALSE);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns text region for which schema was updated during last
		//      parse/reparse operation.
		// Returns:
		//      CXTPSyntaxEditTextRegion object.
		//-----------------------------------------------------------------------
		virtual const CXTPSyntaxEditTextRegion& GetUpdatedTextRegion() const;

		//-----------------------------------------------------------------------
		// Summary:
		//      This method run parse for the text on screen.
		// Parameters:
		//      pTxtIter             - [in] Text iterator object.
		//      nRowStart            - [in] Start row for parse.
		//      nRowEnd              - [in] End row for parse.
		//      rPtrScreenSchFirstTB - [out] Reference to store first text
		//                             block of text schema which is built.
		// Remarks:
		//      This method perform full parsing for the specified text region
		//      using previously parsed short text schema (or global blocks schema).
		// Returns:
		//      TRUE if successful, FALSE otherwise
		//-----------------------------------------------------------------------
		virtual BOOL RunParseOnScreen(CTextIter* pTxtIter,
										int nRowStart, int nRowEnd,
										CXTPSyntaxEditLexTextBlockPtr& rPtrScreenSchFirstTB );

		//-----------------------------------------------------------------------
		// Summary:
		//      This method get row as list of blocks with different colors
		//      to draw.
		// Parameters:
		//      pTxtIter            - [in] Text iterator object.
		//      nRow                - [in] Row number.
		//      clrDefault          - [in] Default colors.
		//      rBlocks             - [out] List of color blocks.
		//      pptrTBStartCache    - [in out] Cached text block to start search.
		//                            May be NULL.
		//      pFirstSchTB         - [in] External text schema first block or NULL.
		//                            Used for OnScreen schemas.
		// Remarks:
		//      This method build list of color blocks for the specified row
		//      using external or internal text schema.
		//-----------------------------------------------------------------------
		virtual void GetRowColors(CTextIter* pTxtIter, int nRow,
									int nColFrom, int nColTo,
									const XTP_EDIT_COLORVALUES& clrDefault,
									CXTPSyntaxEditTextBlockList& rBlocks,
									CXTPSyntaxEditLexTextBlockPtr* pptrTBStartCache = NULL,
									CXTPSyntaxEditLexTextBlock* pFirstSchTB = NULL);

		//-----------------------------------------------------------------------
		// Summary:
		//      This method get row as list of collapsible blocks which
		//      include this row.
		// Parameters:
		//      nRow                - [in] Row number. By Leva: passing nRow < 0 will cause
		//                            to return block for all rows
		//      rArBlocks           - [out] List of collapsible blocks which
		//                            include this row.
		//      pptrTBStartCache    - [in out] Cached text block to start search.
		//                            May be NULL.
		// Remarks:
		//      This method build list of collapsible blocks which include
		//      the specified row using external or internal text schema.
		//-----------------------------------------------------------------------
		virtual void GetCollapsableBlocksInfo(int nRow, CXTPSyntaxEditRowsBlockArray& rArBlocks,
								CXTPSyntaxEditLexTextBlockPtr* pptrTBStartCache = NULL);

		//-----------------------------------------------------------------------
		// Summary:
		//      This helper method read attributes of the text from lex class
		//      of specified text block.
		// Parameters:
		//      rTB         - [out] Draw text block object to fill text attributes.
		//      pTextBlock  - [in] Text block object.
		//-----------------------------------------------------------------------
		virtual void GetTextAttributes(XTP_EDIT_TEXTBLOCK& rTB, CXTPSyntaxEditLexTextBlock* pTextBlock);

		//-----------------------------------------------------------------------
		// Summary:
		//      Apply color theme to the text schema.
		// Parameters:
		//      pTheme  - [in] A pointer to Color Theme object.
		//-----------------------------------------------------------------------
		virtual void ApplyTheme(CXTPSyntaxEditColorTheme* pTheme);

		//--------------------------------------------------------------------
		// Summary:
		//      Load class schema.
		// Parameters:
		//      pDescArray  - [in] A pointer to lex class descriptions array.
		// Returns:
		//      TRUE if successful, FALSE otherwise
		//--------------------------------------------------------------------
		BOOL LoadClassSchema(CXTPSyntaxEditLexClassInfoArray& arClassInfo);

		//--------------------------------------------------------------------
		// Summary:
		//      Determine is this schema support specified file extension.
		// Parameters:
		//      strExt  - [in] Extension string.
		// Remarks:
		//      Extension string format: ".ext"
		//      For example: ".cpp"
		// Returns:
		//      TRUE if specified file extension is supported, FALSE otherwise.
		//--------------------------------------------------------------------
		BOOL IsFileExtSupported(const CString& strExt);

		//--------------------------------------------------------------------
		// Summary:
		//      Get top lex class for specified file extension.
		// Parameters:
		//      strExt  - [in] Extension string.
		// Remarks:
		//      Extension string format: ".ext"
		//      For example: ".cpp"
		//      InternalAddRef() is called for the returned object.
		// Returns:
		//      Pointer to the lex class or NULL.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexClass* GetTopClassForFileExt(const CString& strExt);

		//--------------------------------------------------------------------
		// Summary:
		//      Get text block by ID. Used together with xtpEditOnTextBlockParsed
		//      notification.
		// Parameters:
		//      dwID - [in] ID from xtpEditOnTextBlockParsed notification.
		// Remarks:
		//      InternalAddRef() is called for the returned object.
		// Returns:
		//      Pointer to the text block or NULL.
		//--------------------------------------------------------------------
		virtual CXTPSyntaxEditLexTextBlock* GetLastParsedBlock(WPARAM dwID = 0);

		//--------------------------------------------------------------------
		// Summary:
		//      Get first text schema block..
		// Remarks:
		//      InternalAddRef() is called for the returned object.
		// Returns:
		//      Pointer to the text block or NULL.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexTextBlock* GetBlocks();

		//--------------------------------------------------------------------
		// Summary:
		//      Get critical section object to lock schema data (text blocks and others).
		// Returns:
		//      Pointer to the critical section object.
		//--------------------------------------------------------------------
		CMutex* GetDataLoker();

		//--------------------------------------------------------------------
		// Summary:
		//      Get critical section object to lock class schema.
		// Returns:
		//      Pointer to the critical section object.
		//--------------------------------------------------------------------
		CMutex* GetClassSchLoker();

		//--------------------------------------------------------------------
		// Summary:
		//      Get event object to break parsing process.
		// Returns:
		//      Pointer to the event object.
		//--------------------------------------------------------------------
		CEvent*             GetBreakParsingEvent();

		//{{AFX_CODEJOCK_PRIVATE
		static void AFX_CDECL TraceClrBlocks(CXTPSyntaxEditTextBlockArray& arBlocks);
		void TraceTxtBlocks(BOOL bFull); // for DEBUG only
		//}}AFX_CODEJOCK_PRIVATE

	protected:

		//========================================================================
		// Summary:
		//      This enum describes set of UpdateTextBlocks() return results.
		//========================================================================
		enum XTPSyntaxEditUpdateTBRes
		{
			xtpEditUTBNothing           = 0,        // void result
			xtpEditUTBError             = 0x0001,   // Error result.
			xtpEditUTBReparse           = 0x0002,   // one or few global blocks changed and reparse is need.
			xtpEditUTBNearestUpdated    = 0x0100,   // This flag returned from UpdateTBNearest() only.
		};

		//--------------------------------------------------------------------
		// Summary:
		//      Get Class Schema for this text schema..
		// Remarks:
		//      InternalAddRef() is called for the returned object.
		// Returns:
		//      Pointer to the Class Schema object.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexClassSchema* GetClassSchema();

		//-----------------------------------------------------------------------
		// Summary:
		//      Apply color theme to specified lex classes.
		// Parameters:
		//      pTheme      - [in] A pointer to Color Theme object.
		//      ptrClasses  - [in] A pointer to lex classes array.
		// See Also:
		//      ApplyTheme().
		//-----------------------------------------------------------------------
		virtual void ApplyThemeRecursive(CXTPSyntaxEditColorTheme* pTheme,
										CXTPSyntaxEditLexClassPtrArray* ptrClasses);

		//-----------------------------------------------------------------------
		// Summary:
		//      Update text schema positions after editing action.
		// Parameters:
		//      posFrom     - [in] Start editing position.
		//      posTo       - [in] End editing position.
		//      eEditAction - [in] Edit action. See XTPSyntaxEditEditAction.
		// Returns:
		//      Update result - values from enum XTPSyntaxEditUpdateTBRes.
		//-----------------------------------------------------------------------
		int UpdateTextBlocks(XTP_EDIT_LINECOL posFrom, XTP_EDIT_LINECOL posTo,
							int eEditAction);

//{{AFX_CODEJOCK_PRIVATE
		virtual int RunChildren(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextBlockPtr ptrTxtBlock,
								CXTPSyntaxEditLexClass* pBase,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual CXTPSyntaxEditLexTextBlock* GetPrevBlock(BOOL bWithAddRef = TRUE);

		virtual CXTPSyntaxEditLexTextBlock* GetNewBlock();
//}}AFX_CODEJOCK_PRIVATE

	private:
		int UpdateTBNearest(CXTPSyntaxEditLexTextBlock* pNarestTB1,
						int nLineDiff, int nColDiff,
						XTP_EDIT_LINECOL posFrom, XTP_EDIT_LINECOL posTo,
						int eEditAction );

		//-------------------------------
		CXTPSyntaxEditLexTextBlock* FindNearestTextBlock(XTP_EDIT_LINECOL posText);

		virtual BOOL InitScreenSch(CTextIter* pTxtIter, int nRowStart, int nRowEnd,
									CXTPSyntaxEditLexTextBlockPtr& rPtrScreenSchFirstTB,
									CXTPSyntaxEditLexTextBlockPtr& rPtrTBParentToRun);

		virtual CXTPSyntaxEditLexTextBlockPtr InitScreenSch_RunTopClass(CTextIter* pTxtIter);

		virtual CXTPSyntaxEditLexTextBlock* CopyShortTBtoFull(CXTPSyntaxEditLexTextBlock* pTB);
		virtual CXTPSyntaxEditLexClass* FindLexClassByID(CXTPSyntaxEditLexClassPtrArray* pClassesAr,
												int nClassID);

		virtual int Run_OnScreenTBStack(CTextIter* pTxtIter,
										CXTPSyntaxEditLexTextBlock* pTBParentToRun,
										CXTPSyntaxEditLexOnScreenParseCnt* pRunCnt);


		virtual void UpdateLastSchBlock(CXTPSyntaxEditLexTextBlock* pLastTB, BOOL bPermanently = FALSE);
		virtual CXTPSyntaxEditLexTextBlock* GetLastSchBlock(BOOL bWithAddRef = TRUE);


		virtual UINT SendEvent_OnTextBlockParsed(CXTPSyntaxEditLexTextBlock* pTB);

		virtual int Run_ParseUpdate0(BOOL bShort, CTextIter* pTxtIter,
									 const XTP_EDIT_LINECOL* pLCStart,
									 const XTP_EDIT_LINECOL* pLCEnd,
									 BOOL bSendProgressEvents);

		virtual int Run_ClassesUpdate1(CTextIter* pTxtIter,
										CXTPSyntaxEditLexTextBlockPtr ptrStartTB,
										BOOL bStarted);

		virtual int Run_ClassesUpdate2(CTextIter* pTxtIter,
										CXTPSyntaxEditLexClassPtrArray* pArClasses,
										CXTPSyntaxEditLexTextBlockPtr ptrParentTB,
										CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual BOOL IsBlockStartStillHere(CTextIter* pTxtIter, CXTPSyntaxEditLexTextBlock* pTB);
		virtual void FinishNewChain(BOOL bByBreak, BOOL bEOF);
		virtual void UpdateNewChainParentsChildren();
		virtual void EndBlocksByParent(CXTPSyntaxEditLexTextBlock* pTBStart, CXTPSyntaxEditLexTextBlock* pTBEnd);

		virtual void SeekNextEx(CTextIter* pTxtIter,
								CXTPSyntaxEditLexClass* pRunClass = NULL,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL,
								int nChars = 1
								);

		static void AFX_CDECL AddClrBlock(XTP_EDIT_TEXTBLOCK& rClrB, CXTPSyntaxEditTextBlockArray& arBlocks);

		//==== Run parse update context members ====
		CXTPSyntaxEditTextRegion        m_curInvalidZone;

		CXTPSyntaxEditLexTextBlockPtr m_ptrNewChainTB1;
		CXTPSyntaxEditLexTextBlockPtr m_ptrNewChainTB2;
		CXTPSyntaxEditLexTextBlockPtr m_ptrOldChainTBFirst;

		typedef CMap<WPARAM, WPARAM, CXTPSyntaxEditLexTextBlockPtr, CXTPSyntaxEditLexTextBlockPtr&>
				CXTPSyntaxEditLexID2TBmap;

		CXTPSyntaxEditLexID2TBmap       m_mapLastParsedBlocks;
		CXTPSyntaxEditLexTextBlockPtr m_ptrLastParsedBlock;
		BOOL                m_bSendProgressEvents;

		int     m_nNoEndedClassesCount;
		//==== END Run parse update context members ====

	private:
		//CCriticalSection m_csData;
		//CCriticalSection m_csClassSch;
		CMutex m_lockData;
		CMutex m_lockClassSch;

		CEvent           m_evBreakParsing;

		CXTPSyntaxEditLexClassSchema* m_pClassSchema;

		CXTPSyntaxEditLexTextBlockPtr   m_ptrFirstBlock;
		CXTPSyntaxEditLexTextBlockPtr   m_ptrLastSchBlock;

		int m_nSeekNext_TagWaitChars;

		CString m_strSchName;

		CXTPNotifyConnectionMT* m_pConnectMT;
	};


	//===========================================================================
	// Summary:
	//      This class used to 'run' lex schema parsing in separate thread.
	//      It also control this working thread and provide interthread interaction.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexParser : public CXTPCmdTarget
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//-----------------------------------------------------------------------
		CXTPSyntaxEditLexParser();

		//-----------------------------------------------------------------------
		// Summary:
		//      Object destructor. Handles clean up and deallocations.
		//-----------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexParser();

		//-------------------------------------------------------------------------
		// Summary:
		//      Use this member function to stop parsing, destroy working thread and
		//      unadvise from all events.
		// See Also: CloseParseThread, StopParseInThread
		//-------------------------------------------------------------------------
		virtual void Close();

		//-------------------------------------------------------------------------
		// Summary:
		//      Use this member function to stop parsing, destroy working thread.
		// See Also: Close, StopParseInThread
		//-------------------------------------------------------------------------
		virtual void CloseParseThread();

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to start parsing of specified text range
		//      in separate (working) thread. If parser already running and parse
		//      other text range, the new range added to a queue.
		// Parameters:
		//      pBuffer     - A pointer to CXTPSyntaxEditBufferManager object.
		//      pLCStart    - Start text position to reparse or NULL to start from begin.
		//      pLCEnd      - End text position to reparse or NULL to parse until end.
		//      eEdinAction - Zero (0) or value from XTPSyntaxEditEditAction enum.
		//      bRunWithoutWait - Start parse immediately or wait a timeout specified
		//                        in global schema options.
		// See Also: StopParseInThread, CloseParseThread
		//-----------------------------------------------------------------------
		virtual void StartParseInThread(CXTPSyntaxEditBufferManager* pBuffer,
									  const XTP_EDIT_LINECOL* pLCStart = NULL,
									  const XTP_EDIT_LINECOL* pLCEnd = NULL,
									  int eEdinAction = 0,
									  BOOL bRunWithoutWait = FALSE);

		//-------------------------------------------------------------------------
		// Summary:
		//      Use this member function to stop parsing.
		// See Also: Close, StartParseInThread
		//-------------------------------------------------------------------------
		virtual void StopParseInThread();

		//-------------------------------------------------------------------------
		// Summary:
		//      Use this member function to stop parsing in thread before call
		//      OnEditChanged.
		// See Also: OnEditChanged
		//-------------------------------------------------------------------------
		virtual void OnBeforeEditChanged();

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to reparse text after editing.
		// Parameters:
		//      posFrom     - Start position for edit action.
		//      posTo       - End position for edit action.
		//      eEdinAction - Value from XTPSyntaxEditEditAction enum.
		//      pBuffer     - A pointer to CXTPSyntaxEditBufferManager object.
		// See Also: OnBeforeEditChanged
		//-----------------------------------------------------------------------
		virtual void OnEditChanged(const XTP_EDIT_LINECOL& posFrom,
								const XTP_EDIT_LINECOL& posTo, int eEditAction,
								CXTPSyntaxEditBufferManager* pBuffer);

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to get active text shema.
		// Returns:
		//      A pointer to CXTPSyntaxEditLexTextSchema.
		// See Also: SetTextSchema
		//-----------------------------------------------------------------------
		virtual CXTPSyntaxEditLexTextSchema* GetTextSchema();

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to set active text shema.
		// Parameters:
		//      pTextSchema - A pointer to CXTPSyntaxEditLexTextSchema.
		// See Also: GetTextSchema
		//-----------------------------------------------------------------------
		virtual void SetTextSchema(CXTPSyntaxEditLexTextSchema* pTextSchema);

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to get text shema global options.
		// Parameters:
		//      strExt - A file extension to lookup schema.
		// Returns:
		//      A pointer to CXTPSyntaxEditLexParserSchemaOptions.
		//-----------------------------------------------------------------------
		virtual const CXTPSyntaxEditLexParserSchemaOptions* GetSchemaOptions(const CString& strExt);

		//-----------------------------------------------------------------------
		// Summary:
		//      Get priority for a working thread.
		// Returns:
		//      A working thread priority.
		// See Also: SetParseThreadPriority, CWinThread::GetThreadPriority
		//-----------------------------------------------------------------------
		virtual int GetParseThreadPriority();

		//-----------------------------------------------------------------------
		// Summary:
		//      Set priority for a working thread.
		// Parameters:
		//      nPriority - A new working thread priority.
		// Returns:
		//      TRUE if successful, FALSE otherwise.
		// See Also:
		//      GetParseThreadPriority, CWinThread::GetThreadPriority
		//-----------------------------------------------------------------------
		virtual BOOL SetParseThreadPriority(int nPriority);

		//-----------------------------------------------------------------------
		// Summary:
		//      Use this member function to get Auto Complete words for active
		//      text shema.
		// Parameters:
		//      rArTokens   - An array to store Auto Complete words.
		//      bAppend     - If this parameter TRUE - array will be appended,
		//                    otherwise previous items will be removed from array.
		// Returns:
		//      TRUE if successful, FALSE otherwise.
		//-----------------------------------------------------------------------
		virtual BOOL GetTokensForAutoCompleate(CXTPSyntaxEditLexTokensDefArray& rArTokens, BOOL bAppend);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns notification connection.
		// Returns:
		//      CXTPNotifyConnectionPtr pointer.
		//-----------------------------------------------------------------------
		virtual CXTPNotifyConnection* GetConnection();

	protected:
		//{{AFX_CODEJOCK_PRIVATE
		class CXTPSyntaxEditParseThreadParams
		{
		public:
			CXTPSyntaxEditParseThreadParams();

			void AddParseZone(const CXTPSyntaxEditTextRegion& rZone);

			//---------------------------------
			CMutex lockThreadParams;
			CXTPSyntaxEditBufferManager* ptrBuffer;
			CXTPSyntaxEditInvalidZoneArray arInvalidZones;
			CEvent      evParseRun;
			CEvent      evRunWithoutWait;
			CEvent      evExitThread;
		};

		virtual CXTPSyntaxEditParseThreadParams* GetParseInThreadParams();
		//}}AFX_CODEJOCK_PRIVATE


		//{{AFX_CODEJOCK_PRIVATE
		virtual void SelfCloseParseThread();

		static UINT AFX_CDECL ThreadParseProc(LPVOID);

		CCriticalSection m_csParserData;
		CWinThread* m_pParseThread;
		int         m_nParseThreadPriority; // Saved priority to the parse thread

		CXTPSyntaxEditParseThreadParams m_PThreadParams;

		CXTPSyntaxEditLexTextSchema*    m_ptrTextSchema;

		typedef CMap<CString, LPCTSTR, CXTPSyntaxEditLexParserSchemaOptions*, CXTPSyntaxEditLexParserSchemaOptions*>
			CXTPSyntaxEditLexParserSchemaOptionsMap;

		CXTPSyntaxEditLexParserSchemaOptionsMap m_mapSchOptions;
		CXTPSyntaxEditLexParserSchemaOptions*   m_pSchOptions_default;

		CXTPNotifyConnection* m_pConnect;
		DECLARE_XTP_SINK_MT(CXTPSyntaxEditLexParser, m_SinkMT)
		//}}AFX_CODEJOCK_PRIVATE


		//{{AFX_CODEJOCK_PRIVATE
		virtual void OnParseEvent_NotificationHandler(XTP_NOTIFY_CODE Event,
													  WPARAM wParam, LPARAM lParam);

		virtual void GetStrsFromLVArray(CXTPSyntaxEditLexVariant* pLVArray, CStringArray& rArStrs) const;

		virtual BOOL ReadSchemaOptions(const CString& strExt,
										CXTPSyntaxEditLexTextSchema* pTextSchema,
										CXTPSyntaxEditLexParserSchemaOptions* pOpt);
		//}}AFX_CODEJOCK_PRIVATE

		private:
			void RemoveAllOptions();

	};

//}}AFX_CODEJOCK_PRIVATE

////////////////////////////////////////////////////////////////////////////
	AFX_INLINE const CXTPSyntaxEditTextRegion& CXTPSyntaxEditLexTextSchema::GetUpdatedTextRegion() const {
		return m_curInvalidZone;
	}

	AFX_INLINE CMutex* CXTPSyntaxEditLexTextSchema::GetDataLoker() {
		return &m_lockData;
	}

	AFX_INLINE CMutex* CXTPSyntaxEditLexTextSchema::GetClassSchLoker() {
		return &m_lockClassSch;
	}

	AFX_INLINE CEvent* CXTPSyntaxEditLexTextSchema::GetBreakParsingEvent() {
		return  &m_evBreakParsing;
	}

	AFX_INLINE XTP_EDIT_LINECOL    CXTPSyntaxEditLexTextBlock::GetPosEndLC(BOOL bMaxIfInvalid)
	{
		if(!m_PosEndLC.IsValidData()) {
			return bMaxIfInvalid ? XTP_EDIT_LINECOL::MAXPOS : XTP_EDIT_LINECOL::MINPOS;
		}
		return m_PosEndLC;
	}

	AFX_INLINE DWORD CXTPSyntaxEditLexTextBlock::GetStartTagEndXLC() {
		return m_PosStartLC.GetXLC() + m_nStartTagLen;
	}

	AFX_INLINE DWORD CXTPSyntaxEditLexTextBlock::GetEndTagBeginXLC() {
		return m_PosEndLC.GetXLC() - (m_nEndTagXLCLen > 0 ? m_nEndTagXLCLen : 0);
	}

	AFX_INLINE DWORD CXTPSyntaxEditLexTextBlock::GetEndTagEndXLC() {
		return m_PosEndLC.GetXLC() - (m_nEndTagXLCLen < 0 ? m_nEndTagXLCLen : 0);
	}
}

////////////////////////////////////////////////////////////////////////////
//#pragma warning(pop)
////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPSYNTAXEDITLEXPARSER_H__)
