// XTPSyntaxEditBufferManager.cpp
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

#include "stdafx.h"
#include "Resource.h"

// common includes
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"

// syntax editor includes
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditUndoManager.h"
#include "XTPSyntaxEditLineMarksManager.h"
#include "XTPSyntaxEditLexPtrs.h"
#include "XTPSyntaxEditLexClassSubObjT.h"
#include "XTPSyntaxEditTextIterator.h"
#include "XTPSyntaxEditSectionManager.h"
#include "XTPSyntaxEditLexCfgFileReader.h"
#include "XTPSyntaxEditLexClassSubObjDef.h"
#include "XTPSyntaxEditLexClass.h"
#include "XTPSyntaxEditLexParser.h"
#include "XTPSyntaxEditLexColorFileReader.h"
#include "XTPSyntaxEditBufferManager.h"
#include "XTPSyntaxEditToolTipCtrl.h"
#include "XTPSyntaxEditAutoCompleteWnd.h"
#include "XTPSyntaxEditCtrl.h"

#include <malloc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CXTPSyntaxEditBufferManager, CCmdTarget)

//===========================================================================
// CRLF styles
static const char *g_pcszCRLFStyles[] =
{
	"\x0D\x0A",         //  DOS/Windows style
	"\x0A\x0D",         //  UNIX style
	"\x0A"              //  Macintosh style
};

static const WCHAR *g_pcszCRLFStylesW[] =
{
	L"\x0D\x0A",        //  DOS/Windows style
	L"\x0A\x0D",        //  UNIX style
	L"\x0A"             //  Macintosh style
};

//===========================================================================
// CXTPSyntaxEditBufferManager
//===========================================================================
CXTPSyntaxEditConfigurationManagerPtr CXTPSyntaxEditBufferManager::s_ptrLexConfigurationManager_Default;
LONG CXTPSyntaxEditBufferManager::s_dwLexConfigurationManager_DefaultRefs = 0;

//===========================================================================

template <class _TFileChar>
inline _TFileChar* _T_GetCRLF(int nCRLFLenStyle, _TFileChar c = 0)
{
	UNREFERENCED_PARAMETER(c);

	_TFileChar** ppStyles = NULL;
	if (sizeof(_TFileChar) == 1)
	{
		ppStyles = (_TFileChar**)g_pcszCRLFStyles;
	}
	else
	{
		ASSERT(sizeof(_TFileChar) == 2);
		ppStyles = (_TFileChar**)g_pcszCRLFStylesW;
	}

	if (nCRLFLenStyle < 0 || nCRLFLenStyle >= _countof(g_pcszCRLFStyles))
	{
		ASSERT(FALSE);
		nCRLFLenStyle = 0;
	}

	return ppStyles[nCRLFLenStyle];
}

template <class _TFileChar>
inline int _T_StrLen(_TFileChar* pStr)
{
	if (sizeof(_TFileChar) == 1)
	{
		return (int)strlen((CHAR*)pStr);
	}
	ASSERT(sizeof(_TFileChar) == 2);
	return (int)wcslen((WCHAR*)pStr);
}

CString GetFileExtention(CFile* pFile)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFileName[_MAX_FNAME], szExt[_MAX_EXT];

	szDrive[0]    = '\0';
	szDir[0]      = '\0';
	szFileName[0] = '\0';
	szExt[0]      = '\0';

	SPLITPATH_S(pFile->GetFileName(), szDrive, szDir, szFileName, szExt);

	return szExt;
}

//---------------------------------------------------------------------------
int _cdecl _Compare_BYTE(const void* pb1, const void* pb2)
{
	if ( *((BYTE*)pb1) > *((BYTE*)pb2) )
		return 1;
	if ( *((BYTE*)pb2) > *((BYTE*)pb1) )
		return -1;
	return 0;
}

BOOL _IsUnicodeFile_heuristic(CFile *pFile)
{
	pFile->SeekToBegin();

	const int cnBufferSize = 1024;
	BYTE arData[cnBufferSize];

	BYTE arData0[cnBufferSize/2];
	BYTE arData1[cnBufferSize/2];

	UINT uDataSize = pFile->Read(arData, cnBufferSize);

	pFile->SeekToBegin();

	UINT uDataXcount = uDataSize / 2;

	UINT i;
	for (i = 0; i < uDataXcount; i++)
	{
		arData0[i] = arData[i*2];
		arData1[i] = arData[i*2 + 1];

		if ((arData0[i] == 0xA || arData0[i] == 0xD) && arData1[i] == 0)
			return TRUE;
	}

	qsort(arData0, uDataXcount, sizeof(BYTE), _Compare_BYTE);
	qsort(arData1, uDataXcount, sizeof(BYTE), _Compare_BYTE);


	int nVariation0 = 0;
	int nVariation1 = 0;
	int nSmallNumbers1 = 0;

	for (i = 1; i < uDataXcount; i++)
	{
		if (arData0[i] != arData0[i - 1])
			nVariation0++;
	}

	for (i = 1; i < uDataXcount; i++)
	{
		if (arData1[i] != arData1[i - 1])
			nVariation1++;

		if (arData1[i] < 9)
			nSmallNumbers1++;
	}

	if (uDataXcount < 10)
		return nVariation0 > nVariation1 || nSmallNumbers1;

	int nVariationDiff = nVariation0 * 100 / max(1, nVariation1);
	int nSmallNumbersPc = nSmallNumbers1 * 100 / max(1, uDataXcount);

	return nVariationDiff > 150 || nSmallNumbersPc > 3;
}

BOOL IsUnicodeFile(CFile *pFile)
{
	pFile->SeekToBegin();

	WORD wPrefix;
	UINT uReaded = pFile->Read(&wPrefix, 2);
	if (uReaded == 2 && wPrefix == 0xFEFF)
	{
		return TRUE;
	}

	DWORD_PTR dwFileSize = (DWORD_PTR)pFile->GetLength();

	if (dwFileSize % 2 == 0)
	{
		return _IsUnicodeFile_heuristic(pFile);
	}

	pFile->SeekToBegin();
	return FALSE;
}

//===========================================================================
template <class _TFileChar>
inline int _T_GetCRLFStyle(_TFileChar chEOL1, _TFileChar chEOL2, int* pnCRLFLen)
{
	int nCRLFLen = 1;
	int nCRLFStyle = xtpEditCRLFStyleUnknown;

	if (chEOL1 == 13)
	{
		//ASSERT(chEOL2 == 10); // incorrect text - reversed DOS CRLF style
		nCRLFLen = chEOL2 == 10 ? 2 : 1;
		nCRLFStyle = xtpEditCRLFStyleDos;
	}
	else if (chEOL1 == 10)
	{
		if (chEOL2 == 13)
		{
			nCRLFLen = 2;
			nCRLFStyle = xtpEditCRLFStyleUnix;
		}
		else
		{
			nCRLFLen = 1;
			nCRLFStyle = xtpEditCRLFStyleMac;
		}
	}

	if (pnCRLFLen)
	{
		*pnCRLFLen = nCRLFLen;
	}
	return nCRLFStyle;
}

