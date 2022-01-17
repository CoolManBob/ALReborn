// AgcdEffBase.h: interface for the AgcdEffBase class.
//

// * AgcdEffBase
//		- m_eEffBaseType	: 종류
//		- m_dwDelay			: 이펙트 시작후 대기상태로 있을 시간
//		- m_dwLife 			: 발동후 유지될 시간
//		- m_eLoopOpt		: 루프 옵션( 반복 유무 )
//		- m_dwBitFlags 		: 각종 옵션플래그들
//		- m_stVarSizeInfo	: 가지고 있는 에니메이션 갯수
//		- m_vecLPEffAni	: 에니메이션 컨테이너

// * AgcdEffRenderBase	: public AgcdEffBase
//		- m_eBlendType	: 블렌드 타입
//		- m_cEffTexInfo	: 텍스쳐 정보
//		- m_v3dInitPos	: 초기 위치
//		- m_stInitAngle	: 초기 회전각
//		- m_matTrans	: 초기 위치에 해당하는 변환 매트릭스
//		- m_matRot		: 초기 회전각에 해당하는 회전 변환 매트릭스

///////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFBASE_H__18546D52_BEA5_41BB_9BD8_891EB7616913__INCLUDED_)
#define AFX_AGCDEFFBASE_H__18546D52_BEA5_41BB_9BD8_891EB7616913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffAnimUtil.h"
#include "AgcdEffAnim.h"
#include "AgcdEffTex.h"

#include <vector>
using namespace std;


///////////////////////////////////////////////////////////////////////////////
// FLAG
//{
// effBase
#define FLAG_EFFBASE_BILLBOARD					0x00000001
#define FLAG_EFFBASE_BILLBOARDY					0x00000002
#define FLAG_EFFBASE_REPEAT						0x00000004

#define FLAG_EFFBASE_BASEDEPENDANCY				0x00000008
#define FLAG_EFFBASE_BASEDPND_PARENT			0x00000010
#define FLAG_EFFBASE_BASEDPND_CHILD				0x00000020
#define	FLAG_EFFBASE_MISSILE					0x00000040
#define FLAG_EFFBASE_DEPEND_ONLYPOS				0x00000080
#define FLAG_EFFBASE_BASEDPND_TOOBJ				0x00000100
#define	FLAG_EFFBASE_BASEDPND_TOMISSILEOBJ		0x00000200	//미사일 오브젝트에 디펜던시가 걸린것들은 초기에 x축으로 -90만큼 돌린다.

//{
// effBaseTail 
#define FLAG_EFFBASETAIL_Y45					0x00010000	//Y축을 기준으로 회전한 방향으로..
#define FLAG_EFFBASETAIL_Y90					0x00020000
#define FLAG_EFFBASETAIL_Y135					0x00040000
//}
		
//{
// effBaseObj
#define FLAG_EFFBASEOBJ_DUMMY					0x00010000
#define FLAG_EFFBASEOBJ_CHILDOFMISSILE			0x00020000
//}		
		
//{		
// effBaseSound
#define FLAG_EFFBASESOUND_STOPATEFFECTEND		0x00010000
#define FLAG_EFFBASESOUND_3DTOSAMEPLE			0x00020000
#define FLAG_EFFBASESOUND_NOFRUSTUMCHK			0x00040000	//프러스텀 체크를 안함. ambience sound->위치가 없다.
//}	
	
//{	
// effBaseParticleSystem
#define FLAG_EFFBASEPSYS_RANDCOLR				0x00010000
#define FLAG_EFFBASEPSYS_INTERPOLATION			0x00020000
#define FLAG_EFFBASEPSYS_CHILDDEPENDANCY		0x00040000
#define FLAG_EFFBASEPSYS_PARTICLEBILLBOARD		0x00080000
#define FLAG_EFFBASEPSYS_PARTICLEBILLBOARDY		0x00100000
#define FLAG_EFFBASEPSYS_CIRCLEEMITER			0x00200000	
#define	FLAG_EFFBASEPSYS_FILLCIRCLE				0x00400000	
#define FLAG_EFFBASEPSYS_CAPACITYLIMIT			0x00800000	//입자 갯수가 용량을 초과하면 입자 생성을 하지 안는다.
#define FLAG_EFFBASEPSYS_TARGETDIR				0x01000000	// 몬스터가 있는 방향으로 파티클 위치 계산을 한다
//}	
	
