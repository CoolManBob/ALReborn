// AgcmResourceLoader.cpp: implementation of the AgcmResourceLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmResourceLoader.h"
#include <winbase.h>
#include <process.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <D3D9.h>
#include "MagDebug.h"
#include "AcuTexture.h"
#include "AuMD5Encrypt.h"
#include "AcuRpUVAnimData.h"

#include <RtDict.h>
#include <RpUVAnim.h>
#include <RtPITexD.h>
#include <RpUsrDat.h>
#include <RtBMP.h>
#include <RtPNG.h>
#include <RtTIFF.h>
#include "AcDefine.h"

#include "ApMemoryTracker.h"
#include "AgcEngine.h"

#include "AcuCreateClump.h"

// 원래의 설계에서는 벗어나지만, Project가 합쳐져서 큰 상관 없을듯.
#include "AgcmGlyph.h"

//#include "../../../Client/AlefClient/MyEngine.h"

#define	AGCD_RESOURCELOADER_MAX_ENTRY			200
#define AGCD_RESOURCELOADER_MAX_RESOURCE		400

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//@{ Jaewon 20050628
// ;)
#include <ScopeGuard.h>
static RwUInt32 _textureLoadingTime = 0, _textureDecryptionTime = 0;
//@} Jaewon

RwTextureCallBackRead	AgcmResourceLoader::m_fnDefaultTextureReadCB = NULL;
AgcmResourceLoader *	AgcmResourceLoader::m_pThis = NULL;
BOOL					AgcmResourceLoader::m_bFrameLocked = FALSE;
CHAR				*	AgcmResourceLoader::m_pszTexturePath = NULL;

RwCamera *				m_pCamera = NULL;

#define	FILE_MAX_SIZE	(1024 * 1024)

ApMemoryPool			g_csEntryPool;
ApMemoryPool			g_csFilePool;

typedef struct
{
	RpHAnimHierarchy *		pstHierarchy;
	INT32					lFilterMode;
} AgcdRLInitAtomicParam;

#define ANMSAVELISTFILENAME "anmsavelist.log"

//. 2006. 7. 10. Nonstopdj.
static RpAtomic* ReleaseAllAtomicFrame(RpAtomic* pAtomic, void* pData);


// 마고자 (2006-02-16 오전 10:25:28) : 
// 텍스쳐 요청 정보 생성.

#ifdef _DEBUG
	struct TextureAccessInfo
	{
		INT32	nReq	;
		INT32	nFault	;
		INT32	nHasExt	;

		TextureAccessInfo():nReq(0), nFault( 0 ) , nHasExt( nHasExt ) {}

		~TextureAccessInfo()
		{
			char strReport[ 256 ];
			OutputDebugString( "===================================================\n" );
			OutputDebugString( "=================텍스쳐 억세스 결과 ===============\n" );
			OutputDebugString( "===================================================\n" );

			wsprintf( strReport , "총 텍스쳐 요청 횟수 = %d\n" , nReq );
			OutputDebugString( strReport );

			wsprintf( strReport , "텍스쳐 읽기 시도 횟수 = %d\n" , nFault );
			OutputDebugString( strReport );

			wsprintf( strReport , "확장자를 가진 텍스쳐 읽기 시도 = %d\n" , nHasExt );
			OutputDebugString( strReport );
		}
	};

	TextureAccessInfo	g_stTAI;

	#define TEXTURE_REQUEST	g_stTAI.nReq++;
	#define TEXTURE_FAULT	g_stTAI.nFault++;
	#define TEXTURE_HASEXT	g_stTAI.nHasExt++;
#else
	#define TEXTURE_REQUEST	
	#define TEXTURE_FAULT	
	#define TEXTURE_HASEXT	
#endif




AgcmResourceLoader::AgcmResourceLoader() : m_listQueue(AGCD_RESOURCELOADER_MAX_ENTRY),
										   m_listDone(AGCD_RESOURCELOADER_MAX_ENTRY),
										   m_listRemoveResource(AGCD_RESOURCELOADER_MAX_RESOURCE)
{
	SetModuleName("AgcmResourceLoader");

	EnableIdle(TRUE);

	m_pcsAgcmGlyph = NULL;

	m_lLoaderCount = 0;

	m_pThis = this;

	m_csMutex.Init();
	m_csMutexLoader.Init();
	m_csMutexDone.Init();
	m_csMutexRemoveResource.Init();
	//@{ Jaewon 20050525
	// ;)
	m_csMutexInitAtomic.Init();
	//@} Jaewon

	// TRUE로 Default를 바꿈. 처음에 HealthCheck에서 문제 생길 수 있다. (Parn, 20050317)
	m_bForceImmediate = FALSE;

	m_lEntryRemain = 0;

	m_lMaxNumBones = 0;

	m_bFrameLocked = FALSE;
	
	// 마고자 (2004-03-24 오후 10:08:09) : 초기화 추가.
	strcpy( m_szTexDictPath , "" );
	m_pstDefaultTexDict	= NULL	;
	m_bEnd				= FALSE	;
	m_hThread			= NULL	;
	//@{ Jaewon 20050817
	// ;)
	m_threadId			= 0;
	//@} Jaewon

	memset(m_aszDefaultTexturePath, 0, sizeof(CHAR) * AGCM_RESOURCE_DEFAULT_TEXTURE_PATH_NUM * AGCM_RESOURCE_PATH_LENGTH);
	m_lDefaultTexturePath = 0;

	m_bUseEncryption	= TRUE;

	m_pstCurrentEntry	= NULL;

	m_ulLoaderDelay		= 0;
	m_ulDoneDelay		= 0;

	_LoadColor();

#ifdef USE_MFC
	//. 2006. 2. 7. Nonstopdj
	//. 모델툴 애니메이션 키프레임조정 및 저장
	m_iCurrentKeyframeRate	= 0;
	m_bIsSaveAnmfile		= FALSE;
#endif	
}

AgcmResourceLoader::~AgcmResourceLoader()
{	
}

BOOL		AgcmResourceLoader::OnAddModule()
{
	m_fnDefaultTextureReadCB = RwTextureGetReadCallBack();
	RwTextureSetReadCallBack(CBTextureRead);

	//m_pcsAgcmRWThread = (AgcmRWThread *) GetModule("AgcmRWThread");
	//{@kday
	g_csFilePool.Initialize(FILE_MAX_SIZE, 4);
	//4->16 임으로 늘림.. 체크요망.
	//g_csFilePool.Initialize(FILE_MAX_SIZE, 16);
	//}@kday

	m_pCamera = GetCamera();

	return TRUE;
}

BOOL		AgcmResourceLoader::OnInit()
{
	// AgcmGlyph는 아래 있는 모듈이므로, 이렇게 가져다 쓴다.
	m_pcsAgcmGlyph = (AgcmGlyph *) GetModule("AgcmGlyph");

	// 2004.12.07. steeple. _beginthread 를 _beginthreadex 로 변경
	unsigned int uiThreadID;
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, ProcessLoader, this, 0, &uiThreadID);
	if (m_hThread == (HANDLE) -1)
		return FALSE;

	//@{ Jaewon 20050817
	// ;)
	m_threadId = uiThreadID;
	//@} Jaewon

	SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);

	if (!m_csTexDicts.InitializeObject(sizeof(AgcdTexDict *), 100))
		return FALSE;

	m_bEnd = FALSE;

	m_pstDefaultTexDict = RwTexDictionaryCreate();

	RwFrameListSetAutoUpdate(FALSE);

	// 마고자 (2004-03-23 오전 11:50:06) : 툴에서 못읽음..2만으로 증가시킴..
	g_csEntryPool.Initialize(sizeof(AgcdLoaderEntry), AGCD_RESOURCELOADER_MAX_ENTRY);

	// Skin에서 사용되는 최대 Bone의 개수를 구했다가 그보다 더 많은 Bone을 사용하는 넘들은 SkinSplit 시킨다.
	const D3DCAPS9 *pstD3DCaps = (D3DCAPS9 *) RwD3D9GetCaps();

	//@{ Jaewon 20040916
	// 15 -> 25
	//@{ Jaewon 20050309
	// 25 -> 30
	m_lMaxNumBones = (pstD3DCaps->MaxVertexShaderConst - 30) / 3;
	//@} Jaewon
	//@} Jaewon

#ifdef USE_MFC
	//. 2006. 2. 7. Nonstopdj
	FILE* pFile = fopen(ANMSAVELISTFILENAME, "r");

	if(pFile != NULL)
	{
		CHAR	szTemp[256];

		while(fscanf(pFile, "%s", szTemp) != EOF)
		{
			string strPath(szTemp);
			if(strPath.size() > 0)
				m_vecSaveList.push_back(strPath);
		}
		fclose(pFile);
	}
#endif	



	return TRUE;
}

BOOL		AgcmResourceLoader::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmResourceLoader::OnIdle");

	AgcdLoader *					pstLoader;
	AgcdLoaderEntry *				pstEntry;
	AuNode <AgcdLoaderEntry *> *	pNode;
	INT32							lCount = GetForeThreadWaitingCount() / 30 + 1;

	if (!lCount)
		lCount = 1;

	m_csMutexDone.Lock();

	// Done Queue에 있는 넘들을 처리한다.
	while (TRUE)
	{
		PROFILE("AgcmResourceLoader::OnIdle - DoneCB");

		pNode = m_listDone.GetHeadNode();
		if (!pNode)
		{
			m_csMutexDone.Unlock();
			break;
		}

		pstEntry = pNode->GetData();

		m_listDone.RemoveNode(pNode);

		m_csMutexDone.Unlock();

		pstLoader = m_astLoaderInfo + pstEntry->m_lLoaderID;

		//TRACE("AgcmResourceLoader::OnIdle() Entry Module(Done) : %s\n", ((ApModule *) pstLoader->m_pvClass)->GetModuleName());

		pstEntry->m_ulDoneTime = GetClockCount();
		m_ulDoneDelay = pstEntry->m_ulDoneTime - pstEntry->m_ulEntryTime;
		pstLoader->m_fnDoneCallback(pstEntry->m_pvData1, pstLoader->m_pvClass, pstEntry->m_pvData2);

		g_csEntryPool.Free(pstEntry);
		
		if (!m_listQueue.GetCount() && !m_listDone.GetCount())
			EnumCallback(AGCM_RL_CB_POINT_LOADEND, NULL, NULL);

		--lCount;

		if (!lCount)
		{
			break;
		}

		m_csMutexDone.Lock();
	}

	//RemoveResources(m_listRemoveResource.GetCount() / 10 + 1);

	return TRUE;
}

BOOL		AgcmResourceLoader::OnPreDestroy()
{
	m_bEnd	= TRUE	;

	WaitForSingleObject(m_hThread, 2000);

	// 2004.12.07. steeple                                                     Thread Handle 닫음
	CloseHandle(m_hThread);
	m_hThread = NULL;

	return TRUE;
}

BOOL		AgcmResourceLoader::OnDestroy()
{
	RemoveResources();

	//@{ Jaewon 20050628
	// ;)
	//FILE *f = std::fopen("loadTexTimings.txt", "wt");
	//ScopeGuard guard = MakeGuard(std::fclose, f);

	//fprintf(f, "%d/%d\n", _textureDecryptionTime, _textureLoadingTime);
	//@} Jaewon

	return TRUE;
}

