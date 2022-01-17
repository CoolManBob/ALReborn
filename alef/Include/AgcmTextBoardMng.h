#pragma once

#include "AgcmTextBoardDef.h"
#include "CFlagImoticon.h"

typedef vector< RwTexture* >		RwTextureVec;
typedef RwTextureVec::iterator		RwTextureVecItr;

class AgcmPostFX;
class AgcmSound;
class AgcmUIOption;

class AgcmBaseBoard;
class AgcmHPBar;
class AgcmDmgBoard;
class AgcmIDBoard;
class AgcmImoticonBoard;
class AgcmTextBoard;
class AgcmUIManager2;

class CPostFX
{
public:
	CPostFX();
	~CPostFX();

	BOOL	Init();
	VOID	Destory();
	VOID	AddModules( AgcmPostFX* postFX  );
	VOID	Update();

	VOID	PlayEffectFX( const char* szText, float fSpeedScale );

public:
	AgcmPostFX*		m_pcsAgcmPostFX;

private:
	float	m_fStart;
	bool	m_bCircleFX;
	char	m_szPipeBU[1024];
	BOOL	m_bPostFXOn;
	float	m_fSpeedScale;
};

typedef struct _BoardAttr
{
	eBoardType  eType;
	string      String;
	INT32       fType;
	INT32       ViewCount;
	INT         nDamage;
	DWORD       Color;

	_BoardAttr() { eType = (eBoardType)0 ; String = ""; fType = 0; ViewCount = 0; nDamage = 0; Color = 0;}
}sBoardAttr , *sBoardAttrPtr;

typedef struct _sBoardData : ApMemory< _sBoardData , 1000 >
{
	ApBase*                           pBase;           // pBase로 값을 찾는다
	
	AgcmIDBoard*                      pIDBoard;        // Character ID or ITEM ID
	AgcmHPBar*                        pHPBar;          // HP Bar
	AgcmTextBoard*                    pTextBoard;      // Text Board
	std::list< AgcmDmgBoard* >        pDmgBoard;       // Damage and Critical Damage
	std::list< AgcmImoticonBoard* >   pImotioconBoard; // Emoticon

	// Initialize
	_sBoardData() 
	{ pBase   = NULL;   pIDBoard  = NULL;   pHPBar  = NULL;    pTextBoard  = NULL;  }

}sBoardData , *sBoardDataPtr;

