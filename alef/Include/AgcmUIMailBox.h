/*====================================================================

	AgcmUIMailBox.h
	
====================================================================*/


#ifndef _AGCMUI_MAILBOX_H_
	#define _AGCMUI_MAILBOX_H_


#include "ApBase.h"
#include "ApModule.h"
#include "AgpmMailBox.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"
#include "AgcmUIConsole.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGCMUIMAILBOX_EVENT
	{
	AGCMUIMAILBOX_EVENT_OPEN_LIST = 0,
	AGCMUIMAILBOX_EVENT_OPEN_WRITE,
	AGCMUIMAILBOX_EVENT_CLOSE_WRITE,
	AGCMUIMAILBOX_EVENT_OPEN_READ,
	AGCMUIMAILBOX_EVENT_CLOSE_READ,
	AGCMUIMAILBOX_EVENT_ICON_READ,
	AGCMUIMAILBOX_EVENT_ICON_UNREAD,
	AGCMUIMAILBOX_EVENT_ICON_ATTACH,
	AGCMUIMAILBOX_EVENT_ICON_UNATTACH,
	AGCMUIMAILBOX_EVENT_NOTIFY_UNREAD,
	AGCMUIMAILBOX_EVENT_MAX
	};


enum eAGCMUIMAILBOX_MESSAGE
	{
	AGCMUIMAILBOX_MESSAGE_LETTER_COUNT = 0,
	AGCMUIMAILBOX_MESSAGE_CLOSE_CONFIRM,
	AGCMUIMAILBOX_MESSAGE_WRITE_SUCCESS,
	AGCMUIMAILBOX_MESSAGE_WRITE_FAIL,
	AGCMUIMAILBOX_MESSAGE_ITEM_SAVED,
	AGCMUIMAILBOX_MESSAGE_WRITE_DESC,
	AGCMUIMAILBOX_MESSAGE_MAX,
	};




/****************************************************/
/*		The Definition of AgcmUIMailBox class		*/
/****************************************************/
//
class AgcmUIMailBox : public ApModule
	{
	private:
		//	Related modules
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmMailBox		*m_pAgpmMailBox;
		AgpmGrid		*m_pAgpmGrid;
		AgpmItem		*m_pAgpmItem;
		
		AgcmCharacter	*m_pAgcmCharacter;
		AgcmItem		*m_pAgcmItem;
		AgcmUIManager2	*m_pAgcmUIManager2;
		AgcmUIControl	*m_pAgcmUIControl;
		AgcmUIMain		*m_pAgcmUIMain;

		// Event
		INT32			m_lEvent[AGCMUIMAILBOX_EVENT_MAX];
		static TCHAR	s_szEvent[AGCMUIMAILBOX_EVENT_MAX][30];

		// Message Key
		static TCHAR	s_szMessage[AGCMUIMAILBOX_MESSAGE_MAX][30];

		// Grid
		AgpdGrid		m_AgpdGridRead;
		AgpdGrid		m_AgpdGridWrite;
		AgpdGridItem	*m_pAgpdGridItemRead;

		// User Data
		AgcdUIUserData	*m_pstListDummy;
		AgcdUIUserData	*m_pstListMail;
		INT32			m_lListMailIndex[AGPMMAILBOX_MAX_MAILS_IN_EFFECT];		// dummy?
		AgcdUIUserData	*m_pstReadItem;
		AgcdUIUserData	*m_pstReadDummy;
		AgcdUIUserData	*m_pstReadItemSave;				// item save button
		AgcdUIUserData	*m_pstWriteItem;
		AgcdUIUserData	*m_pstWriteDummy;
		AgcdUIUserData	*m_pstCommonWriteMail;			// write button
		
		// My
		AgpdCharacter	*m_pAgpdCharacterSelf;
		AgpdMailCAD		*m_pAgpdMailCADSelf;
		INT32			m_lSelectedIndex;
		
		// Reply
		BOOL			m_bReply;

	public:
		AgcmUIMailBox();
		virtual ~AgcmUIMailBox();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		BOOL	OnIdle(UINT32 ulClockCount);

		//	
		BOOL	OpenWriteWindow();

		//	Module callback
		static BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBAddMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWriteMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRemoveMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBItemSave(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	UI Function callback
		//	List
		static BOOL	CBOpenWindowList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickButtonListRead(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickButtonListDelete(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickButtonListWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		//	Write
		static BOOL CBClickButtonWriteSend(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBCloseWindowWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBMoveItemWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBMoveEndItemWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBInputTabWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		//	Read
		static BOOL	CBCloseWindowRead(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickButtonReadItemSave(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL	CBClickButtonReadReply(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Display callback
		//	List
		static BOOL	CBDisplayListSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayListDate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayAvailableLetter(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	Read
		static BOOL	CBDisplayReadToCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayReadSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayReadContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayReadFromCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayReadDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		//	Write
		static BOOL	CBDisplayWriteFromCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayWriteDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayWriteToCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayWriteSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL	CBDisplayWriteContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//	Boolean callback
		static BOOL CBIsActiveDeleteButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveItemSaveButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL	CBIsActiveWriteButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

		//	Packet
		BOOL	SendWriteMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, INT32 lItemID);
		BOOL	SendReadMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail);
		BOOL	SendRemoveMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail);
		BOOL	SendItemSave(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail);

	  #ifdef _DEBUG
		void	TestOpenWriteWindow();
		void	TestOpenList();
	  #endif

	protected :
		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		BOOL	AddBoolean();
		
		//	helper
		TCHAR*		GetMessageTxt(eAGCMUIMAILBOX_MESSAGE eMessage);
		AgpdMail*	GetMailOfIndex(INT32 lIndex);
		void		RefreshList();
		BOOL		AddItemToWriteGrid(AgpdGridItem *pAgpdGridItem, INT16 nLayer = 0, INT16 lRow = 0, INT16 nColumn = 0);
		BOOL		AddItemToReadGrid(INT32 lItemTID, INT32 lCount);
		void		RemoveItemFromWriteGrid(AgpdGridItem *pAgpdGridItem);
		void		RemoveItemFromReadGrid();
		BOOL		SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte);

	public :
		BOOL		IsOpenMailRead( void );
		BOOL		IsHaveNotReadMail( void );
		void		CheckHaveNotReadMail( void );
	};


#endif