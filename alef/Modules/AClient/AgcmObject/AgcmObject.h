// AgcmObject.h: interface for the AgcmObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMOBJECT_H__CF99C609_205A_4581_959C_0D2D8A215430__INCLUDED_)
#define AFX_AGCMOBJECT_H__CF99C609_205A_4581_959C_0D2D8A215430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcModule.h"

#include "AgcdObject.h"

#include "ApmObject.h"
#include "ApdObject.h"

#include "AgcmResourceLoader.h"
#include "AgcmRender.h"
#include "AgcmLODManager.h"
#include "AgcmMap.h"

#include "AgcaAnimation.h"
#include "AgcuObject.h"

#include "AuPacket.h"

#include "AgcmObjectList.h"

#include "AgcmShadow.h"
#include "AgcmOcTree.h"
#include "ApmOcTree.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmObjectD" )
#else
#pragma comment ( lib , "AgcmObject" )
#endif
#endif

#define AGCMOBJECT_MAX_STR							256
#define AGCMOBJECT_MAX_PATH_CLUMP					128
#define AGCMOBJECT_HIERARCHY_MAX_KEYFRAME			100

#define	AGCMOBJECT_DATA_OBJECT						0
#define AGCMOBJECT_DATA_OBJECT_TEMPLATE				1

#define AGCMOBJECT_DEFAULT_ANIM_BLEND_DURATION		200

#define AGCMOBJECT_INI_NAME_DFF						"DFF"
#define AGCMOBJECT_INI_NAME_CATEGORY				"Category"
#define AGCMOBJECT_INI_NAME_RIDABLEMATERIALTYPE		"RidableMaterialType"
//#define AGCMOBJECT_INI_NAME_USE_AMBIENT			"UseAmbient"
//#define AGCMOBJECT_INI_NAME_USE_ALPHA			"UseAlpha"
//#define AGCMOBJECT_INI_NAME_NO_CAMERA_ALPHA		"NoCameraAlpha"
#define AGCMOBJECT_INI_NAME_OBJECT_TYPE				"ObjectType"
#define AGCMOBJECT_INI_NAME_LOD_LEVEL				"LOD_LEVEL"
#define AGCMOBJECT_INI_NAME_LOD_DISTANCE			"LOD_DISTANCE"
//#define AGCMOBJECT_INI_NAME_LOD_DISTANCE		"LODDistance"
#define AGCMOBJECT_INI_NAME_LOD_HAS_BILLBOARD_NUM	"LOD_BILLBOARD_NUM"
#define AGCMOBJECT_INI_NAME_LOD_BILLBOARD_INFO		"LOD_BILLBOARD_INFO"
#define AGCMOBJECT_INI_NAME_COLLISION_DFF			"COLLISION_DFF"
#define AGCMOBJECT_INI_NAME_PICK_DFF				"PICK_DFF"
#define AGCMOBJECT_INI_NAME_PRE_LIGHT				"PRE_LIGHT"
#define AGCMOBJECT_INI_NAME_ANIMATION				"ANIMATION"
#define AGCMOBJECT_INI_NAME_ANIM_SPEED				"ANIM_SPEED"
#define AGCMOBJECT_INI_NAME_BSPHERE					"BSPHERE"
#define AGCMOBJECT_INI_NAME_GROUP_COUNT				"GroupCount"

#define AGCMOBJECT_GET_ANIM_NAME					"OBJ_ANIM"

#define AGCMOBJECT_INI_NAME_OCTREEDATANUM			"OCTREE_DNUM"
#define AGCMOBJECT_INI_NAME_OCTREEDATA				"OCTREE_DATA"
#define AGCMOBJECT_INI_NAME_OCTREEDATA_MAXBBOX		"OCTREE_MAXBBOX"

//@{ 2006/05/03 burumal
#define AGCMOBJECT_INI_NAME_DNF_1					"DID_NOT_FINISH_KOREA"
#define AGCMOBJECT_INI_NAME_DNF_2					"DID_NOT_FINISH_CHINA"
#define AGCMOBJECT_INI_NAME_DNF_3					"DID_NOT_FINISH_WESTERN"
#define AGCMOBJECT_INI_NAME_DNF_4					"DID_NOT_FINISH_JAPAN"
//@}

