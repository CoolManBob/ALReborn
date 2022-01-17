/*===================================================================

	AgsmDatabasePool.h

===================================================================*/

#ifndef _AGSM_DATABASE_POOL_H_
	#define _AGSM_DATABASE_POOL_H_

#include "AgsEngine.h"
#include "AgsmDatabaseConfig.h"
#include "IOCPDatabase.h"
#include "AgpmConfig.h"

/********************************************************/
/*		The Definition of AgsmDatabasePool class		*/
/********************************************************/
//
class AgsmDatabasePool : public AgsModule
	{
	public:
		AgsmDatabaseConfig	*m_pAgsmDatabaseConfig;
		AgpmConfig			*m_pAgpmConfig;

	private:
		IOCPDatabase		*m_pIOCPDatabases;

	public:
		INT16				m_nForcedStart;
		INT16				m_nTotalForced;
		UINT32				m_ulLastFlushBufferingCommitClock;
		
	public:
		AgsmDatabasePool();
		virtual ~AgsmDatabasePool();

		//	ApModule inherited
		BOOL OnAddModule();
		BOOL OnInit();
		BOOL OnIdle(UINT32 ulClockCount);
		BOOL OnDestroy();

		//	Create
		BOOL	Create();

		//	Execute
		BOOL	Execute(AgsdQuery* pQuery, INT16 nForcedIndex = 0);

		//
		INT16	GetForced();
		
		//	Status
		INT32	GetPosted(INT16 nIndex = -1000);		// -1000 goes total
		INT32	GetExecuted(INT16 nIndex = -1000);
		INT32	GetFailed(INT16 nIndex = -1000);
	};

#endif
