#ifndef  __WBANetwork_SessionManager_H
#define  __WBANetwork_SessionManager_H

#include <map>
#include <list>

#include <Net/ServerSession.h>
#include <Threading/Mutex.h>

namespace WBANetwork
{
	class SessionManager
	{
		friend class ServerNetwork;

	public:
				ServerSession*		Find( DWORD uid );

				ServerSession*		BindSession( SOCKET s, Socket::SocketAddr* addr );
				bool				ActiveSession( ServerSession* session );
				void				InactiveSession( ServerSession* session );

				int					GetActiveSessionCnt()		{	return ( int )m_mapActive.size();	}
				int					GetInactiveSessionCnt()		{	return ( int )m_Pool.size();		}

	protected:
				SessionManager();
		virtual	~SessionManager();

		virtual	ServerSession*		CreateSession();
		virtual	void				DeleteSession( ServerSession* session );

		virtual	void				Update();
		virtual	void				UpdateInactive();

	private:
				bool				Create( int sessionCnt );
				void				Destroy();


		typedef	std::map	< DWORD, ServerSession* >	MapActive;
		typedef std::list	< ServerSession* >			ListPool;

				MapActive			m_mapActive;
				ListPool			m_Pool;
				Mutex				m_MutexActive, m_MutexPool;
				ServerNetwork*		m_pServerNetwork;
	};
}

#endif