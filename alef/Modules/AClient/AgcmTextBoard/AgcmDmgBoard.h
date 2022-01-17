#pragma once

#include "AgcmBaseBoard.h"
//#include "StringConverter.h"

class AgcmDmgBoard
	:	public AgcmBaseBoard	,
		public ApMemory< AgcmDmgBoard , AGCMTEXTBOARD_MAX_BOARD >
{
public:
	AgcmDmgBoard( eBoardType eType = AGCM_BOARD_COUNT );
	virtual ~AgcmDmgBoard( VOID );

	virtual VOID			Render				( VOID );
	virtual BOOL			Update				( IN DWORD tickDiff , IN RwMatrix* CamMat );

	//set----------------------------------------------------------------------------------------------
	inline VOID				SetCharacter		( IN AgpdCharacter*	pKeyObj		)	{ m_KeyObj      = pKeyObj;   }
	inline VOID				SetColor			( IN RwRGBA&		Color		)	{ m_Color       = Color;     }
	inline VOID				SetAniCount			( IN INT32			AniCnt		)	{ m_AniCount    = AniCnt;    }
	inline VOID				SetAniEndCount		( IN INT32			AniEndCnt	)	{ m_AniEndCount = AniEndCnt; }
	inline VOID				SetNumCount			( IN INT32			NumCount	)	{	m_NumCount	=	NumCount; }
	inline VOID				SetDamage			( IN const char*	Damage		)	{ memcpy_s( m_Text , TB_DAMAGE_MAX_NUM , Damage , TB_DAMAGE_MAX_NUM ); }
	//-------------------------------------------------------------------------------------------------

	//get--------------------------------------------------------------------
	inline AgpdCharacter*	GetCharacter		( VOID )	{ return m_KeyObj;			}
	inline RwRGBA&			GetColor			( VOID )	{ return m_Color;		    }
	inline INT				GetNumCount			( VOID )	{ return m_NumCount;		}
	inline INT32			GetAniCount			( VOID )	{ return m_AniCount;		}
	inline INT32			GetAniEndCount		( VOID )	{ return m_AniEndCount;		}
	//inline INT32			GetDamage			( VOID )	{ return StringConverter::parseInt( m_Text ); }
	//-----------------------------------------------------------------------

protected:
	VOID					_DrawDamage			( VOID );
	VOID					_DrawCriticalDamage	( VOID );

	AgpdCharacter*			m_KeyObj;
	RwRGBA					m_Color;

	INT32					m_AniCount;
	INT32					m_AniEndCount;

	char					m_Text[ TB_DAMAGE_MAX_NUM ];
	INT						m_NumCount;
};
