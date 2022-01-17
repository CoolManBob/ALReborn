// AcUIButton.h: interface for the AcUIButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_)
#define AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcUIBase.h"
#include "AcUIGrid.h"

#define	 ACUIBUTTON_H_MAX_STRING				256
#define	 ACUIBUTTON_INIT_INDEX					-1

// Command Message
enum 
{
	UICM_BUTTON_MESSAGE_CLICK					= UICM_BASE_MAX_MESSAGE,
	UICM_BUTTON_MESSAGE_PUSHDOWN,
	UICM_BUTTON_MESSAGE_PUSHUP,
	UICM_BUTTON_MESSAGE_DRAG_DROP,
	UICM_BUTTON_MAX_MESSAGE
};

// Button Mode 
enum
{
	ACUIBUTTON_MODE_NORMAL						= 0,
	ACUIBUTTON_MODE_ONMOUSE, 
	ACUIBUTTON_MODE_CLICK, 
	ACUIBUTTON_MODE_DISABLE,
	ACUIBUTTON_MAX_MODE,
};

typedef struct
{
	BOOL										m_bPushButton;
	BOOL										m_bStartOnClickStatus;
	BOOL										m_bClickOnButtonDown;
} AcUIButtonProperty;

class AcUIButton : public AcUIBase
{
public:
	AcUIButton( void );
	virtual ~AcUIButton( void );

protected:
	INT8										m_cButtonMode;				//Button Mode : 0-일반, 1-포커스(마우스가올라옴), 2-클릭(왼쪽버튼클릭), 3-비활성 
	INT8										m_cPrevButtonMode;			// Disable되기 전의 Mode

public:	
	INT32										m_lButtonTextureID[ ACUIBUTTON_MAX_MODE ];	//Button Mode에 따른 Texture의 ID 
	char										m_szButtonText[ ACUIBUTTON_H_MAX_STRING ];	//Button위에 Text쓰기 

	float										m_fTextureWidth;			// Button Texture 의 Width
	float										m_fTextureHeight;			// Button Texture 의 Height 
	
	static char*								m_pszButtonClickSound;		// 버튼 클릭시 발생할 사운드

	AcUIButtonProperty							m_stProperty;				// 버튼 속성
	AcUIGridItem*								m_pcsDragDropGridItem;		// 이 버튼에 떨궈진 Grid Item 포인터

	INT32										m_lButtonDownStringOffsetX;	// 버튼이 눌러졌을때 String의 X축 위치 변화
	INT32										m_lButtonDownStringOffsetY;	// 버튼이 눌러졌을때 String의 X축 위치 변화

	DWORD										m_lDisableTextColor;		// Disable Text Color

// 버튼 조작관련
public:
	void			AddButtonImage				( char * filename, UINT8 cButtonMode );
	void			SetButtonImage				( INT32 lImageID, INT8 cButtonMode );
	INT32			GetButtonImage				( INT8 cButtonMode );

	void			SetButtonEnable				( BOOL bEnable );	
	void			SetButtonMode				( UINT8	cMode, BOOL bSendMessage = FALSE );
	UINT8			GetButtonMode				( void );

// 특수기능
public :
	AcUIGridItem*	GetDragDropGridItem();

// Virtual Functions
public :
	virtual void	OnAddImage					( RwTexture* pTexture );
	virtual void	OnPostRender				( RwRaster *raster );		// 월드 렌더한 후.
	virtual	void	OnWindowRender				( void );
	virtual BOOL	OnLButtonDown				( RsMouseStatus *ms	);	
	virtual BOOL	OnLButtonDblClk				( RsMouseStatus *ms	);
	virtual BOOL	OnLButtonUp					( RsMouseStatus *ms );
	virtual BOOL	OnMouseMove					( RsMouseStatus *ms );
	virtual BOOL	OnDragDrop					( PVOID pParam1, PVOID pParam2 );
	virtual void	OnSetFocus					( void );
	virtual void	OnKillFocus					( void );

/**

	Not Use.. Commented in 2008. 06. 12. by 성일
public :
	void			SetDisableTextColor			( DWORD lColor ) { m_lDisableTextColor = lColor; }

*/


};

class AcUIPushButton : public AcUIButton
{
public:
	AcUIPushButton( void )						{ m_stProperty.m_bPushButton = TRUE; }
	virtual ~AcUIPushButton( void )				{	}
};

#endif // !defined(AFX_ACUIBUTTON_H__9862A601_119C_4D25_AEC1_98DE27B959CC__INCLUDED_)
