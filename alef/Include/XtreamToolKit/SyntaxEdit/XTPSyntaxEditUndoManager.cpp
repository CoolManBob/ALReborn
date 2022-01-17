// XTPSyntaxEditUndoManager.cpp : implementation file
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
#include "Common/XTPImageManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPResourceManager.h"

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

/////////////////////////////////////////////////////////////////////////////

CXTPSyntaxEditUndoRedoManager::CXTPSyntaxEditUndoRedoManager()
{
	m_posSavedMark = m_posFirstUndo = NULL;
	//m_posSavedMark = BEFORE_START_POSITION;
	m_bGroupInsertMode = FALSE;
}

CXTPSyntaxEditUndoRedoManager::~CXTPSyntaxEditUndoRedoManager()
{
	Clear();
}

int CXTPSyntaxEditUndoRedoManager::DoUndo(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	int nRet = 0;
	if (m_posFirstUndo != NULL)
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetAt(m_posFirstUndo);
		if (pCmd != NULL)
		{
			nRet = pCmd->UnExecute(lcFrom, lcTo, pEditCtrl);
		}
		m_CommandList.GetPrev(m_posFirstUndo);
	}
	return nRet;
}

int CXTPSyntaxEditUndoRedoManager::DoRedo(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	int nRet = 0;
	// get next item
	if (m_posFirstUndo != NULL)
	{
		m_CommandList.GetNext(m_posFirstUndo);
	}
	else
	{
		m_posFirstUndo = m_CommandList.GetHeadPosition();
	}
	// do action
	if (m_posFirstUndo != NULL)
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetAt(m_posFirstUndo);
		if (pCmd != NULL)
		{
			nRet = pCmd->Execute(lcFrom, lcTo, pEditCtrl);
		}
	}
	return nRet;
}

void CXTPSyntaxEditUndoRedoManager::RemoveTail()
{
	// remove tail of the list
	for (POSITION posTail = m_CommandList.GetTailPosition();
		posTail != m_posFirstUndo;
		posTail = m_CommandList.GetTailPosition())
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetTail();
		m_CommandList.RemoveTail();
		delete pCmd;
	}
}

void CXTPSyntaxEditUndoRedoManager::AddCommand(CXTPSyntaxEditCommand* pCommand)
{
	RemoveTail();

	// insert undo command
	if (!m_bGroupInsertMode)
	{
		m_CommandList.AddTail(pCommand);
	}
	else
	{
		CXTPSyntaxEditCommand* pCmd = m_CommandList.GetTailPosition() ?
			(CXTPSyntaxEditCommand*)m_CommandList.GetTail() : NULL;
		CXTPSyntaxEditBatchCommand* pBatchCmd = DYNAMIC_DOWNCAST(CXTPSyntaxEditBatchCommand, pCmd);
		if (!pBatchCmd)
		{
			pBatchCmd = new CXTPSyntaxEditBatchCommand();
			m_CommandList.AddTail(pBatchCmd);
		}
		pBatchCmd->AddCommand(pCommand);
	}
	m_posFirstUndo = m_CommandList.GetTailPosition();
}

void CXTPSyntaxEditUndoRedoManager::Clear()
{
	for (POSITION posTail = m_CommandList.GetTailPosition();
		posTail != NULL;
		posTail = m_CommandList.GetTailPosition())
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetTail();
		m_CommandList.RemoveTail();
		delete pCmd;
	}
	m_posFirstUndo = NULL;
}

BOOL CXTPSyntaxEditUndoRedoManager::CanUndo()
{
	if (m_posFirstUndo == NULL)
		return FALSE;

	return TRUE;
}

BOOL CXTPSyntaxEditUndoRedoManager::CanRedo()
{
	POSITION posLast = m_CommandList.GetTailPosition();
	if (posLast == m_posFirstUndo)
		return FALSE;

	return TRUE;
}

void CXTPSyntaxEditUndoRedoManager::MarkSaved()
{
	m_posSavedMark = m_posFirstUndo;
}

BOOL CXTPSyntaxEditUndoRedoManager::IsModified()
{
	return m_posSavedMark != m_posFirstUndo;
}

