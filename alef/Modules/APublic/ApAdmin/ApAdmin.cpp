
/******************************************************************************
Modle:  ApAdmin.cpp
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2002. 04. 08
******************************************************************************/

#include "ApAdmin.h"

ApMutualEx	g_csGlobalAdminMutex;

ApAdmin::ApAdmin()
{
	m_nObjectSize	= 0;
	m_lObjectCount	= 0;
	
	m_nIdentity		= 0;
	m_pvClass		= NULL	;
	m_pfConstructor	= NULL	;
	m_pfDestructor	= NULL	;
}

ApAdmin::~ApAdmin()
{
}

//		InitializeObject
//	Functions
//		- set object size
//		- memory allocation (Object, Mutex)
//		- initialize B+tree
//	Arguments
//		- nObjectSize : object data size
//		- lObjectCount : # of object
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::InitializeObject(INT16 nObjectSize, INT32 lObjectCount, 
							   ApAdminDefaltCallBack pfConstructor, ApAdminDefaltCallBack pfDestructor, PVOID pvClass, INT16 nID)
{
	BOOL	bRetVal = TRUE;

	if ((bRetVal = m_csObject.Initialize(lObjectCount, nObjectSize)) == FALSE)
	{
		// memory allocation error
	}
	else
	{
		m_nObjectSize = nObjectSize;
		m_lObjectCount = lObjectCount;
	}

	ClearHashMapINT32();
	ClearHashMapstring();

	m_pfConstructor = pfConstructor;
	m_pfDestructor = pfDestructor;
	m_pvClass = pvClass;
	m_nIdentity = nID;

	return bRetVal;
}

INT32	ApAdmin::GetHashMapINT32(INT_PTR nKey)
{
	HashMap_INT32::iterator iter = m_HashMapINT32.find(nKey);

	if (iter != m_HashMapINT32.end())
		return (*iter).second;
	else return 0;
}

INT32	ApAdmin::GetHashMapstring(CHAR* szKey)
{
	if (NULL == szKey)	return 0;	

	// Lock 추가.
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return 0;

	HashMap_string::iterator iter = m_HashMapstring.find(szKey);
	if (iter != m_HashMapstring.end())
		return (*iter).second;
	else return 0;
}

BOOL	ApAdmin::SetHashMapINT32(INT_PTR nKey, INT32 lIndex)
{
	if (lIndex < 0)		return FALSE;

	m_HashMapINT32.insert(HashMap_INT32::value_type(nKey, lIndex));

	return TRUE;
}

BOOL	ApAdmin::SetHashMapstring(CHAR* szKey, INT32 lIndex)
{
	if (NULL == szKey)	return FALSE;	
	if (lIndex < 0)	return FALSE;

	m_HashMapstring.insert(HashMap_string::value_type(szKey, lIndex));

	return TRUE;
}

BOOL	ApAdmin::RemoveHashMapINT32(INT_PTR nKey)
{
	m_HashMapINT32.erase(nKey);

	return TRUE;
}

BOOL	ApAdmin::RemoveHashMapstring(CHAR* szKey)
{
	if (NULL == szKey)	return FALSE;	

	m_HashMapstring.erase(szKey);

	return TRUE;
}

VOID	ApAdmin::ClearHashMapINT32()
{
	m_HashMapINT32.clear();
}

VOID	ApAdmin::ClearHashMapstring()
{
	m_HashMapstring.clear();
}

//		AddObject
//	Functions
//		- add object data in m_pObject
//		- insert object data in B+tree (INT32 key)
//	Arguments
//		- pObject : object data
//		- nKey : B+tree key value
//	Return value
//		- PVOID : object data pointer into Btree
///////////////////////////////////////////////////////////////////////////////

