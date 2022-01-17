// XTPSyntaxEditLexParser.cpp : implementation file
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
#include "Common/XTPImageManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"

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

#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
//  #define DBG_TRACE_LOAD_CLASS_SCH

// Trace text blocks start,end when parser running;
//  #define DBG_TRACE_PARSE_START_STOP TRACE

	// Trace text blocks start,end when parser running;
//  #define DBG_TRACE_PARSE_RUN_BLOCKS TRACE

	// Trace updating schema mechanism when parser running;
//  #define DBG_TRACE_PARSE_RUN_UPDATE TRACE

//  #define DBG_TRACE_PARSE_RUN_RESULTS

//  #define DBG_TRACE_PARSE_TIME
//  #define DBG_TRACE_PARSE_TIME_DLG

//  #define DBG_TRACE_DRAW_BLOCKS
#endif

#ifndef DBG_TRACE_PARSE_RUN_BLOCKS
	#define DBG_TRACE_PARSE_RUN_BLOCKS
#endif
#ifndef DBG_TRACE_PARSE_RUN_UPDATE
	#define DBG_TRACE_PARSE_RUN_UPDATE
#endif
#ifndef DBG_TRACE_PARSE_START_STOP
	#define DBG_TRACE_PARSE_START_STOP
#endif

////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#define DBG_TRACE_TIME_BEGIN(nID) DWORD dbg_TT_##nID##_dwTime_0 = ::GetTickCount();

	#define DBG_TRACE_TIME_END(nID, strComment) { DWORD dwTime1 = ::GetTickCount(); \
		TRACE(_T("%s (%.3f sec) \n"), strComment, labs(dwTime1-dbg_TT_##nID##_dwTime_0)/1000.0); }

class C_DBG_Time {
	CString m_strComment;
	DWORD m_dwTime0;

public:
	C_DBG_Time(LPCTSTR lpcszComment)
	{
		m_dwTime0 = ::GetTickCount();
		m_strComment = lpcszComment;
	};
	~C_DBG_Time()
	{
		DWORD dwTime1 = ::GetTickCount();
		TRACE(_T("%s (%.3f sec) \n"), (LPCTSTR)m_strComment, labs(dwTime1-m_dwTime0)/1000.0);
	};
};
	//#define DBG_TRACE_TIME(nID, strComment) C_DBG_Time dbg_TT_##nID(strComment);
#endif

#ifndef DBG_TRACE_TIME
	#define DBG_TRACE_TIME(nID, strComment)
#endif

////////////////////////////////////////////////////////////////////////////
using namespace XTPSyntaxEditLexAnalyser;

namespace XTPSyntaxEditLexAnalyser
{
	extern CXTPSyntaxEditLexAutomatMemMan* XTPGetLexAutomatMemMan();

	extern void ConcatenateLVArrays(CXTPSyntaxEditLexVariantPtrArray* pArDest,
						CXTPSyntaxEditLexVariantPtrArray* pAr2, int nMaxCount = INT_MAX);

	extern BOOL SortTagsInLexVarArray(CXTPSyntaxEditLexVariantPtrArray& rarTags,
										BOOL bAscending, BOOL bNoCase);

	int FindStrCount(const CStringArray& rarData, LPCTSTR pcszStr, BOOL bCase = FALSE)
	{
		int nStrCount = 0;
		int nCount = (int)rarData.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			const CString& strI = rarData[i];

			if (bCase)
			{
				if (strI.Compare(pcszStr) == 0)
				{
					nStrCount++;
				}
			}
			else
			{
				if (strI.CompareNoCase(pcszStr) == 0)
				{
					nStrCount++;
				}
			}
		}
		return nStrCount;
	}

	int FindStr(const CStringArray& rarData, LPCTSTR pcszStr, BOOL bCase = FALSE)
	{
		int nCount = (int)rarData.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			const CString& strI = rarData[i];

			if (bCase)
			{
				if (strI.Compare(pcszStr) == 0)
				{
					return i;
				}
			}
			else
			{
				if (strI.CompareNoCase(pcszStr) == 0)
				{
					return i;
				}
			}
		}
		return -1;
	}

	int Find_noCase(CStringArray& rarData, LPCTSTR strData)
	{
		return FindStr(rarData, strData, FALSE);
	}

	void AddIfNeed_noCase(CStringArray& rarData, LPCTSTR strNew)
	{
		int nFIdx = Find_noCase(rarData, strNew);
		if (nFIdx < 0)
		{
			rarData.Add(strNew);
		}
	}

	void ConcatenateArrays_noCase(CStringArray& rarDest, CStringArray& rarSrc)
	{
		int nCount = (int)rarSrc.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CString strI = rarSrc[i];
			AddIfNeed_noCase(rarDest, strI);
		}

	}

	BOOL IsEventSet(HANDLE hEvent)
	{
		DWORD dwRes = ::WaitForSingleObject(hEvent, 0);
		return dwRes == WAIT_OBJECT_0;
	}

	BOOL IsMutexLocked(CMutex* pMu)
	{
		if (!pMu)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		BOOL bEntered = pMu->Lock(0);
		if (bEntered)
		{
			pMu->Unlock();
		}
		return !bEntered;
	}


	CString DBG_TraceIZone(const XTP_EDIT_LINECOL* pLCStart, const XTP_EDIT_LINECOL* pLCEnd)
	{
		CString sDBGpos, sTmp;
		if (pLCStart && !pLCEnd && pLCStart->GetXLC() == XTP_EDIT_XLC(1, 0) )
		{
			sDBGpos = _T("All:(1,0 - NULL)");
		}
		else if (pLCStart || pLCEnd)
		{
			sDBGpos = _T("Part:(");

			sTmp = _T("NULL");
			if (pLCStart)
			{
				sTmp.Format(_T("%d,%d"), pLCStart->nLine, pLCStart->nCol);
			}
			sDBGpos += sTmp + _T(" - ");

			sTmp = _T("NULL");
			if (pLCEnd)
			{
				sTmp.Format(_T("%d,%d"), pLCEnd->nLine, pLCEnd->nCol);
			}
			sDBGpos += sTmp;

			sDBGpos += _T(")");

		} else {
			sDBGpos = _T("Rest(NULL - NULL)");
		}
		return sDBGpos;
	}

	CString DBG_TraceTB_StartEndCls(CXTPSyntaxEditLexTextBlock* pTB)
	{
		if (!pTB)
		{
			return _T("?<NULL> (? - ?) - ?");
		}
		CString str;
		str.Format(_T("(%d,%d - %d,%d) - %s"), pTB->m_PosStartLC.nLine,
			pTB->m_PosStartLC.nCol, pTB->m_PosEndLC.nLine, pTB->m_PosEndLC.nCol,
			pTB->m_ptrLexClass ? (LPCTSTR)pTB->m_ptrLexClass->GetClassName() : _T("???<NULL>") );
		return str;
	}

}

//BEGIN_IMPLEMENT_XTPSINK(CXTPSyntaxEditLexParser, m_SinkMT)
//  ON_XTP_NOTIFICATION(xtpEditOnParserStarted,     OnParseEvent_NotificationHandler)
//  ON_XTP_NOTIFICATION(xtpEditOnTextBlockParsed,   OnParseEvent_NotificationHandler)
//  ON_XTP_NOTIFICATION(xtpEditOnParserEnded,       OnParseEvent_NotificationHandler)
//END_IMPLEMENT_XTPSINK

////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLexTextSchema

void CXTPSyntaxEditTextRegion::Clear()
{
	m_posStart.Clear();
	m_posEnd.Clear();
}

void CXTPSyntaxEditTextRegion::Set(const XTP_EDIT_LINECOL* pLCStart, const XTP_EDIT_LINECOL* pLCEnd)
{
	m_posStart.nLine = INT_MAX;
	m_posStart.nCol = 0;

	m_posEnd.Clear();

	if (pLCStart)
	{
		m_posStart = *pLCStart;
	}
	if (pLCEnd)
	{
		m_posEnd = *pLCEnd;
	}
}

CXTPSyntaxEditLexTokensDef::~CXTPSyntaxEditLexTokensDef()
{
}

CXTPSyntaxEditLexTokensDef::CXTPSyntaxEditLexTokensDef(const CXTPSyntaxEditLexTokensDef& rSrc)
{
	m_arTokens.Copy(rSrc.m_arTokens);
	m_arStartSeps.Copy(rSrc.m_arStartSeps);
	m_arEndSeps.Copy(rSrc.m_arEndSeps);
}

const CXTPSyntaxEditLexTokensDef& CXTPSyntaxEditLexTokensDef::operator=(const CXTPSyntaxEditLexTokensDef& rSrc)
{
	m_arTokens.RemoveAll();
	m_arStartSeps.RemoveAll();
	m_arEndSeps.RemoveAll();

	m_arTokens.Append(rSrc.m_arTokens);
	m_arStartSeps.Append(rSrc.m_arStartSeps);
	m_arEndSeps.Append(rSrc.m_arEndSeps);

	return *this;
}

CXTPSyntaxEditLexTextSchema::CXTPSyntaxEditLexTextSchema(LPCTSTR pcszSchName):
	m_evBreakParsing(FALSE, TRUE)
{
	//m_CloserManager.SetParentObject(this);

	m_pClassSchema = new CXTPSyntaxEditLexClassSchema();
	m_pConnectMT = new CXTPNotifyConnectionMT();

	m_nNoEndedClassesCount = 0;

	m_curInvalidZone.Clear();

	m_mapLastParsedBlocks.InitHashTable(101);

	m_nSeekNext_TagWaitChars = 0;

	m_strSchName = pcszSchName;

	m_bSendProgressEvents = FALSE;
}

CXTPSyntaxEditLexTextSchema::~CXTPSyntaxEditLexTextSchema()
{
	Close();

	CMDTARGET_RELEASE(m_pClassSchema);
	CMDTARGET_RELEASE(m_pConnectMT);
}

CString CXTPSyntaxEditLexTextSchema::GetSchName() const
{
	return m_strSchName;
}

//CXTPSyntaxEditLexTextSchemaCloserManPtr CXTPSyntaxEditLexTextSchema::GetCloserManager()
//{
//  CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
//  CSingleLock singleLock(GetDataLoker(), TRUE);
//
//  ASSERT(m_CloserManager.m_ptrParentObj || m_CloserManager.m_dwRef == 0);
//  ASSERT(this == (CXTPSyntaxEditLexTextSchema*)m_CloserManager.m_ptrParentObj || !m_CloserManager.m_ptrParentObj);
//
//  m_CloserManager.SetParentObject(this);
//
//  CXTPSyntaxEditLexTextSchemaCloserManPtr ptrRes(&m_CloserManager, TRUE);
//  return ptrRes;
//}

CXTPNotifyConnection* CXTPSyntaxEditLexTextSchema::GetConnection()
{
	return m_pConnectMT;
}

CXTPSyntaxEditLexClassSchema* CXTPSyntaxEditLexTextSchema::GetClassSchema()
{
	return m_pClassSchema;
}

CXTPSyntaxEditLexTextSchema* CXTPSyntaxEditLexTextSchema::Clone()
{
	CXTPSyntaxEditLexTextSchema* ptrNewSch = new CXTPSyntaxEditLexTextSchema(m_strSchName);
	if (!ptrNewSch)
	{
		return NULL;
	}

	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (!m_pClassSchema->Copy(ptrNewSch->m_pClassSchema))
	{
		ptrNewSch->InternalRelease();
		return NULL;
	}

	return ptrNewSch;
}

void CXTPSyntaxEditLexTextSchema::Close()
{
	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
	CSingleLock singleLock(GetDataLoker(), TRUE);

	RemoveAll();
	m_pClassSchema->Close();
}

void CXTPSyntaxEditLexTextSchema::RemoveAll()
{
	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
	CSingleLock singleLock(GetDataLoker(), TRUE);

	Close(m_ptrFirstBlock);

	m_ptrFirstBlock = NULL;

	m_mapLastParsedBlocks.RemoveAll();
	m_ptrLastParsedBlock = NULL;
}

void CXTPSyntaxEditLexTextSchema::Close(CXTPSyntaxEditLexTextBlock* pFirst)
{
	CXTPSyntaxEditLexTextBlockPtr ptrChTB(pFirst, TRUE);

	while (ptrChTB)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrChTBnext = ptrChTB->m_ptrNext;

		ptrChTB->Close();

		ptrChTB = ptrChTBnext;
	}
}

BOOL CXTPSyntaxEditLexTextSchema::IsBlockStartStillHere(CTextIter* pTxtIter, CXTPSyntaxEditLexTextBlock* pTB)
{
	if (!pTxtIter || !pTB || !pTB->m_ptrLexClass)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!pTxtIter->SeekPos(pTB->m_PosStartLC, m_evBreakParsing))
	{
		return FALSE;
	}

	CXTPSyntaxEditLexTextBlockPtr ptrTBtmp;

	int nPres = pTB->m_ptrLexClass->RunParse(pTxtIter, this, ptrTBtmp);

	BOOL bStarted = (nPres & xtpEditLPR_StartFound) != 0;

	return bStarted;
}

// DEBUG ////////////////////////////////////////////////////////////////
#ifdef DBG_TRACE_PARSE_RUN_RESULTS
	#define DBG_TRACE_PARSE_RUN_RESULTS_PROC(bFull) TraceTxtBlocks(bFull);
#else
	#define DBG_TRACE_PARSE_RUN_RESULTS_PROC(bFull)
#endif
// END DEBUG ////////////////////////////////////////////////////////////

void CXTPSyntaxEditLexTextSchema::TraceTxtBlocks(BOOL bFull)
{
	TRACE(_T("\n*** DBG_TRACE_PARSE_RUN_RESULTS *** --( %s )---------\n"),
				bFull ? _T("FULL") : _T("updated part") );

	CXTPSyntaxEditLexTextBlockPtr ptrTB = bFull ? m_ptrFirstBlock : m_ptrNewChainTB1;

	for (int i = 0; ptrTB && (bFull || ptrTB->m_ptrPrev != m_ptrNewChainTB2); i++)
	{
		TRACE(_T("(%05d) startPos=(%d,%d) endPos=(%d,%d), [%s]\n"), i,
			ptrTB->m_PosStartLC.nLine, ptrTB->m_PosStartLC.nCol,
			ptrTB->m_PosEndLC.nLine, ptrTB->m_PosEndLC.nCol,
			(LPCTSTR)ptrTB->m_ptrLexClass->m_strClassName);

		ptrTB = ptrTB->m_ptrNext;
	}

}

int CXTPSyntaxEditLexTextSchema::RunParseUpdate(BOOL bShort, CTextIter* pTxtIter,
									  const XTP_EDIT_LINECOL* pLCStart,
									  const XTP_EDIT_LINECOL* pLCEnd,
									  BOOL bSendProgressEvents)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);
	m_mapLastParsedBlocks.RemoveAll();
	singleLock.Unlock();

	if (bSendProgressEvents)
	{
		m_pConnectMT->PostEvent(xtpEditOnParserStarted, 0, 0,
								 xtpNotifyGuarantyPost | xtpNotifyDirectCallForOneThread);
	}

	int nParseRes = Run_ParseUpdate0(bShort, pTxtIter, pLCStart, pLCEnd, bSendProgressEvents);

	if (bSendProgressEvents)
	{
		m_pConnectMT->PostEvent(xtpEditOnParserEnded, nParseRes, 0,
								 xtpNotifyGuarantyPost|xtpNotifyDirectCallForOneThread);
	}

	return nParseRes;
}

int CXTPSyntaxEditLexTextSchema::Run_ParseUpdate0(BOOL bShort, CTextIter* pTxtIter,
										const XTP_EDIT_LINECOL* pLCStart,
										const XTP_EDIT_LINECOL* pLCEnd,
										BOOL bSendProgressEvents)
{
	if (!pTxtIter)
	{
		ASSERT(FALSE);
		return xtpEditLPR_Error;
	}
	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);

	//-------------------------
	m_curInvalidZone.Set(pLCStart, pLCEnd);

	m_ptrNewChainTB1 = NULL;
	m_ptrNewChainTB2 = NULL;
	m_ptrOldChainTBFirst = NULL;
	m_nNoEndedClassesCount = 0;

	m_bSendProgressEvents = bSendProgressEvents;

	int nParseRes = 0;
	//** (1) ** -------------------------
	CXTPSyntaxEditLexTextBlockPtr ptrStartTB = FindNearestTextBlock(m_curInvalidZone.m_posStart);
	if (ptrStartTB)
	{
		nParseRes = Run_ClassesUpdate1(pTxtIter, ptrStartTB, FALSE);
		if (nParseRes != -1)
		{
			if (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished))
			{
				DBG_TRACE_PARSE_RUN_RESULTS_PROC(TRUE);
				DBG_TRACE_PARSE_RUN_RESULTS_PROC(FALSE);

				if (m_ptrNewChainTB1)
				{
					BOOL bByBreak = (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked)) != 0;
					FinishNewChain(bByBreak, pTxtIter->IsEOF());
				}
				return nParseRes;
			}
		}
	}

	if (!pLCStart || *pLCStart == XTP_EDIT_LINECOL::Pos1 ||
		nParseRes == -1 || !ptrStartTB )
	{
		nParseRes = 0;
		m_ptrOldChainTBFirst = m_ptrFirstBlock ? m_ptrFirstBlock->m_ptrNext : NULL;

		//** (2) ** -------------------------
		CXTPSyntaxEditLexClassPtrArray* ptrArClasses = m_pClassSchema->GetClasses(bShort);
		int nCCount = ptrArClasses ? (int)ptrArClasses->GetSize() : 0;
		CXTPSyntaxEditLexClassPtr ptrTopClass = nCCount ? ptrArClasses->GetAt(0) : NULL;
		BOOL bRunEOF = TRUE;

		while (nCCount && (!pTxtIter->IsEOF() || bRunEOF) )
		{
			bRunEOF = !pTxtIter->IsEOF();

			nParseRes = Run_ClassesUpdate2(pTxtIter, ptrArClasses, m_ptrFirstBlock);

			if (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished))
			{
				DBG_TRACE_PARSE_RUN_RESULTS_PROC(TRUE);
				DBG_TRACE_PARSE_RUN_RESULTS_PROC(FALSE);

				if (m_ptrNewChainTB1)
				{
					BOOL bByBreak = (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked)) != 0;
					FinishNewChain(bByBreak, pTxtIter->IsEOF());
				}
				return nParseRes;
			}

			//** -------------------------------------------------------------
			if (!pTxtIter->IsEOF() && nCCount && !(nParseRes & xtpEditLPR_Iterated))
			{
				if (IsEventSet(m_evBreakParsing))
				{
					return xtpEditLPR_RunBreaked;
				}

				SeekNextEx(pTxtIter, ptrTopClass);
			}
		}
	}

	DBG_TRACE_PARSE_RUN_RESULTS_PROC(TRUE);
	DBG_TRACE_PARSE_RUN_RESULTS_PROC(FALSE);

	if (m_ptrNewChainTB1)
	{
		BOOL bByBreak = (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked)) != 0;
		FinishNewChain(bByBreak, pTxtIter->IsEOF());
	}

	return xtpEditLPR_RunFinished;
}