void CXTPSyntaxEditUndoRedoManager::SetGroupInsertMode(BOOL bInsertInGroup)
{
	if (m_bGroupInsertMode == bInsertInGroup)
		return;

	m_bGroupInsertMode = bInsertInGroup;
	if (m_bGroupInsertMode)
	{
		// add empty batch command
		RemoveTail();
		m_CommandList.AddTail(new CXTPSyntaxEditBatchCommand());
		m_posFirstUndo = m_CommandList.GetTailPosition();
	}
	else
	{
		// delete empty batch command if necessary
		CXTPSyntaxEditCommand* pCmd = m_CommandList.GetTailPosition() ?
			(CXTPSyntaxEditCommand*)m_CommandList.GetTail() : NULL;
		CXTPSyntaxEditBatchCommand* pBatchCmd = DYNAMIC_DOWNCAST(CXTPSyntaxEditBatchCommand, pCmd);
		if (pBatchCmd && pBatchCmd->GetCommandsCount() < 1)
		{
			m_CommandList.RemoveTail();
			delete pBatchCmd;
			m_posFirstUndo = m_CommandList.GetTailPosition();
		}
	}
	// m_posFirstUndo = m_CommandList.GetTailPosition(); By Leva - execute this statement only if real changes was made
}

const CStringList& CXTPSyntaxEditUndoRedoManager::GetUndoTextList()
{
	m_lstUndoText.RemoveAll();

	// iterate all undo items
	POSITION posHead = m_CommandList.GetHeadPosition();
	do
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetAt(posHead);
		if (pCmd)
		{
			m_lstUndoText.AddTail(pCmd->GetCommandText());
		}
		if (posHead == m_posFirstUndo)
			break;
		else
			m_CommandList.GetNext(posHead);
	}
	while (posHead != NULL);

	return m_lstUndoText;
}

const CStringList& CXTPSyntaxEditUndoRedoManager::GetRedoTextList()
{
	m_lstRedoText.RemoveAll();

	// iterate all redo items
	for (POSITION posTail = m_CommandList.GetTailPosition();
		posTail != m_posFirstUndo;
		m_CommandList.GetPrev(posTail))
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetAt(posTail);
		if (pCmd)
		{
			m_lstRedoText.AddHead(pCmd->GetCommandText());
		}
	}

	return m_lstRedoText;
}

void CXTPSyntaxEditUndoRedoManager::ChainLastCommand()
{
	POSITION posPrev = m_posFirstUndo;

	// Take latest command and the previous, if any not exists - return
	CXTPSyntaxEditCommand* pCmd = posPrev ?
		(CXTPSyntaxEditCommand*)m_CommandList.GetPrev(posPrev) : NULL;
	if (!pCmd || !posPrev)
		return;

	CXTPSyntaxEditCommand* pPrevCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetAt(posPrev);
	if (!pPrevCmd)
		return;

	// Convert previous command to batch, or create new batch if not exist
	CXTPSyntaxEditBatchCommand* pBatchCmd = DYNAMIC_DOWNCAST(CXTPSyntaxEditBatchCommand, pPrevCmd);
	if (!pBatchCmd)
	{
		// Create batch command
		pBatchCmd = new CXTPSyntaxEditBatchCommand();
		// add previous command to the batch
		pBatchCmd->AddCommand(pPrevCmd);
		// reinsert batch command instead of usual previous command
		m_CommandList.SetAt(posPrev, pBatchCmd);
	}
	pBatchCmd->AddCommand(pCmd);

	// make previous batch command as latest undo position
	m_CommandList.RemoveAt(m_posFirstUndo);
	m_posFirstUndo = posPrev;
}

void CXTPSyntaxEditUndoRedoManager::SetLastCommandText(LPCTSTR szText)
{
	// Take the latest command
	CXTPSyntaxEditCommand* pCmd = m_posFirstUndo ?
		(CXTPSyntaxEditCommand*)m_CommandList.GetAt(m_posFirstUndo) : NULL;
	if (!pCmd)
		return;

	// Set its text
	pCmd->SetCommandText(szText);
}

