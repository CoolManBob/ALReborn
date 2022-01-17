// XTPSyntaxEditUndoManager.h
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
#if !defined(__XTPSYNTAXEDITUNDOMANAGER_H__)
#define __XTPSYNTAXEDITUNDOMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSyntaxEditBufferManager;
class CXTPSyntaxEditCtrl;

//===========================================================================
// Summary:
//      This class represents abstract Edit Control command interface.
//      Its descendants must implement custom Execute and UnExecute
//      functionality depending on a command type.
//
//      This class is used internally by the library only.
//
// See Also: CXTPSyntaxEditUndoRedoManager, CXTPSyntaxEditBatchCommand,
//      CXTPSyntaxEditBufferCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditCommand : public CObject
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object, handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      This is an interface function for operation execution.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the action.
	//      lcTo:   [out] End text position affected by the action.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      UnExecute
	//-----------------------------------------------------------------------
	virtual int Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      This is an interface function for operation undoing.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the action.
	//      lcTo:   [out] End text position affected by the action.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      Execute
	//-----------------------------------------------------------------------
	virtual int UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is responsible for retrieving action description text.
	//
	//      Each command have a string describing contained changes,
	//      for example for showing in the multiple Undo/Redo dialogs, i.e.
	//      "Typing: something", "Delete", "Paste", etc.
	//
	//      This function allows a command to overwrite its own command text
	//      retrieving, for example for adding any specific suffix/prefix, etc.
	//
	// Returns:
	//      Command description string.
	// See also:
	//      SetCommandText
	//-----------------------------------------------------------------------
	virtual LPCTSTR GetCommandText();

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is responsible for redefining action description text.
	//
	//      It allows a command to overwrite command text setting behavior,
	//      for example making a text as a suffix/prefix of the description, etc.
	//
	// Parameters:
	//     szText: [in] New description string for the command.
	//
	// See also:
	//      GetCommandText
	//-----------------------------------------------------------------------
	virtual void SetCommandText(LPCTSTR szText);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Protected constructor because this class could not be instantiated
	//      itself, only by its descendants
	//-----------------------------------------------------------------------
	CXTPSyntaxEditCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets line and a column position for the specified control using
	//      internal fast methods.
	//-----------------------------------------------------------------------
	static void SetPositionInternally(CXTPSyntaxEditCtrl* pEditCtrl, const XTP_EDIT_LINECOL& lcPos);

	CString m_strCommandText; // A text description of the command.
};

AFX_INLINE void CXTPSyntaxEditCommand::SetCommandText(LPCTSTR szText) {
	m_strCommandText = szText;
}

AFX_INLINE LPCTSTR CXTPSyntaxEditCommand::GetCommandText() {
	return m_strCommandText;
}

//===========================================================================
// Summary:
//      This class represents a pack of some commands in one.
//      It stores commands in the list, and implements standard command
//      execution interfaces on command (un)execution.
//
// See Also: CXTPSyntaxEditCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditBatchCommand : public CXTPSyntaxEditCommand
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditBatchCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	CXTPSyntaxEditBatchCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object with all sub-commands,
	//      handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	~CXTPSyntaxEditBatchCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Executes batch command by successive execution of all stored commands.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the actions chain.
	//      lcTo:   [out] End text position affected by the actions chain.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      UnExecute
	//-----------------------------------------------------------------------
	virtual int Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Unexecutes batch command by successive undoing of all stored commands.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the actions chain.
	//      lcTo:   [out] End text position affected by the actions chain.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      Execute
	//-----------------------------------------------------------------------
	virtual int UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Adds a command to the batch buffer.
	// Parameters:
	//      pCommand: [in] A pointer to the newly added command.
	// Returns:
	//      A position of the newly added command inside the internal list.
	// Example:
	//      pBatchCmd->AddCommand(new CXTPSyntaxEditDeleteStringCommand(...));
	//-----------------------------------------------------------------------
	POSITION AddCommand(CXTPSyntaxEditCommand* pCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculates a number of single command items inside the batch
	//      command buffer.
	// Returns:
	//      A number of single command items inside the batch command buffer.
	//-----------------------------------------------------------------------
	int GetCommandsCount();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Clears batch command buffer and delete all commands.
	//-----------------------------------------------------------------------
	void Clear();

	CPtrList m_CommandList;   // Commands buffer storage.
};

