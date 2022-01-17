// XTPSyntaxEditLexCfgFileReader.cpp: implementation of the CXTPSyntaxEditLexCfgFileReader class.
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

// common includes
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPResourceManager.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef DBG_READ_TRACE
	#define DBG_READ_TRACE
#endif

namespace XTPSyntaxEditLexAnalyser
{
	//===========================================================================
	// XTP_EDIT_LEXPROPINFO
	//===========================================================================
	XTP_EDIT_LEXPROPINFO::XTP_EDIT_LEXPROPINFO()
	{
		nLine           = 0;
		nOffset         = 0;
		nPropertyLen    = 0;
	}

	XTP_EDIT_LEXPROPINFO::XTP_EDIT_LEXPROPINFO(const XTP_EDIT_LEXPROPINFO& rSrc)
	{
		arPropName.RemoveAll();
		int i;
		for (i = 0; i < rSrc.arPropName.GetSize(); ++i) {
			arPropName.Add(rSrc.arPropName[i]);
		}

		arPropValue.RemoveAll();
		for (i = 0; i < rSrc.arPropValue.GetSize(); ++i) {
			arPropValue.Add(rSrc.arPropValue[i]);
		}

		nLine           = rSrc.nLine;
		nOffset         = rSrc.nOffset;
		nPropertyLen    = rSrc.nPropertyLen;
	}

	const XTP_EDIT_LEXPROPINFO& XTP_EDIT_LEXPROPINFO::operator = (const XTP_EDIT_LEXPROPINFO& rSrc)
	{
		arPropName.RemoveAll();
		int i;
		for (i = 0; i < rSrc.arPropName.GetSize(); ++i) {
			arPropName.Add(rSrc.arPropName[i]);
		}

		arPropValue.RemoveAll();
		for (i = 0; i < rSrc.arPropValue.GetSize(); ++i) {
			arPropValue.Add(rSrc.arPropValue[i]);
		}

		nLine           = rSrc.nLine;
		nOffset         = rSrc.nOffset;
		nPropertyLen    = rSrc.nPropertyLen;

		return *this;
	}

	//===========================================================================
	// CXTPSyntaxEditLexPropInfoArray
	//===========================================================================
	CXTPSyntaxEditLexPropInfoArray::CXTPSyntaxEditLexPropInfoArray()
	{

	}

	CXTPSyntaxEditLexPropInfoArray::CXTPSyntaxEditLexPropInfoArray(const CXTPSyntaxEditLexPropInfoArray& rSrc)
	{
		RemoveAll();

		for (int i = 0; i < rSrc.GetSize(); ++i)
		{
			XTP_EDIT_LEXPROPINFO info(rSrc[i]);
			Add(info);
		}
	}

	const CXTPSyntaxEditLexPropInfoArray& CXTPSyntaxEditLexPropInfoArray::operator = (const CXTPSyntaxEditLexPropInfoArray& rSrc)
	{
		RemoveAll();

		for (int i = 0; i < rSrc.GetSize(); ++i)
		{
			XTP_EDIT_LEXPROPINFO info(rSrc[i]);
			Add(info);
		}

		return *this;
	}

	//===========================================================================
	// XTP_EDIT_LEXCLASSINFO
	//===========================================================================
	XTP_EDIT_LEXCLASSINFO::XTP_EDIT_LEXCLASSINFO()
	{
		nEndLine    = 0;
		nStartLine  = 0;
	}

	XTP_EDIT_LEXCLASSINFO::XTP_EDIT_LEXCLASSINFO(const XTP_EDIT_LEXCLASSINFO& rSrc)
	{
		arPropertyDesc.RemoveAll();
		for (int i = 0; i < rSrc.arPropertyDesc.GetSize(); ++i)
		{
			XTP_EDIT_LEXPROPINFO info(rSrc.arPropertyDesc[i]);
			arPropertyDesc.Add(info);
		}

		nEndLine    = rSrc.nEndLine;
		nStartLine  = rSrc.nStartLine;
		csClassName = rSrc.csClassName;
	}

	const XTP_EDIT_LEXCLASSINFO& XTP_EDIT_LEXCLASSINFO::operator = (const XTP_EDIT_LEXCLASSINFO& rSrc)
	{
		arPropertyDesc.RemoveAll();
		for (int i = 0; i < rSrc.arPropertyDesc.GetSize(); ++i)
		{
			XTP_EDIT_LEXPROPINFO info(rSrc.arPropertyDesc[i]);
			arPropertyDesc.Add(info);
		}

		nEndLine    = rSrc.nEndLine;
		nStartLine  = rSrc.nStartLine;
		csClassName = rSrc.csClassName;

		return *this;
	}

	//===========================================================================
	// CXTPSyntaxEditLexClassInfoArray
	//===========================================================================
	CXTPSyntaxEditLexClassInfoArray::CXTPSyntaxEditLexClassInfoArray()
	{
		m_bModified = FALSE;
	}

	CXTPSyntaxEditLexClassInfoArray::CXTPSyntaxEditLexClassInfoArray(const CXTPSyntaxEditLexClassInfoArray& rSrc)
	{
		RemoveAll();
		m_bModified = rSrc.m_bModified;

		for (int i = 0; i < rSrc.GetSize(); ++i)
		{
			XTP_EDIT_LEXCLASSINFO info(rSrc[i]);
			Add(info);
		}
	}

	const CXTPSyntaxEditLexClassInfoArray& CXTPSyntaxEditLexClassInfoArray::operator = (const CXTPSyntaxEditLexClassInfoArray& rSrc)
	{
		RemoveAll();
		m_bModified = rSrc.m_bModified;

		for (int i = 0; i < rSrc.GetSize(); ++i)
		{
			XTP_EDIT_LEXCLASSINFO info(rSrc[i]);
			Add(info);
		}

		return *this;
	}

