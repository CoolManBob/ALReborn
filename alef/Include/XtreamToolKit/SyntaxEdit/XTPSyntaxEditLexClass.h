// XTPSyntaxEditLexClass.h
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
#if !defined(__XTPSYNTAXEDITLEXCLASS_H__)
#define __XTPSYNTAXEDITLEXCLASS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma warning(disable: 4097 4786)

//============================================================================
// Summary:
//      XTPSyntaxEditLexAnalyser namespace is used group text parser
//      classes, separate them from others and to make class names shorter
//      without risk to be duplicated.
//============================================================================
namespace XTPSyntaxEditLexAnalyser
{
	//========================================================================
	// Summary:
	//      CXTPSyntaxEditLexVariable class is used to represent lexical variables.
	//      In the class schema variables begins from symbol <b>@<b>.
	//      Standard variables are: @alpha, @digit, @HexDigit, @specs, @EOL.
	// See Also:
	//      Classes schema syntax.
	//========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexVariable
	{
	public:
		//--------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexVariable();

		//--------------------------------------------------------------------
		// Summary:
		//      Default object destructor.
		//--------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexVariable();

		//--------------------------------------------------------------------
		// Summary:
		//      XTPSyntaxEditLexVarID type defines the constants used to identify
		//      standard lexical variables: @alpha, @digit, @HexDigit,
		//      @specs, @EOL.
		// See Also:
		//      CXTPSyntaxEditLexVariable::m_nVarID
		//--------------------------------------------------------------------
		enum XTPSyntaxEditLexVarID
		{
			xtpEditVarID_Unknown    = 0, // Undefined value.
			xtpEditVarID_alpha      = 1, // ID for lex variable - @alpha
			xtpEditVarID_digit      = 2, // ID for lex variable - @digit
			xtpEditVarID_HexDigit   = 3, // ID for lex variable - @HexDigit
			xtpEditVarID_specs      = 4, // ID for lex variable - @specs
			xtpEditVarID_EOL        = 5, // ID for lex variable - @EOL

			// values from 0 to 100 are reserved.
			// for custom variables IDs please use values bigger than 100
		};

		//--------------------------------------------------------------------
		// Summary:
		//      XTPSyntaxEditLexVarFlags type defines the constants used to
		//      specify additional lex variable properties.
		// See Also:
		//      CXTPSyntaxEditLexVariable::m_nVarFlags.
		//--------------------------------------------------------------------
		enum XTPSyntaxEditLexVarFlags
		{
			xtpEditVarfNot      = 0x001, // specify <b>NOT</b> operation for the lex variable.

			// values from 0 to 0xFFFF are reserved.
			// for custom flags please use values bigger than 0xFFFF
		};

		//--------------------------------------------------------------------
		int m_nVarID;    // Lex variable identifier.
		CString m_strVar;
		int m_nVarFlags; // Additional lex variable properties (flags).
		//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		// Summary:
		//      Copy operator.
		// Parameters:
		//      rSrc :  [in] Constant reference to the source object.
		// Returns:
		//      Constant reference to the destination object.
		// Remarks:
		//      This operator fill object members using source object
		//      values.
		// See also:
		//      Operator copy (=) in C++.
		//--------------------------------------------------------------------
		const CXTPSyntaxEditLexVariable& operator=(const CXTPSyntaxEditLexVariable& rSrc);

		//--------------------------------------------------------------------
		// Summary:
		//      Set object members using string lex variable
		//      representation.
		// Parameters:
		//      pcszVarName :   [in] string lex variable representation (with
		//                      flags). Like "@alpha" or "@digit:not".
		// Returns:
		//      TRUE if successful, otherwise - FALSE.
		// Remarks:
		//      This method parse specified string and determine
		//      standard lex variable ID and additional flags.
		// See also:
		//      CXTPSyntaxEditLexVariable::GetVarID()
		//--------------------------------------------------------------------
		virtual BOOL SetVariable(LPCTSTR pcszVarName);

		//--------------------------------------------------------------------
		// Summary:
		//      Equal-to operator.
		// Parameters:
		//      rSrc :  [in] Constant reference to the other object.
		// Returns:
		//      It returns TRUE if both operands have the same value;
		//      otherwise, it returns FALSE.
		// Remarks:
		//      This operator determine are objects equal.
		// See also:
		//      Equality Operators in C++.
		//--------------------------------------------------------------------
		BOOL operator == (const CXTPSyntaxEditLexVariable& rSrc) const;

		//--------------------------------------------------------------------
		// Summary:
		//      Determine standard lex variable ID using string lex
		//      variable representation.
		// Parameters:
		//      pcszVarName :   [in] string lex variable representation (without
		//                      flags). Like "@alpha" or "@digit".
		// Returns:
		//      xtpEditVarID_Unknown if fails, otherwise, if successful,
		//      other value from enum XTPSyntaxEditLexVarID.
		// Remarks:
		//      This method determine standard lex variable ID using
		//      specified string.
		// See also:
		//      CXTPSyntaxEditLexVariable::XTPSyntaxEditLexVarID type,
		//      CXTPSyntaxEditLexVariable::SetVariable()
		//--------------------------------------------------------------------
		static int AFX_CDECL GetVarID(LPCTSTR pcszVarName);

		//-----------------------------------------------------------------------
		// Summary:
		//      This method is used to get lex variable chars.
		// Parameters:
		//      nVarID     - [in] A value from CXTPSyntaxEditLexVariable::XTPSyntaxEditLexVarID enum.
		//      rarVarData - [out] A reference to CStringArray to get data.
		// Returns:
		//      TRUE if successful, otherwise - FALSE.
		// See Also:
		//      CXTPSyntaxEditLexVariable::XTPSyntaxEditLexVarID
		//-----------------------------------------------------------------------
		static BOOL AFX_CDECL GetVariableData(int nVarID, CStringArray& rarVarData);

	protected:
		//--------------------------------------------------------------------
		// Summary:
		//      Initialize internal Lex variable name to ID map.
		// Remarks:
		//      This method initialize internal Lex variable name to
		//      ID map if it is not yet initialized.
		// See also:
		//      CXTPSyntaxEditLexVariable::s_mapVar2ID,
		//      CXTPSyntaxEditLexVariable::s_bVarMapInitialized()
		//--------------------------------------------------------------------
		static void AFX_CDECL InitStandartVarsIfNeed();

		static CMapStringToPtr  s_mapVar2ID; // Lex variable name to ID map.
		static BOOL             s_bVarMapInitialized; // Is initialized state of the lex variable name to ID map.
	};

