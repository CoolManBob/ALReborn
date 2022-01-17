/***************************************************************************************************
**
** profile.cpp
**
** Real-Time Hierarchical Profiling for Game Programming Gems 3
**
** by Greg Hjelstrom & Byon Garrabrant
**
***************************************************************************************************/

#include "AuProfileManager.h"

AuProfileManager g_csAuProfileManager;

//CProfileNode	AuProfileManager::Root( "Root", NULL );
CProfileNode *	AuProfileManager::Root = NULL;
//CProfileNode *	AuProfileManager::CurrentNode = AuProfileManager::Root;
CProfileNode *	AuProfileManager::CurrentNode = NULL;
int				AuProfileManager::FrameCounter = 0;
__int64			AuProfileManager::ResetTime = 0;
__int64			AuProfileManager::StartTime = 0;
bool			AuProfileManager::InitStart = 0;


ApCriticalSection *	AuProfileManager::Mutex = NULL;
int				AuProfileManager::RootIndex = 0;
CProfileNode **	AuProfileManager::Roots = NULL;
CProfileNode **	AuProfileManager::RootsOrg = NULL;

inline void Profile_Get_Ticks(_int64 * ticks)
{
#ifndef _M_X64
	__asm
	{
		push edx;
		push ecx;
		mov ecx,ticks;
		_emit 0Fh
		_emit 31h
		mov [ecx],eax;
		mov [ecx+4],edx;
		pop ecx;
		pop edx;
	}
#endif
}

//@{ Jaewon 20050923
// The following function will work out the processor clock frequency to a
// specified accuracy determined by the target average deviation required.
// Note that the worst average deviation of the result is less than 5MHz for
// a mean frequency of 90MHz. So basically the target average deviation is
// supplied only if you want a more accurate result, it won't let you get a
// worse one. (Units are Hz.)
//
// (The average deviation is a better and more robust measure than it's cousin
// the standard deviation of a quantity. The item determined by each is
// essentially similar. See "Numerical Recipies", W.Press et al for more
// details.)
//
// This function will run for a maximum of 20 seconds before giving up on
// trying to improve the average deviation, with the average deviation
// actually achieved replacing the supplied target value. Use "max_loops" to
// change this. To improve the value the function converges to increase
// "interval" (which is in units of ms, default value=1000ms).
#include <math.h>
double Profile_Get_CPU_Frequency(double& target_ave_dev, unsigned long interval=1000, unsigned int max_loops=20)
{
	register LARGE_INTEGER goal,period,current;
	register unsigned int ctr=0;
	double curr_freq,ave_freq;	// In Hz.
	double ave_dev,tmp=0;
	_int64 s,f;
	if (!QueryPerformanceFrequency(&period))
		return FALSE;
	period.QuadPart*=interval;
	period.QuadPart/=1000;

	// Start of tight timed loop.
	QueryPerformanceCounter(&goal);
	goal.QuadPart+=period.QuadPart;
	Profile_Get_Ticks(&s);
	do
	{
		QueryPerformanceCounter(&current);
	} while(current.QuadPart<goal.QuadPart);
	Profile_Get_Ticks(&f);
	// End of tight timed loop.

	ave_freq=1000*((double) (f - s))/interval;
	do
	{
		// Start of tight timed loop.
		QueryPerformanceCounter(&goal);
		goal.QuadPart+=period.QuadPart;
		Profile_Get_Ticks(&s);
		do
		{
			QueryPerformanceCounter(&current);
		} while(current.QuadPart<goal.QuadPart);
		Profile_Get_Ticks(&f);
		// End of tight timed loop.

		// Average of the old frequency plus the new.
		curr_freq=1000*((double) (f - s))/interval;
		ave_freq=(curr_freq+ave_freq)/2;

		// Work out the current average deviation of the frequency.
		tmp+=fabs(curr_freq-ave_freq);
		ave_dev=tmp/++ctr;
	} while (ave_dev>target_ave_dev && ctr<max_loops);
	target_ave_dev=ave_dev;
	return ave_freq;
}
//@} Jaewon