	extern CString MakeStr(const CStringArray& rArProps, LPCTSTR strSplitter);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace XTPSyntaxEditLexAnalyser;

const CString cstrDelims(_T(" \t,"));
const CString cstrEOL(_T("\r\n"));
const CString cstrHex(_T("0123456789ABCDEFGabcdefg"));

AFX_STATIC BOOL AFX_CDECL AfxCompareTime(const FILETIME& ft1, const FILETIME& ft2, DWORD dwTimeDiff = 500)
{
	ULARGE_INTEGER uFT1, uFT2;

	uFT1.LowPart = ft1.dwLowDateTime;
	uFT1.HighPart = ft1.dwHighDateTime;

	uFT2.LowPart = ft2.dwLowDateTime;
	uFT2.HighPart = ft2.dwHighDateTime;

	ULONGLONG ullDiff = uFT1.QuadPart > uFT2.QuadPart ? uFT1.QuadPart - uFT2.QuadPart : uFT2.QuadPart - uFT1.QuadPart;

	//ullDiff = the number of 100-nanosecond intervals since January 1, 1601.
	if (ullDiff <= (dwTimeDiff*1000*1000/100))
	{
		return TRUE;
	}
	return FALSE;
}

AFX_STATIC BOOL AFX_CDECL AfxOpenFile(CFile& file, LPCTSTR lpszFileName, UINT nOpenFlags)
{
	if (nOpenFlags == CFile::modeRead)
	{
		if (!FILEEXISTS_S(lpszFileName))
		{
			return FALSE;
		}
	}

	CFileException e;
	if (!file.Open(lpszFileName, nOpenFlags, &e))
	{
		AfxThrowFileException(e.m_cause,e.m_lOsError, e.m_strFileName);
	}

	return TRUE;
}

AFX_STATIC CString AFX_CDECL AfxMakeStrES(const CStringArray& rArProps, LPCTSTR strSplitter)
{
	CString strResult;
	int nCount = (int)rArProps.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CString csProp = XTPSyntaxEditLexConfig()->ESToStr(rArProps[i], TRUE);
		if (i)
		{
			strResult += strSplitter;
		}
		CString strI = csProp;
		strResult += strI;
	}
	return strResult;
}

////////////////////////////////////////////////////////////////////////////

CXTPSyntaxEditLexCfgFileReader& AFX_CDECL CXTPSyntaxEditLexCfgFileReader::Instance()
{
	static CXTPSyntaxEditLexCfgFileReader instance;
	return instance;
}

CXTPSyntaxEditLexCfgFileReader::CXTPSyntaxEditLexCfgFileReader()
{
	m_csDataBuffer = XTP_EDIT_LEXCLASS_EMPTYSTR;
	m_nCurrLine = 0;
	m_nCurrLine_pos = 0;
	m_nCurrPos = 0;
	m_nEOFPos = 0;
	m_strToken = XTP_EDIT_LEXCLASS_EMPTYSTR;
	m_nTokenType = xtpEditTokType_Unknown;
	m_bReadNames = TRUE;
	m_bES = FALSE;
}

CXTPSyntaxEditLexCfgFileReader::~CXTPSyntaxEditLexCfgFileReader()
{
	CleanInfoMap(m_mapLexClassInfo);
}

void CXTPSyntaxEditLexCfgFileReader::CleanInfoMap(CMapStringToPtr& mapInfo)
{
	for (POSITION pos = mapInfo.GetStartPosition(); pos;)
	{
		CString csFileName;
		CXTPSyntaxEditLexClassInfoArray* pInfoArray = NULL;
		mapInfo.GetNextAssoc(pos, csFileName, (void*&)pInfoArray);
		SAFE_DELETE(pInfoArray);
	}
	mapInfo.RemoveAll();
}

void CXTPSyntaxEditLexCfgFileReader::CopyInfoMap(CMapStringToPtr& mapInfo)
{
	CleanInfoMap(mapInfo);

	for (POSITION pos = m_mapLexClassInfo.GetStartPosition(); pos;)
	{
		// get the source data.
		CString csFileName;
		CXTPSyntaxEditLexClassInfoArray* pSource = NULL;
		m_mapLexClassInfo.GetNextAssoc(pos, csFileName, (void*&)pSource);

		// instantiate a new data pointer.
		CXTPSyntaxEditLexClassInfoArray* pDest =
			new CXTPSyntaxEditLexClassInfoArray(*pSource);

		// set the data.
		mapInfo[csFileName] = pDest;
	}
}

void CXTPSyntaxEditLexCfgFileReader::ReadSource2(const CString& csFileName, CStringArray& arBuffer)
{
	try
	{
		arBuffer.RemoveAll();

		CStdioFile file;
		if (AfxOpenFile(file, csFileName, CFile::modeRead))
		{
			arBuffer.Add(_T(""));

			CString csBuffer;
			while (file.ReadString(csBuffer))
			{
				int nLen = csBuffer.GetLength();
				if (nLen && csBuffer[nLen-1] == _T('\r'))
				{
					DELETE_S(csBuffer, nLen-1);
				}
				arBuffer.Add(csBuffer);
			}
		}
	}
	catch(CFileException* expFile)
	{
	#ifdef _DEBUG
		ProcessFileException(expFile);
	#endif
		expFile->Delete();
	}
}

void CXTPSyntaxEditLexCfgFileReader::ReadSource(const CString& csFileName, BOOL bSaveInfo)
{
	try
	{
		CFile file;
		if (AfxOpenFile(file, csFileName, CFile::modeRead))
		{
			int iLen = (int)file.GetLength();
			char* pszBuffer = new char[iLen+2];
			int iBytes = (int)file.Read(pszBuffer, iLen);

			ASSERT(iLen == iBytes);

			pszBuffer[iLen] = '\0';
			m_csDataBuffer = pszBuffer;

			SAFE_DELETE_AR(pszBuffer);
			file.Close();

			Parse(m_arLexClassInfo);

			if (bSaveInfo)
			{
				CString csKey = csFileName;
				csKey.MakeLower();

				CXTPSyntaxEditLexClassInfoArray* pInfo = NULL;
				if (!m_mapLexClassInfo.Lookup(csKey, (void*&)pInfo))
				{
					m_mapLexClassInfo[csKey] = new CXTPSyntaxEditLexClassInfoArray(m_arLexClassInfo);
				}
				else
				{
					*pInfo = m_arLexClassInfo;
				}
			}
		}
	}
	catch(CFileException* expFile)
	{
		#ifdef _DEBUG
		ProcessFileException(expFile);
		#endif
		expFile->Delete();
	}
}

