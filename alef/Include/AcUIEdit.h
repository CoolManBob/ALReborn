#ifndef			_ACUIEDIT_H_
#define			_ACUIEDIT_H_

#include "AcUIBase.h"
#include "AcUIScroll.h"

#define	ACUI_EDIT_DEFAULT_BUFFER_SIZE					2048
#define ACUI_EDIT_LF_SIZE								8
#define ACUI_EDIT_MAX_LINE								300
#define ACUI_EDIT_LEFT_MARGIN							10
#define ACUI_EDIT_DEFAILT_LF							"\n"

// Command Message 
enum
{
	UICM_EDIT_INPUT_END									= UICM_BASE_MAX_MESSAGE,	// Input이 끝났다 처리해 주라	
	UICM_EDIT_INPUT_TAB,															// Edit창에 Tab을 쳤다
	UICM_EDIT_INPUT_SHIFT_TAB,														// Edit창에 Shift + Tab를 쳤다
	UICM_EDIT_ACTIVE,																// Edit창이 Active된다.
	UICM_EDIT_DEACTIVE,																// Edit창이 Deactive된다.
	UICM_EDIT_MAX_MESSAGE
};

// Tool Tip에 쓰일 String Struct - List로 관리된다 -
typedef struct AgcdUIEditStringInfo
{
	BOOL												bNewLine;		// 이녀석이 TRUE이면 아래 값들은 의미가 없다 
	CHAR*												szString;

	INT32												lOffsetX;
	INT32												lOffsetY;
	INT32												lWidth;
	INT32												lHeight;
	INT32												lFontType;	
	INT32												lLineIndex;
	INT32												lStringLength;

	BOOL												bBold;
	DWORD												dwColor;
	DWORD												dwBGColor;

	AgcdUIEditStringInfo *								pstNext;

	AgcdUIEditStringInfo( void )
	{
		bNewLine		= FALSE;
		szString		= NULL;
		lOffsetX		= 0;
		lOffsetY		= 0;
		lWidth			= 0;
		lFontType		= 0;
		dwColor			= 0;
		dwBGColor		= 0;
		bBold			= FALSE;
		pstNext			= NULL;
		lLineIndex		= 0;
		lStringLength	= -1;
	}
} AgcdUIEditStringInfo;

class AcUIEdit : public AcUIBase 
{
public:
	AcUIEdit( BOOL bPassword = FALSE, BOOL bNumber = FALSE );
	virtual ~AcUIEdit( void );

public:
	enum LineControl
	{
		CURRENTLINE	= 0,														// 현재줄
		NEWLINE	= 1,															// 다음줄
		CONTINOUSLINE = 2														// 다음줄인데 이어지는 줄..
	};

	CHAR*												m_szText;
	CHAR*												m_szTextDisplay;

	INT32												m_lHotkey;
	AgcdUIHotkeyType									m_eHotkeyType;

	ApSafeArray< CHAR, ACUI_EDIT_LF_SIZE + 1 >			m_szLineDelimiter;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE + 1 >		m_alStringStartY;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringHeight;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringWidth;
	ApSafeArray< INT32, ACUI_EDIT_MAX_LINE >			m_alStringLength;

	CHAR												m_szComposing[ 256 ];

	AgcdUIEditStringInfo*								m_pstStringInfoHead;
	AgcdUIEditStringInfo*								m_pstStringInfoTail;
	AgcdUIEditStringInfo*								m_pstStringInfoCursor;

	INT32												m_lTextLength;			// Text 길이
	INT32												m_lTextDisplayLength;	// Display되는 Text 길이
	INT32												m_lDelimiterLength;		// Delimiter 길이

	INT32												m_lCursorPosX;			// Cursor X 좌표
	INT32												m_lCursorPosY;			// Cursor Y 좌표
	INT32												m_lCursorPoint;			// 현재 Cursor Point (m_szText내에서의 Index)
	INT32												m_lCursorPointDisplay;	// Display되는 Cursor Point

	UINT32												m_lCursorCount;			// 0-4 none 5-9 draw cursor
	UINT32												m_lLastTick;

	INT32												m_lCursorRow;			// 현재 Cursor의 Row
	INT32												m_lCursorColumn;		// 현재 Cursor가 StringItem에서 가지는 Column

	AcUIScroll*											m_pcsVScroll;
	BOOL												m_bVScrollLeft;

