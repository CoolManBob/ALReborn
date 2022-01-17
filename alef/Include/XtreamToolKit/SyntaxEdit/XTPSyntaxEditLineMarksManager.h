// XTPSyntaxEditLineMarksManager.h : header file
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
#if !defined(__XTPSYNTAXEDITLINEMARKSMANAGER_H__)
#define __XTPSYNTAXEDITLINEMARKSMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef LPCTSTR XTP_EDIT_LINEMARKTYPE;

// You can define your own line mark types (as string constants)
// Strings are case sensitive!
//
static const XTP_EDIT_LINEMARKTYPE xtpEditLMT_Bookmark     = _T("Bookmark");
static const XTP_EDIT_LINEMARKTYPE xtpEditLMT_Breakpoint   = _T("Breakpoint");
static const XTP_EDIT_LINEMARKTYPE xtpEditLMT_Collapsed    = _T("Collapsed");

//===========================================================================
// Summary: Enumerates types of mark refreshing
//===========================================================================
enum XTPSyntaxEditLineMarksRefreshType
{
	xtpEditLMRefresh_Unknown    = 0, // unknown refresh state
	xtpEditLMRefresh_Insert     = 1, // mark inserted
	xtpEditLMRefresh_Delete     = 2, // mark deleted
};


//{{AFX_CODEJOCK_PRIVATE

//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditVoidObj : public CXTPCmdTarget
{
public:
	typedef void (AFX_CDECL* TPFDeleter)(void*);

protected:
	void*       m_pPtr;     // A pointer to a handled object
	TPFDeleter  m_pfDeleter;

public:

	//-----------------------------------------------------------------------
	// Parameters:  pPtr                : [in]Pointer to the handled object.
	//          bCallInternalAddRef : [in]If this parameter is TRUE
	//                                pPtr->InternalAddRef() will be
	//                                called in constructor.
	//                                By default this parameter is FALSE.
	// Summary: Default class constructor.
	// See Also: ~CXTPSmartPtrInternalT()
	//-----------------------------------------------------------------------
	CXTPSyntaxEditVoidObj(void* pPtr, TPFDeleter pfDeleter = NULL) {
		m_pPtr = pPtr;
		m_pfDeleter = pfDeleter;
	};

	//-----------------------------------------------------------------------
	// Summary: Default class destructor.
	// Remarks: Call InternalRelease() for the not NULL handled object.
	// See Also: CXTPSmartPtrInternalT constructors
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditVoidObj()
	{
		if(m_pfDeleter) {
			m_pfDeleter(m_pPtr);
		}
		else
		{
			SAFE_DELETE(m_pPtr);
		}
	};

	//-----------------------------------------------------------------------
	// Summary: Get a handled object.
	// Returns: Pointer to the handled object.
	//-----------------------------------------------------------------------
	operator void*() const {
		return m_pPtr;
	}

	void* GetPtr() const {
		return m_pPtr;
	}

	//-----------------------------------------------------------------------
	// Summary: Check is handled object equal NULL.
	// Returns: TRUE if handled object equal NULL, else FALSE.
	//-----------------------------------------------------------------------
	BOOL operator !() const {
		return !m_pPtr;
	}
};
//---------------------------------------------------------------------------
typedef CXTPSmartPtrInternalT<CXTPSyntaxEditVoidObj> CXTPSyntaxEditVoidObjPtr;


//===========================================================================
enum XTPSyntaxEditLMParamType
{
	xtpEditLMPT_Unknown     = 0,    // unknown refresh state
	xtpEditLMPT_DWORD       = 1,
	xtpEditLMPT_double      = 2,
	xtpEditLMPT_Ptr         = 3,
};

struct _XTP_EXT_CLASS XTP_EDIT_LMPARAM
{
	// Data type
	XTPSyntaxEditLMParamType    m_eType;

	// Data
	union
	{
		DWORD       m_dwValue;  // xtpEditLMPT_DWORD
		double      m_dblValue; // xtpEditLMPT_double
	};

protected:
	CXTPSyntaxEditVoidObjPtr    m_Ptr;
public:
	// END Data

	XTP_EDIT_LMPARAM();
	virtual ~XTP_EDIT_LMPARAM();

	XTP_EDIT_LMPARAM(const XTP_EDIT_LMPARAM& rSrc);