template <class _TFileChar, int _cnMaxStr2_chars>
inline void _T_StrAdd(CString& rStr, _TFileChar* pStr2, UINT uCodePage)
{
	if (sizeof(TCHAR) == 2 && sizeof(_TFileChar) == 1)
	{
		const int cnBuf2Size = _cnMaxStr2_chars+1;

		WCHAR szBuf2[cnBuf2Size];
		::ZeroMemory(szBuf2, sizeof(szBuf2));

		int nLen = MultiByteToWideChar(uCodePage, 0, (LPCSTR)pStr2, -1, szBuf2, cnBuf2Size-1);
		ASSERT(nLen <= cnBuf2Size-1);

		rStr += szBuf2;
	}
	else
	{
		//ASSERT(sizeof(TCHAR) == sizeof(_TFileChar));
		UNREFERENCED_PARAMETER(uCodePage);

		rStr += pStr2;
	}
}

//===========================================================================
template <class _TFileChar>
BOOL _T_ReadStringFromFile(CFile *pFile, CString& rstrString, int& rnCRLFStyle,
							UINT uCodePage, _TFileChar c = 0)
{
	UNREFERENCED_PARAMETER(c);
	ASSERT(pFile);

	rstrString = _T("");
	rnCRLFStyle = -1;

	const int cnBuffSize = 128; //512;
	_TFileChar pBuff[cnBuffSize+10];

	UINT uFileSize = (UINT)pFile->GetLength();

	while ((UINT)pFile->GetPosition() < uFileSize)
	{
		UINT uReadSizeB = (cnBuffSize) * sizeof(_TFileChar);
		UINT uBufDataCountB = pFile->Read(pBuff, uReadSizeB);
		if (!uBufDataCountB)
		{
			ASSERT(FALSE);
			break;
		}
		ASSERT(uBufDataCountB%sizeof(_TFileChar) == 0);
		int nBufDataLen = uBufDataCountB/sizeof(_TFileChar);

		pBuff[nBufDataLen] = _T('\0');
		int nBufStrLen = _T_StrLen(pBuff);
		ASSERT(nBufStrLen <= nBufDataLen);

		_TFileChar* pEOL = NULL;

		if (sizeof(_TFileChar) == 1)
		{
			#ifdef _MBCS
				pEOL = (_TFileChar*)_mbspbrk((BYTE*)pBuff, (BYTE*)"\x0A\x0D");
			#else
				pEOL = (_TFileChar*)strpbrk((char*)pBuff, "\x0A\x0D");
			#endif
		}
		else
		{
			pEOL = (_TFileChar*)wcspbrk((wchar_t*)pBuff, L"\x0A\x0D");
		}

		BOOL bEOL_badFormat = FALSE;
		// check bad case: when zero char is rathe then readed buffer end
		if (!pEOL && nBufStrLen < nBufDataLen)
		{
			pEOL = pBuff + nBufStrLen;
			bEOL_badFormat = TRUE;
		}

		//---------------------------------
		if (!pEOL)
		{
			_T_StrAdd<_TFileChar, cnBuffSize>(rstrString, pBuff, uCodePage);
			continue;
		}

		_TFileChar chEOL1 = pEOL[0];
		_TFileChar chEOL2 = pEOL[1];

		*pEOL = 0;
		_T_StrAdd<_TFileChar, cnBuffSize>(rstrString, pBuff, uCodePage);

		if (!bEOL_badFormat && chEOL2 == 0 && (chEOL1 == 10 || chEOL1 == 13))
		{   // buffer ended on the middle of CRLF ?
		    // read one more char to be sure in CRLF style.
			if (uReadSizeB == uBufDataCountB && (UINT)pFile->GetPosition() < uFileSize)
			{
				_TFileChar* pBufTail = pBuff + uBufDataCountB/sizeof(_TFileChar);
				UINT uReadedB = pFile->Read(pBufTail, sizeof(_TFileChar));
				pBufTail[1] = 0;
				chEOL2 = pBufTail[0];

				uReadSizeB += uReadedB;
				uBufDataCountB += uReadedB;
			}
		}

		int nCRLFLen = 1;
		if (!bEOL_badFormat)
		{
			rnCRLFStyle = _T_GetCRLFStyle<_TFileChar>(chEOL1, chEOL2, &nCRLFLen);
			ASSERT(rnCRLFStyle != xtpEditCRLFStyleUnknown);
		}

		int nEolPos = int(pEOL - pBuff);
		int nSeekBack = (nEolPos + nCRLFLen)*sizeof(_TFileChar) - uBufDataCountB;
		pFile->Seek(nSeekBack, CFile::current);

		return TRUE;
	}

	return rstrString.GetLength() > 0;
}