	INT32												m_lBoxWidth;
	INT32												m_lOriginalBoxWidth;

	BOOL												m_bPasswordEdit;		// 패스워드 사용 Edit로 '*'출력
	BOOL												m_bForHotkey;			// Hotkey용
	BOOL												m_bNumberOnly;			// Number Only Edit?
	BOOL												m_bReadOnly;			// Read Only? (No Edit)
	BOOL												m_bEnableTag;			// Is Tag enabled? (<>)
	BOOL												m_bMultiLine;
	BOOL												m_bAutoLF;				// 자동 줄 바꿈
	BOOL												m_bFilterCase;			// 대소문자 자동 세팅
	BOOL												m_bReleaseEditInputEnd;	// 엔터를 누르면 Edit가 Release될것인가?
	BOOL												m_bEnableTextLapping;	// 단어 붙어있는거 다음라인으로 넘기기 ( 유럽용 )
	BOOL												m_bChangeText;			// 키보드가 눌렸음.

	UINT32												m_ulTextMaxLength;		// 최대 Text 길이

	UINT32												m_ulLine;				// 현재 Line
	INT32												m_lStartLine;			// Display되는 첫Line

	INT32												m_lStartX;				// Clipping용 Text X 좌표
	INT32												m_lStartY;				// Clipping용 Text X 좌표

	INT32												m_nOffSetFromLeftEdge;	// 왼쪽에서부터 여백
	INT32												m_nOffSetFromTopEdge;	// 위쪽에서부터 여백

	BOOL												m_bIsShowReadingWin;	// reading window show/hide flag
	BOOL												m_bIsShowCandidateWin;	// candidadte window show/hide flag

	//@{ 2006/07/02 burumal
	static AcUIDefalutCallback							m_pCBHotkeyOverwriteCheck;
	static PVOID										m_pHotkeyCheckTargetInstacne;
	//@}

	// Virtual Functions
public:
	virtual	VOID				OnClose					( void );				// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close되는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
	virtual VOID				OnAddImage				( RwRaster*	pRaster );
	virtual VOID				OnPostRender			( RwRaster* pRaster	);	// 월드 렌더한 후.
	virtual VOID				OnWindowRender			( void );				// 실제 Render하는 함수 
	virtual BOOL				OnLButtonDown			( RsMouseStatus *ms	);	
	virtual BOOL				OnChar					( CHAR* pChar, UINT lParam );
	virtual	BOOL				OnKeyDown				( RsKeyStatus* ks );
	virtual	VOID				OnMoveWindow			( void );
	virtual	BOOL				OnCommand				( INT32	nID, PVOID pParam );
	virtual	BOOL				OnMouseWheel			( INT32	lDelta );

	virtual BOOL				OnIdle					( UINT32 ulClockCount );
	virtual BOOL				OnIMEComposing			( CHAR* composing, UINT lParam );

	virtual BOOL				OnIMEReading			( void );				// reading window 그리기
	virtual BOOL				OnIMEHideReading		( void );				// reading window 감추기
	virtual BOOL				OnIMECandidate			( void );				// candidate window 그리기
	virtual BOOL				OnIMEHideCandidate		( void );				// candidate window 감추기

	virtual VOID				OnEditActive			( void );
	virtual VOID				OnEditDeactive			( void );

	//VOID	PasswordEditOnPostRender( RwRaster *raster );	// Password Edit이면 이 함수에서 OnPostRender	

private :
	INT32						_GetStringWidth			( INT32 nLineIndex );
	INT32						_GetStringHeight		( INT32 nLineIndex );
	INT32						_GetStringStartPosY		( INT32 nLineIndex );

	void						_DrawStringAlignCenter	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawStringAlignRight	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawStringAlignLeft	( AgcdUIEditStringInfo* pStringList,
		AgcdFontClippingBox* pClippingBox, INT32 nFontType,
		INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin );
	void						_DrawCaretComposer		( INT32 nAbsX, INT32 nAbsY, INT32 nOffSetX, INT32 nScrollMargin );

	BOOL						_ProcessKeyEnter		( char* pInputChar );
	BOOL						_ProcessKeyESC			( char* pInputChar );
	BOOL						_ProcessKeyBackSpace	( char* pInputChar );
	BOOL						_ProcessKeyTab			( char* pInputChar );
	BOOL						_ProcessKeyText			( char* pInputChar );