int CXTPSyntaxEditLexTextSchema::Run_ClassesUpdate1(CTextIter* pTxtIter,
										  CXTPSyntaxEditLexTextBlockPtr ptrStartTB,
										  BOOL bStarted)
{
	if (!pTxtIter || !ptrStartTB || !ptrStartTB->m_ptrLexClass)
	{
		return xtpEditLPR_Error;
	}

	BOOL bIterated = FALSE;

	int nPres = 0;

	BOOL bRunEOF = TRUE;
	BOOL bEnded = FALSE;
	BOOL bStarted1 = bStarted;

	if (!bStarted)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrTB = ptrStartTB;
		XTP_EDIT_LINECOL posStart = ptrStartTB->m_PosStartLC;

		bStarted1 = IsBlockStartStillHere(pTxtIter, ptrTB);

		while (!bStarted1 && ptrTB->m_ptrPrev)
		{
			ptrTB = ptrTB->m_ptrPrev;

			posStart = ptrTB->m_PosStartLC;

			bStarted1 = IsBlockStartStillHere(pTxtIter, ptrTB);

			if (IsEventSet(m_evBreakParsing))
			{
				return xtpEditLPR_RunBreaked;
			}
		}

		if (!bStarted1)
		{
			return -1; // Full reparse
		}

		if (m_curInvalidZone.m_posEnd.IsValidData() &&
			ptrTB->m_PosEndLC > m_curInvalidZone.m_posEnd)
		{
			if (ptrTB->m_ptrParent || !ptrTB->m_ptrNext)
			{
				m_curInvalidZone.m_posEnd = ptrTB->m_PosEndLC;
			}
			else
			{ // level: file
				ASSERT(ptrTB->m_ptrNext);
				if (ptrTB->m_ptrNext->m_PosEndLC > m_curInvalidZone.m_posEnd)
				{
					m_curInvalidZone.m_posEnd = ptrTB->m_ptrNext->m_PosEndLC;
				}
			}
		}
		m_curInvalidZone.m_posStart = posStart;

		DBG_TRACE_PARSE_RUN_BLOCKS(_T("\nREPARSE will start from pos =(%d,%d), Run class [%s] {%d}-noEndedStack \n"),
			posStart.nLine, posStart.nCol,
			ptrTB->m_ptrParent ?
				(ptrTB->m_ptrParent->m_ptrLexClass ?
					ptrTB->m_ptrParent->m_ptrLexClass->m_strClassName : _T("?<NULL> (parent)") )
				:( ptrTB->m_ptrLexClass ?
						ptrTB->m_ptrLexClass->m_strClassName : _T("?<NULL>") )
			, m_nNoEndedClassesCount );

		CString sDBGzone = DBG_TraceIZone(&m_curInvalidZone.m_posStart, &m_curInvalidZone.m_posEnd);
		DBG_TRACE_PARSE_RUN_UPDATE(_T("- Parser change invalid Zone [ %s ] \n"), sDBGzone);

		// Seek iterator to begin of the block
		if (!pTxtIter->SeekPos(posStart, m_evBreakParsing))
		{
			return xtpEditLPR_Error;
		}

		if (IsEventSet(m_evBreakParsing))
		{
			return xtpEditLPR_RunBreaked;
		}

		ASSERT(!m_ptrNewChainTB1);

		m_ptrNewChainTB1 = ptrTB;

		if (ptrTB->m_ptrPrev)
		{
			m_ptrNewChainTB1 = ptrTB->m_ptrPrev;
		}
		m_ptrOldChainTBFirst = m_ptrNewChainTB1->m_ptrNext;

		DBG_TRACE_PARSE_RUN_UPDATE(_T("    NewChainTB1: %s \n"), DBG_TraceTB_StartEndCls(m_ptrNewChainTB1));
		DBG_TRACE_PARSE_RUN_UPDATE(_T("    OldChainTBFirst: %s \n"), DBG_TraceTB_StartEndCls(m_ptrOldChainTBFirst));

		ptrStartTB = ptrTB;
	}

	CXTPSyntaxEditLexClassPtr ptrRunClass = ptrStartTB->m_ptrLexClass;

	//** 1 **// Run existing block with children until block end
	while (bStarted && !bEnded && (bRunEOF || !pTxtIter->IsEOF()) )
	{
		if (IsEventSet(m_evBreakParsing))
		{
			return xtpEditLPR_RunBreaked;
		}

		bRunEOF = !pTxtIter->IsEOF();

		BOOL bSkipIterate = FALSE;

		nPres = ptrRunClass->RunParse(pTxtIter, this, ptrStartTB);

		if (nPres & (xtpEditLPR_Error|/*xtpEditLPR_RunBreaked|*/xtpEditLPR_RunFinished))
		{
			return nPres;
		}

		//---------------------------------------------------------------------------
		if (nPres & xtpEditLPR_Iterated)
		{
			bSkipIterate = TRUE;
			bIterated = TRUE;
		}

		//---------------------------------------------------------------------------
		bEnded = (nPres & xtpEditLPR_EndFound) != 0;

		if (bEnded)
		{
			CSingleLock singleLock(GetDataLoker(), TRUE);

			m_nNoEndedClassesCount--;

			int nECount = ptrStartTB->EndChildren(this);
			m_nNoEndedClassesCount -= nECount;

			//*** VALIDATION (when reparsing)
			if (m_ptrNewChainTB2 && m_ptrNewChainTB2->m_ptrNext)
			{
				while (m_ptrNewChainTB2->m_ptrNext &&
					m_ptrNewChainTB2->m_ptrNext->m_PosStartLC < ptrStartTB->m_PosEndLC)
				{
					m_ptrNewChainTB2->m_ptrNext = m_ptrNewChainTB2->m_ptrNext->m_ptrNext;
				}
			}

			DBG_TRACE_PARSE_RUN_BLOCKS(_T("(%08x) ENDED startPos=(%d,%d) endPos=(%d,%d), [%s] {%d}-noEndedStack \n"),
				(CXTPSyntaxEditLexTextBlock*)ptrStartTB,
				ptrStartTB->m_PosStartLC.nLine, ptrStartTB->m_PosStartLC.nCol, ptrStartTB->m_PosEndLC.nLine,
				ptrStartTB->m_PosEndLC.nCol, ptrStartTB->m_ptrLexClass->m_strClassName, m_nNoEndedClassesCount);

			SendEvent_OnTextBlockParsed(ptrStartTB);
		}

		//---------------------------------------------------------------------------
		if (!bEnded && !bSkipIterate)
		{
			if (IsEventSet(m_evBreakParsing))
			{
				return xtpEditLPR_RunBreaked;
			}
			SeekNextEx(pTxtIter, ptrRunClass);
		}
	}
	//** end run existing **//
	if (bStarted && !bEnded && pTxtIter->IsEOF())
	{
		CXTPSyntaxEditLexTextBlock* pTB2end = ptrStartTB;
		for (; pTB2end; pTB2end = pTB2end->m_ptrParent)
		{
			pTB2end->m_PosEndLC = pTxtIter->GetPosLC();
		}
		return xtpEditLPR_RunFinished;
	}

	if (bStarted && bEnded || pTxtIter->IsEOF())
	{
		return xtpEditLPR_RunFinished;
	}

	//===========================================================================
	CXTPSyntaxEditLexTextBlockPtr ptrTBrun = ptrStartTB;
	if (ptrStartTB->m_ptrParent)
	{
		ptrTBrun = ptrStartTB->m_ptrParent;
		bEnded = FALSE;

		if (ptrTBrun->m_PosEndLC >= m_curInvalidZone.m_posStart &&
			ptrTBrun->m_PosEndLC <= m_curInvalidZone.m_posEnd)
		{
			m_nNoEndedClassesCount++;
		}
	}

	if (!bEnded && !pTxtIter->IsEOF())
	{
		if (IsEventSet(m_evBreakParsing))
		{
			return xtpEditLPR_RunBreaked;
		}

		nPres = Run_ClassesUpdate1(pTxtIter, ptrTBrun, bStarted1);

		if (nPres & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished))
		{
			return nPres;
		}
		//---------------------------------
		if (nPres & xtpEditLPR_Iterated)
		{
			bIterated = TRUE;
		}
	}
	//---------------------------------------------------------------------------
	return (bIterated ? xtpEditLPR_Iterated : 0) | (pTxtIter->IsEOF() ? xtpEditLPR_RunFinished : 0);
}

int CXTPSyntaxEditLexTextSchema::Run_ClassesUpdate2(CTextIter* pTxtIter,
										CXTPSyntaxEditLexClassPtrArray* pArClasses,
										CXTPSyntaxEditLexTextBlockPtr ptrParentTB,
										CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt)
{
	int nCCount = (int)pArClasses->GetSize();
	BOOL bIterated = FALSE;
	int nReturn = 0;

	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexClass* pClass = pArClasses->GetAt(i, FALSE);
		ASSERT(pClass);
		CXTPSyntaxEditLexTextBlockPtr ptrTB = NULL;

		int nPres = 0;
		BOOL bStarted = FALSE;
		BOOL bEnded = FALSE;
		BOOL bRunEOF = FALSE;

		do
		{
			if (IsEventSet(m_evBreakParsing))
			{
				return xtpEditLPR_RunBreaked;
			}

			bRunEOF = !pTxtIter->IsEOF();

			BOOL bSkipIterate = FALSE;

			nPres = pClass->RunParse(pTxtIter, this, ptrTB, pOnScreenRunCnt);

			if (nPres & (xtpEditLPR_Error|/*xtpEditLPR_RunBreaked|*/xtpEditLPR_RunFinished))
			{
				return nPres;
			}

			//---------------------------------------------------------------------------
			if ((nPres & xtpEditLPR_StartFound) && ptrTB && !ptrTB->m_ptrPrev)
			{
				bStarted = TRUE;

				CSingleLock singleLock(GetDataLoker(), TRUE);

				ptrTB->m_ptrParent = ptrParentTB;
				if (ptrTB->m_ptrParent)
				{
					ptrTB->m_ptrParent->m_ptrLastChild = ptrTB;
				}

				if (!pOnScreenRunCnt)
				{
					//*** VALIDATION (when reparsing)
					if (m_ptrNewChainTB2 && m_ptrNewChainTB2->m_ptrNext)
					{
						while (m_ptrNewChainTB2->m_ptrNext &&
							m_ptrNewChainTB2->m_ptrNext->m_PosStartLC < ptrTB->m_PosStartLC)
						{
							m_ptrNewChainTB2->m_ptrNext = m_ptrNewChainTB2->m_ptrNext->m_ptrNext;
						}

						if (m_ptrNewChainTB2->m_ptrNext && m_curInvalidZone.m_posEnd.IsValidData() )
						{
							DBG_TRACE_PARSE_RUN_UPDATE(_T("\n    <VALIDATE> TB current: %s \n"), DBG_TraceTB_StartEndCls(ptrTB));
							DBG_TRACE_PARSE_RUN_UPDATE(_T("    <VALIDATE> TB next OLD: %s [NoEndedStack=%d]\n"),
								DBG_TraceTB_StartEndCls(m_ptrNewChainTB2->m_ptrNext), m_nNoEndedClassesCount);

							if (m_ptrNewChainTB2->m_ptrNext->m_PosStartLC == ptrTB->m_PosStartLC &&
								ptrTB->IsEqualLexClasses(m_ptrNewChainTB2->m_ptrNext) &&
								m_curInvalidZone.m_posEnd.IsValidData() &&
								m_ptrNewChainTB2->m_ptrNext->m_PosStartLC > m_curInvalidZone.m_posEnd)
							{
								// BREAK.
								if (m_nNoEndedClassesCount <= 0)
								{
									FinishNewChain(FALSE, pTxtIter->IsEOF());
									return xtpEditLPR_RunFinished;
								}
							}
						}
					}
					//***

					m_nNoEndedClassesCount++;
					//**************************
					if (!m_ptrNewChainTB1)
					{
						// Full reparse
						m_ptrNewChainTB1 = ptrTB;
						m_ptrNewChainTB2 = NULL;

						//ASSERT(m_ptrFirstBlock == NULL);
						m_ptrFirstBlock = ptrTB;

						DBG_TRACE_PARSE_RUN_UPDATE(_T("    NewChainTB1 (&FirstBlock): %s \n"), DBG_TraceTB_StartEndCls(m_ptrNewChainTB1));
					}
					else if (!m_ptrNewChainTB2)
					{
						m_ptrNewChainTB2 = m_ptrNewChainTB1;
					}

					if (nPres&xtpEditLPR_TBpop1)
					{
						if (m_ptrNewChainTB2)
						{
							if (m_ptrNewChainTB2->m_ptrPrev)
							{
								m_ptrNewChainTB2->m_ptrPrev->m_ptrNext = ptrTB;
							}
							ptrTB->m_ptrPrev = m_ptrNewChainTB2->m_ptrPrev;
							ptrTB->m_ptrNext = m_ptrNewChainTB2;

							m_ptrNewChainTB2->m_ptrPrev = ptrTB;
							m_ptrNewChainTB2->m_ptrParent = ptrTB;
						}
						else
						{
							m_ptrNewChainTB2 = ptrTB; //SetPrevBlock(ptrTB);
						}
					}
					else
					{
						if (m_ptrNewChainTB2)
						{
							ptrTB->m_ptrNext = m_ptrNewChainTB2->m_ptrNext;
							ptrTB->m_ptrPrev = m_ptrNewChainTB2; //GetPrevBlock(); //
							m_ptrNewChainTB2->m_ptrNext = ptrTB;
						}
						m_ptrNewChainTB2 = ptrTB; //SetPrevBlock(ptrTB);
					}
					//**************************
				}
				else
				{
					if (nPres&xtpEditLPR_TBpop1)
					{
						if (pOnScreenRunCnt->m_ptrTBLast)
						{
							if (pOnScreenRunCnt->m_ptrTBLast->m_ptrPrev)
							{
								pOnScreenRunCnt->m_ptrTBLast->m_ptrPrev->m_ptrNext = ptrTB;
							}
							ptrTB->m_ptrPrev = pOnScreenRunCnt->m_ptrTBLast->m_ptrPrev;
							ptrTB->m_ptrNext = pOnScreenRunCnt->m_ptrTBLast;

							pOnScreenRunCnt->m_ptrTBLast->m_ptrPrev = ptrTB;
							pOnScreenRunCnt->m_ptrTBLast->m_ptrParent = ptrTB;
						}
						else
						{
							pOnScreenRunCnt->m_ptrTBLast = ptrTB;
						}
					}
					else
					{
						ptrTB->m_ptrPrev = pOnScreenRunCnt->m_ptrTBLast;
						if (pOnScreenRunCnt->m_ptrTBLast)
						{
							pOnScreenRunCnt->m_ptrTBLast->m_ptrNext = ptrTB;
						}
						pOnScreenRunCnt->m_ptrTBLast = ptrTB;
					}
				}

				// DEBUG ////////////////////////////////////////////////////////////////
				DBG_TRACE_PARSE_RUN_BLOCKS(_T("(%08x) START startPos=(%d,%d), ______________ [%s] {%d}-noEndedStack \n"),
					(CXTPSyntaxEditLexTextBlock*)ptrTB,
					ptrTB->m_PosStartLC.nLine, ptrTB->m_PosStartLC.nCol,
					ptrTB->m_ptrLexClass->m_strClassName, m_nNoEndedClassesCount);
				// END DEBUG ////////////////////////////////////////////////////////////
			}

			//---------------------------------------------------------------------------
			if (nPres & xtpEditLPR_Iterated)
			{
				bSkipIterate = TRUE;
				bIterated = TRUE;
			}

			//---------------------------------------------------------------------------
			bEnded = (nPres & xtpEditLPR_EndFound) != 0;

			if (bEnded)
			{
				CSingleLock singleLock(GetDataLoker(), TRUE);

				m_nNoEndedClassesCount--;

				int nECount = ptrTB->EndChildren(pOnScreenRunCnt ? NULL : this);
				m_nNoEndedClassesCount -= nECount;

				// DEBUG ////////////////////////////////////////////////////////////////
				DBG_TRACE_PARSE_RUN_BLOCKS(_T("(%08x) ENDED startPos=(%d,%d) endPos=(%d,%d), [%s] {%d}-noEndedStack \n"),
					(CXTPSyntaxEditLexTextBlock*)ptrTB,
					ptrTB->m_PosStartLC.nLine, ptrTB->m_PosStartLC.nCol,
					ptrTB->m_PosEndLC.nLine, ptrTB->m_PosEndLC.nCol,
					ptrTB->m_ptrLexClass->m_strClassName, m_nNoEndedClassesCount);
				// END DEBUG ////////////////////////////////////////////////////////////

				if (!pOnScreenRunCnt)
				{
					SendEvent_OnTextBlockParsed(ptrTB);
				}
			}
			if (nPres & xtpEditLPR_RunBreaked)
			{
				return nPres;
			}

			//---------------------------------------------------------------------------
			if (bStarted && !bEnded && !bSkipIterate)
			{
				if (IsEventSet(m_evBreakParsing))
				{
					return xtpEditLPR_RunBreaked;
				}

				SeekNextEx(pTxtIter, pClass, pOnScreenRunCnt);
				bIterated = TRUE;
			}

			//---------------------------------------------------------------------------
			if (pOnScreenRunCnt)
			{
				XTP_EDIT_LINECOL lcTextPos = pTxtIter->GetPosLC();
				if (lcTextPos.nLine > pOnScreenRunCnt->m_nRowEnd)
				{
					return xtpEditLPR_RunFinished;
				}
			}
		}
		while (bStarted && !bEnded && (bRunEOF || !pTxtIter->IsEOF()) );

		if (bEnded && pClass->IsRestartRunLoop())
		{
			nReturn |= xtpEditLPR_RunRestart;
			break;
		}
	}

	return nReturn | (bIterated ? xtpEditLPR_Iterated : 0);
}

UINT CXTPSyntaxEditLexTextSchema::SendEvent_OnTextBlockParsed(CXTPSyntaxEditLexTextBlock* pTB)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (!m_bSendProgressEvents)
	{
		return 0;
	}

	CXTPSyntaxEditLexTextBlockPtr ptrTB(pTB, TRUE);
	WPARAM dwTBid = (WPARAM)pTB;

	m_mapLastParsedBlocks[dwTBid] = ptrTB;
	m_ptrLastParsedBlock.SetPtr(pTB, TRUE);

	BOOL bIsSubscribers = m_pConnectMT->PostEvent(xtpEditOnTextBlockParsed, dwTBid,
													0, xtpNotifyDirectCallForOneThread);
	if (bIsSubscribers)
	{
		// WARNING: Why? EventsWnd must call GetLastParsedBlock()
		// to withdraw objects from the map!
		ASSERT(m_mapLastParsedBlocks.GetCount() < 10*1000);
	}
	else
	{
		m_mapLastParsedBlocks.RemoveAll();
	}

	return 0;
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::GetLastParsedBlock(WPARAM dwID)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (dwID)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrTB;
		if (m_mapLastParsedBlocks.Lookup(dwID, ptrTB))
		{
			m_mapLastParsedBlocks.RemoveKey(dwID);

			return ptrTB.Detach();
		}
	}
	else
	{
		return m_ptrLastParsedBlock.GetInterface(TRUE);
	}

//  ASSERT(FALSE);
	return NULL;
}

