#pragma once

#include "AgcmBaseBoard.h"

class AgcmImoticonBoard
	:	public AgcmBaseBoard	,
		public ApMemory< AgcmImoticonBoard , AGCMTEXTBOARD_MAX_BOARD >
{
public:
	AgcmImoticonBoard( eBoardType     eType = AGCM_BOARD_IMOTICON );
	virtual ~AgcmImoticonBoard( VOID );

	//Render,Update----------------------------------------------------
	virtual VOID				Render( VOID );
	virtual BOOL				Update( IN DWORD tickDiff , IN RwMatrix* CamMat );
	//-----------------------------------------------------------------

	//set------------------------------------------------------------------------
	inline VOID					SetCharacter	( IN AgpdCharacter* pKeyObj		)	{	m_KeyObj   = pKeyObj;			}
	inline VOID					SetImageID		( IN INT32          ImgID		)	{	m_ImgID    = ImgID;				}
	inline VOID					SetAniType		( IN INT32          AniType		)	{	m_AniType  = AniType;			}
	inline VOID					SetAniCount		( IN INT32          AniCnt		)	{	m_AniCount = AniCnt;			}
	inline VOID					SetAniEndCount	( IN INT32          AniEndCnt	)	{	m_AniEndCount = AniEndCnt;		}
	inline VOID					SetAniParam		( INT Index , INT Data			)	{	m_AniParam[Index]	=	Data; 	}
	inline VOID					SetAniVal		( INT Index	, FLOAT Data		)	{	m_AniVal[Index]		=	Data;	}
	//---------------------------------------------------------------------------

	//get-------------------------------------------------------------------------
	inline const AgpdCharacter* GetCharacter	( VOID )		const	{	return m_KeyObj;			}
	inline INT32                GetImageID		( VOID )		const	{	return m_ImgID;				}
	inline INT32                GetAniType		( VOID )		const	{	return m_AniType;			}
	inline INT32                GetAniCount		( VOID )		const	{	return m_AniCount;			}
	inline INT32                GetAniEndCount	( VOID )		const	{	return m_AniEndCount;		}
	inline INT					GetAniParam		( INT Index )	const	{	return m_AniParam[Index];	}
	inline FLOAT				GetAniVal		( INT Index )	const	{	return m_AniVal[Index];		}
	//-----------------------------------------------------------------------------

	inline FLOAT				GetPer			( VOID )	
	{	
		return (FLOAT)( m_AniCount - m_AniParam[0] ) / (FLOAT)( m_AniEndCount - m_AniParam[0] );	
	}

protected:
	AgpdCharacter*				m_KeyObj;
	INT32						m_ImgID;
	INT32						m_AniType;
	INT32						m_AniCount;
	INT32						m_AniEndCount;

	INT							m_AniParam[ 2 ];
	FLOAT						m_AniVal  [ 4 ];
};