void CXTPSyntaxEditLexCfgFileReader::ReadSource(UINT nResourceID)
{
	VERIFY(XTPResourceManager()->LoadHTML(&m_csDataBuffer, nResourceID));
}

void CXTPSyntaxEditLexCfgFileReader::Parse(CXTPSyntaxEditLexClassInfoArray& arLexClassDesc)
{
	arLexClassDesc.RemoveAll();

	m_nEOFPos = m_csDataBuffer.GetLength() - 1;
	m_nPrevPos = 0;
	if (m_nEOFPos < 0)
	{
		return;
	}
	m_nCurrLine = 1;
	m_nCurrLine_pos = 0;
	m_nCurrPos = 0;
	m_strToken = XTP_EDIT_LEXCLASS_EMPTYSTR;

	int nTokenType = 0;
	while (m_nEOFPos > m_nCurrPos && m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) != 0)
	{
		nTokenType = GetLexToken();
	}

	if (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) == 0)
	{
		while (m_nEOFPos > m_nCurrPos)
		{
			XTP_EDIT_LEXCLASSINFO infoClass;
			ParseLexClass(infoClass);
			if (!infoClass.csClassName.IsEmpty())
			{
				arLexClassDesc.Add(infoClass);
			}
		}
	}
}

void CXTPSyntaxEditLexCfgFileReader::ParseLexClass(XTP_EDIT_LEXCLASSINFO& infoClass)
{
	if (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) == 0)
	{
		infoClass.nStartLine = m_nCurrLine;
		infoClass.nEndLine = 0;
	}

	int nTokenType = GetLexToken();

	int nPrevToken = 0;
	while (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) != 0 && m_nEOFPos > m_nCurrPos )
	{
		XTP_EDIT_LEXPROPINFO infoProp;

		if (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_ENDTOKEN) == 0)
		{
			infoClass.nEndLine = m_nCurrLine;
		}

		infoProp.nLine = 0;
		infoProp.nPropertyLen = 0;
		infoProp.nOffset = 0;
		int nValBeginPos = 0;

		while (m_strToken != _T('=') && m_nEOFPos > m_nCurrPos && m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) != 0)
		{

			if (nTokenType != xtpEditTokType_EOL && m_nEOFPos > m_nCurrPos)
			{
				if (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_PROPNAME) == 0)
				{
					infoClass.csClassName = m_strToken;
					//TRACE(_T("\nName = %s"), m_strToken);
				}
				else
				{
					infoProp.arPropName.Add(m_strToken);
					DBG_READ_TRACE(_T("%s$"),m_strToken);
				}
				infoProp.nLine = m_nCurrLine;
			}
			nTokenType = GetLexToken();
		}

		if (m_strToken.CompareNoCase(XTP_EDIT_LEXCLASS_STARTTOKEN) != 0 &&
			nTokenType != xtpEditTokType_EOL)
		{ // get value's chain

			nTokenType = GetLexToken();

			infoProp.nOffset = m_nCurrPos - m_nPrevPos - m_strToken.GetLength();
			nValBeginPos = m_nCurrPos - m_strToken.GetLength();
			if (nPrevToken == xtpEditTokType_Quoted)
			{
				infoProp.nOffset += 1;
			}
			if (nTokenType == xtpEditTokType_Quoted )
			{
				infoProp.nOffset -= 1;
				nValBeginPos -= 1;
			}

			while (nTokenType != xtpEditTokType_EOL && m_nEOFPos > m_nCurrPos)
			{
				if (nTokenType != xtpEditTokType_EOL)
				{
					if (infoClass.csClassName.CompareNoCase(XTP_EDIT_LEXCLASS_PROPNAME) == 0)
					{
						infoClass.csClassName = m_strToken;
						DBG_READ_TRACE(_T("\nClass name = %s "),m_strToken);
					}
					else
					{
						m_nPrevPos = m_nCurrPos;
						CString str = StrToES(m_strToken, TRUE);
						infoProp.arPropValue.Add(str);

						str = ESToStr(str, TRUE);
						DBG_READ_TRACE(_T("|%s\n"),str);
					}

				}

				nPrevToken = nTokenType;
				nTokenType = GetLexToken();
			}

			if (infoProp.arPropValue.GetSize() > 0)
			{
				infoProp.nPropertyLen +=  m_nPrevPos - nValBeginPos;
				if (nPrevToken == xtpEditTokType_Quoted)
				{
					infoProp.nPropertyLen -= 1;
				}

				infoClass.arPropertyDesc.Add(infoProp);
			}

			nTokenType = GetLexToken();
		}
	}

	DBG_READ_TRACE(_T("\n ---------------- %s -----------------\n"), infoClass.csClassName);

	if (infoClass.nEndLine == 0)
	{
		int nCount = (int)infoClass.arPropertyDesc.GetSize();
		if (nCount)
		{
			infoClass.nEndLine = infoClass.arPropertyDesc[nCount-1].nLine;
		}
		else
		{
			infoClass.nEndLine = infoClass.nStartLine;
		}
	}
}


int CXTPSyntaxEditLexCfgFileReader::GetLexToken()
{
	int nIgnoredTokensMask = xtpEditTokType_Unknown;
	nIgnoredTokensMask |= xtpEditTokType_Delim;
	nIgnoredTokensMask |= xtpEditTokType_Comment;

	m_strToken = XTP_EDIT_LEXCLASS_EMPTYSTR;
	int nTokenType = GetToken();
	if (m_strToken == _T(":"))
			nTokenType = xtpEditTokType_Unknown;

	while ((nTokenType == xtpEditTokType_Unknown ||
		   nTokenType == xtpEditTokType_Delim||
		   nTokenType == xtpEditTokType_Comment
		   ) && m_nEOFPos > m_nCurrPos
		   )
	{
		m_strToken = XTP_EDIT_LEXCLASS_EMPTYSTR;
		nTokenType = GetToken();
	}
	if (m_strToken == _T("lexClass"))
	{
		GetToken();
	}

	return nTokenType;

}

