/***************************************************************************************************
**
** profile.h
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

/*
** A node in the Profile Hierarchy Tree
*/
#ifndef _AU_PROFILE_MANAGER_
#define _AU_PROFILE_MANAGER_

#include <winsock2.h>
#include <windows.h>
#include "ApMutualEx.h"

#ifdef	_DEBUG
#ifndef __PROFILE__
#define _PROFILE_
#endif
#endif

// 2005/1/29 18:43 Parn
// Profile이 필요한 사람들은 Local로 PROFILE 쓸것.
#undef _PROFILE_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuProfileManagerD" )
#else
#pragma comment ( lib , "AuProfileManager" )
#endif
#endif

#define AUPROFILEMANAGER_MAX_THREADS	100

class	CProfileNode {

public:
	CProfileNode( const char * name, CProfileNode * parent );
	~CProfileNode( void );

	CProfileNode * Get_Sub_Node( const char * name );

	CProfileNode * Get_Parent( void )		{ return Parent; }
	CProfileNode * Get_Sibling( void )		{ return Sibling; }
	CProfileNode * Get_Child( void )			{ return Child; }

	CProfileNode * Get_Sibling( int index );
	CProfileNode * Get_Child( int index );

	void				Reset( void );
	void				Call( void );
	bool				Return( void );

	const char *	Get_Name( void )				{ return Name; }
	int				Get_Total_Calls( void )		{ return TotalCalls; }
	float			Get_Total_Time( void )		{ return TotalTime; }
	int				Get_Thread_Index( void )	{ return ThreadIndex; }
	DWORD			Get_Thread_ID( void )		{ return ThreadID; }

public:

	const char *	Name;
	int				TotalCalls;
	float			TotalTime;
	__int64			StartTime;
	int				RecursionCounter;
	int				ThreadIndex;
	DWORD			ThreadID;

	int				AccumulationCalls;			// 누적 통계를 위함
	float			AccumulationTime;	

	CProfileNode *	Parent;
	CProfileNode *	Child;
	CProfileNode *	Sibling;
};

/*
** The Manager for the Profile system
*/
class	AuProfileManager {
public:
	AuProfileManager();
	~AuProfileManager();

	static	void						Start_Profile( const char * name );
	static	void						Stop_Profile( void );

	static	void						Reset( void );
	static	void						Increment_Frame_Counter( void );
	static	int							Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
	static	float						Get_Time_Since_Reset( void );
	static	float						Get_Time_Since_Start( void );

	static	VOID						Lock()		{ Mutex->Lock(); }
	static	VOID						Unlock()	{ Mutex->Unlock(); }

public:
	__declspec( thread ) static CProfileNode *			Root;
	__declspec( thread ) static	CProfileNode *			CurrentNode;
	__declspec( thread ) static	int						FrameCounter;
	__declspec( thread ) static	__int64					ResetTime;
	__declspec( thread ) static	int						RootIndex;

	__declspec( thread ) static	__int64					StartTime;
	__declspec( thread ) static	bool					InitStart;

	static ApCriticalSection *			Mutex;
	static CProfileNode **				Roots;
	static CProfileNode **				RootsOrg;			// When Multi-Threaded, destruntor occurs problem...
};


/*
** ProfileSampleClass is a simple way to profile a function's scope
** Use the PROFILE macro at the start of scope to time
*/
class	CProfileSample {
public:
	CProfileSample( const char * name )
	{ 
		AuProfileManager::Start_Profile( name ); 
	}
	
	~CProfileSample( void )					
	{ 
		AuProfileManager::Stop_Profile(); 
	}
};

#ifdef _PROFILE_
#define	PROFILE( name )			//CProfileSample __profile( name )		// 읽기 전용으로 풀어서 쓰세요. 대부분 사람들은 이거 안쓴답니다. 특히 서버는... 이거 쓰면 대박..
#else
#define	PROFILE( name )
#endif


#endif