//===========================================================================
template <class _TFileChar, class _TFile >
BOOL _T_WriteStringToFile(_TFile *pFile, LPCTSTR pszString, int nCRLFStyle,
					   UINT uCodePage, CByteArray* parBuffer = NULL, _TFileChar c = 0)
{
	UNREFERENCED_PARAMETER(c);

	CByteArray arBufferInt;
	if (!parBuffer)
	{
		parBuffer = &arBufferInt;
	}

	int nStrLen = (int)_tcslen(pszString);

	BYTE* pFileBuffer = NULL;
	int nFileBufferLenB = 0;

	//------------------------------------------------------------------------
	if (sizeof(TCHAR) == sizeof(_TFileChar))
	{
		pFileBuffer = (BYTE*)pszString;
		nFileBufferLenB = nStrLen * sizeof(_TFileChar);
	}
	else
	{
		int nNeedBufSizeB = nStrLen * 2 + 30;
		if (parBuffer->GetSize() < nNeedBufSizeB)
		{
			parBuffer->SetSize(nNeedBufSizeB + 256);
		}

		pFileBuffer = parBuffer->GetData();

		ASSERT(parBuffer->GetSize() >= nNeedBufSizeB);
		ZeroMemory(pFileBuffer, nNeedBufSizeB);

		if (sizeof(TCHAR) == 2 && sizeof(_TFileChar) == 1)
		{
			int nMBCStextLen = WideCharToMultiByte(uCodePage, 0,
								(LPCWSTR)pszString, -1,
								(LPSTR)pFileBuffer, nNeedBufSizeB, NULL, NULL);

			nFileBufferLenB = max(nMBCStextLen - 1, 0);
		}
		else if (sizeof(TCHAR) == 1 && sizeof(_TFileChar) == 2)
		{
			int nWtextLen = MultiByteToWideChar(uCodePage, 0,
							(LPCSTR)pszString, -1,
							(LPWSTR)pFileBuffer, nNeedBufSizeB/2);

			nFileBufferLenB = max(nWtextLen - 1, 0) * 2;
		}
		else
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	//------------------------------------------------------------------------
	if (nFileBufferLenB > 0)
	{
		pFile->Write(pFileBuffer, nFileBufferLenB);
	}

	if (nCRLFStyle >= 0)
	{
		_TFileChar* tszCRLF = _T_GetCRLF<_TFileChar>(nCRLFStyle);
		int nCRLFlenB = _T_StrLen(tszCRLF) * sizeof(_TFileChar);

		pFile->Write(tszCRLF, nCRLFlenB);
	}

	return TRUE;
}

//===========================================================================
AFX_INLINE BOOL ReadStringFromFileEx(CFile *pFile, CString& rstrString, int& rnCRLFStyle,
									 UINT uCodePage, BOOL bUnicode)
{
	if (bUnicode)
	{
		return _T_ReadStringFromFile<WCHAR>(pFile, rstrString, rnCRLFStyle, uCodePage);
	}
	return _T_ReadStringFromFile<CHAR>(pFile, rstrString, rnCRLFStyle, uCodePage);
}

template <class _TFile >
AFX_INLINE BOOL _T_WriteStringToFileEx(_TFile *pFile, LPCTSTR pszString, int nCRLFStyle,
					   UINT uCodePage, CByteArray* parBuffer, BOOL bUnicode)

{
	if (bUnicode)
	{
		return _T_WriteStringToFile<WCHAR>(pFile, pszString, nCRLFStyle, uCodePage, parBuffer);
	}
	return _T_WriteStringToFile<CHAR>(pFile, pszString, nCRLFStyle, uCodePage, parBuffer);
}
//===========================================================================

CXTPSyntaxEditBufferManager::CXTPSyntaxEditBufferManager()
{
	m_pConnect = new CXTPNotifyConnection();

	m_pLexParser = new CXTPSyntaxEditLexParser();

	m_bOverwrite = FALSE;
	m_nCodePage = CP_ACP;
	m_bUnicodeFileFormat = FALSE;

	m_nTabSize = 4;
	m_iCRLFStyle = xtpEditCRLFStyleDos;

	m_nAverageLineLen = XTP_EDIT_AVELINELEN;

	m_bIsParserEnabled = TRUE;

	if (!s_ptrLexConfigurationManager_Default)
	{
		s_ptrLexConfigurationManager_Default = new CXTPSyntaxEditConfigurationManager();
	}
	if (s_ptrLexConfigurationManager_Default)
	{
		SetLexConfigurationManager(s_ptrLexConfigurationManager_Default);
		s_dwLexConfigurationManager_DefaultRefs++;
	}

	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp)
	{
		m_nTabSize = pWinApp->GetProfileInt(
			XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_TABSIZE, m_nTabSize);
	}

}

CXTPSyntaxEditBufferManager::~CXTPSyntaxEditBufferManager()
{
	Close();

	if (s_dwLexConfigurationManager_DefaultRefs)
	{
		s_dwLexConfigurationManager_DefaultRefs--;
		if (s_dwLexConfigurationManager_DefaultRefs == 0)
		{
			s_ptrLexConfigurationManager_Default->Close();
			s_ptrLexConfigurationManager_Default = NULL;
		}
	}

	CMDTARGET_RELEASE(m_pLexParser);
	CMDTARGET_RELEASE(m_pConnect);
}

void CXTPSyntaxEditBufferManager::Close()
{
	m_LexConfigManSinkMT.UnadviseAll();
	m_pLexParser->Close();

	CleanUp();
}

void CXTPSyntaxEditBufferManager::CleanUp()
{
	m_Strings.RemoveAllStrs();

	m_mgrUndoRedo.Clear();

	m_strFileExt = _T("");
}

void CXTPSyntaxEditBufferManager::Load(CFile *pFile, LPCTSTR pcszFileExt)
{
	CleanUp();

	if (!pFile)
	{
		ASSERT(FALSE);
		return;
	}
	m_bUnicodeFileFormat = IsUnicodeFile(pFile);

	// IsUnicodeFile is take care about start text offset
	//pFile->Seek(m_bUnicodeFileFormat ? 2 : 0, CFile::begin);

	CString strString;
	int nCRLFStyle = -1;
	int nCRLFLastLineStyle = -1;
	BOOL bSETeol = TRUE;

	int nRow = 1;

	while (ReadStringFromFileEx(pFile, strString, nCRLFStyle, m_nCodePage, m_bUnicodeFileFormat))
	{
		m_Strings.SetAtGrowStr(nRow, strString);

		if (bSETeol && nCRLFStyle >= 0)
		{
			SetCRLFStyle(nCRLFStyle);
			bSETeol = FALSE;
		}
		nCRLFLastLineStyle = nCRLFStyle;
		nRow++;
	}

	if (nCRLFLastLineStyle >= 0)
	{
		m_Strings.SetAtGrowStr(nRow, _T(""));
	}

	CString strExt = pcszFileExt ? pcszFileExt : GetFileExtention(pFile);
	SetFileExt(strExt);
}

void CXTPSyntaxEditBufferManager::Serialize(CArchive &ar)
{
	if (m_bIsParserEnabled)
	{
		m_pLexParser->StopParseInThread();
	}

	CFile* pFile = ar.GetFile();

	if (ar.IsLoading())
	{
		Load(pFile);
	}
	else if (ar.IsStoring())
	{
		if (m_bUnicodeFileFormat)
		{
			ar << (BYTE)255;
			ar << (BYTE)254;
		}

		CByteArray arBuffer;

		int nCRLFStyle = GetCurCRLFType();

		int nRowsCount = m_Strings.GetCount();
		for (int i = 1; i < nRowsCount; i++)
		{
			CString strLine = m_Strings.GetStr(i);

			if (i == nRowsCount-1)
			{ // last line
				nCRLFStyle = -1;// do not write CRLF
			}

			_T_WriteStringToFileEx(&ar, strLine, nCRLFStyle, m_nCodePage,
									&arBuffer, m_bUnicodeFileFormat);
		}

		m_mgrUndoRedo.MarkSaved();

		//---------------
		CString strExt = GetFileExtention(pFile);
		SetFileExt(strExt);
	}
}

