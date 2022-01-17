#ifndef __CLASS_INTELIGENSE_NPC_DIALOG_H__
#define __CLASS_INTELIGENSE_NPC_DIALOG_H__



#include "ContainerUtil.h"
#include "AgppEventNPCDialog.h"
#include "AgcdUIManager2.h"



struct stExNPCButtonEntry
{
	void*										m_pBtn;
	char										m_strBtnText[ MAX_BUTTON_TEXT_SIZE ];
	int											m_nEventID;
	int											m_nStep;
	int											m_nItemTID;

	stExNPCButtonEntry( void )
	{
		m_pBtn = NULL;
		ReSet();
	}

	void				ReSet					( void )
	{
		memset( m_strBtnText, 0, sizeof( char ) * MAX_BUTTON_TEXT_SIZE );
		m_nEventID = -1;
		m_nStep = -1;
		m_nItemTID = -1;
	}
};


typedef BOOL ( *fnExNPCDialogCallBack )( void* pClass, void* pControl );


class CExNPCDialogButton : public AcUIButton
{
private :
	void*										m_pCallBackClass;
	fnExNPCDialogCallBack						m_fnCallBack_Click;

public :
	CExNPCDialogButton( void );
	virtual ~CExNPCDialogButton( void );

public :
	void			OnRegisterCallBack			( void* pClass, fnExNPCDialogCallBack fnClick ) { m_pCallBackClass = pClass; m_fnCallBack_Click = fnClick; } 

public :
	virtual BOOL	OnLButtonDown				( RsMouseStatus* MouseStatus );
};


class CExNPCDialog
{
public :
	void*										m_pModule;

private :
	void*										m_pUIManager;
	void*										m_pExNPCUI;
	void*										m_pExNPCDialog;

	AcUIEdit									m_EditTitleBar;
	AcUIEdit									m_EditDialogText;
	CExNPCDialogButton							m_BtnClose;

	ContainerMap< int, stExNPCButtonEntry >		m_mapDialogButtons;
	PACKET_EVENTNPCDIALOG_GRANT_MENU			m_LastDialogMenu;

	BOOL										m_bIsOpen;

protected :
	BOOL										m_bIsResourceReady;

public :
	CExNPCDialog( void );
	~CExNPCDialog( void );

public :
	void			OnInitNPCDialog				( void* pUIManager );

	void			OnShowNPCDialog				( void* pPacketData );
	BOOL			OnHideNPCDialog				( void );
	void			OnSencNPCDialog				( void* pBtn );

	void			OnSetDialogTitle			( char* pText );
	void			OnSetDialogMessage			( char* pText );
	void			OnSetDialogButton			( int nIndex, char* pText );

	void			OnUpdateNPCDialog			( void );

private :
	void			_MakeExNPCDialogBase		( void );
	void			_MakeExNPCDialogTitleBar	( void );
	void			_MakeExNPCDIalogTextBox		( void );
	void			_MakeExNPCDIalogBtnClose	( void );
	void			_MakeExNPCDialogBtnMenu		( void );

	void			_ClearExNPCDialogButton		( void );
	void			_DeleteExNPCDialogButton	( void );

	void			_UpdateNPCDialogBtn			( void* pBtn, void* pBtnData );
	void*			_GetNPCDialogBtn			( int nIndex );

	void			_SetButtonEventInfo			( int nIndex, int nEventID, int nStepID, int nItemTID );
	int				_GetEventIDByButton			( CExNPCDialogButton* pBtn );
	int				_GetEventStepByButton		( CExNPCDialogButton* pBtn );

public :
	void			SetModule					( void* pModule ) { m_pModule = pModule; }
	void*			GetModule					( char* pModuleName );
	BOOL			IsExNpcDialog				( char* pUiName );

public :
	static BOOL		CBClickMenuButton			( void* pClass, void* pControl );
	static BOOL		CBClickCloseButton			( void* pClass, void* pControl );
	static BOOL		CBAutoCloseByCharacterMove	( void* pData, void* pClass, void* pCustData );
};



#endif