typedef enum AgcmObjectCallbackPoint
{
	AGCMOBJECT_CB_ID_OBJECT_CREATE			= 0,
	AGCMOBJECT_CB_ID_OBJECT_TEMPLATE_CREATE	,
	AGCMOBJECT_CB_ID_INIT_OBJECT			,
	AGCMOBJECT_CB_ID_WORK_OBJECT			,
	AGCMOBJECT_CB_ID_BUFF_OBJECT			,
	AGCMOBJECT_CB_ID_LOAD_CLUMP				,
	AGCMOBJECT_CB_ID_PRE_REMOVE_DATA		,
	AGCMOBJECT_CB_ID_RIDABLEUPDATE			,
	AGCMOBJECT_CB_ID_POST_LOADMAP			,	// 오브젝트 로딩이 끝난후 불림..
	AGCMOBJECT_CB_ID_NUM
} AgcmObjectCallbackPoint;

typedef enum _eAgcmObjectPacketOperation {
	AGCMOBJECT_OPERATION_UPDATE					= 0,
} eAgcmObjectPacketOperation;

enum eAgcmObjectFlags
{
	E_AGCM_OBJECT_FLAGS_NONE		= 0x0000,
	E_AGCM_OBJECT_FLAGS_EXPORT		= 0x0001
};

class AgcmObject : public AgcModule  
{
protected:
	// RpWorld, Clump가 만들어지거나 Destroy될때 쓴다.
	RpWorld				*m_pWorld;

	CHAR				m_szClumpPath[AGCMOBJECT_MAX_PATH_CLUMP];
	CHAR				m_szAnimationPath[AGCMOBJECT_MAX_STR];

	// ApmObject Module, Data도 붙이고, Callback도 정의하고, Stream도 붙인다.
	ApmObject			*m_pcsApmObject;
	AgcmRender			*m_pcsAgcmRender;
	AgcmLODManager		*m_pcsAgcmLODManager;
	ApmMap				*m_pcsApmMap;
	AgcmMap				*m_pcsAgcmMap;
	AgcmResourceLoader	*m_pcsAgcmResourceLoader;
	AgcmShadow			*m_pcsAgcmShadow;
	AgcmOcTree			*m_pcsAgcmOcTree;
	ApmOcTree			*m_pcsApmOcTree;

	// Animation 관리
	UINT32				m_ulCurTick;
	INT32				m_lMaxAnimNum;	
//	AgcaAnimation		m_csAnimation;
	AgcaAnimation2		m_csAnimation2;

	// 오브젝트에서 쓰이는 각종 list관리!
	AgcmObjectList		m_csObjectList;	

	// ApmObject에 Data 붙일 때 나오는 Index
	INT16				m_nObjectAttachIndex;
	INT16				m_nObjectTemplateAttachIndex;

	BOOL				m_bRangeApplied	;
	FLOAT				m_fRangeStartX	;
	FLOAT				m_fRangeEndX	;
	FLOAT				m_fRangeStartZ	;
	FLOAT				m_fRangeEndZ	;

	BOOL				m_bUseTexDict			;
	CHAR				m_szTexDict			[AGCMOBJECT_MAX_PATH_CLUMP];
	RwTexDictionary		*m_pstTexDict			;
	
	CHAR				m_szTexturePath		[AGCMOBJECT_MAX_PATH_CLUMP];

	AuPacket			m_csPacket		;

	// enumerate용
	ApdObject			*m_CurObject	; 
	AgcdObject			*m_CurGroupNode	;

	BOOL				m_bAutoLoad				;

	BOOL				m_bSetupNormalObject	;
	BOOL				m_bSetupSystemObject	;

	INT32				m_lLoaderTemplateIndex			;
	INT32				m_lLoaderInitIndex				;

	RwTextureCallBackRead	m_fnDefaultTextureReadCB	;
	
