#pragma once

#include "AgcmIDBoard.h"

class AgcmHPBar
	:	public AgcmIDBoard	,
		public ApMemory< AgcmHPBar , AGCMTEXTBOARD_MAX_BOARD >
{
public:
	AgcmHPBar( eBoardType  eType = AGCM_BOARD_COUNT );
	virtual ~AgcmHPBar(void);

	// Overriding
	virtual VOID			Render( VOID );
	virtual BOOL			Update( IN DWORD tickDiff , IN RwMatrix* CamMat );

	//set------------------------------------------------------------------------------------------------------------
	inline VOID             SetCamPosHPMP		( IN RwV3d& Pos         )		{ m_CamPosHPMP     = Pos;					} 
	inline VOID             SetScrPosHPMP		( IN RwV3d& Pos         )		{ m_ScrPosHPMP     = Pos;					}
	inline VOID             SetOffsetHPMP		( IN RwV2d& Offset      )		{ m_OffsetHPMP     = Offset;				}
	inline VOID             SetRecipzHPMP		( IN FLOAT  Recipz      )		{ m_RecipzHPMP     = Recipz;				}
	inline VOID             SetDrawHPMP			( IN BOOL   bDraw       )		{ m_bDrawHPMP      = bDraw;					}

	inline VOID             SetDecreaseHP		( IN BOOL   bDecrease   )		{ m_bDecreaseHP    = bDecrease;				}
	inline VOID             SetPerResultHP		( IN FLOAT  PerResultHP )		{ m_PerResultHP    = PerResultHP;			}
	inline VOID             SetDecreaseMP		( IN BOOL   bDecrease   )		{ m_bDecreaseMP    = bDecrease;				}
	inline VOID             SetPerResultMP		( IN FLOAT  PerResultMP )		{ m_PerResultMP    = PerResultMP;			}
	inline VOID             SetRemove			( IN BOOL   bRemove     )		{ m_bRemove        = bRemove;				}
	inline VOID             SetRemoveCount		( IN INT32  nRemoveCnt  )		{ m_nRemoveCount   = nRemoveCnt;			}

	inline VOID             SetPerAfterImageHP  ( IN FLOAT PerAfterImageHP )	{ m_PerAfterImageHP   = PerAfterImageHP;	}
	inline VOID             SetSpeedAfterImageHP( IN FLOAT SpdAfterImageHP )	{ m_SpeedAfterImageHP = SpdAfterImageHP;	}

	inline VOID             SetPerAfterImageMP  ( IN FLOAT PerAfterImageMP )	{ m_SpeedAfterImageMP = PerAfterImageMP;	}
	inline VOID             SetSpeedAfterImageMP( IN FLOAT SpdAfterImageMP )	{ m_SpeedAfterImageMP = SpdAfterImageMP;	}

	VOID					SetData				( IN AgcmIDBoard* pIDBoard );
	//--------------------------------------------------------------------------------------------------------------

	//get-----------------------------------------------------------------------------------
	inline RwV3d&			GetCamPosHPMP		( VOID )  {  return m_CamPosHPMP;			}
	inline RwV3d&			GetScrPosHPMP		( VOID )  {  return m_ScrPosHPMP;			}
	inline RwV2d&			GetOffsetHPMP		( VOID )  {  return m_OffsetHPMP;			}
	inline FLOAT            GetRecipzHPMP		( VOID )  {  return m_RecipzHPMP;			}
	inline FLOAT*           GetRecipzHPMPPtr    ( VOID )  {  return &m_RecipzHPMP;			}
	inline BOOL             GetDrawHPMP		    ( VOID )  {  return m_bDrawHPMP;			}
	inline BOOL             GetDecreaseHP		( VOID )  {  return m_bDecreaseHP;			}
	inline BOOL             GetDecreaseMP		( VOID )  {  return m_bDecreaseMP;			}
	inline FLOAT            GetPerResultHP		( VOID )  {  return m_PerResultHP;			}
	inline FLOAT            GetPerResultMP		( VOID )  {  return m_PerResultMP;			}
	inline BOOL             GetRemove           ( VOID )  {  return m_bRemove;				}
	inline INT32            GetRemoveCount      ( VOID )  {  return m_nRemoveCount;			}

	inline FLOAT            GetPerAfterImageHP  ( VOID )  {  return m_PerAfterImageHP;		}
	inline FLOAT            GetSpeedAfterImageHP( VOID )  {  return m_SpeedAfterImageHP;	}
	inline FLOAT            GetPerAfterImageMP  ( VOID )  {  return m_PerAfterImageMP;		}
	inline FLOAT            GetSpeedAfterImageMP( VOID )  {  return m_SpeedAfterImageMP;	}
	//--------------------------------------------------------------------------------------

protected:
	// Update
	VOID					_DrawHPBar		( VOID );
	VOID					_DrawHPMPBar	( VOID );

	// Draw
	BOOL					_HPBarUpdate	( DWORD tickDiff , RwMatrix* CamMat );
	BOOL					_HPMPBarUpdate	( DWORD tickDiff , RwMatrix* CamMat );


	// member variable----------------------------------------------------
	RwV3d				m_CamPosHPMP;
	RwV3d				m_ScrPosHPMP;
	FLOAT				m_RecipzHPMP;

	BOOL				m_bDrawHPMP;
	RwV2d				m_OffsetHPMP;

	BOOL				m_bDecreaseHP;
	FLOAT				m_PerResultHP;
	FLOAT				m_PerAfterImageHP;
	FLOAT				m_SpeedAfterImageHP;			// 잔상 감소 속도

	BOOL				m_bDecreaseMP;
	FLOAT				m_PerResultMP;
	FLOAT				m_PerAfterImageMP;
	FLOAT				m_SpeedAfterImageMP;			// 잔상 감소 속도

	BOOL				m_bRemove;
	INT32				m_nRemoveCount;

};