INT32		AgcmResourceLoader::RegisterLoader(PVOID pvClass, ApModuleDefaultCallBack fnLoadCallback, ApModuleDefaultCallBack fnDoneCallback)
{
	if (m_lLoaderCount >= AGCDLOADER_MAX_TYPE)
		return -1;

	m_astLoaderInfo[m_lLoaderCount].m_fnLoadCallback	= fnLoadCallback;
	m_astLoaderInfo[m_lLoaderCount].m_fnDoneCallback	= fnDoneCallback;
	m_astLoaderInfo[m_lLoaderCount].m_pvClass			= pvClass;

	++m_lLoaderCount;

	return (m_lLoaderCount - 1);
}

BOOL		AgcmResourceLoader::AddLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2, BOOL bForceImmediate)
{
	if (lLoaderIndex < 0 || lLoaderIndex >= AGCDLOADER_MAX_TYPE)
		return FALSE;

	if (bForceImmediate || m_bForceImmediate)
	{
		AgcdLoader *					pstLoader;

		pstLoader = m_astLoaderInfo + lLoaderIndex;

		//TRACE("AgcmResourceLoader::AddLoadEntry() Entry Module : %s\n", ((ApModule *) pstLoader->m_pvClass)->GetModuleName());
		if (pstLoader->m_fnLoadCallback && !pstLoader->m_fnLoadCallback(pvData1, pstLoader->m_pvClass, pvData2)) return FALSE;

		//TRACE("AgcmResourceLoader::AddLoadEntry() Entry Module(Done) : %s\n", ((ApModule *) pstLoader->m_pvClass)->GetModuleName());
		if (pstLoader->m_fnDoneCallback && !pstLoader->m_fnDoneCallback(pvData1, pstLoader->m_pvClass, pvData2)) return FALSE;

		return TRUE;
	}

	AgcdLoaderEntry *	pstEntry;

	
	//pstEntry = new AgcdLoaderEntry;
	pstEntry = (AgcdLoaderEntry *) g_csEntryPool.Alloc();
	if (!pstEntry)
		return FALSE;

	memset(pstEntry, 0, sizeof(AgcdLoaderEntry));

	pstEntry->m_lLoaderID	= lLoaderIndex;
	pstEntry->m_pvData1		= pvData1;
	pstEntry->m_pvData2		= pvData2;
	pstEntry->m_bRemoved	= FALSE;
	pstEntry->m_ulEntryTime	= GetClockCount();

	m_csMutexLoader.Lock();

	if (!m_listQueue.AddTail(pstEntry))
	{
		m_csMutexLoader.Unlock();

		//delete pstEntry;
		g_csEntryPool.Free(pstEntry);

		return FALSE;
	}

	m_csMutexLoader.Unlock();

	return TRUE;
}

BOOL		AgcmResourceLoader::RemoveLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2, BOOL bLoadList, BOOL bDoneList)
{
	if (lLoaderIndex < 0 || lLoaderIndex >= AGCDLOADER_MAX_TYPE)
		return FALSE;

	AuNode <AgcdLoaderEntry *> *	pNode;
	AuNode <AgcdLoaderEntry *> *	pNodeNext;
	AgcdLoader *					pstLoader = m_astLoaderInfo + lLoaderIndex;
	AgcdLoaderEntry *				pstEntry;

	if (bLoadList)
	{
		m_csMutexLoader.Lock();

		pNode = m_listQueue.GetHeadNode();
		while (pNode)
		{
			pNodeNext = pNode->GetNextNode();

			pstEntry = pNode->GetData();

			if (pstEntry->m_lLoaderID == lLoaderIndex && pstEntry->m_pvData1 == pvData1 && pstEntry->m_pvData2 == pvData2)
			{
				m_listQueue.RemoveNode(pNode);
				//delete pstEntry;
				g_csEntryPool.Free(pstEntry);

				if (!m_listQueue.GetCount() && !m_listDone.GetCount())
					EnumCallback(AGCM_RL_CB_POINT_LOADEND, NULL, NULL);
			}

			pNode = pNodeNext;
		}

		m_csMutexLoader.Unlock();
	}

	if (bDoneList)
	{
		m_csMutexDone.Lock();

		// 만약 현재 BackThread로 Loading 중인 Entry가 해당 Entry면 Remove Flag Setting
		if (m_pstCurrentEntry &&
			m_pstCurrentEntry->m_lLoaderID == lLoaderIndex &&
			m_pstCurrentEntry->m_pvData1 == pvData1 &&
			m_pstCurrentEntry->m_pvData2 == pvData2)
			m_pstCurrentEntry->m_bRemoved = TRUE;

		pNode = m_listDone.GetHeadNode();
		while (pNode)
		{
			pNodeNext = pNode->GetNextNode();

			pstEntry = pNode->GetData();

			if (pstEntry->m_lLoaderID == lLoaderIndex && pstEntry->m_pvData1 == pvData1 && pstEntry->m_pvData2 == pvData2)
			{
				m_listDone.RemoveNode(pNode);
				//delete pstEntry;
				g_csEntryPool.Free(pstEntry);

				if (!m_listQueue.GetCount() && !m_listDone.GetCount())
					EnumCallback(AGCM_RL_CB_POINT_LOADEND, NULL, NULL);
			}

			pNode = pNodeNext;
		}

		m_csMutexDone.Unlock();
	}

	return TRUE;
}

VOID		AgcmResourceLoader::RemoveAllEntry(INT32 lLoaderIndex)
{
	if (lLoaderIndex < 0 || lLoaderIndex >= AGCDLOADER_MAX_TYPE)
		return;

	AuNode <AgcdLoaderEntry *> *	pNode;
	AuNode <AgcdLoaderEntry *> *	pNodeNext;
	AgcdLoader *					pstLoader = m_astLoaderInfo + lLoaderIndex;
	AgcdLoaderEntry *				pstEntry;

	m_csMutexLoader.Lock();

	pNode = m_listQueue.GetHeadNode();
	while (pNode)
	{
		pNodeNext = pNode->GetNextNode();

		pstEntry = pNode->GetData();

		if (pstEntry->m_lLoaderID == lLoaderIndex)
		{
			m_listQueue.RemoveNode(pNode);
			//delete pstEntry;
			g_csEntryPool.Free(pstEntry);
		}

		pNode = pNodeNext;
	}

	m_csMutexLoader.Unlock();

	m_csMutexDone.Lock();

	pNode = m_listDone.GetHeadNode();
	while (pNode)
	{
		pNodeNext = pNode->GetNextNode();

		pstEntry = pNode->GetData();

		if (pstEntry->m_lLoaderID == lLoaderIndex)
		{
			m_listDone.RemoveNode(pNode);
			//delete pstEntry;
			g_csEntryPool.Free(pstEntry);
		}

		pNode = pNodeNext;
	}

	m_csMutexDone.Unlock();
}

BOOL		AgcmResourceLoader::IsEmptyEntry(INT32 lLoaderIndex)
{
	if (lLoaderIndex < 0 || lLoaderIndex >= AGCDLOADER_MAX_TYPE)
		return TRUE;

	AuNode <AgcdLoaderEntry *> *	pNode;

	m_csMutexLoader.Lock();

	pNode = m_listQueue.GetHeadNode();
	while (pNode)
	{
		if (pNode->GetData()->m_lLoaderID == lLoaderIndex)
		{
			m_csMutexLoader.Unlock();
			return FALSE;
		}

		pNode = pNode->GetNextNode();
	}

	m_csMutexLoader.Unlock();

	m_csMutexDone.Lock();

	pNode = m_listDone.GetHeadNode();
	while (pNode)
	{
		if (pNode->GetData()->m_lLoaderID == lLoaderIndex)
		{
			m_csMutexDone.Unlock();
			return FALSE;
		}

		pNode = pNode->GetNextNode();
	}

	m_csMutexDone.Unlock();

	return TRUE;
}

RpGeometry *	CBFreeResEntryGeometry(RpGeometry *pstGeometry, PVOID pvData)
{
	PROFILE("AgcmResourceLoader::CBFreeResEntryGeometry");

	if (pstGeometry && pstGeometry->repEntry && pstGeometry->refCount == 1)
	{
		AgcmResourceLoader *	pThis = (AgcmResourceLoader *) pvData;

		pThis->LockFrame();

		RwResourcesFreeResEntry(pstGeometry->repEntry);
		pstGeometry->repEntry = NULL;

		pThis->UnlockFrame();
	}

	return pstGeometry;
}

RpAtomic *		CBFreeResEntryAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	CBFreeResEntryGeometry(RpAtomicGetGeometry(pstAtomic), pvData);

	RpLODAtomicForAllLODGeometries(pstAtomic, CBFreeResEntryGeometry, pvData);

	return pstAtomic;
}

//. 2006. 7. 10. Nonstopdj
RpAtomic* ReleaseAllAtomicFrame(RpAtomic* pAtomic, void* pData)
{
	//.RpAtomicRemoveDirtyFrameListEx(pAtomic);
	return pAtomic;
}

BOOL		AgcmResourceLoader::AddDestroyClump(RpClump *pstClump)
{
	if (!pstClump)
		return FALSE;

	ASSERT(!(pstClump->ulFlag & RWFLAG_RENDER_ADD) && "Error Clump added to render!!!");

	//if (m_bForceImmediate)
	{
		//@{ 2004/04/05 burumal 
	#ifndef USE_MFC
		BOOL bGlyphLock = FALSE;
		if ( m_pcsAgcmGlyph )
			bGlyphLock = TRUE;

		if ( bGlyphLock )
		{
			m_pcsAgcmGlyph->LockDestEnemyTarget();

			if ( m_pcsAgcmGlyph->IsGlyphedFrame(RpClumpGetFrame(pstClump)) )
				m_pcsAgcmGlyph->StopEnemyTarget();
		}
	#endif
		//@}

		LockFrame();
		RpClumpDestroy(pstClump);
		UnlockFrame();

		//@{ 2004/04/05 burumal
	#ifndef USE_MFC
		if ( bGlyphLock )
			m_pcsAgcmGlyph->UnlockDestEnemyTarget();
	#endif
		//@}

		return TRUE;
	}
	//. 2006. 07. 14. Nonstopdj
	//. BackThread에서 지워지면 문제발생.

	//. 2006. 7. 10. Nonstopdj
	//. 나중에 FrameSync하다가 지워질수 있으므로 명시적으로 일단 지운다. 
	//. 어차피 화면에 있어서는 안되는 atomic이다.
	//. RpClumpForAllAtomics(pstClump, ReleaseAllAtomicFrame, NULL);

	//AgcdRLResource		stResource = {AGCDRL_RESOURCE_TYPE_CLUMP, pstClump};

	// ResourceArena 내부 locking해줬음 2005.3.15 gemani
	//RpClumpForAllAtomics(pstClump, CBFreeResEntryAtomic, this);

	//m_csMutexRemoveResource.Lock();
	//m_listRemoveResource.AddTail(stResource);
	//m_csMutexRemoveResource.Unlock();

	return TRUE;
}