PVOID ApAdmin::AddObject(PVOID pObject, INT_PTR nKey)
{
	INT32	lIndex;
	PVOID	pRetval = NULL;

	if (!pObject || nKey == 0)
	{
		SetLastError( AEC_PARAMETER_ERROR );
		return NULL;
	}

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	if (GetHashMapINT32(nKey))
	{
		SetLastError( AEC_KEY_ALEREADY_EXIST );
		return NULL;
	}

	// insert m_pObject array
	lIndex = m_csObject.Add(pObject);

	if (!SetHashMapINT32(nKey, lIndex))
	{
		if( m_csObject.m_lDataCount >= m_csObject.m_lArraySize )
			SetLastError( AEC_OBJECT_COUNT_LIMITED );
		else
			SetLastError( AEC_INSERT_FAILED );
		return NULL;
	}

	pRetval = m_csObject.GetData(lIndex);

	if (m_pfConstructor)
	{
		m_pfConstructor(pRetval, m_pvClass, m_nIdentity);
	}

	return pRetval;
}

//		AddObject
//	Functions
//		- add object data in m_pObject
//		- insert object data in B+tree (string key)
//	Arguments
//		- pObject : object data
//		- szKey : B+tree key value
//	Return value
//		- PVOID : object data pointer into Btree
///////////////////////////////////////////////////////////////////////////////
PVOID ApAdmin::AddObject(PVOID pObject, CHAR* szKey)
{
	INT32	lIndex;
	PVOID	pRetval = NULL;

	if (pObject == NULL || szKey == NULL)	return NULL;
	
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	if (GetHashMapstring(szKey))
		return NULL;

	// insert m_pObject array
	lIndex = m_csObject.Add(pObject);
	if (lIndex == -1)
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "ApAdmin::AddObject() Adding Object Error(Buffer Full!!!)\n");
		AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
#endif
	}

	if (!SetHashMapstring(szKey, lIndex)) return NULL;

	pRetval = m_csObject.GetData(lIndex);

	if (m_pfConstructor)
	{
		m_pfConstructor(pRetval, m_pvClass, m_nIdentity);
	}

	return pRetval;
}

//		AddObject
//	Functions
//		- add object data in m_pObject
//		- insert object data in B+tree (INT32, string key)
//	Arguments
//		- pObject : object data
//		- nKey1 : B+tree key value
//		- szKey2 : B+tree Key value
//	Return value
//		- PVOID : object data pointer into Btree
///////////////////////////////////////////////////////////////////////////////
PVOID ApAdmin::AddObject(PVOID pObject, INT_PTR nKey1, CHAR* szKey2)
{
	INT32	lIndex;
	PVOID	pRetval = NULL;

	// HashMap 2개의 동기화를 위해서는 Set하기전에 데이터를 모두 점검해야됨
	if (pObject == NULL || nKey1 == 0)	return NULL;
	
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	if (GetHashMapINT32(nKey1) ||
		(szKey2 && GetHashMapstring(szKey2)))
		return NULL;

	// insert m_csObject
	lIndex = m_csObject.Add(pObject);
	if (lIndex < 0)		return NULL;

	SetHashMapINT32(nKey1, lIndex);
	
	if (szKey2 != NULL)
		SetHashMapstring(szKey2, lIndex);

	pRetval = m_csObject.GetData(lIndex);

	if (m_pfConstructor)
	{
		m_pfConstructor(pRetval, m_pvClass, m_nIdentity);
	}

	return pRetval;
}

//		AddStringKey
//	Functions
//		- 이미 nKey로 들어가있는 데이타에 szKey를 더한다.
//	Arguments
//		- nKey : B+tree key value
//		- szKey : new string key
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::AddStringKey(INT_PTR nKey, CHAR *szKey)
{
	if (!szKey)	return FALSE;
		
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	if (GetHashMapstring(szKey))
		return FALSE;

	// search object in B+tree
	INT32 lIndex = GetHashMapINT32(nKey);

	if (!SetHashMapstring(szKey, lIndex)) return FALSE;

	return TRUE;
}

//		AddStringKey
//	Functions
//		- 이미 nKey로 들어가있는 데이타에 szKey를 더한다.
//	Arguments
//		- nKey : B+tree key value
//		- szKey : new string key
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::UpdateStringKey(CHAR *szOriginalKey, CHAR *szNewKey)
{
	if (!szOriginalKey || !szOriginalKey[0] || !szNewKey || !szNewKey[0])
		return FALSE;

	INT32	lIndex;
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	// search object in B+tree
	lIndex = GetHashMapstring(szOriginalKey);

	if (0 == lIndex) return FALSE;

	if (!RemoveHashMapstring(szOriginalKey))	return FALSE;
	if (!SetHashMapstring(szNewKey, lIndex))	return FALSE;

	return TRUE;
}

