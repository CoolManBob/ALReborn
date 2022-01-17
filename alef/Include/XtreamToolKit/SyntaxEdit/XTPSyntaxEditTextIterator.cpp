// XTPSyntaxEditTextIterator.cpp : implementation file
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
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// common includes
#include "Common/XTPImageManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"

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
#include "XTPSyntaxEditTextIterator.h"

#define XTP_EDIT_LEX_TEXT_BUF_SIZE 512

#ifdef _UNICODE
	#define CHAR_W 1
#else
	#define CHAR_W 2
#endif

//---------------------------------------------------------------------------
#define CHR_TO_B(nChr) ( (int)((nChr)*sizeof(TCHAR)) )

////////////////////////////////////////////////////////////////////////////
AFX_INLINE int xtpEdit_StrLenInBytes(LPCTSTR pcszStr, int nChars)
{
#ifdef _UNICODE
	return (int)_tcsnbcnt(pcszStr, nChars) * sizeof(TCHAR);
#else
	return (int)_tcsnbcnt(pcszStr, nChars);
#endif
}

AFX_INLINE int xtpEdit_StrCharsToBytes(LPCTSTR pcszStr, int nChars)
{
#ifdef _UNICODE
	UNREFERENCED_PARAMETER(pcszStr);
	return nChars * sizeof(TCHAR);
#else
	LPCTSTR p2;
	if (nChars == 1)
	{
		p2 = _tcsinc(pcszStr);
	}
	else
	{
		p2 = _tcsninc(pcszStr, nChars);
	}
	int nB = int(p2 - pcszStr);
	return nB;

	//return _tcsnbcnt(pcszStr, nChars);
#endif
}
////////////////////////////////////////////////////////////////////////////

static const TCHAR szEOL[] = _T("\x0D\x0A");

////////////////////////////////////////////////////////////////////////////
//class CXTPSyntaxEditTextIterator
namespace XTPSyntaxEditLexAnalyser
{
	BOOL IsEventSet(HANDLE hEvent);
}

CXTPSyntaxEditTextIterator::CXTPSyntaxEditTextIterator(CXTPSyntaxEditBufferManager* pData)
{
	m_pData = pData;
	if (m_pData)
	{
		m_pData->InternalAddRef();
	}

	int nMaxBackOffset_sch = (int)m_pData->GetLexParser()->GetSchemaOptions(
								m_pData->GetFileExt())->m_dwMaxBackParseOffset;

	// WARNING! - value should be in reasonable range
	ASSERT(nMaxBackOffset_sch >= 10 && nMaxBackOffset_sch <= 10*1000);

	m_nBufOffsetB_normal = max(nMaxBackOffset_sch, 20);
	m_nBufOffsetB_max = XTP_EDIT_LEX_TEXT_BUF_SIZE;
	m_nBufOffsetB = 0;

	SeekBegin();
}

CXTPSyntaxEditTextIterator::~CXTPSyntaxEditTextIterator()
{
	if (m_pData)
	{
		m_pData->InternalRelease();
	}
}

CString CXTPSyntaxEditTextIterator::GetEOL() // "\r\n", "\n\r", "\r"
{
	if (!m_pData)
	{
		ASSERT(FALSE);
		return szEOL;
	}
	CString strEOL = m_pData->GetCurCRLF();
	return strEOL;
}

void CXTPSyntaxEditTextIterator::SeekBegin()
{
	m_LCpos.nLine = 1;
	m_LCpos.nCol = 0;

	m_mapLine2Len.RemoveAll();

	int nEndRow = m_pData->GetRowCount();
	int nHSize = max(nEndRow/10, 500);
	m_mapLine2Len.InitHashTable(nHSize, FALSE);

	m_nNextLine = 1;
	m_nBufSizeB = 0;

	m_nTmpOffsetC = 0;
	m_nTmpOffsetB = 0;

	m_nBufOffsetB = 10;

	if (m_arBuffer.GetSize() < max(m_nBufOffsetB_normal, 500))
	{
		m_arBuffer.SetSize(max(m_nBufOffsetB_normal, 500));
	}
	memset(m_arBuffer.GetData(), 0, m_arBuffer.GetSize());

	TCHAR* pData = GetBuffer(m_nBufOffsetB);

	STRCPY_S(pData, _tcslen(szEOL) + 1, szEOL);
	m_nBufOffsetB += CHR_TO_B(2);

	//---
	int nLen = GetLineLen(m_nNextLine, TRUE);
	m_bEOF = nLen == 0;
}