BOOL		AgcmResourceLoader::AddDestroyAtomic(RpAtomic *pstAtomic)
{
	if (!pstAtomic)
		return FALSE;

	ASSERT(!(pstAtomic->ulFlag & RWFLAG_RENDER_ADD) && "Error Atomic added to render!!!");

	//if (m_bForceImmediate)
	{
		//@{ 2006/04/05 burumal 
	#ifndef USE_MFC
		BOOL bGlyphLock = FALSE;
		if ( m_pcsAgcmGlyph )
			bGlyphLock = TRUE;

		if ( bGlyphLock )
		{
			m_pcsAgcmGlyph->LockDestEnemyTarget();

			if ( m_pcsAgcmGlyph->IsGlyphedFrame(RpAtomicGetFrame(pstAtomic)) )
				m_pcsAgcmGlyph->StopEnemyTarget();
		}
	#endif
		//@}

		LockFrame();
		RpAtomicDestroy(pstAtomic);
		UnlockFrame();

		//@{ 2006/04/05 burumal 		
	#ifndef USE_MFC
		if ( bGlyphLock )
			m_pcsAgcmGlyph->UnlockDestEnemyTarget();
	#endif
		//@}

		return TRUE;
	}

	#ifdef _DEBUG
	if(pstAtomic->geometry != NULL)
	{
		// 각각 텍스쳐별로 확인한다.
		//. roop fot material count
		for(INT32 nCount = 0; nCount < pstAtomic->geometry->matList.numMaterials; nCount++)
		{
			//. RwTexture pointer force set 0
			if( pstAtomic->geometry->matList.materials[nCount]->texture != NULL )
			{
				ASSERT( pstAtomic->geometry->matList.materials[nCount]->texture->refCount > 0 );
			}
		}
	}
	#endif

	//. 2006. 07. 14. Nonstopdj
	//. BackThread에서 지워지면 문제발생.

	//. 2006. 7. 10. Nonstopdj
	//. 나중에 FrameSync하다가 지워질수 있으므로 명시적으로 일단 지운다. 
	//. 어차피 화면에 있어서는 안되는 atomic이다.
	//RpAtomicRemoveDirtyFrameListEx(pstAtomic);

	//AgcdRLResource		stResource = {AGCDRL_RESOURCE_TYPE_ATOMIC, pstAtomic};

	//ResourceArena 내부 locking해줬음 2005.3.15 gemani
	//CBFreeResEntryAtomic(pstAtomic, this);

	//m_csMutexRemoveResource.Lock();
	//m_listRemoveResource.AddTail(stResource);
	//m_csMutexRemoveResource.Unlock();

	return TRUE;
}

BOOL		AgcmResourceLoader::AddDestroyHierarchy(RpHAnimHierarchy *pstHierarchy)
{
	if (!pstHierarchy)
		return FALSE;

	//if (m_bForceImmediate)
	{
		LockFrame();
		RpHAnimHierarchyDestroy(pstHierarchy);
		UnlockFrame();

		return TRUE;
	}

	//. 2006. 07. 14. Nonstopdj
	//. BackThread에서 지워지면 문제발생.
	//AgcdRLResource		stResource = {AGCDRL_RESOURCE_TYPE_HIERARCHY, pstHierarchy};

	//m_csMutexRemoveResource.Lock();
	//m_listRemoveResource.AddTail(stResource);
	//m_csMutexRemoveResource.Unlock();

	return TRUE;
}

VOID		AgcmResourceLoader::RemoveResources(INT32 lCount)
{
	PROFILE("AgcmResourceLoader::RemoveResources");

	AuNode <AgcdRLResource> *		pNodeDummy;
	AgcdRLResource					stResource;

	m_csMutexRemoveResource.Lock();

	while (pNodeDummy = m_listRemoveResource.GetHeadNode())
	{
		stResource = pNodeDummy->GetData();

		m_listRemoveResource.RemoveNode(pNodeDummy);

		m_csMutexRemoveResource.Unlock();

		LockFrame();

		switch (stResource.m_eType)
		{
		case AGCDRL_RESOURCE_TYPE_ATOMIC:
			{
				ASSERT(!(((RpAtomic *) stResource.m_pvResource)->ulFlag & RWFLAG_RENDER_ADD) && "Error Atomic added to render!!!");
				RpAtomic* pAtomic = (RpAtomic *) stResource.m_pvResource;
				////. 2005. 12. 22. Nonstopdj
				////. RwTexture의 원본삭제 문제 임시방편으로 막기 -_-
				////. gemetry도 NULL일 경우가 있다. by kday..
				//if(pAtomic->geometry != NULL)
				//{
				//	// 각각 텍스쳐별로 확인한다.
				//	//. roop fot material count
				//	for(INT32 nCount = 0; nCount < pAtomic->geometry->matList.numMaterials; nCount++)
				//	{
				//		//. RwTexture pointer force set 0
				//		if( pAtomic->geometry->matList.materials[nCount]->texture != NULL &&
				//			IsBadReadPtr( pAtomic->geometry->matList.materials[nCount]->texture->raster , sizeof(RwRaster)))
				//		{
				//		#ifdef	_DEBUG
				//			DebugBreak();
				//		#endif
				//			pAtomic->geometry->matList.materials[nCount]->texture = NULL;
				//		}
				//	}
				//}

				//@{ 2006/04/05 burumal 
			#ifndef USE_MFC
				BOOL bGlyphLock = FALSE;
				if ( m_pcsAgcmGlyph )
					bGlyphLock = TRUE;

				if ( bGlyphLock )
				{
					m_pcsAgcmGlyph->LockDestEnemyTarget();

					if ( m_pcsAgcmGlyph->IsGlyphedFrame(RpAtomicGetFrame(pAtomic)) )
						m_pcsAgcmGlyph->StopEnemyTarget();
				}
			#endif
				//@}

				RpAtomicDestroy(pAtomic);

				//@{ 2006/04/05 burumal 				
			#ifndef USE_MFC
				if ( bGlyphLock )
					m_pcsAgcmGlyph->UnlockDestEnemyTarget();
			#endif
				//@}
			}
			break;

		case AGCDRL_RESOURCE_TYPE_CLUMP:
			{
				ASSERT(!(((RpClump *) stResource.m_pvResource)->ulFlag & RWFLAG_RENDER_ADD) && "Error Atomic clump to render!!!");
				RpClump* pClump = (RpClump *) stResource.m_pvResource;
				////. 2006. 1. 3. Nonstopdj
				////. RwTexture의 원본삭제 문제 임시방편으로 막기 -_-
				//if(pClump->atomicList)
				//{
				//	if(pClump->atomicList->geometry)
				//	{
				//		// 각각 텍스쳐별로 확인한다.
				//		//. roop fot material count
				//		for(INT32 nCount = 0; nCount < pClump->atomicList->geometry->matList.numMaterials; nCount++)
				//		{
				//			//. RwTexture pointer force set 0
				//			if( pClump->atomicList->geometry->matList.materials[nCount]->texture != NULL &&
				//				IsBadReadPtr( pClump->atomicList->geometry->matList.materials[nCount]->texture->raster , sizeof(RwRaster)))
				//			{
				//			#ifdef	_DEBUG
				//				DebugBreak();
				//			#endif
				//				pClump->atomicList->geometry->matList.materials[nCount]->texture = NULL;
				//			}
				//		}
				//	}
				//}

				//@{ 2006/04/05 burumal 
			#ifndef USE_MFC
				BOOL bGlyphLock = FALSE;
				if ( m_pcsAgcmGlyph )
					bGlyphLock = TRUE;

				if ( bGlyphLock )
				{
					m_pcsAgcmGlyph->LockDestEnemyTarget();

					if ( m_pcsAgcmGlyph->IsGlyphedFrame(RpClumpGetFrame((RpClump*) stResource.m_pvResource)) )
						m_pcsAgcmGlyph->StopEnemyTarget();
				}
			#endif
				//@}
				
				RpClumpDestroy((RpClump *) stResource.m_pvResource);

				//@{ 2006/04/05 burumal 
			#ifndef USE_MFC
				if ( bGlyphLock )
					m_pcsAgcmGlyph->UnlockDestEnemyTarget();
			#endif
				//@}
			}
			break;

		case AGCDRL_RESOURCE_TYPE_HIERARCHY:
			RpHAnimHierarchyDestroy((RpHAnimHierarchy *) stResource.m_pvResource);
			break;
		}

		UnlockFrame();

		m_csMutexRemoveResource.Lock();

		--lCount;
		if (lCount == 0)
			break;
	}

	m_csMutexRemoveResource.Unlock();
}

unsigned __stdcall AgcmResourceLoader::ProcessLoader(LPVOID pvArgs)
{
	AgcmResourceLoader *			pThis = (AgcmResourceLoader *) pvArgs;
//	INT32							lIndex;
	AgcdLoader *					pstLoader;
	AgcdLoaderEntry *				pstEntry;
	AuNode <AgcdLoaderEntry *> *	pNode;
	BOOL							bLoad;

	pThis->m_bFrameLocked = FALSE;

	while (!pThis->m_bEnd)
	{
		bLoad = FALSE;

		if( g_pEngine->GetDebugFlag() & AgcEngine::SLEEP_BACK_THREAD )
		{
			if( GetAsyncKeyState( VK_F5 ) < 0 ) 
			{
				Sleep( 300 );
				continue;
			}
		}

		pThis->m_csMutexLoader.Lock();

		while (!pThis->m_bEnd && (pNode = pThis->m_listQueue.GetHeadNode()))
		{
			pstEntry = pNode->GetData();
			pThis->m_pstCurrentEntry = pstEntry;

			bLoad = TRUE;

			pThis->m_listQueue.RemoveNode(pNode);

			pThis->m_csMutexLoader.Unlock();

			pstLoader = pThis->m_astLoaderInfo + pstEntry->m_lLoaderID;

			//TRACE("AgcmResourceLoader::ProcessLoader() Entry Module : %s\n", ((ApModule *) pstLoader->m_pvClass)->GetModuleName());

			pstEntry->m_ulLoaderTime = pThis->GetClockCount();
			pThis->m_ulLoaderDelay = pstEntry->m_ulLoaderTime - pstEntry->m_ulEntryTime;
			if (pstLoader->m_fnLoadCallback && !pstLoader->m_fnLoadCallback(pstEntry->m_pvData1, pstLoader->m_pvClass, pstEntry->m_pvData2))
			{
				//delete pstEntry;
				g_csEntryPool.Free(pstEntry);

				if (!pThis->m_listQueue.GetCount() && !pThis->m_listDone.GetCount())
					pThis->EnumCallback(AGCM_RL_CB_POINT_LOADEND, NULL, NULL);

				pThis->m_csMutexLoader.Lock();

				continue;
			}

			pThis->m_csMutexDone.Lock();

			pThis->m_pstCurrentEntry = NULL;

			if (pstLoader->m_fnDoneCallback && !pstEntry->m_bRemoved)
			{
				pThis->m_listDone.AddTail(pstEntry);
			}
			else
			{
				//delete pstEntry;
				g_csEntryPool.Free(pstEntry);

				if (!pThis->m_listQueue.GetCount() && !pThis->m_listDone.GetCount())
					pThis->EnumCallback(AGCM_RL_CB_POINT_LOADEND, NULL, NULL);
			}

			pThis->m_csMutexDone.Unlock();

			pThis->m_csMutexLoader.Lock();
		}

		pThis->m_csMutexLoader.Unlock();

		// BackThread에서 지우니 문제가 좀 있다.
		pThis->RemoveResources();

		if (!bLoad)
			MsgWaitForMultipleObjects(0, NULL, 0, 100, 0);
	}

	// 2004.12.07. steeple. _endthread() 를 _endthreadex() 로 변경
	_endthreadex(0);
	return 0;
}

/******************************************************************************************************/
/******************************************************************************************************/
/********************   Texture Dictionary	  *******************************************************/
/******************************************************************************************************/
/******************************************************************************************************/

VOID			AgcmResourceLoader::SetTexDictPath(CHAR *szPath)
{
	strcpy(m_szTexDictPath, szPath);
}