void CXTPSyntaxEditBufferManager::SetLexConfigurationManager(
	CXTPSyntaxEditConfigurationManager* pMan)
{
	m_LexConfigManSinkMT.UnadviseAll();

	m_ptrLexConfigurationManager.SetPtr(pMan, TRUE);

	if (pMan)
	{
		CXTPNotifyConnection* ptrConn = pMan->GetConnection();
		ASSERT(ptrConn);
		if (ptrConn)
		{
			m_LexConfigManSinkMT.Advise(ptrConn, xtpEditClassSchWasChanged, &CXTPSyntaxEditBufferManager::OnLexConfigManEventHandler);
			m_LexConfigManSinkMT.Advise(ptrConn, xtpEditThemeWasChanged, &CXTPSyntaxEditBufferManager::OnLexConfigManEventHandler);
			m_LexConfigManSinkMT.Advise(ptrConn, xtpEditAllConfigWasChanged, &CXTPSyntaxEditBufferManager::OnLexConfigManEventHandler);
		}
	}
}

void CXTPSyntaxEditBufferManager::OnLexConfigManEventHandler(XTP_NOTIFY_CODE Event,
															  WPARAM wParam, LPARAM lParam)
{
	if (!m_bIsParserEnabled)
	{
		return;
	}

	if (Event == xtpEditClassSchWasChanged || Event == xtpEditAllConfigWasChanged)
	{
		CXTPSyntaxEditTextSchemaPtr ptrNewTxtSch;
		BOOL bSetTxtSch = FALSE;

		if (Event == xtpEditClassSchWasChanged)
		{
			// wParam = LPCTSTR - Schema name;
			// lParam = CXTPSyntaxEditLexTextSchema* New text schema pointer
			//            or NULL if schema was removed.

			CXTPSyntaxEditLexTextSchema* pTxtSch = (CXTPSyntaxEditLexTextSchema*)lParam;
			if (pTxtSch)
			{
				if (pTxtSch->IsFileExtSupported(m_strFileExt) )
				{
					ptrNewTxtSch.SetPtr(pTxtSch, TRUE);
					bSetTxtSch = TRUE;
				}
				else
				{
					CXTPSyntaxEditLexTextSchema* ptrTxtSch_current = m_pLexParser->GetTextSchema();
					CString strName_current = ptrTxtSch_current ? ptrTxtSch_current->GetSchName() : _T("");
					CString strName_New = pTxtSch->GetSchName();

					if (strName_current.CompareNoCase(strName_New) == 0)
					{
						//such file ext is no longer supported by this schema
						ptrNewTxtSch = NULL;
						bSetTxtSch = TRUE;
					}
				}
			}
			else
			{
				ptrNewTxtSch = GetMasterTextSchema(m_strFileExt);
				bSetTxtSch = !ptrNewTxtSch;
			}
		}

		if (Event == xtpEditAllConfigWasChanged)
		{
			ptrNewTxtSch = GetMasterTextSchema(m_strFileExt);
			bSetTxtSch = TRUE;
		}

		//====================================================================
		if (bSetTxtSch)
		{
			m_pLexParser->SetTextSchema(ptrNewTxtSch);

			if (ptrNewTxtSch)
			{
				XTP_EDIT_LINECOL pos1_0 = {1,0};
				BOOL bParseInThread = m_pLexParser->GetSchemaOptions(
							GetFileExt())->m_bConfigChangedReparceInSeparateThread;
				if (bParseInThread)
				{

					m_pLexParser->StartParseInThread(this, &pos1_0, NULL, 0, TRUE);
				}
				else
				{
					CXTPSyntaxEditLexTextSchema* ptrTextSch = m_pLexParser->GetTextSchema();
					if (ptrTextSch)
					{
						CXTPSyntaxEditTextIterator txtIter(this);
						ptrTextSch->RunParseUpdate(TRUE, &txtIter, &pos1_0, NULL);
					}
				}
			}

			//----------------------------------------------
			m_pConnect->SendEvent(Event, wParam, lParam);
		}
	}

	if (Event == xtpEditThemeWasChanged)
	{
		// wParam = LPCTSTR - Theme name;
		// lParam = CXTPSyntaxEditColorTheme* New theme pointer
		//            or NULL if theme was removed.

		CXTPSyntaxEditColorTheme* pNewTheme = (CXTPSyntaxEditColorTheme*)lParam;

		CXTPSyntaxEditLexTextSchema* ptrTxtSch = m_pLexParser->GetTextSchema();
		if (ptrTxtSch)
		{
			ptrTxtSch->ApplyTheme(pNewTheme);

			//----------------------------------------------
			m_pConnect->SendEvent(Event, wParam, lParam);
		}
	}
}

BOOL CXTPSyntaxEditBufferManager::InsertText(LPCTSTR szText, int nRow, int nCol,
											BOOL bCanUndo, XTP_EDIT_LINECOL* pFinalLC)
{
	int nStrLenB = (int)_tcslen(szText) * sizeof(TCHAR);
	CMemFile memFile((BYTE*)szText, nStrLenB);
	CString strString;
	CString strReplased;
	int nLastLineEOLstyle = -1;

	CString strRow1, strRow2;
	memFile.SeekToBegin();

	// process first row (cut and concatenate)
	if (_T_ReadStringFromFile<TCHAR>(&memFile, strString, nLastLineEOLstyle, m_nCodePage))
	{
		strRow1 = m_Strings.GetStr(nRow);
		int nLen = strRow1.GetLength();
		if (nCol-1 < nLen)
		{
			strRow2 = strRow1.Mid(nCol-1, nLen-nCol+1);
			strRow1 = strRow1.Left(nCol-1);

			strRow1 += strString;
			if (m_bOverwrite)
			{
				int nReplaceLen = min(strString.GetLength(), strRow2.GetLength());
				strReplased = strRow2.Left(nReplaceLen);
				DELETE_S(strRow2, 0, nReplaceLen);
			}
			m_Strings.SetAtGrowStr(nRow, strRow1);
		}
		else
		{
			m_Strings.InsertText(nRow, nCol-1, strString, TRUE);
		}
	}

	// process middle rows (insert them)
	int nCRLFStyleX = -1;
	int r = nRow + 1;
	while (_T_ReadStringFromFile<TCHAR>(&memFile, strString, nCRLFStyleX, m_nCodePage))
	{
		m_Strings.InsertStr(r, strString);
		r++;

		nLastLineEOLstyle = nCRLFStyleX;
	}

	// process last row (concatenate)
	int nRowTo = nRow;
	int nColTo = nCol+1;

	if (nLastLineEOLstyle >= 0)
	{
		m_Strings.InsertStr(r, strRow2);
		nRowTo = r;
		nColTo = strString.GetLength()+1;
	}
	else
	{
		strString = m_Strings.GetStr(r-1);

		nRowTo = r-1;
		nColTo = strString.GetLength()+1;

		strString += strRow2;
		m_Strings.SetAtGrowStr(r-1, strString);

	}

	//===========================================================================
	if (pFinalLC)
	{
		pFinalLC->nLine = nRowTo;
		pFinalLC->nCol = nColTo;
	}

	if (bCanUndo)
	{
		if (!m_bOverwrite || strReplased == "") // By Leva: added 2nd condition to eliminate undo bug
		{
			m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditInsertStringCommand(this,
								 szText,
								 XTP_EDIT_LINECOL::MakeLineCol(nRow, nCol),
								 XTP_EDIT_LINECOL::MakeLineCol(nRowTo, nColTo)) );
		}
		else
		{
			m_mgrUndoRedo.SetGroupInsertMode(FALSE);
			m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditReplaceStringCommand(this,
				szText, strReplased,
				XTP_EDIT_LINECOL::MakeLineCol(nRow, nCol),
				XTP_EDIT_LINECOL::MakeLineCol(nRowTo, nColTo)) );
		}
	}

	return TRUE;
}

