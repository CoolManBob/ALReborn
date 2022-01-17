// XTPShortcutManager.cpp : implementation of the CXTPShortcutManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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

#include "stdafx.h"
#include "Common/XTPPropExchange.h"

#include "XTPShortcutManager.h"
#include "XTPCommandBars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPShortcutManagerKeyNameText::CXTPShortcutManagerKeyNameText

CXTPShortcutManager::CKeyNameText::CKeyNameText()
{
	static const struct
	{
		WORD    wKey;       // Virtual Key Code.
		LPCTSTR szKeyName;  // Display Name (i.e "CTRL").
	}
	virtKeys[] =
	{
		{ _T('0'), _T("0") },
		{ _T('1'), _T("1") },
		{ _T('2'), _T("2") },
		{ _T('3'), _T("3") },
		{ _T('4'), _T("4") },
		{ _T('5'), _T("5") },
		{ _T('6'), _T("6") },
		{ _T('7'), _T("7") },
		{ _T('8'), _T("8") },
		{ _T('9'), _T("9") },
		{ _T('A'), _T("A") },
		{ _T('B'), _T("B") },
		{ _T('C'), _T("C") },
		{ _T('D'), _T("D") },
		{ _T('E'), _T("E") },
		{ _T('F'), _T("F") },
		{ _T('G'), _T("G") },
		{ _T('H'), _T("H") },
		{ _T('I'), _T("I") },
		{ _T('J'), _T("J") },
		{ _T('K'), _T("K") },
		{ _T('L'), _T("L") },
		{ _T('M'), _T("M") },
		{ _T('N'), _T("N") },
		{ _T('O'), _T("O") },
		{ _T('P'), _T("P") },
		{ _T('Q'), _T("Q") },
		{ _T('R'), _T("R") },
		{ _T('S'), _T("S") },
		{ _T('T'), _T("T") },
		{ _T('U'), _T("U") },
		{ _T('V'), _T("V") },
		{ _T('W'), _T("W") },
		{ _T('X'), _T("X") },
		{ _T('Y'), _T("Y") },
		{ _T('Z'), _T("Z") },
		{ VK_LBUTTON, _T("Left Button") },
		{ VK_RBUTTON, _T("Right Button") },
		{ VK_CANCEL, _T("Ctrl+Break") },
		{ VK_MBUTTON, _T("Middle Button") },
		{ VK_BACK, _T("Backspace") },
		{ VK_TAB, _T("Tab") },
		{ VK_CLEAR, _T("Clear") },
		{ VK_RETURN, _T("Enter") },
		{ VK_SHIFT, _T("Shift") },
		{ VK_CONTROL, _T("Ctrl") },
		{ VK_MENU, _T("Alt") },
		{ VK_PAUSE, _T("Pause") },
		{ VK_CAPITAL, _T("Caps Lock") },
		{ VK_ESCAPE, _T("Esc") },
		{ VK_SPACE, _T("Space") },
		{ VK_PRIOR, _T("Page Up") },
		{ VK_NEXT, _T("Page Down") },
		{ VK_END, _T("End") },
		{ VK_HOME, _T("Home") },
		{ VK_LEFT, _T("Left Arrow") },
		{ VK_UP, _T("Up Arrow") },
		{ VK_RIGHT, _T("Right Arrow") },
		{ VK_DOWN, _T("Down Arrow") },
		{ VK_SELECT, _T("Select") },
		{ VK_PRINT, _T("Print") },
		{ VK_EXECUTE, _T("Execute") },
		{ VK_SNAPSHOT, _T("Snapshot") },
		{ VK_INSERT, _T("Ins") },
		{ VK_DELETE, _T("Del") },
		{ VK_HELP, _T("Help") },
		{ VK_LWIN , _T("Left Windows") },
		{ VK_RWIN, _T("Right Windows") },
		{ VK_APPS, _T("Application") },
		{ VK_MULTIPLY, _T("Num *") },
		{ VK_ADD, _T("Num +") },
		{ VK_SEPARATOR, _T("Separator") },
		{ VK_SUBTRACT, _T("Num -") },
		{ VK_DECIMAL, _T("Num .") },
		{ VK_DIVIDE, _T("Num /") },
		{ VK_F1, _T("F1") },
		{ VK_F2, _T("F2") },
		{ VK_F3, _T("F3") },
		{ VK_F4, _T("F4") },
		{ VK_F5, _T("F5") },
		{ VK_F6, _T("F6") },
		{ VK_F7, _T("F7") },
		{ VK_F8, _T("F8") },
		{ VK_F9, _T("F9") },
		{ VK_F10, _T("F10") },
		{ VK_F11, _T("F11") },
		{ VK_F12, _T("F12") },
		{ VK_NUMPAD0, _T("Num 0") },
		{ VK_NUMPAD1, _T("Num 1") },
		{ VK_NUMPAD2, _T("Num 2") },
		{ VK_NUMPAD3, _T("Num 3") },
		{ VK_NUMPAD4, _T("Num 4") },
		{ VK_NUMPAD5, _T("Num 5") },
		{ VK_NUMPAD6, _T("Num 6") },
		{ VK_NUMPAD7, _T("Num 7") },
		{ VK_NUMPAD8, _T("Num 8") },
		{ VK_NUMPAD9, _T("Num 9") },
		{ VK_NUMLOCK, _T("Num Lock") },
		{ VK_SCROLL, _T("Scrl Lock") },
		{ VK_ATTN, _T("Attn") },
		{ VK_CRSEL, _T("Crsel") },
		{ VK_EXSEL, _T("Exsel") },
		{ VK_EREOF, _T("Ereof") },
		{ VK_PLAY, _T("Play") },
		{ VK_ZOOM, _T("Zoom") },
		{ VK_NONAME, _T("No Name") },
		{ VK_PA1, _T("Pa1") },
		{ VK_OEM_CLEAR, _T("Oem Clear") },
	};

	for (int i = 0; i < _countof(virtKeys); i++)
	{
		SetAt(virtKeys[i].wKey, virtKeys[i].szKeyName);
	}
}