//		RemoveObject
//	Functions
//		- remove object in B+tree
//		- remove object in m_csObject array
//	Arguments
//		- nKey : B+tree key value
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::RemoveObject(INT_PTR nKey)
{
	INT32	lIndex = 0;
	PVOID	pObject;

	pObject = GetObject(nKey);
	if (pObject && m_pfDestructor)
		m_pfDestructor(pObject, m_pvClass, m_nIdentity);

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	// delete object in B+tree
	lIndex = GetHashMapINT32(nKey);

	if (!RemoveHashMapINT32(nKey)) return FALSE;

	// delete m_csObject
	if (0 == lIndex) return FALSE;
	if (!m_csObject.Delete(lIndex)) return FALSE;

	return TRUE;
}

//		RemoveObject
//	Functions
//		- remove object in B+tree
//		- remove object in m_csObject array
//	Arguments
//		- szKey : B+tree key value
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::RemoveObject(CHAR* szKey)
{
	INT32	lIndex = 0;
	PVOID	pObject;

	if (szKey == NULL)
		return FALSE;
	
	pObject = GetObject(szKey);
	if (pObject && m_pfDestructor)
		m_pfDestructor(pObject, m_pvClass, m_nIdentity);

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	// delete object in B+tree
	lIndex = GetHashMapstring(szKey);
	if (!RemoveHashMapstring(szKey)) return FALSE;

	if (0 == lIndex) return FALSE;

	// delete m_csObject
	if (!m_csObject.Delete(lIndex)) return FALSE;

	return TRUE;
}

//		RemoveObject
//	Functions
//		- remove object in B+trees
//		- remove object in m_csObject array
//	Arguments
//		- nKey1 : B+tree key value 1
//		- szKey2 : B+tree key value 2
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::RemoveObject(INT_PTR nKey1, CHAR* szKey2)
{
	INT32	lIndex = 0;
	PVOID	pObject;

	pObject = GetObject(nKey1);
	if (pObject && m_pfDestructor)
		m_pfDestructor(pObject, m_pvClass, m_nIdentity);

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	if (szKey2 && szKey2[0])	// 키값이 NULL이 아닌경우만 트리에서 삭제한다.
		RemoveHashMapstring(szKey2);

	// delete object in B+tree
	lIndex = GetHashMapINT32(nKey1);

	if (!RemoveHashMapINT32(nKey1)) return FALSE;

	// delete m_csObject
	if (0 == lIndex) return FALSE;
	if (!m_csObject.Delete(lIndex)) return FALSE;

	return TRUE;
}

//		GetObject
//	Functions
//		- get object
//	Arguments
//		- nKey : B+tree key value
//	Return value
//		- PVOID : data pointer
//		- NULL : failed get data
///////////////////////////////////////////////////////////////////////////////
PVOID ApAdmin::GetObject(INT_PTR nKey)
{
	INT32	lIndex;
	PVOID	pvObject;

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	// search object in B+tree
	lIndex = GetHashMapINT32(nKey);
	if ( 0 == lIndex ) return NULL;

	pvObject = m_csObject.GetData(lIndex);

	return pvObject;
}

//		GetObject
//	Functions
//		- get object
//	Arguments
//		- szKey : B+tree key value
//	Return value
//		- PVOID : data pointer
//		- NULL : failed get data
///////////////////////////////////////////////////////////////////////////////
PVOID ApAdmin::GetObject(CHAR* szKey)
{
	INT32	lIndex;
	PVOID	pvObject;

	if (szKey == NULL)	return NULL;
	
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	lIndex = GetHashMapstring(szKey);

	if (0 == lIndex)	return NULL;
	
	pvObject = m_csObject.GetData(lIndex);

	return pvObject;
}

//		GetObjectSequence
//	Functions
//		- get object (순서대로 다 가져온다)
//				마지막 데이타는 pObjectCxt 멤버들이 NULL이다.
//	Arguments
//		- plIndex : 가져오기 시작할 인덱스
//	Return value
//		- PVOID : data pointer
//		- NULL : 더이상 데이타가 없당
///////////////////////////////////////////////////////////////////////////////
PVOID ApAdmin::GetObjectSequence(INT32* plIndex)
{
	if (plIndex == NULL)
		return NULL;

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return NULL;

	return m_csObject.GetSequence(plIndex);
}