BOOL CXTPSyntaxEditBufferManager::InsertTextBlock(LPCTSTR szText, int nRow,
							int nCol, BOOL bCanUndo, XTP_EDIT_LINECOL* pFinalLC)
{
	int nStrLenB = (int)_tcslen(szText) * sizeof(TCHAR);
	CMemFile memFile((BYTE*)szText, nStrLenB);
	memFile.SeekToBegin();

	CString strString;
	int nEndPos = 0;
	int nCRLFStyleX = -1;
	int r = nRow;

	int nDispCol = StrPosToCol(nRow, nCol-1);

	if (bCanUndo)
	{
		m_mgrUndoRedo.SetGroupInsertMode(TRUE);
	}

	while (_T_ReadStringFromFile<TCHAR>(&memFile, strString, nCRLFStyleX, m_nCodePage))
	{
		CString strRow = m_Strings.GetStr(r);

		int nRowLen = strRow.GetLength();
		int nRowCols = StrPosToCol(r, nRowLen);

		int nPos = ColToStrPos(r, nDispCol);
		if (nRowCols < nDispCol)
		{
			nPos = nRowLen + (nDispCol - nRowCols);
		}

		m_Strings.InsertText(r, nPos, strString, TRUE);

		//--------------------------------------------------------------------
		nEndPos = nPos + strString.GetLength();

		if (bCanUndo)
		{
			XTP_EDIT_LINECOL posFrom = {r, nPos+1};
			XTP_EDIT_LINECOL posTo = {r, nEndPos+1};

			m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditInsertStringCommand(this,
									strString, posFrom, posTo) );
		}

		//--------------------------------------------------------------------
		r++;
	}

	if (bCanUndo)
	{
		m_mgrUndoRedo.SetGroupInsertMode(FALSE);
	}
	//===========================================================================
	if (pFinalLC)
	{
		pFinalLC->nLine = r-1;
		pFinalLC->nCol = nEndPos+1;
	}

	return TRUE;
}

void CXTPSyntaxEditBufferManager::GetLineText(int nLine, CString& strText,
											 BOOL bAddCRLF, int nCRLFStyle)
{
	strText = GetLineText(nLine, bAddCRLF, nCRLFStyle);
}

CString CXTPSyntaxEditBufferManager::GetLineText(int nLine, BOOL bAddCRLF, int nCRLFStyle)
{
	CString strText = m_Strings.GetStr(nLine);
	if (bAddCRLF)
	{
		strText += GetCRLF(nCRLFStyle);
	}
	return strText;
}

int CXTPSyntaxEditBufferManager::GetLineTextLength(int nLine, BOOL bAddCRLF, int nCRLFStyle)
{
	int nLen = m_Strings.GetStrLen(nLine);

	if (bAddCRLF)
	{
		int nEOLlen = (int)_tcsclen(GetCRLF(nCRLFStyle));
		nLen += nEOLlen;
	}
	return nLen;
}

int CXTPSyntaxEditBufferManager::GetMaxLineTextLength() const
{
	return CalcMaxLineTextLength();
}

int CXTPSyntaxEditBufferManager::CalcMaxLineTextLength(int nLineFrom, int nLineTo) const
{
	nLineFrom = min(max(1, nLineFrom), GetRowCount());
	nLineTo = nLineTo < 0 ? GetRowCount() : min(nLineTo, GetRowCount());

	if (nLineFrom >= m_Strings.GetCount() || nLineTo >= m_Strings.GetCount() )
	{
		return 0;
	}

	int nMAXlen = 0;
	for (int i = nLineFrom; i <= nLineTo; i++)
	{
		CString* pCStrData = m_Strings.GetStrDataC(i);
		int nLen = pCStrData ? pCStrData->GetLength() : 0;

		if (nLen > nMAXlen)
		{
			nMAXlen = nLen;
		}
	}

	return nMAXlen;
}

BOOL CXTPSyntaxEditBufferManager::DeleteText(int nRowFrom, int nColFrom,
						int nRowTo, int nColTo, BOOL bCanUndo, BOOL bDispCol)
{
	ASSERT(nColFrom >= 1 && nColTo >= 1);

	if (bCanUndo)
	{
		int nPos1 = bDispCol ? nColFrom : StrPosToCol(nRowFrom, nColFrom - 1); // By Leva: moved -1 into brackets
		int nPos2 = bDispCol ? nColTo : StrPosToCol(nRowTo, nColTo - 1); // By Leva: moved -1 into brackets

		CMemFile file(CalcAveDataSize(nRowFrom, nRowTo));
		if (GetBuffer(nRowFrom, nPos1, nRowTo, nPos2, file))
		{
			TCHAR szNull = NULL;
			file.SeekToEnd();
			file.Write((const BYTE *)&szNull, sizeof(TCHAR));

			BYTE *pBytes = file.Detach();

			m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditDeleteStringCommand(this, (LPCTSTR)pBytes,
				XTP_EDIT_LINECOL::MakeLineCol(nRowFrom, bDispCol ? ColToStrPos(nRowFrom, nColFrom)+1 : nColFrom),
				XTP_EDIT_LINECOL::MakeLineCol(nRowTo, bDispCol ? ColToStrPos(nRowTo, nColTo)+1 : nColTo)) );

			free(pBytes);
		}
	}

	BOOL bResult = FALSE;
	if (nRowFrom < nRowTo)
	{
		// Process nRowFrom
		int nStrLenR1 = m_Strings.GetStrLen(nRowFrom);
		int nStrPos = bDispCol ? ColToStrPos(nRowFrom, nColFrom) : nColFrom-1;
		int nCount = nStrLenR1 - nStrPos;

		if (nStrLenR1 && nCount > 0)
		{
			ASSERT(nStrPos >=0);
			bResult = m_Strings.DeleteText(nRowFrom, nStrPos, nCount);
			ASSERT(bResult);
		}

		// Process nRowTo
		int nStrLenR2 = m_Strings.GetStrLen(nRowTo);
		nStrPos = 0;
		nCount = bDispCol ? ColToStrPos(nRowTo, nColTo) : nColTo-1;

		if (nStrLenR2 && nCount > 0)
		{
			ASSERT(nStrPos >=0);
			bResult = m_Strings.DeleteText(nRowTo, nStrPos, nCount);
			ASSERT(bResult);
		}

		// Concatenate row <from> and row <to>
		CString strRowNew = m_Strings.GetStr(nRowFrom);
		CString strRow2 = m_Strings.GetStr(nRowTo);
		strRowNew += strRow2;
		m_Strings.SetAtGrowStr(nRowFrom, strRowNew);

		for (int r = nRowTo; r > nRowFrom; r--)
		{
			bResult = m_Strings.RemoveStr(r);
			//ASSERT(bResult);
		}
	}
	else
	{
		ASSERT(nRowFrom == nRowTo);

		int nPos1 = bDispCol ? ColToStrPos(nRowFrom, nColFrom) : nColFrom-1;
		int nPos2 = bDispCol ? ColToStrPos(nRowTo, nColTo) : nColTo-1;

		int nCount = abs(nPos2 - nPos1); //abs(nColTo - nColFrom);

		if (nCount)
		{
			bResult = m_Strings.DeleteText(nRowFrom, min(nPos1, nPos2), nCount);
		}
	}

	return bResult;
}