inline float Profile_Get_Tick_Rate(void)
{
	static float _CPUFrequency = -1.0f;
	
	if (_CPUFrequency == -1.0f) {
		//@{ Jaewon 20050923
		// Get the real CPU frequency instead of the performance frequency.
		double ave_dev = 0;
		_CPUFrequency = (float)(Profile_Get_CPU_Frequency(ave_dev, 1000, 5)/1000.0);
		//@} Jaewon
	} 
	
	return _CPUFrequency;
}

/***************************************************************************************************
**
** CProfileNode
**
***************************************************************************************************/

/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - pointer to a static string which is the name of this profile node                    *
 * parent - parent pointer                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The name is assumed to be a static pointer, only the pointer is stored and compared for     *
 * efficiency reasons.                                                                         *
 *=============================================================================================*/
CProfileNode::CProfileNode( const char * name, CProfileNode * parent ) :
	Name( name ),
	TotalCalls( 0 ),
	TotalTime( 0 ),
	StartTime( 0 ),
	RecursionCounter( 0 ),
	Parent( parent ),
	Child( NULL ),
	Sibling( NULL ),
	ThreadIndex( AuProfileManager::RootIndex ),
	ThreadID( GetCurrentThreadId()), 
	AccumulationCalls( 0 ),
	AccumulationTime( 0 )
{
	Reset();
}


CProfileNode::~CProfileNode( void )
{
	if (Child)
		delete Child;

	if (Sibling)
		delete Sibling;


}


/***********************************************************************************************
 * INPUT:                                                                                      *
 * name - static string pointer to the name of the node we are searching for                   *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All profile names are assumed to be static strings so this function uses pointer compares   *
 * to find the named node.                                                                     *
 *=============================================================================================*/
CProfileNode * CProfileNode::Get_Sub_Node( const char * name )
{
	// Try to find this sub node
	CProfileNode * child = Child;
	while ( child ) {
		if ( child->Name == name ) {
			return child;
		}
		child = child->Sibling;
	}

	// We didn't find it, so add it
	CProfileNode * node = new CProfileNode( name, this );
	node->Sibling = Child;
	Child = node;
	return node;
}


void	CProfileNode::Reset( void )
{
	AccumulationCalls += TotalCalls;
	AccumulationTime += TotalTime;

	TotalCalls = 0;
	TotalTime = 0.0f;

	if ( Child ) {
		Child->Reset();
	}
	if ( Sibling ) {
		Sibling->Reset();
	}
}


void	CProfileNode::Call( void )
{
	TotalCalls++;
	if (RecursionCounter++ == 0) {
		Profile_Get_Ticks(&StartTime);
	}
}


bool	CProfileNode::Return( void )
{
	if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
		__int64 time;
		Profile_Get_Ticks(&time);
		time-=StartTime;
		TotalTime += (float)time / Profile_Get_Tick_Rate();
	}
	return ( RecursionCounter == 0 );
}

CProfileNode* CProfileNode::Get_Child( int index )
{
	CProfileNode* cur_node = this;
	while(cur_node && index-- >0)
	{
		cur_node = cur_node->Sibling; 
	}

	if(cur_node) 
		return cur_node->Child;
	else
		return NULL;
}		

CProfileNode* CProfileNode::Get_Sibling( int index )
{
	CProfileNode* cur_node = this;
	while(cur_node && index-- >0)
	{
		cur_node = cur_node->Sibling; 
	}

	if(cur_node) 
		return cur_node;
	else
		return NULL;
}

/***************************************************************************************************
**
** CProfileManager
**
***************************************************************************************************/


/***********************************************************************************************
 * CProfileManager::Start_Profile -- Begin a named profile                                    *
 *                                                                                             *
 * Steps one level deeper into the tree, if a child already exists with the specified name     *
 * then it accumulates the profiling; otherwise a new child node is added to the profile tree. *
 *                                                                                             *
 * INPUT:                                                                                      *
 * name - name of this profiling record                                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * The string used is assumed to be a static string; pointer compares are used throughout      *
 * the profiling code for efficiency.                                                          *
 *=============================================================================================*/