	//. 2006. 05. 08. nonstopdj
	//. instead of crt new operation. using apmemory pool 
	struct	RidableOverlapping : public ApMemory<RidableOverlapping, 2000>
	{
		enum FLAG
		{
			NONE		= 0x00,
			RIDABLE		= 0x01,
			BLOCKING	= 0x02
		};

		UINT32		uFlag		;

		INT32		nObjectIndex;
		// RwSphere	stSphere	;
		RwBBox		stBBox		;
	};

	AuList< RidableOverlapping >	m_listRidableObjectlapping;

	UINT32		m_ulModuleFlags;

	BOOL				m_bShowPickingInfo;

public:
	BOOL		IsShowPickingInfo() { return m_bShowPickingInfo; };
	// Animation
//	static RtAnimInterpolator	*OnAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData);
//	static RtAnimInterpolator	*OnAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData);

	AgcaAnimation2		*GetAgcaAnimation2()	{return &m_csAnimation2;}

	VOID				SetupSystemObject(BOOL bSet)			{m_bSetupSystemObject = bSet;}

	BOOL				SetAnimation(AgcdObjectGroupData *pcsData, RtAnimAnimation *pstAnim);

	inline VOID			AddFlags(UINT32 ulFlags)	{m_ulModuleFlags	|= ulFlags;}


/*	AgcaAnimation		*GetAnimAdmin()	{return &m_csAnimation;}
	AgcdAnimation		*GetAnimation(INT32 lTID, INT32 lIndex, INT32 lType, BOOL bAdd = FALSE);
	AgcdAnimData		*GetAnimData(CHAR *szName, CHAR *szPath = NULL);
	CHAR				*GetClumpPath() {return m_szClumpPath;}*/

/*	BOOL				StartAnimation(INT32 lOID, AgcdAnimation *pstNextAnim, BOOL bEndThenChangeCB = FALSE);
	BOOL				StartAnimation(ApdObject *pcsApdObject, AgcdAnimation *pstNextAnim, BOOL bEndThenChangeCB = FALSE);
	BOOL				StartAnimation(AgcdObject *pstAgcdObject, AgcdAnimation *pstNextAnim, BOOL bEndThenChangeCB = FALSE);

	BOOL				UpdateAnimation(AgcdObject *pstAgcdObject, UINT32 ulDeltaTime);

	BOOL				DropAnimation(AgcdObject *pstAgcdObject, BOOL bInitSkin = TRUE);*/
	
/*	static RtAnimInterpolator	*OnAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData);
	static RtAnimInterpolator	*OnAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData);

	BOOL				SetAnimation(AgcdObjectGroupData *pcsData, AgcdAnimation *pstAnim, eObjectAnimationType eType);
	BOOL				StartAnimation(AgcdObject *pstAgcdObject, eObjectAnimationType eAnimType, BOOL bEndThenChange = TRUE);

	BOOL				StopAnimation(AgcdObjectGroupData *pcsData);
	BOOL				DropAnimation(AgcdObjectGroupData *pcsData);

	PVOID				GetAnimationAttachedData(AgcdAnimation *pstAnim, CHAR *szKey);*/

//	BOOL				UpdateAnimation(AgcdObjectGroupData *pcsData, UINT32 ulDeltaTime);

	BOOL				CopyObjectTemplateGroupData(INT32 lTID, AgcdObjectTemplateGroupData *pcsDest, AgcdObjectTemplateGroupData *pcsSrc);
	BOOL				CopyObjectTemplateGroup(INT32 lTID, AgcdObjectTemplateGroup *pcsDest, AgcdObjectTemplateGroup *pcsSrc);
	BOOL				RemoveObjectTemplateGroup(AgcdObjectTemplateGroup *pcsGroup);
	BOOL				RemoveObjectTemplateGroupData(AgcdObjectTemplateGroupData *pcsData);

