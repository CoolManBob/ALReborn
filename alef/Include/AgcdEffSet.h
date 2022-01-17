#if !defined(AFX_AGCDEFFSET_H__8F27FAC4_E7E2_46B5_9CCF_457EA2AD34C2__INCLUDED_)
#define AFX_AGCDEFFSET_H__8F27FAC4_E7E2_46B5_9CCF_457EA2AD34C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "ApModule.h"
#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"
#include "AgcdEffPublicStructs.h"

#include <map>
#include <list>


#define FLAG_EFFSET_BILLBOARD	0x00000001
#define FLAG_EFFSET_TARGET		0x00000002
#define FLAG_EFFSET_MISSILE		0x00000004
#define FLAG_EFFSET_ONLYTARGET	0x00000008

#define	FLAG_EFFSET_ONLYSOUND	0x00000010		// sound 만 있는경우.
#define	FLAG_EFFSET_ONLYTAIL	0x00000020		// tail 만 있는경우.

#define FLAG_EFFSET_MFRM		0x00000040		//moving frame
#define FLAG_EFFSET_MFRM_SRC	0x00000080		//moving frame source
#define FLAG_EFFSET_MFRM_DST	0x00000100		//moving frame destnation
#define FLAG_EFFSET_MFRM_CAM	0x00000200		//moving frame camera

#define	FLAG_EFFSET_HASTAIL		0x00000400		//베이스 디펜던시가 없는 테일을 가지고 있다. ex) 칼궤적

#define FLAG_EFFBASE_DEPENDANCY_ONLYPOS	0x00000001
#define FLAG_EFFBASE_DEPENDANCY_HASNODE	0x00000002	// parent 가 object 일때..

enum	eMissileMoveType
{
	e_missile_linear	= 0	,
	e_missile_bezier3		,
	e_missile_linear_rot	,
	e_missile_bezier3_rot	,
	e_missile_zigzag		,

	e_missile_num,
};
struct tagStMissileInfo
{	
	eMissileMoveType	m_eMissileType;
	RwV3d				m_v3dOffset;	//relative to missile_direction

	tagStMissileInfo() : m_eMissileType( e_missile_linear )
	{
		m_v3dOffset.x = m_v3dOffset.y = m_v3dOffset.z = 0.f;
	};
};
typedef tagStMissileInfo STMISSILEINFO, *PSTMISSILEINFO, *LPSTMISSILEINFO;

enum AGCDEFFSETRESOURCELOADSTATUS
{
	AGCDEFFSETRESOURCELOADSTATUS_LOADING = 0,
	AGCDEFFSETRESOURCELOADSTATUS_LOADED,
	AGCDEFFSETRESOURCELOADSTATUS_UNKNOWN
};

class AgcdEffSetMng;
class AgcdEffSet  : public ApBase,  public ApMemory<AgcdEffSet, 3000>
{
	EFFMEMORYLOG_SMV;

	friend class AgcdEffSetMng;

public:
	struct StVarSizeInfo
	{
		StVarSizeInfo() : m_nNumOfTex(0), m_nNumOfBase(0), m_nNumOfBaseDependancy(0)	{		};

		RwInt32		m_nNumOfTex;				//텍스쳐가 몇장?
		RwInt32		m_nNumOfBase;				//EffBase가 몇개?
		RwInt32		m_nNumOfBaseDependancy;		//EffBase Dependancy가 몇개?
	};

	struct stBaseDependancy
	{
	private:
		RwInt32		m_nPIndex,					//parent
					m_nCIndex;					//child
		RwUInt32	m_dwBaseDependancyBitFlags;	//옵션 스위치 ( 예 : only pos = 0x00000001)

	public:
		//constructor
		stBaseDependancy() : m_nPIndex(0), m_nCIndex(0), m_dwBaseDependancyBitFlags(0x00000000)		{		};
		stBaseDependancy(const stBaseDependancy& cpy): m_nPIndex(cpy.m_nPIndex), m_nCIndex(cpy.m_nCIndex), m_dwBaseDependancyBitFlags(cpy.m_dwBaseDependancyBitFlags)		{		};

		//operator
		bool operator == (const stBaseDependancy& cmp)const
		{
			return ( m_nCIndex == cmp.m_nCIndex );//차일드가 같으면.. 이미 존재..
		};

