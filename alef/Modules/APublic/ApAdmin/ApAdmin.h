/******************************************************************************
Modle:  ApAdmin.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2003. 11. 16
			성능상의 이유로 SGI STL의 hash_map으로 변경 - 정남영
******************************************************************************/

#if !defined(__APADMIN_H__)
#define __APADMIN_H__

#include "ApBase.h"
#include "ApArray.h"
#include <unordered_map>
#include <string>

using namespace std;

#define APADMIN_MAX_CALLBACK_FUNC	10

#pragma warning( disable : 4786 )

// 마고자 (2004-12-06 오전 11:44:15) : 
// Archlord Error Code
// GetLastError 로 확인하세요..
enum AECodes
{
	AEC_PARAMETER_ERROR			= 0x20000001	,
	AEC_KEY_ALEREADY_EXIST		= 0x20000002	,
	AEC_INSERT_FAILED			= 0x20000003	,
	AEC_OBJECT_COUNT_LIMITED	= 0x20000004	,
	AEC_UNKNOWN					= 0x2fffffff	
};

typedef BOOL (*ApAdminDefaltCallBack) (PVOID data, PVOID pClass, INT16 nID);

//	class ApAdmin
///////////////////////////////////////////////////////////////////////////////
class ApAdmin {
public:
	ApArray<DONT_USE_AUTOLOCK>		m_csObject;					// object data store
	
private:
	//@{ Jaewon 20041118
	// unordered_map is not a part of the standard library according to C++ standard.
	typedef /*stdext::*/unordered_map<INT_PTR, INT32> HashMap_INT32;
	typedef /*stdext::*/unordered_map<std::string, INT32> HashMap_string;
	//@} Jaewon

	HashMap_INT32	m_HashMapINT32;
	HashMap_string	m_HashMapstring;

	INT32			m_lObjectCount;				// # of stored object
	INT16			m_nObjectSize;				// object data size

	INT16			m_nIdentity;				// object identity

	// constructor/destructor
	PVOID					m_pvClass;			// constructor/destructor class
	ApAdminDefaltCallBack	m_pfConstructor;	// constructor functions
	ApAdminDefaltCallBack	m_pfDestructor;		// destructor functions

private:
	INT32			GetHashMapINT32(INT_PTR nKey);
	INT32			GetHashMapstring(CHAR* szKey);

	BOOL			SetHashMapINT32(INT_PTR nKey, INT32 lIndex);
	BOOL			SetHashMapstring(CHAR* szKey, INT32 lIndex);

	BOOL			RemoveHashMapINT32(INT_PTR nKey);
	BOOL			RemoveHashMapstring(CHAR* szKey);

	VOID			ClearHashMapINT32();		// RemoveAll과 같은 기능
	VOID			ClearHashMapstring();		// RemoveAll과 같은 기능
	
public:
	ApCriticalSection		m_Mutex;
	
public:
	ApAdmin();
	virtual ~ApAdmin();

	INT32 GetCount();
	BOOL SetCount(INT32 lObjectCount);
	INT32 GetObjectCount();	// 2004.03.24. steeple

	// object initialize
	BOOL InitializeObject(INT16 nObjectSize, INT32 lObjectCount, ApAdminDefaltCallBack pfConstructor = NULL, ApAdminDefaltCallBack pfDestructor = NULL, PVOID pvClass = NULL, INT16 nID = 0);

	// add object... (m_csObject, B+tree)
	PVOID AddObject(PVOID pObject, INT_PTR nKey);
	PVOID AddObject(PVOID pObject, CHAR* szKey);
	PVOID AddObject(PVOID pObject, INT_PTR nKey1, CHAR* szKey2);

	BOOL AddStringKey(INT_PTR nKey, CHAR *szKey);
	BOOL UpdateStringKey(CHAR *szOriginalKey, CHAR *szNewKey);

	// remove object... (m_csObject, B+tree)
	BOOL RemoveObject(INT_PTR nKey);
	BOOL RemoveObject(CHAR* szKey);
	BOOL RemoveObject(INT_PTR nKey1, CHAR* szKey2);

	BOOL UpdateKey(INT_PTR lKey, CHAR* szUpdateKey);

	BOOL Reset();

	BOOL RemoveObjectAll(BOOL bIsReset = TRUE);

	// get object...
	PVOID GetObject(INT_PTR nKey);
	PVOID GetObject(CHAR* szKey);

	PVOID GetObjectSequence(INT32* plIndex);

	BOOL IsObject(INT_PTR nKey);
	BOOL IsObject(CHAR* szKey);

	// mutex lock
	BOOL RLock(ApMutualEx* pMutex);
	BOOL WLock(ApMutualEx* pMutex);
	BOOL Release(ApMutualEx* pMutex);

	BOOL GlobalRLock();
	BOOL GlobalWLock();
	BOOL GlobalRelease();

	BOOL SetNotUseLockManager();
};

#endif // __APADMIN_H__