BOOL			AgcmResourceLoader::SetCurrentTexDict(CHAR *szTexDict)
{
	if (!szTexDict)
		return FALSE;

	AgcdTexDict *		pstAgcdTexDict = GetTexDict(szTexDict);
	RwTexDictionary *	pstTexDict;

	if (!pstAgcdTexDict)
	{
		// Add된 TexDictionary가 없으면 File에서 읽어 온다.
		CHAR				szTemp[128];
		RwStream *			pstStream;

		sprintf(szTemp, "%s%s", m_szTexDictPath, szTexDict);

		TRACE("AgcmResourceLoader::SetCurrentTexDict() Read TexDict Start (%s)\n", szTemp);

		AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
		ApdFile		csApdFile;

		if(!pPackingManager->OpenFile(szTemp,&csApdFile))
		{
			// 해당 파일이 없으면 새로 만든다.
			LockFrame();
			pstTexDict = RwTexDictionaryCreate();
			UnlockFrame();

			if (!pstTexDict) return FALSE;
		}
		else
		{
			RwMemory	stMemoryStream;

			stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);

			if (stMemoryStream.length <= FILE_MAX_SIZE)
				stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
			else
				stMemoryStream.start = new BYTE[stMemoryStream.length];

			if (!stMemoryStream.start)
			{
				pPackingManager->CloseFile(&csApdFile);
				return FALSE;
			}
			
			if (pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile )!= stMemoryStream.length)			
			{
				pPackingManager->CloseFile(&csApdFile);

				if (stMemoryStream.length <= FILE_MAX_SIZE)
					g_csFilePool.Free(stMemoryStream.start);
				else
					delete []stMemoryStream.start;

				return FALSE;
			}

			pPackingManager->CloseFile(&csApdFile);

			pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
			if (!pstStream )
			{
				if (stMemoryStream.length <= FILE_MAX_SIZE)
					g_csFilePool.Free(stMemoryStream.start);
				else
					delete []stMemoryStream.start;

				// 해당 파일이 없으면 새로 만든다.
				LockFrame();
				pstTexDict = RwTexDictionaryCreate();
				UnlockFrame();

				if (!pstTexDict) return FALSE;
			}
			else
			{
				// 파일이 있으면 읽는다.
				if (RwStreamFindChunk(pstStream, rwID_TEXDICTIONARY, NULL, NULL) )
				{
					pstTexDict = RwTexDictionaryStreamRead(pstStream);
				}

				RwStreamClose(pstStream, NULL);

				if (stMemoryStream.length <= FILE_MAX_SIZE)
					g_csFilePool.Free(stMemoryStream.start);
				else
					delete []stMemoryStream.start;
			}
		}

		TRACE("AgcmResourceLoader::SetCurrentTexDict() Read TexDict End (%s)\n", szTemp);

		// Add한다.
		pstAgcdTexDict = AddTexDict(szTexDict, pstTexDict);
		if (!pstAgcdTexDict)
		{
			LockFrame();
			RwTexDictionaryDestroy(pstTexDict);
			UnlockFrame();

			return FALSE;
		}
	}
	else
		pstTexDict = pstAgcdTexDict->m_pstTexDict;

//	RwTexDictionarySetCurrent(pstTexDict);
//	RwTexDictionarySetCurrent(m_pstDefaultTexDict);

	return TRUE;
}

AgcdTexDict *	AgcmResourceLoader::AddTexDict(CHAR *szTexDict, RwTexDictionary *pstTexDict)
{
	AgcdTexDict *	pstAgcdTexDict = new AgcdTexDict;

	strcpy(pstAgcdTexDict->m_szName, szTexDict);
	pstAgcdTexDict->m_pstTexDict = pstTexDict;

	if (!m_csTexDicts.AddObject(&pstAgcdTexDict, (INT32) pstAgcdTexDict, szTexDict))
		return NULL;

	return pstAgcdTexDict;
}

AgcdTexDict	*	AgcmResourceLoader::GetTexDict(CHAR *szTexDict)
{
	AgcdTexDict **	ppstAgcdTexDict = (AgcdTexDict **) m_csTexDicts.GetObject(szTexDict);

	if (!ppstAgcdTexDict)
		return NULL;

	return *ppstAgcdTexDict;
}

BOOL			AgcmResourceLoader::RemoveTexDict(CHAR *szTexDict)
{
	AgcdTexDict *	pstAgcdTexDict = GetTexDict(szTexDict);

	if (!pstAgcdTexDict)
		return FALSE;

	return m_csTexDicts.RemoveObject((INT32) pstAgcdTexDict, pstAgcdTexDict->m_szName);
}

RpMaterial *	CBSetTextureMaterial(RpMaterial *pstMaterial, PVOID pvData)
{
	RwTexture *	pstTexture = RpMaterialGetTexture(pstMaterial);

	if (pstTexture)
	{
		RwD3D9SetTexture(pstTexture, 0);
	}

	return pstMaterial;
}

RpAtomic *		CBSetTextureAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	if (RpAtomicGetGeometry(pstAtomic))
		RpGeometryForAllMaterials(RpAtomicGetGeometry(pstAtomic), CBSetTextureMaterial, NULL);

	return pstAtomic;
}

RpClump* AgcmResourceLoader::LoadClump(CHAR *szClump, CHAR *szTexDict, RwTexDictionary *pstTexDict, INT32 lFilterMode, CHAR *szTexturePath)
{
	PROFILE("ResourceLoader-LoadClump");

	TRACE("AgcmResourceLoader::LoadClump() Loading %s\n", szClump);

	RwStream *	pstStream	= NULL;
	RpClump *	pstClump	= NULL;

	RwMemory	stMemoryStream;
	RwChunkHeaderInfo	stHeader;
	RtDict *			pstDict = NULL;	
	
	SetTexturePath(szTexturePath);

	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	ApdFile		csApdFile;

	// 파일에서 메모리로 읽어서...
	if(!pPackingManager->OpenFile(szClump,&csApdFile))
	{
		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] pPackingManager->OpenFile() in AgcmResourceLoader::LoadClump(%s) failed.\n", GetCurrentThreadId(), szClump);

		return NULL;
	}

	stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);
	//@{ kday 20050715
	// ;)
	if( 0 == stMemoryStream.length )
	{
		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] 0 == stMemoryStream.length in AgcmResourceLoader::LoadClump(%s) failed.\n", GetCurrentThreadId(), szClump);
	}
	//@} kday

	if (stMemoryStream.length <= FILE_MAX_SIZE)
		stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
	else
		stMemoryStream.start = new BYTE[stMemoryStream.length];

	if (!stMemoryStream.start)
	{
		pPackingManager->CloseFile(&csApdFile);

		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] %d memory allocation in AgcmResourceLoader::LoadClump(%s) failed.\n", GetCurrentThreadId(), stMemoryStream.length, szClump);
		
		return NULL;
	}
	
	int lengthRead = pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile );
	if(lengthRead != stMemoryStream.length)
	{
		pPackingManager->CloseFile(&csApdFile);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] %d = pPackingManager->ReadFile(0x%08x,%d) in AgcmResourceLoader::LoadClump(%s) failed.\n", 
								GetCurrentThreadId(), lengthRead, stMemoryStream.start, stMemoryStream.length, szClump);
		return NULL;
	}

	pPackingManager->CloseFile(&csApdFile);

	pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
	if (pstStream)
	{
		//@{ kday 20050715
		// ;)
		bool	streamreadsuccess	= false;
		//@} kday

		while (RwStreamReadChunkHeaderInfo(pstStream, &stHeader))
		{
			//@{ kday 20050715
			// ;)
			streamreadsuccess = true;
			//@} kday

			switch (stHeader.type)
			{
			case rwID_UVANIMDICT:
				{
					LockFrame();
					pstDict = RtDictSchemaStreamReadDict(RpUVAnimGetDictSchema(), pstStream);
					UnlockFrame();
				}

				break;

			case rwID_CLUMP:
				{
					LockFrame();
					m_bFrameLocked = TRUE;

		//			if (pstTexDict)
		//				RwTexDictionarySetCurrent(pstTexDict);
						RwTexDictionarySetCurrent(m_pstDefaultTexDict);
		//			else if (szTexDict)
		//				SetCurrentTexDict(szTexDict);

					RtDictSchemaSetCurrentDict(RpUVAnimGetDictSchema(), pstDict);

					pstClump = RpClumpStreamRead(pstStream);
					if (!pstClump)
					{
						if(m_threadId == GetCurrentThreadId())
							MD_SetErrorMessage("[%d] RpClumpStreamRead() in AgcmResourceLoader::LoadClump(%s) failed.\n", 
												GetCurrentThreadId(), szClump);
					}

		//			if (szTexDict || pstTexDict)
		//				RwTexDictionarySetCurrent(NULL);

					m_bFrameLocked = FALSE;
					UnlockFrame();
				}

				break;

			default :
				RwStreamSkip(pstStream, stHeader.length);
			}

			if (pstClump)
				break;
		}

		//@{ kday 20050715
		// ;)
		if( false == streamreadsuccess )
		{
			if(m_threadId == GetCurrentThreadId())
				MD_SetErrorMessage(
				"[%d] 1st RwStreamReadChunkHeaderInfo faild in AgcmResourceLoader::LoadClump(%s) failed.\nstMemoryStream.length = %d\n"
				, GetCurrentThreadId()
				, szClump
				, stMemoryStream.length
				);
		}
		//@} kday

		RwStreamClose(pstStream, NULL);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;
	}
	else
	{
		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] RwStreamOpen(0x%08x, %d) in AgcmResourceLoader::LoadClump(%s) failed.\n", 
								GetCurrentThreadId(), stMemoryStream.start, stMemoryStream.length, szClump);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	if (pstClump)
	{		
		AgcdRLInitAtomicParam	stParam;

		stParam.pstHierarchy	= GetHierarchy(pstClump);
		stParam.lFilterMode		= lFilterMode;

		if (pstDict)
			RpUVAnimDataClumpSetDict(pstClump, pstDict);

		LockFrame();

		// Atomic을 Init 시킨다.
		//@{ Jaewon 20050525
		// ;)
		m_csMutexInitAtomic.Lock();

		//@} Jaewon
		RpClumpForAllAtomics(pstClump, CBInitAtomic, &stParam);
		//@{ Jaewon 20050525

		// ;)
		m_csMutexInitAtomic.Unlock();
		//@} Jaewon

		UnlockFrame();
	}

	//@{ kday 20050715
	// ;)
	else
	{
		if(m_threadId == GetCurrentThreadId())
			MD_SetErrorMessage("[%d] pstClump == NULL in AgcmResourceLoader::LoadClump(%s) failed.\n", 
			GetCurrentThreadId(), szClump);
	}
	//@} kday

	/*
	TRACE("AgcmResourceLoader::LoadClump() SetTexture Start...\n");
	RpClumpForAllAtomics(pstClump, CBSetTextureAtomic, NULL);
	TRACE("AgcmResourceLoader::LoadClump() SetTexture End...\n");
	*/

	TRACE("AgcmResourceLoader::LoadClump() Loading %s(%x) Success\n", szClump, pstClump);

	return pstClump;
}

RpAtomic *		CBGetFirstAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	*(RpAtomic **) pvData = pstAtomic;

	return NULL;
}

