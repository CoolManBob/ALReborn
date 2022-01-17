#pragma once

#include "AgcmBaseBoard.h"

class AgcmIDBoard;

class AgcmTextBoard
	:	public AgcmBaseBoard	,
		public ApMemory< AgcmTextBoard , AGCMTEXTBOARD_MAX_BOARD >
{
public:
	AgcmTextBoard( eBoardType eType = AGCM_BOARD_TEXT );
	virtual ~AgcmTextBoard( VOID );
	
	virtual VOID			Render	( VOID );
	virtual BOOL			Update	( IN DWORD tickDiff , IN RwMatrix* CamMat );

	VOID					InitText( VOID );

	//set---------------------------------------------------------------------------------
	inline VOID				SetBoardW   ( INT32 BoardW    ) { m_BoardW     = BoardW;      }
	inline VOID				SetBoardH   ( INT32 BoardH    ) { m_BoardH     = BoardH;      }
	inline VOID				SetLifeCount( INT32 LifeCount ) { m_LifeCount  = LifeCount;   }
	inline VOID				SetTextColor( DWORD TextColor ) { m_TextColor  = TextColor;   }
	inline VOID				SetTailType ( INT32 TailType  ) { m_TailType   = TailType;    }
	inline VOID				SetLineCount( INT32 LineCount ) { m_LineCount   = LineCount;  }
	inline VOID				SetFontType ( INT32 FontType  ) { m_FontType   = FontType;    }

	inline VOID				SetCharacter( AgpdCharacter* pCharacter ) { m_pCharacter = pCharacter; }
	inline VOID				SetIDBoard	( AgcmIDBoard* pIDBoard		) { m_pIDBoard  = pIDBoard;     }
	inline VOID				SetText		( INT Line , const char* str , INT Length ) { sprintf_s( m_Text[ Line ] , TB_TEXT_MAX_CHAR + 2 , "%s" , str ); m_Text[Line][Length] = 0;  }

	//get----------------------------------------------------------
	inline INT32			GetBoardW		( VOID ) { return m_BoardW;     }
	inline INT32			GetBoardH		( VOID ) { return m_BoardH;     }
	inline INT32			GetLifeCount	( VOID ) { return m_LifeCount;  }
	inline INT32			GetTailType		( VOID ) { return m_TailType;   }
	inline INT32			GetLineCount	( VOID ) { return m_LineCount;  }
	inline INT32			GetFontType		( VOID ) { return m_FontType;   }
	inline DWORD			GetTextColor	( VOID ) { return m_TextColor;  }
	inline AgpdCharacter*	GetCharacter	( VOID ) { return m_pCharacter; }

	inline char*			GetText		( INT Line ) { return m_Text[ Line ]; }

protected:
	INT32					m_BoardW;
	INT32					m_BoardH;
	INT32					m_LifeCount;
	INT32					m_FontType;

	char					m_Text[ TB_TEXT_MAX_CHARLINE ][ TB_TEXT_MAX_CHAR + 2 ];
	DWORD					m_TextColor;

	INT32					m_TailType;
	INT32					m_LineCount;

	AgpdCharacter*			m_pCharacter;
	AgcmIDBoard*			m_pIDBoard;
};