BOOL CXTPSyntaxEditBufferManager::GetBuffer(
			int row1, int col1, int row2, int col2,
			CMemFile& file, BOOL bColumnSelection, BOOL bForceDOSStyleCRLF)
{
	int nTempRow1 = row1, nTempRow2 = row2;
	int nTempCol1 = col1, nTempCol2 = col2;

	if (nTempRow2 < nTempRow1)
	{
		row1 = nTempRow2;
		col1 = nTempCol2;
		row2 = nTempRow1;
		col2 = nTempCol1;
	}
	if (nTempRow1 == nTempRow2 || bColumnSelection)
	{
		col1 = min(nTempCol1, nTempCol2);
		col2 = max(nTempCol1, nTempCol2);
	}

	BOOL bStart = TRUE;
	BOOL bEnd = FALSE;

	int nStartPos = 0, nEndPos = 0;
	int nSizeCopy = 0;

	int nNewCRLFStyle = (bForceDOSStyleCRLF) ? xtpEditCRLFStyleDos : m_iCRLFStyle;
	const CString strCRLF(GetCRLF(nNewCRLFStyle));

	for (int i = row1; i <= row2; i++)
	{
		if (i == row2)
			bEnd = TRUE;

		CString strBuffer;
		GetLineText(i, strBuffer);

		const int nLineLen = (int)_tcslen(strBuffer);

		nStartPos = min(bStart || bColumnSelection ? ColToStrPos(i, col1) : 0, nLineLen);
		nEndPos = min(bEnd || bColumnSelection ? ColToStrPos(i, col2) : nLineLen, nLineLen);

		nSizeCopy = abs(nEndPos - nStartPos) * sizeof(TCHAR);

		if (nSizeCopy > 0)
			file.Write((LPVOID)((LPCTSTR)strBuffer + nStartPos), nSizeCopy);

		if (bColumnSelection)
		{
			// add spaces for the free size of the column block
			int nAddSpaces = abs(col2 - col1) - abs(nEndPos - nStartPos);
			if (nAddSpaces > 0)
			{
				const CString strSpaces(_T(' '), nAddSpaces);
				file.Write((LPVOID)(LPCTSTR)strSpaces, nAddSpaces * sizeof(TCHAR));
			}
		}
		if (row1 != row2 && i != row2 || bColumnSelection)
		{
			// add end of the line
			file.Write((LPVOID)(LPCTSTR)strCRLF, strCRLF.GetLength() * sizeof(TCHAR));
		}

		bStart = FALSE;
	}

	return TRUE;
}

void CXTPSyntaxEditBufferManager::SetOverwriteFlag(BOOL bOverwrite)
{
	m_bOverwrite = bOverwrite;
}

BOOL CXTPSyntaxEditBufferManager::GetOverwriteFlag()
{
	return m_bOverwrite;
}


BOOL CXTPSyntaxEditBufferManager::IsTextCRLF(LPCTSTR szCompText, BOOL bFindReverse)
{
	CString strCRLF(g_pcszCRLFStyles[m_iCRLFStyle]);
	LPCTSTR szCurCRLF = (LPCTSTR)strCRLF;
	LPCTSTR szTextToCompare = szCompText;
	int nCRLFSize = lstrlen(szCurCRLF);

	if (bFindReverse)
		szTextToCompare = (szCompText - (nCRLFSize - 1));

	BOOL bIsCRLF = (_tcsncmp(szTextToCompare, szCurCRLF, nCRLFSize) == 0);

	return bIsCRLF;
}

#ifdef _UNICODE
BOOL CXTPSyntaxEditBufferManager::IsTextCRLF(LPCSTR szCompText, BOOL bFindReverse)
{
	LPCSTR szCurCRLF = (LPCSTR)g_pcszCRLFStyles[m_iCRLFStyle];
	LPCSTR szTextToCompare = szCompText;
	int nCRLFSize = (int)strlen(szCurCRLF);

	if (bFindReverse)
		szTextToCompare = (szCompText - (nCRLFSize - 1));

	BOOL bIsCRLF = (strncmp(szTextToCompare, szCurCRLF, nCRLFSize) == 0);

	return bIsCRLF;
}
#endif

CString CXTPSyntaxEditBufferManager::GetCurCRLF() const
{
	return GetCRLF();
}

LPCTSTR CXTPSyntaxEditBufferManager::GetCRLF(int nCRLFStyle) const
{
	ASSERT(m_iCRLFStyle >= 0 && m_iCRLFStyle < _countof(g_pcszCRLFStyles));

	int nCRLFidx = m_iCRLFStyle;
	if (nCRLFStyle >= 0 && nCRLFStyle < _countof(g_pcszCRLFStyles))
	{
		nCRLFidx = nCRLFStyle;
	}
	return _T_GetCRLF<TCHAR>(nCRLFidx);
}

void CXTPSyntaxEditBufferManager::SetCRLFStyle(int nCRLFStyle)
{

	if (nCRLFStyle >= 0 && nCRLFStyle < _countof(g_pcszCRLFStyles))
	{
		m_iCRLFStyle = nCRLFStyle;
	} else {
		ASSERT(FALSE);
	}
}