RpAtomic *		AgcmResourceLoader::LoadAtomic(CHAR *szAtomic, CHAR *szTexDict, RwTexDictionary *pstTexDict, INT32 lFilterMode, CHAR *szTexturePath, RpHAnimHierarchy* pstHierarchy)
{
	TRACE("AgcmResourceLoader::LoadAtomic() Loading %s\n", szAtomic);

	RwStream *	pstStream	= NULL;
	RpAtomic *	pstAtomic	= NULL;

	//szTexDict = NULL;

	RwMemory			stMemoryStream;
	RwChunkHeaderInfo	stHeader;
	RtDict *			pstDict = NULL;

	//@{ 2006/02/14 burumal
	UINT32 uAtomicCount = 0;
	//@}

	SetTexturePath(szTexturePath);

	// 먼저 파일에서 메모리로 올린다.
	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	ApdFile		csApdFile;

	// 파일에서 메모리로 읽어서...
	if(!pPackingManager->OpenFile(szAtomic,&csApdFile))	return NULL;

	stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);

	if (stMemoryStream.length <= FILE_MAX_SIZE)
		stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
	else
		stMemoryStream.start = new BYTE[stMemoryStream.length];

	if (!stMemoryStream.start)
	{
		pPackingManager->CloseFile(&csApdFile);
		return NULL;
	}
	
	if(pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile )!= stMemoryStream.length)	
	{
		pPackingManager->CloseFile(&csApdFile);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	pPackingManager->CloseFile(&csApdFile);
	
	pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
	if (pstStream)
	{
		while (RwStreamReadChunkHeaderInfo(pstStream, &stHeader))
		{
			switch (stHeader.type)
			{
			case rwID_UVANIMDICT:
				{
					LockFrame();
					pstDict = RtDictSchemaStreamReadDict(RpUVAnimGetDictSchema(), pstStream);
					UnlockFrame();
				}
				break;

			case rwID_ATOMIC:
				{
					LockFrame();
					m_bFrameLocked = TRUE;

					RwTexDictionarySetCurrent(m_pstDefaultTexDict);
					RtDictSchemaSetCurrentDict(RpUVAnimGetDictSchema(), pstDict);

					pstAtomic = RpAtomicStreamRead(pstStream);

					m_bFrameLocked = FALSE;
					UnlockFrame();
				}

				break;

			case rwID_CLUMP:
				{
					RpClump *			pstClump;

					LockFrame();
					m_bFrameLocked = TRUE;

	//				if (pstTexDict)
						RwTexDictionarySetCurrent(m_pstDefaultTexDict);
	//				else if (szTexDict)
	//					SetCurrentTexDict(szTexDict);

					RtDictSchemaSetCurrentDict(RpUVAnimGetDictSchema(), pstDict);

					pstClump = RpClumpStreamRead(pstStream);

	//				if (szTexDict || szTexDict)
	//					RwTexDictionarySetCurrent(NULL);

					m_bFrameLocked = FALSE;
					UnlockFrame();
					
					if (pstClump)
					{
						RpAtomic* pAtomicTable[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
						ZeroMemory(pAtomicTable, sizeof(RpAtomic*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);

						RpClumpForAllAtomics(pstClump, CBGetFirstAtomic, &pAtomicTable[0]);

						//@{ 2006/02/14 burumal
						RpAtomic* pTempAtomic = pAtomicTable[0];
						if ( pTempAtomic )
						{
							do {
								pAtomicTable[uAtomicCount++] = pTempAtomic;
								pTempAtomic = pTempAtomic->next;

								if ( !pTempAtomic || (pAtomicTable[0] == pTempAtomic) )
									break;

							} while(true);
						}

						if( uAtomicCount >= D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP )
						{
							uAtomicCount = D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP;
						}
						//@}

						//@{ 2006/04/05 burumal 
					#ifndef USE_MFC
						BOOL bGlyphLock = FALSE;
						if ( m_pcsAgcmGlyph )
							bGlyphLock = TRUE;

						if ( bGlyphLock )
						{
							m_pcsAgcmGlyph->LockDestEnemyTarget();

							if ( m_pcsAgcmGlyph->IsGlyphedFrame(RpClumpGetFrame(pstClump)) )
								m_pcsAgcmGlyph->StopEnemyTarget();
						}
					#endif
						//@}

						LockFrame();

						//@{ 2006/02/14 burumal
						for ( UINT32 uIdx = 0; uIdx < D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP; uIdx++ )
						{							
							if ( pAtomicTable[uIdx] )
							{
								if( pAtomicTable[uIdx]->clump == pstClump )
								{
									RpClumpRemoveAtomic(pstClump, pAtomicTable[uIdx]);
								}

								if ( RpClumpGetFrame(pstClump) != RpAtomicGetFrame(pAtomicTable[uIdx]) )
								{
									RwFrameRemoveChild(RpAtomicGetFrame(pAtomicTable[uIdx]));
								}
								//@{ Jaewon 20050630
								// If such, the frame of the atomic will be invalidated as soon as the clump is destroyed.
								// So set its frame to NULL.
								// This may fix crashes in AgcmCharacter::ReleaseTemplateDefaultFace().
								else
								{
									RpAtomicSetFrame(pAtomicTable[uIdx], NULL);
								}
								//@} Jaewon

								//@{ Jaewon 20050831
								// Set it name to the filename for debugging.
	#ifdef _DEBUG
								RpAtomicSetName(pAtomicTable[uIdx], szAtomic);
	#endif
								//@} Jaewon

								if ( uIdx > 0 )
								{
									pAtomicTable[uIdx]->prev = pAtomicTable[uIdx - 1];
									pAtomicTable[uIdx]->next = pAtomicTable[(uIdx + 1) % uAtomicCount];
								}
								else
								{
									pAtomicTable[0]->prev = pAtomicTable[uAtomicCount - 1];
									pAtomicTable[0]->next = pAtomicTable[1 % uAtomicCount];
								}
							}
						}

						pstAtomic = pAtomicTable[0];
						//@}

						RpClumpDestroy(pstClump);
						
						UnlockFrame();

						//@{ 2006/04/05 burumal
					#ifndef USE_MFC
						if ( bGlyphLock )
							m_pcsAgcmGlyph->UnlockDestEnemyTarget();
					#endif
						//@}
					}
				}

				break;

			default:
				RwStreamSkip(pstStream, stHeader.length);
			}

			if (pstAtomic)
				break;
		}

		RwStreamClose(pstStream, NULL);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;
	}
	else
	{
		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	if (pstAtomic)
	{

	//@{ 2006/02/14 burumal

//		AgcdRLInitAtomicParam	stParam;
//
//		stParam.pstHierarchy	= pstHierarchy;
//		stParam.lFilterMode		= lFilterMode;
//
//		if (pstDict)
//			RpUVAnimDataAtomicSetDict(pstAtomic, pstDict);
//
//		LockFrame();
//
//		//@{ Jaewon 20050525
//		// ;)
//		m_csMutexInitAtomic.Lock();
//		//@} Jaewon
//
//
//		CBInitAtomic(pstAtomic, &stParam);
//
//		//@{ Jaewon 20050525
//		// ;)
//		m_csMutexInitAtomic.Unlock();
//		//@} Jaewon
//		
//		UnlockFrame();

		RpAtomic* pCurAtomic = pstAtomic;
		
		if ( pCurAtomic && pCurAtomic == pCurAtomic->next )
			uAtomicCount = 1;		

		for ( UINT32 uIdx = 0; uIdx < uAtomicCount; uIdx++ )
		{
			AgcdRLInitAtomicParam	stParam;

			stParam.pstHierarchy	= pstHierarchy;
			stParam.lFilterMode		= lFilterMode;

			if (pstDict)
				RpUVAnimDataAtomicSetDict(pCurAtomic, pstDict);

			LockFrame();

			//@{ Jaewon 20050525
			// ;)
			m_csMutexInitAtomic.Lock();
			//@} Jaewon


			CBInitAtomic(pCurAtomic, &stParam);

			//@{ Jaewon 20050525
			// ;)
			m_csMutexInitAtomic.Unlock();
			//@} Jaewon
			
			UnlockFrame();

			pCurAtomic = pCurAtomic->next;
		}
		//@}
	}

	TRACE("AgcmResourceLoader::LoadAtomic() Loaded %s Success\n", szAtomic);

	return pstAtomic;
}

RtAnimAnimation*	AgcmResourceLoader::LoadRtAnim(CHAR*	szName)
{
	RwStream *			pstStream	= NULL;
	RtAnimAnimation *	pstAnim	= NULL;

	RwMemory			stMemoryStream;
		
	// 먼저 파일에서 메모리로 올린다.
	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	ApdFile		csApdFile;

	// 파일에서 메모리로 읽어서...
	if(!pPackingManager->OpenFile(szName,&csApdFile)) return NULL;

	stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);

	if (stMemoryStream.length <= FILE_MAX_SIZE)
		stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
	else
		stMemoryStream.start = new BYTE[stMemoryStream.length];

	if (!stMemoryStream.start)
	{
		pPackingManager->CloseFile(&csApdFile);
		return NULL;
	}
	
	if(pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile )!= stMemoryStream.length)	
	{
		pPackingManager->CloseFile(&csApdFile);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	pPackingManager->CloseFile(&csApdFile);
	
	pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
	if (pstStream)
	{
		if (!RwStreamFindChunk(pstStream, rwID_ANIMANIMATION,
						   (RwUInt32 *)NULL, (RwUInt32 *)NULL))
		{
			if (stMemoryStream.length <= FILE_MAX_SIZE)
				g_csFilePool.Free(stMemoryStream.start);
			else
				delete []stMemoryStream.start;

			return NULL;
		}

		pstAnim = RtAnimAnimationStreamRead(pstStream);

		RwStreamClose(pstStream, NULL);

#ifdef USE_MFC
		//. 2006. 2. 7. Nonstopdj
		//. 모델툴 애니메이션 키프레임조정 및 저장

		vector<string>::iterator iter1 = m_pThis->m_vecSaveList.begin();
		vector<string>::iterator iter2 = m_pThis->m_vecSaveList.end();
		string strPath(szName);
		//. force string make to lower.
		transform(strPath.begin(), strPath.end(), strPath.begin(), tolower);

		iter1 = find(iter1, iter2, strPath);

		if(iter1 == m_pThis->m_vecSaveList.end())
		{
			//. 저장리스트에 목록이 없는 경우에만..
			if(m_pThis->m_iCurrentKeyframeRate > 1)
			{
				RpHAnimConvertKeyFrameRate(pstAnim, m_pThis->m_iCurrentKeyframeRate);
			}
			if(m_pThis->m_bIsSaveAnmfile)
			{
				RtAnimAnimationWrite(pstAnim, szName);
				m_pThis->m_vecSaveList.push_back(strPath);
			};
		}
		else
		{
			MessageBox( NULL, szName, "Already convert KeyFrame or saved.", MB_OK );
		}
#endif

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;
	}
	else
	{
		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	return pstAnim;
}

RpSpline*	AgcmResourceLoader::LoadRpSpline(CHAR*	szName)
{
	RwStream *			pstStream	= NULL;
	RpSpline *			pstSpline	= NULL;

	RwMemory			stMemoryStream;
		
	// 먼저 파일에서 메모리로 올린다.
	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	ApdFile		csApdFile;

	// 파일에서 메모리로 읽어서...
	if(!pPackingManager->OpenFile(szName,&csApdFile)) return NULL;

	stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);

	if (stMemoryStream.length <= FILE_MAX_SIZE)
		stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
	else
		stMemoryStream.start = new BYTE[stMemoryStream.length];

	if (!stMemoryStream.start)
	{
		pPackingManager->CloseFile(&csApdFile);
		return NULL;
	}
	
	if(pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile )!= stMemoryStream.length)
	{
		pPackingManager->CloseFile(&csApdFile);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	pPackingManager->CloseFile(&csApdFile);
	
	pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
	if (pstStream)
	{
		RwUInt32			version;

		if (!RwStreamFindChunk(pstStream, rwID_SPLINE,
						   (RwUInt32 *)NULL, &version))
		{

			if (stMemoryStream.length <= FILE_MAX_SIZE)
				g_csFilePool.Free(stMemoryStream.start);
			else
				delete []stMemoryStream.start;

			return NULL;
		}

		ASSERT(version >= rwLIBRARYBASEVERSION);
		ASSERT(version <= rwLIBRARYCURRENTVERSION);

		if ((version >= rwLIBRARYBASEVERSION) &&
			(version <= rwLIBRARYCURRENTVERSION))
		{
			pstSpline = RpSplineStreamRead(pstStream);
		}

		RwStreamClose(pstStream, NULL);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;
	}
	else
	{
		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete []stMemoryStream.start;

		return NULL;
	}

	return pstSpline;
}

RwTexture *		AgcmResourceLoader::LoadTexture(CHAR *szName, CHAR *szMask, CHAR *szTexDict, RwTexDictionary *pstTexDict, INT32 lFilterMode, CHAR *szTexturePath)
{
	PROFILE("ResourceLoader-LoadTexture");
//	TRACE("AgcmResourceLoader::LoadTexture() Loading %s\n", szName);

	//@{ 2006/09/22 burumal
	if ( szName == NULL || szName[0] == NULL )
		return NULL;
	//@}

	RwStream *	pstStream	= NULL;
	RwTexture *	pstTexture	= NULL;

	SetTexturePath(szTexturePath);

//	if (pstTexDict)
//		RwTexDictionarySetCurrent(pstTexDict);
		RwTexDictionarySetCurrent(m_pstDefaultTexDict);
//	else if (szTexDict)
//		SetCurrentTexDict(szTexDict);
		
	pstTexture = RwTextureRead(szName, szMask);

	// 마고자 레퍼런스 카운트 이상 확인용.

	#ifdef _DEBUG
	if( pstTexture )
	{
		ASSERT( (*pstTexture).refCount > 0 );
	}
	#endif

	if (pstTexture && lFilterMode != -1)
		RwTextureSetFilterMode(pstTexture, (RwTextureFilterMode) lFilterMode);

	return pstTexture;
}

RwTexture *		AgcmResourceLoader::CBGetTextureFromTexDict(RwTexture *pstTexture, PVOID pvData)
{
	RwTexture **	ppstTexture = (RwTexture **) pvData;
	
	//@{ 2006/09/22 burumal
	//ASSERT(!*ppstTexture);
	ASSERT(*ppstTexture);
	if ( *ppstTexture == NULL )
		return NULL;
	//@}

	*ppstTexture = pstTexture;

	return pstTexture;
}

//@{ Jaewon 20051012
#ifdef _DEBUG
#include <sstream>
#endif
//@} Jaewon

RwTexture *		AgcmResourceLoader::LoadTextureFromFile(const CHAR *szName , AcuTextureType eType )
{
	RwTexture *			pstTexture = NULL;
	RwImage *			pstImage = NULL;
	RwStream *			pstStream;
	RwMemory			stMemoryStream;

	//@{ 2006/09/22 burumal
	if ( szName == NULL || szName[0] == NULL )
		return NULL;
	//@}

	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	ASSERT(pPackingManager);

	ApdFile		csApdFile;

	if(!pPackingManager->OpenFile((char*)szName,&csApdFile))
	{	
		//@{ 2006/09/22 burumal
	#ifdef _DEBUG
		char szDbgMsg[256];
		sprintf(szDbgMsg, "Error: Can't find texture File. [%s]\n", szName);
	#endif
		//@}

		return NULL;
	}

	stMemoryStream.length = pPackingManager->GetFileSize(&csApdFile);

	if (stMemoryStream.length <= FILE_MAX_SIZE)
		stMemoryStream.start = (unsigned char *) g_csFilePool.Alloc();
	else
		stMemoryStream.start = new BYTE[stMemoryStream.length];

	if (!stMemoryStream.start)
	{
		pPackingManager->CloseFile(&csApdFile);		
		return FALSE;
	}
	
	if(pPackingManager->ReadFile( stMemoryStream.start, stMemoryStream.length, &csApdFile )!= stMemoryStream.length)
	{
		pPackingManager->CloseFile(&csApdFile);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete [] stMemoryStream.start;		
		
		return FALSE;
	}

	pPackingManager->CloseFile(&csApdFile);
	
	pstStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemoryStream);
	if (pstStream )
	{
		if( ACUTEXTURE_TYPE_TX1 == eType )
		{
			AuMD5Encrypt		csMD5;

			if (csMD5.DecryptString(ACU_TEXTURE_MD5_HASH_STRING, (CHAR *) stMemoryStream.start, stMemoryStream.length))
				pstTexture = LoadTextureFromStream(pstStream, szName, ACUTEXTURE_TYPE_TX1);
		}
		else
		{
			pstTexture = LoadTextureFromStream(pstStream, szName, eType);
		}

		RwStreamClose(pstStream, NULL);

		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete [] stMemoryStream.start;
	}
	else
	{
		if (stMemoryStream.length <= FILE_MAX_SIZE)
			g_csFilePool.Free(stMemoryStream.start);
		else
			delete [] stMemoryStream.start;
		
		return NULL;
	}

	return pstTexture;
}

RwTexture *		AgcmResourceLoader::LoadTextureFromTexDict(RwStream *pstStream, const CHAR *szName)
{
	RwTexture *			pstTexture = NULL;
	RwTexDictionary *	pTexDictionary = NULL;
	RwChunkHeaderInfo	stHeader;

	if (pstStream )
	{
		AgcEngine *pEngine = (AgcEngine *) GetModuleManager();

		while (RwStreamReadChunkHeaderInfo(pstStream, &stHeader))
		{
			switch (stHeader.type)
			{
			case rwID_TEXDICTIONARY:
				//m_pThis->LockFrame();
				pEngine->RwLockTexture();
				pTexDictionary = RwTexDictionaryStreamRead(pstStream);
				pEngine->RwUnlockTexture();
				//m_pThis->UnlockFrame();
				break;

			case rwID_PITEXDICTIONARY:
				//m_pThis->LockFrame();
				pEngine->RwLockTexture();
				pTexDictionary = RtPITexDictionaryStreamRead(pstStream);
				pEngine->RwUnlockTexture();
				//m_pThis->UnlockFrame();
				break;

			default:
				RwStreamSkip(pstStream, stHeader.length);
			}

			if (pTexDictionary)
				break;
		}
	}
	else
	{
		return NULL;
	}

	if (pTexDictionary)
	{
		// Texture Dictionary가 있으면, 거기서 Texture를 가져온다.
		
		//. 2006. 2. 1. Nonstopdj
		//. find하지 않고 직접 link되어 있는 RwTexture pointer를 반환한다.
		//. RwTexDictionaryForAllTextures(pTexDictionary, CBGetTextureFromTexDict, &pstTexture);
		pstTexture = RwTextureDictionaryGetLastTexture(pTexDictionary);

		if (pstTexture)
		{
			//@{ 2006/09/22 burumal
			g_pEngine->RwLockTexture();
			//@}

			RwTexDictionaryRemoveTexture(pstTexture);

			//m_pThis->LockFrame();
			RwTexDictionaryDestroy(pTexDictionary);
			//m_pThis->UnlockFrame();

			//@{ 2006/09/22 burumal
			g_pEngine->RwUnlockTexture();
			//@}
		}
		//@{ Jaewon 20051006
#ifdef _DEBUG
		else
		{
			//@{ 2006/09/22 burumal			
			/*
			std::stringstream output;
			output << "AgcmResourceLoader::LoadTextureFromTexDict() - There is no texture(" << szName << ") in the dictionary!" << std::endl;
			OutputDebugString(output.str().c_str());
			*/
			RwTexDictionaryForAllTextures(pTexDictionary, CBGetTextureFromTexDict, &pstTexture);

			if ( pstTexture )
			{
				g_pEngine->RwLockTexture();
				RwTexDictionaryRemoveTexture(pstTexture);
				RwTexDictionaryDestroy(pTexDictionary);
				g_pEngine->RwUnlockTexture();
			}
			else
			{
				std::stringstream output;
				output << "AgcmResourceLoader::LoadTextureFromTexDict() - There is no texture(" << szName << ") in the dictionary!" << std::endl;
				OutputDebugString(output.str().c_str());
			}
			//@}
		}
#endif
		//@} Jaewon
	}
	//@{ Jaewon 20051006
#ifdef _DEBUG
	else
	{
		OutputDebugString("AgcmResourceLoader::LoadTextureFromTexDict() - No texture dictionary can be found!\n");
	}
#endif
	//@} Jaewon

	return pstTexture;
}

char* szTextureType[ACUTEXTURE_MAX_TYPE] = {
	"",
	".png",
	".dds",
	".tx1",
	".tif",
	".bmp"
};

RwTexture *		AgcmResourceLoader::CBTextureRead(const RwChar *name, const RwChar *maskName)
{
	PROFILE("AgcmResourceLoader::CBTextureRead()");

	RwTexture *pstTexture = NULL;

	// maskName이 있다는 것은 직접 파일에서 읽겠다는 이야기임
	if (maskName && maskName[0])
	{
		pstTexture = m_fnDefaultTextureReadCB(name, maskName);
		if (pstTexture)
			return pstTexture;
	}

	if( strlen( name ) == 0 ) return NULL;

	//if (!strcmp("full_map.png", name))
	//	return NULL;

	//m_pThis->LockRender();

	//if (m_pThis->m_bFrameLocked)
	//	m_pThis->UnlockFrame();

	TEXTURE_REQUEST

#ifdef USE_MFC
	TEXTURE_FAULT
	pstTexture = m_fnDefaultTextureReadCB(name, maskName);
	if (!pstTexture )
#else
	if (m_pThis->m_bUseEncryption)
#endif
	{
		CHAR		szNameTemp[256];
		CHAR		szNameTemp2[256];
		INT32		lIndex;

		strcpy(szNameTemp, name);

		// 확장자를 없에는 코드..
		// 한번만 계산하게 변경
		lIndex = strlen(szNameTemp);
		if ( lIndex > 4 && szNameTemp[ lIndex - 4 ] == '.')
		{
			szNameTemp[ lIndex - 4 ] = 0;
			TEXTURE_HASEXT
		}

		/*
		for (lIndex = strlen(szNameTemp) - 1; lIndex >= 0; --lIndex)
		{
			if (szNameTemp[lIndex] == '.')
			{
				szNameTemp[lIndex] = 0;
				TEXTURE_HASEXT
				break;
			}
		}
		*/

		if (m_pThis->m_pszTexturePath)
		{
			for( int i=ACUTEXTURE_TYPE_PNG; i<ACUTEXTURE_MAX_TYPE; ++i )
			{
				sprintf( szNameTemp2, "%s%s%s", m_pThis->m_pszTexturePath, szNameTemp, szTextureType[i] );
				pstTexture = m_pThis->LoadTextureFromFile(szNameTemp2 , (AcuTextureType)i );
				if( pstTexture )
					break;
			}
		}

		if( !pstTexture )
		{
			for (lIndex = 0; lIndex < m_pThis->m_lDefaultTexturePath; ++lIndex)
			{
				for( int i=ACUTEXTURE_TYPE_PNG; i<ACUTEXTURE_MAX_TYPE; ++i )
				{
					sprintf( szNameTemp2, "%s%s%s", m_pThis->m_aszDefaultTexturePath[lIndex], szNameTemp, szTextureType[i] );
					pstTexture = m_pThis->LoadTextureFromFile(szNameTemp2 , (AcuTextureType)i );
					if( pstTexture )
						break;
				}

				if( pstTexture )
					break;
			}
		}
	}

	if (!pstTexture)
	{
		TEXTURE_FAULT
		pstTexture = m_fnDefaultTextureReadCB(name, maskName);
	}

	//if (m_pThis->m_bFrameLocked)
	//	m_pThis->LockFrame();

	if (pstTexture)
	{
		RwTextureSetName(pstTexture, name);
		if (maskName)
			RwTextureSetMaskName(pstTexture, maskName);
		else
			RwTextureSetMaskName(pstTexture, "");
	}

	//m_pThis->UnlockRender();

	return pstTexture;
}

BOOL			AgcmResourceLoader::WriteAllTexDicts()
{
	RwStream *		pstStream;
	AgcdTexDict **	ppstAgcdTexDict;
	INT32			lIndex = 0;
	CHAR			szPath[256];

	for (ppstAgcdTexDict = (AgcdTexDict **) m_csTexDicts.GetObjectSequence(&lIndex); ppstAgcdTexDict; ppstAgcdTexDict = (AgcdTexDict **) m_csTexDicts.GetObjectSequence(&lIndex))
	{
		if (!*ppstAgcdTexDict)
			continue;

		sprintf(szPath, "%s%s", m_szTexDictPath, (*ppstAgcdTexDict)->m_szName);
		pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, RWSTRING(szPath));
		if (pstStream )
		{
			RwTexDictionaryStreamWrite((*ppstAgcdTexDict)->m_pstTexDict, pstStream);
			RwStreamClose(pstStream, NULL);
		}
	}

	return TRUE;
}


