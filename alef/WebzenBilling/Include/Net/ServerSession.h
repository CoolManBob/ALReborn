#ifndef  __WBANetwork_ServerSession_H
#define  __WBANetwork_ServerSession_H

#include <IO/EventQueue.h>
#include <Util/CircuitQueue.h>
#include <Util/Stream.h>
#include <Threading/Mutex.h>

namespace WBANetwork
{
	enum SEND_RET {SUCESS, FAILD_OVERFLOW, FAILD_CLOSED, FAILD_NULL};

	class ServerNetwork;
	class ServerSession : public EventHandler
	{
		friend class SessionManager;

	public:
				ServerSession( int sendBufSize, int recvBufSize );
		virtual	~ServerSession();

		//  Derived virtual function
		virtual	void			HandleEvent( AsyncResult* result );
		virtual	Socket*			GetHandle()							{	return &m_Socket;						}
		virtual	char*			GetBuffer()							{	return 0;								}
		virtual	int				GetBufferSize()						{	return 0;								}

				void			Flush();

				void			Create();
				void			Create( SOCKET s, Socket::SocketAddr& addr );
				void			Close();
				DWORD			SetKeepAlive(u_long onoff, u_long KeepaliveTime, u_long Keepaliveinterval);
				void			Connect( TCHAR* ipAddress, unsigned short portNo );
				void			SetKill( DWORD err = 0 );

				//bool			CompulsionSendPacket( PBYTE buffer, int length, AsyncResult* result );

				void			SendPacket( void* buffer, int size, SEND_RET& ret );
		virtual	bool			SendPacket( void* buffer, int size );
		virtual	bool			SendPacket( Stream& stream );

				TCHAR*			GetIPAddress()						{	return m_Socket.GetIPAddress();			}
				DWORD			GetUID()							{	return m_UID;							}

				PBYTE			GetSendBuffer()						{	return m_CQSendBuffer.GetReadPtr();		}
				PBYTE			GetRecvBuffer()						{	return m_CQRecvBuffer.GetWritePtr();		}
				DWORD			GetNextSendSize()					{	return m_CQSendBuffer.GetReadableSize();	}
				void			GetSendBufferSize( DWORD* remain, DWORD* max );
				void			GetRecvBufferSize( DWORD* remain, DWORD* max );

				DWORD			GetSendReadableSize()				{	return m_CQSendBuffer.GetReadableSize();	}
				DWORD			GetSendWritableSize()				{	return m_CQSendBuffer.GetWritableSize();	}

				DWORD			GetRecvReadableSize()				{	return m_CQRecvBuffer.GetReadableSize();	}
				DWORD			GetRecvWritableSize()				{	return m_CQRecvBuffer.GetWritableSize();	}

				void			SetMPU( DWORD size )				{	m_dwMaxSizePerUpdate = size;				}
				DWORD			GetMPU()							{	return m_dwMaxSizePerUpdate;				}


				AsyncResult		m_AResultAccept, m_AResultConnect, m_AResultClose;
				AsyncResult		m_AResultSend, m_AResultRecv;

	protected:
		//  Member virtual function
		virtual	void			OnAccept()										{}
		virtual	void			OnConnect( bool /*success*/, DWORD /*error*/ )	{}
		virtual	void			OnSend( int /*size*/ )							{}
		virtual	void			OnReceive( PBYTE /*buffer*/, int /*size*/ )		{}
		virtual	void			OnClose( DWORD /*error*/ )						{}
		virtual	void			Update();


		virtual	bool			IsValidPacket( PBYTE ptr, DWORD recvBytes, DWORD* totalSize )
				{
					*totalSize = *( DWORD* )ptr;

					return ( recvBytes >= *totalSize );
				}

	private:
				void			Clear();
				void			WaitForRecv();

				DWORD			m_UID;
				Socket			m_Socket;

				CircuitQueue	< BYTE >	m_CQSendBuffer;
				CircuitQueue	< BYTE >	m_CQRecvBuffer;
				PBYTE			m_szDequeueBuffer;

				bool			m_bReadyToSend;
				DWORD			m_dwSendBufSize;
				DWORD			m_dwRecvBufSize;
				DWORD			m_dwSendReqSize;
				DWORD			m_dwSendCompletionSize;
				DWORD			m_dwMaxSizePerUpdate;

				ServerNetwork*	m_pServerNetwork;
				Mutex			m_MutexSend, m_MutexRecv;
	};
}

#endif