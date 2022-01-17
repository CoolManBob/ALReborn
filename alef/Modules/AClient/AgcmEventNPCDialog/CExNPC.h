#ifndef __CLASS_INTELIGENSE_NPC_H__
#define __CLASS_INTELIGENSE_NPC_H__



#include "CExNPCDialog.h"
#include "CUiExNPCMsgBoxGrid.h"



class CExNPC : public CExNPCDialog
{
private :
	AcUIMessageDialog								m_MsgBoxOK;
	AcUIOKCancelDialog								m_MsgBoxYesNo;
	AcUIEditOKDialog								m_MsgBoxEditOK;
	CUiExNPCMsgBoxGrid								m_MsgBoxGridItem;
	AcUIWaitingDialog								m_MsgBoxWait;

	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX			m_LastMsgBoxPacket;

public :
	CExNPC( void );
	~CExNPC( void );

public :
	void				OnRegisterCallBack			( void );
	void				OnMakeResource				( void );
	void				OnInitNPCMsgBox				( void* pUIManager );

	int					OnShowNPCMsgBox				( int nMsgBoxType, char* pText, char* pEditString );
	void				OnSendNPCMsgBox				( void* pMsgBoxData, int nResult, char* pEditText = NULL );
	void				OnSendNPCMsgBoxItem			( int nItemID, int nItemTID );

	BOOL				OnHideNPCMsgBox				( void );

private :
	int					_ShowMsgBox					( int nMsgBoxType, char* pText, char* pEditString );
	void				_HideMsgBox					( int nMsgBoxType );

public :
	void				AddMsgBoxImage				( char* pImageName, int nBtnMode = -1 );
	void				SetMsgBoxButtonName			( char* pBtnNameOK, char* pBtnNameNo );
	void				CopyLastReceiveMsgBoxPacket	( void* pPacket );
	BOOL				IsOpenModalWindow			( void );
	BOOL				IsOpenWaitingMsgBox			( void );

public :
	static BOOL			CBShowExNPCMsgBox			( void* pData, void* pClass, void* pCustData );
	static BOOL			CBShowExNPCDialog			( void* pData, void* pClass, void* pCustData );

	static void			CBMsgBoxGridReturnItem		( void* pCallerClass, int nItemID, int nItemTID );
	static void			CBMsgBoxGridCancelItem		( void* pCallerClass );
};



#endif