AuProfileManager::AuProfileManager()
{
	Roots = new CProfileNode*[AUPROFILEMANAGER_MAX_THREADS];//(CProfileNode **) malloc( sizeof(CProfileNode *) * AUPROFILEMANAGER_MAX_THREADS );
	RootsOrg = Roots;

	memset( Roots, NULL, sizeof(CProfileNode *) * AUPROFILEMANAGER_MAX_THREADS );
	Mutex = new ApCriticalSection();

	InitStart = false;

	/*
	Root = new CProfileNode( "Root", NULL );
	Root->Sibling = NULL;

	CurrentNode = Root;
	*/
}

AuProfileManager::~AuProfileManager()
{
	int	i;

	delete Mutex;

	if (RootsOrg)
	{
		for (i = 0; i < AUPROFILEMANAGER_MAX_THREADS; ++i)
		{
			if (RootsOrg[i])
				delete RootsOrg[i];
		}

		delete []RootsOrg;
	}

	/*
	delete Root;
	*/
	//for(i = 0;i <AUPROFILEMANAGER_MAX_THREADS;++i)
	//{
		//free(Roots[i]);
	//}
}

void	AuProfileManager::Start_Profile( const char * name )
{
	if (!Root)
	{
		Root = new CProfileNode( "Root", NULL );
		Root->Sibling = NULL;

		CurrentNode = Root;

		Lock();

		for (RootIndex = 0; RootIndex < AUPROFILEMANAGER_MAX_THREADS; ++RootIndex)
		{
			if (!Roots[RootIndex])
			{
				Roots[RootIndex] = Root;
				break;
			}
		}

		Unlock();
	}

	Lock();

	if (CurrentNode)
	{
		if (name != CurrentNode->Get_Name()) {
			CurrentNode = CurrentNode->Get_Sub_Node( name );
		} 
		
		CurrentNode->Call();
	}

	Unlock();
}


/***********************************************************************************************
 * CProfileManager::Stop_Profile -- Stop timing and record the results.                       *
 *=============================================================================================*/
void	AuProfileManager::Stop_Profile( void )
{
	if (CurrentNode)
	{
		// Return will indicate whether we should back up to our parent (we may
		// be profiling a recursive function)
		if (CurrentNode->Return()) {
			CurrentNode = CurrentNode->Get_Parent();
		}
	}
}

/***********************************************************************************************
 * CProfileManager::Reset -- Reset the contents of the profiling system                       *
 *                                                                                             *
 *    This resets everything except for the tree structure.  All of the timing data is reset.  *
 *=============================================================================================*/
void	AuProfileManager::Reset( void )
{ 
	int	i;

	for (i = 0; i < AUPROFILEMANAGER_MAX_THREADS; ++i)
	{
		if (Roots[i])
		{
			Roots[i]->Reset(); 
		}
	}

	FrameCounter = 0;
	Profile_Get_Ticks(&ResetTime);

	if(!InitStart)
	{
		StartTime = ResetTime;
		InitStart = true;
	}
}


/***********************************************************************************************
 * CProfileManager::Increment_Frame_Counter -- Increment the frame counter                    *
 *=============================================================================================*/
void AuProfileManager::Increment_Frame_Counter( void )
{
	FrameCounter++;
}


/***********************************************************************************************
 * CProfileManager::Get_Time_Since_Reset -- returns the elapsed time since last reset         *
 *=============================================================================================*/
float AuProfileManager::Get_Time_Since_Reset( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= ResetTime;
	return (float)time / Profile_Get_Tick_Rate();
}

float AuProfileManager::Get_Time_Since_Start( void )
{
	__int64 time;
	Profile_Get_Ticks(&time);
	time -= StartTime;
	return (float)time / Profile_Get_Tick_Rate();
}