	//===========================================================================
	// Summary: Enumerates types of values which supported by CXTPSyntaxEditLexVariant class.
	// See also: CXTPSyntaxEditLexVariant
	//===========================================================================
	enum XTPSyntaxEditLexVariantType
	{
		xtpEditLVT_Unknown      = 0,        // designates unknown type of value
		xtpEditLVT_className    = 0x0001,   // designates value of lex class name (or class names array) type
		xtpEditLVT_classPtr     = 0x0002,   // designates value of lex class Pointer type
		xtpEditLVT_LVArrayPtr   = 0x0004,   // designates value of LexVariants array Pointer type
		xtpEditLVT_valInt       = 0x0100,   // designates value of integer type
		xtpEditLVT_valStr       = 0x0200,   // designates value of character string (or string array) type
		xtpEditLVT_valVar       = 0x0400,   // designates value of lex variable
	};

	//===========================================================================
	// Summary:
	//      CXTPSyntaxEditLexVariant provides functionality to manipulate Lexical Analyzer's
	//      related variant type.
	// See also:
	//      XTPSyntaxEditLexVariantType, CXTPSyntaxEditLexClass, CXTPSyntaxEditLexVariable,
	//      CXTPSyntaxEditLexVariantPtrArray
	//===========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexVariant : public CXTPCmdTarget
	{
	public:
		//--------------------------------------------------------------------
		// Summary:
		//      Object constructor.
		// Parameters:
		//      rSrc        : [in] Initialize new object using other
		//                          LexVariant object.
		//      pClass      : [in] Set new object type to <b> xtpEditLVT_classPtr </b>,
		//                      and initialize </b> m_ptrClass </b> member
		//                      using specified pointer. InternalAddRef()
		//                      is called for the pClass object.
		//                      InternalRelease() will be called in the destructor.
		//      pLVArray    : [in] Set new object type to <b> xtpEditLVT_LVArrayPtr </b>,
		//                      and initialize </b> m_ptrLVArrayPtr </b>
		//                      member using specified pointer. If
		//                      bWithAddRef parameter value is TRUE
		//                      InternalAddRef() is called for the
		//                      pLVArray object.
		//      bWithAddRef : [in] If parameter value is TRUE InternalAddRef()
		//                      is called for the specified object.
		//      rSrcVar     : [in] Set new object type to <b> xtpEditLVT_valVar </b>,
		//                      and initialize </b> m_Variable </b>
		//                      member using specified object.
		//      pcszStr     : [in] Set new object type to <b> eType </b>
		//                      parameter value, and initialize
		//                      </b> m_arStrVals[0] </b> member using
		//                      specified string.
		//                      </b> eType </b> parameter value should be
		//                      a string type: </b> xtpEditLVT_valStr </b>,
		//                      </b> xtpEditLVT_className </b>.
		//      eType       : [in] Object type value from XTPSyntaxEditLexVariantType.
		//      pArStrVals  : [in] Set new object type to <b> xtpEditLVT_valStr </b>,
		//                      and initialize </b> m_arStrVals </b>
		//                      member using specified pointer.
		//      nValue      : [in] Set new object type to <b> xtpEditLVT_valInt </b>,
		//                      and initialize </b> m_nValue </b>
		//                      member using specified value.
		// Remarks:
		//      Construct an empty object, or initialize it using
		//      specified parameters.
		// See also:
		//      XTPSyntaxEditLexVariantType, CXTPSyntaxEditLexClass, CXTPSyntaxEditLexVariable,
		//      CXTPSyntaxEditLexVariantPtrArray
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexVariant();

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(const CXTPSyntaxEditLexVariant& rSrc);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(CXTPSyntaxEditLexClass* pClass);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(CXTPSyntaxEditLexVariantPtrArray* pLVArray);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(const CXTPSyntaxEditLexVariable& rSrcVar);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(LPCTSTR pcszStr, int eType = xtpEditLVT_valStr);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(const CStringArray* pArStrVals);

		// <COMBINE CXTPSyntaxEditLexVariant>
		CXTPSyntaxEditLexVariant(int    nValue);

		//--------------------------------------------------------------------
		// Summary:
		//      Default object destructor.
		//--------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexVariant();


		//====================================================================
		int             m_nObjType; // Object type value from XTPSyntaxEditLexVariantType.

		//---------------------------------
		CXTPSyntaxEditLexClassPtr               m_ptrClass;      // Value storage for type <b> xtpEditLVT_classPtr </b>
		CXTPSyntaxEditLexVariantPtrArray*   m_ptrLVArrayPtr; // Value storage for type <b> xtpEditLVT_LVArrayPtr </b>

		int             m_nValue;    // Value storage for type <b> xtpEditLVT_valInt </b>
		CStringArray    m_arStrVals; // Value storage for types <b> xtpEditLVT_className </b>, <b> xtpEditLVT_valStr </b>

		CXTPSyntaxEditLexVariable m_Variable;    // Value storage for type <b> xtpEditLVT_valVar </b>
		//====================================================================

		//--------------------------------------------------------------------
		// Summary:
		//      Copy operator.
		// Parameters:
		//      rSrc :  [in] Constant reference to the source object.
		// Returns:
		//      Constant reference to the destination object.
		// Remarks:
		//      This operator fill object members using source object values.
		// See also:
		//      Operator copy (=) in C++.
		//--------------------------------------------------------------------
		const CXTPSyntaxEditLexVariant& operator = (const CXTPSyntaxEditLexVariant& rSrc);

		//--------------------------------------------------------------------
		// Summary:
		//      Copy operator.
		// Parameters:
		//      nValue : [in] Value to be stored in the lex variant object.
		// Returns:
		//      Constant reference to the destination object.
		// Remarks:
		//      This operator sets the object type to <b> xtpEditLVT_valInt </b>,
		//      and initialize </b> m_nValue </b> member using specified value.
		// See also:
		//      Operator copy (=) in C++.
		//--------------------------------------------------------------------
		const CXTPSyntaxEditLexVariant& operator = (int nVal);

		//--------------------------------------------------------------------
		// Summary:
		//      Copy operator.
		// Parameters:
		//      rSrcVar : [in] Value to be stored in the lex variant object.
		// Returns:
		//      Constant reference to the destination object.
		// Remarks:
		//      This operator sets the object type to <b> xtpEditLVT_valVar </b>,
		//      and initialize </b> m_Variable </b> member using specified
		//      object.
		// See also:
		//      Operator copy (=) in C++.
		//--------------------------------------------------------------------
		const CXTPSyntaxEditLexVariant& operator = (const CXTPSyntaxEditLexVariable& rSrcVar);

		//--------------------------------------------------------------------
		// Summary:
		//      Equal-to operator.
		// Parameters:
		//      rSrc :  [in] Constant reference to the other object.
		// Returns:
		//      It returns TRUE if both operands have the same value;
		//      otherwise, it returns FALSE.
		// Remarks:
		//      This operator determine are objects equal.
		// See also:
		//      Equality Operators in C++.
		//--------------------------------------------------------------------
		BOOL operator == (const CXTPSyntaxEditLexVariant& rSrc) const;

		//--------------------------------------------------------------------
		// Summary:
		//      Set object members using string lex variable
		//      representation.
		// Parameters:
		//      pcszVarName :   [in] string lex variable representation (with
		//                      flags). Like "@alpha" or "@digit:not".
		// Returns:
		//      TRUE if successful, otherwise - FALSE.
		// Remarks:
		//      This method sets the object type to <b> xtpEditLVT_valVar </b>,
		//      and initialize </b> m_Variable </b> member using
		//      specified string lex variable representation.
		// See also:
		//      CXTPSyntaxEditLexVariable::SetVariable()
		//--------------------------------------------------------------------
		virtual BOOL SetVariable(LPCTSTR pcszVarName);

		//--------------------------------------------------------------------
		// Summary:
		//      Is variant object contains string(s).
		// Returns:
		//      TRUE if object is string type: </b> xtpEditLVT_valStr </b>,
		//      </b> xtpEditLVT_className </b>, otherwise - FALSE.
		// Remarks:
		//      Call this method to determine is object has a string type. In
		//      this case </b> m_arStrVals </b> member is used to access data.
		// See also:
		//      XTPSyntaxEditLexVariantType, CXTPSyntaxEditLexVariant::m_arStrVals.
		//--------------------------------------------------------------------
		virtual BOOL IsStrType() const;

		//--------------------------------------------------------------------
		// Summary:
		//      Get string for the string type variant object.
		// Returns:
		//      Value of </b>m_arStrVals[0]</b> if object is string type,
		//      otherwise - empty string.
		// Remarks:
		//      Call this method to get first string stored in the object (</b>
		//      m_arStrVals[0] </b> member value).
		// See also:
		//      CXTPSyntaxEditLexVariant::IsStrType(), XTPSyntaxEditLexVariantType,
		//              CXTPSyntaxEditLexVariant::m_arStrVals.
		//--------------------------------------------------------------------
		virtual LPCTSTR GetStr() const;

		//--------------------------------------------------------------------
		// Summary:
		//      Clone variant object.
		// Returns:
		//      Pointer to the new object.
		// Remarks:
		//      Create a new CXTPSyntaxEditLexVariant object using <b>new</b> operator and
		//      initialize it using this object. New object reference counter is
		//      initialized by 1. InternalRelease() should be called to delete
		//      unused object.
		//--------------------------------------------------------------------
		virtual CXTPSyntaxEditLexVariant* Clone() const;

	#ifdef _DEBUG
		//--------------------------------------------------------------------
		// Summary:
		//      Print stored object value(s) to the trace.
		// Remarks:
		//      Used for debug only.
		//--------------------------------------------------------------------
		void Dump(CDumpContext& dc) const;
	#endif
	};