CString CXTPShortcutManager::CKeyNameText::Translate(UINT nKey)
{
	CString strKeyNameText;

	if (m_mapVirtualKeys.Lookup(nKey, strKeyNameText))
	{
		return strKeyNameText;
	}

	return _T("");
}

void CXTPShortcutManager::CKeyNameText::SetAt(UINT uiVirtKey, LPCTSTR strKeyNameText)
{
	m_mapVirtualKeys.SetAt(uiVirtKey, strKeyNameText);
}


#define SAFE_DESTROY_ACCELTABLE(hAccelTable) if (hAccelTable) {::DestroyAcceleratorTable (hAccelTable); hAccelTable = NULL;}

CXTPShortcutManager::CXTPShortcutManager(CXTPCommandBars* pCommandBars)
	: m_pCommandBars(pCommandBars)
{
	m_hAccelTable = NULL;
	m_hOriginalAccelTable = NULL;

	m_bAllowEscapeShortcut = FALSE;
	m_bUseSystemKeyNameText = FALSE;

	m_pKeyNameText = new CKeyNameText();

	m_nDisableShortcuts = 0;

}

CXTPShortcutManager::~CXTPShortcutManager()
{
	SAFE_DESTROY_ACCELTABLE(m_hAccelTable);
	SAFE_DESTROY_ACCELTABLE(m_hOriginalAccelTable);

	delete m_pKeyNameText;

}

void CXTPShortcutManager::SetAccelerators(UINT nIDResource)
{
	SAFE_DESTROY_ACCELTABLE(m_hAccelTable);
	SAFE_DESTROY_ACCELTABLE(m_hOriginalAccelTable);

	LPCTSTR lpszResourceName = MAKEINTRESOURCE(nIDResource);
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_ACCELERATOR);

	if (hInst)
	{
		HACCEL hAccel = ::LoadAccelerators(hInst, lpszResourceName);

		m_hOriginalAccelTable = CopyAccelTable(hAccel);
		m_hAccelTable = CopyAccelTable(hAccel);
	}
}