//{
// effBaseParticleSys_SBH 
#define FLAG_EFFBASEPSYSSBH_RANDCOLR			0x00010000
#define FLAG_EFFBASEPSYSSBH_INTERPOLATION		0x00020000
#define FLAG_EFFBASEPSYSSBH_CHILDDEPENDANCY		0x00040000
#define FLAG_EFFBASEPSYSSBH_INVS				0x00080000
#define FLAG_EFFBASEPSYSSBH_REGULARANGLE		0x00100000
#define FLAG_EFFBASEPSYSSBH_CAPACITYLIMIT		0x00200000	//입자 갯수가 용량을 초과하면 입자 생성을 하지 안는다.
#define	FLAG_EFFBASEPSYSSBH_OLDTYPE				0x00400000	//이전처럼 카메라의 영향으로 틀어지는 현상.
#define FLAG_EFFBASEPSYSSBH_CIRCLEEMITER		0x00800000	
#define	FLAG_EFFBASEPSYSSBH_FILLCIRCLE			0x01000000	
#define FLAG_EFFBASEPSYSSBH_MINANGLE			0x02000000	//생성각도가 1보다 작거나 같으면 예외 처리 한다.. 왜? 원래 그랬으니까.
//}

//{
// effBasePostFX
#define	FLAG_EFFBASEPOSTFX_CENTERLOCK			0x00010000	//저장된 center 사용 플래그오프시 이펙트위치를 사용

//{
// effBaseParticleSystem_Emiter
#define FLAG_EFFBASEPSYS_EMITER_SHOOTRAND		0x00000001	//포의 방향백터 옵션
#define FLAG_EFFBASEPSYS_EMITER_SHOOTCONE		0x00000002	
#define FLAG_EFFBASEPSYS_EMITER_YAWONEDIR		0x00000004	//각속도와 제한 각도의 계산 옵션
#define FLAG_EFFBASEPSYS_EMITER_YAWBIDIR		0x00000008
#define FLAG_EFFBASEPSYS_EMITER_PITCHONEDIR		0x00000010
#define FLAG_EFFBASEPSYS_EMITER_PITCHBIDIR		0x00000020
#define FLAG_EFFBASEPSYS_EMITER_NODEPND			0x00000040	//에미터 프레임의 위치만, 방향은 회전속도로만
#define FLAG_EFFBASEPSYS_EMITER_SHOOTNOMOVE		0x00000080	//파티클 안움직임.
//}
//}


//----------------------- AgcdEffBase -----------------------
class AgcdEffBase
{
public:
	struct StVarSizeInfo
	{
		StVarSizeInfo() : m_nNumOfAnim( 0 )		{		};

		RwInt32		m_nNumOfAnim;
	};

	enum E_EFFBASETYPE
	{
		E_EFFBASE_BOARD					= 0,
		E_EFFBASE_PSYS,
		E_EFFBASE_PSYS_SIMPLEBLACKHOLE,
		E_EFFBASE_TAIL,
		E_EFFBASE_OBJECT,
		E_EFFBASE_LIGHT,
		E_EFFBASE_SOUND,
		E_EFFBASE_MOVINGFRAME,
		E_EFFBASE_TERRAINBOARD,
		E_EFFBASE_POSTFX,
		E_EFFBASE_CAMERA,
		E_EFFBASE_NUM,
	};

#ifdef USE_MFC
	static const RwChar* EffBaseTypeString( E_EFFBASETYPE eType )
	{
		static const RwChar* s_szEffectType[E_EFFBASE_NUM] = 
		{
			"E_EFFBASE_BOARD",
			"E_EFFBASE_PSYS",
			"E_EFFBASE_PSYS_SIMPLEBLACKHOLE",
			"E_EFFBASE_TAIL",
			"E_EFFBASE_OBJECT",
			"E_EFFBASE_LIGHT",
			"E_EFFBASE_SOUND",
			"E_EFFBASE_MOVINGFRAME",
			"E_EFFBASE_TERRAINBOARD",
			"E_EFFBASE_POSTFX",
		};

		return s_szEffectType[eType];
	}
#endif

	typedef std::vector<LPEFFANIM>			LPEffAniVec;
	typedef LPEffAniVec::iterator			LPEffAniVecItr;
	typedef LPEffAniVec::const_iterator		LPEffAniVecCItr;

public:
	explicit AgcdEffBase( E_EFFBASETYPE eEffBaseType );
	virtual ~AgcdEffBase( VOID );

	virtual const RwMatrix*	bGetCPtrMat_Trans	( VOID ) const		{ return NULL; };
	virtual const RwMatrix*	bGetCPtrMat_Rot		( VOID ) const		{ return NULL; };
	virtual bool			bIsRenderBase		( VOID ) const		{ return false; };

	virtual RwInt32			bToFile				( FILE* fp )		{return 1;};
	virtual RwInt32			bFromFile			( FILE* fp )		{return 1;};