int CXTPSyntaxEditLexCfgFileReader::GetToken()
{
	TCHAR tchCurrSymbol = _T('\0');
	int nFoundedToken = 0;
	if (m_nCurrPos >= m_nEOFPos)
	{
		return xtpEditTokType_Unknown;
	}

	//------------------------------------------------------------------------
	TCHAR tchCurr_0 = m_csDataBuffer.GetAt(m_nCurrPos);

	if (tchCurr_0 == _T('\x0A') && m_nCurrPos > m_nCurrLine_pos)
	{
		m_nCurrLine++;
		m_nCurrLine_pos = m_nCurrPos;
	}
	//------------------------------------------------------------------------

	tchCurrSymbol = m_csDataBuffer.GetAt(m_nCurrPos++);

	switch (m_nTokenType)
	{
	case xtpEditTokType_Unknown :
		nFoundedToken = ProcessUnknowToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Unknown)
		{
			GetToken();
		}
		break;
	case xtpEditTokType_Delim :
		nFoundedToken = ProcessDelimToken(tchCurrSymbol);
		break;
	case xtpEditTokType_Name :
		nFoundedToken = ProcessNameToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Name)
		{
			m_strToken += tchCurrSymbol;
			GetToken();
		}
		break;
	case xtpEditTokType_Value :
		nFoundedToken = ProcessValueToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Value)
		{
			m_strToken += tchCurrSymbol;
			GetToken();
		}
		break;
	case xtpEditTokType_Quoted :
		nFoundedToken = ProcessQuotedToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Quoted)
		{
			m_strToken += tchCurrSymbol;
			GetToken();
		}
		break;
	case xtpEditTokType_Comment :
		nFoundedToken = ProcessCommentToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Comment)
		{
			m_strToken += tchCurrSymbol;
			GetToken();
		}
		break;
	case xtpEditTokType_EOL :
		nFoundedToken = ProcessEOLToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_EOL)
		{
			m_strToken += tchCurrSymbol;
			GetToken();
		}
		break;
	case xtpEditTokType_Control :
		nFoundedToken = ProcessControlToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Control)
		{
		m_strToken += tchCurrSymbol;
			GetToken();
		}
	break;
	default :
		nFoundedToken = ProcessUnknowToken(tchCurrSymbol);
		if (m_nTokenType == xtpEditTokType_Unknown)
		{
			GetToken();
		}
	}

	return nFoundedToken;
}

int CXTPSyntaxEditLexCfgFileReader::ProcessUnknowToken(TCHAR tchCurrSymbol)
{
	int nFoundedToken = m_nTokenType;
	if (iswalpha(tchCurrSymbol))
	{
		m_nTokenType = xtpEditTokType_Name;
	}
	else if (tchCurrSymbol == _T('\''))
	{
		m_nTokenType = xtpEditTokType_Quoted;
	}
	else if (tchCurrSymbol == _T('/'))
	{
		m_nTokenType = xtpEditTokType_Comment;
	}
	else if (cstrDelims.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_Delim;
	}
	else if (cstrEOL.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_EOL;
	}
	else if (tchCurrSymbol == _T('='))
	{
		m_nTokenType = xtpEditTokType_Control;
	}
	else if (tchCurrSymbol == _T(':'))
	{
		m_nTokenType = xtpEditTokType_Control;
	}
	else
	{
		m_nTokenType = xtpEditTokType_Value;
	}

	if (nFoundedToken != m_nTokenType)
	{
		m_nCurrPos--;
	}

	return nFoundedToken;
}

int CXTPSyntaxEditLexCfgFileReader::ProcessDelimToken(TCHAR tchCurrSymbol)
{
	return ProcessUnknowToken(tchCurrSymbol);
}

int CXTPSyntaxEditLexCfgFileReader::ProcessNameToken(TCHAR tchCurrSymbol)
{
	int nFoundedToken = m_nTokenType;
	if (tchCurrSymbol == _T('\''))
	{
		m_nTokenType = xtpEditTokType_Quoted;
	}
	else if (tchCurrSymbol == _T('/'))
	{
		m_nTokenType = xtpEditTokType_Comment;
	}
	else if (cstrDelims.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_Delim;
	}
	else if (cstrEOL.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_EOL;
	}
	else if (tchCurrSymbol == _T('='))
	{
		m_nTokenType = xtpEditTokType_Control;
	}
	else if (tchCurrSymbol == _T(':'))
	{
		m_nTokenType = xtpEditTokType_Control;
	}

	if (nFoundedToken != m_nTokenType)
	{
		m_nCurrPos--;
	}

	return nFoundedToken;
}

int CXTPSyntaxEditLexCfgFileReader::ProcessValueToken(TCHAR tchCurrSymbol)
{
	int nFoundedToken = m_nTokenType;
	if (cstrDelims.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_Delim;
	}
	else if (cstrEOL.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_EOL;
	}
	else if (tchCurrSymbol == _T('/'))
	{
		m_nTokenType = xtpEditTokType_Comment;
	}

	if (nFoundedToken != m_nTokenType)
	{
		m_nCurrPos--;
	}

	return nFoundedToken;

}

int CXTPSyntaxEditLexCfgFileReader::ProcessQuotedToken(TCHAR tchCurrSymbol)
{

	int nFoundedToken = m_nTokenType;

	int nLen = m_strToken.GetLength();

	if (cstrEOL.Find(tchCurrSymbol) > -1)
		m_nTokenType = xtpEditTokType_EOL;
	else if (m_nCurrPos == m_nEOFPos)
		m_nTokenType = xtpEditTokType_Unknown;
	else if (nLen == 0)
		return nFoundedToken;
	else if (m_strToken.GetAt(max(0, m_strToken.GetLength() - 1)) == _T('\\'))
	{
		return nFoundedToken;
	}
	else if (tchCurrSymbol == _T('\''))
	{
		if (m_strToken == _T('\''))
		{
			nFoundedToken = GetToken();
		}
		else
		{
			m_nTokenType = xtpEditTokType_Unknown;
			m_strToken += tchCurrSymbol;
			m_nCurrPos++;
		}
	}

	return nFoundedToken;
}

