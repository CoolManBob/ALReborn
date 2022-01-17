// XTPSyntaxEditStruct.h
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
#if !defined(__XTPSYNTAXEDITSTRUCT_H__)
#define __XTPSYNTAXEDITSTRUCT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Common/XTPColorManager.h"

namespace XTPSyntaxEditLexAnalyser
{
	class CXTPSyntaxEditLexTextBlock;
}

//===========================================================================
// max line: 1 048 575
// max col:  4 095
//===========================================================================
#define XTP_EDIT_XLC(nL, nC) ( (((DWORD)nL) << 12) | (((DWORD)nC) & 0x00000FFF))

//===========================================================================
// Summary:
//      XTP_EDIT_LINECOL structure represents coordinates of text items on the
//      displaying context in terms of line/column.
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_LINECOL
{
	int nLine;  // stores line identifier
	int nCol;   // stores column identifier

	static const XTP_EDIT_LINECOL MAXPOS; // maximum position value
	static const XTP_EDIT_LINECOL MINPOS; // minimum position value
	static const XTP_EDIT_LINECOL Pos1; // {1,0} position value

	//----------------------------------------------------------------------
	// Summary:
	//      Constructs XTP_EDIT_LINECOL from the pair of coordinates
	//      (line and column).
	// Parameters:
	//      nParamLine  : [in] Line identifier.
	//      nParamCol   : [in] Column identifier.
	//----------------------------------------------------------------------
	static const XTP_EDIT_LINECOL MakeLineCol(int nParamLine, int nParamCol);

	//----------------------------------------------------------------------
	// Summary:
	//      Returns coordinates in the packed form.
	// Returns:
	//      DWORD value.
	//----------------------------------------------------------------------
	DWORD GetXLC() const;

	//----------------------------------------------------------------------
	// Summary:
	//      Returns data validation flag.
	// Returns:
	//      TRUE if line/column identifiers > 0; FALSE otherwise
	//----------------------------------------------------------------------
	BOOL IsValidData() const;

	//----------------------------------------------------------------------
	// Summary:
	//      Clears data members.
	// Remarks:
	//      Call this member function to set line/column coordinates to 0.
	//----------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------
	// Summary:
	//      Less than operator. Compares current struct with given.
	// Parameters:
	//      pos2 : [in] Reference to struct to compare.
	// Returns:
	//      TRUE if current struct less than given; FALSE otherwise
	//----------------------------------------------------------------------
	BOOL operator < (const XTP_EDIT_LINECOL& pos2) const;

	//----------------------------------------------------------------------
	// Summary:
	//      Less than or equal operator. Compares current struct with given.
	// Parameters:
	//      pos2 : [in] Reference to struct to compare.
	// Returns:
	//      TRUE if current struct less than  or equal given; FALSE otherwise
	//----------------------------------------------------------------------
	BOOL operator <= (const XTP_EDIT_LINECOL& pos2) const;

	//----------------------------------------------------------------------
	// Summary:
	//      Greater than operator. Compares current struct with given.
	// Parameters:
	//      pos2 : [in] Reference to struct to compare.
	// Returns:
	//      TRUE if current struct greater than given; FALSE otherwise.
	//----------------------------------------------------------------------
	BOOL operator > (const XTP_EDIT_LINECOL& pos2) const;

	//----------------------------------------------------------------------
	// Summary:
	//      Greater or equal operator. Compares current struct with given.
	// Parameters:
	//      pos2 : [in] Reference to struct to compare.
	// Returns:
	//      TRUE if current struct greater than  or equal given; FALSE otherwise
	//----------------------------------------------------------------------
	BOOL operator >= (const XTP_EDIT_LINECOL& pos2) const;

	//----------------------------------------------------------------------
	// Summary:
	//      Equal operator. Compares current struct with given.
	// Parameters:
	//      pos2 : [in] Reference to struct to compare.
	// Returns:
	//      TRUE if current struct equal given; FALSE otherwise
	//----------------------------------------------------------------------
	BOOL operator == (const XTP_EDIT_LINECOL& pos2) const;
};

