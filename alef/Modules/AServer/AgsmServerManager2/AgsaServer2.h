/*============================================================================

	AgsaServer2.h

============================================================================*/

#ifndef _AGSA_SERVER2_H_
	#define _AGSA_SERVER2_H_

#include "ApBase.h"
#include "AsDefine.h"
#include "ApAdmin.h"
#include "AgsdServer2.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMSERVER_MAX_GAME_SERVER			2000
#define AGSMSERVER_MAX_LOGIN_SERVER			2000

/************************************************/
/*		The Definition of AgsaServer2 class		*/
/************************************************/
//
class AgsaServer2 : public ApAdmin
	{
	private:
		ApSafeArray<AgsdServer2 *, AGSMSERVER_MAX_LOGIN_SERVER>	m_pcsLoginServers;
		ApSafeArray<AgsdServer2 *, AGSMSERVER_MAX_GAME_SERVER>	m_pcsGameServers;

		INT16				m_nNumServers;
		INT16				m_nNumGameServers;
		INT16				m_nNumLoginServers;

	public:
		AgsaServer2();
		virtual ~AgsaServer2();
		
		static int SortByOrder(const void * p1, const void * p2);
		void SortGameServers();

		// add/remove
		AgsdServer2*		AddServer(AgsdServer2 *pcsServer);
		BOOL				RemoveServer(INT32 lServerID);

		// get
		AgsdServer2*		GetServer(INT32 lServerID);
		AgsdServer2*		GetServer(CHAR *pszIP);

		// iterate
		AgsdServer2*		GetLoginServers(INT16 *pnIndex);
		AgsdServer2*		GetGameServers(INT16 *pnIndex);
	};



/****************************************************/
/*		The Definition of AgsaServerWorld class		*/
/****************************************************/
//
class AgsaServerWorld : public ApAdmin
	{
	public:
		AgsaServerWorld();
		virtual ~AgsaServerWorld();
		
		AgsdServer2*		Add(CHAR *pszWorld, AgsdServer2 *pcsServer);
		BOOL				Remove(CHAR *pszWorld);
		
		AgsdServer2*		Get(CHAR *pszWorld);
	};



#endif	// _AGSA_SERVER_H_
