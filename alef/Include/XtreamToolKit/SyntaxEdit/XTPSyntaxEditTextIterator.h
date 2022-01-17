// XTPSyntaxEditTextIterator.h
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
#if !defined(__XTPSYNTAXEDITTEXTITERATOR_H__)
#define __XTPSYNTAXEDITTEXTITERATOR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditBufferManager;

//===========================================================================
// Summary:
//      This class provides functionality to navigate text in the data
//      buffer, seek forward/backward, retrieve text by lines, determine
//      control symbols like CRLF.
//      When you create instance of CXTPSyntaxEditTextIterator class you
//      should provide pointer to data storage, represented by
//      CXTPSyntaxEditTextIterator class.
// See Also:
//      CXTPSyntaxEditBufferManager.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditTextIterator : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pData   : [in] pointer to the CXTPSyntaxEditBufferManager object.
	// See Also:
	//      CXTPSyntaxEditBufferManager.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditTextIterator(CXTPSyntaxEditBufferManager* pData);

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditTextIterator object, handles cleanup
	//      and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditTextIterator(void);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the CRLF of the current document.
	// Returns:
	//      CString object containing CRLF text.
	// Remarks:
	//      Call this member function to get current CRLF control symbol.
	//      Strings like "\r\n", "\n\r", "\r" are recognized as CRLF
	//      control symbol.
	//-----------------------------------------------------------------------
	virtual CString GetEOL(); // "\r\n", "\n\r", "\r"

	//-----------------------------------------------------------------------
	// Summary:
	//      Repositions the "current position pointer" in the internal data
	//      buffer to the begin.
	// Remarks:
	//      Call this member function to set the "current position pointer"
	//      in the internal data buffer to the begin. Besides it resets
	//      all others internal text counters.
	// See Also:
	//      SeekPos()
	//-----------------------------------------------------------------------
	virtual void SeekBegin();

	//-----------------------------------------------------------------------
	// Summary:
	//      Repositions the "current position pointer" in the internal data
	//      buffer to the specified position.
	// Parameters:
	//      posLC       : [in] Position in the text to set as current.
	//      hBreakEvent : [in] Handle to the break event to stop function execution.
	// Remarks:
	//      Call this member function to set the "current position pointer"
	//      in the internal data buffer to the specified position.
	//      For big texts this operation may take a lot of time. hBreakEvent
	//      is used to to stop function execution without reaching the specified position.
	// Returns:
	//      TRUE if successful< otherwise FALSE.
	// See Also:
	//      SeekBegin()
	//-----------------------------------------------------------------------
	virtual BOOL SeekPos(const XTP_EDIT_LINECOL& posLC, HANDLE hBreakEvent = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns text of required length.
	// Parameters:
	//      nCharsBuf : [in] length of the text in bytes to be returned.
	// Remarks:
	//      Call this member function to get text of required length.
	//      CRLF symbols are taken in account and don't remove from the
	//      returned string.
	// Returns:
	//      Pointer to the character string.
	//-----------------------------------------------------------------------
	virtual LPCTSTR GetText(int nCharsBuf = 512);  // don't remove line end chars

	//-----------------------------------------------------------------------
	// Summary:
	//      Moves current position and return pointer to the text begin.
	// Parameters:
	//      dwChars     : [in] counts to move.
	//      nCharsBuf   : [in] length of the text in bytes to be returned.
	// Returns:
	//      Pointer to the character string.
	//-----------------------------------------------------------------------
	virtual LPCTSTR SeekNext(DWORD dwChars = 1, int nCharsBuf = 512);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if end of data is reached.
	// Returns:
	//      Nonzero if the data buffer  contains no records or if you have
	//      scrolled beyond the last position; otherwise 0
	// Remarks:
	//      Call this member function to determine if end of data is
	//      reached. You can also use IsEOF to determine whether the
	//      data buffer contains any data or is empty.
	//-----------------------------------------------------------------------
	virtual BOOL IsEOF();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current position as XTP_EDIT_LINECOL structure.
	// Returns:
	//      XTP_EDIT_LINECOL structure with current position.
	// See Also:
	//      XTP_EDIT_LINECOL .
	//-----------------------------------------------------------------------
	virtual XTP_EDIT_LINECOL GetPosLC();

	//-----------------------------------------------------------------------
	// Summary:
	//      Decrement of position.
	// Parameters:
	//      rLC : [in] reference to position description structure.
	//-----------------------------------------------------------------------
	virtual void LCPosDec(XTP_EDIT_LINECOL& rLC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Increment of position.
	// Parameters:
	//      rLC : [in] reference to position description structure.
	//-----------------------------------------------------------------------
	virtual void LCPosAdd(XTP_EDIT_LINECOL& rLC, int nCharsAdd);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns length of specified line.
	// Parameters:
	//      nLine       : [in] numeric line identifier.
	//      bWithEOL    : [in] boolean flag determines if end of line symbols
	//                        are taken into account.
	// Returns:
	//      Length of specified line.
	// Remarks:
	//      Call this member function to determine length of line. You
	//      can request to ignore end of line characters.
	//-----------------------------------------------------------------------
	virtual int GetLineLen(int nLine, BOOL bWithEOL = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns length of specified line in bytes.
	// Parameters:
	//      nLine       : [in] numeric line identifier.
	//      bWithEOL    : [in] boolean flag determines if end of line symbols
	//                        are taken into account.
	// Returns:
	//      Length of specified line.
	// Remarks:
	//      Call this member function to determine length of line. You
	//      can request to ignore end of line characters.
	//-----------------------------------------------------------------------
	virtual int GetLineLenBytes(int nLine, BOOL bWithEOL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets new offset value.
	// Parameters:
	//      nOffset     : [in] new offset value.
	//-----------------------------------------------------------------------
	virtual void SetTxtOffset(int nOffsetChars);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns file name.
	// Returns:
	//      CString object containing file name.
	//-----------------------------------------------------------------------
	virtual CString GetFileExt();

	//-----------------------------------------------------------------------
	// Summary:
	//      Get maximum buffer size for back text offset.
	// Returns:
	//      Back buffer size (chars).
	// See Also:
	//      SetTxtOffset()
	//-----------------------------------------------------------------------
	virtual int GetMaxBackOffset() const;

protected:
	CXTPSyntaxEditBufferManager* m_pData; // Pointer to used CXTPSyntaxEditBufferManager object

	XTP_EDIT_LINECOL   m_LCpos;    // current position in the text.
	BOOL m_bEOF;            // flag end of file.

	CMap<DWORD, DWORD, int, int> m_mapLine2Len; // store map of line - length of line


	//-----------------------------------------------------------------------
	// Summary:
	//      Get maximum buffer size for back text offset.
	// Parameters:
	//      nOffcetB : [in] offest in bytes.
	// Returns:
	//      Pointer to TCHAR buffer.
	//-----------------------------------------------------------------------
	TCHAR* GetBuffer(int nOffcetB);

	CArray<CHAR, CHAR>  m_arBuffer; // buffer of chars
	int                 m_nBufSizeB;    // buffer length
	int                 m_nNextLine;    // next line id

	int                 m_nBufOffsetB;  // offset

	int                 m_nBufOffsetB_normal;   // offset
	int                 m_nBufOffsetB_max;  // offset

	int                 m_nTmpOffsetC;  // offset
	int                 m_nTmpOffsetB;  // offset
};

////////////////////////////////////////////////////////////////////////////
AFX_INLINE TCHAR* CXTPSyntaxEditTextIterator::GetBuffer(int nOffcetB) {
	return (TCHAR*)((CHAR*)m_arBuffer.GetData() + nOffcetB);
}
AFX_INLINE int CXTPSyntaxEditTextIterator::GetMaxBackOffset() const {
	return m_nBufOffsetB_normal;
}

////////////////////////////////////////////////////////////////////////////
#endif // !defined(__XTPSYNTAXEDITTEXTITERATOR_H__)
