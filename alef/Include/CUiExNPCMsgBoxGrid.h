#ifndef __CLASS_UI_EX_NPC_DIALOG_GRID_H__
#define __CLASS_UI_EX_NPC_DIALOG_GRID_H__



#include "CUiGridItem.h"



typedef void ( *fnCallBack_MsgBoxBtnClickOK )( void* pCallerClass, int nItemID, int nItemTID );
typedef void ( *fnCallBack_MsgBoxBtnClickNo )( void* pCallerClass );



class CUiExNPCMsgBoxGrid : public AcUIOKCancelDialog
{
private :
	CUiGridSlot								m_ItemGrid;

	fnCallBack_MsgBoxBtnClickOK				m_fnCallBackOK;
	void*									m_pCallerClassOK;

	fnCallBack_MsgBoxBtnClickNo				m_fnCallBackNo;
	void*									m_pCallerClassNo;

public :
	CUiExNPCMsgBoxGrid( void );
	virtual ~CUiExNPCMsgBoxGrid( void );

public :
	virtual BOOL		OnInit				( void );
	virtual void		OnWindowRender		( void );
	virtual BOOL		OnCommand			( INT32 nID, void* pParam );
	virtual BOOL		OnKeyDown			( RsKeyStatus* pKeyState );

public :
	void				OnClearGridItem		( void );

public :
	void				SetButtonName		( char* pBtnText, char* pBtnTextNo );
	void				SetReturnCallBack	( void* pCallerClass, fnCallBack_MsgBoxBtnClickOK fnFunction ) { m_pCallerClassOK = pCallerClass; m_fnCallBackOK = fnFunction; }
	void				SetCancelCallBack	( void* pCallerClass, fnCallBack_MsgBoxBtnClickNo fnFunction ) { m_pCallerClassNo = pCallerClass; m_fnCallBackNo = fnFunction; }

private :
	void				_LoadResource		( void );
};



#endif