//===========================================================================
// XTP_EDIT_COLORVALUES struct stores the colors for different phrases
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_COLORVALUES
{
	CXTPPaintManagerColor crText;           // text color
	CXTPPaintManagerColor crBack;           // back color
	CXTPPaintManagerColor crHiliteText;     // text color of selection
	CXTPPaintManagerColor crHiliteBack;     // text color of selection back
	CXTPPaintManagerColor crInactiveHiliteText;     // inactive text color of selection
	CXTPPaintManagerColor crInactiveHiliteBack;     // inactive text color of selection back
	CXTPPaintManagerColor crLineNumberText; // Line numbers text and line color
	CXTPPaintManagerColor crLineNumberBack; // Line numbers text and line color

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign operator needed for working with CList.
	// Parameters:
	//      src : [in] Reference to source structure.
	// Returns:
	//      Reference to XTP_EDIT_COLORVALUES struct.
	//-----------------------------------------------------------------------
	const XTP_EDIT_COLORVALUES& operator=(const XTP_EDIT_COLORVALUES& src);
};

//===========================================================================
// XTP_EDIT_ROWSBLOCK struct
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_ROWSBLOCK
{
	XTP_EDIT_LINECOL lcStart;  // Start position.
	XTP_EDIT_LINECOL lcEnd;    // End Position.

	CString strCollapsedText;   //Collapsed text.

	//-----------------------------------------------------------------------
	// Summary:
	//      Default destructor.Handles clean up and deallocation.
	//-----------------------------------------------------------------------
	virtual ~XTP_EDIT_ROWSBLOCK(){};

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign operator.
	// Parameters:
	//      src : [in] Reference to source structure.
	// Returns:
	//      Reference to XTP_EDIT_COLORVALUES struct.
	//-----------------------------------------------------------------------
	const XTP_EDIT_ROWSBLOCK& operator=(const XTP_EDIT_ROWSBLOCK& src);
};

typedef CArray<XTP_EDIT_ROWSBLOCK, const XTP_EDIT_ROWSBLOCK&> CXTPSyntaxEditRowsBlockArray;

//===========================================================================
// Summary:
//      Stores collapsed block parameters.
// See also:
//      XTP_EDIT_ROWSBLOCK
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_COLLAPSEDBLOCK
{
	XTP_EDIT_ROWSBLOCK collBlock;      // Collapsed block parameters
	CRect        rcCollMark;    // Rectangle area to display collapsed block
	                            // mark.

	//-----------------------------------------------------------------------
	// Summary:
	//      Default destructor.Handles clean up and deallocation.
	//-----------------------------------------------------------------------
	virtual ~XTP_EDIT_COLLAPSEDBLOCK(){};

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign operator.
	// Parameters:
	//      src : [in] Reference to source structure.
	// Returns:
	//      Reference to XTP_EDIT_COLORVALUES struct.
	//-----------------------------------------------------------------------
	const XTP_EDIT_COLLAPSEDBLOCK& operator=(const XTP_EDIT_COLLAPSEDBLOCK& src);
};

const UINT XTP_EDIT_ROWNODE_NOTHING    = 0x00;  // Define that row has no node mark.
const UINT XTP_EDIT_ROWNODE_COLLAPSED  = 0x01;  // Define that row has collapsed node mark.
const UINT XTP_EDIT_ROWNODE_EXPANDED   = 0x02;  // Define that row has expanded node mark.
const UINT XTP_EDIT_ROWNODE_ENDMARK    = 0x04;  // Define that row has end node mark.
const UINT XTP_EDIT_ROWNODE_NODEUP     = 0x10;  // Define that row has up-line node mark.
const UINT XTP_EDIT_ROWNODE_NODEDOWN   = 0x20;  // Define that row has down-line node mark.

//===========================================================================
// Summary:
//      Used with XTP_EDIT_FONTOPTIONS struct and define unspecified member value.
// See Also: XTP_EDIT_FONTOPTIONS
//===========================================================================
#define XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION (BYTE)-1

//===========================================================================
// XTP_EDIT_FONTOPTIONS struct is a self initializing LOGFONT structure that
// initializes all values equal to XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION except
// for lfFaceName which is set to zero length.
// See Also: XTP_EDIT_FONTOPTIONS_UNSPEC_OPTION
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_FONTOPTIONS : public LOGFONT
{
	//-----------------------------------------------------------------------
	// Summary:
	//      Default constructor.
	//-----------------------------------------------------------------------
	XTP_EDIT_FONTOPTIONS();

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign operator.
	// Parameters:
	//      src : [in] Reference to source structure.
	// Returns:
	//      Reference to XTP_EDIT_FONTOPTIONS struct.
	//-----------------------------------------------------------------------
	const XTP_EDIT_FONTOPTIONS& operator=(const XTP_EDIT_FONTOPTIONS& src);
};

