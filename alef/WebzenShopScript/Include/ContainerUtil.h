#ifndef __CONTAINER_UTILITY_H__
#define __CONTAINER_UTILITY_H__


#include <map>
#include <vector>


namespace ContainerUtil
{
	template< class KEY_TYPE, class TYPE >
	class ContainerMap
	{
	private :
		std::map< KEY_TYPE, TYPE >								m_mapContainers;

	public :
		ContainerMap( void )									{ m_mapContainers.clear(); }
		~ContainerMap( void )									{ m_mapContainers.clear(); }

	public :
		void						Add							( KEY_TYPE Key, TYPE tEntry );
		void						Delete						( KEY_TYPE Key );
		void						DeleteByIndex				( const int nIndex );
		TYPE*						Get							( KEY_TYPE Key );
		TYPE*						GetByIndex					( const int nIndex );
		bool						Find						( KEY_TYPE Key );		
		const int					GetSize						( void ) { return ( int )m_mapContainers.size(); }
		void						Clear						( void ) { m_mapContainers.clear(); }

		//-----------------------------------------------------------------------
		//
		ContainerMap( ContainerMap const & other )
		{
			if( !other.m_mapContainers.empty() )
			{
				m_mapContainers.insert( other.m_mapContainers.begin(), other.m_mapContainers.end() );
			}
		}

	private:
		ContainerMap & operator=( ContainerMap const & other );

		//-----------------------------------------------------------------------
	};

	template< class TYPE >
	class ContainerVector
	{
	private :
		std::vector< TYPE >										m_vecContainers;

	public :
		ContainerVector( void )								{ m_vecContainers.clear(); }
		~ContainerVector( void )								{ m_vecContainers.clear(); }

	public :
		const int					Add							( TYPE tEntry );
		void						Delete						( const int nIndex );
		TYPE*						Get							( const int nIndex );
		const int					GetSize						( void ) { return ( int )m_vecContainers.size(); }
		void						Clear						( void ) { m_vecContainers.clear(); }

		//-----------------------------------------------------------------------
		//

		ContainerVector( ContainerVector const & other ) 
		{
			if( !other.m_vecContainers.empty() )
			{
				m_vecContainers.insert( other.m_vecContainers.begin(), other.m_vecContainers.end() );
			}
		}

	private:
		void operator=(ContainerVector const & other);

		//-----------------------------------------------------------------------
	};
};
using namespace ContainerUtil;



template< class KEY_TYPE, class TYPE >
void ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::Add( KEY_TYPE Key, TYPE tEntry )
{
	m_mapContainers.insert( std::map< KEY_TYPE, TYPE >::value_type( Key, tEntry ) );
}


template< class KEY_TYPE, class TYPE >
void ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::Delete( KEY_TYPE Key )
{
	std::map< KEY_TYPE, TYPE >::iterator Iter;
	Iter = m_mapContainers.find( Key );

	if( Iter != m_mapContainers.end() )
	{
		//Iter = m_mapContainers.erase( Iter );
		m_mapContainers.erase( Iter );
	}
}

template< class KEY_TYPE, class TYPE >
void ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::DeleteByIndex( const int nIndex )
{
	int nMapSize = ( int )m_mapContainers.size();

	std::map< KEY_TYPE, TYPE >::iterator Iter;
	Iter = m_mapContainers.begin();

	for( int nCount = 0 ; nCount < nMapSize ; nCount++ )
	{
		if( nCount == nIndex )
		{
			m_mapContainers.erase( Iter );
			return;
		}
		else
		{
			Iter++;
		}
	}
}

template< class KEY_TYPE, class TYPE >
TYPE* ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::Get( KEY_TYPE Key )
{
	std::map< KEY_TYPE, TYPE >::iterator Iter;
	Iter = m_mapContainers.find( Key );

	if( Iter != m_mapContainers.end() )
	{
		return &Iter->second;
	}
	
	return NULL;
}


template< class KEY_TYPE, class TYPE >
TYPE* ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::GetByIndex( const int nIndex )
{
	int nMapSize = ( int )m_mapContainers.size();

	std::map< KEY_TYPE, TYPE >::iterator Iter;
	Iter = m_mapContainers.begin();

	for( int nCount = 0 ; nCount < nMapSize ; nCount++ )
	{
		if( nCount == nIndex )
		{
			return &Iter->second;
		}

		Iter++;
	}

	return NULL;
}


template< class KEY_TYPE, class TYPE >
bool ContainerUtil::ContainerMap< KEY_TYPE, TYPE >::Find( KEY_TYPE Key )
{
	std::map< KEY_TYPE, TYPE >::iterator Iter;
	Iter = m_mapContainers.find( Key );

	if( Iter != m_mapContainers.end() )
	{
		return true;
	}
	
	return false;
}


template< class TYPE >
const int ContainerUtil::ContainerVector< TYPE >::Add( TYPE tEntry )
{
	int nSize = ( int )m_vecContainers.size();
	m_vecContainers.push_back( tEntry );
	return nSize;
}


template< class TYPE >
void ContainerUtil::ContainerVector< TYPE >::Delete( const int nIndex )
{
	int nVectorSize = ( int )m_vecContainers.size();

	std::vector< TYPE >::iterator Iter;
	Iter = m_vecContainers.begin();

	for( int nCount = 0 ; nCount < nVectorSize ; nCount++ )
	{
		if( nCount == nIndex )
		{
			Iter = m_vecContainers.erase( Iter );
			return;
		}

		Iter++;
	}
}


template< class TYPE >
TYPE* ContainerUtil::ContainerVector< TYPE >::Get( const int nIndex )
{
	int nVectorSize = ( int )m_vecContainers.size();

	if( nIndex >= nVectorSize )
		return NULL;

	return &m_vecContainers[ nIndex ];
}



#endif