void CXTPSyntaxEditLexTextSchema::FinishNewChain(BOOL bByBreak, BOOL bEOF)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	CXTPSyntaxEditLexTextBlockPtr ptrTBOldLast = NULL;

	DBG_TRACE_PARSE_RUN_UPDATE(_T("    <F> NewChainTB1: %s \n"), DBG_TraceTB_StartEndCls(m_ptrNewChainTB1));

	if (m_ptrNewChainTB2)
	{
		DBG_TRACE_PARSE_RUN_UPDATE(_T("    <F> NewChainTB2-raw: %s \n"), DBG_TraceTB_StartEndCls(m_ptrNewChainTB2));
		if (bByBreak && !bEOF)
		{
			// roll back no-ended blocks
			while ( !m_ptrNewChainTB2->m_PosEndLC.IsValidData() &&
					m_ptrNewChainTB2->m_ptrPrev &&
					m_ptrNewChainTB2 != m_ptrNewChainTB1)
			{
				CXTPSyntaxEditLexTextBlockPtr ptrTBclose = m_ptrNewChainTB2;
				m_ptrNewChainTB2 = m_ptrNewChainTB2->m_ptrPrev;
				m_ptrNewChainTB2->m_ptrNext = ptrTBclose->m_ptrNext;

				DBG_TRACE_PARSE_RUN_UPDATE(_T("       <F> CLOSE TB-RollBacked: %s \n"), DBG_TraceTB_StartEndCls(ptrTBclose));
				ptrTBclose->Close();
			}
			DBG_TRACE_PARSE_RUN_UPDATE(_T("    <F> NewChainTB2-RollBacked: %s \n"), DBG_TraceTB_StartEndCls(m_ptrNewChainTB2));
		}

		// end no-ended blocks
		EndBlocksByParent(m_ptrNewChainTB1, m_ptrNewChainTB2);

		if (m_ptrNewChainTB2 == m_ptrNewChainTB1)
		{
			UpdateLastSchBlock(m_ptrNewChainTB2);
			m_ptrOldChainTBFirst = NULL;
			return;
		}

		// synchronize new and old chains
		while (m_ptrNewChainTB2->m_ptrNext &&
			(m_ptrNewChainTB2->m_ptrNext->m_PosStartLC < m_ptrNewChainTB2->m_PosStartLC ||
			 m_ptrNewChainTB2->m_ptrNext->m_PosStartLC == m_ptrNewChainTB2->m_PosStartLC &&
			 m_ptrNewChainTB2->m_ptrNext->IsEqualLexClasses(m_ptrNewChainTB2) )
			)
		{
			m_ptrNewChainTB2->m_ptrNext = m_ptrNewChainTB2->m_ptrNext->m_ptrNext;
		}

		if (m_ptrNewChainTB2->m_ptrNext)
		{
			m_ptrNewChainTB2->m_ptrNext->m_ptrPrev = m_ptrNewChainTB2;

			m_curInvalidZone.m_posEnd = m_ptrNewChainTB2->m_ptrNext->m_PosStartLC;
		}
		else
		{
			m_curInvalidZone.m_posEnd = m_ptrNewChainTB2->m_PosStartLC;
		}
		if (!bEOF)
		{
			ptrTBOldLast = m_ptrNewChainTB2->m_ptrNext;
		}
	}
	else if (bByBreak)
	{
		if (m_ptrNewChainTB2)
		{
			UpdateLastSchBlock(m_ptrNewChainTB2);
		}

		m_ptrOldChainTBFirst = NULL;
		m_ptrNewChainTB1 = NULL;
		m_ptrNewChainTB2 = NULL;
		m_nNoEndedClassesCount = 0;
		return;
	}
	else
	{
		// No new blocks found after chain branch.
		// (All rest blocks where deleted)
		if (m_ptrNewChainTB1)
		{
			m_ptrNewChainTB1->m_ptrNext = NULL;
			UpdateLastSchBlock(m_ptrNewChainTB1, TRUE);
		}
		else if (m_ptrFirstBlock)
		{
			m_ptrFirstBlock->m_ptrNext = NULL;
			UpdateLastSchBlock(m_ptrFirstBlock, TRUE);
		}
	}

	DBG_TRACE_PARSE_RUN_UPDATE(_T("    <F> OldChainTBFirst: %s \n"), DBG_TraceTB_StartEndCls(m_ptrOldChainTBFirst));
	DBG_TRACE_PARSE_RUN_UPDATE(_T("    <F> TBOldLast: %s \n"), DBG_TraceTB_StartEndCls(ptrTBOldLast));
	CXTPSyntaxEditLexTextBlockPtr ptrTBOld = m_ptrOldChainTBFirst;
	while (ptrTBOld && ptrTBOld != ptrTBOldLast)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrTBclose = ptrTBOld;
		ptrTBOld = ptrTBOld->m_ptrNext;
		ptrTBclose->Close();
	}

	if (m_ptrNewChainTB2 && m_ptrNewChainTB2->m_ptrNext &&
		m_ptrNewChainTB2->m_ptrNext->IsLookLikeClosed())
	{
		m_ptrNewChainTB2->m_ptrNext = NULL;
		UpdateLastSchBlock(m_ptrNewChainTB2, TRUE);
	}

	//===================================
	if (m_ptrNewChainTB2 && m_ptrNewChainTB1)
	{
		UpdateNewChainParentsChildren();
	}

	//===================================
	if (m_ptrNewChainTB2)
	{ // && m_ptrNewChainTB2->m_ptrNext == NULL)
		UpdateLastSchBlock(m_ptrNewChainTB2);
	}

	//-----------------------------------
	m_ptrOldChainTBFirst = NULL;
	m_ptrNewChainTB1 = NULL;
	m_ptrNewChainTB2 = NULL;
	m_nNoEndedClassesCount = 0;
}

void CXTPSyntaxEditLexTextSchema::UpdateNewChainParentsChildren()
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (!m_ptrNewChainTB2 || !m_ptrNewChainTB1)
	{
		return;
	}

	if (m_ptrNewChainTB1 != m_ptrFirstBlock)
	{
		//update children for new chain blocks
		CXTPSyntaxEditLexTextBlock* pTB_chi = m_ptrNewChainTB2->m_ptrNext;
		for (; pTB_chi; pTB_chi = pTB_chi->m_ptrNext)
		{
			if (pTB_chi->m_ptrParent && pTB_chi->m_ptrParent->IsLookLikeClosed())
			{
				CXTPSyntaxEditLexTextBlock* pTB_Par = pTB_chi->m_ptrPrev;
				for (; pTB_Par; pTB_Par = pTB_Par->m_ptrPrev)
				{
					if (pTB_Par->IsInclude(pTB_chi))
					{
						pTB_chi->m_ptrParent.SetPtr(pTB_Par, TRUE);
						ASSERT(!pTB_Par->IsLookLikeClosed());
						break;
					}
				}
			}
		}
	}
}

void CXTPSyntaxEditLexTextSchema::EndBlocksByParent(CXTPSyntaxEditLexTextBlock* pTBStart, CXTPSyntaxEditLexTextBlock* pTBEnd)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (!pTBStart)
	{
		return;
	}

	CXTPSyntaxEditLexTextBlock* pTB_chi = pTBStart;
	for (; pTB_chi != pTBEnd->m_ptrNext; pTB_chi = pTB_chi->m_ptrNext)
	{
		if (!pTB_chi->m_PosEndLC.IsValidData())
		{
			//ASSERT(pTB_chi->m_ptrParent && pTB_chi->m_ptrParent->m_PosEndLC.IsValidData());
			if (pTB_chi->m_ptrParent && pTB_chi->m_ptrParent->m_PosEndLC.IsValidData())
			{
				pTB_chi->m_PosEndLC = pTB_chi->m_ptrParent->m_PosEndLC;
			}
		}
	}
}


int CXTPSyntaxEditLexTextSchema::RunChildren(CTextIter* pTxtIter,
								CXTPSyntaxEditLexTextBlockPtr ptrTxtBlock,
								CXTPSyntaxEditLexClass* pBase,
								CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt)
{
	if (!pTxtIter || !pBase)
	{
		ASSERT(FALSE);
		return xtpEditLPR_Error;
	}
	int nRes = 0;
	CXTPSyntaxEditLexClassPtrArray* pArClasses[3];
	pArClasses[0] = pBase->GetChildren();
	pArClasses[1] = pBase->GetChildrenDyn();
	pArClasses[2] = pBase->GetChildrenSelfRef();

	for (int i = 0; i < _countof(pArClasses); i++)
	{
		if (pArClasses[i] && pArClasses[i]->GetSize())
		{
			int nResLocal = Run_ClassesUpdate2(pTxtIter, pArClasses[i],
												ptrTxtBlock, pOnScreenRunCnt);
			nRes |= nResLocal;

			if (nRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked|xtpEditLPR_RunFinished|xtpEditLPR_RunRestart))
			{
				break;
			}
		}
	}
	return nRes;
}

void CXTPSyntaxEditLexTextSchema::SeekNextEx(CTextIter* pTxtIter,
								   CXTPSyntaxEditLexClass* pRunClass,
								   CXTPSyntaxEditLexOnScreenParseCnt* pOnScreenRunCnt,
								   int nChars )
{
	if (!pTxtIter || !pRunClass)
	{
		ASSERT(FALSE);
		return;
	}

	pTxtIter->SeekNext(nChars);
	m_nSeekNext_TagWaitChars -= nChars;

	CXTPSyntaxEditLexObj_ActiveTags* pAT = m_pClassSchema->GetActiveTagsFor(pRunClass);
	if (pAT && m_nSeekNext_TagWaitChars)
	{
		int i = 0;
		//BOOL bCase = FALSE; //pRunClass->IsCaseSensitive();

		CString strTmp;
		BOOL bTag = FALSE;

		while (!bTag && !pTxtIter->IsEOF() && m_nSeekNext_TagWaitChars)
		{

			//bTag = CXTPSyntaxEditLexClass::Run_Tags1(pTxtIter, pAT, strTmp, bCase, FALSE);
			//

			bTag = pAT->FindMinWord(pTxtIter->GetText(1024), strTmp, 1024, TRUE, FALSE);

			// for test(DEBUG) Only
		#ifdef DBG_AUTOMAT
			BOOL bCase = FALSE; //pRunClass->IsCaseSensitive();
			CString strTmpX;
			BOOL bTagX = CXTPSyntaxEditLexClass::Run_Tags1(pTxtIter, pAT, strTmpX, bCase, FALSE);
			if (bTagX != bTag)
			{
				//ASSERT(FALSE);

				pAT->BuildAutomat(TRUE);
				bTag = pAT->FindMinWord(pTxtIter->GetText(1024), strTmp, 1024, TRUE, FALSE);
			}

			if (bTag)
			{
				strTmp.Empty();
				bTag = CXTPSyntaxEditLexClass::Run_Tags1(pTxtIter, pAT, strTmp, bCase, FALSE);
			}
		#endif

			if (!bTag)
			{
				pTxtIter->SeekNext(1);
				m_nSeekNext_TagWaitChars--;
			}

			if (++i%10 == 0 && IsEventSet(m_evBreakParsing))
			{
				return;
			}

			if (pOnScreenRunCnt)
			{
				XTP_EDIT_LINECOL lcTextPos = pTxtIter->GetPosLC();
				if (lcTextPos.nLine > pOnScreenRunCnt->m_nRowEnd)
				{
					return;
				}
			}
		}

		ASSERT(bTag || pTxtIter->IsEOF() || !m_nSeekNext_TagWaitChars);
		//----------------------
		if (bTag)
		{
			m_nSeekNext_TagWaitChars = (int)_tcsclen(strTmp);
			ASSERT(m_nSeekNext_TagWaitChars > 0);
		}

	}
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::GetPrevBlock(BOOL bWithAddRef)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	return m_ptrNewChainTB2.GetInterface(bWithAddRef);
}

void CXTPSyntaxEditLexTextSchema::UpdateLastSchBlock(CXTPSyntaxEditLexTextBlock* pLastTB, BOOL bPermanently)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (!pLastTB || !m_ptrLastSchBlock || bPermanently)
	{
		m_ptrLastSchBlock.SetPtr(pLastTB, TRUE);
		return;
	}

	if (pLastTB->m_PosStartLC >= m_ptrLastSchBlock->m_PosStartLC)
	{
		m_ptrLastSchBlock.SetPtr(pLastTB, TRUE);
	}
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::GetLastSchBlock(BOOL bWithAddRef)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	if (m_ptrLastSchBlock && !m_ptrNewChainTB2)
	{
		return m_ptrLastSchBlock.GetInterface(bWithAddRef);
	}

	if (!m_ptrLastSchBlock && m_ptrNewChainTB2)
	{
		return m_ptrNewChainTB2.GetInterface(bWithAddRef);
	}

	if (m_ptrLastSchBlock && m_ptrNewChainTB2)
	{
		if (m_ptrLastSchBlock->m_PosStartLC < m_ptrNewChainTB2->m_PosStartLC)
		{
			return m_ptrNewChainTB2.GetInterface(bWithAddRef);
		}
		else
		{
			return m_ptrLastSchBlock.GetInterface(bWithAddRef);
		}
	}
	return NULL;
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::GetNewBlock()
{
	CXTPSyntaxEditLexTextBlock* p = new CXTPSyntaxEditLexTextBlock();
	return p;
}

void CXTPSyntaxEditLexTextSchema::GetTextAttributes(XTP_EDIT_TEXTBLOCK& rTB, CXTPSyntaxEditLexTextBlock* pTextBlock)
{
	if (!pTextBlock || !pTextBlock->m_ptrLexClass)
	{
		return;
	}

	pTextBlock->m_ptrLexClass->GetTextAttributes(rTB);
}

void CXTPSyntaxEditLexTextSchema::TraceClrBlocks(CXTPSyntaxEditTextBlockArray& arBlocks)
{
	// DEBUG ////////////////////////////////////////////////////////////////
#ifdef DBG_TRACE_DRAW_BLOCKS
	TRACE(_T("\n*** bloks --------------------\n"));

	for (int i = 0; i < arBlocks.GetSize(); i++)
	{
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[i];

		TRACE(_T("(%02d) start =%d, ?end=%d, color=%X \n"), i, BlkI.nPos,
			BlkI.nNextBlockPos, BlkI.clrBlock.crText);

	}
	TRACE(_T("***\n"));
#else
	UNREFERENCED_PARAMETER(arBlocks);
#endif
	// END DEBUG ////////////////////////////////////////////////////////////
}


void CXTPSyntaxEditLexTextSchema::AddClrBlock(XTP_EDIT_TEXTBLOCK& rClrB,
									CXTPSyntaxEditTextBlockArray& arBlocks)
{
#ifdef DBG_TRACE_DRAW_BLOCKS
	TRACE(_T(" --- ADD --- start =%d, ?end=%d, color=%X \n"), rClrB.nPos,
		rClrB.nNextBlockPos, rClrB.clrBlock.crText);
#endif

	int nCount = (int)arBlocks.GetSize();
	//** A. ** new block is included in the existing block
	int i;
	for (i = 0; i < nCount; i++)
	{
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[i];
		if (rClrB.nNextBlockPos < BlkI.nPos ||
			rClrB.nPos >= BlkI.nNextBlockPos )
		{
			continue;
		}

		XTP_EDIT_TEXTBLOCK BlkI2 = BlkI;

		if (rClrB.nPos >= BlkI.nPos &&
			rClrB.nNextBlockPos <= BlkI.nNextBlockPos)
		{
			// override equal block
			if (rClrB.nPos == BlkI.nPos &&
				rClrB.nNextBlockPos == BlkI.nNextBlockPos)
			{
				arBlocks[i] = rClrB;
				return;
			}
			// override BEGIN of the existing block
			if (rClrB.nPos == BlkI.nPos &&
				rClrB.nNextBlockPos < BlkI.nNextBlockPos)
			{
				BlkI.nPos = rClrB.nNextBlockPos;
				arBlocks[i] = BlkI;
				arBlocks.InsertAt(i, rClrB);
				return;
			}
			// override END of the existing block
			if (rClrB.nPos > BlkI.nPos &&
				rClrB.nNextBlockPos == BlkI.nNextBlockPos)
			{
				BlkI.nNextBlockPos = rClrB.nPos;
				arBlocks[i] = BlkI;
				arBlocks.InsertAt(i+1, rClrB);
				return;
			}

			// INSERT inside to the existing block
			BlkI2.nPos = rClrB.nNextBlockPos;
			BlkI.nNextBlockPos = rClrB.nPos;
			arBlocks[i] = BlkI;
			arBlocks.InsertAt(i+1, BlkI2);
			arBlocks.InsertAt(i+1, rClrB);
			return;
		}
	}

	//** B. ** new block is bigger then existing block
	nCount = (int)arBlocks.GetSize();
	BOOL bInserted = FALSE;
	for (i = nCount-1; i >= 0; i--)
	{
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[i];

		if (rClrB.nNextBlockPos > BlkI.nPos && rClrB.nNextBlockPos < BlkI.nNextBlockPos)
		{
			BlkI.nPos = rClrB.nNextBlockPos;
			arBlocks[i] = BlkI;
			if (!bInserted)
			{
				arBlocks.InsertAt(i, rClrB);
				bInserted = TRUE;
			}
		} else
		if (rClrB.nPos <= BlkI.nPos && rClrB.nNextBlockPos >= BlkI.nNextBlockPos)
		{
			arBlocks.RemoveAt(i);
		} else
		if (rClrB.nNextBlockPos > BlkI.nPos && rClrB.nNextBlockPos < BlkI.nNextBlockPos )
		{
			BlkI.nNextBlockPos = rClrB.nPos;
			arBlocks[i] = BlkI;
			if (!bInserted)
			{
				arBlocks.InsertAt(i+1, rClrB);
				bInserted = TRUE;
			}
		}
	}

	//----------------
	if (!bInserted)
	{
		arBlocks.Add(rClrB);
	}
}

void CXTPSyntaxEditLexTextSchema::GetRowColors(CTextIter* pTxtIter, int nRow,
									 int nColFrom, int nColTo,
									 const XTP_EDIT_COLORVALUES& clrDefault,
									 CXTPSyntaxEditTextBlockList& rBlocks,
									 CXTPSyntaxEditLexTextBlockPtr* pptrTBStartCache,
									 CXTPSyntaxEditLexTextBlock* pFirstSchTB)

