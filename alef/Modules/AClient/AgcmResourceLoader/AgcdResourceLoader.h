#ifndef	_AGCDRESOURCELOADER_H_
#define _AGCDRESOURCELOADER_H_

#include "ApBase.h"
#include "rwcore.h"
#include "ApMemory.h"

#define AGCD_RESOURCE_TEX_DICT_NAME_LEN		64
typedef struct
{
	CHAR				m_szName[AGCD_RESOURCE_TEX_DICT_NAME_LEN];
	RwTexDictionary *	m_pstTexDict;

} AgcdTexDict;

struct AgcdLoaderEntry : public ApMemory<AgcdLoaderEntry, 10000> 
{
	INT32	m_lLoaderID;
	PVOID	m_pvData1;
	PVOID	m_pvData2;
	BOOL	m_bRemoved;
	UINT32	m_ulEntryTime;		// Added Time
	UINT32	m_ulLoaderTime;		// Loader CB Start Time
	UINT32	m_ulDoneTime;		// Done CB Start Time
};

typedef struct
{
	ApModuleDefaultCallBack			m_fnLoadCallback;
	ApModuleDefaultCallBack			m_fnDoneCallback;
	PVOID							m_pvClass;
//	AuList < AgcdLoaderEntry * >	m_listQueue;
	ApMutualEx						m_csMutex;
} AgcdLoader;

typedef enum
{
	AGCD_RESOURCE_CLUMP,
	AGCD_RESOURCE_ATOMIC,
	AGCD_RESOURCE_TEXTURE
} AgcdResourceType;

#endif // _AGCDRESOURCELOADER_H_