		//Get
		RwInt32		GetParentIndexItself		( VOID ) const				{	return m_nPIndex;	}
		RwInt32		GetParentIndex				( VOID ) const				{	return ( DEF_FLAG_CHK( m_dwBaseDependancyBitFlags, FLAG_EFFBASE_DEPENDANCY_HASNODE ) ? static_cast<RwInt32>(Eff2Ut_GET_LOWORD(m_nPIndex)) : m_nPIndex );	}
		RwInt32		GetParentNodeID				( VOID ) const				{	return static_cast<RwInt32>( Eff2Ut_GET_HIWORD(m_nPIndex) );	}
		RwInt32		GetChildIndex				( VOID ) const				{	return m_nCIndex;	}
		RwInt32		GetFlag						( VOID ) const				{	return m_dwBaseDependancyBitFlags;	}
		RwUInt32	FlagChk						( RwUInt32 flag ) const		{	return DEF_FLAG_CHK(m_dwBaseDependancyBitFlags, flag);	}

		//Set
		VOID		SetParentIndex				( RwInt32 parentIndex )		{ 	m_nPIndex = DEF_FLAG_CHK(m_dwBaseDependancyBitFlags, FLAG_EFFBASE_DEPENDANCY_HASNODE) ? Eff2Ut_SET_LONG( parentIndex, Eff2Ut_GET_HIWORD(parentIndex))  : parentIndex;	}
		VOID		SetParentNodeID				( RwInt32 nodeID )			{	m_nPIndex = Eff2Ut_SET_LONG( Eff2Ut_GET_LOWORD(m_nPIndex), nodeID)  ;		}
		VOID		SetChildIndex				( RwInt32 childIndex )		{	m_nCIndex = childIndex;	}
		VOID		FlagOn						( RwUInt32 flag )			{	DEF_FLAG_ON( m_dwBaseDependancyBitFlags, flag );	}
		VOID		FlagOff						( RwUInt32 flag )			{	DEF_FLAG_OFF( m_dwBaseDependancyBitFlags, flag );	}
		RwInt32		SetDataFromStream			( const RwChar* szData )	{	return sscanf( szData, "%d:%d:%x", &m_nPIndex, &m_nCIndex, &m_dwBaseDependancyBitFlags );		}

		//for tool
		RwUInt32*	ForTool_GetPtrFlag			( VOID )					{	return &m_dwBaseDependancyBitFlags;	}
		RwInt32*	ForTool_GetPtrParentIndex	( VOID )					{	return &m_nPIndex;	}
		RwInt32*	ForTool_GetPtrChildIndex	( VOID )					{	return &m_nCIndex;	}
	};
	typedef std::vector<stBaseDependancy>		BaseDependancyVec;
	typedef BaseDependancyVec::iterator			BaseDependancyVecItr;
	typedef BaseDependancyVec::const_iterator	BaseDependancyVecCItr;

	typedef std::vector<LPEFFTEX>				LPEffectExVec;
	typedef LPEffectExVec::iterator				LPEffectExVecItr;
	typedef LPEffectExVec::const_iterator		LPEffectExVecCItr;

	typedef std::vector<LPEFFBASE>				LPEffectBaseVec;
	typedef LPEffectBaseVec::iterator			LPEffectBaseVecItr;
	typedef LPEffectBaseVec::const_iterator		LPEffectBaseVecCItr;

private:
	//do not use
	//AgcdEffSet(const AgcdEffSet& cpy){cpy;};
	//AgcdEffSet& operator = (const AgcdEffSet& cpy){cpy; return *this;};
	AgcdEffSet(const AgcdEffSet& cpy);
	AgcdEffSet& operator = (const AgcdEffSet& cpy);

public:
	AgcdEffSet(RwUInt32 dwEffSetID=0);
	virtual ~AgcdEffSet();

	//static AgcdEffSet*		Clone( VOID )		{	return new AgcdEffSet;	}

