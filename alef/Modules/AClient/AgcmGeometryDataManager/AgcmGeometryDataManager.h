#ifndef __AGCM_GEOMETRY_DATA_MANAGER_H__
#define __AGCM_GEOMETRY_DATA_MANAGER_H__

#if _MSC_VER < 1300
#ifdef _DEBUG
#pragma comment (lib , "AgcmGeometryDataManagerD")
#else
#pragma comment (lib , "AgcmGeometryDataManager")
#endif // _DEBUG
#endif

#include "ApModule.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmObject.h"

#include "AgcdGeometryDataManager.h"

#define AGCM_GDM_MAX_KEY_ARRAY				10
#define AGCM_GDM_MAX_KEY_BUFFER				64
#define AGCM_GDM_INI_NAME_DATA				"GEOM_DATA"

typedef enum
{
	E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT	= 0,
	E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER,
	E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM,
	E_AGCM_GEOM_DATA_STREAM_TYPE_NUM
} eAgcmGeomDataStreamType;

typedef enum
{
	E_AGCM_GEOM_DATA_MERGE_FLAGS_ADD			= 1,
	E_AGCM_GEOM_DATA_MERGE_FLAGS_SRC1,
	E_AGCM_GEOM_DATA_MERGE_FLAGS_SRC2,
	E_AGCM_GEOM_DATA_MERGE_FLAGS_SKIP,
} eAgcmGeomDataMergeFlags;

class AgcmGeometryDataAdmin
{
public:
	AgcmGeometryDataAdmin();
	virtual ~AgcmGeometryDataAdmin() {}

public:
	VOID					SetNumAllocGeom(UINT8 unNum);
	BOOL					CreateGeometryLink(AgcdGeometryFrame *pcsFrame);

	AcuGeometry				*AddGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, AcuGeometry *pstGeom = NULL);
	AcuGeometry				*AddGeometry(AgcdGeometryDataLink *pcsLink, AcuGeometry *pstGeom = NULL);
	BOOL					RemoveGeometry(AgcdGeometryData *pcsHead, INT32 lRemIndex = -1);
	VOID					RemoveAllGeometry(AgcdGeometryDataLink *pcsLink);
	BOOL					RemoveAllGeometry(AgcdGeometryFrame *pcsFrame);

protected:
	UINT8					m_unNumAllocGeom;
};

class AgcmGeometryDataManager : public ApModule
{
public:
	AgcmGeometryDataManager();
	virtual ~AgcmGeometryDataManager() {}

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

protected:
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmItem				*m_pcsAgcmItem;
	AgcmObject				*m_pcsAgcmObject;

	AgcmGeometryDataAdmin	m_csAdmin;

	UINT8					m_unNumAttachedData;
	CHAR					*m_paszKey[AGCM_GDM_MAX_KEY_ARRAY];

	INT32					m_lAttachedDataIndexChar;
	INT32					m_lAttachedDataIndexItem;
	INT32					m_lAttachedDataIndexObjt;

public:
	INT32					AttachData(CHAR *szKey);

	AcuGeometry				*AddGeometry(AgcdGeometryFrame *pcsFrame, CHAR *szKey, AcuGeometry *pstGeom = NULL);
	AcuGeometry				*AddGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, AcuGeometry *pstGeom = NULL);
	AcuGeometry				*AddGeometry(AgcdGeometryDataLink *pcsLink, AcuGeometry *pstGeom = NULL);

	AcuGeometry				*GetGeometry(AgcdGeometryFrame *pcsFrame, CHAR *szKey, INT32 lIndex, BOOL bCreate = FALSE);
	AcuGeometry				*GetGeometry(AgcdGeometryFrame *pcsFrame, UINT8 unKey, INT32 lIndex, BOOL bCreate = FALSE);

	AgcdGeometryDataLink	*GetGeometryDataLink(ApBase *pcsObj, CHAR *szKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(ApBase *pcsObj, UINT8 unKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(AgpdCharacterTemplate *pcsObj, CHAR *szKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(AgpdCharacterTemplate *pcsObj, UINT8 unKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(AgpdItemTemplate *pcsObj, CHAR *szKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(AgpdItemTemplate *pcsObj, UINT8 unKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(ApdObjectTemplate *pcsObj, CHAR *szKey);
	AgcdGeometryDataLink	*GetGeometryDataLink(ApdObjectTemplate *pcsObj, UINT8 unKey);

	AgcdGeometryFrame		*GetGeometryFrame(ApBase *pcsObj);
	AgcdGeometryFrame		*GetGeometryFrame(AgpdCharacterTemplate *pcsObj);
	AgcdGeometryFrame		*GetGeometryFrame(AgpdItemTemplate *pcsObj);
	AgcdGeometryFrame		*GetGeometryFrame(ApdObjectTemplate *pcsObj);

	BOOL					ResetGeometryList(ApBase *pcsObj, CHAR *szKey);
	BOOL					ResetGeometryList(AgcdGeometryFrame *pcsFrame, CHAR *szKey);
	BOOL					ResetGeometryList(AgcdGeometryData **ppcsHead);

	BOOL					WriteObjGeomDataFile(CHAR *szPath);
	BOOL					WriteCharGeomDataFile(CHAR *szPath);
	BOOL					WriteItemGeomDataFile(CHAR *szPath);

	BOOL					ReadObjGeomDataFile(CHAR *szPath);
	BOOL					ReadCharGeomDataFile(CHAR *szPath);
	BOOL					ReadItemGeomDataFile(CHAR *szPath);

	BOOL					CopyGeomFrame(AgcdGeometryFrame *pcsDestFrame, AgcdGeometryFrame *pcsSrcFrame);
	INT32					FindSectionIndex(INT32 lFindSection, INT32 *alSections, INT32 lNumSections, INT32 lStart = 0);
	BOOL					MergeGeomDataFile(CHAR *szDestPath, CHAR *szSrcPath1, CHAR *szSrcPath2);
	
protected:
	VOID					InitializeVariable();
	UINT8					FindKeyIndex(CHAR *szKey);
	BOOL					SetGeomData(CHAR *szBuffer, AgcdGeometryFrame *pcsFrame);

	BOOL					ReleaseGeometryFrame(AgcdGeometryFrame *pcsFrame);

	UINT8					GetAttachedDataNum();
	CHAR					*GetAttachedDataName(UINT8 unIndex);

	static BOOL				StreamWriteGeoemtryDataCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				StreamReadGeoemtryDataCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				GeometryFrameDestructCB(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // __AGCM_GEOMETRY_DATA_MANAGER_H__