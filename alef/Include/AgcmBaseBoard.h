#pragma once

#include "AgcmTextBoardDef.h"
#include "Image2D.h"
#include "AgcmTextBoardMng.h"


class AgcmBaseBoard
{
public:
	AgcmBaseBoard( eBoardType     eType = AGCM_BOARD_COUNT );
	AgcmBaseBoard( AgcmBaseBoard& rhs   );
	virtual ~AgcmBaseBoard( VOID );

	eBoardType					GetBoardType( VOID )  { return m_Type; }
	VOID						SetBoardType( eBoardType eType ) { m_Type	=	eType;}

	virtual VOID				Render( VOID )										PURE;
	virtual BOOL				Update( IN DWORD tickDiff , IN RwMatrix* CamMat )	PURE;

	// set ---------------------------------------------------------------------------------------
	inline VOID					SetCameraPos( IN RwV3d&   Pos				) { m_CameraPos  = Pos;				}
	inline VOID					SetScreenPos( IN RwV3d&   Pos				) { m_ScreenPos  = Pos;				}
	inline VOID					SetScreenPos( FLOAT x , FLOAT y , FLOAT z	);
	inline VOID					SetOffset   ( IN RwV2d&   Offset			) { m_OffSet     = Offset;			}
	inline VOID					SetOffset   ( IN RwReal x , RwReal y		) { m_OffSet.x = x; m_OffSet.y = y;	}
	inline VOID					SetHeight   ( IN FLOAT    fHeight			) { m_Height     = fHeight;			}
	inline VOID					SetRecipz   ( IN FLOAT    Recipz			) { m_Recipz     = Recipz;			}
	inline VOID					SetClump    ( IN RpClump* pClump			) { m_pClump     = pClump;			}
	inline VOID					SetDraw     ( IN BOOL     bDraw				) { m_bDraw      = bDraw;			}
	inline VOID					SetVisible  ( IN BOOL     bVisible			) { m_bVisible   = bVisible;		}
	inline VOID					SetAlpha    ( IN INT      nAlpha			) { m_nAlpha     = nAlpha;			}
	// -------------------------------------------------------------------------------------------

	// get -------------------------------------------------------------------
	inline RwV3d&				GetScreenPos	( VOID )	{ return m_ScreenPos; }
	inline RwV3d&				GetCameraPos	( VOID )	{ return m_CameraPos; }
	inline RwV2d&				GetOffset		( VOID )	{ return m_OffSet;    }
	inline RpClump*				GetClump		( VOID )	{ return m_pClump;    }
	inline FLOAT				GetHeight		( VOID )	{ return m_Height;    }
	inline FLOAT				GetRecipz		( VOID )	{ return m_Recipz;    }
	inline FLOAT*				GetRecipzPtr	( VOID )	{ return &m_Recipz;   }
	inline BOOL					GetDraw			( VOID )    { return m_bDraw;     }
	inline BOOL					GetVisible		( VOID )	{ return m_bVisible;  }
	inline INT					GetAlpha		( VOID )	{ return m_nAlpha;    }
	// -----------------------------------------------------------------------

	static VOID					SetCamera		( RwCamera*         pCamera )	{ ms_pCamera   = pCamera;	}
	static VOID					SetMng			( AgcmTextBoardMng* pMng    )	{ ms_pMng      = pMng;		}

protected:

	eBoardType					m_Type;

	RwV3d						m_CameraPos;
	RwV3d						m_ScreenPos;
	RwV2d						m_OffSet;
	
	FLOAT						m_Height;
	FLOAT						m_Recipz;

	BOOL						m_bDraw;
	RpClump*					m_pClump;

	BOOL						m_bVisible;
	INT							m_nAlpha;

	static RwCamera*            ms_pCamera;
	static Image2D              ms_Image2D;
	static AgcmTextBoardMng*    ms_pMng;
};