class AgcmTextBoardMng
	: public AgcModule , 
	  public CPostFX,
	  public CFlagImoticon    
	 // public SingleTon< AgcmTextBoardMng > , 싱클톤은 여기서 필요없어 보여서 그냥 뺐음 
{
private:
	typedef std::list  < sBoardDataPtr >   		BoardDataList;      // 모든 Board 정보는 여기에
	typedef BoardDataList::iterator        		BoardDataListIter;  

public:
	AgcmTextBoardMng( VOID );
	virtual ~AgcmTextBoardMng( VOID );

	BOOL				OnAddModule				( VOID );
	BOOL				OnInit					( VOID );
	BOOL				OnDestroy				( VOID );
	BOOL				OnIdle					( UINT32 ulClockCount );
	
	sBoardDataPtr		CreateBoard				( UINT32    nID   , sBoardAttrPtr pAttr  );
	sBoardDataPtr		CreateBoard				( ApBase*   pBase , sBoardAttrPtr pAttr  );

	sBoardDataPtr		EnableHPBar				( ApBase*   pBase , BOOL bRemove = FALSE , INT32 nRemoveCount = 1000 );
	VOID				DisableHPBar			( ApBase*   pBase  );
	VOID				DisableHPBar			( RpClump*  pClump );

	sBoardDataPtr		EnableGuild				( ApBase*     pBase , const char* szString , DWORD dwGuildColor );
	VOID				DisableGuild			( ApBase*     pBase );

	sBoardDataPtr		EnableParty				( ApBase*     pBase );
	VOID				DisableParty			( ApBase*     pBase );
	VOID				DisableAllParty			( VOID );

	sBoardDataPtr		EnableIDFlag			( ApBase*     pBase , DWORD dwFlag );
	VOID				DisableIDFlag			( ApBase*     pBase , DWORD dwFlag );

	sBoardDataPtr		GetBoard				( UINT32         nID        );
	sBoardDataPtr		GetBoard				( ApBase*        pBase      );

	BOOL				DestroyBoard			( ApBase*          pBase      );
	AgcmBaseBoard*		DestroyBoard			( AgcmBaseBoard*   pTextBoard );
	BOOL				DestroyBoard			( sBoardDataPtr    pBoardData );
	BOOL				DestroyMainBoard		( VOID );

	VOID				PostRender				( VOID );
	VOID				Update					( INT32 TickDiff , RwMatrix* CamMat );
	VOID				_Update					( sBoardDataPtr	pBoardData , INT32 TickDiff , ApBase*	pBase );

	VOID				ToggleIDDisplay			( VOID ) {  m_bDrawID          = !m_bDrawID;           }
	VOID				ToggleMiniHPBar			( VOID ) {  m_bEnableMiniHPBar = !m_bEnableMiniHPBar;  }

	VOID				SetWorldCamera			( RwCamera* pCamera ); 
	const RwCamera*		GetWorldCamera			( VOID )                   { return m_pCamera;      }

	VOID				Trans3DTo2D				( RwV3d* pCameraPos , RwV3d* pScreenPos, FLOAT* pRecip , const RwV3d* pWorldPos, const FLOAT fHeight = 0.f, const RpClump* pClump = NULL, const FLOAT fDepth = 0.f );
	FLOAT				GetWorldPosToScreenPos	( RwV3d* pWorldPos  , RwV3d* pCameraPos, RwV3d* pScreenPos , float* recip_z);	// 리턴값은 w값

	BOOL				SetTexture				( INT32 nType, INT32 nIndex, RwTexture* pTexture );
	RwV2d				GetTextureSize			( RwTexture* pTexture );

	VOID				ChangeIDColor			( AgpdCharacter* pCharacter , DWORD Color );

	DWORD				GetGuildIDColor			( const char* szMemberID );
	DWORD				GetGuildIDColor			( const char* szGuildID , const char* szMemberID , DWORD dwDefaultColor = 0xFF81FAB2 );
	DWORD				GetTargetIDColor		( AgpdCharacter* pcsCharacter );
	FLOAT				GetPointPercent			( AgpdCharacter* pdCharacter, INT32 nMode );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Callback Function  - start																			//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	static BOOL				CB_POST_RENDER					( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_UpdateIDMurderIconFlag       ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_UpdateIDFirstAttackerIconFlag( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_SetSelfCharacter             ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_ReleaseSelfCharacter         ( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_AddChar						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_RemoveChar					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_UpdateFactor					( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_MissAttack					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_MissCastSkill				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_BlockCastSkill				( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_DisplayActionResult			( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_ItemAdd						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_ItemRemove					( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_UpdateActionStatus			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_RegionChange					( PVOID pData, PVOID pClass, PVOID pCustData );
	
	static BOOL				CB_SkillAddHPBar				( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_HPUpdate						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_MPUpdate						( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_Guild						( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_PartyAdd						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_PartyLeave					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_PartyAddMember				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_PartyLeaveMember				( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_UpdateIDBoard				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_UpdateIDBoardHPBar			( PVOID pData, PVOID pClass, PVOID pCustData );		

	static BOOL				CB_PvPRefreshChar				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_PvPRefreshGuild				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_PvPInit						( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_KillCharacter				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL				CB_UpdateNickName				( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL				CB_DestroyBoard					( PVOID pData, PVOID pClass, PVOID pCustData );
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Callback Function  - end																				//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	//get-------------------------------------------------------------------------------------
	AgpmCharacter*			GetAgpmCharacter	( VOID )		{	return m_pcsAgpmCharacter;			}
	AgcmRender*				GetAgcmRender		( VOID )		{	return m_pcRender;					}
	AgcmCharacter*			GetAgcmCharacter	( VOID )		{	return m_pcCmCharacter;				}
	AgcmFont*				GetAgcmFont			( VOID )		{	return m_pcFont;					}
	ApmMap*					GetApmMap			( VOID )		{	return m_pcApmMap;					}

	AgpmFactors*			GetAgpmFactors		( VOID )		{	return m_pcsAgpmFactors;			}
	AgpmItem*				GetAgpmItem			( VOID )		{	return m_pcsAgpmItem;				}
	AgcmItem*				GetAgcmItem			( VOID )		{	return m_pcsAgcmItem;				}
	ApmObject*				GetApmObject		( VOID )		{	return m_pcsApmObject;				}
	AgcmObject*				GetAgcmObject		( VOID )		{	return m_pcsAgcmObject;				}

	ApmEventManager*		GetApmEventManager	( VOID )		{	return m_pcsApmEventManager;		}
	AgpmEventTeleport*		GetAgpmEventTeleport( VOID )		{	return m_pcsAgpmEventTeleport;		}
	AgpmSkill*				GetAgpmSkill		( VOID )		{	return m_pcsAgpmSkill;				}
	AgcmSkill*				GetAgcmSkill		( VOID )		{	return m_pcsAgcmSkill;				}
	AgpmGuild*				GetAgpmGuild		( VOID )		{	return m_pcsAgpmGuild;				}

	AgpmParty*				GetAgpmParty		( VOID )		{	return m_pcsAgpmParty;				}
	AgpmPvP*				GetAgpmPvP			( VOID )		{	return m_pcsAgpmPvP;				}
	AgpmBillInfo*			GetAgpmBillInfo		( VOID )		{	return m_pcsAgpmBillInfo;			}
	AgpmSiegeWar*			GetAgpmSiegeWar		( VOID )		{	return m_pcsAgpmSiegeWar;			}
	AgcmSiegeWar*			GetAgcmSiegeWar		( VOID )		{	return m_pcsAgcmSiegeWar;			}

	AgpmBattleGround*		GetAgpmBattleGround	( VOID )		{	return m_pcsAgpmBattleGround;		}
	AgcmUIControl*			GetAgcmUIcontrol	( VOID )		{	return m_pcsAgcmUIControl;			}
	AgcmSound*				GetAgcmSound		( VOID )		{	return m_pcsAgcmSound;				}
	AgcmUIOption*			GetAgcmUIOption		( VOID )		{	return m_pcsAgcmUIOption;			}

	BOOL					GetMakeTextBaloon	( VOID )		{  return m_bMakeTextBaloon;			}
	BOOL					GetDrawNameMine		( VOID )		{  return m_bDrawNameMine;				}
	BOOL					GetDrawNameMonster	( VOID )		{  return m_bDrawNameMonster;			}
	BOOL					GetDrawNameGuild	( VOID )		{  return m_bDrawNameGuild;				}
	BOOL					GetDrawNameParty	( VOID )		{  return m_bDrawNameParty;				}

	BOOL					GetDrawNameClass	( VOID )		{  return m_bDrawNameClass;				}
	BOOL					GetDrawNameOther	( VOID )		{  return m_bDrawNameOthers;			}
	BOOL					GetDrawHPMP			( VOID )		{  return m_bDrawHPMP;					}
	BOOL					GetDrawID			( VOID )		{  return m_bDrawID;					}
	BOOL					GetEnableMiniHPBar	( VOID )		{  return m_bEnableMiniHPBar;			}

	RwTexture*				GetImgBalloon		( INT Index )	{	return m_ImgBalloon[Index];			}
	RwTexture*				GetImgBalloonTail	( INT Index )	{	return m_ImgBalloonTail[Index];		}
	RwTexture*				GetImgIDTag			( INT Index )	{	return m_ImgIDTag[Index];			}
	RwTexture*				GetImgImoticon		( INT Index	)	{	return m_ImgImoticon[Index];		}
	RwTexture*				GetImgGoMark		( VOID )		{	return m_ImgGOMark;					}
	RwTexture*				GetImgDamage		( VOID )		{	return m_ImgDamage;					}
	RwTexture*				GetImgEtc			( INT Index	)	{	return m_ImgIDETC[Index];			}
	RwTexture*				GetImgHPBar			( INT Index )	{	return m_ImgHPBar[Index];			}
	RwTexture*				GetCastleTexture	( UINT64 uiState );
	RwTexture*				GetStateTexture		( AgpdCharacter* pdCharacter );

	FLOAT					GetDamageSizeSmall	( INT Index )	{	return m_fDamageSizeSmall[Index];	}
	RwV3d&					GetDamageUVW		( INT Index )	{	return m_vDamageUVW[Index];			}
	const RwTextureVec&		GetTextureVec		( VOID )		{	return m_vecTexCharisma;			}

	DWORD					GetTickDiff			( VOID )		{	return m_TickDiff;					}
	DWORD					GetLastTick			( VOID )		{	return m_LastTick;					}	

	sBoardDataPtr			GetMainCharacter	( VOID )		{	return m_pMainCharacterID;			}
	BoardDataList&			GetBoardDataList	( VOID )		{	return m_BoardDataList;				}

	DWORD					GetBGNameColor		( eBattleGroundNameColor eType )	{	return m_BattleNameColor[ eType ]; }
	//----------------------------------------------------------------------------------------

	//set------------------------------------------------------------------------------------------------------------
	VOID					SetAgcmUIOption		( AgcmUIOption* pUIOpTion	) { m_pcsAgcmUIOption	= pUIOpTion;		}

	VOID					SetMakeTextBaloon	( BOOL bMakeTextBaloon		) { m_bMakeTextBaloon   = bMakeTextBaloon;  }
	VOID					SetDrawNameMine		( BOOL bDrawNameMine		) { m_bDrawNameMine     = bDrawNameMine;    }
	VOID					SetDrawNameMonster	( BOOL bDrawNameMonster		) { m_bDrawNameMonster  = bDrawNameMonster; }
	VOID					SetDrawNameGuild	( BOOL bDrawNameGuild		) { m_bDrawNameGuild    = bDrawNameGuild;   }
	VOID					SetDrawNameParty	( BOOL bDrawNameParty		) { m_bDrawNameParty    = bDrawNameParty;   }
	VOID					SetDrawNameClass	( BOOL bDrawNameClass		) { m_bDrawNameClass    = bDrawNameClass;   }
	VOID					SetDrawNameOther	( BOOL bDrawNameOther		) { m_bDrawNameOthers   = bDrawNameOther;   }
	VOID					SetDrawHPMP			( BOOL bDrawHPMP			) { m_bDrawHPMP         = bDrawHPMP;        }
	VOID					SetDrawID			( BOOL bDrawID				) { m_bDrawID           = bDrawID;          }
	VOID					SetEnableMiniHPBar	( BOOL bEnable				) { m_bEnableMiniHPBar  = bEnable;          }
	//--------------------------------------------------------------------------------------------------------------

	BOOL					SetMainCharacter	( sBoardDataPtr pBoardData );
	VOID					SetHeight			( ApBase* pBase );

	VOID					SetAllIDEnable		( TEXTBOARD_IDTYPE eIDType , BOOL bVal );
	VOID					SetAllGuildEnable	( BOOL bVal );
	VOID					SetAllPartyEnable	( BOOL bVal );
	
	VOID					UpdateIDBoardHPBar	( AgcmHPBar*   pHPBar );
	BOOL					UpdateNickName		( ApBase* pBase );
	BOOL					ChangeGuildIDColor	( AgpdCharacter* pdCharacter , const char* szOtherGuildID );
	
	BOOL					GetFactorDamage		( AgpdFactor* pFactor, AgpdFactorCharPointType eFactorType, INT32& nTotalDamage, INT32& nPreDamage );
	BOOL					DisplayUpdateFactor	(AgpdCharacter *pcsCharacter, AgpdFactor *pcsFactor);

	BOOL					StreamReadTemplate	( CHAR *szFile, BOOL bDecryption );
	BOOL					PvPRefreshSiege		( CHAR *pszGuildName );

	VOID					RenderText			( char* szText, RwV3d& vPos, RwFrame* pFrame, float fDepth = 1.f );
	VOID					RenderTexture		( RwTexture* pTexture, RwV3d& vCamera, RwV3d& vScreen, float fRecipz = 1.f, RwV2d* pOffset = NULL );

	BOOL					CheckPeculiarity	( AgpdCharacter*	pCharacter , OUT INT& nResult );

	BOOL					IsRender			( AgpdCharacter* pdCharacter );
	VOID					DrawGuildImage		( AgcmIDBoard* pIDBoard , RwReal fPosX, RwTexture* pTexture, DWORD dwColor = 0xffffffff, float fScale = 1.f, int nJoinMax = 0, int nJoinIndex = 0, BOOL bIsJoinSiegeWar = FALSE );
	VOID					CalculateJointGuildMarkPositionOffset(RwV2d* pOffset, float fSize, int TotalMarkCount, int CurrentMarkIndex);
	
private:
	sBoardDataPtr			_CreateItemID		( ApBase* pBase , sBoardAttrPtr pAttr );
	sBoardDataPtr			_CreateIDBoard		( ApBase* pBase , sBoardAttrPtr pAttr );
	sBoardDataPtr			_CreateHPBar		( ApBase* pBase , BOOL bRemove , INT32 nRemoveCount  );
	sBoardDataPtr			_CreateDmg			( ApBase* pBase , sBoardAttrPtr pAttr );
	sBoardDataPtr			_CreateText			( ApBase* pBase , sBoardAttrPtr pAttr );
	sBoardDataPtr			_CreateImoticon		( ApBase* pBase , sBoardAttrPtr pAttr );

	// member variable-------------------------------------------------------------------------------------------------------
	BoardDataList				m_BoardDataList;

	AgpmCharacter*				m_pcsAgpmCharacter;
	AgcmCharacter*				m_pcsAgcmCharacter;
	AgcmRender*					m_pcRender;
	AgcmCharacter*				m_pcCmCharacter;
	AgcmFont*					m_pcFont;
	ApmMap*						m_pcApmMap;
	AgpmFactors*				m_pcsAgpmFactors;
	AgpmItem*					m_pcsAgpmItem;
	AgcmItem*					m_pcsAgcmItem;
	ApmObject*					m_pcsApmObject;
	AgcmObject*					m_pcsAgcmObject;
	ApmEventManager*			m_pcsApmEventManager;
	AgpmEventTeleport*			m_pcsAgpmEventTeleport;
	AgpmSkill*					m_pcsAgpmSkill;
	AgcmSkill*					m_pcsAgcmSkill;
	AgpmGuild*					m_pcsAgpmGuild;
	AgpmParty*					m_pcsAgpmParty;
	AgpmPvP*					m_pcsAgpmPvP;
	AgpmBillInfo*				m_pcsAgpmBillInfo;
	AgpmSiegeWar*				m_pcsAgpmSiegeWar;
	AgcmSiegeWar*				m_pcsAgcmSiegeWar;
	AgpmBattleGround*			m_pcsAgpmBattleGround;
	AgcmUIControl*				m_pcsAgcmUIControl;
	AgcmSound*				    m_pcsAgcmSound;
	AgcmUIOption*			    m_pcsAgcmUIOption;
	AgcmUIManager2*				m_pcsAgcmUIManager2;

	RwV3d				        m_vDamageUVW      [10];
	FLOAT				        m_fDamageSizeSmall[10];

	RwCamera*				    m_pCamera;

	RwTexture*                  m_ImgIDETC		[ TBID_TEXTURE_ADDINTIONAL_SIZE ];
	RwTexture*                  m_ImgBalloon	[ TB_IMAGE_COUNT      ];
	RwTexture*                  m_ImgBalloonTail[ TB_IMAGE_TAIL_COUNT ];
	RwTexture*                  m_ImgImoticon	[ IMOTICON_COUNT-530  ];
	RwTexture*                  m_ImgIDTag		[ TB_MAX_TAG          ];
	RwTexture*                  m_ImgHPBar		[ 3 ];
	RwTexture*                  m_ImgGOMark;
	RwTexture*                  m_ImgDamage;

	RwTextureVec                m_vecTexCharisma;

	UINT32                      m_TickDiff;
	UINT32                      m_LastTick;

	BOOL                        m_bMakeTextBaloon;
	BOOL					    m_bDrawNameMine;
	BOOL						m_bDrawNameMonster;
	BOOL						m_bDrawNameGuild;
	BOOL						m_bDrawNameParty;
	BOOL						m_bDrawNameClass;
	BOOL						m_bDrawNameOthers;
	BOOL						m_bDrawHPMP;
	BOOL						m_bDrawID;
	BOOL						m_bEnableMiniHPBar;

	sBoardDataPtr        		m_pMainCharacterID;

	DWORD						m_BattleNameColor[ BATTLEGROUND_COLOR_COUNT ];
};