	//access
	RwUInt32				bGetID				( VOID ) const			{ return m_dwEffSetID; }
	const RwChar*			bGetTitle			( VOID ) const			{ return m_szTitle; }
	RwUInt32				bGetLife			( VOID ) const			{ return m_dwEffSetLife; }
	E_LOOPOPT				bGetLoopOpt			( VOID ) const			{ return m_eLoopOpt; }
	RwUInt32				bGetFlag			( VOID ) const			{ return m_dwEffSetBitFlags; }
	const STMISSILEINFO&	bGetMissileInfo		( VOID ) const			{ return m_stMissileInfo; }
	STMISSILEINFO&			bGetMissileInfo		( VOID )				{ return m_stMissileInfo; }
	const RwSphere&			bGetBSphere			( VOID ) const			{ return m_BSphere; }
	const RwBBox&			bGetBBox			( VOID ) const			{ return m_BBox; }
	RwInt32					bGetRefCnt			( VOID ) const			{ return m_nRefCnt; }
	RwInt32					bGetAcumCnt			( VOID ) const			{ return m_nAcumCnt; }
	RwInt32					bGetLastShootTime	( VOID ) const			{ return m_dwLastShootTime; }
	const StVarSizeInfo&	bGetVarSizeInfo		( VOID ) const			{ return m_stVarSizeInfo; }
	LPEFFTEX				bGetPtrEffTex		( RwInt32 nTex )		{ return (nTex < (int)m_vecLPEffectEx.size() ) ? m_vecLPEffectEx[nTex] : NULL; 	}
	const stBaseDependancy&	bGetCRefBaseDpnd	( RwInt32 nIndex )		{ return m_vecBaseDependancy[nIndex]; }
	RwInt32					bGetSoundLendth		( RwInt32* pnLength );

	//for tool
	LPEFFBASE				bGetPtrEffBase		( RwInt32 nIndex )		{ return m_vecLPEffectBase[nIndex]; };
	
	//setup
	void					bSetID				( RwUInt32 dwEffSetID )					{	m_dwEffSetID = dwEffSetID;	};
	void					bSetTitle			( const RwChar* szTitle )				{	strncpy( m_szTitle, szTitle, sizeof(m_szTitle)-1 );	};
	void					bSetLife			( RwUInt32 dwEffSetLife )				{	m_dwEffSetLife = dwEffSetLife;	};
	void					bSetLoopOpt			( E_LOOPOPT eLoopOpt )					{	m_eLoopOpt = eLoopOpt;	};
	void					bSetBitFlags		( RwUInt32 dwEffSetBitFlags )			{	m_dwEffSetBitFlags = dwEffSetBitFlags;	};
	void					bSetMissileInfo		( const STMISSILEINFO& stMissileInfo )	{ m_stMissileInfo = stMissileInfo; };
	void					bSetBBox			( const RwBBox& bbox)					{	m_BBox = bbox;	};
	void					bSetBSphere			( const RwSphere& bsphere)				{	m_BSphere = bsphere;	};
	void					bSetVarSizeInfo		( const StVarSizeInfo& StVarSizeInfo )	{ m_stVarSizeInfo = StVarSizeInfo; };

	//reference count
	RwInt32					bAddRef();	//Reference Count increase
	RwInt32					bRelease(void);	//Reference count decrease

	//insert/del
	// DrawIndexedPrimitiveUP 
	RwInt32					bInsEffTex			( const RwChar* szTex, const RwChar* szMask );
	RwInt32					bInsEffBase			( AgcdEffBase::E_EFFBASETYPE eEffBaseType );
	RwInt32					bInsDependancy		( const stBaseDependancy& dependancy );

	RwInt32					bSetEffTexName		( RwInt32 nIndex, const RwChar* szTex, const RwChar* szMask=NULL );
	RwInt32					bSetDependancy		( RwInt32 nIndex, const stBaseDependancy& BaseDpndncy );
	