//		IsObject
//	Functions
//		- object가 존재하는지 검사한다.
//	Arguments
//		- nKey : 찾을 object key data
//	Return value
//		- BOOL : 존재 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::IsObject(INT_PTR nKey)
{
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	return (BOOL)GetHashMapINT32(nKey);
}

//		IsObject
//	Functions
//		- object가 존재하는지 검사한다.
//	Arguments
//		- szKey : 찾을 object key data
//	Return value
//		- BOOL : 존재 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::IsObject(CHAR *szKey)
{
	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	return (BOOL)GetHashMapstring(szKey);
}

//		RLock
//	Functions
//		- mutex read lock
//	Arguments
//		- pMutex : mutex object pointer
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::RLock(ApMutualEx* pMutex)
{
	if (pMutex == NULL)
		return FALSE;

	return pMutex->RLock();
}

//		WLock
//	Functions
//		- mutex write lock
//	Arguments
//		- pMutex : mutex object pointer
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::WLock(ApMutualEx* pMutex)
{
	if (pMutex == NULL)
		return FALSE;

	return pMutex->WLock();
}

//		Release
//	Functions
//		- release mutex lock
//	Arguments
//		- pMutex : mutex object pointer
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::Release(ApMutualEx* pMutex)
{
	if (pMutex == NULL)
		return FALSE;

	return pMutex->Release();
}


BOOL ApAdmin::SetCount(INT32 lObjectCount)
{
	if (m_lObjectCount)
		return FALSE;

	m_lObjectCount = lObjectCount;

	return TRUE;
}

INT32 ApAdmin::GetCount()
{
	return m_lObjectCount;
}

// 2004.03.24. steeple
// GetCount() 가 동작하지 않길래 하나 만듬
INT32 ApAdmin::GetObjectCount()
{
	INT32 lCount = 0;

	lCount = m_csObject.m_lDataCount;

	return lCount;
}

BOOL ApAdmin::Reset()
{
	if (!RemoveObjectAll(TRUE))
		return FALSE;

	//m_pBplusTree[AUBTREE_FLAG_KEY_NUMBER].Initialize(m_nOrderOfTree, AUBTREE_FLAG_KEY_NUMBER);
	//m_pBplusTree[AUBTREE_FLAG_KEY_STRING].Initialize(m_nOrderOfTree, AUBTREE_FLAG_KEY_STRING);

	return TRUE;
}

BOOL ApAdmin::RemoveObjectAll(BOOL bIsReset)
{
	INT32	lIndex = 0;

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	if (bIsReset)
		m_csObject.Reset();

	ClearHashMapINT32();
	ClearHashMapstring();

	return TRUE;
}

//		UpdateKey
//	Functions
//		- Update key in B+tree
//	Arguments
//		- lKey : 기존 데이타를 찾기위해 ID key 값을 넘겨준다.
//				  이걸로 데이타를 찾아 업데이트한다. (ex. CID, TID 등등)
//		- szUpdateKey : 새로 변경된 이름 (업데이트할 이름)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApAdmin::UpdateKey(INT_PTR lKey, CHAR* szUpdateKey)
{
	INT32	lIndex;

	if (szUpdateKey == NULL)
		return FALSE;	

	AuAutoLock _Lock(m_Mutex);
	if (!_Lock.Result()) return FALSE;

	lIndex = GetHashMapINT32(lKey);
	if (0 == lIndex) return FALSE;

	if (!SetHashMapstring(szUpdateKey, lIndex)) return FALSE;

	return TRUE;
}

BOOL ApAdmin::GlobalRLock()
{
	return g_csGlobalAdminMutex.RLock();
}

BOOL ApAdmin::GlobalWLock()
{
	return g_csGlobalAdminMutex.WLock();
}

BOOL ApAdmin::GlobalRelease()
{
	return g_csGlobalAdminMutex.Release();
}

BOOL ApAdmin::SetNotUseLockManager()
{
	return m_csObject.SetNotUseLockManager();
}
