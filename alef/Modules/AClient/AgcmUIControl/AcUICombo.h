#ifndef			_ACUICOMBO_H_
#define			_ACUICOMBO_H_

#include "AcUIBase.h"
#include "AcUIButton.h"

#define ACUICOMBO_STRING_MAX_LENGTH						64
#define ACUICOMBO_MIN_WIDTH								50
#define	ACUICOMBO_STRING_WRITE_GAP						4

enum 
{
	UICM_COMBO_MESSAGE_SELECT_CHANGED					= UICM_BASE_MAX_MESSAGE,
	UICM_COMBO_MAX_MESSAGE,
};

typedef struct AcUIComboItem
{
	CHAR												szComboString[ ACUICOMBO_STRING_MAX_LENGTH ];	// Combo String
	UINT32												lComboData;	// Combo User Data
	BOOL												bEnable;	// Enable flag
	AcUIComboItem *										pcsNext;	// Next Combo Item
} AcUIComboItem;

// Combo Textures
enum
{
	ACUICOMBO_TEXTURE_NORMAL							= 0,
	ACUICOMBO_TEXTURE_ONMOUSE, 
	ACUICOMBO_TEXTURE_CLICK, 
	ACUICOMBO_TEXTURE_BOTTOM,
	ACUICOMBO_MAX_TEXTURE,
} ;

class AcUICombo : public AcUIBase
{
public:
	AcUICombo( void );
	virtual ~AcUICombo( void );

public:
	AcUIButton *										m_pcsComboButton; 
	AcUIComboItem *										m_listComboItem;	// Combo Item의 List
	AcUIComboItem *										m_pSelectedItem;

	INT32												m_lListItemNum;		// 리스트에 들어있는 AcUIComboItem 총 Num - Index의 갯수 
	INT32												m_lSelectedIndex;	// 선택된 Item의 Index
	BOOL												m_bComboOpenMode;	// Open되면 TRUE, 선택되면 FALSE

	INT32												m_lLineHeight;		// Open되었을때 칸의 간격
	INT32												m_lOriginalHeight;	// Close때의 h

	INT32												m_lMouseOnIndex;	// Open된 상태에서 Mouse가 위에 있는 Index
	INT32												m_lDropTextureID[ACUICOMBO_MAX_TEXTURE];	//Button Mode에 따른 Texture의 ID 

public:
	INT32					AddString					( CHAR* szString, INT32 lComboData = 0, BOOL bFirst = FALSE );
	VOID					ClearAllString				( BOOL bResetSelection = TRUE );
	VOID					OpenComboControl			( void );
	VOID					CloseComboControl			( void );
	BOOL					GetItemData					( CHAR* szString, INT32* lComboData );
	BOOL					GetItemData					( INT32 lIndex, INT32* lComboData );
	BOOL					DeleteString				( CHAR* szString );
	BOOL					DeleteString				( INT32 lIndex );
	BOOL					SetItemData					( CHAR* szString, INT32 lNewComboData );
	BOOL					SetItemData					( INT32 lIndex, INT32 lNewComboData );
	CHAR*					GetSelectedString			( void );
	INT32					GetSelectedIndex			( void );
	BOOL					SelectString				( CHAR* szString );
	BOOL					SelectIndex					( INT32 lIndex );

	BOOL					EnableString				( CHAR* szString, BOOL bEnable = TRUE );
	BOOL					EnableString				( INT32 lIndex, BOOL bEnable = TRUE );

	AcUIComboItem*			GetComboItem				( CHAR *szString, INT32 *plIndex = NULL );
	AcUIComboItem*			GetComboItem				( INT32 lIndex );

	VOID					SetComboButton				( AcUIButton *pcsButton );

	VOID					SetDropDownTexture			( INT32 lMode, INT32 lTextureID );
	INT32					GetDropDownTexture			( INT32 lMode ) { return m_lDropTextureID[ lMode ]; }

	VOID					SetOpenLineHeight			( INT32 lHeight ) { m_lLineHeight = lHeight; }
	INT32					GetOpenLineHeight			( void ) { return m_lLineHeight; }

private :
	RwTexture*				_GetTextureNormal			( void );
	RwTexture*				_GetTextureOn				( void );
	RwTexture*				_GetTextureBottom			( void );

	INT32					_GetTextureWidth			( RwTexture* pTexture );
	INT32					_GetTextureHeight			( RwTexture* pTexture );

	void					_DrawComboTexture			( RwTexture* pTexture, float fPosX, float fPosY, float fWidth, float fHeight, float fUStart, float fVStart, float fUEnd, float fVEnd );
	void					_DrawComboText				( char* pString, float fPosX, float fPosY, UINT32 nFontType, UINT8 nAlpha, DWORD dwColor );

	void					_DrawSelectedItemText		( AcUIComboItem* pItem, INT32 nAbsX, INT32 nAbsY, INT32 nFontHeight );
	void					_DrawComboItemList			( AcUIComboItem* pItem, INT32 nAbsX, INT32 nAbsY, INT32 nFontHeight );

// Virtual Functions
public :
	virtual	VOID 			WindowRender				( void );
	virtual	BOOL 			OnMouseMove					( RsMouseStatus *ms	);
	virtual	VOID 			OnMoveWindow				( void );
	virtual	BOOL 			OnInit						( void );
	virtual	VOID 			OnClose						( void );
	virtual	VOID 			OnWindowRender				( void );
	virtual	BOOL 			OnCommand					( INT32	nID, PVOID pParam );
	virtual	BOOL 			OnLButtonDown				( RsMouseStatus* ms	);
};

#endif			// _ACUICOMBO_H_
