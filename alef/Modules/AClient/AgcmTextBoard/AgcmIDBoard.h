#pragma once
#include "AgcmBaseBoard.h"

class AgcmIDBoard :
	public AgcmBaseBoard
{
public:
	AgcmIDBoard( eBoardType     eType = AGCM_BOARD_COUNT );  
	virtual ~AgcmIDBoard( VOID );

	virtual VOID 			Render					( VOID );
	virtual BOOL 			Update					( IN DWORD tickDiff , IN RwMatrix* CamMat );
	//-------------------------------------------------------------------

	//set-----------------------------------------------------------------------------------------------------------------
	inline VOID				SetEnabled				( IN  BOOL     bEnable			) { m_bEnabled   = bEnable;				}
	inline VOID				SetFontType				( IN  INT      FontType			) { m_FontType   = FontType;			}
	inline VOID				SetDepth				( IN  FLOAT    fDepth			) { m_Depth      = fDepth;				}
	inline VOID				SetColor				( IN  DWORD    Color			) { m_Color      = Color;				}

	inline VOID				SetApBase				( IN  ApBase*  pBase			) { m_pBase      = pBase;				}
	inline VOID				SetPosition				( IN  AuPOS*   pPos				) { m_pPosition  = pPos;				}

	inline VOID				SetCharismaStep			( IN  INT32    CharismaStep		) { m_CharismaStep   = CharismaStep;	}
	inline VOID				SetColorCharisma		( IN  DWORD    Color			) { m_ColorCharisma  = Color;			}
	inline VOID				SetColorNickName		( IN  DWORD    Color			) { m_ColorNickName  = Color;			}
	inline VOID				SetColorTitleName		( IN  DWORD    Color			) { m_ColorTitleName = Color;			}

	inline VOID				SetEnableParty			( IN  INT32    nEnableParty		) { m_nEnableParty = nEnableParty;		}
	inline VOID				SetParty				( IN  BOOL     bParty			) { m_bParty       = bParty;			}

	inline VOID				SetEnableGuild			( IN  INT32	   nEnableGuild		) { m_nEnableGuild = nEnableGuild;		}
	inline VOID				SetGuild				( IN  BOOL	   bGuild			) { m_bGuild       = bGuild;			}
	inline VOID				SetColorGuild			( IN  DWORD	   Color			) { m_ColorGuild   = Color;				}
	inline VOID				SetOffsetGuild			( IN  RwV2d&   OffsetGuild		) { m_OffSetGuild  = OffsetGuild;		}
	inline VOID				SetOffsetTitle			( IN  RwV2d&   OffsetTitle		) { m_OffSetTitle  = OffsetTitle;		}
	inline VOID				SetIDType				( IN  TEXTBOARD_IDTYPE eType	) { m_IDType       = eType;				}
	inline VOID				SetFlag					( IN  DWORD    dwFlag			) { m_dwFlag       = dwFlag;			}
	inline VOID				SetTitle				( IN  BOOL     bTitle			) { m_bTitle	   = bTitle;			}

	inline VOID				SetText					( IN const string& str			) { sprintf_s( m_Text        , sizeof(m_Text), "%s" , str.c_str() ); }
	inline VOID				SetGuildText			( IN const string& str			) { sprintf_s( m_strGuild    , sizeof(m_strGuild), "%s" , str.c_str() ); }
	inline VOID				SetCharismaText			( IN const string& str			) { sprintf_s( m_strCharisma , sizeof(m_strCharisma), "%s" , str.c_str() ); }
	inline VOID				SetNickNameText			( IN const string& str			) { sprintf_s( m_strNickName , sizeof(m_strNickName), "%s" , str.c_str() ); }
	inline VOID				SetTitleNameText		( IN const string& str			) { sprintf_s( m_strTitleName, sizeof(m_strTitleName), "%s" , str.c_str() ); }
	inline VOID				SetBattleText			( IN const string& str			) { sprintf_s( m_strBattle   , sizeof(m_strBattle), "%s" , str.c_str() ); }
	//--------------------------------------------------------------------------------------------------------------------

	//get-----------------------------------------------------------------
	
	inline BOOL				GetEnabled				( VOID )  { return m_bEnabled;		}
	inline INT				GetFontType				( VOID )  { return m_FontType;		}
	inline FLOAT			GetDepth				( VOID )  { return m_Depth;			}
	inline DWORD			GetColor				( VOID )  { return m_Color;			}

	inline INT32			GetCharismaStep			( VOID )  { return m_CharismaStep;  }
	inline DWORD			GetColorCharisma		( VOID )  { return m_ColorCharisma; }
	inline DWORD			GetColorNickName		( VOID )  { return m_ColorNickName; }
	inline DWORD			GetColorTitleName		( VOID )  { return m_ColorTitleName;}

	inline INT32			GetEnableParty			( VOID )  { return m_nEnableParty;  }
	inline BOOL				GetParty				( VOID )  { return m_bParty;        }

	inline INT32			GetEnableGuild			( VOID )  { return m_nEnableGuild;  }
	inline BOOL				GetGuild				( VOID )  { return m_bGuild;        }
	inline DWORD	  		GetColorGuild			( VOID )  { return m_ColorGuild;    }
	inline DWORD			GetFlag					( VOID )  { return m_dwFlag;        }

	inline BOOL				GetTitle				( VOID )  { return m_bTitle;		}
	inline RwV2d&			GetOffsetTitle			( VOID )  { return m_OffSetTitle;	}

	inline TEXTBOARD_IDTYPE GetIDType				( VOID )  { return m_IDType;        }

	inline RwV2d&			GetOffsetGuild			( VOID )  { return m_OffSetGuild;   }
	inline AuPOS*			GetPosition				( VOID )  { return m_pPosition;     }      
	inline ApBase*			GetApBase				( VOID )  { return m_pBase;         }
	
	inline char*			GetText					( VOID )  { return m_Text;          }
	inline char*			GetGuildText			( VOID )  { return m_strGuild;      }
	inline char*			GetCharismaText			( VOID )  { return m_strCharisma;   }
	inline char*			GetNickNameText			( VOID )  { return m_strNickName;   }
	inline char*			GetTitleNameText		( VOID )  { return m_strTitleName;  }
	inline char*			GetBattleText			( VOID )  { return m_strBattle;     }

	inline float			GetOffsetXLeft			( VOID )  { return m_fOffsetXLeft;	}
	inline float			GetOffsetXRight			( VOID )  { return m_fOffsetXRight;	}

	//--------------------------------------------------------------------

public :
	VOID					ReCalulateOffsetX		( VOID );

protected:
	VOID					DrawItem				( VOID );
	VOID					DrawChar				( VOID );

	VOID					_DrawName				( RwReal fPosX, RwReal fPosY, INT nAlpha , BOOL bDraw );
	VOID					_DrawTitleName			( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawGoMark				( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawGuild				( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawNameEx				( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawCharisma			( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawMonsterAbility		( RwReal fPosX, RwReal fPosY, INT nAlpha );
	VOID					_DrawFlag				( VOID );
	VOID					_DrawGuildCompetition	( VOID );

	AuPOS*					m_pPosition;					
	ApBase*					m_pBase; 

	BOOL					m_bEnabled;
	INT						m_FontType;
	FLOAT					m_Depth;
	char					m_Text[ TB_ID_MAX_CHAR + 1 ];
	DWORD					m_Color;

	TEXTBOARD_IDTYPE		m_IDType;

	// Charisma Step?
	INT32					m_CharismaStep;
	char					m_strCharisma[ TB_ID_MAX_CHAR + 1 ];
	char					m_strNickName[ TB_ID_MAX_CHAR + 1 ];
	DWORD					m_ColorNickName;
	DWORD					m_ColorCharisma;

	char					m_strBattle[ TB_ID_MAX_CHAR + 1 ];
	
	// Party
	INT32					m_nEnableParty;
	BOOL					m_bParty;

	// Guild
	INT32					m_nEnableGuild;
	BOOL					m_bGuild;
	char					m_strGuild[ TB_ID_MAX_CHAR + 1 ];
	DWORD					m_ColorGuild;
	RwV2d					m_OffSetGuild;

	// Flag
	DWORD					m_dwFlag;
	UINT8					m_nPrevFlag;

	// Title
	char					m_strTitleName[ TB_ID_MAX_CHAR + 1 ];
	DWORD					m_ColorTitleName;
	BOOL					m_bTitle;
	RwV2d					m_OffSetTitle;

	// Common
	float					m_fOffsetXLeft;
	float					m_fOffsetXRight;

private :
	void					_Draw_Normal			( AgpdCharacter* ppdCharacter );
	void					_Draw_InGuildStadium	( AgpdCharacter* ppdCharacter );

	BOOL					_IsInGuildStadium		( AgpdCharacter* ppdCharacter );

	DWORD					_GetIDColorByPVP		( AgpdCharacter* ppdCharacter );
};