	E_EFFBASETYPE			bGetBaseType		( VOID ) const				{ return m_eEffBaseType; };
	const RwChar*			bGetPtrTitle		( VOID ) const				{ return m_szBaseTitle; };
	RwUInt32				bGetDelay			( VOID ) const				{ return m_dwDelay; };
	RwUInt32				bGetLife			( VOID ) const				{ return m_dwLife; };
	E_LOOPOPT				bGetLoopOpt			( VOID ) const				{ return m_eLoopOpt; };
	RwUInt32				bGetFlag			( VOID ) const				{ return m_dwBitFlags; };
	const StVarSizeInfo*	bGetPtrVarSizeInfo	( VOID ) const				{ return &m_stVarSizeInfo; };
	RwInt32					bGetEffAnimSize		( VOID ) const				{ return (RwInt32)m_vecLPEffAni.size(); };
	LPEffAniVec&			bGetRefAnimList		( VOID )					{ return m_vecLPEffAni; };
	const LPEFFANIM			bGetPtrEffAnim		( RwInt32 nIndex ) const	{ return m_vecLPEffAni[nIndex]; };
	LPEFFANIM				bGetPtrEffAnim		( RwInt32 nIndex )			{ return m_vecLPEffAni[nIndex]; };


	//set
	void					bSetVarSizeInfo		( const StVarSizeInfo& rStVarSizeInfo);
	RwInt32					bSetTitle			( const RwChar* szTitle );
	void					bSetDelay			( RwUInt32 dwDelay )			{ m_dwDelay	= dwDelay; };
	void					bSetLife			( RwUInt32 dwLife )				{ m_dwLife = dwLife; };
	void					bSetLoopOpt			( E_LOOPOPT eLoopOpt )			{ m_eLoopOpt = eLoopOpt; };
	void					bSetFlag			( RwUInt32 dwFlag )				{ m_dwBitFlags = dwFlag; };
	void					bFlagOn				( RwUInt32 dwFlag )				{ DEF_FLAG_ON( m_dwBitFlags, dwFlag ); };
	void					bFlagOff			( RwUInt32 dwFlag )				{ DEF_FLAG_OFF( m_dwBitFlags, dwFlag ); };
	RwUInt32				bFlagChk			( RwUInt32 dwFlag )const		{ return DEF_FLAG_CHK( m_dwBitFlags, dwFlag ); };

	//insert/delete
	RwInt32					bInsEffAnim			( LPSTCREATEPARAM_EFFANIM lpCreateParam_EffAnim );
	RwInt32					bDelEffAnim			( RwInt32 nIndex );

	RwInt32					bInsEffAnimVal		( RwInt32 nlpEffAnimIndex, RwUInt32 dwTime, void* pIns );
	RwInt32					bDelEffAnimVal		( RwInt32 nlpEffAnimIndex, RwUInt32 dwTime );

	//edit animvalue
	INT32					bEditEffAnim		( RwInt32 nlpEffAnimIndex, RwInt32 nIndexAtEffAnim, RwUInt32 dwTime, void* pEdit );
	void					vEffAnimClear		( VOID );

protected:
	RwInt32					tToFile				( FILE* fp );
	RwInt32					tFromFile			( FILE* fp );
	RwInt32					tToFileVariableData	( FILE* fp );
	RwInt32					tFromFileVariableData(FILE* fp );

#ifdef USE_MFC
public:
	INT32		bForTool_InsAnim( LPSTCREATEPARAM_EFFANIM lpCreateParam_EffAnim );	//effect tool 에서 STCREATEPARAM_EFFANIM에 해당 하는 dialogue
	INT32		bForTool_FindAnimIndex(LPEFFANIM pAnim);
	INT32		bForTool_DelAnim( RwInt32 nIndex );
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

protected:
	AgcdEffBase() : m_eEffBaseType(E_EFFBASE_BOARD) {};
	AgcdEffBase(const AgcdEffBase& cpy) : m_eEffBaseType(E_EFFBASE_BOARD)	{ cpy; };
	AgcdEffBase& operator = (const AgcdEffBase& cpy) { cpy; return *this; };

public:
	const E_EFFBASETYPE	m_eEffBaseType;
	RwChar				m_szBaseTitle[EFF2_BASE_TITLE_MAX_NUM];

	RwUInt32			m_dwDelay;
	RwUInt32			m_dwLife;
	E_LOOPOPT			m_eLoopOpt;
	RwUInt32			m_dwBitFlags;

	StVarSizeInfo		m_stVarSizeInfo;		//가변크기.

	LPEffAniVec			m_vecLPEffAni;

	BOOL				m_bForceImmediate;
};
typedef AgcdEffBase EFFBASE, *PEFFBASE, *LPEFFBASE;

extern const char* EFFBASETYPE_NAME[AgcdEffBase::E_EFFBASE_NUM];

//----------------------- AgcdEffRenderBase -----------------------
class AgcdEffRenderBase	: public AgcdEffBase
{
public:
	enum E_EFFBLENDTYPE
	{
		EFFBLEND_NONE			= 0,	//AddClumpToWorld

		EFFBLEND_ADD_TEXSTAGE,			//AddAlphaBModeClumpToWorld