//===========================================================================
// Summary: XTP_EDIT_TEXTBLOCK structure stores the text blocks after parsing
//===========================================================================
struct _XTP_EXT_CLASS XTP_EDIT_TEXTBLOCK
{
	int             nPos;           // Position
	int             nNextBlockPos;  // Position for the next block
	XTP_EDIT_COLORVALUES   clrBlock;   // Color for this block
	XTP_EDIT_FONTOPTIONS  lf;// Font options for this block;

	//-----------------------------------------------------------------------
	// Summary:
	//      Default constructor.
	//-----------------------------------------------------------------------
	XTP_EDIT_TEXTBLOCK();

	//-----------------------------------------------------------------------
	// Summary:
	//      Assign operator.
	// Parameters:
	//      src : [in] Reference to source structure.
	// Returns:
	//      Reference to XTP_EDIT_TEXTBLOCK struct.
	//-----------------------------------------------------------------------
	const XTP_EDIT_TEXTBLOCK& operator=(const XTP_EDIT_TEXTBLOCK& src);
};

//===========================================================================
// Summary: This is used to define row and col selection rect
//===========================================================================
struct XTP_EDIT_ROWCOLRECT
{
	int nRow1;          // First row index for a text block.
	int nCol1;          // First column index for a text block.
	int nRow2;          // Last row index for a text block.
	int nCol2;          // Last column index for a text block.
};

//////////////////////////////
// Notification structures
//////////////////////////////

//===========================================================================
// Summary: Bookmark notification structure
//          Provided as LPARAM with WM_NOTIFY while
//          bookmarks are needed to be drawn
//===========================================================================
struct XTP_EDIT_SENMBOOKMARK
{
	NMHDR nmhdr;        // First param should be NMHDR
	HDC hDC;            // HDC for drawing
	RECT rcBookmark;    // RECT for bookmark
	int nRow;           // The row number
};

//===========================================================================
// Summary: Row column notification structure
//          Provided as LPARAM with WM_NOTIFY
//          This is used to display current row and column
//===========================================================================
struct XTP_EDIT_NMHDR_ROWCOLCHANGED
{
	NMHDR nmhdr;    // First param should be NMHDR
	int nRow;       // Current document row
	int nCol;       // Current document col
};

//===========================================================================
// Summary: Document notification structure
//          Provided as LPARAM with WM_NOTIFY
//          used to notify of a change in the document's modified state.
//===========================================================================
struct XTP_EDIT_NMHDR_DOCMODIFIED
{
	NMHDR nmhdr;    // First param should be NMHDR
	BOOL bModified; // TRUE if the document was modified.
};

//===========================================================================
// Summary: Structure to notify "Edit changed" event
//===========================================================================
struct XTP_EDIT_NMHDR_EDITCHANGED
{
	NMHDR   nmhdr;      // First param should be NMHDR
	int     nRowFrom;   // The start row for the action
	int     nRowTo;     // The end row for the action
	int     nAction;    // The actual action
};

//===========================================================================
// Summary: Structure to notify XTP_EDIT_NM_MARGINCLICKED event
//===========================================================================
struct XTP_EDIT_NMHDR_MARGINCLICKED
{
	NMHDR   nmhdr;      // First param should be NMHDR
	int     nRow;       // Document row number
	int     nDispRow;   // Visible row number (start from 1);
};

//===========================================================================
// Summary: Structure to notify XTP_EDIT_NM_UPDATESCROLLPOS event
//===========================================================================
struct XTP_EDIT_NMHDR_SETSCROLLPOS
{
	NMHDR   nmhdr;      // First param should be NMHDR
	DWORD   dwUpdate;   // Scroll position update flags can be any combination of XTP_EDIT_UPDATE_HORZ, XTP_EDIT_UPDATE_VERT or XTP_EDIT_UPDATE_DIAG.
};

//===========================================================================
// Summary: Structure to notify XTP_EDIT_NM_PARSEEVENT event
//===========================================================================
struct XTP_EDIT_NMHDR_PARSEEVENT
{
	NMHDR   nmhdr;      // First param should be NMHDR
	DWORD   code;       // Parser event. A value from XTPSyntaxEditOnParseEvent enum.
	WPARAM  wParam;     // First event parameter.
	LPARAM  lParam;     // Second event parameter.
};

#endif // !defined(__XTPSYNTAXEDITSTRUCT_H__)