void CXTPSyntaxEditUndoRedoManager::SetLastCommandText(UINT nTextId)
{
	// load text from resources
	CString strText;
	XTPResourceManager()->LoadString(&strText, nTextId);

	// run text changing
	SetLastCommandText(strText);
}
//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditCommand, CObject)

CXTPSyntaxEditCommand::CXTPSyntaxEditCommand()
{
	XTPResourceManager()->LoadString(&m_strCommandText, XTP_IDS_EDIT_UNDEFINED);
}

CXTPSyntaxEditCommand::~CXTPSyntaxEditCommand()
{
}

void CXTPSyntaxEditCommand::SetPositionInternally(CXTPSyntaxEditCtrl* pEditCtrl, const XTP_EDIT_LINECOL& lcPos)
{
	if (pEditCtrl)
	{
		pEditCtrl->SetCurrentDocumentRow(lcPos.nLine);
		pEditCtrl->m_nCurrentCol = lcPos.nCol;
	}
}
//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditBatchCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditBatchCommand, CXTPSyntaxEditCommand)

CXTPSyntaxEditBatchCommand::CXTPSyntaxEditBatchCommand()
{
	XTPResourceManager()->LoadString(&m_strCommandText, XTP_IDS_EDIT_MULTIPLE);
}

CXTPSyntaxEditBatchCommand::~CXTPSyntaxEditBatchCommand()
{
	Clear();
}

int CXTPSyntaxEditBatchCommand::Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	int nRet = 0;
	lcFrom = XTP_EDIT_LINECOL::MAXPOS;
	lcTo = XTP_EDIT_LINECOL::MINPOS;

	POSITION posCmd = m_CommandList.GetTailPosition();
	for ( int i = (int)m_CommandList.GetCount() - 1; i >= 0; i-- )
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetPrev(posCmd);
		if (pCmd != NULL)
		{
			XTP_EDIT_LINECOL lcTmpFrom, lcTmpTo;
			nRet |= pCmd->Execute(lcTmpFrom, lcTmpTo, pEditCtrl);
			lcFrom = min(lcFrom, lcTmpFrom);
			lcTo = max(lcTo, lcTmpTo);
		}
	}
	return nRet;
}

int CXTPSyntaxEditBatchCommand::UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	int nRet = 0;
	lcFrom = XTP_EDIT_LINECOL::MAXPOS;
	lcTo = XTP_EDIT_LINECOL::MINPOS;

	POSITION posCmd = m_CommandList.GetHeadPosition();
	for ( int i = 0; i < m_CommandList.GetCount(); i++ )
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetNext(posCmd);
		if (pCmd != NULL)
		{
			XTP_EDIT_LINECOL lcTmpFrom, lcTmpTo;
			nRet |= pCmd->UnExecute(lcTmpFrom, lcTmpTo, pEditCtrl);
			lcFrom = min(lcFrom, lcTmpFrom);
			lcTo = max(lcTo, lcTmpTo);
		}
	}
	return nRet;
}

POSITION CXTPSyntaxEditBatchCommand::AddCommand(CXTPSyntaxEditCommand* pCmd)
{
	// insert command
	return m_CommandList.AddHead(pCmd);
}

void CXTPSyntaxEditBatchCommand::Clear()
{
	for ( POSITION posTail = m_CommandList.GetTailPosition();
		posTail != NULL;
		posTail = m_CommandList.GetTailPosition() )
	{
		CXTPSyntaxEditCommand* pCmd = (CXTPSyntaxEditCommand*)m_CommandList.GetTail();
		m_CommandList.RemoveTail();
		delete pCmd;
	}
}

int CXTPSyntaxEditBatchCommand::GetCommandsCount()
{
	return (int)m_CommandList.GetCount();
}
//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditBufferCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditBufferCommand, CXTPSyntaxEditCommand)

CXTPSyntaxEditBufferCommand::CXTPSyntaxEditBufferCommand(
		CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo)
	: m_pBufferMgr(pMgr), m_strText(szText), m_lcFrom(lcFrom), m_lcTo(lcTo)
{
}