BOOL			AgcmResourceLoader::InstanceAtomic(RpAtomic *pstAtomic)
{
	return TRUE;
	// Camera가 없으면, 당근 안된다.
	RpGeometry *	pstGeometry = RpAtomicGetGeometry(pstAtomic);

	if (!pstGeometry || (RpGeometryGetFlags(pstGeometry) & rpGEOMETRYNATIVE))
		return TRUE;

	/*
	if (!RpSkinGeometryGetSkin(RpAtomicGetGeometry(pstAtomic)))
		return TRUE;
	*/

	LockRender();

	TRACE("Instancing Start.....\n");
	// Camera Begin Update가 안되도 안된다.
	//if (!RwCameraBeginUpdate(m_pCamera))
	//	return FALSE;

	// Instancing
	if (RpSkinGeometryGetSkin(pstGeometry))
	{
		RpAtomicRender(pstAtomic);
		/*
		RxPipeline *				pPipeline;
		RxPipelineNodeInstance *	pNode;

		RpAtomicGetPipeline(pstAtomic, &pPipeline);
		pNode = (RxPipelineNodeInstance *) RxPipelineFindNodeByIndex(pPipeline, 0);

		_rxD3D9SkinInstanceNodeData *	pNodeData = (_rxD3D9SkinInstanceNodeData *) pNode->privateData;
		RxD3D9AllInOneRenderCallBack	pRenderCB = pNodeData->renderCallback;
		RxD3D9AllInOneLightingCallBack	pLightingCB = pNodeData->lightingCallback;

		pNodeData->renderCallback = NULL;
		pNodeData->lightingCallback = NULL;

		RpAtomicInstance(pstAtomic);

		pNodeData->renderCallback = pRenderCB;
		pNodeData->lightingCallback = pLightingCB;
		*/
	}
	else
	{
		RpAtomicInstance(pstAtomic);
	}

	UnlockRender();

	//RwCameraEndUpdate(m_pCamera);
	TRACE("Instancing End.....\n");

	return TRUE;
}