	//========================================================================
	// Summary:
	//      CXTPSyntaxEditLexOnScreenParseCnt provides context data storage for
	//      OnScreen iterated parsing process.
	// See also:
	//      CXTPSyntaxEditLexClass, CXTPSyntaxEditLexTextSchema
	//========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexOnScreenParseCnt
	{
	public:
		//--------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexOnScreenParseCnt();

		//--------------------------------------------------------------------
		// Summary:
		//      Default object destructor.
		//--------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexOnScreenParseCnt();

		int m_nRowStart;    // Start row for parse.
		int m_nRowEnd;      // End row for parse.
		CXTPSyntaxEditLexTextBlockPtr m_ptrTBLast; // Latest found text block.
	};

	//========================================================================
	// Summary:
	//      Enumerates flags which indicate results of parse functions calls.
	// See also:
	//      CXTPSyntaxEditLexClass, CXTPSyntaxEditLexTextSchema
	//========================================================================
	enum XTPSyntaxEditLexParseResult
	{
		xtpEditLPR_Unknown      = 0,     // Undefined value.
		xtpEditLPR_StartFound   = 0x001, // Start text block was found
		xtpEditLPR_EndFound     = 0x002, // End text block was found
		xtpEditLPR_Iterated     = 0x010, // Indicate that current text position has already moved to the next char inside a function call.
		xtpEditLPR_TBpop1       = 0x020, // Indicate that new found text block should be inserted before previous one and as a parent of it.
		xtpEditLPR_Error        = 0x100, // Iterated parse process breaked due to some error. See trace for details.
		xtpEditLPR_RunFinished  = 0x200, // Run iterated parse process (or some big part of it) is finished.
		xtpEditLPR_RunBreaked   = 0x400, // Run iterated parse process (or some big part of it) is breaked.
		xtpEditLPR_RunRestart   = 0x800, // Restart run children from the beginning.