//===========================================================================
// Summary:
//      The base class for all edit commands related to CXTPSyntaxEditBufferManager.
//      It stores handle of the buffer manager, and provides implementation
//      of common buffer operations, such as text insertion or deletion.
//
// See Also:
//      CXTPSyntaxEditCommand, CXTPSyntaxEditInsertStringCommand,
//      CXTPSyntaxEditDeleteStringCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditBufferCommand : public CXTPSyntaxEditCommand
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditBufferCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object, handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditBufferCommand();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the buffer command object, initializes its members.
	// Parameters:
	//      pMgr:   [in] Pointer to the associated buffer manager object.
	//      szText: [in] A text which was changed during buffer operation.
	//              It will be used for insert/remove text operations.
	//      lcFrom: [in] Start text position affected by the command.
	//      lcTo:   [in] End text position affected by the command.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditBufferCommand(CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo);

	CXTPSyntaxEditBufferManager* m_pBufferMgr; // Pointer to the associated text buffer.

	//=======================================================================
	// Summary:
	//  Keeps buffer overwrite flag for the specified buffer manager from
	//  class construction until destruction, and resets this flag to FALSE
	//  during class lifetime.
	// Example:
	//  void DoSomething()
	//  {
	//      CXTPSyntaxEditBufferKeepOverwriteSettings bufOwr(m_pBufferMgr);
	//      ...
	//  }
	//=======================================================================
	class CXTPSyntaxEditBufferKeepOverwriteSettings
	{
	public:
		//-------------------------------------------------------------------
		// Summary:
		//      Creates the object and stores buffer overwrite settings.
		// Parameters:
		//      pBufferMgr: [in] Pointer to the associated buffer manager object.
		//-------------------------------------------------------------------
		CXTPSyntaxEditBufferKeepOverwriteSettings(CXTPSyntaxEditBufferManager* pBufferMgr);

		//-------------------------------------------------------------------
		// Summary:
		//      Destroys the object and restore buffer overwrite settings.
		//-------------------------------------------------------------------
		virtual ~CXTPSyntaxEditBufferKeepOverwriteSettings();

	private:
		CXTPSyntaxEditBufferManager* m_pBufMgr; // Associated buffer manager
		BOOL m_bOldSettings;    // Stored buffer overwrite settings.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//      Inserts stored text at the stored positions, and returns start and
	//      final affected text positions.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      DoDeleteText
	//-----------------------------------------------------------------------
	int DoInsertText(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Deletes text at the stored positions, and returns start and
	//      final affected text positions.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      DoInsertText
	//-----------------------------------------------------------------------
	int DoDeleteText(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

protected:
	CString m_strText;      // Stored changed text.
	XTP_EDIT_LINECOL m_lcFrom; // Start edit position.
	XTP_EDIT_LINECOL m_lcTo;       // End edit position.
};

//===========================================================================
// Summary:
//      This class represents text insertion command.
//      Command execution will insert a specified text string in the buffer,
//      un-execution will delete this text from the buffer.
//
// See Also:
//      CXTPSyntaxEditBufferCommand,    CXTPSyntaxEditDeleteStringCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditInsertStringCommand : public CXTPSyntaxEditBufferCommand
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditInsertStringCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the insert string command object, initializes its members.
	// Parameters:
	//      pMgr:   [in] Pointer to the associated buffer manager object.
	//      szText: [in] A text which was changed during buffer operation.
	//              It will be used for insert/remove text operations.
	//      lcFrom: [in] Start text position affected by the command.
	//      lcTo:   [in] End text position affected by the command.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditInsertStringCommand(CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo);

	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object, handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditInsertStringCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Executes insert string command.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      UnExecute
	//-----------------------------------------------------------------------
	virtual int Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Unexecutes insert string command (i.e. deletes the string).
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW
	// See also:
	//      Execute
	//-----------------------------------------------------------------------
	virtual int UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);
};

//===========================================================================
// Summary:
//      This class represents text deletion command.
//      Command execution will delete text from the buffer between specified
//      text positions, its un-execution will insert this text into the buffer.
//
// See Also:
//      CXTPSyntaxEditBufferCommand,    CXTPSyntaxEditInsertStringCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditDeleteStringCommand : public CXTPSyntaxEditBufferCommand
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditDeleteStringCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the delete string command object, initializes its members.
	// Parameters:
	//      pMgr:   [in] Pointer to the associated buffer manager object.
	//      szText: [in] A text which was changed during buffer operation.
	//              It will be used for insert/remove text operations.
	//      lcFrom: [in] Start text position affected by the command.
	//      lcTo:   [in] End text position affected by the command.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditDeleteStringCommand(CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo);

	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object, handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditDeleteStringCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Executes delete string command.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW
	// See also:
	//      UnExecute
	//-----------------------------------------------------------------------
	virtual int Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Unexecutes delete string command (i.e. inserts the string).
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      Execute
	//-----------------------------------------------------------------------
	virtual int UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);
};