RpAtomic *		AgcmResourceLoader::CBInstanceAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	//INT32			lIndex;
	//BOOL			bIsSkin;
	//BOOL			bLODInstance;
	AgcmResourceLoader *	pThis = (AgcmResourceLoader *) pvData;

	if (!pThis->InstanceAtomic(pstAtomic))
		return NULL;

	/*
	for (lIndex = 1; lIndex < RPLODATOMICMAXLOD; ++lIndex)
	{
		if (!RpLODAtomicSetCurrentLOD(pstAtomic, lIndex))
			continue;

		if((bIsSkin) && (!AcuObject_RwUtilAtomicHasSkin(pstAtomic)))
			bLODInstance = FALSE;
		else
			bLODInstance = TRUE;

		if(bLODInstance)
		{
			if (!InstanceAtomic(pstAtomic))
				return NULL;
		}
	}
	*/

	return pstAtomic;
}

BOOL			AgcmResourceLoader::InstanceClump(RpClump *pstClump)
{
	if (!RpClumpForAllAtomics(pstClump, CBInstanceAtomic, this))
		return FALSE;

	return TRUE;
}

BOOL			AgcmResourceLoader::SetCallbackLoadEntryEmpty(ApModuleDefaultCallBack pfnCallback, PVOID pvClass)
{
	return SetCallback(AGCM_RL_CB_POINT_LOADEND, pfnCallback, pvClass);
}

RpHAnimHierarchy *	AgcmResourceLoader::GetHierarchy(RpClump *pstClump)
{
	RpHAnimHierarchy *pstHierarchy = NULL;

	RwFrameForAllChildren(RpClumpGetFrame(pstClump), GetChildFrameHierarchy, (void *)&pstHierarchy);

	return pstHierarchy;
}

VOID			AgcmResourceLoader::SetTexturePath(CHAR *szTexturePath)
{
	m_pszTexturePath = szTexturePath;
//	m_szTexturePath[0] = 0;
//	if (szTexturePath)
//		strncat(m_szTexturePath, szTexturePath, AGCM_RESOURCE_PATH_LENGTH - 1);
}

BOOL			AgcmResourceLoader::AddDefaultTexturePath(CHAR *szTexturePath)
{
	if (m_lDefaultTexturePath >= AGCM_RESOURCE_DEFAULT_TEXTURE_PATH_NUM || !szTexturePath)
		return FALSE;

	m_aszDefaultTexturePath[m_lDefaultTexturePath][0] = 0;

	strncat(m_aszDefaultTexturePath[m_lDefaultTexturePath], szTexturePath, AGCM_RESOURCE_PATH_LENGTH - 1);

	++m_lDefaultTexturePath;

	return TRUE;
}

RwFrame *		AgcmResourceLoader::GetChildFrameHierarchy(RwFrame *frame, PVOID pvData)
{
	RpHAnimHierarchy **ppstHierarchy = (RpHAnimHierarchy **) pvData;

	*ppstHierarchy = RpHAnimFrameGetHierarchy(frame);
	if( *ppstHierarchy == NULL )
	{
		// Recursive하게 FrameHierarchy 가져온다.
		RwFrameForAllChildren(frame, GetChildFrameHierarchy, pvData);

		return frame;
	}

	return NULL;
}

RpAtomic *		AgcmResourceLoader::CBInitAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	if(!pstAtomic) return NULL;

	AgcdRLInitAtomicParam *	stParam = (AgcdRLInitAtomicParam *) pvData;
	INT32					lIndex;
	INT32					lIndex2;
	INT32					lLODIndex;
	RpGeometry *			pstGeometry;
	RpClump *				pstClump;

	// Clump에 붙어있다면, Atomic이 Frame을 가지도록 한다.
	pstClump = RpAtomicGetClump(pstAtomic);
	if (pstClump)
	{
		RwFrame*	pClumpFrame = RpClumpGetFrame(pstClump);
		ASSERT( pClumpFrame );

		if(pClumpFrame && RpAtomicGetFrame(pstAtomic) == pClumpFrame)
		{
			RwFrame *	pstFrame = RwFrameCreate();
			RpAtomicSetFrame(pstAtomic, pstFrame);

			RwFrameAddChild(pClumpFrame, pstFrame);
		}
	}

	// 일단 LOD Unhook 시킨다.
	RpLODAtomicUnHookRender(pstAtomic);

	// 현재 LOD Index를 가져온다. RW에서 나머지 LOD와 저장하는 방식이 다르다... 이 넘은 GeometryDestroy하면 안된다...물론 RW Bug다.
	lLODIndex = RpLODAtomicGetCurrentLOD(pstAtomic);

	// Split Skin 하기 위해서 Hierarchy Set
	if (stParam->pstHierarchy)
		RpSkinAtomicSetHAnimHierarchy(pstAtomic, stParam->pstHierarchy);

	if (RpAtomicGetGeometry(pstAtomic) && RpSkinGeometryGetSkin(RpAtomicGetGeometry(pstAtomic)))
	{
		if (AgcmResourceLoader::m_pThis->m_lMaxNumBones && RtSkinSplitAtomicSplitGeometry(pstAtomic, m_pThis->m_lMaxNumBones))
		{
			TRACE("AgcmResourceLoader::CBInitAtomic() Splitting Skin\n");
		}
	}
	else
	{
		//. 2006. 1. 17. Nonstopdj
		pstAtomic->skinSplitFlags = rpATOMICSKINSPLITNOTUSE;
	}

	// LOD Geometry들에 대해서
	RwUInt32 numBonesInLod0=0;
	for (lIndex = 0; lIndex < RPLODATOMICMAXLOD; ++lIndex)
	{
		pstGeometry = RpLODAtomicGetGeometry(pstAtomic, lIndex);

		if (!pstGeometry)
			continue;

		//. 2006. 1. 17. Nonstopdj
		//. geometry lock이 안걸린 몇몇 함수(RpGeometryForAllMaterials, RpLODAtomicSetCurrentLOD, 
		//. RpSkinGeometryGetSkin, RpLODAtomicSetGeometry)들이 있으므로.
		//#ifdef _DEBUG
		//RpGeometryCSLock(pstGeometry);
		//#endif

		// 만약 Filter Mode를 바꿀거면 여기서 바꾸고
		if (stParam->lFilterMode != -1)
			RpGeometryForAllMaterials(pstGeometry, CBFilterModeMaterial, (PVOID) stParam->lFilterMode);

		RpLODAtomicSetCurrentLOD(pstAtomic, lIndex);

		// 일단, Morph Target을 다 없앤다. 왜냐하면, MorphTarget이 2개이상 있으면, DynamicGeometry를 사용하는데 이쪽에 버그가 많다. (RW버그다.. 당근 -_-;)
		// 나중에 이쪽 버그 다 없어지면, 그때, 이코드는 빼도 된다.
		for (lIndex2 = RpGeometryGetNumMorphTargets(pstGeometry) - 1; lIndex2 > 0; --lIndex2)
		{
			RpGeometryRemoveMorphTarget(pstGeometry, lIndex2);
		}

		// 이거는 RW Bug때문에 넣은 코드... 왜 LOD Geometry들은 RefCount가 1만큼 더 높냐고... -_-;
		if (lIndex != lLODIndex)
		{
			while (pstGeometry->refCount > 2)
				RpGeometryDestroy(pstGeometry);
		}

		// Skin이 없으면 SplitSkin 할 필요 없으니 continue
		RpSkin *pSkin = RpSkinGeometryGetSkin(pstGeometry);
		if (!pSkin)
		{
			//. 2006. 1. 17. Nonstopdj
			//#ifdef _DEBUG
			//RpGeometryCSUnLock(pstGeometry);
			//#endif
			continue;
		}

		//@{ Jaewon 20051123
		// Crash prevention for inconsistent bone numbers in LODs
		if (lIndex == 0)
			numBonesInLod0 = RpSkinGetNumBones(pSkin);
		else
		{
			RwUInt32 numBones = RpSkinGetNumBones(pSkin);
			if (numBonesInLod0 != numBones)
			{
				ASSERT(!"Inconsistent bone numbers in LODs!!!");
				for (lIndex2 = lIndex; lIndex2 < RPLODATOMICMAXLOD; ++lIndex2)
					RpLODAtomicSetGeometry(pstAtomic, lIndex2, NULL);
				break;
			}
		}
		//@} Jaewon

		// SplitSkin을 한다.. VShader를 쓸 수 있다면 말이다.
		// SplitSkin도 DynamicGeometry를 사용하지 않기 위해서 사용한다.
		if (AgcmResourceLoader::m_pThis->m_lMaxNumBones && RtSkinSplitAtomicSplitGeometry(pstAtomic, m_pThis->m_lMaxNumBones))
		{
			TRACE("AgcmResourceLoader::CBInitAtomic() Splitting Skin\n");
		}

		//. 2006. 1. 17. Nonstopdj
		//#ifdef _DEBUG
		//RpGeometryCSUnLock(pstGeometry);
		//#endif
	}

	// Default는 0레벨로
	RpLODAtomicSetCurrentLOD(pstAtomic, 0);

	// PartID 설정
	pstGeometry	= RpAtomicGetGeometry(pstAtomic);
	if (pstGeometry)
	{
		INT32	lPartID	= m_pThis->GetGeometryUsrDataArrayInt(pstGeometry, "PartID");
		if (lPartID)
			pstAtomic->iPartID	= lPartID;
	}

	if (stParam->pstHierarchy)
		RpSkinAtomicSetHAnimHierarchy(pstAtomic, NULL);

	return pstAtomic;
}


