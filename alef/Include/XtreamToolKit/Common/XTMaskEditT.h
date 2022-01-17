// XTMaskEditT.h interface for the CXTMaskEditT class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTMASKEDITEX_H__)
#define __XTMASKEDITEX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////
// Summary:
//     CXTMaskEditT is a template class. It allows text masking to be
//     applied to the control to format it for special editing restrictions.
//////////////////////////////////////////////////////////////////////
template <class TBase>
class CXTMaskEditT : public TBase
{

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTMaskEditT object
	//-----------------------------------------------------------------------
	CXTMaskEditT()
		: m_nStartChar(0)
		, m_nEndChar(0)
		, m_bOverType(FALSE)
		, m_bUseMask(TRUE)
		, m_bRedo(FALSE)
		, m_bModified(FALSE)
		, m_strWindowText(_T(""))
		, m_strMask(_T(""))
		, m_strLiteral(_T(""))
		, m_strDefault(_T(""))
		, m_strUndoBuffer(_T(""))
		, m_chPrompt(_T('_'))
		, m_bUpdateUndo(TRUE)
	{
	}

public:

	//-----------------------------------------------------------------------
	// Parameters:
	//     bUseMask - TRUE to enable the mask. FALSE to disable the mask.
	// Summary:
	//     Call this member function to enable or disable the mask for the mask
	//     edit control.
	//-----------------------------------------------------------------------
	void SetUseMask(BOOL bUseMask) {
		m_bUseMask = bUseMask;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to determine if the mask for the edit
	//     control can be used.
	// Returns:
	//     TRUE if the mask can be used, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL CanUseMask() const {
		return m_bUseMask && m_hWnd && ((GetStyle() & ES_READONLY) == 0) && !m_strMask.IsEmpty();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to enable or disable type over, also
	//     known as insert mode.
	// Parameters:
	//     bOverType - TRUE to enable type over.
	//-----------------------------------------------------------------------
	void SetOverType(BOOL bOverType) {
		m_bOverType = bOverType;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to determine if type over has been enabled.
	// Returns:
	//     TRUE if type over is enabled, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL CanOverType() const {
		return m_bOverType;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to determine if the index specified
	//     by 'iPos' is a valid index for the currently displayed edit text.
	// Parameters:
	//     iPos - Index of the character to check.
	// Returns:
	//     TRUE if the index is valid, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL PosInRange(int iPos) const {
		return ((iPos >= 0) && (iPos < m_strLiteral.GetLength()));
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the character that is currently used as
	//     the mask prompt. The mask prompt indicates that the field is editable.
	// Returns:
	//     A TCHAR data type.
	//-----------------------------------------------------------------------
	TCHAR GetPromptChar() const {
		return m_chPrompt;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This helper member function retrieves the string contains nLength prompt characters.
	// Parameters:
	//     nLength - Length of string to create.
	// Returns:
	//     CString contains nLength prompt characters
	//-----------------------------------------------------------------------
	CString GetPromptString(int nLength) const {
		CString strPrompt;

		while (nLength--)
			strPrompt += m_chPrompt;

		return strPrompt;
	}


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the prompt character that is
	//     displayed to the user that indicates the field can be edited.
	// Parameters:
	//     ch - A TCHAR data type.
	//     bAutoReplace - TRUE to auto replace
	//-----------------------------------------------------------------------
	void SetPromptChar(TCHAR ch, BOOL bAutoReplace = TRUE)
	{
		if (m_chPrompt == ch)
			return;

		if (bAutoReplace)
		{
			GetMaskState();

			for (int i = 0; i < m_strLiteral.GetLength(); i++)
				if (m_strLiteral[i] == m_chPrompt) m_strLiteral.SetAt(i, ch);

			for (int j = 0; j < m_strWindowText.GetLength(); j++)
				if (m_strWindowText[j] == m_chPrompt) m_strWindowText.SetAt(j, ch);

			SetMaskState();
		}

		m_chPrompt = ch;

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to perform a cut operation using the
	//     currently selected text.
	//-----------------------------------------------------------------------
	afx_msg BOOL MaskCut()
	{
		if (!CanUseMask())
			return (BOOL)DefWindowProc(WM_CUT, 0, 0);

		MaskCopy();
		MaskClear();

		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to perform a copy operation using the
	//     currently selected text.
	//-----------------------------------------------------------------------
	afx_msg BOOL MaskCopy()
	{
		if (!CanUseMask())
			return (BOOL)DefWindowProc(WM_COPY, 0, 0);

		GetMaskState();

		CString strMaskedText = GetMaskedText(m_nStartChar, m_nEndChar);
		CopyToClipboard(strMaskedText);

		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to perform a replace operation using the
	//     currently selected text.
	// Parameters:
	//     lpszNewText - Text to replace.
	//-----------------------------------------------------------------------
	void MaskReplaceSel(LPCTSTR lpszNewText)
	{

		ASSERT(CanUseMask());

		if (m_nStartChar != m_nEndChar)
			MaskDeleteSel();

		int x = m_nStartChar, nNewTextLen = (int)_tcslen(lpszNewText);
		int nWindowTextLen = m_strWindowText.GetLength();

		if (x >= nWindowTextLen)
			return;

		for (int i = 0; i < nNewTextLen; ++i)
		{
			TCHAR ch = lpszNewText[i];

			if (ch == m_chPrompt || CheckChar(ch, x))
			{
				InsertCharAt(x, ch);
				x++;

				while (x < nWindowTextLen && !IsPromptPos(x))
					x++;

				if (x >= m_strWindowText.GetLength())
					break;
			}
		}
		CorrectPosition(x);
		m_nStartChar = m_nEndChar = x;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to perform a paste operation using the
	//     current clipboard text.
	//-----------------------------------------------------------------------
	afx_msg BOOL MaskPaste()
	{
		if (!CanUseMask())
			return (BOOL)DefWindowProc(WM_PASTE, 0, 0);

		GetMaskState();

		if (!OpenClipboard())
			return FALSE;

	#ifndef _UNICODE
		HGLOBAL hglbPaste = ::GetClipboardData(CF_TEXT);
	#else
		HGLOBAL hglbPaste = ::GetClipboardData(CF_UNICODETEXT);
	#endif

		if (hglbPaste != NULL)
		{
			TCHAR* lpszClipboard = (TCHAR*)GlobalLock(hglbPaste);

			MaskReplaceSel(lpszClipboard);

			GlobalUnlock(hglbPaste);

			SetMaskState();
		}
		::CloseClipboard();

		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to delete the selection.
	//-----------------------------------------------------------------------
	void MaskDeleteSel()
	{
		if (m_nStartChar == m_nEndChar)
			return;

		CString strMaskedText = GetMaskedText(m_nEndChar);
		SetMaskedText(strMaskedText, m_nStartChar, FALSE);

		m_nEndChar = m_nStartChar;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to clear the current text selection.
	//-----------------------------------------------------------------------
	afx_msg BOOL MaskClear()
	{
		if (!CanUseMask())
			return (BOOL)DefWindowProc(WM_CLEAR, 0, 0);

		GetMaskState();

		MaskDeleteSel();

		SetMaskState();

		return TRUE;

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to undo the previous action.
	//-----------------------------------------------------------------------
	afx_msg BOOL MaskUndo()
	{
		if (!CanUseMask())
		{
			return (BOOL)DefWindowProc(EM_UNDO, 0, 0);
		}
		else
		{
			MaskGetSel();

			if (m_bRedo)
			{
				SetWindowText(m_strRedoBuffer);
			}
			else
			{
				GetWindowText(m_strRedoBuffer);
				SetWindowText(m_strUndoBuffer);
			}

			m_bRedo = !m_bRedo;
			m_bModified = TRUE;

			m_nEndChar = m_nStartChar;
			SetSel(m_nStartChar, m_nEndChar);
		}

		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to select all text in the mask edit
	//     control.
	//-----------------------------------------------------------------------
	afx_msg void MaskSelectAll()
	{
		if (!CanUseMask())
		{
			SetSel(0, -1);
		}
		else
		{
			m_nStartChar = 0;
			CorrectPosition(m_nStartChar);
			SetSel(m_nStartChar, -1);
		}
	}

	//-----------------------------------------------------------------------}
	// Summary:
	//     This member function is called to determine if the text has been modified.
	// Returns:
	//     TRUE if the text has changed, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsModified() const {
		return m_bModified;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to set masked text for the edit control.
	// Parameters:
	//     lpszMaskedText - Text string without mask.
	//     iPos          - Start position from which current masked text
	//                     will be updated.
	//                     Default value 0 (the first position).
	//     bUpdateWindow - If TRUE - edit control window text and selection
	//                     will be updated, otherwise only internal members
	//                     take changes.
	// Remarks:
	//     If a mask is used, then the mask will be applied to the text
	//     in lpszMaskedText.
	//-----------------------------------------------------------------------
	void SetMaskedText(LPCTSTR lpszMaskedText, int iPos = 0, BOOL bUpdateWindow = TRUE)
	{
		int nMaskedTextLength = (int)_tcslen(lpszMaskedText);

		m_strWindowText = m_strWindowText.Left(iPos);
		int nIndex = 0;

		for (; (iPos <  m_strLiteral.GetLength()) && (nIndex < nMaskedTextLength) ; iPos++)
		{
			TCHAR uChar = lpszMaskedText[nIndex];

			if (IsPromptPos(iPos) && ((uChar == m_chPrompt) || ProcessMask(uChar, iPos)))
			{
				m_strWindowText += (TCHAR)uChar;
				nIndex ++;
			}
			else
			{
				m_strWindowText += m_strLiteral[iPos];
			}
		}

		if (bUpdateWindow)
		{
			SetMaskState(FALSE);
		}
		else
		{
			CorrectWindowText();
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will set the mask for the edit control.
	// Parameters:
	//     lpszMask    - The format for the mask field. For example, if you wanted to set
	//                   the mask for a phone number, and you only wanted digits to be entered,
	//                   your mask might look like this; _T("(000) 000-0000").
	//     lpszLiteral - The literal format is entered here. Wherever you place an underscore
	//                   ('_') is where the user will be allowed to enter data only. Using
	//                   the phone number example; _T("(___) ___-____").
	//     lpszDefault - Text that is to be displayed when the control is initialized. For
	//                   example; _T("(800) 555-1212"). If NULL, 'lpszLiteral' is used to initialize
	//                   the edit text.
	// Remarks:
	//     The values that can be set are:
	//     <TABLE>
	//          <b>Mask Character</b>  <b>Description</b>
	//          ---------------------  ------------------------
	//          0                      Numeric (0-9)
	//          9                      Numeric (0-9) or space (' ')
	//          #                      Numeric (0-9) or space (' ') or ('+') or ('-')
	//          L                      Alpha (a-Z)
	//          ?                      Alpha (a-Z) or space (' ')
	//          A                      Alpha numeric (0-9 and a-Z)
	//          a                      Alpha numeric (0-9 and a-Z) or space (' ')
	//          &                      All print character only
	//          H                      Hex digit (0-9 and A-F)
	//          X                      Hex digit (0-9 and A-F) and space (' ')
	//          >                      Forces characters to upper case (A-Z)
	//          <                      Forces characters to lower case (a-z)
	//     </TABLE>
	//-----------------------------------------------------------------------
	virtual BOOL SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszDefault=NULL)
	{
		ASSERT(lpszMask);
		ASSERT(lpszLiteral);

		// initialize the mask for the control.
		m_strMask    = lpszMask;
		m_strLiteral = lpszLiteral;

		ASSERT(m_strMask.GetLength() == m_strLiteral.GetLength());

		if (m_strMask.GetLength() != m_strLiteral.GetLength())
			return FALSE;

		if (lpszDefault == NULL)
		{
			m_strWindowText = m_strDefault = lpszLiteral;
		}
		else
		{
			m_strWindowText = m_strDefault = lpszDefault;

			if (m_strDefault.GetLength() != m_strLiteral.GetLength())
			{
				SetMaskedText(m_strDefault, 0, FALSE);
				m_strDefault = m_strWindowText;
			}
		}

		ASSERT(m_strWindowText.GetLength() == m_strLiteral.GetLength());

		// set the window text for the control.
		m_bRedo = FALSE;
		m_bModified = FALSE;
		SetWindowText(m_strWindowText);

		m_strUndoBuffer = m_strWindowText;
		return TRUE;

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts character to Upper/Lower case.
	// Parameters:
	//     nChar - Char to be converted
	//     bUpperCase  - TRUE to convert to upper case
	// Returns:
	//     Converted character.
	//-----------------------------------------------------------------------
	TCHAR ConvertUnicodeAlpha(TCHAR nChar, BOOL bUpperCase) const
	{
		CString strTemp(nChar);
		if (bUpperCase) strTemp.MakeUpper(); else strTemp.MakeLower();
		return strTemp[0];
	}



	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used internally to validate the character indicated
	//     by 'nChar'.
	// Parameters:
	//     nChar - Contains the character code value of the key.
	//     nPos - Sting length.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL CheckChar(TCHAR& nChar, int nPos)
	{
		// do not use mask
		if (!CanUseMask())
			return FALSE;

		// control character, OK
		if (!IsPrintChar(nChar))
			return TRUE;

		// make sure the string is not longer than the mask
		if (nPos >= m_strMask.GetLength())
			return FALSE;

		if (!IsPromptPos(nPos))
			return FALSE;

		return ProcessMask(nChar, nPos);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used internally to process the character passed
	//     in by 'nChar' whose index is specified by 'nEndPos'.
	// Parameters:
	//     nChar - Contains the character code value of the key.
	//     nEndPos - Index of character in display string.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL ProcessMask(TCHAR& nChar, int nEndPos)
	{
		ASSERT(nEndPos < m_strMask.GetLength());
		if (nEndPos < 0 || nEndPos >= m_strMask.GetLength())
			return FALSE;

		// check the key against the mask
		switch (m_strMask.GetAt(nEndPos))
		{
		case '0':       // digit only //completely changed this
			return _istdigit(nChar);

		case '9':       // digit or space
			return _istdigit(nChar) || _istspace(nChar);

		case '#':       // digit or space or '+' or '-'
			return _istdigit(nChar) || (_istspace(nChar) || nChar == _T('-') || nChar == _T('+'));

		case 'd':       // decimal
			return _istdigit(nChar) || (_istspace(nChar) || nChar == _T('-') || nChar == _T('+') || nChar == _T('.') || nChar == _T(','));

		case 'L':       // alpha only
			return IsAlphaChar(nChar);

		case '?':       // alpha or space
			return IsAlphaChar(nChar) || _istspace(nChar);

		case 'A':       // alpha numeric only
			return _istalnum(nChar) || IsAlphaChar(nChar);

		case 'a':       // alpha numeric or space
			return _istalnum(nChar) || IsAlphaChar(nChar) || _istspace(nChar);

		case '&':       // all print character only
			return IsPrintChar(nChar);

		case 'H':       // hex digit
			return _istxdigit(nChar);

		case 'X':       // hex digit or space
			return _istxdigit(nChar) || _istspace(nChar);

		case '>':
			if (IsAlphaChar(nChar))
			{
				nChar = ConvertUnicodeAlpha(nChar, TRUE);
				return TRUE;
			}
			return FALSE;

		case '<':
			if (IsAlphaChar(nChar))
			{
				nChar = ConvertUnicodeAlpha(nChar, FALSE);
				return TRUE;
			}
			return FALSE;
		}

		return FALSE;
	}

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Used by class CWinApp to translate window messages before they are dispatched to theTranslateMessage andDispatchMessage Windows functions.
	// Parameters:
	//     pMsg - Points to a MSG structure that contains the message to process.
	// Returns:
	//     Nonzero if the message was translated and should not be dispatched; 0 if the message was not translated and should be dispatched.
	//-----------------------------------------------------------------------
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (!CanUseMask())
			return TBase::PreTranslateMessage(pMsg);


		// intercept Ctrl+C (copy), Ctrl+V (paste), Ctrl+X (cut) and Ctrl+Z (undo)
		// before CEdit base class gets a hold of them.

		if (pMsg->message == WM_KEYDOWN)
		{
			if (::GetKeyState(VK_SUBTRACT) < 0)
			{
				OnChar('-', 1, 1);
				return TRUE;
			}

			if (::GetKeyState(VK_ADD) < 0)
			{
				OnChar('+', 1, 1);
				return TRUE;
			}

			if (::GetKeyState(VK_CONTROL) < 0)
			{
				switch (pMsg->wParam)
				{
				case 'X':
				case 'x':
					{
						MaskCut();
						return TRUE;
					}

				case 'C':
				case 'c':
					{
						MaskCopy();
						return TRUE;
					}

				case 'V':
				case 'v':
					{
						MaskPaste();
						return TRUE;
					}

				case 'Z':
				case 'z':
					{
						MaskUndo();
						return TRUE;
					}
				}
			}

		}

		return TBase::PreTranslateMessage(pMsg);

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes character in specified position
	// Parameters:
	//     iPos - Position for character to be deleted.
	//-----------------------------------------------------------------------
	void DeleteCharAt(int iPos)
	{
		ASSERT(PosInRange(iPos));

		if (!PosInRange(iPos))
			return;

		CString strMaskedText = GetMaskedText(iPos + 1) + m_chPrompt;

		SetMaskedText(strMaskedText, iPos, FALSE);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Inserts character to specified position.
	// Parameters:
	//     iPos - Position to insert
	//     nChar - Character to be inserted
	//-----------------------------------------------------------------------
	void InsertCharAt(int iPos, TCHAR nChar)
	{
		ASSERT(PosInRange(iPos));

		if (!PosInRange(iPos))
			return;

		CString strMaskedText = CString(nChar) + GetMaskedText(iPos);

		SetMaskedText(strMaskedText, iPos, FALSE);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies text to system clipboard
	// Parameters:
	//     strText - Text to be copied
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL CopyToClipboard(const CString& strText)
	{
		if (!OpenClipboard())
			return FALSE;

		::EmptyClipboard();

		int iLen = (strText.GetLength() + 1) * sizeof(TCHAR);

		HGLOBAL hglbCopy = ::GlobalAlloc(GMEM_MOVEABLE, iLen);

		if (hglbCopy == NULL)
		{
			::CloseClipboard();
			return FALSE;
		}

		LPTSTR lptstrCopy = (TCHAR*)GlobalLock(hglbCopy);
		STRCPY_S(lptstrCopy, strText.GetLength() + 1, (LPCTSTR)strText);
		GlobalUnlock(hglbCopy);

	#ifndef _UNICODE
		::SetClipboardData(CF_TEXT, hglbCopy);
	#else
		::SetClipboardData(CF_UNICODETEXT, hglbCopy);
	#endif

		if (!::CloseClipboard())
			return FALSE;

		return TRUE;

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves masked text of the control
	// Parameters:
	//     nStartPos - Start position
	//     nEndPos - End position
	// Returns:
	//     Masked text of the control.
	//-----------------------------------------------------------------------
	CString GetMaskedText(int nStartPos = 0, int nEndPos = -1) const
	{
		if (nEndPos == -1)
			nEndPos = m_strWindowText.GetLength();
		else
			nEndPos = min(nEndPos, m_strWindowText.GetLength());

		CString strBuffer;

		for (int i = nStartPos; i < nEndPos; ++i)
		{
			if (IsPromptPos(i))
			{
				strBuffer += m_strWindowText[i];
			}
		}

		return strBuffer;

	}


protected:


//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (!CanUseMask())
		{
			TBase::OnKeyDown(nChar, nRepCnt, nFlags); // default processing.
			return;
		}

		BOOL bShift = (::GetKeyState(VK_SHIFT) < 0);
		BOOL bCtrl = (::GetKeyState(VK_CONTROL) < 0);

		switch (nChar)
		{
		case VK_UP:
		case VK_LEFT:
		case VK_HOME:
			{
				TBase::OnKeyDown(nChar, nRepCnt, nFlags);

				GetMaskState(FALSE);

				int iStartChar = m_nStartChar;
				CorrectPosition(iStartChar, FALSE);

				if (m_nStartChar < iStartChar)
				{
					m_nStartChar = iStartChar;

					if (!bShift)
						m_nEndChar = iStartChar;
				}

				SetMaskState();
			}
			return;

		case VK_DOWN:
		case VK_RIGHT:
		case VK_END:
			{
				TBase::OnKeyDown(nChar, nRepCnt, nFlags);

				GetMaskState(FALSE);

				int iEndChar = m_nEndChar;
				CorrectPosition(iEndChar);

				if (m_nEndChar > iEndChar)
				{
					m_nEndChar = iEndChar;

					if (!bShift)
						m_nStartChar = iEndChar;
				}

				SetMaskState();
			}
			return;


		case VK_INSERT:
			{
				if (bCtrl)
				{
					MaskCopy();
				}
				else if (bShift)
				{
					MaskPaste();
				}
				else
				{
					m_bOverType = !m_bOverType; // set the type-over flag
				}

			}
			return;

		case VK_DELETE:
			{
				GetMaskState();

				if (m_nStartChar == m_nEndChar)
				{
					m_nEndChar = m_nStartChar +1;
				}
				else if (bShift)
				{
					MaskCopy();
				}

				MaskDeleteSel();
				SetMaskState();
			}
			return;

		case VK_SPACE:
			{
				GetMaskState();

				if (!PosInRange(m_nStartChar) || !IsPromptPos(m_nStartChar))
				{
					NotifyPosNotInRange();
					return;
				}

				TCHAR chSpace = _T(' ');

				if (!ProcessMask(chSpace, m_nStartChar))
					chSpace = m_chPrompt;

				ProcessChar(chSpace);

				SetMaskState();
			}
			return;

		case VK_BACK:
			{
				GetMaskState(FALSE);

				if ((m_nStartChar > 0) &&
					(m_nStartChar <= m_strLiteral.GetLength()))
				{
					if (m_nStartChar == m_nEndChar)
					{
						m_nStartChar--;
						CorrectPosition(m_nStartChar, FALSE);

						if (m_bOverType && PosInRange(m_nStartChar))
						{
							m_strWindowText.SetAt(m_nStartChar, m_strDefault[m_nStartChar]);
							m_nEndChar = m_nStartChar;
						}
					}

					MaskDeleteSel();
					SetMaskState();
				}
				else
				{
					NotifyPosNotInRange();
				}
			}
			return;
		}

		TBase::OnKeyDown(nChar, nRepCnt, nFlags);

	}

	void ProcessChar(TCHAR nChar)
	{
		int iLen = m_strLiteral.GetLength();

		if (m_nStartChar >= iLen)
		{
			NotifyPosNotInRange();
			return;
		}

		if (m_nStartChar != m_nEndChar)
		{
			MaskDeleteSel();
		}

		ASSERT(m_nStartChar == m_nEndChar);

		CorrectPosition(m_nStartChar);

		if (CanOverType())
		{
			m_strWindowText.SetAt(m_nStartChar, nChar);
		}
		else
		{
			InsertCharAt(m_nStartChar, nChar);
		}

		if (m_nStartChar < iLen)
			m_nStartChar++;

		CorrectPosition(m_nStartChar);

		m_nEndChar = m_nStartChar;
	}

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (!CanUseMask())
		{
			TBase::OnChar(nChar, nRepCnt, nFlags);
			return;
		}

		switch (nChar)
		{
		case VK_UP:
		case VK_LEFT:
		case VK_DOWN:
		case VK_RIGHT:
		case VK_HOME:
		case VK_END:
		case VK_DELETE:
		case VK_SPACE:
		case VK_BACK:
			return; // handled in WM_KEYDOWN
		}

		GetMaskState();

		if (!PosInRange(m_nStartChar) || !IsPromptPos(m_nStartChar))
		{
			NotifyPosNotInRange();
			return;
		}

		TCHAR ch = (TCHAR)nChar;

		if (!CheckChar(ch, m_nStartChar))
		{
			NotifyInvalidCharacter(ch, m_strMask[m_nStartChar]);
			return;
		}

		if (IsPrintChar(ch))
		{
			ProcessChar(ch);
			SetMaskState(FALSE);
		}
		else
		{
			if (nChar != 127)
				TBase::OnChar(nChar, nRepCnt, nFlags);
		}
	}

	afx_msg void OnSetFocus(CWnd* pOldWnd)
	{
		TBase::OnSetFocus(pOldWnd);

		if (!CanUseMask())
		{
			return;
		}

		MaskGetSel();
		CorrectPosition(m_nStartChar);

		m_nEndChar = m_nStartChar;
		SetSel(m_nStartChar, m_nEndChar);

	}

	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI)
	{
		if (!CanUseMask())
		{
			pCmdUI->Enable(CanUndo());
		}
		else
		{
			pCmdUI->Enable(m_bModified);
		}

	}
//}}AFX_CODEJOCK_PRIVATE


//{{AFX_CODEJOCK_PRIVATE

	// Some goodies
	BOOL CorrectPosition(int& iPos, BOOL bForward = TRUE) // used internally
	{
		int iLen = m_strLiteral.GetLength();

		if (IsPromptPos(iPos))
			return TRUE;

		if (bForward)
		{
			for (; iPos < iLen; iPos++)
			{
				if (IsPromptPos(iPos))
					return TRUE;
			}

			for (; iPos >= 0; iPos--)
			{
				if (IsPromptPos(iPos - 1))
					return FALSE;
			}
		}
		else
		{
			for (; iPos >= 0; iPos--)
			{
				if (IsPromptPos(iPos))
					return TRUE;
			}

			for (; iPos < iLen; iPos++)
			{
				if (IsPromptPos(iPos))
					return FALSE;
			}
		}

		return FALSE;
	}

	virtual BOOL IsPrintChar(TCHAR nChar)
	{
		return _istprint(nChar) || IsAlphaChar(nChar);
	}
	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines if nChar is alpha character
	// Parameters:
	//     nChar - Character need to test
	// Returns:
	//     TRUE if nChar is alpha character.
	//-----------------------------------------------------------------------
	virtual BOOL IsAlphaChar(TCHAR nChar)
	{
		if (_istalpha(nChar))
			return TRUE;

		if (ConvertUnicodeAlpha(nChar, TRUE) != nChar)
			return TRUE;

		if (ConvertUnicodeAlpha(nChar, FALSE) != nChar)
			return TRUE;

		return FALSE;
	}

	virtual void NotifyPosNotInRange()
	{
		::MessageBeep((UINT)-1);
	}

	virtual void NotifyInvalidCharacter(TCHAR /*nChar*/, TCHAR /*chMask*/)
	{
		::MessageBeep((UINT)-1);
	}

	BOOL IsPromptPos(int nPos) const
	{
		return IsPromptPos(m_strLiteral, nPos);
	}

	BOOL IsPromptPos(const CString& strLiteral, int nPos) const
	{
		return (nPos >= 0 && nPos < strLiteral.GetLength()) && (strLiteral[nPos] == m_chPrompt);
	}

	void CorrectWindowText()
	{
		int nLiteralLength = m_strLiteral.GetLength();
		int nWindowTextLength = m_strWindowText.GetLength();

		if (nWindowTextLength > nLiteralLength)
		{
			m_strWindowText = m_strWindowText.Left(nLiteralLength);
		}
		else if (nWindowTextLength < nLiteralLength)
		{
			m_strWindowText += m_strLiteral.Mid(nWindowTextLength, nLiteralLength - nWindowTextLength);
		}
	}

	void GetMaskState(BOOL bCorrectSelection = TRUE)
	{
		if (!m_hWnd)
			return;

		ASSERT(m_bUseMask);

		MaskGetSel();
		GetWindowText(m_strWindowText);

		ASSERT(m_strDefault.GetLength() == m_strLiteral.GetLength());
		ASSERT(m_strMask.GetLength() == m_strLiteral.GetLength());

		CorrectWindowText();

		if (bCorrectSelection)
		{
			CorrectPosition(m_nStartChar);
			CorrectPosition(m_nEndChar);

			if (m_nEndChar < m_nStartChar)
				m_nEndChar = m_nStartChar;
		}
	}

	void MaskGetSel() // To allow CEdit and CRichEditCtrl
	{
		if (m_hWnd)
		{
			SendMessage(EM_GETSEL, (WPARAM)&m_nStartChar, (LPARAM)&m_nEndChar);
		}
	}

	void SetMaskState(BOOL bUpdateUndo = TRUE)
	{
		if (!m_hWnd)
			return;

		ASSERT(m_bUseMask);

		CString strWindowText;
		GetWindowText(strWindowText);

		CorrectWindowText();
		HideCaret();

		if (strWindowText != m_strWindowText)
		{
			SetWindowText(m_strWindowText);

			if (bUpdateUndo || m_bUpdateUndo)
				m_strUndoBuffer = strWindowText;

			m_bRedo = FALSE;
			m_bModified = TRUE;
		}

		m_bUpdateUndo = bUpdateUndo;

		SetSel(m_nStartChar, m_nEndChar);
		ShowCaret();
	}


//}}AFX_CODEJOCK_PRIVATE


	//-----------------------------------------------------------------------
	// Summary:
	//     The framework calls this member function when the user selects an item from a menu
	// Parameters:
	//     wParam - The low-order word of wParam identifies the command ID of the menu item, control, or accelerator. The high-order word of wParam specifies the notification message if the message is from a control. If the message is from an accelerator, the high-order word is 1. If the message is from a menu, the high-order word is 0
	//     lParam - Identifies the control that sends the message if the message is from a control. Otherwise, lParam is 0.
	// Returns:
	//     An application returns nonzero if it processes this message; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch (LOWORD(wParam))
		{
			case ID_EDIT_CUT:
				MaskCut();
				return TRUE;

			case ID_EDIT_COPY:
				MaskCopy();
				return TRUE;

			case ID_EDIT_PASTE:
				MaskPaste();
				return TRUE;

			case ID_EDIT_CLEAR:
				MaskClear();
				return TRUE;

			case ID_EDIT_UNDO:
				MaskUndo();
				return TRUE;

			case ID_EDIT_SELECT_ALL:
				MaskSelectAll();
				return TRUE;
		}
		return TBase::OnCommand(wParam, lParam);
	}

protected:

	int         m_nStartChar;       // Current position of the first character in the current selection.
	int         m_nEndChar;         // Current position of the first non-selected character past the end of the current selection.
	BOOL        m_bUseMask;         // TRUE to use the edit mask.
	BOOL        m_bOverType;        // TRUE to over type the text, set with VK_INSERT key press.
	BOOL        m_bRedo;            // TRUE to redo, or FALSE to undo.
	BOOL        m_bModified;        // TRUE if mask edit has been modified.
	TCHAR       m_chPrompt;         // Prompt character used to identify the text entry.
	CString     m_strMask;          // Buffer that holds the actual edit mask value.
	CString     m_strDefault;       // Contains the edit controls default display text.
	CString     m_strUndoBuffer;    // Holds the contents of the undo buffer.
	CString     m_strRedoBuffer;    // Holds the contents of the redo buffer.
	CString     m_strWindowText;    // Buffer that holds the actual edit text.
	CString     m_strLiteral;       // Literal format that restricts where the user can enter text.
	BOOL        m_bUpdateUndo;      // TRUE to update undo
};

//{{AFX_CODEJOCK_PRIVATE
#define ON_MESSAGE_BOOL(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_bv, \
		(AFX_PMSG)(AFX_PMSGW)(BOOL (AFX_MSG_CALL CWnd::*)(void))&memberFxn },

#define ON_MASKEDIT_REFLECT\
	ON_MESSAGE_BOOL(WM_CUT, MaskCut)\
	ON_MESSAGE_BOOL(WM_PASTE, MaskPaste)\
	ON_MESSAGE_BOOL(WM_CLEAR, MaskClear)\
	ON_MESSAGE_BOOL(WM_UNDO, MaskUndo)\
	ON_MESSAGE_BOOL(WM_COPY, MaskCopy)\
	ON_WM_KEYDOWN()\
	ON_WM_CHAR()\
	ON_WM_SETFOCUS

//}}AFX_CODEJOCK_PRIVATE
//////////////////////////////////////////////////////////////////////


#endif // #if !defined(__XTMASKEDITEX_H__)