//===========================================================================
// Summary:
//      This class represents text replacing command.
//      Command execution will replace text from the buffer between specified
//      text positions, un-execution will do contrary replacement.
//
// See Also:
//      CXTPSyntaxEditBufferCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditReplaceStringCommand : public CXTPSyntaxEditBufferCommand
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPSyntaxEditReplaceStringCommand)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the replace string command object, initializes its members.
	// Parameters:
	//      pMgr:   [in] Pointer to the associated buffer manager object.
	//      szText: [in] A text which was insert during buffer operation.
	//      szReplacedText: [in] A text which was changed during buffer operation.
	//      lcFrom: [in] Start text position affected by the command.
	//      lcTo:   [in] End text position affected by the command.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditReplaceStringCommand(CXTPSyntaxEditBufferManager* pMgr,
		LPCTSTR szText, LPCTSTR szReplacedText, const XTP_EDIT_LINECOL& lcFrom, const XTP_EDIT_LINECOL& lcTo);

	//-----------------------------------------------------------------------
	// Summary:
	//      A default command destructor.
	//      Destroys the command object, handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditReplaceStringCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Executes delete string command.
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW
	// See also:
	//      UnExecute
	//-----------------------------------------------------------------------
	virtual int Execute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Unexecutes delete string command (i.e. inserts the string).
	// Parameters:
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      Execute
	//-----------------------------------------------------------------------
	virtual int UnExecute(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

protected:

	CString m_strReplacedText; // Buffer which holds replaced text.

	//-----------------------------------------------------------------------
	// Summary:
	//      Performs text replacement operation.
	// Parameters:
	//      szText: [in] A text to replace to.
	//      lcFrom: [out] Start text position affected by the command.
	//      lcTo:   [out] End text position affected by the command.
	// Returns:
	//      A bitwise combination of the happened edit actions:
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_INSERTROW
	//-----------------------------------------------------------------------
	int DoReplaceText(LPCTSTR szText, XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

};

//===========================================================================
// Summary:
//      This class maintains the list of undo/redo commands.
//      It is managed by CEditBufferManager class.
//
//      It allows adding new commands in the buffer (see AddCommand()),
//      undoing last added operation (see DoUndo()), and redoing
//      operations following the latest unexecuted (see DoRedo()).
//
//      It also allows checking for the possibility of performing undo/redo
//      operations (see CanUndo() / CanRedo()).
//
//      It also allows marking current position of undo/redo operation queue
//      as saved, and returns "modified" flag if the queue position will
//      be changed (see MarkSaved() / IsModified()).
//
//      Another piece of the functionality is commands grouping. There are
//      following methods related to this: SetGroupInsertMode() and
//      ChainLastCommand(). The second one forces latest added undo
//      command to be merged in group with the previous one. It could be
//      useful for group operations like moving a selected text by mouse.
//      In this case there will be 2 different commands added: remove
//      selected text from its original position, and insert it into the
//      new position. By using ChainLastCommand() methods both these
//      operations will be merged into the single one, which will be
//      further done/undone together.
//      The first method SetGroupInsertMode() forces undo/redo manager
//      to merge in the single chain all added operations. For example,
//      it could be useful when the user is typing some text, which
//      could be later done/undoe in the single word, instead of by each
//      character.
//
//      Also, one more piece of functionality is related to the command
//      description text management. Related functions allow setting the
//      new text description for the last undo command (SetLastCommandText()).
//      As an example, it could be used after ChainLastCommand() method
//      for setting new description for the merged command ('Move' instead
//      of 'Delete'+'Insert').
//      Another 2 functions allow retrieving string description lists
//      for the list of undo and redo commands (GetUndoTextList() /
//      GetRedoTextList()).
//
//      However, this class is used internally by the library only.
//
// See Also:
//      CXTPSyntaxEditCommand
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditUndoRedoManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Creates the undo/redo manager object, initializes its members.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditUndoRedoManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys the undo/redo manager object,
	//      handles its cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditUndoRedoManager();

	//-----------------------------------------------------------------------
	// Summary:
	//      Perform single undo operation:
	//      Unexecutes the latest command.
	// Parameters:
	//      lcFrom    - [out] Start text position affected by the command.
	//      lcTo      - [out] End text position affected by the command.
	//      pEditCtrl - [in] A pointer to edit control.
	// Returns:
	//      A bitwise combination of the affected edit actions.
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      DoRedo
	//-----------------------------------------------------------------------
	int DoUndo(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Perform single redo operation:
	//      Executes the next command.
	// Parameters:
	//      lcFrom    - [out] Start text position affected by the command.
	//      lcTo      - [out] End text position affected by the command.
	//      pEditCtrl - [in] A pointer to edit control.
	// Returns:
	//      A bitwise combination of the affected edit actions.
	//      XTP_EDIT_EDITACTION_MODIFYROW, XTP_EDIT_EDITACTION_DELETEROW, XTP_EDIT_EDITACTION_INSERTROW
	// See also:
	//      DoUndo
	//-----------------------------------------------------------------------
	int DoRedo(XTP_EDIT_LINECOL& lcFrom, XTP_EDIT_LINECOL& lcTo, CXTPSyntaxEditCtrl* pEditCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//      Perform undo operation:
	//      Unexecutes <i>nCount</i> last commands.
	// Parameters:
	//      nCount: [in] A number of commands to execute Undo.
	// See also:
	//      DoRedo
	//-----------------------------------------------------------------------
	//void DoUndo(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Perform redo operation:
	//      Executes <i>nCount</i> next commands.
	// Parameters:
	//      nCount: [in] A number of commands to execute Redo.
	// See also:
	//      DoUndo
	//-----------------------------------------------------------------------
	//void DoRedo(int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//      Add command to the undo buffer.
	//      Delete all redo commands if exist.
	// Parameters:
	//      pCommand:   [in] A pointer of the command to add.
	//-----------------------------------------------------------------------
	void AddCommand(CXTPSyntaxEditCommand* pCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//      Clears the undo manager buffer and delete all commands.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculate possibility to perform undo action.
	// Returns:
	//      TRUE if performing undo action is possible, FALSE otherwise.
	// See also:
	//      CanRedo
	//-----------------------------------------------------------------------
	BOOL CanUndo();

	//-----------------------------------------------------------------------
	// Summary:
	//      Calculate possibility to perform redo action.
	// Returns:
	//      TRUE if performing redo action is possible, FALSE otherwise.
	// See also:
	//      CanUndo
	//-----------------------------------------------------------------------
	BOOL CanRedo();

	//-----------------------------------------------------------------------
	// Summary:
	//      Marks current command buffer position as last saved.
	//      Used for the further calculating modified flag.
	// See also:
	//      IsModified
	//-----------------------------------------------------------------------
	void MarkSaved();

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks whether the document was modified by any command from the
	//      buffer since the last document saving or loading.
	// Returns:
	//      TRUE if any action was performed since last saving, FALSE otherwise.
	// See also:
	//      MarkSaved
	//-----------------------------------------------------------------------
	BOOL IsModified();

	//-----------------------------------------------------------------------
	// Summary:
	//      Group insert mode is required when a user is typing certain text
	//      in normal INS mode. In the time of undo/redo a group of text is
	//      undone in a single shot. This setting is cleared in the case of
	//      overwrite mode or while user presses SPACE, TAB, ENTER etc.
	// Parameters:
	//      bInsertInGroup: [in] Pass TRUE to switch group insert mode on,
	//                           FALSE otherwise
	// See also:
	//      ChainLastCommand
	//-----------------------------------------------------------------------
	void SetGroupInsertMode(BOOL bInsertInGroup = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Forces latest Undo command to merge in batch command with the
	//      previous one.
	// See also:
	//      SetGroupInsertMode
	//-----------------------------------------------------------------------
	void ChainLastCommand();

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the text for the last undo command in the stack.
	// Parameters:
	//      szText: [in] A text to be set on the last undo command.
	// See also:
	//      GetUndoTextList, GetRedoTextList
	//-----------------------------------------------------------------------
	void SetLastCommandText(LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//      Changes the text for the last undo command in the stack.
	// Parameters:
	//      nTextId:[in] A resource text identifier of the text string
	//                   to be set on the last undo command.
	// See also:
	//      GetUndoTextList, GetRedoTextList
	//-----------------------------------------------------------------------
	void SetLastCommandText(UINT nTextId);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the list of text for undo operations.
	// Returns:
	//      The text list for undo operations.
	// See also:
	//      GetRedoTextList, SetLastCommandText
	//-----------------------------------------------------------------------
	const CStringList& GetUndoTextList();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the list of text for redo operations.
	// Returns:
	//      The texts for redo.
	// See also:
	//      GetUndoTextList, SetLastCommandText
	//-----------------------------------------------------------------------
	const CStringList& GetRedoTextList();

private:
	//-----------------------------------------------------------------------
	// Summary:
	//      Removes command queue tail after the current element.
	//-----------------------------------------------------------------------
	void RemoveTail();

protected:
	CPtrList m_CommandList;   // Commands buffer.
	POSITION m_posFirstUndo;  // Actual command position in the buffer.
	POSITION m_posSavedMark;  // Last saved command position in the buffer.

	BOOL m_bGroupInsertMode;// TRUE if group insert mode is on, FALSE otherwise

private:
	CStringList m_lstUndoText;  // A temporary storage for undo text strings.
	CStringList m_lstRedoText;  // A temporary storage for redo text strings.
};

#endif // !defined(__XTPSYNTAXEDITUNDOMANAGER_H__)