		//xtpEditLPR_NeedFullReparse = 0x1000, // Cannot reparse only specified piece of text. Full reparse is necessary.
	};

	//========================================================================
	// Summary:
	//      Enumerates parent class relation for the Lex Class object.
	// See also:
	//      CXTPSyntaxEditLexClass::CXTPSyntaxEditParent.
	//========================================================================
	enum XTPSyntaxEditLexClass_OptParent
	{
		xtpEditOptParent_Unknown    = 0,  // Undefined value.
		xtpEditOptParent_Default    = -1, // Use default relation value.
		xtpEditOptParent_file       = 1,  // Lex Class is the top in classes hierarchy: level file.
		xtpEditOptParent_direct     = 2,  // Lex Class can be child of specified parent class only directly.
		xtpEditOptParent_dyn        = 3,  // Lex Class can be child of specified parent class dynamically. (no only directly, but child of child of child ...)
	};

	//========================================================================
	// Summary:
	//      Enumerates children classes relation for the Lex Class object.
	// See also:
	//      CXTPSyntaxEditLexClass::CXTPSyntaxEditChildren
	//========================================================================
	enum XTPSyntaxEditLexClass_OptChildren
	{
		xtpEditOptChildren_Unknown  = 0, // Undefined value.
		xtpEditOptChildren_No       = 1, // Lex Class cannot have children.
		xtpEditOptChildren_Any      = 2, // Lex Class have children specified by their 'parent' property
		xtpEditOptChildren_List     = 3, // Lex Class can have only children from list and specified by their 'parent' property.
	};

	//------------------------------------------------------------------------
	// Class schema (public) attributes

	static const TCHAR XTPLEX_ATTR_TXTPREFIX[]          = _T("txt:");               // Text properties (like color, bold, ...) attributes prefix.
	static const TCHAR XTPLEX_ATTR_COLORPREFIX[]        = _T("txt:color");          // Prefix of properties which value is color.
	static const TCHAR XTPLEX_ATTR_TXT_COLORFG[]        = _T("txt:colorFG");        // Text color
	static const TCHAR XTPLEX_ATTR_TXT_COLORBK[]        = _T("txt:colorBK");        // Text back ground color
	static const TCHAR XTPLEX_ATTR_TXT_COLORSELFG[]     = _T("txt:colorSelFG");     // Selected text color
	static const TCHAR XTPLEX_ATTR_TXT_COLORSELBK[]     = _T("txt:colorSelBK");     // Selected text back ground color
	static const TCHAR XTPLEX_ATTR_TXT_BOLD[]           = _T("txt:Bold");           // Text font Bold flag.
	static const TCHAR XTPLEX_ATTR_TXT_ITALIC[]         = _T("txt:Italic");         // Text font Italic flag.
	static const TCHAR XTPLEX_ATTR_TXT_UNDERLINE[]      = _T("txt:Underline");      // Text font Underline flag.
	static const TCHAR XTPLEX_ATTR_CASESENSITIVE[]      = _T("CaseSensitive");      // Lex Class CaseSensitive flag.
	static const TCHAR XTPLEX_ATTR_COLLAPSABLE[]        = _T("Collapsable");        // Lex Class Collapsable flag.
	static const TCHAR XTPLEX_ATTR_COLLAPSEDTEXT[]      = _T("CollapsedText");      // Collapsed Lex Class mark text. (like [..])
	static const TCHAR XTPLEX_ATTR_PARSEONSCREEN[]      = _T("ParseOnScreen");      // Parse Lex Class only for screen drawing.
	static const TCHAR XTPLEX_ATTR_RESTARTRUNLOOP[]     = _T("RestartRunLoop");     // if set to 1 - run children loop will be restarted from the beginning when class ended, otherwise next child class will run.
	static const TCHAR XTPLEX_ATTR_ENDCLASSPARENT[]     = _T("End:Class:Parent");   // if set to 'this' the end block must have this class as parent.
	static const TCHAR XTPLEX_ATTR_RECURRENCEDEPTH[]    = _T("RecurrenceDepth");    // define depth of the recurrence blocks.
	static const TCHAR XTPLEX_ATTR_DISPLAYNAME[]        = _T("DisplayName");        // Friendly name to display in options dialog.