int CXTPSyntaxEditBufferManager::GetCurCRLFType()
{
	return m_iCRLFStyle;
}

BOOL CXTPSyntaxEditBufferManager::SetTabSize(int nTabSize, BOOL bUpdateReg/*=FALSE*/)
{
	m_nTabSize = nTabSize;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_TABSIZE, nTabSize))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

UINT CXTPSyntaxEditBufferManager::GetCodePage()
{
	return m_nCodePage;
}

BOOL CXTPSyntaxEditBufferManager::IsModified()
{
	return m_mgrUndoRedo.IsModified();
}

CString CXTPSyntaxEditBufferManager::GetFileExt()
{
	return m_strFileExt;
}

void CXTPSyntaxEditBufferManager::SetFileExt(const CString& strExt)
{
	if (m_strFileExt.CompareNoCase(strExt) == 0)
	{
		return;
	}
	m_strFileExt = strExt;

	if (m_bIsParserEnabled)
	{
		CXTPSyntaxEditTextSchemaPtr ptrMasterTxtSch = GetMasterTextSchema(strExt);
		m_pLexParser->SetTextSchema(ptrMasterTxtSch);
	}
}

CString CXTPSyntaxEditBufferManager::GetConfigFile()
{
	if (!m_ptrLexConfigurationManager)
	{
		return _T("");
	}
	return m_ptrLexConfigurationManager->GetConfigFile();
}

BOOL CXTPSyntaxEditBufferManager::SetConfigFile(LPCTSTR szPath)
{
	if (!m_ptrLexConfigurationManager)
	{
		return FALSE;
	}
	m_ptrLexConfigurationManager->ReloadConfig(szPath);
	return TRUE;
}

CXTPSyntaxEditTextSchemaPtr CXTPSyntaxEditBufferManager::GetMasterTextSchema(const CString& strExt)
{
	if (!m_ptrLexConfigurationManager)
	{
		return NULL;
	}

	CXTPSyntaxEditTextSchemaPtr ptrMasterSch;
	ptrMasterSch = m_ptrLexConfigurationManager->GetTextSchemesManager().FindSchema(strExt);

	return ptrMasterSch;
}

UINT CXTPSyntaxEditBufferManager::CalcAveDataSize(int nRowStart, int nRowEnd)
{
	UINT uSize = m_nAverageLineLen * abs(nRowStart - nRowEnd);
	uSize = (uSize / 1024 + 1) * 1024;
	return uSize;
}

BOOL CXTPSyntaxEditBufferManager::IsParserEnabled()
{
	return m_bIsParserEnabled;
}

void CXTPSyntaxEditBufferManager::EnableParser(BOOL bEnable)
{
	if (m_bIsParserEnabled == bEnable)
	{
		return;
	}
	m_bIsParserEnabled = bEnable;

	if (m_bIsParserEnabled)
	{
		CXTPSyntaxEditTextSchemaPtr ptrMasterTxtSch = GetMasterTextSchema(m_strFileExt);
		m_pLexParser->SetTextSchema(ptrMasterTxtSch);

		XTP_EDIT_LINECOL pos1_0 = {1,0};
		BOOL bParseInThread = m_pLexParser->GetSchemaOptions(
					GetFileExt())->m_bConfigChangedReparceInSeparateThread;
		if (bParseInThread)
		{
			m_pLexParser->StartParseInThread(this, &pos1_0, NULL, 0, TRUE);
		}
		else
		{
			CXTPSyntaxEditLexTextSchema* ptrTextSch = m_pLexParser->GetTextSchema();
			if (ptrTextSch)
			{
				CXTPSyntaxEditTextIterator txtIter(this);
				ptrTextSch->RunParseUpdate(TRUE, &txtIter, &pos1_0, NULL);
			}
		}
	}
	else
	{
		m_pLexParser->SetTextSchema(NULL);
	}
}

int CXTPSyntaxEditBufferManager::ColToStrPos(int nLine, int nDispCol) const
{
	int nStrPos = 0;
	int nCol = 0;

	const CString strText = m_Strings.GetStr(nLine);

	int nChars = (int)_tcsclen(strText);
	LPCTSTR pChar = strText;
	int nTabSize = GetTabSize();

	for (int i = 0; *pChar != 0 && i < nChars && (nCol+1) < nDispCol; i++)
	{
		if (*pChar == _T('\t'))
		{
			nCol += nTabSize - (nCol % nTabSize);
		}
		else
		{
			nCol++;
		}
		LPCTSTR pChar_prev = pChar;
		pChar = _tcsinc(pChar);

		nStrPos += int(pChar - pChar_prev);
	}

	ASSERT(nStrPos <= strText.GetLength() || strText.IsEmpty() && nStrPos == 0);
	return nStrPos;
}

int CXTPSyntaxEditBufferManager::StrPosToCol(int nLine, int nStrPos) const
{
	int nDispCol = 1;

	const CString strText = m_Strings.GetStr(nLine);

	int nChars = (int)_tcsclen(strText);
	nStrPos = min(nStrPos, nChars);

	LPCTSTR pChar = strText;
	int nTabSize = GetTabSize();

	for (int i = 0; *pChar != 0 && i < nStrPos;)
	{
		if (*pChar == _T('\t'))
		{
			nDispCol += nTabSize - ((nDispCol-1) % nTabSize);
		}
		else
		{
			nDispCol++;
		}
		LPCTSTR pChar_prev = pChar;
		pChar = _tcsinc(pChar);

		i += int(pChar - pChar_prev);
	}

	return nDispCol;
}

void CXTPSyntaxEditBufferManager::ChangeCase(int nRow, int nDispFrom, int nDispTo, BOOL bUpper, BOOL bCanUndo)
{
	CString strText(m_Strings.GetStr(nRow));
	int nFrom = ColToStrPos(nRow, nDispFrom);
	int nTo = ColToStrPos(nRow, nDispTo);

	CString strOriginal = strText.Mid(nFrom, nTo - nFrom);
	CString strChanged(strOriginal);

	// upper/lower text at row nRow from nFrom to nTo
	if (bUpper)
		strChanged.MakeUpper();
	else
		strChanged.MakeLower();

	for (int i = nFrom; i < nTo; ++i)
		strText.SetAt(i, strChanged[i-nFrom]);

	m_Strings.SetAtGrowStr(nRow, strText);

	if (bCanUndo)
	{
		m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditReplaceStringCommand(this,
				strChanged, strOriginal,
				XTP_EDIT_LINECOL::MakeLineCol(nRow, nFrom + 1),
				XTP_EDIT_LINECOL::MakeLineCol(nRow, nTo + 1)) );
	}
}