void CXTPShortcutManager::SetDefaultAccelerator(HACCEL hAccelTable)
{
	SAFE_DESTROY_ACCELTABLE(m_hAccelTable);
	SAFE_DESTROY_ACCELTABLE(m_hOriginalAccelTable);

	m_hAccelTable = hAccelTable;
	m_hOriginalAccelTable = CopyAccelTable(hAccelTable);
}

void CXTPShortcutManager::CreateOriginalAccelTable()
{
	SAFE_DESTROY_ACCELTABLE(m_hOriginalAccelTable);
	m_hOriginalAccelTable = CopyAccelTable(m_hAccelTable);
}

void CXTPShortcutManager::Reset()
{
	SAFE_DESTROY_ACCELTABLE(m_hAccelTable);
	m_hAccelTable = CopyAccelTable(m_hOriginalAccelTable);
}

CString CXTPShortcutManager::Format(LPACCEL lpAccel, int* pPriority)
{
	CString str;
	CKeyHelper helper (lpAccel, this);
	helper.Format (str);

	if (pPriority)
	{
		*pPriority = helper.Priority();
	}

	return str;
}

BOOL CXTPShortcutManager::OnPreviewEditKey(ACCEL hAccel)
{
	if ((hAccel.fVirt & FCONTROL) == 0
		&& (hAccel.fVirt & FSHIFT) == 0
		&& (hAccel.fVirt & FALT) == 0
		&& (hAccel.fVirt & FVIRTKEY)
		&& (hAccel.key == VK_ESCAPE) && !m_bAllowEscapeShortcut)
		return FALSE;

	return TRUE;
}

void CXTPShortcutManager::DisableShortcuts(BOOL bDisable)
{
	m_nDisableShortcuts += bDisable ? +1 : -1;
}

BOOL CXTPShortcutManager::TranslateAccelerator(HWND hWnd, LPMSG lpMsg)
{
	if (m_nDisableShortcuts > 0)
		return FALSE;

	HACCEL hAccel = GetDefaultAccelerator();

	if (hAccel != NULL && ::TranslateAccelerator(hWnd, hAccel, lpMsg))
		return TRUE;

	return FALSE;
}


BOOL CXTPShortcutManager::FindDefaultAccelerator (UINT nCmd, CString& strShortcut)
{
	HACCEL hAccelTable = m_hAccelTable;

	if (m_hAccelTable == NULL && m_hOriginalAccelTable == NULL)
	{
		CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, m_pCommandBars->GetSite());
		hAccelTable = pFrame ? pFrame->m_hAccelTable : NULL;
	}

	if (!hAccelTable)
		return FALSE;

	ASSERT(hAccelTable);

	int nAccelSize = ::CopyAcceleratorTable (hAccelTable, NULL, 0);

	LPACCEL lpAccel = new ACCEL[nAccelSize];
	::CopyAcceleratorTable (hAccelTable, lpAccel, nAccelSize);

	BOOL bFound = FALSE;
	BOOL bEqual = FALSE;
	CString strFirst = _T("");
	int nFirstPriorety = 0;


	for (int i = 0; i < nAccelSize; i++)
	{
		if (lpAccel[i].cmd == nCmd)
		{
			int nPriority = 0;
			CString str = Format(&lpAccel[i], &nPriority);

			if (str == strShortcut)
				bEqual = TRUE;

			if (strFirst.IsEmpty() || (nFirstPriorety < nPriority))
			{
				strFirst = str;
				nFirstPriorety = nPriority;
			}

			bFound = TRUE;
		}
	}
	delete[] lpAccel;

	if (!bFound)
		strShortcut = "";
	else if (!bEqual)
		strShortcut = strFirst;


	return bFound;
}

CXTPShortcutManager::CKeyHelper::CKeyHelper(LPACCEL lpAccel, CXTPShortcutManager* pManager/*= NULL*/) :
	m_pManager(pManager), m_lpAccel (lpAccel)
{
}

CXTPShortcutManager::CKeyHelper::~CKeyHelper()
{
}

int CXTPShortcutManager::CKeyHelper::Priority()
{
	if (m_lpAccel == NULL)
		return 0;
	if (m_lpAccel->fVirt & FCONTROL)
		return 3;
	if (m_lpAccel->fVirt & FALT)
		return 2;
	if (m_lpAccel->fVirt & FSHIFT)
		return 1;
	return 4;
}