	RwInt32					bFromFile			( FILE* fp );
	RwInt32					bToFile				( FILE* fp );
	RwInt32					bToFileVariableData	( FILE* fp );
	RwInt32					bFromFileVariableData( FILE* fp, const BOOL bForceImmediate );

private:
	void					vClear				( VOID );
	void					vAllocMem			( VOID );

#ifdef USE_MFC
public:
	BOOL					bForTool_IsTexUsed			( RwInt32 nTexIndex );
	BOOL					bForTool_IsAllTexUsed		( RwChar* szOut );
	RwInt32					bForTool_FindEffTex			( const RwChar* szTex, const RwChar* szMask );
	RwInt32					bForTool_InsEffEffTex		( const RwChar* szTex, const RwChar* szMask );
	RwInt32					bForTool_InsEffBase			( AgcdEffBase::E_EFFBASETYPE eEffBaseType );
	RwInt32					bForTool_InsEffBase			( AgcdEffBase* pEffBase );
	RwInt32					bForTool_InsDependancy		( const stBaseDependancy& dependancy );

	BOOL					bForTool_ChkDependancy		( RwInt32 nBaseIndex );
	BOOL					bForTool_DelDependancyAll	( RwInt32 nBaseIndex );
	RwInt32					bForTool_FindBaseIndex		( LPEFFBASE pBase );

	RwInt32					bForTool_DelEffTex			( RwInt32 nIndex );
	RwInt32					bForTool_DelAllEffTexNotUsed( VOID );
	RwInt32					bForTool_DelEffBase			( RwInt32 nIndex );
	RwInt32					bForTool_DelDependancy		( RwInt32 nIndex );

	RwInt32					bForTool_ChangeBaseTexIndex	( AgcdEffRenderBase* pRenderBase, RwInt32 nOldIndex, RwInt32 nNewIndex );
	RwInt32					bForTool_FindDependancy		( const stBaseDependancy& dependancy )const;

	RwInt32					bForTool_FlagChk			( RwUInt32 flag )const;
#endif

public:
	RwUInt32				m_dwEffSetID;			//파일네임을 딱히 정하지 않고.. 아이디로 일정한 규칙으로..
	RwChar					m_szTitle[EFF2_BASE_TITLE_MAX_NUM];

	RwUInt32				m_dwEffSetLife;
	E_LOOPOPT				m_eLoopOpt;
	RwUInt32				m_dwEffSetBitFlags;

	STMISSILEINFO			m_stMissileInfo;

	RwSphere				m_BSphere;
	RwBBox					m_BBox;

	StVarSizeInfo			m_stVarSizeInfo;		//가변크기.

	RwInt32					m_nFileOffset;
	RwInt32					m_nFileSize;

	RwInt32					m_nRefCnt;				//reference count
	RwInt32					m_nAcumCnt;
	RwUInt32				m_dwLastShootTime;
	
	LPEffectBaseVec			m_vecLPEffectBase;
	LPEffectExVec			m_vecLPEffectEx;
	BaseDependancyVec		m_vecBaseDependancy;

	RwInt32							m_nCurrentNumOfLoadingBase;		//. 0일 경우 로딩이 전부 끝난 Effset이므로. AGCDEFFSETRESOURCELOADSTATUS_LOADED상태가 된다.
	AGCDEFFSETRESOURCELOADSTATUS	m_enumLoadStatus;
	BOOL							m_bForceImmediate;
};
typedef AgcdEffSet EFFSET, *PEFFSET, *LPEFFSET;

//typedef std::map< RwUInt32 , LPEFFSET >		LPEffectSetMap;
typedef std::multimap< RwUInt32 , LPEFFSET >		LPEffectSetMap;
typedef LPEffectSetMap::iterator			LPEffectSetMapItr;
typedef LPEffectSetMap::const_iterator		LPEffectSetMapCItr;

typedef std::list<LPEFFSET>					LPEffectList;

class AGCDEFFSET_CBARG : public ApMemory<AGCDEFFSET_CBARG, 3000>
{
	EFFMEMORYLOG_SMV;

public:
	AGCDEFFSET_CBARG()
	{
		EFFMEMORYLOG_CON;

		m_pOwnerSet = NULL;
		m_pLoadBase = NULL;
	}

	~AGCDEFFSET_CBARG()
	{
		EFFMEMORYLOG_DES;
	}

	AgcdEffSet*		m_pOwnerSet;				//. AgcdEffBase* 를 갖는 AgcdEffSet*
	AgcdEffBase*	m_pLoadBase;				//. 로딩되어야할 Effect Base.

	AgcdEffBase::E_EFFBASETYPE	m_eBaseType;	//. Effect Base Type.
};

#endif