void CXTPSyntaxEditBufferManager::ChangeTabification(int nRow, int nDispFrom, int nDispTo, BOOL bTabify, BOOL bCanUndo)
{
	CString strText(m_Strings.GetStr(nRow));
	const int nTextLen = (int)_tcsclen(strText);
	int nFrom = ColToStrPos(nRow, nDispFrom);
	int nTo = ColToStrPos(nRow, nDispTo);
	const int nTabSize = GetTabSize();
	const CString strTab(_T(' '), nTabSize);

	CString strOriginal = strText.Mid(nFrom, nTo - nFrom);
	CString strChanged(strOriginal);
	// upper/lower text at row nRow from nFrom to nTo
	if (bTabify)
	{
		REPLACE_S(strChanged, strTab, _T("\x09"));
	}
	else
	{
		REPLACE_S(strChanged, _T("\x09"), strTab);
	}

	strText = strText.Left(nFrom) + strChanged + strText.Right(nTextLen - nTo);

	m_Strings.SetAtGrowStr(nRow, strText);

	if (bCanUndo)
	{
		m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditDeleteStringCommand(this,
			strOriginal,
			XTP_EDIT_LINECOL::MakeLineCol(nRow, nFrom + 1),
			XTP_EDIT_LINECOL::MakeLineCol(nRow, nTo + 1)) );
		m_mgrUndoRedo.AddCommand(new CXTPSyntaxEditInsertStringCommand(this,
			strChanged,
			XTP_EDIT_LINECOL::MakeLineCol(nRow, nFrom + 1),
			XTP_EDIT_LINECOL::MakeLineCol(nRow, nFrom + (int)_tcsclen(strChanged) + 1)) );
	}
}

////////////////////////////////////////////////////////////////////////////
//class CXTPSyntaxEditStringsManager
CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::CXTPSyntaxEditStringsManager()
{
}

CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::~CXTPSyntaxEditStringsManager()
{
}

int CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::GetCount() const
{
	return (int)m_arStrings.GetSize();
}

CString CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::GetStr(int nRow) const
{
	CString* pCStr = GetStrDataC(nRow);

	if (pCStr)
	{
		return *pCStr;
	}
	return _T("");
}

int CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::GetStrLen(int nRow) const
{
	CString* pCStr = GetStrDataC(nRow);

	if (!pCStr)
	{
		return 0;
	}

	int nLen = pCStr->GetLength();
	return nLen;
}


void CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::SetAtGrowStr(int nRow, LPCTSTR pcszText)
{
	CString* pCStr = GetStrData(nRow, TRUE);

	if (!pCStr)
	{
		return;
	}

	*pCStr = pcszText;
}

void CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::SetAtGrowStr(int nRow,
														const CString& strText)
{
	CString* pCStr = GetStrData(nRow, TRUE);

	if (!pCStr)
	{
		return;
	}

	*pCStr = strText;
}

void CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::InsertStr(int nRow,
													  const CString& strText)
{
	if (nRow >=0 && nRow < m_arStrings.GetSize() )
	{
		CString* pCStr = new CString(strText);
		m_arStrings.InsertAt(nRow, pCStr);
	}
	else
	{
		SetAtGrowStr(nRow, strText);
	}
}

BOOL CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::RemoveStr(int nRow)
{
	if (nRow >=0 && nRow < m_arStrings.GetSize() )
	{
		CString* pCStr = m_arStrings[nRow];
		m_arStrings.RemoveAt(nRow);

		if (pCStr)
		{
			delete pCStr;
		}
		return TRUE;
	}
	return FALSE;
}

CString* CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::GetStrDataC(int nRow) const
{
	CString* pCStr = NULL;

	if (nRow >=0 && nRow < m_arStrings.GetSize() )
	{
		pCStr = m_arStrings[nRow];
	}
	return pCStr;
}


CString* CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::
							GetStrData(int nRow, BOOL bGrowArrayIfNeed)
{
	CString* pCStr = NULL;

	if (nRow >=0 && nRow < m_arStrings.GetSize() )
	{
		pCStr = m_arStrings[nRow];
	}
	else if (bGrowArrayIfNeed)
	{
		m_arStrings.SetSize(nRow+1);
	}
	else
	{
		return NULL;
	}

	//---------------------------
	if (!pCStr)
	{
		pCStr = new CString();
		m_arStrings[nRow] = pCStr;
	}

	return pCStr;
}


BOOL CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::InsertText(int nRow, int nPos,
									LPCTSTR pcszText, BOOL bGrowArrayIfNeed,
									TCHAR chLeftSpaceFiller)
{
	ASSERT(nPos >= 0 && pcszText);

	CString* pCStr = GetStrData(nRow, bGrowArrayIfNeed);
	if (!pCStr || !pcszText && nPos < 0)
	{
		return FALSE;
	}

	int nStrLen0 = pCStr->GetLength();
	int nFillCount = nPos > nStrLen0 ? nPos - nStrLen0 : 0;
	int nInsPos = nPos;

	if (nFillCount)
	{
		if (chLeftSpaceFiller != _T('\0'))
		{
			CString strFill(chLeftSpaceFiller, nFillCount);
			*pCStr += strFill;
		}
		else
		{
			nInsPos = nStrLen0;
		}
	}

	INSERT_S(*pCStr, nInsPos, pcszText);
	return TRUE;
}

BOOL CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::
		DeleteText(int nRow, int nPos, int nCount)
{
	ASSERT(nPos >= 0 && nCount > 0);

	CString* pCStr = GetStrData(nRow, FALSE);
	if (!pCStr || nPos < 0 || nCount <= 0)
	{
		return FALSE;
	}

	int nStrLen0 = pCStr->GetLength();

	if (nPos >= nStrLen0)
	{
		return FALSE;
	}

	if (nCount > nStrLen0 - nPos)
	{
		//ASSERT(FALSE);
		nCount = nStrLen0 - nPos;
	}

	DELETE_S(*pCStr, nPos, nCount);
	return TRUE;
}

void CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::RemoveAllStrs()
{
	m_arStrings.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////
//class CStringPtrArray : public CArray<CString*, CString*>
CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::
CStringPtrArray::CStringPtrArray()
{

}

CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::
CStringPtrArray::~CStringPtrArray()
{
	RemoveAll();
}

void CXTPSyntaxEditBufferManager::CXTPSyntaxEditStringsManager::
CStringPtrArray::RemoveAll()
{
	int nCount = (int)GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString* pCStr = GetAt(i);
		if (pCStr)
		{
			SetAt(i, NULL);
			delete pCStr;
		}
	}
	TBase::RemoveAll();
}

