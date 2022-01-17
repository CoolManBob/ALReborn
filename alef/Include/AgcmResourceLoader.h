// AgcmResourceLoader.h: interface for the AgcmResourceLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMRESOURCELOADER_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_)
#define AFX_AGCMRESOURCELOADER_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcModule.h"
#include "AuList.h"
#include "ApBase.h"
#include "ApAdmin.h"

//#include "AgcmRWThread.h"

#include "AgcdResourceLoader.h"

#include "RpWorld.h"
#include "RpLODAtm.h"
#include "RpSkin.h"
#include "RtSkinSp.h"
#include "RpSpline.h"

#include "AcuTexture.h"
#include "AuPackingManager.h"

#include <map>

#define AGCDLOADER_MAX_TYPE								16

#define AGCM_RESOURCE_PATH_LENGTH						128

#define	AGCM_RESOURCE_DEFAULT_TEXTURE_PATH_NUM			5

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmResourceLoaderD" )
#else
#pragma comment ( lib , "AgcmResourceLoader" )
#endif
#endif

#pragma comment ( lib , "RtSkinSp" )

// 마고자 2004/06/21 강제로 라이버르리 삽입.
#pragma comment ( lib , "RtDict"	)
#pragma comment ( lib , "RpUVAnim"	)


typedef struct __rxD3D9SkinInstanceNodeData _rxD3D9SkinInstanceNodeData;
struct __rxD3D9SkinInstanceNodeData
{
    RxD3D9AllInOneRenderCallBack    renderCallback;
    RxD3D9AllInOneLightingCallBack  lightingCallback;
};

typedef enum
{
	AGCDRL_RESOURCE_TYPE_ATOMIC = 0,
	AGCDRL_RESOURCE_TYPE_CLUMP,
	AGCDRL_RESOURCE_TYPE_HIERARCHY
} AgceResourceType;

typedef struct 
{
	AgceResourceType	m_eType;
	PVOID				m_pvResource;
} AgcdRLResource;

typedef enum
{
	AGCM_RL_CB_POINT_LOADEND = 0,
} AgcmResourceLoaderCBPoint;

class AgcmGlyph;

class AgcmResourceLoader : public AgcModule  
{
private:
	typedef std::map< string , DWORD	>				mapColor;
	typedef std::map< string , DWORD >::iterator		mapColorIter;

	//AgcmRWThread *					m_pcsAgcmRWThread;
	AgcmGlyph *						m_pcsAgcmGlyph;

	ApAdmin							m_csTexDicts;
	BOOL							m_bEnd;
	HANDLE							m_hThread;
	//@{ Jaewon 20050817
	// ;)
	UINT32							m_threadId;
	//@} Jaewon
	INT32							m_lLoaderCount;
	AgcdLoader						m_astLoaderInfo[AGCDLOADER_MAX_TYPE];

public:
	ApCriticalSection				m_csMutexLoader;
	ApCriticalSection				m_csMutexDone;
	ApCriticalSection				m_csMutexRemoveResource;
	//@{ Jaewon 20050525
	// ;)
	ApCriticalSection				m_csMutexInitAtomic;
	//@} Jaewon
private:

	AuList <AgcdLoaderEntry *>		m_listQueue;
	AuList <AgcdLoaderEntry *>		m_listDone;
	AuList <AgcdRLResource>			m_listRemoveResource;

	CHAR		m_szTexDictPath[128];
	static RwTextureCallBackRead	m_fnDefaultTextureReadCB	;
	static AgcmResourceLoader *		m_pThis;

	RwTexDictionary *				m_pstDefaultTexDict;

	ApMutualEx						m_csMutex;

	INT32							m_lEntryRemain;

	INT32							m_lMaxNumBones;

	__declspec( thread ) static BOOL	m_bFrameLocked;
	//__declspec( thread ) static CHAR	m_szTexturePath[AGCM_RESOURCE_PATH_LENGTH];
	__declspec( thread ) static CHAR	*m_pszTexturePath;

	CHAR							m_aszDefaultTexturePath[AGCM_RESOURCE_DEFAULT_TEXTURE_PATH_NUM][AGCM_RESOURCE_PATH_LENGTH];
	INT32							m_lDefaultTexturePath;

	BOOL							m_bUseEncryption;		// 암호화 Texture를 사용한다.

	AgcdLoaderEntry *				m_pstCurrentEntry;		// 현재 Thread에서 실행중인 Entry

	UINT32							m_ulLoaderDelay;		// Entry 부터 Loader CB가 실행될때까지의 Delay
	UINT32							m_ulDoneDelay;			// Entry 부터 Done CB가 실행될때까지의 Delay

#ifdef USE_MFC
	//. 2006. 2. 7. Nonstopdj
	//. 모델툴 애니메이션 키프레임조정 및 저장
	UINT32							m_iCurrentKeyframeRate;
	BOOL							m_bIsSaveAnmfile;
	vector<string>					m_vecSaveList;
#endif

public:
	BOOL							m_bForceImmediate;

	AgcmResourceLoader();
	virtual ~AgcmResourceLoader();

public:
	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnPreDestroy();
	BOOL		OnDestroy();
	BOOL		OnIdle(UINT32 ulClockCount);

