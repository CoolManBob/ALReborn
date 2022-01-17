#ifndef  __WBANetwork_Pool_H
#define  __WBANetwork_Pool_H

#include <queue>

namespace  WBANetwork
{
	template < class _alloc >
	class Pool
	{
	public:
				Pool()
				{
				}
				~Pool()
				{
				}

				bool			Create( int size )
				{
					if( Size() > 0 )
					{
						return false;
					}

					for(int i = 0 ; i < size ; i++ )
					{
						_alloc* _item = _new_dbg_ _alloc;
						m_Queue.push( _item );
					}

					return true;
				}

				void			Destroy()
				{
					while( m_Queue.empty() == false )
					{
						_alloc*	_item = m_Queue.front();
						m_Queue.pop();

						delete _item;
					}
				}

				void			IncSize( int inc )
				{
					for(int i = 0 ; i < inc ; i++ )
						m_Queue.push( ( _alloc* )( _new_dbg_ _alloc ) );
				}

				int				Size()
				{
					return ( int )m_Queue.size();
				}

				_alloc*			Pop()
				{
					if( Size() == 0 )
						return 0;

					_alloc* item = m_Queue.front();
					m_Queue.pop();

					return item;
				}

				void			Push( _alloc* t )
				{
					m_Queue.push( t );
				}

	private:
				std::queue		< _alloc* >		m_Queue;
	};
}

#endif