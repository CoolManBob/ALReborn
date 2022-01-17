#ifndef __AGCM_OBJECT_LIST_H__
#define __AGCM_OBJECT_LIST_H__

#include "AgcdObject.h"
#include "ApMemoryPool.h"

class AgcmObjectList
{
protected:
	ApMemoryPool				m_csGroupPool;
//	AgcdAnimObjectGroupDataList	*m_pstAgcdAnimObjectGroupDataList;

public:
	AgcmObjectList();
	virtual ~AgcmObjectList();

protected:
	BOOL						CreateObjectGroup(AgcdObjectGroupData *pstData);
	BOOL						CreateObjectTemplateGroup(AgcdObjectTemplateGroupData *pstData);

public:	
	// Animation
//	BOOL						RemoveAllAnimObjectGroupData();
//	BOOL						AddAnimObjectGroupData(AgcdObjectGroupData *pcsData);
//	BOOL						RemoveAnimObjectGroupData(AgcdObjectGroupData *pcsData);
//	BOOL						CheckAnimObjectGroupData(AgcdObjectGroupData *pcsData);
//	AgcdAnimObjectGroupDataList	*GetObjectGroupDataAnimList()		{return m_pstAgcdAnimObjectGroupDataList;}

	// Object-Group
	BOOL						RemoveAllObjectGroup(AgcdObjectGroup *pstGroup);
	AgcdObjectGroupData			*AddObjectGroup(AgcdObjectGroup *pstGroup/*, AgcdObjectGroupData *pcsData = NULL*/);
//	BOOL						RemoveObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex);
	AgcdObjectGroupData			*GetObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex);

	// ObjectTemplate-Group
	BOOL						RemoveAllObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup);
	AgcdObjectTemplateGroupData	*AddObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup/*, AgcdObjectTemplateGroupData *pcsData = NULL*/);
	BOOL						RemoveObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex);
	AgcdObjectTemplateGroupData	*GetObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex);
//	BOOL						CopyObjectTemplateGroup(AgcdObjectTemplateGroup *pcsDest, AgcdObjectTemplateGroup *pcsSrc);
};

#endif // __AGCM_OBJECT_LIST_H__