	// 초기화~
	BOOL		InitObject(ApdObject *pcsApdObject);
	// 아토믹 생성!
	RpAtomic	*LoadAtomic(CHAR *szDffName);
	// 모든 템플릿의 클럼프 생성!
	BOOL		LoadAllTemplateData(BOOL bUseTexDict = TRUE, INT32 *plIndex = NULL, CHAR *szErrorMessage = NULL);
	BOOL		LoadTemplateData(AgcdObjectTemplate * pAgcdObjectTemplate, BOOL bUseTexDict = TRUE, CHAR *szErrorMessage = NULL);

protected:
	
	BOOL		CheckingRange( ApdObject * pObject );

public:
	static	AgcmObject	*	m_pThis;
	BOOL	SetRange		( FLOAT x1 , FLOAT z1 , FLOAT x2 , FLOAT z2	);
	BOOL	UseRange		( BOOL bUse									);
	BOOL	IsRangeApplied	(){ return m_bRangeApplied; }

	BOOL	SetupObjectClump(ApdObject *pstApdObject);
	BOOL	SetupObjectClump(ApdObject *pstApdObject, AgcdObject *pstAgcdObject);
	BOOL	SetupObjectClump1(ApdObject *pstApdObject, AgcdObject *pstAgcdObject);
	BOOL	SetupObjectClump2(ApdObject *pstApdObject, AgcdObject *pstAgcdObject);
	BOOL	SetupObjectClump_TransformOnly(ApdObject *pstApdObject);

	// 초기화시 불러줘야 할 것덜...
	BOOL	SetRpWorld(RpWorld *pWorld);
	BOOL	SetClumpPath(CHAR *szPath);
	VOID	SetAnimationPath(CHAR *szAnim);
	void	SetTexDictFile(CHAR *szTexDict);
	VOID	SetMaxAnimation(INT32 lMaxAnimNum);

	VOID	SetTexturePath(CHAR *szTexturePath);

	// Object Stream Read / Write Callback function
	static BOOL	ObjectStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	ObjectStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	// Object Template Stream Read / Write Callback function
	static BOOL	ObjectTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	ObjectTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	// Object Template Group Stream Read / Write function
	BOOL		ObjectTemplateGroupStreamWrite(ApModuleStream *pcsStream, AgcdObjectTemplate *pcsAgcdObjectTemplate);
	INT32		ObjectTemplateGroupStreamRead(ApModuleStream *pcsStream, INT32 lTID, AgcdObjectTemplateGroup *pstGroup);
	AgcdObjectTemplateGroupData	*GetObjectTemplateGroupData(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex, BOOL bAdd = TRUE);
	AgcdObjectGroupData			*GetObjectGroupData(AgcdObjectGroup *pstGroupData, INT32 lIndex, BOOL bAdd = TRUE);

	// Object Template Data 생성자, 파괴자
	static BOOL	OnObjectTemplateDestroy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	OnObjectTemplateCreate(PVOID pData, PVOID pClass, PVOID pCustData);

	// Object Data 생성자, 파괴자
	static BOOL	OnObjectDestroy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	OnObjectCreate(PVOID pData, PVOID pClass, PVOID pCustData);

	// Clump를 Load한다.
	RpClump *	LoadClump(CHAR *szDFF, CHAR *szTexDict = NULL);
	BOOL		SaveClump(RpClump *pstClump, CHAR *szDFF);

	// 템플릿의 클럼프를 모두 읽는다.(Bob, 070603)
	//BOOL		LoadAllTemplateClump(BOOL bUseTexDict = FALSE);

	// ApdObject로부터 AgcdObject를 가져온다.
	ApdObject *				GetObject(AgcdObject *pstAgcdObject);
	AgcdObject *			GetObjectData(ApdObject *pstObject);

	// ApdObjectTemplate으로부터 AgcdObjectTemplate을 가져온다.
	AgcdObjectTemplate *	GetTemplateData(ApdObjectTemplate *pstObjectTemplate);