	// Global attributes
	static const TCHAR XTPLEX_ATTRG_FIRSTPARSEINSEPARATETHREAD[]            = _T("global:FirstParseInSeparateThread");              // {0,1} default=1
	static const TCHAR XTPLEX_ATTRG_EDITREPARCEINSEPARATETHREAD[]           = _T("global:EditReparceInSeparateThread");             // {0,1} default=1
	static const TCHAR XTPLEX_ATTRG_CONFIGCHANGEDREPARCEINSEPARATETHREAD[]  = _T("global:ConfigChangedReparceInSeparateThread");    // {0,1} default=1
	static const TCHAR XTPLEX_ATTRG_EDITREPARCETIMEOUT_MS[]                 = _T("global:EditReparceTimeout_ms");                   // time out for start reparse after last key was pressed.
	static const TCHAR XTPLEX_ATTRG_MAXBACKPARSEOFFSET[]                    = _T("global:MaxBackParseOffset");                      // maximum back buffer size. Some times parser look back for the text from current position.
	static const TCHAR XTPLEX_ATTRG_ONSCREENSCHCACHELIFETIME_SEC[]          = _T("global:OnScreenSchCacheLifeTime_sec");            // default= 180 sec; -1 and 0 means infinite; time out for on screen parsed pieces of text. for memory using optimization.
	static const TCHAR XTPLEX_ATTRG_PARSERTHREADIDLELIFETIME_SEC[]          = _T("global:ParserThreadIdleLifeTime_sec");            // default=60 sec; -1 and 0 means infinite; time out for existing of parser thread when parser idle (no parse requests).

	// internal attributes
	static const TCHAR XTPLEX_ATTRCLASSID[]             = _T("ClassID");            // Lex Class ID, unique for the classes tree.