{
	if (!pTxtIter)
	{
		ASSERT(FALSE);
		return;
	}

	XTP_EDIT_TEXTBLOCK tmpBlk;
	tmpBlk.nPos = 0;
	tmpBlk.nNextBlockPos = 0;

	tmpBlk.clrBlock = clrDefault;

	int nLineLen = pTxtIter->GetLineLen(nRow, FALSE);

	if (!nLineLen)
	{
		return;
	}

	XTP_EDIT_LINECOL LCStart= {nRow, nColFrom};
	XTP_EDIT_LINECOL LCEnd = {nRow, nLineLen};
	if (nColTo > 0 && nColTo < nLineLen)
	{
		LCEnd.nCol = nColTo;
	}

	CXTPSyntaxEditTextBlockArray arBlocks;
	XTP_EDIT_TEXTBLOCK bltTB;
	bltTB.nPos = 0;

	CSingleLock singleLock(GetDataLoker());
	//BOOL bLocked = TryLockCS(&singleLock, GetDataLoker(), 30, 5);
	BOOL bLocked = singleLock.Lock(30);

	//---------------------------------------------------------------------------
	CXTPSyntaxEditLexTextBlockPtr ptrTBfirst, ptrTBstart;
	if (bLocked)
	{
		ptrTBfirst = GetBlocks();
		if (pFirstSchTB)
		{
			ptrTBfirst.SetPtr(pFirstSchTB, TRUE);
		}

		if (pptrTBStartCache)
		{
			ptrTBstart = *pptrTBStartCache;
		}

		if (!ptrTBstart || ptrTBstart == ptrTBfirst ||
			ptrTBstart->IsLookLikeClosed())
		{
			ptrTBstart = ptrTBfirst ? ptrTBfirst->m_ptrNext : NULL;
		}
	}

	//= process first blk ====================================================
	//
	if (ptrTBfirst)
	{
		bltTB.clrBlock = clrDefault;
		bltTB.lf = tmpBlk.lf;
		GetTextAttributes(bltTB, ptrTBfirst);

		bltTB.nPos = 0;
		bltTB.nNextBlockPos = nLineLen;

		AddClrBlock(bltTB, arBlocks);
		TraceClrBlocks(arBlocks);
	}
	//========================================================================
	CXTPSyntaxEditLexTextBlockPtrArray arTBStack;

	CXTPSyntaxEditLexTextBlockPtr ptrTBStartCache;

	for (CXTPSyntaxEditLexTextBlock* pTB = ptrTBstart; pTB; pTB = pTB->m_ptrNext)
	{
		//BOOL bEndValid = pTB->m_PosEndLC.IsValidData();
		//if (bEndValid &&
		//  (pTB->m_PosEndLC < LCStart || pTB->m_PosEndLC < pTB->m_PosStartLC) ||
		//  !bEndValid && pTB->m_ptrParent)
		if (pTB->GetPosEndLC() < LCStart || pTB->m_PosEndLC < pTB->m_PosStartLC)
		{
			continue;
		}

		if (LCEnd < pTB->m_PosStartLC)
		{
			break;
		}

		XTP_EDIT_LINECOL TB_PosEndLC = pTB->m_PosEndLC;
		if (!pTB->m_PosEndLC.IsValidData() && !pTB->m_ptrParent)
		{
			TB_PosEndLC.nLine = nRow;
			TB_PosEndLC.nCol = nLineLen;
		}

		if (!ptrTBStartCache)
		{
			ptrTBStartCache.SetPtr(pTB, TRUE);
			ASSERT(pTB->m_ptrParent);
		}

		CXTPSyntaxEditLexTextBlock* pTBStackLast = NULL;
		while (pTB->m_ptrNext && pTB->m_ptrNext->m_PosStartLC <= pTB->m_PosStartLC &&
			pTB->m_ptrNext->m_PosEndLC.IsValidData() && pTB->m_PosEndLC.IsValidData() &&
			pTB->m_ptrNext->m_PosEndLC > pTB->m_PosEndLC)
		{
			if (arTBStack.GetSize() == 0)
			{
				arTBStack.AddPtr(pTB, TRUE);
			}
			arTBStack.Add(pTB->m_ptrNext);
			pTBStackLast = pTB->m_ptrNext;

			pTB = pTB->m_ptrNext;
		}

		int nStackCount = (int)arTBStack.GetSize();

		for (;nStackCount >= 0;)
		{
			if (nStackCount > 0)
			{
				pTB = arTBStack[nStackCount-1];
			}
			nStackCount--;

			if (nStackCount >= 0)
			{
				arTBStack.RemoveAt(nStackCount);

				TB_PosEndLC = pTB->m_PosEndLC;
				if (!pTB->m_PosEndLC.IsValidData() /*&& !pTB->m_ptrParent*/)
				{
					TB_PosEndLC.nLine = nRow;
					TB_PosEndLC.nCol = nLineLen;
				}
			}

			//restore default values
			bltTB.clrBlock = clrDefault;
			bltTB.lf = tmpBlk.lf;

			GetTextAttributes(bltTB, pTB);

			if (pTB->m_PosStartLC > LCStart)
			{
				bltTB.nNextBlockPos = TB_PosEndLC.nLine == nRow ?
							min(TB_PosEndLC.nCol+1, nLineLen) : nLineLen;
				bltTB.nPos = pTB->m_PosStartLC.nCol;

				AddClrBlock(bltTB, arBlocks);
				TraceClrBlocks(arBlocks);
			}
			else
			{
				bltTB.nNextBlockPos = TB_PosEndLC.nLine == nRow ?
							min(TB_PosEndLC.nCol+1, nLineLen) : nLineLen;
				bltTB.nPos = 0;

				AddClrBlock(bltTB, arBlocks);
				TraceClrBlocks(arBlocks);
			}
		}

		//--------------------
		if (pTBStackLast)
		{
			pTB = pTBStackLast;
		}
	}
	//----------------------------------------
	if (pptrTBStartCache)
	{
		ptrTBstart = *pptrTBStartCache;
		if (!ptrTBstart || ptrTBstart->IsLookLikeClosed())
		{
			*pptrTBStartCache = ptrTBStartCache;
			ASSERT(!ptrTBStartCache || !ptrTBStartCache->IsLookLikeClosed());
		}
	}
	//-----------------------------
	if (bLocked)
	{
		singleLock.Unlock();
	}

	//===========================================================================
	int nPos = 0;
	int i;
	for (i = 0; i < arBlocks.GetSize(); i++)
	{
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[i];
		if (nPos < BlkI.nPos)
		{
			tmpBlk.nPos = nPos;
			tmpBlk.nNextBlockPos = BlkI.nPos;

			AddClrBlock(tmpBlk, arBlocks);
			TraceClrBlocks(arBlocks);
			i++;
		}
		nPos = BlkI.nNextBlockPos;
	}
	//===========================================================================
	if (arBlocks.GetSize() == 0)
	{
		tmpBlk.nPos = 0;
		tmpBlk.nNextBlockPos = nLineLen;
		arBlocks.Add(tmpBlk);
	}
	else
	{
		//---------------------------------------------------------------------------
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[arBlocks.GetSize()-1];
		if (BlkI.nNextBlockPos < nLineLen)
		{
			tmpBlk.nPos = BlkI.nNextBlockPos;
			tmpBlk.nNextBlockPos = nLineLen;
			AddClrBlock(tmpBlk, arBlocks);
			TraceClrBlocks(arBlocks);
		}
	}

	//**************************************************************************

	// DEBUG ////////////////////////////////////////////////////////////////
#ifdef DBG_TRACE_DRAW_BLOCKS
	TRACE(_T("\n*** DBG_TRACE_DRAW_BLOCKS *** --------------------\n"));
	TRACE(_T("row=%d, row_len = %d \n"), nRow, nLineLen);
#endif
	// END DEBUG ////////////////////////////////////////////////////////////

	for (i = 0; i < arBlocks.GetSize(); i++)
	{
		XTP_EDIT_TEXTBLOCK BlkI = arBlocks[i];

		rBlocks.AddTail(BlkI);

		// DEBUG ////////////////////////////////////////////////////////////////
#ifdef DBG_TRACE_DRAW_BLOCKS
		TRACE(_T("(%02d) start =%d, ?end=%d, color=%X \n"), i, BlkI.nPos,
			BlkI.nNextBlockPos, BlkI.clrBlock.crText);
#endif
		// END DEBUG ////////////////////////////////////////////////////////////
	}
}

void CXTPSyntaxEditLexTextSchema::GetCollapsableBlocksInfo(int nRow,
												CXTPSyntaxEditRowsBlockArray& rArBlocks,
												CXTPSyntaxEditLexTextBlockPtr* pptrTBStartCache)
{
	static const CString s_strCollapsedText_def = _T("[..]");

	rArBlocks.RemoveAll();

	CSingleLock singleLock(GetDataLoker());
	//BOOL bLocked = TryLockCS(&singleLock, GetDataLoker(), 30);
	BOOL bLocked = singleLock.Lock(30);
	if (!bLocked)
	{
		return;
	}

	XTP_EDIT_ROWSBLOCK tmpCoBlk;
	//---------------------------------------------------------------------------
	ASSERT(bLocked);
	CXTPSyntaxEditLexTextBlockPtr ptrTBstart;
	if (pptrTBStartCache)
	{
		ptrTBstart = *pptrTBStartCache;
	}
	if (!ptrTBstart || ptrTBstart->IsLookLikeClosed())
	{
		ptrTBstart = GetBlocks();
	}
	CXTPSyntaxEditLexTextBlockPtr ptrTBStartCache;

	for (CXTPSyntaxEditLexTextBlock* pTB = ptrTBstart; pTB; pTB = pTB->m_ptrNext)
	{
		if (nRow >= 0 && nRow < pTB->m_PosStartLC.nLine)
		{
			break;
		}

		XTP_EDIT_LINECOL TB_PosEndLC = pTB->GetPosEndLC(); // pTB->m_PosEndLC;


		if (nRow < 0 || (nRow >= pTB->m_PosStartLC.nLine &&
			nRow <= TB_PosEndLC.nLine) )
		{
			if (!ptrTBStartCache && pTB->m_ptrParent)
			{
				ptrTBStartCache.SetPtr(pTB, TRUE);
			}

			if (pTB->m_PosStartLC.nLine < TB_PosEndLC.nLine &&
				pTB->m_ptrParent )
			{
				CXTPSyntaxEditLexClass* pLexClass = pTB->m_ptrLexClass;
				ASSERT(pLexClass);

				if (pLexClass && pLexClass->IsCollapsable())
				{
					tmpCoBlk.lcStart = pTB->m_PosStartLC;
					tmpCoBlk.lcEnd = TB_PosEndLC;

					CXTPSyntaxEditLexVariantPtr ptrLVtext;
					ptrLVtext = pLexClass->GetAttribute(XTPLEX_ATTR_COLLAPSEDTEXT, FALSE);

					if (ptrLVtext && ptrLVtext->IsStrType())
					{
						tmpCoBlk.strCollapsedText = ptrLVtext->GetStr();
					} else {
						tmpCoBlk.strCollapsedText = s_strCollapsedText_def;
					}

					rArBlocks.Add(tmpCoBlk);
				}
			}
		}
	}

	//----------------------------------------
	if (pptrTBStartCache)
	{
		ptrTBstart = *pptrTBStartCache;
		if (!ptrTBstart || ptrTBstart->IsLookLikeClosed())
		{
			*pptrTBStartCache = ptrTBStartCache;
			ASSERT(!ptrTBStartCache || !ptrTBStartCache->IsLookLikeClosed());
		}
	}
}


void CXTPSyntaxEditLexTextSchema::ApplyThemeRecursive(CXTPSyntaxEditColorTheme* pTheme,
											CXTPSyntaxEditLexClassPtrArray* ptrClasses)
{
	// if pTheme == NULL - Restore default values only

	const static CString strAttrPref = XTPLEX_ATTR_TXTPREFIX;

	ASSERT(ptrClasses);
	int nCount = ptrClasses ? (int)ptrClasses->GetSize() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = ptrClasses->GetAt(i);
		if (!ptrC)
		{
			ASSERT(FALSE);
			continue;
		}
		// Traverse the tree
		ApplyThemeRecursive(pTheme, ptrC->GetChildren());
		ApplyThemeRecursive(pTheme, ptrC->GetChildrenDyn());
		//-------------------------------------------------------

		const CString strCName = ptrC->GetClassName();

		// restore default attributes (colors)
		CXTPSyntaxEditLexClassPtr ptrCDefault = m_pClassSchema->GetPreBuildClass(strCName);
		ASSERT(ptrCDefault);
		if (ptrCDefault)
		{
			ptrC->CopyAttributes(ptrCDefault, XTPLEX_ATTR_TXTPREFIX);
		}

		CXTPSyntaxEditColorInfo* pColors = pTheme ? pTheme->GetColorInfo(strCName, pTheme->GetFileName()) : NULL;
		if (!pColors)
		{
			continue;
		}

		// Apply new attributes (colors)
		POSITION posParam = pColors->GetFirstParamNamePosition();
		while (posParam)
		{
			static const int c_nClrPrefLen = (int)_tcsclen(XTPLEX_ATTR_COLORPREFIX);

			CString strParamName = pColors->GetNextParamName(posParam);
			DWORD dwValue = pColors->GetHexParam(strParamName);
			CString strAttrName = strAttrPref + strParamName;

			if (_tcsnicmp(strAttrName, XTPLEX_ATTR_COLORPREFIX, c_nClrPrefLen) == 0)
			{
				dwValue = XTP_EDIT_RGB_INT2CLR(dwValue);
			}

			CXTPSyntaxEditLexVariant lvAttr((int)dwValue);

			ptrC->SetAttribute(strAttrName, lvAttr);
		}
	}
}

void CXTPSyntaxEditLexTextSchema::ApplyTheme(CXTPSyntaxEditColorTheme* pTheme)
{
	// if pTheme == NULL - Restore default values only

	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
	CSingleLock singleLock(GetDataLoker(), TRUE);

	CXTPSyntaxEditLexClassPtrArray* ptrClassesFull = m_pClassSchema->GetClasses(FALSE);
	CXTPSyntaxEditLexClassPtrArray* ptrClassesShort = m_pClassSchema->GetClasses(TRUE);

	ApplyThemeRecursive(pTheme, ptrClassesFull);
	ApplyThemeRecursive(pTheme, ptrClassesShort);

}

////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLexClassSchema

CXTPSyntaxEditLexClassSchema::CXTPSyntaxEditLexClassSchema()
{
	XTPGetLexAutomatMemMan()->Lock();
}

CXTPSyntaxEditLexClassSchema::~CXTPSyntaxEditLexClassSchema()
{
	XTPGetLexAutomatMemMan()->Unlok();
}

void CXTPSyntaxEditLexClassSchema::AddPreBuildClass(CXTPSyntaxEditLexClass* pClass)
{
	CXTPSyntaxEditLexClassPtr ptrC(pClass, TRUE);
	m_arPreBuildClassesList.Add(ptrC);
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClassSchema::GetChildrenFor(CXTPSyntaxEditLexClass* pClass,
														 BOOL& rbSelfChild)
{
	rbSelfChild = FALSE;

	CXTPSyntaxEditLexClassPtrArray arChildren;

	BOOL bForFile = pClass == NULL;

	int nChidrenOpt = xtpEditOptChildren_Any;
	CStringArray arChidrenData;
	CString strParentName;

	if (pClass)
	{
		pClass->GetChildrenOpt(nChidrenOpt, arChidrenData);
		strParentName = pClass->GetClassName();
	}

	if (nChidrenOpt == xtpEditOptChildren_No)
	{
		return NULL;
	}

	int nCCount = (int)m_arPreBuildClassesList.GetSize();
	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = m_arPreBuildClassesList[i];
		int nParentOpt;
		CStringArray arParentData;
		ptrC->GetParentOpt(nParentOpt, arParentData);
		CString strCName = ptrC->GetClassName();

		if (bForFile && nParentOpt == xtpEditOptParent_file)
		{
			arChildren.Add(ptrC);
		}
		else if (!bForFile && nParentOpt == xtpEditOptParent_direct)
		{
			ASSERT(pClass);

			if (nChidrenOpt == xtpEditOptChildren_List)
			{
				if (Find_noCase(arChidrenData, strCName) < 0)
				{
					continue;
				}
			}

			int nNCount = (int)arParentData.GetSize();
			for (int n = 0; n < nNCount; n++)
			{
				CString strCN = arParentData[n];

				if (strCN.CompareNoCase(strParentName) == 0)
				{
					if (strCName.CompareNoCase(strParentName) == 0)
					{
						rbSelfChild = TRUE;
					}
					else
					{
						arChildren.Add(ptrC);
					}
					break;
				}
			}
		}
	}
	//--------------------------------
	CXTPSyntaxEditLexClassPtrArray* pArChildren = NULL;
	if (arChildren.GetSize())
	{
		pArChildren = new CXTPSyntaxEditLexClassPtrArray;
		if (pArChildren)
		{
			pArChildren->Append(arChildren);
		}
	}
	return pArChildren;
}