CXTPSyntaxEditBufferCommand::~CXTPSyntaxEditBufferCommand()
{
}

CXTPSyntaxEditBufferCommand::CXTPSyntaxEditBufferKeepOverwriteSettings::CXTPSyntaxEditBufferKeepOverwriteSettings(CXTPSyntaxEditBufferManager* pBufferMgr)
: m_pBufMgr(pBufferMgr)
{
	m_bOldSettings = m_pBufMgr ? m_pBufMgr->GetOverwriteFlag() : FALSE;
	if (m_pBufMgr)
	{
		m_pBufMgr->SetOverwriteFlag(FALSE);
	}
}

CXTPSyntaxEditBufferCommand::CXTPSyntaxEditBufferKeepOverwriteSettings::~CXTPSyntaxEditBufferKeepOverwriteSettings()
{
	if (m_pBufMgr)
	{
		m_pBufMgr->SetOverwriteFlag(m_bOldSettings);
	}
}

int CXTPSyntaxEditBufferCommand::DoDeleteText(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	if (!m_pBufferMgr)
		return 0;

	CXTPSyntaxEditBufferKeepOverwriteSettings bufOwr(m_pBufferMgr);

	// performs undoing operation
	m_pBufferMgr->DeleteText(m_lcFrom.nLine, m_lcFrom.nCol, m_lcTo.nLine, m_lcTo.nCol, FALSE);

	// send corresponding notification
	m_pBufferMgr->GetLexParser()->OnEditChanged(m_lcFrom, m_lcTo, xtpEditActDelete, m_pBufferMgr);

	// adjust cursor position
	SetPositionInternally(pEditCtrl, m_lcFrom);

	// calculate edit action
	int nEditAction = XTP_EDIT_EDITACTION_DELETETEXT;
	if (m_lcFrom.nLine != m_lcTo.nLine)
		nEditAction |= XTP_EDIT_EDITACTION_DELETEROW | XTP_EDIT_EDITACTION_MODIFYROW;
	else if (m_lcFrom.nCol != m_lcTo.nCol)
		nEditAction |= XTP_EDIT_EDITACTION_MODIFYROW;

	// set edit action bounds
	lcFrom = min(m_lcFrom, m_lcTo);
	lcTo = max(m_lcFrom, m_lcTo);

	return nEditAction;
}

int CXTPSyntaxEditBufferCommand::DoInsertText(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	if (!m_pBufferMgr)
		return 0;

	// performs undoing operation
	CXTPSyntaxEditBufferKeepOverwriteSettings bufOwr(m_pBufferMgr);

	m_pBufferMgr->InsertText(m_strText, m_lcFrom.nLine, m_lcFrom.nCol, FALSE);

	// send corresponding notification
	m_pBufferMgr->GetLexParser()->OnEditChanged(m_lcFrom, m_lcTo, xtpEditActInsert, m_pBufferMgr);

	// adjust cursor position
	SetPositionInternally(pEditCtrl, m_lcTo);

	// calculate edit action
	int nEditAction = XTP_EDIT_EDITACTION_INSERTTEXT;
	if (m_lcFrom.nLine != m_lcTo.nLine)
		nEditAction |= XTP_EDIT_EDITACTION_INSERTROW | XTP_EDIT_EDITACTION_MODIFYROW;
	else if (m_lcFrom.nCol != m_lcTo.nCol)
		nEditAction |= XTP_EDIT_EDITACTION_MODIFYROW;

	// set edit action bounds
	lcFrom = min(m_lcFrom, m_lcTo);
	lcTo = max(m_lcFrom, m_lcTo);

	return nEditAction;
}

//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditDeleteStringCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditDeleteStringCommand, CXTPSyntaxEditBufferCommand)

CXTPSyntaxEditDeleteStringCommand::CXTPSyntaxEditDeleteStringCommand(
		CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo)
	: CXTPSyntaxEditBufferCommand(pMgr, szText, lcFrom, lcTo)
{
	XTPResourceManager()->LoadString(&m_strCommandText, XTP_IDS_EDIT_DELETE);
	m_strCommandText += m_strText;
}

