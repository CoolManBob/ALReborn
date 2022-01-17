#ifndef	_AGCDOBJECT_H_
#define _AGCDOBJECT_H_

#include "ApBase.h"
#include "rwcore.h"
#include "rpworld.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtanim.h"

#include "ApdObject.h"
#include "acdefine.h"
#include "AgcaAnimation.h"
#include "AgcdRenderType.h"

#include "AgcmOcTree.h"

#define AGCDOBJECT_ANIM_NAME_LENGTH				128
#define	AGCDOBJECT_DFF_NAME_LENGTH				64
#define	AGCDOBJECT_CATEGORY_LENGTH				64
#define AGCDOBJECT_MAX_ALLOC_ANIM_DATA			1
#define AGCDOBJECT_ANIMATION_ATTACHED_DATA_KEY	"AGCDOBJECT"

typedef enum eObjectAnimationType
{
	E_OBJECT_ANIM_TYPE_NONE		= -1,
	E_OBJECT_ANIM_TYPE_NORMAL	= 0,
	E_OBJECT_ANIM_TYPE_EVENT,
	E_OBJECT_ANIM_TYPE_NUM,
};

typedef enum eObjectTemplateGroupStreamReadResult
{
	E_OTGSR_RESULT_ERROR = 0,
	E_OTGSR_RESULT_PASS,
	E_OTGSR_RESULT_READ
};

typedef enum
{
	AGCDOBJECT_STATUS_INIT			= 0,
	AGCDOBJECT_STATUS_LOAD_TEMPLATE	= 0x01,
	AGCDOBJECT_STATUS_REMOVED		= 0x02,
} AgcdObjectStatus;

typedef struct tag_GroupChildInfo
{
	AuPOS				m_stPosition	;		// Base Position
	AuPOS				m_stScale		;		// Scale Vector
	FLOAT				m_fDegreeX		;		// Rotation Degree
	FLOAT				m_fDegreeY		;		// Rotation Degree
}GroupChildInfo;

/*typedef struct AgcdObjectAnimationAttachedData
{
	stAgcdAnimationFlag	m_stAnimFlag;
} AgcdObjectAnimationAttachedData;*/

class AgcdObjectTemplateGroupData
{
public:
	INT32				m_lIndex;										// 내부적으로 쓰이므로 외부에서 설정 미필요!

	CHAR				*m_pszDFFName;
//	CHAR				*m_pszAnimName;

//	CHAR				m_szDFFName[AGCDOBJECT_DFF_NAME_LENGTH];
//	CHAR *				m_aszAnimName[E_OBJECT_ANIM_TYPE_NUM];

	RwSphere			m_stBSphere;

	RpClump				*m_pstClump;
//	AgcdAnimation		*m_pastAnim[E_OBJECT_ANIM_TYPE_NUM];
	AgcdAnimation2		*m_pcsAnimation;

	FLOAT				m_fAnimSpeed;
//	FLOAT				m_afAnimSpeed[E_OBJECT_ANIM_TYPE_NUM];

	AgcdClumpRenderType	m_csClumpRenderType;

	AgcdObjectTemplateGroupData()
	{
//		m_szDFFName[0]		= NULL;
		m_pszDFFName		= NULL;
//		m_pszAnimName		= NULL;
		m_pstClump			= NULL;
		m_fAnimSpeed		= 1.0f;

/*		for(INT32 lCount = 0; lCount < E_OBJECT_ANIM_TYPE_NUM; ++lCount)
		{
			m_pastAnim[lCount]		= NULL;
			m_afAnimSpeed[lCount]	= 1.0f;
		}*/
	}
};

typedef struct _AgcdObjectTemplateGroupList
{
	AgcdObjectTemplateGroupData		m_csData;
	_AgcdObjectTemplateGroupList	*m_pstNext;
} AgcdObjectTemplateGroupList;

typedef struct _AgcdObjectTemplateGroup
{
	INT32							m_lNum;
	AgcdObjectTemplateGroupList		*m_pstList;
} AgcdObjectTemplateGroup;