RpMaterial *	AgcmResourceLoader::CBFilterModeMaterial(RpMaterial *pstMaterial, PVOID pvData)
{
	if (RpMaterialGetTexture(pstMaterial))
		RwTextureSetFilterMode(RpMaterialGetTexture(pstMaterial), (RwTextureFilterMode) (INT32) pvData);

	return pstMaterial;
}

INT32			AgcmResourceLoader::GetAtomicUsrDataArrayInt(RpAtomic *pstAtomic, CHAR *szUsrDataArrayName)
{
	RpGeometry	*pstGeom				= RpAtomicGetGeometry(pstAtomic);

	return GetGeometryUsrDataArrayInt(pstGeom, szUsrDataArrayName);
}

INT32			AgcmResourceLoader::GetGeometryUsrDataArrayInt(RpGeometry *pstGeometry, CHAR *szUsrDataArrayName)
{
	RwInt32		lRt						= 0;

	if(pstGeometry)
	{
		RwInt32 nUsrDatNum				= RpGeometryGetUserDataArrayCount(pstGeometry);
		RpUserDataArray	*pstUserDataArray;
		RwChar			*pszUserDataArrayName;

		for(RwInt32 nCount = 0; nCount < nUsrDatNum; ++nCount)
		{
			pstUserDataArray			= RpGeometryGetUserDataArray(pstGeometry, nCount);
			if(pstUserDataArray)
			{
				pszUserDataArrayName	= RpUserDataArrayGetName(pstUserDataArray);
				if(!strcmp(pszUserDataArrayName, szUsrDataArrayName))
				{
					lRt = RpUserDataArrayGetInt(pstUserDataArray, 0);
					break;
				}
			}
		}
	}

	return lRt;
}

RwTexture *		AgcmResourceLoader::LoadTextureFromStream(RwStream *pstStream, const CHAR *szName, AcuTextureType eType)
{
	RwTexture		*texture = NULL;
	RwRaster		*raster = NULL;
	INT32			rasterWidth = 0;
	INT32			rasterHeight = 0;
	INT32			rasterDepth = 0;
	INT32			rasterFlags = 0;

	if( eType == ACUTEXTURE_TYPE_DDS )
	{
		raster = RwD3D9RasterStreamReadDDS(pstStream);
		if( !raster )	return NULL;

	}
	else if( eType == ACUTEXTURE_TYPE_TX1 )
	{
		return LoadTextureFromTexDict( pstStream, szName );
	}
	else
	{
		RwImage* image = NULL;
		INT32 format;
		switch( eType )
		{
		case ACUTEXTURE_TYPE_PNG:
			format = rwRASTERTYPETEXTURE | rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP ;
			image = RtPNGImageReadStream( pstStream );
			break;

		case ACUTEXTURE_TYPE_BMP:
			format  = rwRASTERTYPETEXTURE | rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP;
			image = RtBMPImageReadStream( pstStream );
			break;

		case ACUTEXTURE_TYPE_TIF:
			format = rwRASTERTYPETEXTURE | rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP;
			image = RtTIFFImageReadStream( pstStream, szName );
			break;
		}

		if( !image )		return NULL;

		if( eType != ACUTEXTURE_TYPE_PNG )
		{
			if( !RwImageFindRasterFormat( image, format, &rasterWidth, &rasterHeight, &rasterDepth, &rasterFlags ) )
			{
				RwImageDestroy(image);
				return NULL;
			}

			/* do we need to resample? */
			if ((RwImageGetWidth(image) != rasterWidth) || (RwImageGetHeight(image) != rasterHeight))
			{
				RwImage			*resampledImage;
				RwImage			*origImage;
				RwInt32			 originalDepth;

				/* and if we do, we'll need to expand up to 32bpp first */
				originalDepth = RwImageGetDepth(image);
				if (originalDepth != 32)
				{
					/* we'll reuse this later */
					origImage = image;

					image = RwImageCreate(RwImageGetWidth(origImage), RwImageGetHeight(origImage), 32);
					if (!image)
					{
						RwImageDestroy(origImage);
						return NULL;
					}
					if (!RwImageAllocatePixels(image))
					{
						RwImageDestroy(image);
						RwImageDestroy(origImage);
						return NULL;
					}
					RwImageCopy(image, origImage);
					RwImageDestroy(origImage);
				}

				resampledImage = RwImageCreate(rasterWidth, rasterHeight, 32);
				if (!resampledImage)
				{
					RwImageDestroy(image);
					return NULL;
				}
				if (!RwImageAllocatePixels(resampledImage))
				{
					RwImageDestroy(resampledImage);
					RwImageDestroy(image);
					return NULL;
				}

				RwImageResample(resampledImage, image);
				RwImageDestroy(image);
				image = resampledImage;

				/* If original image depth was 4 or 8, RE-PALLETIZE
				if (4 == originalDepth)
				{
					PalettizeImage(&image, originalDepth);
				}
				else if (8 == originalDepth)
				{
					PalettizeImage(&image, originalDepth);
				}
				*/
			}

			/* Create a raster */
			raster = RwRasterCreate(rasterWidth, rasterHeight, rasterDepth, rasterFlags);
		}
		else
		{
			rasterWidth = RwImageGetWidth(image);
			rasterHeight = RwImageGetHeight(image);
			rasterDepth = RwImageGetDepth(image);

			raster = RwRasterCreate(rasterWidth, rasterHeight, rasterDepth, rwRASTERTYPETEXTURE);
		}

		if (!raster)
		{
			RwImageDestroy(image);
			return NULL;
		}

		/* create a palette
		if (RwRasterGetFormat(raster) &
			(rwRASTERFORMATPAL4 | rwRASTERFORMATPAL8))
		{
			if (RwRasterGetFormat(raster) & rwRASTERFORMATPAL4)
			{
				PalettizeMipmaps((RwRGBA *)MipPalette, (RwImage *)NULL, &image, 1, 4);
			}
			else
			{
				PalettizeMipmaps((RwRGBA *)MipPalette, (RwImage *)NULL, &image, 1, 8);
			}
			RwImageSetPalette(image, MipPalette);
		}
		*/

		RwImageGammaCorrect(image);

		/* Convert the image into the raster */
		if (!RwRasterSetFromImage(raster, image))
		{
			RwRasterDestroy(raster);
			RwImageDestroy(image);
			return NULL;
		}

		RwImageDestroy(image);
	}

	/* Create a texture */
	texture = RwTextureCreate(raster);
	if (!texture)
	{
		RwRasterDestroy(raster);
		return NULL;
	}

	/* The name [and maskname] */
	RwTextureSetName(texture, szName);
	RwTextureSetMaskName(texture, "");

	return texture;
}

BOOL	AgcmResourceLoader::FindLoadEntry(INT32 lLoaderIndex, PVOID pvData1, PVOID pvData2)
{
	// 리소스엔트리에 올라가 있는지 확인한다.
	AuNode <AgcdLoaderEntry *> *	pNode;
	AgcdLoader *					pstLoader = m_astLoaderInfo + lLoaderIndex;
	AgcdLoaderEntry *				pstEntry;
	BOOL							bFound = FALSE;

	m_csMutexLoader.Lock();

	pNode = m_listQueue.GetHeadNode();
	while (pNode)
	{
		pstEntry = pNode->GetData();

		if (pstEntry->m_lLoaderID	== lLoaderIndex	&& 
			pstEntry->m_pvData1		== pvData1		&& 
			pstEntry->m_pvData2		== pvData2		)
		{
			bFound = TRUE;
			break;
		}

		pNode = pNode->GetNextNode();
	}

	m_csMutexLoader.Unlock();

	return bFound;
}

BOOL			AgcmResourceLoader::_LoadColor( VOID )
{
	AuIniManagerA		ColorINI;
	CHAR				szTemp[ 256 ];
	string				strKeyName;
	DWORD				szValue			=	0;
	INT					nStrLength		=	0;
	INT					nSectionCount	=	0;
	INT					nKeyCount		=	0;

	ColorINI.SetPath( "INI\\ArchlordColor.ini" );
	ColorINI.ReadFile( 0 , TRUE );

	nSectionCount		=	ColorINI.GetNumSection();

	if( !nSectionCount )
		return FALSE;

	// 섹션마다 키값을 검색해서 Color값을 저장한다
	for( INT i = 0 ; i < nSectionCount ; ++i )
	{
		ZeroMemory( szTemp , 256 );
		memset( szTemp , 'f' , 2 );
		strKeyName.clear();
		nKeyCount	=	0;

		// 섹션의 키값에서 데이타를 뽑아온다
		nKeyCount	=	ColorINI.GetNumKeys( i );
		for( INT k = 0 ; k < nKeyCount ; ++k )
		{
			// 키값 이름을 뽑아온다
			strKeyName	=	ColorINI.GetKeyName( i , k );

			// 데이타를 뽑아온다
			ColorINI.GetStringValue( i , k , (szTemp+2) , 254 , NULL );

			// 문자열16진수를 DWORD형태로 변경한다
			szValue		=	_HexStringToDWORD( szTemp );
			m_mapArchlordColor.insert( make_pair(strKeyName , szValue) );
		}

	}

	return TRUE;
}

DWORD			AgcmResourceLoader::_HexStringToDWORD( const char* szHexString )
{
	if( !szHexString )
		return 0;

	DWORD	dwValue		=	0;
	DWORD	dwTotal		=	0;
	DWORD	dwMulti		=	0;
	INT		nLength		=	strlen( szHexString );

	for( INT i = nLength-1 ; i >= 0 ; --i )
	{
		dwMulti		=	1;

		// Ascii Code 의 값을 빼서 숫자로 만든다
		if( szHexString[ i ] >= 'A' && szHexString[ i ] <= 'F' )
			dwValue		=	szHexString[ i ] - 55;

		else if( szHexString[ i ] >= 'a' && szHexString[ i ] <= 'f' )
			dwValue		=	szHexString[ i ] - 87;

		else if( szHexString[ i ] >= '0' && szHexString[ i ] <= '9' )
			dwValue		=	szHexString[ i ] - 48;

		// 승을 구해준다
		for( INT k = 0 ; k < nLength - 1 - i  ; ++k )
			dwMulti	*= 16;

		dwTotal +=	( dwValue * dwMulti );
	}

	return dwTotal;
}

DWORD		AgcmResourceLoader::GetColor( const char* szName )
{
	mapColorIter	Iter = m_mapArchlordColor.find( szName );

	if( Iter == m_mapArchlordColor.end() )
		return 0;

	return Iter->second;
}