void CXTPShortcutManager::CKeyHelper::Format (CString& str) const
{
	str.Empty ();

	if (m_lpAccel == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	if (m_lpAccel->key != VK_CANCEL)
	{
		if (m_lpAccel->fVirt & FCONTROL)
		{
			AddVirtKeyStr (str, VK_CONTROL);
		}

		if (m_lpAccel->fVirt & FSHIFT)
		{
			AddVirtKeyStr (str, VK_SHIFT);
		}

		if (m_lpAccel->fVirt & FALT)
		{
			AddVirtKeyStr (str, VK_MENU);
		}
	}

	if (m_lpAccel->key)
	{
		if (m_lpAccel->fVirt & FVIRTKEY)
		{
			AddVirtKeyStr (str, m_lpAccel->key, TRUE);
		}
		else if (m_lpAccel->key != VK_ESCAPE && m_lpAccel->key != VK_TAB)
		{
			str += (char) m_lpAccel->key;
		}
	}
	else if (str.GetLength() > 0)
	{
		str = str.Left(str.GetLength() - 1);
	}
}

void CXTPShortcutManager::CKeyHelper::AddVirtKeyStr (CString& str, UINT uiVirtKey, BOOL bLast) const
{
	ASSERT(m_pManager);

	CString strKey = m_pManager->GetKeyNameText(uiVirtKey);

	if (!strKey.IsEmpty())
	{
		str += strKey;

		if (!bLast)
		{
			str += '+';
		}
	}
}

CString CXTPShortcutManager::CKeyHelper::GetLocalKeyNameText(UINT uiVirtKey)
{
	#define BUFFER_LEN 50
	TCHAR szBuffer[BUFFER_LEN + 1];
	ZeroMemory(szBuffer, BUFFER_LEN);

	if (uiVirtKey == VK_CANCEL)
		return _T("");

	UINT nScanCode = ::MapVirtualKeyEx (uiVirtKey, 0,
		::GetKeyboardLayout (0)) << 16 | 0x1;

	if (uiVirtKey >= VK_PRIOR && uiVirtKey <= VK_HELP)
	{
		nScanCode |= 0x01000000;
	}

	::GetKeyNameText (nScanCode, szBuffer, BUFFER_LEN);

	CString strKey = szBuffer;

	if (!strKey.IsEmpty())
	{
		strKey.MakeLower();

		for (int i = 0; i < strKey.GetLength(); i++)
		{
			TCHAR c = strKey[i];
			if (IsCharLower(c))
			{
				strKey.SetAt (i, CXTPShortcutManager::ToUpper(c));
				break;
			}
		}
	}

	return strKey;
}

BOOL CXTPShortcutManager::CompareAccelKey(TCHAR chAccel, UINT wParam)
{
	TCHAR tchVirtualKey = (TCHAR)MapVirtualKey(wParam, 2);
	if (tchVirtualKey == NULL)
		return FALSE;

	TCHAR chAccelUpper = ToUpper(chAccel);

	if ((chAccel == (TCHAR)wParam) || (chAccelUpper == (TCHAR)wParam))
		return TRUE;

	UINT nLayoutCount = GetKeyboardLayoutList(0, 0);
	if (nLayoutCount > 1)
	{
		HKL* pLayoutList = (HKL*)_alloca(sizeof(HKL) * nLayoutCount);
		GetKeyboardLayoutList(nLayoutCount, pLayoutList);

		BYTE keyState[256];
		if (!GetKeyboardState (keyState))
			return FALSE;

		for (UINT i = 0; i < nLayoutCount; i++)
		{
			WORD chKey = 0;

#ifdef _UNICODE
			if (ToUnicodeEx((UINT)wParam, 0, keyState, (LPWSTR)&chKey, 1, 0, pLayoutList[i]) == 1)
#else
			if (ToAsciiEx((UINT)wParam, 0, keyState, &chKey, 0, pLayoutList[i]) == 1)
#endif
			{
				if ((chAccel == (TCHAR)chKey) || (chAccelUpper == ToUpper((TCHAR)chKey)))
					return TRUE;
			}

			BYTE ksShift = keyState[VK_SHIFT];
			keyState[VK_SHIFT]= 0x81;

#ifdef _UNICODE
			if (ToUnicodeEx((UINT)wParam, 0, keyState, (LPWSTR)&chKey, 1, 0, pLayoutList[i]) == 1)
#else
			if (ToAsciiEx((UINT)wParam, 0, keyState, &chKey, 0, pLayoutList[i]) == 1)
#endif
			{
				if ((chAccel == (TCHAR)chKey) || (chAccelUpper == ToUpper((TCHAR)chKey)))
					return TRUE;
			}

			keyState[VK_SHIFT] = ksShift;
		}
	}

	return FALSE;
}

void CXTPShortcutManager::SetKeyNameText(UINT uiVirtKey, LPCTSTR strKeyNameText)
{
	m_pKeyNameText->SetAt(uiVirtKey, strKeyNameText);
}

CString CXTPShortcutManager::GetKeyNameText(UINT uiVirtKey)
{
	CString strKey;

	if (m_bUseSystemKeyNameText)
	{
		strKey = CKeyHelper::GetLocalKeyNameText(uiVirtKey);
	}

	if (strKey.IsEmpty())
	{
		strKey = m_pKeyNameText->Translate(uiVirtKey);
	}

	if (strKey.IsEmpty() && !m_bUseSystemKeyNameText)
	{
		strKey = CKeyHelper::GetLocalKeyNameText(uiVirtKey);
	}

	return strKey;
}

TCHAR CXTPShortcutManager::ToUpper(TCHAR vkTCHAR)
{
	TCHAR szChar[2] = {vkTCHAR, _T('\0') };

	CharUpper(szChar);
	return szChar[0];
}


/////////////////////////////////////////////////////////////////////////////
// CKeyAssign

CXTPShortcutManager::CKeyAssign::CKeyAssign(CXTPShortcutManager* pManager/*= NULL*/)
	: m_keyHelper (&m_hAccel, pManager)
{
	m_bKeyDefined = FALSE;
	m_bExtendedOnly = FALSE;
	ResetKey ();
}

CXTPShortcutManager::CKeyAssign::~CKeyAssign()
{
}

BOOL CXTPShortcutManager::CKeyAssign::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_MBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN)
	{
		SetFocus ();
		return TRUE;
	}

	BOOL bPressed = pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN;

	if (bPressed || pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
	{
		BOOL bControlPressed = ::GetKeyState(VK_CONTROL) & 0x8000;
		BOOL bAltPressed = ::GetKeyState(VK_MENU) & 0x8000;

		if (!bControlPressed && !bAltPressed && (pMsg->wParam == VK_TAB))
		{
			if (!m_bKeyDefined)
				ResetKey();

			return FALSE;
		}

		if (bPressed && !((1 << 30) & pMsg->lParam))
		{
			ResetKey();
		}

		if (m_bExtendedOnly)
		{
			if (bPressed)
			{
				SetAccelFlag (FSHIFT, GetKeyState(VK_SHIFT) < 0);
				SetAccelFlag (FCONTROL, GetKeyState(VK_CONTROL) < 0);
				SetAccelFlag (FALT, GetKeyState(VK_MENU) < 0);
			}
		}
		else if (!m_bKeyDefined)
		{
			// read in the actual state because we were not tracking releases if there was a
			// definition
			SetAccelFlag (FSHIFT, ::GetKeyState(VK_SHIFT) & 0x8000);
			SetAccelFlag (FCONTROL, bControlPressed);
			SetAccelFlag (FALT, bAltPressed);

			if (pMsg->wParam == VK_SHIFT
					|| pMsg->wParam == VK_CONTROL
					|| pMsg->wParam == VK_MENU
					)
			{
				// all work is already done
			}
			else if (bPressed)
			{
				m_hAccel.key = (WORD)pMsg->wParam;
				m_bKeyDefined = TRUE;
				SetAccelFlag (FVIRTKEY, TRUE);
			}
		}

		if (m_keyHelper.GetShortcutManager() && !m_keyHelper.GetShortcutManager()->OnPreviewEditKey(m_hAccel))
		{
			ResetKey();
			return TRUE;
		}

		CString str;
		m_keyHelper.Format (str);

		SetWindowText (str);
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

void CXTPShortcutManager::CKeyAssign::ResetKey ()
{
	memset (&m_hAccel, 0, sizeof(ACCEL));
	m_bKeyDefined = FALSE;

	if (m_hWnd != NULL)
	{
		SetWindowText (_T(""));
	}
}

void CXTPShortcutManager::CKeyAssign::SetAccelFlag (BYTE bFlag, BOOL bSet)
{
	if (bSet) m_hAccel.fVirt |= bFlag;
	else m_hAccel.fVirt &= ~bFlag;
}

BOOL CXTPShortcutManager::CKeyAssign::IsKeyDefined () const
{
	return m_bKeyDefined;
}

ACCEL* CXTPShortcutManager::CKeyAssign::GetAccel ()
{
	return &m_hAccel;
}

void CXTPShortcutManager::CKeyAssign::SetAccel(ACCEL& hAccel)
{
	m_hAccel = hAccel;

	if (m_hWnd != NULL)
	{
		CString str;
		m_keyHelper.Format (str);

		SetWindowText (str);
	}
}

#define REG_ENTRY_DATA  _T("Accelerators")

void CXTPShortcutManager::UpdateAcellTable(LPACCEL lpAccel, int nSize)
{
	HACCEL hAccelNew = ::CreateAcceleratorTable(lpAccel, nSize);

	SAFE_DESTROY_ACCELTABLE(m_hAccelTable);

	m_hAccelTable = hAccelNew;
}


BOOL CXTPShortcutManager::GetShortcut(int ID, ACCEL* pAccel)
{
	HACCEL hAccelTable = GetDefaultAccelerator();

	if (!hAccelTable)
		return FALSE;

	BOOL bResult = FALSE;

	int nAccelSize = ::CopyAcceleratorTable (hAccelTable, NULL, 0);

	LPACCEL lpAccel = new ACCEL[nAccelSize];
	::CopyAcceleratorTable (hAccelTable, lpAccel, nAccelSize);

	for (int i = 0; i < nAccelSize; i++)
	{
		if (lpAccel[i].cmd == ID)
		{
			*pAccel = lpAccel[i];
			bResult = TRUE;
		}
	}
	delete[] lpAccel;

	if ((pAccel->fVirt % 2) == 1)
		pAccel->fVirt -= 1;

	return bResult;
}

void CXTPShortcutManager::AddShortcut(long fVirt, long key, long cmd)
{
	HACCEL hAccelTable = m_hAccelTable;

	ACCEL accel;
	accel.fVirt = (BYTE)(fVirt | FVIRTKEY);
	accel.key = (WORD)key;
	accel.cmd = (WORD)cmd;

	int nAccelSize = ::CopyAcceleratorTable (hAccelTable, NULL, 0);

	LPACCEL lpAccel = new ACCEL[nAccelSize + 1];
	::CopyAcceleratorTable (hAccelTable, lpAccel, nAccelSize);

	int nIndex = nAccelSize;

	for (int i = 0; i < nAccelSize; i++)
	{
		if (CKeyHelper::EqualAccels(&lpAccel[i], &accel))
		{
			nIndex = i;
			break;
		}
	}

	lpAccel[nIndex] = accel;

	UpdateAcellTable(lpAccel, nAccelSize + (nIndex == nAccelSize ? 1 : 0));

	delete[] lpAccel;
}

HACCEL CXTPShortcutManager::CopyAccelTable(HACCEL hAccelTable)
{
	if (hAccelTable == NULL)
		return NULL;

	int nAccelSize = ::CopyAcceleratorTable (hAccelTable, NULL, 0);
	if (nAccelSize == 0)
		return NULL;

	LPACCEL lpAccel = new ACCEL[nAccelSize];
	::CopyAcceleratorTable (hAccelTable, lpAccel, nAccelSize);

	HACCEL hAccelNew = ::CreateAcceleratorTable(lpAccel, nAccelSize);
	delete[] lpAccel;

	return hAccelNew;
}


void CXTPShortcutManager::SerializeShortcuts(CArchive& ar)
{
	CXTPPropExchangeArchive px(ar);
	DoPropExchange(&px);
}

void CXTPShortcutManager::SaveShortcuts(LPCTSTR lpszProfileName)
{
	if (!m_hOriginalAccelTable)
		return;

	HACCEL hAccelTable = m_hAccelTable;

	int nAccelSize = ::CopyAcceleratorTable (hAccelTable, NULL, 0);

	LPACCEL lpAccel = new ACCEL[nAccelSize];
	ASSERT (lpAccel != NULL);

	::CopyAcceleratorTable (hAccelTable, lpAccel, nAccelSize);

	AfxGetApp()->WriteProfileBinary(lpszProfileName, REG_ENTRY_DATA, (LPBYTE) lpAccel, nAccelSize * sizeof(ACCEL));

	delete[] lpAccel;

}

void CXTPShortcutManager::LoadShortcuts(LPCTSTR lpszProfileName)
{
	if (!m_hOriginalAccelTable)
		return;

	UINT uiSize;
	LPACCEL lpAccel = 0;
	if (AfxGetApp()->GetProfileBinary(lpszProfileName, REG_ENTRY_DATA, (LPBYTE*) &lpAccel, &uiSize))
	{
		int nAccelSize = uiSize / sizeof(ACCEL);
		ASSERT (lpAccel != NULL);

		UpdateAcellTable(lpAccel, nAccelSize);

		delete[] lpAccel;
	}
}

void CXTPShortcutManager::DoPropExchange(CXTPPropExchange* pPX)
{
	if (pPX->IsStoring())
	{
		int nAccelSize = ::CopyAcceleratorTable (m_hAccelTable, NULL, 0);

		LPACCEL lpAccel = new ACCEL[nAccelSize];
		::CopyAcceleratorTable (m_hAccelTable, lpAccel, nAccelSize);

		if (pPX->IsAllowBlobValues())
		{
			pPX->WriteCount(nAccelSize);
			pPX->Write(_T("Data"), lpAccel, nAccelSize* sizeof(ACCEL));
		}
		else
		{
			CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Accel")));
			POSITION posEnum = pEnumerator->GetPosition(nAccelSize);

			for (int i = 0; i < nAccelSize; i++)
			{
				CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));
				PX_UShort(&secItem, _T("Id"), (USHORT&)(lpAccel[i].cmd), 0);
				PX_Byte(&secItem, _T("virt"), (lpAccel[i].fVirt), 0);
				PX_UShort(&secItem, _T("key"), (USHORT&)(lpAccel[i].key), 0);
			}
		}

		delete[] lpAccel;
	}
	else
	{
		if (pPX->IsAllowBlobValues())
		{
			int nAccelSize = (int)pPX->ReadCount();
			LPACCEL lpAccel = new ACCEL[nAccelSize];
			pPX->Read(_T("Data"), lpAccel, nAccelSize* sizeof(ACCEL));

			UpdateAcellTable(lpAccel, nAccelSize);
			delete[] lpAccel;
		}
		else
		{
			CArray<ACCEL, ACCEL&> accels;
			CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Accel")));
			POSITION posEnum = pEnumerator->GetPosition(0);
			while (posEnum)
			{
				ACCEL accel;
				CXTPPropExchangeSection secItem(pEnumerator->GetNext(posEnum));
				PX_UShort(&secItem, _T("Id"), (USHORT&)(accel.cmd), 0);
				PX_Byte(&secItem, _T("virt"), (accel.fVirt), 0);
				PX_UShort(&secItem, _T("key"), (USHORT&)(accel.key), 0);
				accels.Add(accel);
			}
			UpdateAcellTable(accels.GetData(), (int)accels.GetSize());
		}

	}
}

BOOL CXTPShortcutManager::OnBeforeAdd(LPACCEL pAccel)
{

	UNREFERENCED_PARAMETER(pAccel);
	return FALSE;
}

BOOL CXTPShortcutManager::OnBeforeRemove(LPACCEL pAccel)
{

	UNREFERENCED_PARAMETER(pAccel);
	return FALSE;
}