	// Object Template이 생길때 Callback받을 함수 등록한다.
	BOOL	AddObjectTemplateCreateCallBack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	AddObjectCreateCallBack(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackInitObject		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackWorkObject		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackBuffObject		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackLoadClump		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackPreRemoveData	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRidableUpdate	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackPostLoadMap		(ApModuleDefaultCallBack pfCallback, PVOID pClass) { return SetCallback(AGCMOBJECT_CB_ID_POST_LOADMAP , pfCallback, pClass); }
	
	VOID	ForceEnumCallback(AgcmObjectCallbackPoint ePoint, ApdObject *pcsApdObject);

	// Texture Dictionary 관련 함수
	BOOL	DumpTexDict();

	// Material Color를 바꾸는 함수
	BOOL	UpdateAmbient		( AgcdObjectTemplate *pstAgcdTemplate, BOOL bAmbient);
	BOOL	UpdateAlpha			( AgcdObjectTemplate *pstAgcdTemplate, BOOL bAlpha	);
	BOOL	UpdateObjectType	( AgcdObjectTemplate *pstAgcdTemplate	);
	//BOOL	ChangeAmbient(RwRGBA *pstRGBA);

	// 마고자 테스트 함수..
	// 템플릿의 아토믹을 다시 로딩한다.
	BOOL	ReloadObjectClump( char * pTemplateFileName , ProgressCallback pfCallback = NULL , void * pData = NULL );

	// 해제 함수.
	BOOL	ReleaseObjectData(ApdObject *pstAgdObject, AgcdObject *pstAgcdObject);
	BOOL	ReleaseObjectTemplateData(AgcdObjectTemplate *pstAgcdObjectTemplate);

	// Callback 함수
	static BOOL CBInitObject		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveObject		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateAnimation	(PVOID pData, PVOID pClass, PVOID pCustData);

	static RpAtomic *	CBInitTemplateAtomic	( RpAtomic	*	pstAtomic	, PVOID pvData );
	static RpAtomic *	CBGetCollisionAtomic	( RpClump	*	pstClump	, PVOID pvData );
	static RpAtomic *	CBGetCollisionAtomicFromAtomic( RpAtomic	*	pstAtomic	, PVOID pvData );

	static BOOL			CBOnAddObject			( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBOnGetHeight			(PVOID pData, PVOID pClass, PVOID pCustData);

	// 쓰레드에서 불리면 안된다.
	ApCriticalSection			m_MutexlistSectorObjectIndexUpdate	;
	AuList< ApWorldSector * >	m_listSectorObjectIndexUpdate		;
	void	SectorObjectIndexUpdate( ApWorldSector * pSector );

	// Grouping Functions
	BOOL					GroupAdd		( AgcdObject* obj, AuPOS& pos, AuPOS& scale,FLOAT&	xrot,FLOAT& yrot, BOOL bSetupClump = TRUE);
	AgcdObject*				GetEnumValue	( AgcdObject *pcsObject );		// while ( data = GetEnumValue(obj)) 이런 형식으로 사용가능
	INT32					GetGroupCount	( AgcdObject *pcsObject );
	BOOL					GroupDelete		( AgcdObject *pcsObject );
	INT32					ClumpDelete		( RpClump *pClump , BOOL bForce = FALSE );
	AgcdObject*				GetObjectData	( RpClump * pClump );
	inline BOOL				IsGroupClump	( RpClump * pClump )
	{
		AgcdObject * pAgcdObject	= GetObjectData( pClump );
		if( pAgcdObject && pAgcdObject->m_bGroupChild )	return TRUE;
		else return FALSE;
	}
	inline AgcmObjectList	*GetObjectList()	{return &m_csObjectList;}
	inline CHAR				*GetAnimationPath()	{return m_szAnimationPath;}

		// return : OID..
		// 그룹 아이디일경우 -1 이 리턴..
		// 에러의경우 -2..
	AgcdObject*				FindGroupStart( FLOAT x1,FLOAT z1,FLOAT x2,FLOAT z2,UINT32	tID);
	AgcdObject*				FindGroupStart( AuPOS& pos,UINT32	tID);
	
	BOOL OnInit();
	BOOL OnAddModule();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	AgcmObject();
	virtual ~AgcmObject();

	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	VOID RefreshObjects(INT32 lTID);

	VOID SetSetupObject(BOOL bNormal, BOOL bSystem)	{ m_bSetupNormalObject = bNormal; m_bSetupSystemObject = bSystem;	}
	VOID SetAutoLoad(BOOL bAutoLoad)				{ m_bAutoLoad = bAutoLoad;	}
	
	BOOL ObjectExport	( int x1 , int z1 , int x2 , int z2 , char * pDestinationDirectory = NULL );
	BOOL ServerExport	( INT32 nDivision , char * pDestinationDirectory = NULL );
	static BOOL CBEventObjectOnly(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBLoadMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUnLoadMap(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBLoaderTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBLoaderInit(PVOID pData, PVOID pClass, PVOID pCustData);

	AgcmRender	*GetAgcmRenderModule()	{return m_pcsAgcmRender;}
	ApmObject	*GetApmObjectModule()	{return m_pcsApmObject;}

	// fOffset은 0.0f에서 2.0f 사이의 범위다.
	BOOL SetPreLightForAllObject(FLOAT fOffset);

	// OcTreeID Set용
	static	BOOL CBOcTreeIDSet(PVOID pData, PVOID pClass, PVOID pCustData);
	void	SetAllOcTreeIDs();

	void	ShowPickingInfo();
	void	HidePickingInfo();

	FLOAT	GetBoundingSphereRange(ApdObject *pcsObject);

	bool	IsValidLOD( ApdObjectTemplate* pApdObjectTemplate, AgcdObjectTemplate* pAgcdObjectTemplate );

	
	// 오브젝트 제거 과정 메인스레드에서..
	ApCriticalSection			m_MutexDeleteObjectQueueAccess;
	struct	DeleteObjectQueue
	{
		BOOL	bProcessed;
		INT32	nIndexX;
		INT32	nIndexZ;
		vector< INT32 >	vecObject;

		DeleteObjectQueue(): nIndexX( 0 ) , nIndexZ( 0 ) , bProcessed ( FALSE ) {}

		BOOL	IsSameSector( ApWorldSector * pSector )
		{
			if( pSector->GetIndexX() == nIndexX &&
				pSector->GetIndexZ() == nIndexZ &&
				!bProcessed						)
				return TRUE;
			else
				return FALSE;
		}

		void	SetSector( ApWorldSector * pSector )
		{
			nIndexX = pSector->GetIndexX();
			nIndexZ = pSector->GetIndexZ();

			ApWorldSector::Dimension	* pDimension;
			pDimension = pSector->GetDimension( 0 );

			#ifdef _DEBUG
			// 마고자 호출 요망.
			//@{ 2006/05/03 burumal 디버그 모드 실행이 되지 않으므로 disable 시켜둠
			//DebugBreak();
			//@}
			#endif

			if( pDimension )
			{
				ApWorldSector::IdPos*	pObject = pDimension->pObjects;
				ApWorldSector::IdPos*	pObjectNext;

				while (pObject)
				{
					pObjectNext = pObject->pNext;
					vecObject.push_back( pObject->id );
					pObject = pObjectNext;
				}
			}
		}

		void	CheckProcessd	() { bProcessed = TRUE; }
		BOOL	IsProcessed		() { return bProcessed; }
	};

	vector< DeleteObjectQueue >	m_vecDeleteObjectQueue;
	INT32	AddDeleteObjectQueue( ApWorldSector * pSector );
	DeleteObjectQueue	*	GetDeleteObjectQueue();
	BOOL	IsDeleteObjectQueueAvailable(){ return GetDeleteObjectQueue() ? TRUE : FALSE; }
	BOOL	WaitForQueueFlush();
	// void	RemoveDeleteObjectQueue( ApWorldSector * pSector );
	void	ObjectDeleteIdleProcess();
	void	SectorRidableObjectIndexUpdate();

public :
	BOOL	IsInValidOctreeData( void* pObjectTemplate );
	void	ReCalulateOctreeData( CHAR* pOctreeType, void* pObjectTemplate );
};

#endif // !defined(AFX_AGCMOBJECT_H__CF99C609_205A_4581_959C_0D2D8A215430__INCLUDED_)