int CXTPSyntaxEditLexClassSchema::CanBeParentDynForChild(CString strParentName,
								CXTPSyntaxEditLexClass* pCChild)
{
	if (!pCChild)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	int nParentOpt;
	CStringArray arParentData;
	pCChild->GetParentOpt(nParentOpt, arParentData);
	if (nParentOpt != xtpEditOptParent_dyn)
	{
		return -1;
	}

	int nCCount = (int)m_arPreBuildClassesList.GetSize(), n;
	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = m_arPreBuildClassesList[i];

		CString strCName = ptrC->GetClassName();

		int nNCount = (int)arParentData.GetSize();
		for (n = 0; n < nNCount; n++)
		{
			CString strCN = arParentData[n];

			if (strParentName.CompareNoCase(strCN) == 0)
			{
				return TRUE;
			}
		}

		//-------------------------------------------------------
		if (nParentOpt == xtpEditOptParent_dyn )
		{
			int nDataCount = (int)arParentData.GetSize();
			for (n = 0; n < nDataCount; n++)
			{
				CString strCN = arParentData[n];

				if (strCName.CompareNoCase(strCN) == 0)
				{
					BOOL bCanDyn = CanBeParentDynForChild(strParentName, ptrC);
					if (bCanDyn)
					{
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexClassSchema::GetPreBuildClass(const CString& strName)
{
	int nCCount = (int)m_arPreBuildClassesList.GetSize();
	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = m_arPreBuildClassesList[i];

		const CString strCName = ptrC->GetClassName();

		if (strName.CompareNoCase(strCName) == 0)
		{
			return ptrC.Detach();
		}
	}
	return NULL;
}

void CXTPSyntaxEditLexClassSchema::GetDynParentsList(CXTPSyntaxEditLexClass* pClass,
										   CStringArray& rarDynParents,
										   CStringArray& rarProcessedClasses)
{
	if (!pClass)
	{
		ASSERT(FALSE);
		return;
	}

	CString strClassName = pClass->GetClassName();
	if (Find_noCase(rarProcessedClasses, strClassName) >= 0)
	{
		return;
	}
	rarProcessedClasses.Add(strClassName);

	int nParentOpt;
	CStringArray arParentData;
	pClass->GetParentOpt(nParentOpt, arParentData);

	if (nParentOpt == xtpEditOptParent_file)
	{
		return;
	}

	ConcatenateArrays_noCase(rarDynParents, arParentData);

	int nNCount = (int)arParentData.GetSize();
	for (int n = 0; n < nNCount; n++)
	{
		CString strParent1 = arParentData[n];

		if (strClassName.CompareNoCase(strParent1) == 0)
		{
			continue;
		}

		CXTPSyntaxEditLexClassPtr ptrC = GetPreBuildClass(strParent1);
		if (ptrC)
		{
			GetDynParentsList(ptrC, rarDynParents, rarProcessedClasses);
		}
		else
		{
			//TRACE
		}
	}
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClassSchema::GetDynChildrenFor(CXTPSyntaxEditLexClass* pClass,
															BOOL& rbSelfChild)
{
	rbSelfChild = FALSE;

	int nChidrenOpt = xtpEditOptChildren_Any;
	CStringArray arChidrenData;
	if (pClass)
	{
		pClass->GetChildrenOpt(nChidrenOpt, arChidrenData);
	}

	if (nChidrenOpt == xtpEditOptChildren_No)
	{
		return NULL;
	}

	CString strMainCName = pClass->GetClassName();

	CStringArray arMainParents;
	CStringArray arProcessedClasses;
	GetDynParentsList(pClass, arMainParents, arProcessedClasses);

	CXTPSyntaxEditLexClassPtrArray arDynChildren;

	int nCCount = (int)m_arPreBuildClassesList.GetSize();
	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = m_arPreBuildClassesList[i];
		CString strCName = ptrC->GetClassName();

		if (nChidrenOpt == xtpEditOptChildren_List)
		{
			if (Find_noCase(arChidrenData, strCName) < 0)
			{
				continue;
			}
		}

		int nCanRes = CanBeParentDynForChild(strMainCName, ptrC);
		if (nCanRes > 0)
		{
			if (strMainCName.CompareNoCase(strCName) == 0)
			{
				rbSelfChild = TRUE;
			}
			else
			{
				arDynChildren.Add(ptrC);
			}
			continue;
		}

		if (nCanRes < 0)
		{
			continue;
		}

		int nMPCount = (int)arMainParents.GetSize();
		for (int n = 0; n < nMPCount; n++)
		{
			CString strMPName = arMainParents[n];
			if (CanBeParentDynForChild(strMPName, ptrC))
			{
				arDynChildren.Add(ptrC);
				break;
			}
		}
	}

	//--------------------------------
	CXTPSyntaxEditLexClassPtrArray* pArDynChildren = NULL;
	if (arDynChildren.GetSize())
	{
		pArDynChildren = new CXTPSyntaxEditLexClassPtrArray;
		if (pArDynChildren)
		{
			pArDynChildren->Append(arDynChildren);
		}
	}
	return pArDynChildren;
}


CXTPSyntaxEditLexObj_ActiveTags* CXTPSyntaxEditLexClassSchema::GetActiveTagsFor(
									CXTPSyntaxEditLexClass* pTopClass)
{
	if (!pTopClass)
	{
		ASSERT(FALSE);
		return NULL;
	}
	return pTopClass->GetActiveTags();
}

BOOL CXTPSyntaxEditLexClassSchema::Copy(CXTPSyntaxEditLexClassSchema* pDest)
{
	if (!pDest)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	//------------------------------------------------------------------------
	int nCount = (int)m_arPreBuildClassesList.GetSize();
	int i;
	for (i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC0 = m_arPreBuildClassesList.GetAt(i);
		if (!ptrC0)
		{
			continue;
		}
		CXTPSyntaxEditLexClassPtr ptrC0_new = ptrC0->Clone(this);
		if (!ptrC0_new)
		{
			return FALSE;
		}
		pDest->m_arPreBuildClassesList.Add(ptrC0_new);
	}

	//------------------------------------------------------------------------
	nCount = (int)m_arClassesTreeFull.GetSize();
	for (i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrCfile = m_arClassesTreeFull.GetAt(i);
		if (!ptrCfile)
		{
			continue;
		}


		CXTPSyntaxEditLexClassPtr ptrCFnew = ptrCfile->Clone(this);

		if (!CopyChildrenFor(FALSE, ptrCFnew, ptrCfile, 0))
		{
			return FALSE;
		}

		int nStartCount = 0;
		ptrCFnew->BuildActiveTags(nStartCount);

		// //CXTPSyntaxEditLexVariantPtrArray* ptrAT = ptrCFnew->BuildActiveTags(nStartCount);
		// //ConcatenateLVArrays(&pDest->m_arAllActiveTagsFull, ptrAT, nStartCount);

		pDest->m_arClassesTreeFull.Add(ptrCFnew);
	}

	//------------------------------------------------------------------------
	BOOL bResShortTree = pDest->Build_ShortTree();

	//-* Post Build processing ----------------------------
	if (!PostBuild_Step(&pDest->m_arClassesTreeShort))
	{
		return FALSE;
	}

	if (!PostBuild_Step(&pDest->m_arClassesTreeFull))
	{
		return FALSE;
	}
	return bResShortTree;
}

BOOL CXTPSyntaxEditLexClassSchema::Build()
{
	CXTPSyntaxEditLexClass::CloseClasses(&m_arClassesTreeFull);
	CXTPSyntaxEditLexClass::CloseClasses(&m_arClassesTreeShort);
	int nClassID = 1;

	BOOL bUnused;
	CXTPSyntaxEditLexClassPtrArray* ptrArCfile = GetChildrenFor(NULL, bUnused);

	int nCount = ptrArCfile ? (int)ptrArCfile->GetSize() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrCfile = ptrArCfile->GetAt(i);

		CXTPSyntaxEditLexClassPtr ptrCFnew = GetNewClass(TRUE);
		if (!ptrCFnew)
		{
			delete ptrArCfile;
			return FALSE;
		}
		ptrCFnew->CopyFrom(ptrCfile);

		CXTPSyntaxEditLexVariant lvID(nClassID++);
		ptrCFnew->SetAttribute(XTPLEX_ATTRCLASSID, lvID);

		#ifdef DBG_TRACE_LOAD_CLASS_SCH
			ptrCFnew->Dump(_T(""));
		#endif

		CString strCNnew = ptrCFnew->GetClassName();

		CStringArray arAddedStack;
		arAddedStack.Add(strCNnew);
		if (!Build_ChildrenFor(FALSE, ptrCFnew, arAddedStack, nClassID, 0))
		{
			delete ptrArCfile;
			return FALSE;
		}

		arAddedStack.RemoveAll();
		if (!Build_ChildrenFor(TRUE, ptrCFnew, arAddedStack, nClassID, 0))
		{
			delete ptrArCfile;
			return FALSE;
		}

		int nStartCount = 0;
		ptrCFnew->BuildActiveTags(nStartCount);
		//CXTPSyntaxEditLexVariantPtrArray* ptrAT = ptrCFnew->BuildActiveTags(nStartCount);
		//ConcatenateLVArrays(&m_arAllActiveTagsFull, ptrAT, nStartCount);

		m_arClassesTreeFull.Add(ptrCFnew);
	}

	delete ptrArCfile;

	#ifdef DBG_TRACE_LOAD_CLASS_SCH
//      m_arAllActiveTagsFull.Dump(_T("AllActiveTags FULL=: "));
	#endif

	if (!Build_ShortTree())
	{
		return FALSE;
	}

	//-* Post Build processing ----------------------------
	if (!PostBuild_Step(&m_arClassesTreeShort))
	{
		return FALSE;
	}

	if (!PostBuild_Step(&m_arClassesTreeFull))
	{
		return FALSE;
	}

	//----------------------
	//BOOL bAsc = TRUE, bNoCase = FALSE;
	//SortTagsInLexVarArray(m_arAllActiveTagsFull, bAsc, bNoCase);
	//SortTagsInLexVarArray(m_arAllActiveTagsShort, bAsc, bNoCase);

//  m_arAllActiveTagsFull.BuildAutomat();
//  m_arAllActiveTagsShort.BuildAutomat();
	//-* Post Build END ------------------------------------------------------

#ifdef _DEBUG
	AfxDump(XTPGetLexAutomatMemMan());
#endif
	//------------------------------------------------------------------------
	return TRUE;
}

BOOL CXTPSyntaxEditLexClassSchema::Build_ShortTree()
{
#ifdef DBG_TRACE_LOAD_CLASS_SCH
	TRACE(_T("\n****************** Short classes Tree ********************** \n"));
#endif

	int nCount = (int)m_arClassesTreeFull.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrCfile = m_arClassesTreeFull.GetAt(i);
		if (!ptrCfile || ptrCfile->GetAttribute_BOOL(XTPLEX_ATTR_PARSEONSCREEN, FALSE))
		{
			continue;
		}

		CXTPSyntaxEditLexClassPtr ptrCFnew = GetNewClass(TRUE);
		if (!ptrCFnew)
		{
			return FALSE;
		}
		ptrCFnew->CopyFrom(ptrCfile);

		#ifdef DBG_TRACE_LOAD_CLASS_SCH
			ptrCFnew->Dump(_T(""));
		#endif

		if (!CopyChildrenFor(TRUE, ptrCFnew, ptrCfile, 0))
		{
			return FALSE;
		}

		int nStartCount = 0;
		ptrCFnew->BuildActiveTags(nStartCount);
		//CXTPSyntaxEditLexVariantPtrArray* ptrAT = ptrCFnew->BuildActiveTags(nStartCount);
		//ConcatenateLVArrays(&m_arAllActiveTagsShort, ptrAT, nStartCount);

		m_arClassesTreeShort.Add(ptrCFnew);
	}

	#ifdef DBG_TRACE_LOAD_CLASS_SCH
//      m_arAllActiveTagsShort.Dump(_T("AllActiveTags Short=: "));
	#endif

	return TRUE;
}

BOOL CXTPSyntaxEditLexClassSchema::CopyChildrenFor(BOOL bShort, CXTPSyntaxEditLexClass* pCDest,
												   CXTPSyntaxEditLexClass* pCSrc, int nLevel)
{
#ifdef DBG_TRACE_LOAD_CLASS_SCH
	CString strTraceOffset0, strTraceOffset;
	for (int t = 0; t <= nLevel; t++)
	{
		strTraceOffset0 += _T("   ");
	}
#endif

	CXTPSyntaxEditLexClassPtrArray* pArClasses[3];
	pArClasses[0] = pCSrc->GetChildren();
	pArClasses[1] = pCSrc->GetChildrenDyn();
	pArClasses[2] = pCSrc->GetChildrenSelfRef();

	for (int nC = 0; nC < _countof(pArClasses); nC++)
	{
		BOOL bDynamic = (nC == 1);
		BOOL bSelf  = (nC == 2);


		#ifdef DBG_TRACE_LOAD_CLASS_SCH
			if (bDynamic)
			{
				strTraceOffset = strTraceOffset0 + _T("dyn: ");
			}
			else if (bSelf)
			{
				strTraceOffset = strTraceOffset0 + _T("self: ");
			}
			else
			{
				strTraceOffset = strTraceOffset0;
			}
		#endif

		int nCount = pArClasses[nC] ? (int)pArClasses[nC]->GetSize() : 0;
		for (int i = 0; i < nCount; i++)
		{
			CXTPSyntaxEditLexClassPtr ptrCsrc = pArClasses[nC]->GetAt(i);

			if (!ptrCsrc ||
				bShort && ptrCsrc->GetAttribute_BOOL(XTPLEX_ATTR_PARSEONSCREEN, FALSE))
			{
				continue;
			}

			if (bSelf)
			{
				pCDest->AddChild(pCDest, FALSE);

				#ifdef DBG_TRACE_LOAD_CLASS_SCH
					pCDest->Dump(strTraceOffset);
				#endif
			}
			else
			{
				CXTPSyntaxEditLexClassPtr ptrCnew = GetNewClass(FALSE);
				if (!ptrCnew)
				{
					return FALSE;
				}
				ptrCnew->CopyFrom(ptrCsrc);

				pCDest->AddChild(ptrCnew, bDynamic);

				#ifdef DBG_TRACE_LOAD_CLASS_SCH
					ptrCnew->Dump(strTraceOffset);
				#endif

				ASSERT(pCSrc != ptrCsrc);

				if (!CopyChildrenFor(bShort, ptrCnew, ptrCsrc, nLevel+1))
				{
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CXTPSyntaxEditLexClassSchema::Build_ChildrenFor(BOOL bDynamic,
										   CXTPSyntaxEditLexClass* pCBase,
										   CStringArray& rarAdded,
										   int& rnNextClassID,
										   int nLevel)
{
	if (!pCBase)
	{
		ASSERT(FALSE);
		return FALSE;
	}

#ifdef DBG_TRACE_LOAD_CLASS_SCH
	CString strTraceOffset;
	for (int t = 0; t <= nLevel; t++)
	{
		strTraceOffset += _T("   ");
	}
	if (bDynamic)
	{
		strTraceOffset += _T("dyn: ");
	}
#endif

	BOOL bSelfChild = FALSE;
	CXTPSyntaxEditLexClassPtrArray* ptrArChildren;
	if (bDynamic)
	{
		ptrArChildren = GetDynChildrenFor(pCBase, bSelfChild);
	}
	else
	{
		ptrArChildren = GetChildrenFor(pCBase, bSelfChild);
	}

	int nCount = ptrArChildren ? (int)ptrArChildren->GetSize() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrCsrc = ptrArChildren->GetAt(i);
		CString strCNsrc = ptrCsrc->GetClassName();

		if (nLevel == 0)
		{
			rarAdded.RemoveAll();
		}
		//int nFIdx = Find_noCase(rarAdded, strCNsrc);
		//if (nFIdx >= 0)
		int nClsCount = FindStrCount(rarAdded, strCNsrc, FALSE);
		int nRecurrenceDepth = ptrCsrc->GetAttribute_int(XTPLEX_ATTR_RECURRENCEDEPTH, TRUE, 1);;

		if (nClsCount >= nRecurrenceDepth)
		{
			//CXTPSyntaxEditLexClassPtr ptrC(pCBase->FindParent(strCNsrc), TRUE);
			//if (ptrC) {
			//  pCBase->GetChildrenSelfRef()->Add(ptrC);
			//#ifdef DBG_TRACE_LOAD_CLASS_SCH
			//  strTraceOffset += _T("self: ");
			//  ptrC->Dump(strTraceOffset);
			//#endif
			//}
			continue;
		}
		rarAdded.Add(strCNsrc);

		CXTPSyntaxEditLexClassPtr ptrCnew = GetNewClass(FALSE);
		if (!ptrCnew)
		{

			delete ptrArChildren;
			return FALSE;
		}
		ptrCnew->CopyFrom(ptrCsrc);

		CXTPSyntaxEditLexVariant lvID(rnNextClassID++);
		ptrCnew->SetAttribute(XTPLEX_ATTRCLASSID, lvID);

		pCBase->AddChild(ptrCnew, bDynamic);

		#ifdef DBG_TRACE_LOAD_CLASS_SCH
			ptrCnew->Dump(strTraceOffset);
		#endif

		Build_ChildrenFor(bDynamic, ptrCnew, rarAdded, rnNextClassID, nLevel+1);

		Build_ChildrenFor(!bDynamic, ptrCnew, rarAdded, rnNextClassID, nLevel+1);

		int nStackCount = (int)rarAdded.GetSize();
		if (nStackCount > 0)
		{
			rarAdded.RemoveAt(nStackCount - 1);
		}
	}

	delete ptrArChildren;

	//----------------------------------
	if (bSelfChild)
	{
		if (pCBase->AddChild(pCBase, bDynamic))
		{
			#ifdef DBG_TRACE_LOAD_CLASS_SCH
				strTraceOffset += _T("self: ");
				pCBase->Dump(strTraceOffset);
			#endif
		}
	}
	return TRUE;
}

BOOL CXTPSyntaxEditLexClassSchema::PostBuild_Step(CXTPSyntaxEditLexClassPtrArray* pArClasses)
{
	if (!pArClasses)
	{
		return TRUE;
	}

	int nCount = (int)pArClasses->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = pArClasses->GetAt(i);
		ptrC->SortTags();

		CXTPSyntaxEditLexClassPtrArray* ptrCh1 = ptrC->GetChildren();
		CXTPSyntaxEditLexClassPtrArray* ptrCh2 = ptrC->GetChildrenDyn();

		if (!PostBuild_Step(ptrCh1))
		{
			return FALSE;
		}
		if (!PostBuild_Step(ptrCh2))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CXTPSyntaxEditLexClassSchema::Close()
{
	RemoveAll();
}

void CXTPSyntaxEditLexClassSchema::RemoveAll()
{
	CXTPSyntaxEditLexClass::CloseClasses(&m_arPreBuildClassesList);

	CXTPSyntaxEditLexClass::CloseClasses(&m_arClassesTreeFull);
	CXTPSyntaxEditLexClass::CloseClasses(&m_arClassesTreeShort);
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClassSchema::GetClasses(BOOL bShortSch)
{
	return bShortSch? &m_arClassesTreeShort: &m_arClassesTreeFull;
}

CXTPSyntaxEditLexClassPtrArray* CXTPSyntaxEditLexClassSchema::GetPreBuildClasses()
{
	return &m_arPreBuildClassesList;
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexClassSchema::GetNewClass(BOOL bForFile)
{
	if (bForFile)
	{
		return new CXTPSyntaxEditLexClass_file();
	}
	return new CXTPSyntaxEditLexClass();
}

////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLexParseContext
//class CXTPSyntaxEditLexParseContext : public CXTPInternalUnknown
//CXTPSyntaxEditLexParseContext::CXTPSyntaxEditLexParseContext(CXTPSyntaxEditLexTextBlock* pBlock) :
//                  CXTPInternalUnknown(pBlock)
//{
//  ASSERT(pBlock);
//
//  m_pTextBlock = pBlock;
//}
//
//CXTPSyntaxEditLexParseContext::~CXTPSyntaxEditLexParseContext()
//{
//}
//
//void CXTPSyntaxEditLexParseContext::Set(LPCTSTR pcszParopPath, CXTPSyntaxEditLexVariant* pVar)
//{
//  if (pVar == NULL) {
//      m_mapVars.RemoveKey(pcszParopPath);
//
//  }
//  else {
//      CXTPSyntaxEditLexVariantPtr ptrVar = new CXTPSyntaxEditLexVariant(*pVar);
//
//      m_mapVars.SetAt(pcszParopPath, ptrVar);
//  }
//}
//
//CXTPSyntaxEditLexVariant* CXTPSyntaxEditLexParseContext::Get(LPCTSTR pcszParopPath)
//{
//  CXTPSyntaxEditLexVariantPtr ptrVar;
//
//  if (m_mapVars.Lookup(pcszParopPath, ptrVar)) {
//      return ptrVar.Detach();
//  }
//  return NULL;
//}
//
//
//CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexParseContext::GetTextBlock()
//{
//  if (m_pTextBlock) {
//      m_pTextBlock->InternalAddRef();
//  }
//  return m_pTextBlock;
//}
//
//void CXTPSyntaxEditLexParseContext::Close()
//{
//  m_pTextBlock = NULL;
//  m_mapVars.RemoveAll();
//}
////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLexTextBlock
CXTPSyntaxEditLexTextBlock::CXTPSyntaxEditLexTextBlock() //:
	//m_parseContext(this)
{
	m_PosStartLC.nLine = 0;
	m_PosStartLC.nCol = 0;

	m_PosEndLC.nLine = 0;
	m_PosEndLC.nCol = 0;

	m_nStartTagLen = 0;
	m_nEndTagXLCLen = 0;
}

CXTPSyntaxEditLexTextBlock::~CXTPSyntaxEditLexTextBlock()
{

}

void CXTPSyntaxEditLexTextBlock::Close()
{
	m_ptrLexClass = NULL;
	m_ptrParent = NULL;

	m_ptrPrev = NULL;
	m_ptrNext= NULL;
	m_ptrLastChild = NULL;
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextBlock::GetPrevChild(CXTPSyntaxEditLexTextBlock* pChild,
												 BOOL bWithAddRef)
{
	while (pChild && pChild != this)
	{
		if (!pChild->m_ptrPrev)
		{
			return NULL;
		}

		pChild = pChild->m_ptrPrev;

		if (pChild && (CXTPSyntaxEditLexTextBlock*)pChild->m_ptrParent == this)
		{
			if (bWithAddRef)
			{
				pChild->InternalAddRef();
			}
			return pChild;
		}
	}
	return NULL;
}

int CXTPSyntaxEditLexTextBlock::EndChildren(CXTPSyntaxEditLexTextSchema* pTxtSch)
{
	int nCount = 0;
	//CSingleLock singleLock(GetDataLoker(), TRUE);

	CXTPSyntaxEditLexTextBlock* pChTB = m_ptrLastChild;
	while (pChTB)
	{
		if (!pChTB->m_PosEndLC.IsValidData())
		{
			pChTB->m_PosEndLC = m_PosEndLC;
			pChTB->m_nEndTagXLCLen = m_nEndTagXLCLen;
			nCount++;

			if (pTxtSch)
			{
				pTxtSch->SendEvent_OnTextBlockParsed(pChTB);
			}
			nCount += pChTB->EndChildren(pTxtSch);
		}
		pChTB = GetPrevChild(pChTB, FALSE);
	}
	return nCount;
}

BOOL CXTPSyntaxEditLexTextBlock::IsLookLikeClosed() const
{
	if (!m_ptrLexClass && !m_ptrPrev && !m_ptrNext && !m_ptrLastChild &&
		!m_ptrParent)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPSyntaxEditLexTextBlock::IsInclude(CXTPSyntaxEditLexTextBlock* pTB2) const
{
	if (!pTB2)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	ASSERT(m_PosStartLC.IsValidData() && pTB2->m_PosStartLC.IsValidData());

	if (m_PosStartLC <= pTB2->m_PosStartLC && m_PosEndLC >= pTB2->m_PosEndLC &&
		m_PosEndLC.IsValidData() && pTB2->m_PosEndLC.IsValidData() )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPSyntaxEditLexTextBlock::IsEqualLexClasses(CXTPSyntaxEditLexTextBlock* pTB2) const
{
	if (!m_ptrLexClass || !pTB2 || !pTB2->m_ptrLexClass)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	CString strThisClass = m_ptrLexClass->GetClassName();
	CString strTB2Class = pTB2->m_ptrLexClass->GetClassName();
	int nCmpRes = strThisClass.CompareNoCase(strTB2Class);

	return nCmpRes==0;
}

////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditLexAnalyser

CXTPSyntaxEditLexParser::CXTPSyntaxEditLexParser()
{
	m_pParseThread = NULL;
	m_nParseThreadPriority = THREAD_PRIORITY_NORMAL;
							// THREAD_PRIORITY_BELOW_NORMAL
							// THREAD_PRIORITY_ABOVE_NORMAL;

//  m_SinkMT.SetOuter(this);
	m_pConnect = new CXTPNotifyConnection;

	m_ptrTextSchema = 0;

	m_pSchOptions_default = new CXTPSyntaxEditLexParserSchemaOptions();
}

CXTPSyntaxEditLexParser::~CXTPSyntaxEditLexParser()
{
	Close();
	ASSERT(m_ptrTextSchema == 0);

#ifdef XTP_DBG_DUMP_OBJ
	afxDump.SetDepth( 1 );
#endif
	CMDTARGET_RELEASE(m_pConnect);
	CMDTARGET_RELEASE(m_pSchOptions_default);

	RemoveAllOptions();
}

void CXTPSyntaxEditLexParser::RemoveAllOptions()
{
	POSITION pos = m_mapSchOptions.GetStartPosition();
	while(pos)
	{
		CXTPSyntaxEditLexParserSchemaOptions* pOptions = NULL;
		CString key;

		m_mapSchOptions.GetNextAssoc(pos, key, pOptions);
		CMDTARGET_RELEASE(pOptions);
	}
	m_mapSchOptions.RemoveAll();
}

CXTPNotifyConnection* CXTPSyntaxEditLexParser::GetConnection()
{
	return m_pConnect;
}

CXTPSyntaxEditLexParser::CXTPSyntaxEditParseThreadParams::CXTPSyntaxEditParseThreadParams()
{
	ptrBuffer = NULL;
}

void CXTPSyntaxEditLexParser::CXTPSyntaxEditParseThreadParams::AddParseZone(const CXTPSyntaxEditTextRegion& rZone)
{
	const int cnEpsilon = XTP_EDIT_XLC(3, 0);

	int nCount = (int)arInvalidZones.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditTextRegion zoneI = arInvalidZones[i];

		if (rZone.m_posEnd >= zoneI.m_posStart &&
			rZone.m_posStart <= zoneI.m_posStart
			||
			rZone.m_posEnd.GetXLC()+cnEpsilon >= zoneI.m_posStart.GetXLC() &&
			rZone.m_posStart <= zoneI.m_posStart
			)
		{
			zoneI.m_posStart = rZone.m_posStart;
			arInvalidZones[i] = zoneI;
			return;
		}
		else if (rZone.m_posStart <= zoneI.m_posEnd &&
				rZone.m_posEnd >= zoneI.m_posEnd
				||
				rZone.m_posStart.GetXLC()-cnEpsilon <= zoneI.m_posEnd.GetXLC() &&
				rZone.m_posEnd >= zoneI.m_posEnd
				)
		{
			zoneI.m_posEnd = rZone.m_posEnd;
			arInvalidZones[i] = zoneI;
			return;
		}
		else if (rZone.m_posStart >= zoneI.m_posStart
			&& rZone.m_posEnd <= zoneI.m_posEnd)
		{
			return; //nothing
		}
	}
	arInvalidZones.Add(rZone);
}

void CXTPSyntaxEditLexParser::Close()
{
	CSingleLock singLockMain(&m_csParserData);

	CloseParseThread();

	m_SinkMT.UnadviseAll();

	CMDTARGET_RELEASE(m_ptrTextSchema);
}

void CXTPSyntaxEditLexParser::SelfCloseParseThread()
{
	ASSERT(m_pParseThread);

	if (m_pParseThread)
	{
		m_pParseThread = NULL;

		m_PThreadParams.evExitThread.ResetEvent();
		ASSERT(m_PThreadParams.arInvalidZones.GetSize() == 0);

		if (m_ptrTextSchema)
		{
			m_ptrTextSchema->GetBreakParsingEvent()->ResetEvent();
		}
	}
}

//===========================================================================
//#pragma warning(push)
#pragma warning(disable: 4702)   // warning C4702: unreachable code
//----------------------------
void CXTPSyntaxEditLexParser::CloseParseThread()
{
	StopParseInThread();

	CSingleLock singLockMain(&m_csParserData);

	if (m_pParseThread)
	{
		HANDLE hThread = NULL;

		try
		{
			hThread = m_pParseThread->m_hThread;
		}
		catch (...)
		{
			TRACE(_T("ERROR! Parse Thread is not exist. [CloseParseThread()]\n"));
		}
		m_pParseThread = NULL;

		DWORD dwThreadRes = WAIT_TIMEOUT;
		for (int i = 0; i < 10 && dwThreadRes == WAIT_TIMEOUT; i++)
		{
			m_PThreadParams.evParseRun.ResetEvent();
			m_PThreadParams.evExitThread.SetEvent();

			if (m_ptrTextSchema)
			{
				m_ptrTextSchema->GetBreakParsingEvent()->SetEvent();
			}

			if (hThread)
			{
				dwThreadRes = ::WaitForSingleObject(hThread, 20*1000);
			}
			else
			{
				Sleep(5000);
				break;
			}
		}
		if (dwThreadRes == WAIT_TIMEOUT && hThread)
		{
			::TerminateThread(hThread, 0);
			TRACE(_T("ERROR! Parser thread was not ended by normal way. It was terminated. \n"));
		}
		m_PThreadParams.evExitThread.ResetEvent();
		m_PThreadParams.arInvalidZones.RemoveAll();

		if (m_ptrTextSchema)
		{
			m_ptrTextSchema->GetBreakParsingEvent()->ResetEvent();
		}
	}
}
//#pragma warning(pop)
//===========================================================================

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::GetBlocks()
{
	return m_ptrFirstBlock.GetInterface(TRUE);
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::FindNearestTextBlock(XTP_EDIT_LINECOL posText)
{
	CSingleLock singleLock(GetDataLoker(), TRUE);

	//---------------------------------------------------------------------------
	CXTPSyntaxEditLexTextBlockPtr ptrTBstart = GetBlocks();
	CXTPSyntaxEditLexTextBlock* pTBnearest = ptrTBstart;

	for (CXTPSyntaxEditLexTextBlock* pTB = ptrTBstart; pTB; pTB = pTB->m_ptrNext)
	{
		if (posText < pTB->m_PosStartLC)
		{
			break;
		}
		pTBnearest = pTB;
	}
	//--------------------------
	while (pTBnearest && pTBnearest->m_ptrPrev &&
		  pTBnearest->m_PosStartLC == pTBnearest->m_ptrPrev->m_PosStartLC)
	{
		pTBnearest = pTBnearest->m_ptrPrev;
	}

	//--------------------------
	if (pTBnearest)
	{
		pTBnearest->InternalAddRef();
	}
	return pTBnearest;
}

BOOL CXTPSyntaxEditLexTextSchema::UpdateTBNearest(CXTPSyntaxEditLexTextBlock* pNarestTB1,
										int nLineDiff, int nColDiff,
										XTP_EDIT_LINECOL posFrom, XTP_EDIT_LINECOL posTo,
										 int eEditAction )
{
	int nResult = xtpEditUTBNothing;

	CXTPSyntaxEditLexTextBlock* pTB;
	for (pTB = pNarestTB1; pTB; pTB = pTB->m_ptrParent)
	{
		ASSERT(pTB != pTB->m_ptrParent);

		//-------------------------------------------------------------
		BOOL bIsBlockStartIntersected = eEditAction == xtpEditActDelete &&
			pTB->m_PosStartLC >= posFrom && pTB->m_PosStartLC < posTo
			||
			eEditAction == xtpEditActInsert &&
			pTB->m_PosStartLC < posFrom &&
			posFrom.GetXLC() < pTB->GetStartTagEndXLC();

		if (bIsBlockStartIntersected)
		{
			nResult |= xtpEditUTBReparse;
			if (pTB == pNarestTB1)
			{
				nResult |= xtpEditUTBNearestUpdated;
			}
			// set empty
			pTB->m_PosEndLC.nLine = pTB->m_PosStartLC.nLine;
			pTB->m_PosEndLC.nCol = pTB->m_PosStartLC.nCol-1;
			continue;
		}

		//-------------------------------------------------------------
		BOOL bIsEditingFullyInside = eEditAction == xtpEditActInsert &&
				pTB->GetStartTagEndXLC() <= posFrom.GetXLC() &&
				posFrom.GetXLC() <= pTB->GetEndTagBeginXLC()
				||
				eEditAction == xtpEditActDelete &&
				pTB->GetStartTagEndXLC() <= posFrom.GetXLC() &&
				posTo.GetXLC() <= pTB->GetEndTagBeginXLC();

		if (bIsEditingFullyInside)
		{
			if (pTB == pNarestTB1)
			{
				nResult |= xtpEditUTBNearestUpdated;
			}
			BOOL bEmpty = FALSE;

			pTB->m_PosEndLC.nLine += nLineDiff;

			if (pTB->m_PosEndLC.nLine < pTB->m_PosStartLC.nLine)
			{
				// set empty
				bEmpty = TRUE;
				pTB->m_PosEndLC.nLine = pTB->m_PosStartLC.nLine;
				pTB->m_PosEndLC.nCol = pTB->m_PosStartLC.nCol-1;
			}
			int nELine = eEditAction == xtpEditActInsert ? posTo.nLine : posFrom.nLine;
			if (!bEmpty && pTB->m_PosEndLC.nLine == nELine)
			{
				pTB->m_PosEndLC.nCol += nColDiff;
				if (pTB->m_PosEndLC.nCol < 0)
				{
					pTB->m_PosEndLC.nCol = 0;
				}
			}
			continue;
		}

		//-------------------------------------------------------------
		BOOL bIsBlockEndIntersected = eEditAction == xtpEditActDelete &&
			(!(posTo.GetXLC() <= pTB->GetEndTagBeginXLC() ||
				posFrom.GetXLC() > pTB->GetEndTagEndXLC() )
			)
			||
			eEditAction == xtpEditActInsert &&
			posFrom.GetXLC() > pTB->GetEndTagBeginXLC() &&
			posFrom.GetXLC() <= pTB->GetEndTagEndXLC();

		if (bIsBlockEndIntersected)
		{
			nResult |= xtpEditUTBReparse;
			if (pTB == pNarestTB1)
			{
				nResult |= xtpEditUTBNearestUpdated;
			}
			// set empty
			pTB->m_PosEndLC.nLine = 0;
			pTB->m_PosEndLC.nCol =  0;
			continue;
		}
	}

	//--------------------
	return nResult;
}

int CXTPSyntaxEditLexTextSchema::UpdateTextBlocks(XTP_EDIT_LINECOL posFrom, XTP_EDIT_LINECOL posTo,
										 int eEditAction )

{
	CSingleLock singleLock(GetDataLoker());

	//BOOL bLocked = TryLockCS(&singleLock, GetDataLoker(), 5000, 50);
	BOOL bLocked = singleLock.Lock(5000);

	if (!bLocked)
	{
		ASSERT(FALSE);
		TRACE(_T("ERROR! Cannot enter critical section. [Dead lock, hang up???] CXTPSyntaxEditLexTextSchema::UpdateTextBlocks() \n"));
		return xtpEditUTBError;
	}

	CXTPSyntaxEditLexTextBlockPtr ptrNarestTB1 = FindNearestTextBlock(posFrom);
	if (!ptrNarestTB1)
	{
		return xtpEditUTBNothing;
	}

	int nLineDiff = posTo.nLine - posFrom.nLine;
	int nColDiff = posTo.nCol - posFrom.nCol;

	ASSERT(nLineDiff >= 0);

	if (eEditAction == xtpEditActDelete)
	{
		nLineDiff *= -1;
		nColDiff *= -1;
	}

	//- (1)- update FullyInside nearest and parents blocks
	int nURes = UpdateTBNearest(ptrNarestTB1, nLineDiff,
										nColDiff, posFrom, posTo, eEditAction);

	//- (2)- update block after nearest
	CXTPSyntaxEditLexTextBlock* pTB = (nURes & xtpEditUTBNearestUpdated) ? ptrNarestTB1->m_ptrNext :
							ptrNarestTB1;

	for (; pTB; pTB = pTB->m_ptrNext)
	{
		//-------------------------------------------------------------
		BOOL bIsBlockStartIntersected = eEditAction == xtpEditActDelete &&
			(!(posTo <= pTB->m_PosStartLC ||
				posFrom.GetXLC() >= pTB->GetStartTagEndXLC())
			)
			||
			eEditAction == xtpEditActInsert &&
			pTB->m_PosStartLC < posFrom &&
			posFrom.GetXLC() < pTB->GetStartTagEndXLC();

		if (bIsBlockStartIntersected)
		{
			nURes |= xtpEditUTBReparse;

			// set empty
			pTB->m_PosEndLC.nLine = pTB->m_PosStartLC.nLine;
			pTB->m_PosEndLC.nCol = pTB->m_PosStartLC.nCol-1;
			continue;
		}

		//-------------------------------------------------------------
		BOOL bIsEditingFullyInside = eEditAction == xtpEditActInsert &&
			pTB->GetStartTagEndXLC() <= posFrom.GetXLC() &&
			posFrom.GetXLC() <= pTB->GetEndTagBeginXLC()
			||
			eEditAction == xtpEditActDelete &&
			pTB->GetStartTagEndXLC() <= posFrom.GetXLC() &&
			posTo.GetXLC() <= pTB->GetEndTagBeginXLC(); // ??? +- 1;


		if (bIsEditingFullyInside)
		{
			BOOL bEmpty = FALSE;

			pTB->m_PosEndLC.nLine += nLineDiff;

			if (pTB->m_PosEndLC.nLine < pTB->m_PosStartLC.nLine)
			{
				// set empty
				bEmpty = TRUE;
				pTB->m_PosEndLC.nLine = pTB->m_PosStartLC.nLine;
				pTB->m_PosEndLC.nCol = pTB->m_PosStartLC.nCol-1;
			}
			int nELine = eEditAction == xtpEditActInsert ? posTo.nLine : posFrom.nLine;
			if (!bEmpty && pTB->m_PosEndLC.nLine == nELine)
			{
				pTB->m_PosEndLC.nCol += nColDiff;
				if (pTB->m_PosEndLC.nCol < 0)
				{
					pTB->m_PosEndLC.nCol = 0;
				}
			}
			continue;
		}

		//-------------------------------------------------------------
		BOOL bIsBlockEndIntersected = eEditAction == xtpEditActDelete &&
			(!(posTo.GetXLC() <= pTB->GetEndTagBeginXLC() ||
				posFrom.GetXLC() > pTB->GetEndTagEndXLC() )
			)
			||
			eEditAction == xtpEditActInsert &&
			posFrom.GetXLC() > pTB->GetEndTagBeginXLC() &&
			posFrom.GetXLC() <= pTB->GetEndTagEndXLC();

		if (bIsBlockEndIntersected)
		{
			nURes |= xtpEditUTBReparse;

			// set empty
			pTB->m_PosEndLC.nLine = 0;
			pTB->m_PosEndLC.nCol = 0;
			continue;
		}

		//-------------------------------------------------------------------
		BOOL bIsBlockOutside = eEditAction == xtpEditActInsert &&
					pTB->m_PosStartLC >= posFrom ||
					eEditAction == xtpEditActDelete && pTB->m_PosStartLC >= posTo;
		//-----------------------------
		int nELine = eEditAction == xtpEditActInsert ? posTo.nLine : posFrom.nLine;

		if (bIsBlockOutside && nLineDiff == 0 &&
			pTB->m_PosStartLC.nLine != nELine &&
			pTB->m_PosEndLC.nLine != nELine)
		{
			break;
		}
		//-------------------------------------------------------------------

		if (bIsBlockOutside )
		{
			pTB->m_PosStartLC.nLine += nLineDiff;
			pTB->m_PosEndLC.nLine += nLineDiff;

			if (pTB->m_PosStartLC.nLine == nELine)
			{
				pTB->m_PosStartLC.nCol += nColDiff;
			}
			if (pTB->m_PosEndLC.nLine == nELine)
			{
				pTB->m_PosEndLC.nCol += nColDiff;
			}
		}
		//-------------------------------------------------------------------
	}

	return nURes;
}

BOOL CXTPSyntaxEditLexTextSchema::LoadClassSchema(CXTPSyntaxEditLexClassInfoArray& arClassInfo)
{
	CSingleLock singleLockCls(GetClassSchLoker(), TRUE);
	CSingleLock singleLock(GetDataLoker(), TRUE);

	RemoveAll();
	m_pClassSchema->RemoveAll();

	int nCCount = (int)arClassInfo.GetSize();
	for (int i = 0; i < nCCount; i++)
	{
		const XTP_EDIT_LEXCLASSINFO& infoClass = arClassInfo.GetAt(i);

		CXTPSyntaxEditLexClassPtr ptrLexClass = m_pClassSchema->GetNewClass(FALSE);
		if (!ptrLexClass)
		{
			return FALSE;
		}
		ptrLexClass->m_strClassName = infoClass.csClassName;

		int nPCount = (int)infoClass.arPropertyDesc.GetSize();
		for (int k = 0; k < nPCount; k++)
		{
			const XTP_EDIT_LEXPROPINFO& infoProp = infoClass.arPropertyDesc[k];
			if (!ptrLexClass->SetProp(&infoProp))
			{
				ASSERT(FALSE);
			}
		}

		m_pClassSchema->AddPreBuildClass(ptrLexClass);

		#ifdef DBG_TRACE_LOAD_CLASS_SCH
		{
			BOOL bEmpty = ptrLexClass->IsEmpty() && ptrLexClass->m_Parent.eOpt != xtpEditOptParent_file;
			LPCTSTR cszPref = bEmpty ? _T("* !EMPTY! (it is not used)* ") : _T("* ");
			ptrLexClass->Dump(cszPref);
		}
		#endif
	}

	BOOL bRes = m_pClassSchema->Build();

	return bRes;
}

BOOL CXTPSyntaxEditLexTextSchema::IsFileExtSupported(const CString& strExt)
{
	CXTPSyntaxEditLexClassPtr ptrTopCls = GetTopClassForFileExt(strExt);
	return ptrTopCls != NULL;
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexTextSchema::GetTopClassForFileExt(const CString& strExt)
{
	CString strPropName = _T("IsExt=") + strExt;

	CXTPSyntaxEditLexClassPtrArray* ptrArCfile = m_pClassSchema->GetClasses(TRUE);
	int nCount = ptrArCfile ? (int)ptrArCfile->GetSize() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrCfile = ptrArCfile->GetAt(i);
		CXTPSyntaxEditLexVariantPtr ptrRes = ptrCfile->PropV(strPropName);

		if (!ptrRes || ptrRes->m_nObjType != xtpEditLVT_valInt)
		{
			ASSERT(FALSE);
			continue;
		}
		if (ptrRes->m_nValue)
		{
			return ptrCfile.Detach();
		}
	}
	return NULL;
}


CXTPSyntaxEditLexTextSchema* CXTPSyntaxEditLexParser::GetTextSchema()
{
	return m_ptrTextSchema;
}

void CXTPSyntaxEditLexParser::SetTextSchema(CXTPSyntaxEditLexTextSchema* pTextSchema)
{
	CSingleLock singLockMain(&m_csParserData);

	CloseParseThread();

	if (m_ptrTextSchema)
	{
		m_ptrTextSchema->RemoveAll();
	}

	m_SinkMT.UnadviseAll();

	CMDTARGET_RELEASE(m_ptrTextSchema);
	RemoveAllOptions();

	if (pTextSchema)
	{
		m_ptrTextSchema = pTextSchema->Clone();

		CXTPNotifyConnection* ptrConn = m_ptrTextSchema ? m_ptrTextSchema->GetConnection() : NULL;
		if (ptrConn)
		{
			m_SinkMT.Advise(ptrConn, xtpEditOnParserStarted,   &XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser::OnParseEvent_NotificationHandler);
			m_SinkMT.Advise(ptrConn, xtpEditOnTextBlockParsed, &XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser::OnParseEvent_NotificationHandler);
			m_SinkMT.Advise(ptrConn, xtpEditOnParserEnded,     &XTPSyntaxEditLexAnalyser::CXTPSyntaxEditLexParser::OnParseEvent_NotificationHandler);
		}
	}

#ifdef _DEBUG
	AfxDump(XTPGetLexAutomatMemMan());
#endif
}

BOOL CXTPSyntaxEditLexParser::GetTokensForAutoCompleate(CXTPSyntaxEditLexTokensDefArray& rArTokens,
											  BOOL bAppend)
{
	CSingleLock singLockMain(&m_csParserData);

	if (!bAppend)
	{
		rArTokens.RemoveAll();
	}

	CXTPSyntaxEditLexTextSchema* ptrTxtSch = GetTextSchema();
	CXTPSyntaxEditLexClassSchema* ptrClsSch = ptrTxtSch ? ptrTxtSch->GetClassSchema() : NULL;
	CXTPSyntaxEditLexClassPtrArray* ptrArCls = ptrClsSch ? ptrClsSch->GetPreBuildClasses() : NULL;

	if (!ptrArCls)
	{
		return FALSE;
	}

	//--------------------------------------------------
	int nCCount = (int)ptrArCls->GetSize();
	for (int i = 0; i < nCCount; i++)
	{
		CXTPSyntaxEditLexTokensDef tmpTkDef;

		CXTPSyntaxEditLexClassPtr ptrCls = ptrArCls->GetAt(i);
		CXTPSyntaxEditLexVariantPtr ptrTags = ptrCls->PropV(_T("token:tag"));

		if (!ptrTags)
		{
			continue;
		}
		GetStrsFromLVArray(ptrTags, tmpTkDef.m_arTokens);

		//--------------------
		CXTPSyntaxEditLexVariantPtr ptrSartSeps = ptrCls->PropV(_T("token:start:separators"));
		CXTPSyntaxEditLexVariantPtr ptrEndSeps = ptrCls->PropV(_T("token:end:separators"));
		if (ptrSartSeps)
		{
			GetStrsFromLVArray(ptrSartSeps, tmpTkDef.m_arStartSeps);
		}

		if (ptrEndSeps)
		{
			GetStrsFromLVArray(ptrEndSeps, tmpTkDef.m_arEndSeps);
		}

		//=================
		rArTokens.Add(tmpTkDef);
	}

	return TRUE;
}

void CXTPSyntaxEditLexParser::GetStrsFromLVArray(CXTPSyntaxEditLexVariant* pLVArray,
									  CStringArray& rArStrs) const
{
	if (!pLVArray || pLVArray->m_nObjType != xtpEditLVT_LVArrayPtr ||
		!pLVArray->m_ptrLVArrayPtr)
	{
		ASSERT(FALSE);
		return;
	}

	int nCount = (int)pLVArray->m_ptrLVArrayPtr->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexVariantPtr ptrLVVal = pLVArray->m_ptrLVArrayPtr->GetAt(i);
		ASSERT(ptrLVVal);

		if (ptrLVVal && ptrLVVal->m_nObjType == xtpEditLVT_valStr)
		{
			for (int k = 0; k < ptrLVVal->m_arStrVals.GetSize(); k++)
			{
				CString strVal = ptrLVVal->m_arStrVals[k];
				ASSERT(strVal.GetLength());

				if (strVal.GetLength())
				{
					rArStrs.Add(strVal);
				}
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////
CXTPSyntaxEditLexParser::CXTPSyntaxEditParseThreadParams* CXTPSyntaxEditLexParser::GetParseInThreadParams()
{
	return &m_PThreadParams;
}

void CXTPSyntaxEditLexParser::StartParseInThread(CXTPSyntaxEditBufferManager* pBuffer,
					const XTP_EDIT_LINECOL* pLCStart, const XTP_EDIT_LINECOL* pLCEnd,
					int eEdinAction, BOOL bRunWithoutWait)
{
	UNREFERENCED_PARAMETER(eEdinAction);
	DBG_TRACE_TIME(1, _T("### ### ### StartParseInThread time = "));

	CSingleLock singLockMain(&m_csParserData);

	if (!m_ptrTextSchema)
	{
		return;
	}

	CSingleLock singLockPrm(&m_PThreadParams.lockThreadParams);

	//if (IsCSLocked(m_PThreadParams.lockThreadParams)) {
	if (IsMutexLocked(&m_PThreadParams.lockThreadParams))
	{
		DBG_TRACE_PARSE_START_STOP(_T("- Parser set BREAK Event. \n"));

		VERIFY( m_ptrTextSchema->GetBreakParsingEvent()->SetEvent() );
	}
	if (!singLockPrm.Lock())
	{
		ASSERT(FALSE);
		return;
	}

	m_PThreadParams.ptrBuffer = pBuffer;

	CXTPSyntaxEditTextRegion invZone;

	if (pLCStart && !pLCEnd && pLCStart->GetXLC() == XTP_EDIT_XLC(1, 0) )
	{
		invZone.Set(pLCStart, pLCEnd);
		m_PThreadParams.arInvalidZones.RemoveAll();
		m_PThreadParams.AddParseZone(invZone);
	}
	else if (pLCStart || pLCEnd)
	{
		invZone.Set(pLCStart, pLCEnd);
		m_PThreadParams.AddParseZone(invZone);
	}

	if (!m_pParseThread)
	{// create thread
		m_pParseThread = AfxBeginThread(ThreadParseProc, this, m_nParseThreadPriority);
		if (!m_pParseThread)
		{
			ASSERT(FALSE);
			return;
		}
	}

	if (!SetParseThreadPriority(m_nParseThreadPriority))
	{
		// try to start a new thread
		StartParseInThread(pBuffer, pLCStart, pLCEnd, eEdinAction, bRunWithoutWait);
	}

	CString sDBGpos = DBG_TraceIZone(pLCStart, pLCEnd);
	DBG_TRACE_PARSE_START_STOP(_T("- Parser set START Event. Add invalid Zone [ %s ] \n"), (LPCTSTR)sDBGpos);

	if (bRunWithoutWait)
	{
		VERIFY( m_PThreadParams.evRunWithoutWait.SetEvent() );
	}
	VERIFY( m_PThreadParams.evParseRun.SetEvent() );
}

void CXTPSyntaxEditLexParser::StopParseInThread()
{
	DBG_TRACE_TIME(1, _T("### ### ### STOP Parse In Thread time = "));

	CSingleLock singLockMain(&m_csParserData);

	if (!m_ptrTextSchema)
	{
		return;
	}

	CSingleLock singLockPrm(&m_PThreadParams.lockThreadParams);
	CEvent* pBreakEvent = m_ptrTextSchema->GetBreakParsingEvent();
	ASSERT(pBreakEvent);

	if (IsMutexLocked(&m_PThreadParams.lockThreadParams))
	{
		VERIFY( pBreakEvent->SetEvent() );

		DBG_TRACE_PARSE_START_STOP(_T("- STOP Parser set BREAK Event. \n"));
	}

	if (!singLockPrm.Lock())
	{
		ASSERT(FALSE);
		return;
	}
	if (IsEventSet(*pBreakEvent))
	{
		VERIFY( pBreakEvent->ResetEvent() );

		DBG_TRACE_PARSE_START_STOP(_T("- STOP Parser REset BREAK Event. \n"));
	}

	DBG_TRACE_PARSE_START_STOP(_T("- STOP Parser finished. \n"));
}

int CXTPSyntaxEditLexParser::GetParseThreadPriority()
{
	try
	{
		if (m_pParseThread)
		{
			int nPriority = m_pParseThread->GetThreadPriority();
			return nPriority;
		}
	}
	catch (...)
	{
		m_pParseThread = NULL;
		TRACE(_T("ERROR! Parse Thread is not exist. \n"));
	};

	return m_nParseThreadPriority;
}

BOOL CXTPSyntaxEditLexParser::SetParseThreadPriority(int nPriority)
{
	m_nParseThreadPriority = nPriority;
	try
	{
		if (m_pParseThread)
		{
			if (m_nParseThreadPriority != m_pParseThread->GetThreadPriority())
			{
				m_pParseThread->SetThreadPriority(m_nParseThreadPriority);
			}
		}
	}
	catch (...)
	{
		TRACE(_T("ERROR! Parse Thread is not exist. \n"));

		m_pParseThread = NULL;
		return FALSE;
	}
	return TRUE;
}

void CXTPSyntaxEditLexParser::OnBeforeEditChanged()
{
	//CSingleLock singLockMain(&m_csParserData);

	StopParseInThread();
}

void CXTPSyntaxEditLexParser::OnEditChanged(const XTP_EDIT_LINECOL& posFrom,
						  const XTP_EDIT_LINECOL& posTo, int eEditAction,
						  CXTPSyntaxEditBufferManager* pBuffer)
{
	DBG_TRACE_TIME(1, _T("### ### ### OnEditChanged time = "));

	CSingleLock singLockMain(&m_csParserData);

	if (!m_ptrTextSchema)
	{
		return;
	}

	if (!pBuffer)
	{
		ASSERT(FALSE);
		return;
	}


	m_ptrTextSchema->UpdateTextBlocks(posFrom, posTo, eEditAction);

	BOOL bThreadParse = GetSchemaOptions(pBuffer->GetFileExt())->m_bEditReparceInSeparateThread;
	if (bThreadParse)
	{
		StartParseInThread(pBuffer, &posFrom, &posTo, eEditAction, FALSE);
	}
	else
	{
		CXTPSyntaxEditTextIterator txtIter(pBuffer);
		m_ptrTextSchema->RunParseUpdate(TRUE, &txtIter, &posFrom, &posTo);
	}
}

void CXTPSyntaxEditLexParser::OnParseEvent_NotificationHandler(XTP_NOTIFY_CODE Event,
													 WPARAM wParam, LPARAM lParam)
{
	if (Event == xtpEditOnTextBlockParsed)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrTBended;

		CXTPSyntaxEditLexTextSchema* ptrTextSch = GetTextSchema();
		ptrTBended = ptrTextSch ? ptrTextSch->GetLastParsedBlock(wParam) : NULL;

		if (ptrTBended)
		{
			m_pConnect->SendEvent(Event, (WPARAM)(CXTPSyntaxEditLexTextBlock*)ptrTBended, 0);
		}
	}
	else
	{
		m_pConnect->SendEvent(Event, wParam, lParam);
	}
}
UINT CXTPSyntaxEditLexParser::ThreadParseProc(LPVOID pParentParser)
{
	DBG_TRACE_PARSE_START_STOP(_T("*** Parser Thread is started. %08x\n"), ::GetCurrentThreadId());

	if (!pParentParser)
	{
		ASSERT(FALSE);
		return 111;
	}

//  try
	{
		CXTPSyntaxEditLexParserPtr ptrParser((CXTPSyntaxEditLexParser*)pParentParser, TRUE);
		CXTPSyntaxEditLexParser::CXTPSyntaxEditParseThreadParams* pPRMs = NULL;
		pPRMs = ptrParser->GetParseInThreadParams();
		if (!pPRMs)
		{
			ASSERT(FALSE);
			return 222;
		}
		CSingleLock lockPRMs(&pPRMs->lockThreadParams, TRUE);

		HANDLE arWaiters[] = {pPRMs->evParseRun, pPRMs->evExitThread};
		HANDLE hRunWithoutWait = pPRMs->evRunWithoutWait;

		DWORD dwSelfCloseTimeout_ms = ptrParser->GetSchemaOptions(
				pPRMs->ptrBuffer->GetFileExt())->m_dwParserThreadIdleLifeTime_ms;

		DWORD dwWaitFilter_ms = ptrParser->GetSchemaOptions(
					pPRMs->ptrBuffer->GetFileExt())->m_dwEditReparceTimeout_ms;

		lockPRMs.Unlock();

		DWORD dwWaitRes = 0;
		do
		{
			dwWaitRes = WaitForMultipleObjects(2, arWaiters, FALSE, dwSelfCloseTimeout_ms);

			if (dwWaitRes == WAIT_TIMEOUT)
			{
				CSingleLock lockPRMs1(&pPRMs->lockThreadParams, TRUE);

				if (pPRMs->arInvalidZones.GetSize() == 0)
				{
					ptrParser->SelfCloseParseThread();
					return 1;
				}
				else
				{
					continue;
				}
			}

			//=== Wait Filter === (for keyboard input)
			if (dwWaitRes == WAIT_OBJECT_0 && !IsEventSet(hRunWithoutWait))
			{
				do {
					dwWaitRes = WaitForMultipleObjects(2, arWaiters, FALSE, dwWaitFilter_ms);
				}
				while (dwWaitRes == WAIT_OBJECT_0 && !IsEventSet(hRunWithoutWait));

				if (dwWaitRes == WAIT_TIMEOUT)
				{
					dwWaitRes = WAIT_OBJECT_0;
				}
			}
			//=== Wait Filter === (for keyboard input)

			if (dwWaitRes == WAIT_OBJECT_0)
			{
				//***
				CSingleLock lockPRMs2(&pPRMs->lockThreadParams, TRUE);
				//***

				// Update Options
				dwSelfCloseTimeout_ms = ptrParser->GetSchemaOptions(
					pPRMs->ptrBuffer->GetFileExt())->m_dwParserThreadIdleLifeTime_ms;

				dwWaitFilter_ms = ptrParser->GetSchemaOptions(
					pPRMs->ptrBuffer->GetFileExt())->m_dwEditReparceTimeout_ms;
				// END Update Options

				CXTPSyntaxEditTextIterator txtIter(pPRMs->ptrBuffer);

				CXTPSyntaxEditLexTextSchema* ptrTextSch = ptrParser->GetTextSchema();
				if (!ptrTextSch)
				{
					continue;
				}
				CEvent* pBreakEvent = ptrTextSch->GetBreakParsingEvent();
				ASSERT(pBreakEvent);

				int nZonesCount = 0;
				BOOL bParseRestedBlock = FALSE;
				BOOL bBreaked = FALSE;
				do
				{
					if (IsEventSet(*pBreakEvent))
					{
						VERIFY( pBreakEvent->ResetEvent() );
						DBG_TRACE_PARSE_START_STOP(_T("* Parser Start BREAKED. \n"));
						break;
					}

					XTP_EDIT_LINECOL* pLCStart = NULL;
					XTP_EDIT_LINECOL* pLCEnd = NULL;
					CXTPSyntaxEditTextRegion iZone;

					nZonesCount = (int)pPRMs->arInvalidZones.GetSize();
					//bParseRestedBlock = FALSE; //nZonesCount > 0;

					if (nZonesCount)
					{
						iZone = pPRMs->arInvalidZones[nZonesCount-1];

						if (iZone.m_posStart.IsValidData())
						{
							pLCStart = &iZone.m_posStart;
						}
						if (iZone.m_posEnd.IsValidData())
						{
							pLCEnd = &iZone.m_posEnd;
						}
					}

					CString sDBGpos = DBG_TraceIZone(pLCStart, pLCEnd);
					DBG_TRACE_PARSE_START_STOP(_T("* Parser Started. Invalid Zone [%s] \n"), sDBGpos);

					// run parser
					/*DEBUG*/ DWORD dwTime0 = GetTickCount();   //DEBUG

					int nParseRes = ptrTextSch->RunParseUpdate(TRUE, &txtIter,
													pLCStart, pLCEnd, TRUE);
					if (nParseRes & xtpEditLPR_Error)
					{
						//ASSERT(FALSE);
						//::MessageBeep((UINT)-1);
						TRACE(_T("Lex Parser ERROR! Try Full reparse. \n"));

						ptrTextSch->RemoveAll();

						XTP_EDIT_LINECOL posLC1 = {1,0};
						txtIter.SeekBegin();

						nParseRes = ptrTextSch->RunParseUpdate(TRUE, &txtIter,
																&posLC1, NULL);

						pPRMs->arInvalidZones.RemoveAll();
						nZonesCount = 0;

						if (nParseRes & xtpEditLPR_RunFinished)
						{
							TRACE(_T("Full reparse - (OK) <F I N I S H E D>  \n"));
						}
						else if (nParseRes & xtpEditLPR_RunBreaked)
						{
							TRACE(_T("Full reparse - BREAKED  \n"));

							iZone.Set(NULL, NULL);
							pPRMs->arInvalidZones.Add(iZone);
						}
						else if (nParseRes & xtpEditLPR_Error)
						{
							TRACE(_T("Full reparse - ERROR  \n"));
						}
						nZonesCount = (int)pPRMs->arInvalidZones.GetSize();
					}

					/*DEBUG*/ DWORD dwTime1 = GetTickCount();//DEBUG

					bBreaked = (nParseRes & xtpEditLPR_RunFinished) == 0 ||
								(nParseRes & (xtpEditLPR_RunBreaked|xtpEditLPR_Error)) > 0 ||
								IsEventSet(*pBreakEvent);

					VERIFY( pBreakEvent->ResetEvent() );

					CXTPSyntaxEditTextRegion zoneValid = ptrTextSch->GetUpdatedTextRegion();

					if ((nParseRes&xtpEditLPR_RunFinished) && nZonesCount)
					{
						pPRMs->arInvalidZones.RemoveAt(nZonesCount-1);
						nZonesCount--;  //pPRMs->UpdateZones(zoneValid);
					}

					//DEBUG
					{
						CString sDBGzone = DBG_TraceIZone(&zoneValid.m_posStart, &zoneValid.m_posEnd);
						//TRACE(_T("- Parser set START Event. Add invalid Zone [ %s ] \n"), sDBGpos);
						DBG_TRACE_PARSE_START_STOP(_T("* Parser Ended. (result=%x) %s%s%s (%.3f sec) VALID Zone[ %s ] \n"),
							nParseRes,
							(nParseRes&xtpEditLPR_RunBreaked) ? _T(" BREAKED ") : _T(""),
							(nParseRes&xtpEditLPR_Error) ? _T(" ERROR ") : _T(""),
							(nParseRes&xtpEditLPR_RunFinished) ? _T(" <F I N I S H E D> ") : _T(""),
							labs(dwTime1-dwTime0)/1000.0, (LPCTSTR)sDBGzone);
					}
				}
				while ((bParseRestedBlock || nZonesCount) && !bBreaked);
			}
		}
		while (dwWaitRes != WAIT_OBJECT_0+1);
	}
//  catch(...) {
//      TRACE(_T("* EXCEPTION!!! CXTPSyntaxEditLexParser::ThreadParseProc(2)\n"));
//      return 333;
//  }

	DBG_TRACE_PARSE_START_STOP(_T("*** Parser Thread is Ended. (%x)\n"), ::GetCurrentThreadId());
	return 0;
}

#ifdef _DEBUG
void CXTPSyntaxEditLexTextBlock::Dump( CDumpContext& dc ) const
{

	CObject::Dump( dc );

	// Now do the stuff for our specific class.
	dc << "\t <<TB>>";
	dc << " (ref=" << m_dwRef << ") \n";
	dc << "\t start(" << m_PosStartLC.nLine << ", " << m_PosStartLC.nCol << ") ";
	dc << "\t end(" << m_PosEndLC.nLine << ", " << m_PosEndLC.nCol << ") \n";

#if _MSC_VER >= 1300
	dc << "\t Parent="; dc.DumpAsHex((INT_PTR)(CXTPSyntaxEditLexTextBlock*)m_ptrParent);
	dc << ", Prev=";    dc.DumpAsHex((INT_PTR)(CXTPSyntaxEditLexTextBlock*)m_ptrPrev);
	dc << ", Next=";    dc.DumpAsHex((INT_PTR)(CXTPSyntaxEditLexTextBlock*)m_ptrNext);
	dc << ", LastChild=";   dc.DumpAsHex((INT_PTR)(CXTPSyntaxEditLexTextBlock*)m_ptrLastChild);
#endif

}
#endif

BOOL CXTPSyntaxEditLexTextSchema::RunParseOnScreen(CTextIter* pTxtIter,
										int nRowStart, int nRowEnd,
										CXTPSyntaxEditLexTextBlockPtr& rPtrScreenSchFirstTB
										)
{
	if (!pTxtIter || nRowStart <= 0 || nRowEnd <= 0)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CSingleLock singleLock(GetDataLoker());

	if (!singleLock.Lock(30))
	{
		TRACE(_T("Cannot enter critical section. CXTPSyntaxEditLexTextSchema::RunParseOnScreen() \n"));
		return FALSE;
	}
	//---------------------------------------------------------------------------
	//** (1) ** -------------------------
	CXTPSyntaxEditLexTextBlockPtr ptrTBParentToRun;
	BOOL bInit = InitScreenSch(pTxtIter, nRowStart, nRowEnd, rPtrScreenSchFirstTB,
								ptrTBParentToRun);

	if (!bInit)
	{
		//TRACE(_T("- parser::InitScreenSch return FALSE res.\n"));
		return FALSE;
	}

	XTP_EDIT_LINECOL startLC = {nRowStart, 0};
	if (!pTxtIter->SeekPos(startLC))
	{
		return FALSE;
	}

	int nParseRes = 0;

	CXTPSyntaxEditLexOnScreenParseCnt runCnt;
	runCnt.m_nRowStart  = nRowStart;
	runCnt.m_nRowEnd    = nRowEnd;
	runCnt.m_ptrTBLast  = ptrTBParentToRun;

	//** (2) ** -------------------------
	CXTPSyntaxEditLexClassPtrArray* ptrArClasses = m_pClassSchema->GetClasses(FALSE);
	int nCCount = ptrArClasses ? (int)ptrArClasses->GetSize() : 0;
	BOOL bRunEOF = TRUE;
	XTP_EDIT_LINECOL lcTextPos = {0,0};

	while (!pTxtIter->IsEOF() && nCCount && bRunEOF)
	{
		bRunEOF = !pTxtIter->IsEOF();

		nParseRes = Run_OnScreenTBStack(pTxtIter, ptrTBParentToRun, &runCnt);

		if (nParseRes & (xtpEditLPR_Error|xtpEditLPR_RunBreaked)) // |xtpEditLPR_RunFinished))
		{
			TRACE(_T("- parser::RunParseOnScreen return by erroe or BREAK. \n"));
			return !(nParseRes&xtpEditLPR_Error);
		}

		//** -------------------------------------------------------------
		if (!pTxtIter->IsEOF() && !(nParseRes & xtpEditLPR_Iterated))
		{
			SeekNextEx(pTxtIter, ptrTBParentToRun->m_ptrLexClass, &runCnt);
		}

		//---------------------------------------------------------------------------
		lcTextPos = pTxtIter->GetPosLC();
		if (lcTextPos.nLine > runCnt.m_nRowEnd)
		{
			break;
		}

		//---------------------------------------------------------------------------
		if (ptrTBParentToRun->m_ptrParent)
		{
			ptrTBParentToRun = ptrTBParentToRun->m_ptrParent;
		}
	}
	return TRUE;
}

CXTPSyntaxEditLexTextBlockPtr CXTPSyntaxEditLexTextSchema::InitScreenSch_RunTopClass(CTextIter* pTxtIter)
{
	CXTPSyntaxEditLexTextBlockPtr ptrTBtop;

	CXTPSyntaxEditLexClassPtrArray* ptrClsAr = m_pClassSchema->GetClasses(FALSE);
	int nCount = ptrClsAr ? (int)ptrClsAr->GetSize() : 0;
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClassPtr ptrC = ptrClsAr->GetAt(i);
		if (!ptrC)
		{
			ASSERT(FALSE);
			continue;
		}

		ASSERT(ptrTBtop == NULL);
		int nParseRes = ptrC->RunParse(pTxtIter, this, ptrTBtop);

		if ((nParseRes & xtpEditLPR_StartFound) && ptrTBtop)
		{
			return ptrTBtop;
		}
	}

	return NULL;
}

BOOL CXTPSyntaxEditLexTextSchema::InitScreenSch(CTextIter* pTxtIter, int nRowStart, int nRowEnd,
									 CXTPSyntaxEditLexTextBlockPtr& rPtrScreenSchFirstTB,
									CXTPSyntaxEditLexTextBlockPtr& rPtrTBParentToRun)
{
	CXTPSyntaxEditLexTextBlockPtr ptrTBstart = GetBlocks();

	// process first screen parse in the main thread
	if (nRowStart == 1 && !ptrTBstart)
	{
		rPtrTBParentToRun = rPtrScreenSchFirstTB = InitScreenSch_RunTopClass(pTxtIter);
		return (rPtrTBParentToRun != NULL);
	}

	CXTPSyntaxEditLexTextBlock* pLastSchBlock = GetLastSchBlock(FALSE);
	if (m_ptrNewChainTB2 && pLastSchBlock &&
		(pLastSchBlock->m_PosEndLC.IsValidData() && nRowStart > pLastSchBlock->m_PosEndLC.nLine ||
		!pLastSchBlock->m_PosEndLC.IsValidData() && nRowStart > pLastSchBlock->m_PosStartLC.nLine)
	  )
	{
		return FALSE;
	}

	//=======================================================================
	CXTPSyntaxEditLexTextBlock* pTBLast = NULL;

	XTP_EDIT_LINECOL lcStart = {nRowStart, 0};
	// (1) ---
	CXTPSyntaxEditLexTextBlock* pTB;
	for (pTB = ptrTBstart; pTB; pTB = pTB->m_ptrNext)
	{
		if (pTB->m_PosStartLC < lcStart &&
			pTB->GetPosEndLC() >= lcStart ||
			pTB == ptrTBstart )
		{
			pTBLast = pTB;
		}
		if (pTB->m_PosStartLC.nLine > nRowEnd)
		{
			break;
		}
	}

	// (2) ---
	CXTPSyntaxEditLexTextBlockPtr ptrTBCopyNext;
	for (pTB = pTBLast; pTB; pTB = pTB->m_ptrParent)
	{
		CXTPSyntaxEditLexTextBlockPtr ptrTBCopy = CopyShortTBtoFull(pTB);
		if (!ptrTBCopy)
		{
			return FALSE;
		}

		if (pTB == pTBLast)
		{
			rPtrTBParentToRun = ptrTBCopy;

			if (pTB->m_PosStartLC.nLine <= nRowStart &&
				pTB->m_PosEndLC.nLine >= nRowStart ||
				pTB == ptrTBstart)
			{
				pTBLast = pTB;
			}
		}
		else
		{
			ptrTBCopy->m_ptrNext = ptrTBCopyNext;
			ptrTBCopyNext->m_ptrPrev = ptrTBCopy;

			ptrTBCopyNext->m_ptrParent = ptrTBCopy;
		}

		rPtrScreenSchFirstTB = ptrTBCopy;

		ptrTBCopyNext = ptrTBCopy;
	}

	return (rPtrTBParentToRun != NULL);
}

CXTPSyntaxEditLexTextBlock* CXTPSyntaxEditLexTextSchema::CopyShortTBtoFull(CXTPSyntaxEditLexTextBlock* pTB)
{
	CXTPSyntaxEditLexTextBlockPtr ptrCopyTB = GetNewBlock();

	if (!ptrCopyTB || !pTB || !pTB->m_ptrLexClass)
	{
		ASSERT(pTB && pTB->m_ptrLexClass);
		return NULL;
	}
	ptrCopyTB->m_PosStartLC = pTB->m_PosStartLC;
	ptrCopyTB->m_PosEndLC = pTB->m_PosEndLC;

	int nClassID = pTB->m_ptrLexClass->GetAttribute_int(XTPLEX_ATTRCLASSID, FALSE, -1);
	ASSERT(nClassID > 0);

	CXTPSyntaxEditLexClassPtrArray* ptrTopClassesAr = m_pClassSchema->GetClasses(FALSE);
	ptrCopyTB->m_ptrLexClass = FindLexClassByID(ptrTopClassesAr, nClassID);

	if (!ptrCopyTB->m_ptrLexClass)
	{
		ASSERT(FALSE);
		return NULL;
	}

	return ptrCopyTB.Detach();
}

CXTPSyntaxEditLexClass* CXTPSyntaxEditLexTextSchema::FindLexClassByID(CXTPSyntaxEditLexClassPtrArray* pClassesAr, int nClassID)
{
	if (!pClassesAr)
	{
		return NULL;
	}
	int nCount = (int)pClassesAr->GetSize();
	for (int i = 0; i < nCount; i++)
	{
		CXTPSyntaxEditLexClass* pC = pClassesAr->GetAt(i, FALSE);

		int nC_ID = pC->GetAttribute_int(XTPLEX_ATTRCLASSID, FALSE);

		if (nC_ID == nClassID)
		{
			pC->InternalAddRef();
			return pC;
		}

		//-------------------------------------------------------
		CXTPSyntaxEditLexClass* pCF2 = FindLexClassByID(pC->GetChildren(), nClassID);
		if (pCF2)
		{
			return pCF2;
		}
		pCF2 = FindLexClassByID(pC->GetChildrenDyn(), nClassID);
		if (pCF2)
		{
			return pCF2;
		}
	}
	return NULL;
}

int CXTPSyntaxEditLexTextSchema::Run_OnScreenTBStack(CTextIter* pTxtIter,
											CXTPSyntaxEditLexTextBlock* pTBParentToRun,
											CXTPSyntaxEditLexOnScreenParseCnt* pRunCnt)
{
	if (!pTBParentToRun || !pTBParentToRun->m_ptrLexClass || !pRunCnt)
	{
		ASSERT(FALSE);
		return xtpEditLPR_Error;
	}

	CXTPSyntaxEditLexTextBlockPtr ptrTBParentToRun(pTBParentToRun, TRUE);
	CXTPSyntaxEditLexClassPtr ptrRunClass = pTBParentToRun->m_ptrLexClass;
	int nPres = 0;
	BOOL bIterated = FALSE;

	BOOL bEnded = FALSE;
	BOOL bRunEOF = TRUE;
	XTP_EDIT_LINECOL lcTextPos = {0,0};

	//** 1 **// Run existing block with children until block end
	while (!bEnded && (bRunEOF || !pTxtIter->IsEOF()) )
	{
		bRunEOF = !pTxtIter->IsEOF();

		BOOL bSkipIterate = FALSE;

		nPres = ptrRunClass->RunParse(pTxtIter, this, ptrTBParentToRun, pRunCnt);

		if (nPres & (xtpEditLPR_Error|xtpEditLPR_RunBreaked)) //|xtpEditLPR_RunFinished))
		{
			return nPres;
		}

		//---------------------------------------------------------------------------
		if (nPres & xtpEditLPR_Iterated)
		{
			bSkipIterate = TRUE;
			bIterated = TRUE;
		}

		//---------------------------------------------------------------------------
		bEnded = (nPres & xtpEditLPR_EndFound) != 0;

		if (bEnded)
		{
			CSingleLock singleLock(GetDataLoker(), TRUE);

			ptrTBParentToRun->EndChildren(); //this);

			DBG_TRACE_PARSE_RUN_BLOCKS(_T("(%08x) ENDED startPos=(%d,%d) endPos=(%d,%d), [%s] {%d}-noEndedStack \n"),
				(CXTPSyntaxEditLexTextBlock*)ptrTBParentToRun,
				ptrTBParentToRun->m_PosStartLC.nLine, ptrTBParentToRun->m_PosStartLC.nCol, ptrTBParentToRun->m_PosEndLC.nLine,
				ptrTBParentToRun->m_PosEndLC.nCol, ptrTBParentToRun->m_ptrLexClass->m_strClassName, m_nNoEndedClassesCount);
		}

		//---------------------------------------------------------------------------
		if (!bEnded && !bSkipIterate)
		{
			SeekNextEx(pTxtIter, ptrRunClass, pRunCnt);
		}

		//---------------------------------------------------------------------------
		lcTextPos = pTxtIter->GetPosLC();
		if (lcTextPos.nLine > pRunCnt->m_nRowEnd)
		{
			break;
		}
	}

	// ** end run existing ** //
	if (!bEnded)
	{
		ptrTBParentToRun->m_PosEndLC = pTxtIter->GetPosLC();

		ptrTBParentToRun->EndChildren(); //this);
		return xtpEditLPR_RunFinished;
	}

	//===========================================================================
	return (bIterated ? xtpEditLPR_Iterated : 0) | (pTxtIter->IsEOF() ? xtpEditLPR_RunFinished : 0);
}

CXTPSyntaxEditLexParserSchemaOptions::CXTPSyntaxEditLexParserSchemaOptions()
{
	m_bFirstParseInSeparateThread           = TRUE;
	m_bEditReparceInSeparateThread          = TRUE;
	m_bConfigChangedReparceInSeparateThread = TRUE;
	m_dwMaxBackParseOffset                  = XTP_EDIT_LEXPARSER_MAXBACKOFFSETDEFAULT;
	m_dwEditReparceTimeout_ms               = XTP_EDIT_LEXPARSER_REPARSETIMEOUTMS;
	m_dwOnScreenSchCacheLifeTime_sec        = XTP_EDIT_LEXPARSER_ONSCREENSCHCACHELIFETIMESEC;
	m_dwParserThreadIdleLifeTime_ms         = XTP_EDIT_LEXPARSER_THREADIDLELIFETIMESEC * 1000;
}

CXTPSyntaxEditLexParserSchemaOptions::CXTPSyntaxEditLexParserSchemaOptions(const CXTPSyntaxEditLexParserSchemaOptions& rSrc)
{
	m_bFirstParseInSeparateThread           = rSrc.m_bFirstParseInSeparateThread;
	m_bEditReparceInSeparateThread          = rSrc.m_bEditReparceInSeparateThread;
	m_bConfigChangedReparceInSeparateThread = rSrc.m_bConfigChangedReparceInSeparateThread;
	m_dwMaxBackParseOffset                  = rSrc.m_dwMaxBackParseOffset;
	m_dwEditReparceTimeout_ms               = rSrc.m_dwEditReparceTimeout_ms;
	m_dwOnScreenSchCacheLifeTime_sec        = rSrc.m_dwOnScreenSchCacheLifeTime_sec;
	m_dwParserThreadIdleLifeTime_ms         = rSrc.m_dwParserThreadIdleLifeTime_ms;
}

const CXTPSyntaxEditLexParserSchemaOptions& CXTPSyntaxEditLexParserSchemaOptions::operator=(const CXTPSyntaxEditLexParserSchemaOptions& rSrc)
{
	m_bFirstParseInSeparateThread           = rSrc.m_bFirstParseInSeparateThread;
	m_bEditReparceInSeparateThread          = rSrc.m_bEditReparceInSeparateThread;
	m_bConfigChangedReparceInSeparateThread = rSrc.m_bConfigChangedReparceInSeparateThread;
	m_dwMaxBackParseOffset                  = rSrc.m_dwMaxBackParseOffset;
	m_dwEditReparceTimeout_ms               = rSrc.m_dwEditReparceTimeout_ms;
	m_dwOnScreenSchCacheLifeTime_sec        = rSrc.m_dwOnScreenSchCacheLifeTime_sec;
	m_dwParserThreadIdleLifeTime_ms         = rSrc.m_dwParserThreadIdleLifeTime_ms;

	return *this;
}

BOOL CXTPSyntaxEditLexParser::ReadSchemaOptions(const CString& strExt,
									  CXTPSyntaxEditLexTextSchema* pTextSchema,
									  CXTPSyntaxEditLexParserSchemaOptions* pOpt)
{
	if (pOpt)
	{
		*pOpt = *m_pSchOptions_default;
	}

	if (!pTextSchema || !pOpt)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CXTPSyntaxEditLexClassPtr ptrTopCls = pTextSchema->GetTopClassForFileExt(strExt);
	if (!ptrTopCls)
	{
		return FALSE;
	}

	pOpt->m_bFirstParseInSeparateThread = ptrTopCls->GetAttribute_BOOL(
						XTPLEX_ATTRG_FIRSTPARSEINSEPARATETHREAD, FALSE, TRUE);

	pOpt->m_bEditReparceInSeparateThread = ptrTopCls->GetAttribute_BOOL(
						XTPLEX_ATTRG_EDITREPARCEINSEPARATETHREAD, FALSE, TRUE);

	pOpt->m_bConfigChangedReparceInSeparateThread = ptrTopCls->GetAttribute_BOOL(
				XTPLEX_ATTRG_CONFIGCHANGEDREPARCEINSEPARATETHREAD, FALSE, TRUE);

	pOpt->m_dwMaxBackParseOffset = (DWORD)ptrTopCls->GetAttribute_int(
						XTPLEX_ATTRG_MAXBACKPARSEOFFSET, FALSE,
						XTP_EDIT_LEXPARSER_MAXBACKOFFSETDEFAULT);

	pOpt->m_dwEditReparceTimeout_ms = (DWORD)ptrTopCls->GetAttribute_int(
						XTPLEX_ATTRG_EDITREPARCETIMEOUT_MS, FALSE,
						XTP_EDIT_LEXPARSER_REPARSETIMEOUTMS);

	pOpt->m_dwOnScreenSchCacheLifeTime_sec = (DWORD)ptrTopCls->GetAttribute_int(
						XTPLEX_ATTRG_ONSCREENSCHCACHELIFETIME_SEC, FALSE,
						XTP_EDIT_LEXPARSER_ONSCREENSCHCACHELIFETIMESEC);

	pOpt->m_dwParserThreadIdleLifeTime_ms = (DWORD)ptrTopCls->GetAttribute_int(
						XTPLEX_ATTRG_PARSERTHREADIDLELIFETIME_SEC, FALSE,
						XTP_EDIT_LEXPARSER_THREADIDLELIFETIMESEC);

	if (pOpt->m_dwParserThreadIdleLifeTime_ms != INFINITE)
	{
		pOpt->m_dwParserThreadIdleLifeTime_ms *= 1000;
	}
	if (pOpt->m_dwParserThreadIdleLifeTime_ms == 0)
	{
		pOpt->m_dwParserThreadIdleLifeTime_ms = INFINITE;
	}

	return TRUE;
}

const CXTPSyntaxEditLexParserSchemaOptions* CXTPSyntaxEditLexParser::GetSchemaOptions(const CString& strExt)
{
	CSingleLock singLockMain(&m_csParserData);

	if (!m_ptrTextSchema)
	{
		RemoveAllOptions();
	}

	CXTPSyntaxEditLexParserSchemaOptions* pOptions = NULL;

	if (m_mapSchOptions.Lookup(strExt, pOptions))
	{
		ASSERT(pOptions);
		return pOptions;
	}
	else if (m_ptrTextSchema)
	{
		pOptions = new CXTPSyntaxEditLexParserSchemaOptions();

		if (ReadSchemaOptions(strExt, m_ptrTextSchema, pOptions))
		{
			m_mapSchOptions.SetAt(strExt, pOptions);
			return pOptions;
		}
		else
		{
			CMDTARGET_RELEASE(pOptions);
		}
	}

	return m_pSchOptions_default;
}