	//========================================================================
	// Summary:
	//      CXTPSyntaxEditLexClass class is used to represent lexical block of text
	//      definition and to parse text.
	// See Also:
	//      CXTPSyntaxEditLexClassSchema, CXTPSyntaxEditLexObj_SpecCollT.
	//========================================================================
	class _XTP_EXT_CLASS CXTPSyntaxEditLexClass : public CXTPCmdTarget,
						 protected CXTPSyntaxEditLexObj_SpecCollT<
													CXTPSyntaxEditLexObj_Previous,
													CXTPSyntaxEditLexObj_Start,
													CXTPSyntaxEditLexObj_End,
													CXTPSyntaxEditLexObj_Token,
													CXTPSyntaxEditLexObj_Skip,
													CXTPSyntaxEditLexObj_ActiveTags >
	{
	public:

		//====================================================================
		// Summary:
		//      CXTPSyntaxEditLexClass::TBase is the base collection type for
		//      CXTPSyntaxEditLexClass. It is used to access base members and types.
		// See Also:
		//      CXTPSyntaxEditLexClassSchema, CXTPSyntaxEditLexObj_SpecCollT.
		//====================================================================
		typedef CXTPSyntaxEditLexObj_SpecCollT<
							CXTPSyntaxEditLexObj_Previous,
							CXTPSyntaxEditLexObj_Start,
							CXTPSyntaxEditLexObj_End,
							CXTPSyntaxEditLexObj_Token,
							CXTPSyntaxEditLexObj_Skip,
							CXTPSyntaxEditLexObj_ActiveTags > TBase;

		//--------------------------------------------------------------------
		// Summary:
		//      Default object constructor.
		//--------------------------------------------------------------------
		CXTPSyntaxEditLexClass();

		//--------------------------------------------------------------------
		// Summary:
		//      Default object destructor.
		//--------------------------------------------------------------------
		virtual ~CXTPSyntaxEditLexClass();

	protected:
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(0, m_previous); //   <b>'previous'</b> Lex Class property collection.
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(1, m_start);  // <b>'start'</b> Lex Class property collection.
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(2, m_end);        // <b>'end'</b> Lex Class property collection.
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(3, m_token);  // <b>'token'</b> Lex Class property collection.
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(4, m_skip);   // <b>'skip'</b> Lex Class property collection.

		// internal objects
		XTP_EDIT_LEX_CLASS_OBJ_MEMBER(5, m_ActiveTags); // Active Tags collection. Just only these tags can start or end current and children Lex Class(es).
		int m_nActiv_EndTags_Offset;  // First index of end active tags set in the Active Tags collection (m_ActiveTags member).

		//--------------------------------------------------------------------
		friend class CXTPSyntaxEditLexClass_file;
		friend class CXTPSyntaxEditLexClassSchema;
		friend class CXTPSyntaxEditLexTextSchema;
		friend class CXTPSyntaxEditLexParser;

	public:

		//--------------------------------------------------------------------
		// Summary:
		//      Get object property by property name.
		// Parameters:
		//      pcszPropName :  [in] Property name.
		// Returns:
		//      Smart pointer to CXTPSyntaxEditLexVariant object with type xtpEditLVT_LVArrayPtr.
		// Remarks:
		//      Property names are parent collection (TBase) sub-objects names;
		//      like "start:tag", "token:start:separators", ... For details see
		//      Classes schema syntax and CXTPSyntaxEditLexClass::TBase.
		// See also:
		//      CXTPSyntaxEditLexClass::TBase, Classes schema syntax.
		//--------------------------------------------------------------------
		virtual CXTPSyntaxEditLexVariantPtr PropV(LPCTSTR pcszPropName);

		//--------------------------------------------------------------------
		// Summary:
		//      Get Lex Class name.
		// Returns:
		//      Lex Class name.
		// Remarks:
		//      Lex Class name is specified in classes schema file by 'name'
		//      property.
		// See also:
		//      Classes schema syntax.
		//--------------------------------------------------------------------
		virtual CString GetClassName() const;

		//--------------------------------------------------------------------
		// Summary:
		//      Is Lex Class case sensitive.
		// Returns:
		//      "CaseSensitive" attribute value..
		// Remarks:
		//      Use this method instead of GetAttribute() to fast access
		//      "CaseSensitive" attribute value.
		// See also:
		//      GetAttribute(), XTPLEX_ATTR_CASESENSITIVE, Classes schema syntax.
		//--------------------------------------------------------------------
		virtual BOOL IsCaseSensitive();

		//--------------------------------------------------------------------
		// Summary:
		//      Is Lex Class Collapsable.
		// Returns:
		//      "Collapsable" attribute value..
		// Remarks:
		//      Use this method instead of GetAttribute() to fast access
		//      "Collapsable" attribute value.
		// See also:
		//      GetAttribute(), XTPLEX_ATTR_COLLAPSABLE, Classes schema syntax.
		//--------------------------------------------------------------------
		virtual BOOL IsCollapsable();

		//--------------------------------------------------------------------
		// Summary:
		//      Is Lex Class Collapsable.
		// Parameters:
		//      rTB :   [out] Pointer to the classes array.
		// Returns:
		//      "Collapsable" attribute value..
		// Remarks:
		//      Use this method instead of GetAttribute() to fast access
		//      "Collapsable" attribute value.
		// See also:
		//      GetAttribute(), XTPLEX_ATTR_COLLAPSABLE, Classes schema syntax.
		//--------------------------------------------------------------------
		virtual void GetTextAttributes(XTP_EDIT_TEXTBLOCK& rTB);

		//--------------------------------------------------------------------
		// Summary:
		//      Close Lex Class.
		// Remarks:
		//      Kill All relations between this class and parent and children.
		//      Close children classes. Clear internal collections. Close() must
		//      be called as first step in destroy object procedure. The second
		//      step is InternalRelease() call.
		// See also:
		//      CXTPSyntaxEditLexClass::CloseClasses()
		//--------------------------------------------------------------------
		virtual void Close();

		//--------------------------------------------------------------------
		// Summary:
		//      Close specified Lex Classes.
		// Parameters:
		//      pArClasses : [in] Pointer to the classes array.
		// Remarks:
		//      Call Close() method for every Lex Class in the specified array.
		//      Remove all objects from the array.
		// See also:
		//      CXTPSyntaxEditLexClass::Close()
		//--------------------------------------------------------------------
		static void AFX_CDECL CloseClasses(CXTPSyntaxEditLexClassPtrArray* pArClasses);

		//--------------------------------------------------------------------
		// Summary:
		//      Get Lex Class attribute by attribute name.
		// Parameters:
		//      strName :   [in] Attribute name.
		//      bDyn :      [in] If FALSE - attribute value is searched only in
		//                       this class attribute collection;
		//                       If TRUE - attribute value is searched in this
		//                       class attribute collection at the first and, if
		//                       value is not finded, it is searched in parent(s)
		//                       class(es) attribute collection(s).
		//      bDefault :  [in] Default BOOL value.
		//      nDefault :  [in] Default int value.
		// Returns:
		//      Attribute value or NULL or default value.
		// Remarks:
		//      Call this method to access Lex Classes attributes,
		//              like "txt:colorFG", ...
		// See also:
		//      xtpEditLex_attr<XXX>, SetAttribute(), Classes schema syntax.
		//--------------------------------------------------------------------
		virtual CXTPSyntaxEditLexVariantPtr GetAttribute(LPCTSTR strName, BOOL bDyn) const;

		// <COMBINE GetAttribute>
		virtual BOOL GetAttribute_BOOL(LPCTSTR strName, BOOL bDyn, BOOL bDefault = FALSE) const;

		// <COMBINE GetAttribute>
		virtual int GetAttribute_int(LPCTSTR strName, BOOL bDyn, int nDefault = 0) const;

protected:
		//--------------------------------------------------------------------
		// Summary:
		//      Sets Lex Class attribute.
		// Parameters:
		//      strName :   [in] Attribute name.
		//      rVal :      [in] Attribute value.
		// Remarks:
		//      Call this method to set Lex Class attributes, like "txt:colorFG",
		//      ...
		// See also:
		//      xtpEditLex_attr<XXX>, GetAttribute(), Classes schema syntax.
		//--------------------------------------------------------------------
		virtual void SetAttribute(CString strName, const CXTPSyntaxEditLexVariant& rVal);

		//--------------------------------------------------------------------
		// Summary:
		//      Get specified children for the Lex Class.
		// Parameters:
		//      bWithAddRef : [in] If parameter value is TRUE InternalAddRef()
		//                         is called for the returned object.
		// Remarks:
		//      Call this method(s) to get pointer to the array which contains
		//      children classes.
		// See also:
		//      GetChildren(), GetChildrenDyn(), GetChildrenSelfRef().
		//--------------------------------------------------------------------
		virtual CXTPSyntaxEditLexClassPtrArray* GetChildren();
		// <COMBINE GetChildren>
		virtual CXTPSyntaxEditLexClassPtrArray* GetChildrenDyn();
		// <COMBINE GetChildren>
		virtual CXTPSyntaxEditLexClassPtrArray* GetChildrenSelfRef();

		//--------------------------------------------------------------------
		// Summary:
		//      Get active tags for the Lex Class.
		//--------------------------------------------------------------------
		//virtual CXTPSyntaxEditLexVariantPtrArray* GetActiveTags();
		virtual CXTPSyntaxEditLexObj_ActiveTags* GetActiveTags();


		virtual int RunParse(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);
#ifdef _DEBUG
		//--------------------------------------------------------------------
		// Summary:
		//      Trace parsed lex classes. Write debug info to debug window.
		// Parameters:
		//      pcszOffset : [in] user defined text.
		//--------------------------------------------------------------------
		void DumpOffset(CDumpContext& dc, LPCTSTR pcszOffset);
#endif

	protected:
		virtual BOOL SetProp_ProcessSpecials(const XTP_EDIT_LEXPROPINFO* pPropDesc,
											 BOOL& rbProcessed);

		virtual BOOL SetProp_ProcessSpecObjects(const XTP_EDIT_LEXPROPINFO* pPropDesc,
											 BOOL& rbProcessed);

		virtual BOOL SetProp_ProcessAttributes(const XTP_EDIT_LEXPROPINFO* pPropDesc,
											 BOOL& rbProcessed);

		virtual BOOL ParseValues(const XTP_EDIT_LEXPROPINFO* pPropDesc,
								CXTPSyntaxEditLexVariantPtrArray* pLVArray);

		virtual BOOL IsQuoted(CString strValue, CString* pstrUnQuotedVal = NULL);
		virtual BOOL IsVar(CString strValue, CString* pstrVarVal = NULL);

		virtual void SortTags();

		virtual BOOL Run_Previous(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual int Run_Start(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual int Run_Skip(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock );

		virtual int Run_End(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual int Run_Token(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock );

		virtual BOOL Run_TokenSeparators(CString strToken,
											CTextIter* pTxtIter,
											CXTPSyntaxEditLexTextSchema* pTxtSch,
											CString& rstrSeparator1,
											CString& rstrSeparator2 );

		virtual BOOL Run_Tags(CTextIter* pIter,
								CXTPSyntaxEditLexTextSchema* pTxtSch,
								CXTPSyntaxEditLexVariantPtrArray* pLVTags,
								CString& rstrTagVal,
								BOOL bParseDirection_Back = FALSE);

		static BOOL AFX_CDECL Run_Tags1(CTextIter* pIter,
								CXTPSyntaxEditLexVariantPtrArray* pLVTags,
								CString& rstrTagVal,
								BOOL bCaseSensitive,
								BOOL bParseDirection_Back = FALSE);


		static BOOL AFX_CDECL Run_Tags2(CTextIter* pIter,
								 CXTPSyntaxEditLexVariant* pVTags,
								 CString& rstrTagVal,
								 BOOL bCaseSensitive,
								 BOOL bParseDirection_Back = FALSE );

		virtual BOOL Run_PrevClass(CXTPSyntaxEditLexTextBlock** ppPrevTB,
									CXTPSyntaxEditLexTextSchema* pTxtSch,
									CXTPSyntaxEditLexVariantPtrArray* pLVPrevClasses,
									int nDepth, XTP_EDIT_LINECOL* pMinPrevPos = NULL,
									CXTPSyntaxEditLexTextBlock* pPrevForParentBlockOnly = NULL,
									CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		static BOOL AFX_CDECL StrCmp(LPCTSTR pcszStr1, LPCTSTR pcszStr2, int nLen,
							BOOL bCaseSensitive);

		static BOOL AFX_CDECL StrCmpEQ(LPCTSTR pcszStr1, LPCTSTR pcszStr2, int nLen,
							  BOOL bCaseSensitive,
							  BOOL bParseDirection_Back = FALSE );

		static BOOL AFX_CDECL StrVarCmpEQ(LPCTSTR pcszStr, const CXTPSyntaxEditLexVariable& lexVar,
								 CString& rstrValue,
								 BOOL bParseDirection_Back = FALSE );

		virtual BOOL SetProp(const XTP_EDIT_LEXPROPINFO* pPropDesc);

		virtual BOOL AddChild(CXTPSyntaxEditLexClass* pChild, BOOL bDyn);

		const CXTPSyntaxEditLexClass& operator =(const CXTPSyntaxEditLexClass& rSrc);
		virtual void CopyFrom(const CXTPSyntaxEditLexClass* pSrc);

		virtual CXTPSyntaxEditLexClass* Clone(CXTPSyntaxEditLexClassSchema* pOwnerSch = NULL);

		virtual void CopyAttributes(const CXTPSyntaxEditLexClass* pSrcAttrClass, LPCTSTR pcszAttrPrefix = NULL);
		virtual void RemoveAttributes(LPCTSTR pcszAttrPrefix = NULL);

		virtual void GetParentOpt(int& rnOpt, CStringArray& rArData) const;
		virtual void GetChildrenOpt(int& rnOpt, CStringArray&   rArData) const;

		virtual CXTPSyntaxEditLexVariantPtrArray* BuildActiveTags(int& rnStartCount);

		virtual CXTPSyntaxEditLexClass* FindParent(LPCTSTR pcszClassName);

		//--------------------------------------------------------------------
		// Summary:     Fast (cached) access to "RestartRunLoop" attribute value.
		// Remarks:     If set to 1 - run children loop will be restarted from
		//              the beginning when class ended, otherwise next child
		//              class will run.
		//              Default value is 1.
		// Returns:     "RestartRunLoop" attribute value.
		// See also:    GetAttribute(), XTPLEX_ATTR_RESTARTRUNLOOP,
		//              Classes schema syntax.
		//--------------------------------------------------------------------
		virtual BOOL IsRestartRunLoop();

		virtual BOOL IsEndClassParent_this();

	protected:

		class CXTPSyntaxEditParent
		{
		public:

			XTPSyntaxEditLexClass_OptParent eOpt;
			CXTPSyntaxEditLexClassPtr               ptrDirect;
			CStringArray                arClassNames;

			void Clear()
			{
				eOpt = xtpEditOptParent_Unknown;
				ptrDirect = NULL;
				arClassNames.RemoveAll();
			}
		};

		class CXTPSyntaxEditChildren
		{
		public:
			XTPSyntaxEditLexClass_OptChildren   eOpt;
			CStringArray    arClassNames;

			void Clear() {
				eOpt = xtpEditOptChildren_Any;
				arClassNames.RemoveAll();
			}

		};

		CString m_strClassName;

		CXTPSyntaxEditParent        m_Parent;
		CXTPSyntaxEditChildren      m_Children;

	protected:
		CXTPSyntaxEditLexClassPtrArray  m_arChildrenClasses;
		CXTPSyntaxEditLexClassPtrArray  m_arDynChildrenClasses;
		CXTPSyntaxEditLexClassPtrArray  m_arChildrenSelfRefClasses;

		CMap<CString, LPCTSTR, CXTPSyntaxEditLexVariantPtr, CXTPSyntaxEditLexVariantPtr&> m_mapAttributes;

	private:
		// Cached attributes
		virtual void ClearAttributesCache();

		int m_bCaseSensitive_Cached;
		int m_bCollapsable_Cached;
		int m_bRestartRunLoop_Cached;
		int m_bEndClassParent_this_Cached;

		XTP_EDIT_TEXTBLOCK m_txtAttr_cached;
		BOOL m_bTxtAttr_cached;
	};

	//---------------------------------------------------------------------------
	class _XTP_EXT_CLASS CXTPSyntaxEditLexClass_file : public CXTPSyntaxEditLexClass
	{
	public:
		CXTPSyntaxEditLexClass_file();
		virtual ~CXTPSyntaxEditLexClass_file();

		virtual CXTPSyntaxEditLexVariantPtr PropV(LPCTSTR propName);

	protected:
		virtual int RunParse(CTextIter* pTxtIter,
							 CXTPSyntaxEditLexTextSchema* pTxtSch,
							 CXTPSyntaxEditLexTextBlockPtr& rPtrTxtBlock,
							 CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt = NULL);

		virtual CXTPSyntaxEditLexClass* Clone(CXTPSyntaxEditLexClassSchema* pOwnerSch = NULL);

		BOOL InternalInitExts(BOOL bReInit = FALSE);
		BOOL TestExt(LPCTSTR pcszExt) const;

	protected:
		BOOL         m_bExtInitialized;
		CStringArray m_arExt;
	};
}

////////////////////////////////////////////////////////////////////////////
AFX_INLINE int CXTPSyntaxEditLexClass::StrCmp(LPCTSTR pcszStr1, LPCTSTR pcszStr2,
									int nLen, BOOL bCaseSensitive)
{
	if(bCaseSensitive) {
		return _tcsncmp(pcszStr1, pcszStr2, nLen);
	}
	return _tcsnicmp(pcszStr1, pcszStr2, nLen);
}

AFX_INLINE CString CXTPSyntaxEditLexClass::GetClassName() const {
	return m_strClassName;
}

AFX_INLINE CXTPSyntaxEditLexObj_ActiveTags* CXTPSyntaxEditLexClass::GetActiveTags()
{
	return  &m_ActiveTags;
}

AFX_INLINE int CXTPSyntaxEditLexClass::GetAttribute_int(LPCTSTR strName, BOOL bDyn,
											  int nDefault) const
{
	CXTPSyntaxEditLexVariantPtr ptrAttrVal = GetAttribute(strName, bDyn);
	ASSERT(!ptrAttrVal || ptrAttrVal && ptrAttrVal->m_nObjType == xtpEditLVT_valInt);

	if(ptrAttrVal && ptrAttrVal->m_nObjType == xtpEditLVT_valInt)
	{
		return ptrAttrVal->m_nValue;
	}
	return nDefault;
}

AFX_INLINE BOOL CXTPSyntaxEditLexClass::GetAttribute_BOOL(LPCTSTR strName, BOOL bDyn,
														  BOOL bDefault) const
{
	int nVal = GetAttribute_int(strName, bDyn, -1);

	ASSERT(nVal == -1 || nVal == 0 || nVal == 1);

	return (nVal == -1) ? bDefault : (nVal != 0);
}

AFX_INLINE BOOL CXTPSyntaxEditLexClass::IsRestartRunLoop()
{
	if(m_bRestartRunLoop_Cached < 0) {
		m_bRestartRunLoop_Cached = GetAttribute_BOOL(XTPLEX_ATTR_RESTARTRUNLOOP, FALSE, TRUE);
	}
	return m_bRestartRunLoop_Cached;
}

#endif // !defined(__XTPSYNTAXEDITLEXCLASS_H__)