BOOL CXTPSyntaxEditTextIterator::SeekPos(const XTP_EDIT_LINECOL& posLC, HANDLE hBreakEvent)
{
	SeekBegin();

	if (m_bEOF)
	{
		return FALSE;
	}

	int nSumLenB = 0;
	int nLnStart = posLC.nLine;

	while (nSumLenB < m_nBufOffsetB_normal && nLnStart > 1)
	{
		nSumLenB += GetLineLenBytes(nLnStart, TRUE);
		nLnStart--;

		if (hBreakEvent && XTPSyntaxEditLexAnalyser::IsEventSet(hBreakEvent))
		{
			return FALSE;
		}
	}
	m_nNextLine = nLnStart;
	GetText();

	m_LCpos.nLine = nLnStart;
	m_LCpos.nCol = 0;

	while (m_LCpos < posLC && !m_bEOF)
	{
		int nSeek = 1;
		if (m_LCpos.nLine < posLC.nLine)
		{
			nSeek = 0;
			if (!m_mapLine2Len.Lookup(m_LCpos.nLine, nSeek) || !nSeek)
			{
				ASSERT(nSeek || !nSeek && m_LCpos.nLine == m_pData->GetRowCount());
				nSeek = 1;
			}
		}
		else
		{
			ASSERT(m_LCpos.nLine == posLC.nLine);
			nSeek = posLC.nCol - m_LCpos.nCol;
			ASSERT(nSeek >=0);
		}

		if (nSeek <= 0)
		{
			ASSERT(FALSE);
			break;
		}

		SeekNext(nSeek);

		if (hBreakEvent && XTPSyntaxEditLexAnalyser::IsEventSet(hBreakEvent))
		{
			return FALSE;
		}
	}

	BOOL bRes = (m_LCpos == posLC);
	return bRes;
}

LPCTSTR CXTPSyntaxEditTextIterator::GetText(int nCharsBuf)  // don't remove line end chars
{
	if (!m_pData)
	{
		ASSERT(FALSE);
		return NULL;
	}
	int nSizeB = (int)m_arBuffer.GetSize();

	const int cnReservB = (XTP_EDIT_LEX_TEXT_BUF_SIZE+100)*2;

	if (!m_nBufSizeB || (m_nBufSizeB < CHR_TO_B(nCharsBuf)) )
	{
		int nEndRow = m_pData->GetRowCount();

		while (m_nNextLine <= nEndRow && m_nBufSizeB < CHR_TO_B(nCharsBuf) )
		{
			CString strBuf;
			m_pData->GetLineText(m_nNextLine, strBuf, TRUE);

			int nLenC = (int)_tcsclen(strBuf);
			int nLenB = xtpEdit_StrLenInBytes(strBuf, nLenC);

			//---------------------------------------------------------------------------
			int nNeedSizeB = m_nBufOffsetB + m_nBufSizeB + nLenB +
							 max(256, nCharsBuf*2);

			if (nNeedSizeB > nSizeB)
			{
				m_arBuffer.SetSize(nNeedSizeB + cnReservB);
				nSizeB = (int)m_arBuffer.GetSize();
			}
			//---------------------------------------------------------------------------
			TCHAR* pBuf = GetBuffer(m_nBufOffsetB + m_nBufSizeB);

			ASSERT(*(pBuf-1) != _T('\0'));
			ASSERT(*pBuf == _T('\0'));
			*pBuf = _T('\0');

			STRCPY_S(pBuf, strBuf.GetLength() + 1, strBuf);

			ASSERT(nSizeB-m_nBufOffsetB-m_nBufSizeB > nLenB);

			m_nBufSizeB += nLenB;

			ASSERT(m_nBufSizeB < nSizeB);

			m_mapLine2Len[m_nNextLine] = nLenC;
			//TRACE(_T("TEXT-ITERATOR: line(%d) len = %d \n"),m_nNextLine, nLenC);

			//--------------------------------
			if (m_nNextLine == nEndRow)
			{
				TCHAR* pBufEnd = GetBuffer(m_nBufOffsetB + m_nBufSizeB);
				STRCPY_S(pBufEnd, _tcslen(szEOL) + 1, szEOL);
				int nLenB2 = xtpEdit_StrLenInBytes(szEOL, 2);
				m_nBufSizeB += nLenB2;

				ASSERT(m_nBufSizeB < nSizeB);
			}

			m_nNextLine++;
		}
	}

	ASSERT(m_nBufSizeB < nSizeB);

	TCHAR* pText = GetBuffer(m_nBufOffsetB + m_nTmpOffsetB);

	return pText;
}

// Move cur pos and return pointer to the text begin;
LPCTSTR CXTPSyntaxEditTextIterator::SeekNext(DWORD dwChars, int nCharsBuf)
{
	int nBSize = (int)m_arBuffer.GetSize();
	ASSERT(m_nBufSizeB <= nBSize);

	if (m_nBufSizeB < CHR_TO_B(nCharsBuf + dwChars))
	{
		GetText(nCharsBuf + dwChars);
	}

	if (m_nBufSizeB < CHR_TO_B(dwChars) )
	{
		dwChars = m_nBufSizeB/sizeof(TCHAR);
	}

	if (m_nBufSizeB >= CHR_TO_B(dwChars) && m_nBufSizeB)
	{
		nBSize = (int)m_arBuffer.GetSize();
		ASSERT(nBSize > CHR_TO_B(dwChars) );

		TCHAR* pText = GetBuffer(m_nBufOffsetB);
		//int nStepB = xtpEdit_StrLenInBytes(pText, dwChars);
		int nStepB = xtpEdit_StrCharsToBytes(pText, dwChars);
		ASSERT(nStepB > 0);

		if (m_nBufOffsetB > m_nBufOffsetB_max)
		{
			int nStepRem = m_nBufOffsetB - m_nBufOffsetB_normal;
			ASSERT(nStepRem > 0);

			m_arBuffer.RemoveAt(0, nStepRem);
			m_nBufOffsetB -= nStepRem;
		}

		m_nBufOffsetB += nStepB;
		m_nBufSizeB -= nStepB;

		TCHAR* pBuf = GetBuffer(m_nBufOffsetB + m_nBufSizeB);
		ASSERT(*(pBuf-1) != _T('\0'));
		ASSERT(*pBuf == _T('\0'));

		m_bEOF = m_nBufSizeB <= 0;

		if (dwChars && !m_bEOF)
		{
			LCPosAdd(m_LCpos, dwChars);
		}

		SetTxtOffset(m_nTmpOffsetC);

		pText = GetBuffer(m_nBufOffsetB + m_nTmpOffsetB);

		return pText;
	}

	return NULL;
}