		EFFBLEND_ADD_ONE_ONE,
		EFFBLEND_ADD_SRCA_ONE,
		EFFBLEND_ADD_SRCA_INVSRCA,
		EFFBLEND_ADD_SRCC_INVSRCC,
		
		EFFBLEND_REVSUB_ONE_ONE,
		EFFBLEND_REVSUB_SRCA_ONE,
		EFFBLEND_REVSUB_SRCA_INVSRCA,
		EFFBLEND_REVSUB_SRCC_INVSRCC,
		
		EFFBLEND_SUB_ONE_ONE,
		EFFBLEND_SUB_SRCA_ONE,
		EFFBLEND_SUB_SRCA_INVSRCA,
		EFFBLEND_SUB_SRCC_INVSRCC,

		EFFBLEND_NUM,
	};

public:
	explicit AgcdEffRenderBase( AgcdEffBase::E_EFFBASETYPE eEffBaseType, E_EFFBLENDTYPE eBlendType=EFFBLEND_ADD_ONE_ONE );
	virtual ~AgcdEffRenderBase(){};

	virtual bool		bIsRenderBase( VOID )const { return true; };
	
	E_EFFBLENDTYPE		bGetBlendType		( VOID ) const	{ return m_eBlendType; };
	RwInt32				bGetTexIndex		( VOID ) const	{ return m_cEffTexInfo.bGetIndex(); };
	const RwTexture*	bGetPtrRwTex		( VOID ) const	{ return m_cEffTexInfo.bGetPtrTex(); };
	const STANGLE&		bGetInitAngle		( VOID ) const	{ return m_stInitAngle; };
	const RwV3d&		bGetInitPos			( VOID ) const	{ return m_v3dInitPos; };
	const RwMatrix*		bGetCPtrMat_Trans	( VOID ) const	{ return &m_matTrans; };
	const RwMatrix*		bGetCPtrMat_Rot		( VOID ) const	{ return &m_matRot; };
	RwTexture*			bGetPtrRwTex		( VOID )		{ return m_cEffTexInfo.bGetPtrTex(); };

	//setup
	void				bSetBlendType		( E_EFFBLENDTYPE eBlendType )	{ m_eBlendType = eBlendType; };
	void				bSetTexIndex		( RwInt32 nIndex )				{ m_cEffTexInfo.bSetIndex( nIndex ); };
	RwInt32				bSetPtrEffTex		( LPEFFTEX lpEffTex )			{ return m_cEffTexInfo.bSetPtrEffTex( lpEffTex ); };
	void				bSetInitPos			( const RwV3d& v3Pos );
	void				bSetInitAngle		( const STANGLE& stAngle );

	void				bSet_Renderstate	( VOID );
	void				bRestore_Renderstate( VOID );
	
protected:
	RwInt32				tToFile				( FILE* fp );
	RwInt32				tFromFile			( FILE* fp );

#ifdef USE_MFC
public:
	virtual INT32	bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

protected:
	AgcdEffRenderBase() {};
	AgcdEffRenderBase(const AgcdEffRenderBase& cpy)	: AgcdEffBase(cpy) { cpy; };
	AgcdEffRenderBase& operator = (const AgcdEffRenderBase& cpy) { cpy; return *this; };

public:
	E_EFFBLENDTYPE		m_eBlendType;
	AgcdEffTexInfo		m_cEffTexInfo;
	RwV3d				m_v3dInitPos;
	STANGLE				m_stInitAngle;

	RwMatrix			m_matTrans;
	RwMatrix			m_matRot;
};

//----------------------- AgcuEffBaseCreater -----------------------
class AgcuEffBaseCreater
{
	typedef AgcdEffBase* (*fptrCreate)( VOID );

public:
	static AgcdEffBase*	bCreate		( AgcdEffBase::E_EFFBASETYPE eBaseType );
	static void			bDestroy	( AgcdEffBase*& prEffBase );

	static AgcdEffBase*	vCreate_Board					( VOID );
	static AgcdEffBase*	vCreate_PSys					( VOID );
	static AgcdEffBase*	vCreate_PSys_SimpleBlackHole	( VOID );
	static AgcdEffBase*	vCreate_Tail					( VOID );
	static AgcdEffBase*	vCreate_Object					( VOID );
	static AgcdEffBase*	vCreate_Light					( VOID );
	static AgcdEffBase*	vCreate_Sound					( VOID );
	static AgcdEffBase*	vCreate_MFrm					( VOID );
	static AgcdEffBase*	vCreate_TerrainBoard			( VOID );
	static AgcdEffBase*	vCreate_PostFX					( VOID );
	static AgcdEffBase* vCreate_Camera					( VOID );

private:
	static fptrCreate	m_fptrCreate[AgcdEffBase::E_EFFBASE_NUM];
};

#endif