	XTP_EDIT_LMPARAM(DWORD dwVal);
	XTP_EDIT_LMPARAM(double dblValue);
	XTP_EDIT_LMPARAM(void* pPtr);

	const XTP_EDIT_LMPARAM& operator=(const XTP_EDIT_LMPARAM& rSrc);

	const XTP_EDIT_LMPARAM& operator=(DWORD dwValue);
	const XTP_EDIT_LMPARAM& operator=(double dblValue);
	const XTP_EDIT_LMPARAM& operator=(void* pPtr);

	operator DWORD() const;
	operator double() const;
	operator void*() const;

	void SetPtr(void* pPtr, CXTPSyntaxEditVoidObj::TPFDeleter pfDeleter = NULL);

	void* GetPtr() const;

	BOOL IsValid() const;

	void Clear();
};

//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_LMDATA
{
	int         m_nRow;
	XTP_EDIT_LMPARAM   m_Param;

	//-------------------------------
	XTP_EDIT_LMDATA();
	virtual ~XTP_EDIT_LMDATA();

	XTP_EDIT_LMDATA(const XTP_EDIT_LMDATA& rSrc);
	const XTP_EDIT_LMDATA& operator=(const XTP_EDIT_LMDATA& rSrc);
};

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary: CXTPSyntaxEditLineMarksManager class provides functionality to
//          to manipulate marks on text lines.
//          Used internally by the Smart Edit control.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditLineMarksManager : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditLineMarksManager)
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditLineMarksManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditLineMarksManager object, handles
	//      cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditLineMarksManager();


	//-----------------------------------------------------------------------
	// Summary:
	//      Adds/removes required mark on given text line.
	// Parameters:
	//      nRow    : [in] row number.
	//      lmType  : [in] mark type identifier.
	//      pParam  : [in] pointer to XTP_EDIT_LMPARAM. Default is NULL.
	// Remarks:
	//      Call this member function to add or remove mark on the given
	//      text line: if line don't have specified mark it will be added;
	//      otherwise mark will be deleted.
	// See also:
	//      struct XTP_EDIT_LMPARAM.
	//-----------------------------------------------------------------------
	void AddRemoveLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
											 XTP_EDIT_LMPARAM* pParam = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets required mark on given text line.
	// Parameters:
	//      nRow    : [in]row number.
	//      lmType  : [in]mark type identifier.
	//      pParam  : [in] pointer to XTP_EDIT_LMPARAM. Default is NULL.
	// Remarks:
	//      Call this member function to add mark on the given text line.
	// See also:
	//      struct XTP_EDIT_LMPARAM.
	//-----------------------------------------------------------------------
	void SetLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
									   XTP_EDIT_LMPARAM* pParam = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Removes required mark on given text line.
	// Parameters:
	//      nRow    : [in] row number.
	//      lmType  : [in] mark type identifier.
	// Remarks:
	//      Call this member function to remove mark on the given text
	//      line.
	//-----------------------------------------------------------------------
	void DeleteLineMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if row has mark.
	// Parameters:
	//          nRow    : [in]row number.
	//          lmType  : [in]mark type identifier.
	//          pParam  : [in] pointer to XTP_EDIT_LMPARAM. Default is NULL.
	// Remarks:
	//      Call this member function to determine if specified row has
	//      given type of mark.
	// Returns:
	//      Returns TRUE if row has marked by the specified mark type;
	//      otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL HasRowMark(int nRow, const XTP_EDIT_LINEMARKTYPE lmType,
									  XTP_EDIT_LMPARAM* pParam = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns identifier of the marked line.
	// Parameters:
	//      nRow    : [in]row number.
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get line identifier that have
	//      given mark type and precedes specified line number.
	// Returns:
	//      Number identifier of the marked line.
	//-----------------------------------------------------------------------
	POSITION FindPrevLineMark(int& nRow, const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns identifier of the marked line.
	// Parameters:
	//      nRow    : [in]row number.
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get line identifier that have
	//      given mark type and following specified line number.
	// Returns:
	//      Number identifier of the marked line.
	//-----------------------------------------------------------------------
	POSITION FindNextLineMark(int& nRow, const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns identifier of the marked line.
	// Parameters:
	//      nRow    : [in]row number.
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get the last line identifier that
	//      have given mark type and following specified line number.
	// Returns:
	//      Number identifier of the marked line.
	//-----------------------------------------------------------------------
	POSITION GetLastLineMark(const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns reference to the of the first marked line position
	//      for the specific line marks type.
	// Parameters:
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get the first line identifier that
	//      have given mark type.
	// Returns:
	//      Number identifier of the marked line;
	//-----------------------------------------------------------------------
	POSITION GetFirstLineMark(const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns reference to the of the next marked line position
	//      for the specific line marks type.
	// Parameters:
	//      pos     : [in]Start POSITION value.
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get the next line identifier that
	//      have given mark type.
	// Returns:
	//      Pointer to XTP_EDIT_LMDATA with number identifier of the marked line.
	// See also:
	//      struct XTP_EDIT_LMDATA.
	//-----------------------------------------------------------------------
	XTP_EDIT_LMDATA* GetNextLineMark(POSITION& pos, const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns mark from given position.
	// Parameters:
	//      pos     : [in]Current POSITION value.
	//      lmType  : [in]mark type identifier.
	// Remarks:
	//      Call this member function to get the next line identifier that
	//      have given mark type.
	// Returns:
	//      Number identifier of the marked line.
	//-----------------------------------------------------------------------
	XTP_EDIT_LMDATA* GetLineMarkAt(const POSITION pos, const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Updates line marks for given range of rows.
	// Parameters:
	//      nRowFrom        : [in] Start row identifier.
	//      nRowTo          : [in] End row identifier.
	//      eRefreshType    : [in] refresh type.
	// See also:
	//      XTPSyntaxEditLineMarksRefreshType.
	//-----------------------------------------------------------------------
	virtual void RefreshLineMarks(int nRowFrom, int nRowTo,
									XTPSyntaxEditLineMarksRefreshType eRefreshType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Removes all line marks from the corresponding types list.
	// Parameters:
	//      lmType : [in] pointer to null terminated string with types list.
	//-----------------------------------------------------------------------
	void RemoveAll(const XTP_EDIT_LINEMARKTYPE lmType);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns count of line marks of the specified type.
	// Parameters:
	//      lmType : [in] pointer to null terminated string with types list.
	// Returns:
	//      Count of line marks as integer value.
	//-----------------------------------------------------------------------
	int GetCount(const XTP_EDIT_LINEMARKTYPE lmType);

private:
	//-----------------------------------------------------------------------
	// Summary:
	//      This internal helper class is designed to store sorted list of line
	//      marks data objects, based on CArray and provides typical
	//      operations like fast binary searching inside it and basic line
	//      marks manipulations.
	//-----------------------------------------------------------------------
	class CLineMarksList : public CXTPCmdTarget
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//      Default object destructor.
		//-----------------------------------------------------------------------
		virtual ~CLineMarksList();

		//-----------------------------------------------------------------------
		// Summary:
		//      Inserts element in the list at proper position.
		// Parameters:
		//      lmData : [in] Pointer to element description structure.
		// See also:
		//      XTP_EDIT_LMDATA.
		//-----------------------------------------------------------------------
		void Add(const XTP_EDIT_LMDATA& lmData);

		//-----------------------------------------------------------------------
		// Summary:
		//      Removes element from the list.
		// Parameters:
		//      nKey : [in] id of element
		//-----------------------------------------------------------------------
		void Remove(const int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Removes all elements from the list.
		//-----------------------------------------------------------------------
		void RemoveAll();

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns count of line marks of the specified type.
		// Returns:
		//-----------------------------------------------------------------------
		int GetCount();

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns position of the element with the specified key if exists.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      POSITION value  for an element.
		//-----------------------------------------------------------------------
		POSITION FindAt(int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns position of the element with the key following the
		//      specified.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      POSITION value  for an element.
		//-----------------------------------------------------------------------
		POSITION FindNext(int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns position of the element with the key previous to the
		//      specified.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      POSITION value  for an element.
		//-----------------------------------------------------------------------
		POSITION FindPrev(int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Refreshes marks for all corresponding lines.
		// Parameters:
		//      nRowFrom        : [in] Start row.
		//      nRowTo          : [in] End row.
		//      eRefreshType    : [in] Refresh type.
		// See also:
		//      XTPSyntaxEditLineMarksRefreshType
		//-----------------------------------------------------------------------
		void RefreshLineMarks(int nRowFrom, int nRowTo, XTPSyntaxEditLineMarksRefreshType eRefreshType);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns line mark by give position.
		// Parameters:
		//      pos     : [in] Current POSITION value.
		// Remarks:
		//      Call this member function to get the next line identifier that
		//      have given mark type.
		// Returns:
		//      Number identifier of the marked line.
		//-----------------------------------------------------------------------
		XTP_EDIT_LMDATA* GetLineMarkAt(const POSITION pos);

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns reference to the of the first marked line position
		// Remarks:
		//      Call this member function to get the first line identifier that
		//      have given mark type.
		// Returns:
		//      Number identifier of the marked line.
		//-----------------------------------------------------------------------
		POSITION GetFirstLineMark();

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns reference to the of the next marked line position
		// Parameters:
		//      pos     : [in] Current POSITION value.
		// Remarks:
		//      Call this member function to get the next line identifier that
		//      have given mark type.
		// Returns:
		//      Number identifier of the marked line.
		//-----------------------------------------------------------------------
		XTP_EDIT_LMDATA* GetNextLineMark(POSITION& pos);

	private:
		//-----------------------------------------------------------------------
		// Summary:
		//      Finds index of the element with the specified key.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      -1 if key was not found.
		//-----------------------------------------------------------------------
		int FindIndex(const int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Finds index of the element with the key value previous up to the specified.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      -1 if key was not found.
		//-----------------------------------------------------------------------
		int FindLowerIndex(const int nKey);

		//-----------------------------------------------------------------------
		// Summary:
		//      Finds index of the element with the key value next to the specified.
		// Parameters:
		//      nKey : [in] id of element.
		// Returns:
		//      -1 if key was not found.
		//-----------------------------------------------------------------------
		int FindUpperIndex(const int nKey);

	private:
		typedef CArray<XTP_EDIT_LMDATA*, XTP_EDIT_LMDATA*> CXTPSyntaxEditLineMarkPointersArray;
		CXTPSyntaxEditLineMarkPointersArray m_array; // The array with the actual line data.
	};
public:
	typedef CXTPSmartPtrInternalT<CLineMarksList> CLineMarksListPtr; // SmartPointer for the class.
private:

	//-----------------------------------------------------------------------
	// Summary:
	//      This class contains a map of line marks lists corresponding to the
	//      string values of line marks types.
	//      It allows adding a new list with the new line mark type
	//      and retrieving a smart pointer of the requested list by its
	//      line mark type.
	//-----------------------------------------------------------------------
	class CLineMarksListsMap
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//      Returns list associated with the specified mark type string.
		// Parameters:
		//      szMarkType  : [in] mark type string.
		// See also:
		//      CLineMarksListPtr
		//-----------------------------------------------------------------------
		CLineMarksListPtr GetList(LPCTSTR szMarkType);

		//-----------------------------------------------------------------------
		// Summary:
		//      Adds new list associated with the specified mark type string
		// Parameters:
		//      szMarkType  : [in] mark type string.
		// Returns:
		//      Pointer to the added list.
		// See also:
		//      CLineMarksListPtr
		//-----------------------------------------------------------------------
		CLineMarksListPtr AddList(LPCTSTR szMarkType);

		//-----------------------------------------------------------------------
		// Summary:
		//      Calls RefreshLineMarks for all lists inside.
		// Parameters:
		//      nRowFrom        : [in] Start row.
		//      nRowTo          : [in] End row.
		//      eRefreshType    : [in] Refresh type.
		// See also:
		//      XTPSyntaxEditLineMarksRefreshType
		//-----------------------------------------------------------------------
		void RefreshLineMarks(int nRowFrom, int nRowTo, XTPSyntaxEditLineMarksRefreshType eRefreshType);

	private:
		CMap<CString, LPCTSTR, CLineMarksListPtr, CLineMarksListPtr&> m_map; // A map containing line marks lists for every mark type.

	};

	CLineMarksListsMap m_mapLists; // A collection of line marks lists for all line mark types.
};

#endif // !defined(__XTPSYNTAXEDITLINEMARKSMANAGER_H__)