void CXTPSyntaxEditTextIterator::SetTxtOffset(int nOffsetChars)
{
	m_nTmpOffsetC = nOffsetChars;
	m_nTmpOffsetB = 0;

	if (m_nTmpOffsetC)
	{
		TCHAR* pText = GetBuffer(m_nBufOffsetB);

		if (m_nTmpOffsetC < 0)
		{
			TCHAR* pText_min = GetBuffer(0);
			TCHAR* pText0 = pText;
			for (int i = 0; i < labs(nOffsetChars); i++)
			{
				if (pText0 <= pText_min)
				{
					break;
				}
				pText0 = _tcsdec(pText_min, pText0);
			}
			m_nTmpOffsetB = -1 * int( ((byte*)pText) - ((byte*)pText0) );
		}
		else
		{
			m_nTmpOffsetB = xtpEdit_StrLenInBytes(pText, m_nTmpOffsetC);
		}
	}
}

BOOL CXTPSyntaxEditTextIterator::IsEOF()
{
	return m_bEOF;
}

XTP_EDIT_LINECOL CXTPSyntaxEditTextIterator::GetPosLC()
{
	return m_LCpos;
}

void CXTPSyntaxEditTextIterator::LCPosAdd(XTP_EDIT_LINECOL& rLC, int nCharsAdd)
{
	int nLineLen = 0;

	int nRestChars = nCharsAdd;
	while (nRestChars)
	{
		if (m_mapLine2Len.Lookup(rLC.nLine, nLineLen))
		{
			if (nLineLen == 0)
			{
				break;   // last empty line
			}
			ASSERT(nLineLen > 0);

			int nDiff = nLineLen - rLC.nCol;
			ASSERT(nDiff >= 0);

			if (nDiff <= nRestChars)
			{
				rLC.nLine++;
				rLC.nCol = 0;

				nRestChars -= nDiff;
			}
			else
			{
				rLC.nCol += nRestChars;
				nRestChars = 0;
				break;
			}
		}
		else
		{
			break; // no more lines
		}
	}
}

void CXTPSyntaxEditTextIterator::LCPosDec(XTP_EDIT_LINECOL& rLC)
{
	rLC.nCol--;
	if (rLC.nCol < 0)
	{
		if (rLC.nLine <= 1)
		{
			ASSERT(FALSE);
			rLC.nCol = 0;
		}
		else
		{
			rLC.nLine--;
			int nLineLen = 0;
			if (m_mapLine2Len.Lookup(rLC.nLine, nLineLen))
			{
				ASSERT(nLineLen || rLC.nLine == m_pData->GetRowCount());
				nLineLen = max(1, nLineLen);

				rLC.nCol = nLineLen-1;
			}
			else
			{
				ASSERT(FALSE);
				rLC.nLine++;
				rLC.nCol++;
			}
		}
	}
}

int CXTPSyntaxEditTextIterator::GetLineLen(int nLine, BOOL bWithEOL)
{
	if (!m_pData)
	{
		ASSERT(FALSE);
		return 0;
	}

	int nEndRow = m_pData->GetRowCount();
	if (nLine > 0 && nLine <= nEndRow)
	{
		CString strBuf;
		m_pData->GetLineText(nLine, strBuf, bWithEOL);

		int nLen = (int)_tcsclen(strBuf);
		return nLen;
	}
	return 0;
}

int CXTPSyntaxEditTextIterator::GetLineLenBytes(int nLine, BOOL bWithEOL)
{
	if (!m_pData)
	{
		ASSERT(FALSE);
		return 0;
	}

	int nEndRow = m_pData->GetRowCount();
	if (nLine > 0 && nLine <= nEndRow)
	{
		CString strBuf;
		m_pData->GetLineText(nLine, strBuf, bWithEOL);

		int nLenB = xtpEdit_StrLenInBytes(strBuf, strBuf.GetLength());
		return nLenB;
	}
	return 0;
}

CString CXTPSyntaxEditTextIterator::GetFileExt()
{
	if (!m_pData)
	{
		ASSERT(FALSE);
		return _T("");
	}

	CString strFN = m_pData->GetFileExt();
	return strFN;
}