	BOOL						_ProcessKeyLeft			( void );
	BOOL						_ProcessKeyRight		( void );
	BOOL						_ProcessKeyUp			( void );
	BOOL						_ProcessKeyDown			( void );
	BOOL						_ProcessKeyDelete		( void );

	BOOL						_UpdateCursorPosition	( AgcdUIEditStringInfo* pStringList );
	BOOL						_UpdateScrollBarVert	( void );

	VOID						_ParseLine				( CHAR *szLine, INT32 lStringLength = -1, BOOL bChatting = FALSE );
	VOID						_ParseTag				( CHAR *szTag, INT32 *plFontType, DWORD *pdwColor, DWORD *pdwBGColor, BOOL *pbBold );


	// 함수들..
public :
	inline	VOID				SetBoxWidth				( INT32 w )
	{
		m_lOriginalBoxWidth = w;
		if( m_pcsVScroll )
		{
			m_lBoxWidth = m_lOriginalBoxWidth - m_pcsVScroll->w;
		}
	}

	inline INT32				GetBoxWidth				( void ) { return m_lOriginalBoxWidth; }
	inline const CHAR*			GetText					( void ) { return m_szText;	}

	AgcdUIEditStringInfo* 		GetParseText			( void ) { return m_pstStringInfoHead; }

	BOOL						SetText					( CHAR *szText = NULL, BOOL bChatting = FALSE );
	VOID						ClearText				( void );
	VOID						SetTextStartLine		( INT32 nLine = 0 );

	AgcdUIEditStringInfo*		AddStringItem			( CHAR *szString, DWORD dwColor, INT32 lFontType = -1, LineControl eNewLine = CURRENTLINE, INT32 lStringLength = -1, DWORD dwBGColor = 0x00000000, BOOL bBold = FALSE , BOOL bChatting = FALSE );
	VOID						ClearStringInfo			( void );

	BOOL						SetMeActiveEdit			( void );
	BOOL						ReleaseMeActiveEdit		( void );

	BOOL						CheckSOL				( BOOL bFirstLine = FALSE );	// 만약 현재 Cursor 위치가 Start Of Line이면 TRUE (bFirstLine : 제일 처음 라인의 SOL도 OK?)
	BOOL						CheckEOL				( void );						// 만약 현재 Cursor 위치가 End Of Line이면 TRUE

	BOOL						SetLineDelimiter		( CHAR *szDelimiter );

	VOID						SetTextMaxLength		( UINT32 ulLength );
	inline UINT32				GetTextMaxLength		( void ) { return m_ulTextMaxLength; }

	VOID						SetVScroll				( AcUIScroll* pcsScroll );
	inline AcUIScroll*			GetVScroll				( void ) { return m_pcsVScroll;	}

	VOID 						DrawReadingWindow		( const float fPosX, const float fPosY );	//reading window 그리기
	VOID 						DrawCandidateWindow		( const float fPosX, const float fPosY );	//candidate window 그리기

	// Hotkey 관련
	BOOL						SetHotkey				( INT32 lScanCode );	
	BOOL						SetHotkey				( AgcdUIHotkeyType eType, INT32 lScanCode );
	BOOL						GetHotkey				( INT32 *plScanCode, AgcdUIHotkeyType *peType );

	//@{ 2006/07/02 burumal
	static VOID	SetCallbackHotkeyOverwriteCheck( AcUIDefalutCallback pCB, PVOID pTargetInstance )
	{ 
		m_pCBHotkeyOverwriteCheck = pCB;
		m_pHotkeyCheckTargetInstacne = pTargetInstance;
	}	
	//@}

	// IsDBC는 (str-1)을 사용하기 때문에 주의가 필요하다.
	bool						IsDBC					( unsigned char* str ) { return ( this->*IsDBCPtr )( str ); }

private:
	void						InitFuncPtr				( void );

	// 다국어 버전 지원
	bool 						(AcUIEdit::*IsDBCPtr)	( unsigned char* str );
	bool 						IsDBC_kr				( unsigned char* str );
	bool 						IsDBC_cn				( unsigned char* str );
	bool 						IsDBC_jp				( unsigned char* str );
	bool 						IsDBC_en				( unsigned char* str );
};

#endif			//_ACUIEDIT_H_