int CXTPSyntaxEditLexCfgFileReader::ProcessCommentToken(TCHAR tchCurrSymbol)
{
	int nFoundedToken = m_nTokenType;
	if (cstrEOL.Find(tchCurrSymbol) > -1)
	{
		m_nTokenType = xtpEditTokType_EOL;
		m_nCurrPos--;
	}
	return nFoundedToken;
}

int CXTPSyntaxEditLexCfgFileReader::ProcessEOLToken(TCHAR tchCurrSymbol)
{
	return ProcessUnknowToken(tchCurrSymbol);
}

int CXTPSyntaxEditLexCfgFileReader::ProcessControlToken(TCHAR tchCurrSymbol)
{
	return ProcessUnknowToken(tchCurrSymbol);
}

CXTPSyntaxEditLexClassInfoArray& CXTPSyntaxEditLexCfgFileReader::GetLexClassInfoArray()
{
	return m_arLexClassInfo;
}

BOOL CXTPSyntaxEditLexCfgFileReader::WriteCfgFile(const CString& csFileName, CXTPSyntaxEditLexClassInfoArray& arLexClassDesc)
{
	if (!arLexClassDesc.m_bModified)
		return FALSE;

	CStringArray arBuffer;
	CMapPtrToBool mapSavedClass;

	ReadSource(csFileName, FALSE);
	ReadSource2(csFileName, arBuffer);

	try
	{
		CString csTmpFileName = csFileName + _T(".tmp");

		::SetFileAttributes(csTmpFileName, FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(csTmpFileName);

		CFile file;
		if (!AfxOpenFile(file, csTmpFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate))
			return FALSE;

		int nLastWrittenLine = 0;

		// (1) Update Existing and deleted Classes
		int nCount = (int)m_arLexClassInfo.GetSize();
		int i;
		for (i = 0; i < nCount; i++)
		{
			XTP_EDIT_LEXCLASSINFO& oldClassInfo = m_arLexClassInfo[i];

			XTP_EDIT_LEXCLASSINFO* pNewClassInfo = FindClassDesc(arLexClassDesc, oldClassInfo.csClassName);
			// class was deleted;
			if (!pNewClassInfo)
			{
				WriteStrings(file, arBuffer, nLastWrittenLine+1, oldClassInfo.nStartLine-1);
				nLastWrittenLine = oldClassInfo.nEndLine;
				continue;
			}
			mapSavedClass[pNewClassInfo] = TRUE;

			CMapPtrToBool mapSavedProps;
			CString csOffset(_T("\t"));

			// (1.a) Update Existing and deleted properties
			int nPropsCount = (int)oldClassInfo.arPropertyDesc.GetSize();
			int k;
			for (k = 0; k < nPropsCount; k++)
			{
				XTP_EDIT_LEXPROPINFO& oldInfoProp = oldClassInfo.arPropertyDesc[k];

				WriteStrings(file, arBuffer, nLastWrittenLine+1, oldInfoProp.nLine-1);
				nLastWrittenLine = oldInfoProp.nLine;

				XTP_EDIT_LEXPROPINFO* pNewInfoProp = FindPropDesc(pNewClassInfo, &oldInfoProp, mapSavedProps);
				if (pNewInfoProp)
				{
					WriteProp(file, csOffset, oldInfoProp, *pNewInfoProp, arBuffer);
					mapSavedProps[pNewInfoProp] = TRUE;
				}
			}

			// (1.b) Insert new properties
			nPropsCount = (int)pNewClassInfo->arPropertyDesc.GetSize();
			for (k = 0; k < nPropsCount; k++)
			{
				XTP_EDIT_LEXPROPINFO& newInfoProp = pNewClassInfo->arPropertyDesc[k];

				bool bSaved = false;
				if (!mapSavedProps.Lookup((void*)&newInfoProp, bSaved) || !bSaved)
				{
					WriteProp(file, csOffset, newInfoProp);
				}
			}
		}

		//------------------------------------------------------------------------
		WriteStrings(file, arBuffer, nLastWrittenLine+1, m_nCurrLine);
		nLastWrittenLine = m_nCurrLine;

		//===========================================================================
		// (2) Insert new Classes
		nCount = (int)arLexClassDesc.GetSize();
		for (i = 0; i < nCount; i++)
		{
			XTP_EDIT_LEXCLASSINFO& newClassInfo = arLexClassDesc[i];

			bool bSaved = false;
			if (!mapSavedClass.Lookup((void*)&newClassInfo, bSaved) || !bSaved)
			{
				WriteString(file, _T(""));
				WriteString(file, XTP_EDIT_LEXCLASS_STARTTOKEN);

				// Insert properties
				int nPropsCount = (int)newClassInfo.arPropertyDesc.GetSize();
				for (int k = 0; k < nPropsCount; k++)
				{
					const XTP_EDIT_LEXPROPINFO& newInfoProp = newClassInfo.arPropertyDesc[k];

					CString csOffset(_T("\t"));
					WriteProp(file, csOffset, newInfoProp);
				}

				WriteString(file, XTP_EDIT_LEXCLASS_ENDTOKEN);
			}
		}

		//===========================================================================
		file.Close();

		CString strFileName_prev = csFileName + _T(".old");
		::SetFileAttributes(strFileName_prev, FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(strFileName_prev);

		#ifdef _DEBUG
			//CFile::Rename(csFileName, strFileName_prev);
		#endif

		::SetFileAttributes(csFileName, FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(csFileName);
		CFile::Rename(csTmpFileName, csFileName);

		m_arLexClassInfo.RemoveAll();
		return TRUE;
	}

	catch(CFileException* expFile)
	{
#ifdef _DEBUG
		expFile->ReportError();
		//ProcessFileException(expFile);
#endif
		expFile->Delete();
	}

	m_arLexClassInfo.RemoveAll();
	arLexClassDesc.m_bModified = FALSE;

	return FALSE;
}

XTP_EDIT_LEXCLASSINFO* CXTPSyntaxEditLexCfgFileReader::FindClassDesc(CXTPSyntaxEditLexClassInfoArray& arInfoClass, const CString& csClassName)
{
	if (csClassName.IsEmpty())
	{
		ASSERT(FALSE);
		return NULL;
	}

	for (int i = 0; i < arInfoClass.GetSize(); i++)
	{
		if (arInfoClass[i].csClassName.CompareNoCase(csClassName) == 0)
		{
			return &arInfoClass[i];
		}
	}

	return NULL;
}

XTP_EDIT_LEXPROPINFO* CXTPSyntaxEditLexCfgFileReader::FindPropDesc(XTP_EDIT_LEXCLASSINFO* pInfoClass, XTP_EDIT_LEXPROPINFO* pInfoProp, CMapPtrToBool& mapUsed)
{
	if (!pInfoClass || !pInfoProp)
	{
		ASSERT(FALSE);
		return NULL;
	}

	int nPNameSize = (int)pInfoProp->arPropName.GetSize();
	CString strPropName_src = MakeStr(pInfoProp->arPropName, _T(":"));

	int nCount = (int)pInfoClass->arPropertyDesc.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		XTP_EDIT_LEXPROPINFO* pNewInfoProp = &(*pInfoClass).arPropertyDesc[i];
		ASSERT(pNewInfoProp);

		if (pNewInfoProp && pNewInfoProp->arPropName.GetSize() == nPNameSize)
		{
			CString strPName = MakeStr(pNewInfoProp->arPropName, _T(":"));

			if (strPropName_src.CompareNoCase(strPName) == 0)
			{
				bool bUsed = false;
				if (mapUsed.Lookup(pNewInfoProp, bUsed) && bUsed)
				{
					continue;
				}

				return pNewInfoProp;
			}
		}
	}

	return NULL;
}


void CXTPSyntaxEditLexCfgFileReader::WriteString(CFile& file, LPCTSTR pcszString)
{
	int nStrLen = (int)_tcslen(pcszString);

#ifdef _UNICODE
	CByteArray arBufferMBCS;
	arBufferMBCS.SetSize(nStrLen*2+4);

	char* pTextMBCS = (char*)arBufferMBCS.GetData();

	int nNewLen = WideCharToMultiByte(CP_ACP, 0,
		pcszString, -1, pTextMBCS, nStrLen*2+1, NULL, NULL);

	nStrLen = nNewLen ? nNewLen-1 : 0;
#else
	char* pTextMBCS = (char*)pcszString;
#endif

	file.Write(pTextMBCS, nStrLen);
	file.Write("\r\n", 2);
}

void CXTPSyntaxEditLexCfgFileReader::WriteStrings(CFile& file, CStringArray& arBuffer, int nFrom, int nTo)
{
	int nCount = (int)arBuffer.GetSize();
	if (nFrom >= nCount || nFrom > nTo)
	{
		return;
	}
	if (nTo >= nCount)
	{
		nTo = nCount-1;
	}

	for (int nLine = nFrom; nLine <= nTo; nLine++)
	{
		CString csBuffer = arBuffer[nLine];
		WriteString(file, csBuffer);
	}
}

void CXTPSyntaxEditLexCfgFileReader::WriteProp(CFile& file, CString& csOffset, const XTP_EDIT_LEXPROPINFO& newInfoProp)
{
	CString csPropName = MakeStr(newInfoProp.arPropName, _T(":"));
	CString csPropValue = AfxMakeStrES(newInfoProp.arPropValue, _T(", "));
	CString csPropString = csOffset + csPropName + _T(" = ") + csPropValue;
	WriteString(file, csPropString);
}

void CXTPSyntaxEditLexCfgFileReader::WriteProp(CFile& file, CString& csOffset, const XTP_EDIT_LEXPROPINFO& oldInfoProp, const XTP_EDIT_LEXPROPINFO& newInfoProp, const CStringArray& arBuffer)
{
	if (oldInfoProp.nLine < arBuffer.GetSize())
	{
		CString csBuffer = arBuffer[oldInfoProp.nLine];

		int iIndex = csBuffer.Find(_T("="));
		ASSERT(iIndex > 0);
		if (iIndex > 0)
		{
			csOffset = csBuffer.SpanIncluding(_T(" \t"));

			CString csSep = csBuffer.Mid(iIndex+1);
			csSep = csSep.SpanIncluding(_T(" \t"));
			iIndex += csSep.GetLength();

			CString csNewBuffer = csBuffer;
			DELETE_S(csNewBuffer, iIndex+1, oldInfoProp.nPropertyLen);

			CString csPropValue = AfxMakeStrES(newInfoProp.arPropValue, _T(", "));
			INSERT_S(csNewBuffer, iIndex+1, csPropValue);
			WriteString(file, csNewBuffer);
			return;
		}

	}
	else
	{
		ASSERT(FALSE);
	}

	WriteProp(file, csOffset, newInfoProp);
}

#ifdef _DEBUG
void CXTPSyntaxEditLexCfgFileReader::ProcessFileException(CFileException* pExc)
{
	CString strErrorMsg(_T(" "));
	switch (pExc->m_cause)
	{
#if (_MSC_VER > 1310) // VS2005
	case CFileException::genericException :
#else
	case CFileException::generic :
#endif
		strErrorMsg = _T("An unspecified error occurred.");
		break;
	case CFileException::fileNotFound :
		strErrorMsg = _T("The file could not be located.");
		break;
	case CFileException::badPath :
		strErrorMsg = _T("All or part of the path is invalid.");
		break;
	case CFileException::tooManyOpenFiles :
		strErrorMsg = _T("The permitted number of open files was exceeded.");
		break;
	case CFileException::accessDenied :
		strErrorMsg = _T("The file could not be accessed.");
		break;
	case CFileException::invalidFile :
		strErrorMsg = _T("There was an attempt to use an invalid file handle.");
		break;
	case CFileException::removeCurrentDir :
		strErrorMsg = _T("The current working directory cannot be removed.");
		break;
	case CFileException::directoryFull :
		strErrorMsg = _T("There are no more directory entries.");
		break;
	case CFileException::badSeek :
		strErrorMsg = _T("There was an error trying to set the file pointer.");
		break;
	case CFileException::hardIO :
		strErrorMsg = _T("There was a hardware error.");
		break;
	case CFileException::sharingViolation :
		strErrorMsg = _T("SHARE.EXE was not loaded, or a shared region was locked.");
		break;
	case CFileException::lockViolation :
		strErrorMsg = _T("There was an attempt to lock a region that was already locked.");
		break;
	case CFileException::diskFull :
		strErrorMsg = _T("The disk is full.");
		break;
	case CFileException::endOfFile :
		strErrorMsg = _T("The end of file was reached.");
		break;
	default :
		strErrorMsg = _T("Unknown error.");
	}

	TRACE(_T("ERROR: %s File Name: %s \n"), (LPCTSTR)strErrorMsg, (LPCTSTR)pExc->m_strFileName);
}
#endif


CString CXTPSyntaxEditLexCfgFileReader::StrToES(CString strSrc, BOOL bQuoted)
{
	CString strRez(strSrc);
	strRez.TrimLeft(); strRez.TrimRight();

	int nLen = strRez.GetLength();
	if (nLen < 1)
	{
		return strRez;
	}

	if (bQuoted && strRez.GetAt(0) == _T('\'') && strRez.GetAt(nLen - 1) == _T('\''))
	{
		strRez.SetAt(0,      _T('\x1'));
		strRez.SetAt(nLen-1, _T('\x1'));
	}

	REPLACE_S(strRez, _T("\\\\"), _T("\x2"));

	REPLACE_S(strRez, _T("\\a"), _T("\a"));
	REPLACE_S(strRez, _T("\\b"), _T("\b"));
	REPLACE_S(strRez, _T("\\f"), _T("\f"));
	REPLACE_S(strRez, _T("\\n"), _T("\n"));
	REPLACE_S(strRez, _T("\\r"), _T("\r"));
	REPLACE_S(strRez, _T("\\t"), _T("\t"));
	REPLACE_S(strRez, _T("\\v"), _T("\v"));
	REPLACE_S(strRez, _T("\\'"), _T("\'"));

	REPLACE_S(strRez, _T("\x2"), _T("\\"));
	REPLACE_S(strRez, _T("\x1"), _T("\'"));
	return strRez;
}

CString CXTPSyntaxEditLexCfgFileReader::ESToStr(CString strSrc, BOOL bQuoted)
{
	CString strRez(strSrc);

	strRez.TrimLeft(); strRez.TrimRight();

	int nLen = strRez.GetLength();
	if (nLen < 1)
	{
		return strRez;
	}

	if (bQuoted && strRez.GetAt(0) == _T('\'') && strRez.GetAt(nLen - 1) == _T('\''))
	{
		strRez.SetAt(0,      _T('\x1'));
		strRez.SetAt(nLen-1, _T('\x1'));
	}

	REPLACE_S(strRez, _T("\\"), _T("\x2"));

	REPLACE_S(strRez, _T("\a"), _T("\\a"));
	REPLACE_S(strRez, _T("\b"), _T("\\b"));
	REPLACE_S(strRez, _T("\f"), _T("\\f"));
	REPLACE_S(strRez, _T("\n"), _T("\\n"));
	REPLACE_S(strRez, _T("\r"), _T("\\r"));
	REPLACE_S(strRez, _T("\t"), _T("\\t"));
	REPLACE_S(strRez, _T("\v"), _T("\\v"));
	REPLACE_S(strRez, _T("\'"), _T("\\'"));
	REPLACE_S(strRez, _T("\x2"), _T("\\\\"));

	REPLACE_S(strRez, _T("\x1"), _T("\'"));


	return strRez;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditFileChangesMonitor
//
CXTPSyntaxEditFileChangesMonitor::CXTPSyntaxEditFileChangesMonitor()
{
	m_pThread = NULL;
	m_evExitThread = NULL;
	m_pConfigMgr = NULL;
}

CXTPSyntaxEditFileChangesMonitor::~CXTPSyntaxEditFileChangesMonitor()
{
	StopMonitoring();
}

void CXTPSyntaxEditFileChangesMonitor::SetDefaultFolder(const CString& strPath)
{
	m_strPath = strPath;
}

void CXTPSyntaxEditFileChangesMonitor::RemoveAll()
{
	m_arFiles.RemoveAll();
}

BOOL CXTPSyntaxEditFileChangesMonitor::AddMonitorFile(CString& strFilename,
													 DWORD dwOwnerFlags)
{
	// try open file
	BY_HANDLE_FILE_INFORMATION sysFileInfo;

	BOOL bRes = GetFileInfo(strFilename, &sysFileInfo);
	if (!bRes)
	{
		// try again in the default folder
		CString strFullFilename = m_strPath + strFilename;
		bRes = GetFileInfo(strFullFilename, &sysFileInfo);
		if (bRes)
		{
			strFilename = strFullFilename;
		}
	}

	// get complete file name
	if (bRes)
	{
		// get the directory for the file
		TCHAR szPath[_MAX_PATH];
		LPTSTR lpszName;
		if (::GetFullPathName(strFilename, _MAX_PATH, szPath, &lpszName))
		{
			strFilename = szPath;
		}
	}

	// Prepare file info structure
	CFMFileInfo fmFileInfo;

	fmFileInfo.m_strFileName = strFilename;

	fmFileInfo.m_bExists = bRes;
	fmFileInfo.m_dwOwnerFlags = dwOwnerFlags;

	if (bRes)
	{
		fmFileInfo.m_sysFileInfo = sysFileInfo;
	}

	// add file to the monitoring array
	m_arFiles.Add(fmFileInfo);

	return bRes;
}

void CXTPSyntaxEditFileChangesMonitor::StartMonitoring()
	{
	if (m_pThread)
	{
		ASSERT(FALSE);
		return;
	}

	ASSERT(m_evExitThread == NULL);
	m_evExitThread = CreateEvent(NULL,TRUE,FALSE,NULL);
	ASSERT(m_evExitThread);

	m_pThread = AfxBeginThread(ThreadMonitorProc, (LPVOID)this,
								THREAD_PRIORITY_LOWEST);
								//, 0, CREATE_SUSPENDED, NULL);

	if (!m_pThread)
	{
		ASSERT(FALSE);
		return;
	}
}

void CXTPSyntaxEditFileChangesMonitor::StopMonitoring()
{
	if (m_pThread)
	{
		HANDLE hThread = m_pThread->m_hThread;
		::SetEvent(m_evExitThread);

		DWORD dwThreadRes = ::WaitForSingleObject(hThread, 20*1000);

		if (dwThreadRes == WAIT_TIMEOUT)
		{
			::TerminateThread(hThread, 0);
			//ASSERT(FALSE);
			TRACE(_T("ERROR! FileChangesMonitor thread was not ended by normal way. It was terminated. \n"));
		}
		::CloseHandle(m_evExitThread);

		m_evExitThread = NULL;
		m_pThread = NULL;
	}
	RemoveAll();
}

UINT CXTPSyntaxEditFileChangesMonitor::ThreadMonitorProc(LPVOID p)
{
	try
	{
		CXTPSyntaxEditFileChangesMonitor* pFolderMonitor = (CXTPSyntaxEditFileChangesMonitor*)p;
		if (!pFolderMonitor)
			return 1;

		for (;;)
		{
			// Wait for notification.
			DWORD dwWaitStatus = WaitForSingleObject(pFolderMonitor->m_evExitThread, 3000);

			if (WAIT_OBJECT_0 == dwWaitStatus)
			{
				break;
			}

			if (WAIT_TIMEOUT == dwWaitStatus)
			{
				pFolderMonitor->RefreshFiles();
			}
		}
	}
	catch(...) {
		TRACE(_T("EXCEPTION in CXTPSyntaxEditFileChangesMonitor::ThreadMonitorProc\n"));
	}

	return 0;
}

void CXTPSyntaxEditFileChangesMonitor::RefreshFiles()
{
	// Iterate all stored filenames
	int nCount = (int)m_arFiles.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		//CString strFilename;
		CFMFileInfo& rFMFileInfo = m_arFiles.ElementAt(i);

		// get new info for this file
		BY_HANDLE_FILE_INFORMATION sysFileInfo;

		if (GetFileInfo(rFMFileInfo.m_strFileName, &sysFileInfo) )
		{
			// check whether file status is changed
			if (!AfxCompareTime(sysFileInfo.ftLastWriteTime,
								rFMFileInfo.m_sysFileInfo.ftLastWriteTime) ||
				sysFileInfo.nFileSizeLow != rFMFileInfo.m_sysFileInfo.nFileSizeLow ||
				sysFileInfo.nFileSizeHigh != rFMFileInfo.m_sysFileInfo.nFileSizeHigh
				|| !rFMFileInfo.m_bExists)
			{
				int nCfgFlags = rFMFileInfo.m_bExists ? 0 : xtpEditCfgFileAdd;

				rFMFileInfo.m_sysFileInfo = sysFileInfo;
				rFMFileInfo.m_bExists = TRUE;

				// reload this file
				m_pConfigMgr->ReloadFile(rFMFileInfo.m_strFileName,
										rFMFileInfo.m_dwOwnerFlags, nCfgFlags);
			}
		}
		else
		{
			if (rFMFileInfo.m_bExists)
			{
				rFMFileInfo.m_bExists = FALSE;

				// remove this file from the collection
				m_pConfigMgr->ReloadFile(rFMFileInfo.m_strFileName,
										 rFMFileInfo.m_dwOwnerFlags, xtpEditCfgFileRemove);
			}

			// try again in the default folder
			CString strFullFilename = m_strPath + rFMFileInfo.m_strFileName;
			if (GetFileInfo(strFullFilename, &sysFileInfo))
			{
				rFMFileInfo.m_strFileName = strFullFilename;
				i--;
			}
		}
	}
}

BOOL CXTPSyntaxEditFileChangesMonitor::GetFileInfo(LPCTSTR pcszFilePath,
											  BY_HANDLE_FILE_INFORMATION* pInfo)
{
	const DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	HANDLE hFile = ::CreateFile(pcszFilePath, GENERIC_READ, dwShareMode, NULL,
								 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	if (!pInfo)
	{
		return TRUE;
	}

	ZeroMemory(pInfo, sizeof(*pInfo));

	BOOL bRes = GetFileInformationByHandle(hFile, pInfo);

	CloseHandle(hFile);

	return bRes;
}

////////////////////////////////////////////////////////////////////////////
//class CFMFileInfo
CXTPSyntaxEditFileChangesMonitor::CFMFileInfo::CFMFileInfo()
{
	ZeroMemory(&m_sysFileInfo, sizeof(m_sysFileInfo));
	m_bExists = FALSE;
	m_dwOwnerFlags = 0;
}

const CXTPSyntaxEditFileChangesMonitor::CFMFileInfo&
			CXTPSyntaxEditFileChangesMonitor::CFMFileInfo::operator=(
					const CXTPSyntaxEditFileChangesMonitor::CFMFileInfo& rSrc)
{
	m_strFileName   = rSrc.m_strFileName;

	m_sysFileInfo   = rSrc.m_sysFileInfo;
	m_bExists       = rSrc.m_bExists;
	m_dwOwnerFlags  = rSrc.m_dwOwnerFlags;

	return *this;
}