	INT32		RegisterLoader(PVOID pvClass, ApModuleDefaultCallBack fnLoadCallback, ApModuleDefaultCallBack fnDoneCallback);
	BOOL		AddLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2, BOOL bForceImmediate = FALSE);
	BOOL		RemoveLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2, BOOL bLoadList = TRUE, BOOL bDoneList = TRUE);
	VOID		RemoveAllEntry(INT32 lLoaderIndex);
	BOOL		IsEmptyEntry(INT32 lLoaderIndex);

	BOOL		FindLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2);

	BOOL		AddDestroyClump(RpClump *pstClump);
	BOOL		AddDestroyAtomic(RpAtomic *pstAtomic);
	BOOL		AddDestroyHierarchy(RpHAnimHierarchy *pstHierarchy);
	VOID		RemoveResources(INT32 lCount = -1);			// -1 or 0 : Remove All Resources

	static unsigned __stdcall ProcessLoader(LPVOID pvArgs);

	VOID			SetTexDictPath(CHAR *szPath);

	AgcdTexDict	*	AddTexDict(CHAR *szTexDict, RwTexDictionary *pstTexDict);
	AgcdTexDict	*	GetTexDict(CHAR *szTexDict);
	BOOL			RemoveTexDict(CHAR *szTexDict);
	BOOL			SetCurrentTexDict(CHAR *szTexDict);

	RpClump *		LoadClump(CHAR *szClump, CHAR *szTexDict = NULL, RwTexDictionary *pstTexDict = NULL, INT32 lFilterMode = -1, CHAR *szTexturePath = NULL);
	RpAtomic *		LoadAtomic(CHAR *szAtomic, CHAR *szTexDict = NULL, RwTexDictionary *pstTexDict = NULL, INT32 lFilterMode = -1, CHAR *szTexturePath = NULL, RpHAnimHierarchy* pstHierarchy = NULL );
	RwTexture *		LoadTexture(CHAR *szName, CHAR *szMask, CHAR *szTexDict = NULL, RwTexDictionary *pstTexDict = NULL, INT32 lFilterMode = -1, CHAR *szTexturePath = NULL);
	RwTexture *		LoadTextureFromTexDict(RwStream *pstStream, const CHAR *szName);
	RwTexture *		LoadTextureFromFile(const CHAR *szName , AcuTextureType eType );
	RwTexture *		LoadTextureFromStream(RwStream *pstStream, const CHAR *szName, AcuTextureType eType);

	static	RtAnimAnimation*	LoadRtAnim(CHAR*	szName);
	RpSpline*		LoadRpSpline(CHAR*	szName);

	VOID			SetDefaultTexDict()		{ RwTexDictionarySetCurrent(m_pstDefaultTexDict);	}
	VOID			ResetDefaultTexDict()	{ RwTexDictionarySetCurrent(NULL);					}

	RwTexDictionary *	GetDefaultTexDict()	{ return m_pstDefaultTexDict;						}

	BOOL			WriteAllTexDicts();

	BOOL			LockLoad()		{ return m_csMutex.WLock(); }
	BOOL			UnlockLoad()	{ return m_csMutex.Release(); }

	static RwTexture *	CBTextureRead(const RwChar *name, const RwChar *maskName);
	static RwTextureCallBackRead GetDefaultCBReadTexture( void ) { return m_fnDefaultTextureReadCB; }

	BOOL			InstanceClump(RpClump *pstClump);
	BOOL			InstanceAtomic(RpAtomic *pstAtomic);
	static RpAtomic *	CBInstanceAtomic(RpAtomic *pstAtomic, PVOID pvData);

	BOOL			SetCallbackLoadEntryEmpty(ApModuleDefaultCallBack pfnCallback, PVOID pvClass);
	INT32			GetRemainCount()	{ return m_listQueue.GetCount() + m_listDone.GetCount();	}
	INT32			GetBackThreadWaitingCount()	{ return m_listQueue.GetCount();	}
	INT32			GetForeThreadWaitingCount()	{ return m_listDone.GetCount();	}

	VOID			SetTexturePath(CHAR *szTexturePath);
	BOOL			AddDefaultTexturePath(CHAR *szTexturePath);

	VOID			EnableEncryption(BOOL bEncryption)	{ m_bUseEncryption = bEncryption;	}

	inline UINT32	GetLoaderDelay()	{ return m_ulLoaderDelay;	}
	inline UINT32	GetDoneDelay()		{ return m_ulDoneDelay;		}
	INT32			GetRemoveQueue()	{ return m_listRemoveResource.GetCount();	}

	static RpHAnimHierarchy *	GetHierarchy(RpClump *pstClump);
	static RwFrame *			AgcmResourceLoader::GetChildFrameHierarchy(RwFrame *frame, PVOID pvData);
	static RpAtomic *			CBInitAtomic(RpAtomic *pstAtomic, PVOID pvData);
	static RpMaterial *			CBFilterModeMaterial( RpMaterial*	pstMaterial	, PVOID pvData );
	static RwTexture *			CBGetTextureFromTexDict(RwTexture *pstTexture, PVOID pvData);

	INT32	GetAtomicUsrDataArrayInt(RpAtomic *pstAtomic, CHAR *szUsrDataArrayName);
	INT32	GetGeometryUsrDataArrayInt(RpGeometry *pstGeometry, CHAR *szUsrDataArrayName);

	BOOL	IsBackThread() { return m_threadId == GetCurrentThreadId(); }

	DWORD				GetColor		( const char* szName );

	BOOL				_LoadColor( VOID );
	DWORD				_HexStringToDWORD( const char* szHexString );

	mapColor 						m_mapArchlordColor;


#ifdef USE_MFC
	//. 2006. 2. 7. Nonstopdj
	//. 모델툴 애니메이션 키프레임조정 및 저장
	VOID			SetCurrentAnimKeyFrameRate(const UINT32 rate) { m_iCurrentKeyframeRate = rate; }
	VOID			SetSaveAnmfile(const BOOL bisSaveAnmFile, const TCHAR* szDirectory)
	{
		m_bIsSaveAnmfile = bisSaveAnmFile;
		//_tcscpy(m_szSaveDirectory, szDirectory);
	}
#endif
};

#endif // !defined(AFX_AGCMRESOURCELOADER_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_)
