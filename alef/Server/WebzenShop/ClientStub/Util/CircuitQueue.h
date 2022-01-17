#ifndef  __WBANetwork_CircuitQueue_H
#define  __WBANetwork_CircuitQueue_H


namespace WBANetwork
{
	template < class _T >
	class CircuitQueue
	{
	public:
				CircuitQueue()
					: m_buffer( NULL ), m_dataSize( 0 ), m_bufferSize( 0 ),
					m_posHead( 0 ), m_posTail( 0 ), m_surplusSize( 0 )
				{
				}
		virtual ~CircuitQueue()
				{
					Destroy();
				}

				bool	Create( DWORD bufSize )
				{
					if( m_buffer != 0 )
					{
						return false;
					}

					m_surplusSize	= bufSize * 10 / 100;
					m_buffer		= _new_dbg_ _T[bufSize + m_surplusSize];
					m_bufferSize	= bufSize;

					Clear();

					return true;
				}

				void	Destroy()
				{
					if( m_buffer != 0 )
						delete [] m_buffer;

					m_buffer = 0;
					Clear();
				}

				void	Clear()
				{
					m_dataSize	= 0;
					m_posHead	= 0;
					m_posTail	= 0;
				}

				DWORD		GetRemainBufSize()		{	return m_bufferSize - m_dataSize;	}
				DWORD		GetDataSize()			{	return m_dataSize;					}
				DWORD		GetBufferSize()			{	return m_bufferSize;				}

				bool	IsValidWritePtr( _T* pos, DWORD size )
				{
					DWORD bufStart = 0;

					DWORD bufEnd			= m_bufferSize - 1;
					DWORD posWriteStart	= ( DWORD )( pos - m_buffer );
					DWORD posWriteEnd		= ( DWORD )( pos - m_buffer + size - 1 );

					if( m_posHead <= m_posTail )
					{
						//  버퍼의 시작과 head 사이에 있을 경우
						if( bufStart >= posWriteStart && m_posHead > posWriteEnd )
							return true;

						// tail과 버퍼의 끝 사이에 있을 경우
						if( m_posTail >= posWriteStart && posWriteEnd <= bufEnd )
							return true;
					}
					else
					{
						//  무조건 tail과 head 사이에 있어야 한다.
						if( m_posTail >= posWriteStart && posWriteEnd < m_posHead )
							return true;
					}

					return false;
				}

				bool	Enqueue( _T* data, DWORD size )
				{
					if( GetRemainBufSize() < size )
					{
						return false;
					}

					if( data != 0 )
					{
						if( m_posHead <= m_posTail )
						{
							DWORD		remainSize = m_bufferSize - m_posTail;

							//  공간에 여유가 있다면 한번에 복사한다.
							if( remainSize >= size )
								memcpy( m_buffer + m_posTail, data, sizeof( _T ) * size );
							else
							{
								//  버퍼에 여유가 없다면 데이터를 두개로 나누어
								//  버퍼의 앞/뒤에 복사한다.
								memcpy( m_buffer + m_posTail, data, sizeof( _T ) * remainSize );
								memcpy( m_buffer, data + remainSize, sizeof( _T ) * ( size - remainSize ) );
							}
						}
						else
							memcpy( m_buffer + m_posTail, data, sizeof( _T ) * size );
					}

					m_posTail	= ( m_posTail + size ) % m_bufferSize;
					m_dataSize	+= size;

					return true;
				}

				bool	Dequeue( _T* dest, DWORD size )
				{
					if( Peek( dest, size ) == false )
					{
						return false;
					}

					m_posHead	+= size;
					m_posHead	%= m_bufferSize;
					m_dataSize	-= size;

					return true;
				}

				bool	Peek( _T* dest, DWORD size )
				{
					if( m_dataSize < size )
					{
						return false;
					}

					if( dest != 0 )
					{
						if( m_posHead < m_posTail )
							memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * size );
						else
						{
							DWORD		cutDataSize = m_bufferSize - m_posHead;

							//  요구된 크기만큼 연결되어(Linear) 남아있을 경우
							if( cutDataSize >= size )
								memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * size );
							else
							{
								//  데이터가 tail과 head로 나뉘어 저장된 경우
								memcpy( dest, m_buffer + m_posHead, sizeof( _T ) * cutDataSize );
								memcpy( dest + cutDataSize, m_buffer, sizeof( _T ) * ( size - cutDataSize ) );
							}
						}
					}

					return true;
				}

				_T*		GetReadPtr()			{	return ( m_buffer + m_posHead );		}
				_T*		GetWritePtr()			{	return ( m_buffer + m_posTail );		}

				DWORD		GetReadableSize()
				{
					if( m_posHead == m_posTail )
						return ( m_dataSize > 0 ? m_bufferSize - m_posHead : 0 );
					else if( m_posHead < m_posTail )
						return ( m_posTail - m_posHead );

					//  Tail이 원형 큐를 한바퀴 넘긴 상황에서 Surplus buffer를 사용할 수 있다면...
					if( m_posHead > m_posTail &&
						(( m_bufferSize - m_posHead + m_posTail ) < m_surplusSize) )
					{
						//  0 ~ tail까지의 버퍼를 임의로 Surplus buffer로 옮겨와
						//  한번에 읽을 수 있도록 한다.

						memcpy( m_buffer + m_bufferSize, m_buffer, m_posTail );
						return ( m_bufferSize - m_posHead + m_posTail );
					}

					return ( m_bufferSize - m_posHead );
				}

				DWORD		GetWritableSize()
				{
					if( m_posHead == m_posTail )
					{
						return ( m_dataSize > 0 ? 0 : m_bufferSize - m_posTail );
					}
					else if( m_posHead < m_posTail )
					{
						return m_bufferSize - m_posTail;
					}

					return m_posHead - m_posTail;
				}

	private:
				_T*			m_buffer;
				DWORD		m_dataSize;
				DWORD		m_bufferSize;
				DWORD		m_posHead;
				DWORD		m_posTail;
				DWORD		m_surplusSize;
	};
}

#endif