CXTPSyntaxEditDeleteStringCommand::~CXTPSyntaxEditDeleteStringCommand()
{
}

int CXTPSyntaxEditDeleteStringCommand::Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	return DoDeleteText(lcFrom, lcTo, pEditCtrl);
}

int CXTPSyntaxEditDeleteStringCommand::UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	int nResult = DoInsertText(lcFrom, lcTo, pEditCtrl);

	// adjust cursor position
	//SetPositionInternally(pEditCtrl, lcFrom);

	return nResult;
}
//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditInsertStringCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditInsertStringCommand, CXTPSyntaxEditBufferCommand)

CXTPSyntaxEditInsertStringCommand::CXTPSyntaxEditInsertStringCommand(
		CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo)
   : CXTPSyntaxEditBufferCommand(pMgr, szText, lcFrom, lcTo)
{
	XTPResourceManager()->LoadString(&m_strCommandText, XTP_IDS_EDIT_INSERT);
	m_strCommandText += m_strText;
}

CXTPSyntaxEditInsertStringCommand::~CXTPSyntaxEditInsertStringCommand()
{
}

int CXTPSyntaxEditInsertStringCommand::Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	return DoInsertText(lcFrom, lcTo, pEditCtrl);
}

int CXTPSyntaxEditInsertStringCommand::UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	return DoDeleteText(lcFrom, lcTo, pEditCtrl);
}

//////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditReplaceStringCommand
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPSyntaxEditReplaceStringCommand, CXTPSyntaxEditBufferCommand)

CXTPSyntaxEditReplaceStringCommand::CXTPSyntaxEditReplaceStringCommand(
	CXTPSyntaxEditBufferManager* pMgr,
	LPCTSTR szText, LPCTSTR szReplacedText,
	const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo)
	: CXTPSyntaxEditBufferCommand(pMgr, szText, lcFrom, lcTo), m_strReplacedText(szReplacedText)
{
	XTPResourceManager()->LoadString(&m_strCommandText, XTP_IDS_EDIT_MULTIPLE);
	m_strCommandText += m_strText;
}

CXTPSyntaxEditReplaceStringCommand::~CXTPSyntaxEditReplaceStringCommand()
{
}

int CXTPSyntaxEditReplaceStringCommand::Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	return DoReplaceText(m_strText, lcFrom, lcTo, pEditCtrl);
}

int CXTPSyntaxEditReplaceStringCommand::UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	return DoReplaceText(m_strReplacedText, lcFrom, lcTo, pEditCtrl);
}

int CXTPSyntaxEditReplaceStringCommand::DoReplaceText(LPCTSTR szText, XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl)
{
	if (!m_pBufferMgr)
		return 0;

	// performs undoing operation
	CXTPSyntaxEditBufferKeepOverwriteSettings bufOwr(m_pBufferMgr);
	m_pBufferMgr->SetOverwriteFlag(TRUE);

	XTP_EDIT_LINECOL lcBegin = min(m_lcFrom, m_lcTo);
	m_pBufferMgr->InsertText(szText, lcBegin.nLine, lcBegin.nCol, FALSE);
	// send corresponding notification
	m_pBufferMgr->GetLexParser()->OnEditChanged(m_lcFrom, m_lcTo, xtpEditActInsert, m_pBufferMgr);

	// adjust cursor position
	SetPositionInternally(pEditCtrl, m_lcFrom);

	// calculate edit action
	int nEditAction = XTP_EDIT_EDITACTION_INSERTTEXT | XTP_EDIT_EDITACTION_DELETETEXT;
	if (m_lcFrom.nLine != m_lcTo.nLine)
		nEditAction |= XTP_EDIT_EDITACTION_INSERTROW | XTP_EDIT_EDITACTION_DELETEROW | XTP_EDIT_EDITACTION_MODIFYROW;
	else if (m_lcFrom.nCol != m_lcTo.nCol)
		nEditAction |= XTP_EDIT_EDITACTION_MODIFYROW;

	// set edit action bounds
	lcFrom = min(m_lcFrom, m_lcTo);
	lcTo = max(m_lcFrom, m_lcTo);

	return nEditAction;
}