typedef struct _AgcdObjectTemplate
{
	CHAR						m_szCategory[AGCDOBJECT_CATEGORY_LENGTH];
	CHAR						m_szCollisionDFFName[AGCDOBJECT_DFF_NAME_LENGTH];
	CHAR						m_szPickDFFName[AGCDOBJECT_DFF_NAME_LENGTH];

	RpAtomic					*m_pstCollisionAtomic;
	RpAtomic					*m_pstPickingAtomic;

	OcTreeRenderData			m_stOcTreeData;

	RwRGBA						m_stPreLight;
	INT32						m_lObjectType;

	AgcdLOD						m_stLOD;

	AgcdObjectTemplateGroup		m_stGroup;

	INT32						m_lRefCount;

	AgcdObjectStatus			m_eStatus;

	eApmMapMaterial				m_eRidableMaterialType	;

	//@{ 2006/05/04 burumal
	INT32						m_nDNF;
	//@}
								// 마고자 (2005-05-02 오후 4:37:49) : 
								// 올라탈경우 나오는 소리타입 정의

	BOOL	IsSkipExport( ApServiceArea eArea )
	{
		return m_nDNF & GETSERVICEAREAFLAG( eArea );
	}
} AgcdObjectTemplate;

class AgcdObjectGroupData
{
public:
	INT32						m_lIndex;
	PVOID						m_pvThis;

	RpClump						*m_pstClump;
	BOOL						m_bAddToWorld;

	RpHAnimHierarchy			*m_pstInHierarchy;

	RwSphere					m_stBSphere;

	FLOAT						m_fAnimSpeed;

	BOOL						m_bStopAnimation;

/*	BOOL						m_bStop;

	FLOAT						m_afAnimSpeed[E_OBJECT_ANIM_TYPE_NUM];

	AgcdAnimation				*m_pstCurAnim;
	AgcdAnimation				*m_pstNextAnim;
	AgcdAnimation				*m_pstDefaultAnim;

	eObjectAnimationType		m_eCurAnimType;
	eObjectAnimationType		m_eNextAnimType;
	eObjectAnimationType		m_eDefaultAnimType;*/


	UINT32						m_ulPrevTick;

	ApdObject					*m_pstApdObject;
	AgcdObjectTemplateGroupData	*m_pcsTemplateGroupData;

	AgcdObjectGroupData()
	{
		m_pstClump				= NULL;
		m_pstInHierarchy		= NULL;
//		m_pstCurAnim			= NULL;
//		m_pstNextAnim			= NULL;
//		m_pstDefaultAnim		= NULL;
		m_pstApdObject			= NULL;
		m_pcsTemplateGroupData	= NULL;
//		m_bStop					= TRUE;

//		m_eCurAnimType			= E_OBJECT_ANIM_TYPE_NONE;
//		m_eNextAnimType			= E_OBJECT_ANIM_TYPE_NONE;
//		m_eDefaultAnimType		= E_OBJECT_ANIM_TYPE_NONE;

/*		for(INT32 lCount = 0; lCount < E_OBJECT_ANIM_TYPE_NUM; ++lCount)
		{
			m_afAnimSpeed[lCount] = 1.0f;
		}*/
	}
};

typedef struct _AgcdObjectGroupList
{
	AgcdObjectGroupData		m_csData;
	_AgcdObjectGroupList	*m_pstNext;
} AgcdObjectGroupList;

typedef struct _AgcdObjectGroup
{
	INT32					m_lNum;
	AgcdObjectGroupList		*m_pstList;
} AgcdObjectGroup;

typedef struct _AgcdObject
{
	PVOID				m_pvClass;

	AgcdObjectGroup		m_stGroup;
	OcTreeRenderData	m_stOcTreeData;

	RpAtomic			*m_pstCollisionAtomic;
	RpAtomic			*m_pstPickAtomic;

	_AgcdObject			*m_listNext;
	BOOL				m_bGroupChild;			// 자신이 Group의 자식 노드인지를 나타냄
	GroupChildInfo		*m_pInfoGroup;			// 자신이 Group의 자식 노드이라면 ApdObject를 가지지 않으므로 GroupChildInfo를 가진다.

	INT32				m_lObjectType;

	AgcdObjectTemplate *m_pstTemplate;

	INT32				m_lStatus;				// Object의 현재 상태
} AgcdObject;
/*
typedef struct _AgcdAnimObjectGroupDataList
{
	AgcdObjectGroupData				*m_pcsData;
//	UINT32							m_ulPrevTick;
	_AgcdAnimObjectGroupDataList	*m_pstNext;
} AgcdAnimObjectGroupDataList;*/

#endif //_AGCDOBJECT_H_