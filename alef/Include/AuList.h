///////////////////////////////////////////////////////////////////////////////////////////
// Double Linked List Template Class;
// 2000/06/14 Taken from Yong
// 2000/06/23 AuQueue , AuStack is made from the AuList by Magoja
// 마고자 (2002-04-30 오후 2:06:34) : 템플릿으로 변경
// 마고자 (2002-04-30 오후 2:35:48) : 데이타 딜리트 루틴 삭제. 모드 Mag씨리즈로 변경.
// 마고자 (2002-04-30 오후 2:42:04) : Operator[] is Added;
// Arranged by Magoja

// 주의! : 포인터를 리스트로 관리할경우 , 삭제할때 포인터 deleting 과정은 자동으로 이루어지지않으니
//           직접 delete해주는 과정을 빼면 심각한 메모리 릭이 발생할지도 모른다.

//////////////////////////////////////////////////////////////
// Simple Usage - 1 , List
//
//int main(int argc, char* argv[])
//{
//	AuList<int>	list;
//	for( int i = 0 ; i < 10 ; i ++ )
//		list.AddTail( i );
//
//	AuNode<int> *pNode = list.GetHeadNode();
//	while( pNode )
//	{
//		printf( "%d\n" , pNode->GetData() );
//		list.GetNext( pNode );
//	}
//
//	printf( "%d\n" , list[ 5 ] );
//
//	return 0;
//}
//////////////////////////////////////////////////////////////
// Simple Usage - 2 , like Array
//	AuList<int>	list;
//	list.Alloc( 10 ); // 안해주면 더미값을 끼워넣으면서 생성.
//	for( int i = 0 ; i < 10 ; i ++ )
//		list[ i ] = i;
//
//	list[ 15 ] = 100;	// 10,11,12,13 4개의 더미값을 생성하면서 마지막에 100을 넣음
//	for( i = 0 ; i < list.GetCount() ; i ++ )	// 현재 15개.
//		printf( "%d\n" , list[ i ] );
//
// 출력결과 
//	0
//	1
//	2
//	3
//	4
//	5
//	6
//	7
//	8
//	9
//	-858993460	
//	-858993460
//	-858993460
//	-858993460
//	-858993460		// 초기화 돼지 않은 더미값들!
//	100

#ifndef _AULIST_H_
#define	_AULIST_H_

#include <winsock2.h>
#include <windows.h>

#include "ApBase.h"
#include "ApMemoryPool.h"

template< class Type >
class AuList;

// Node structure for AuList data store
// Do not use it directly
template< class Type >
class AuNode
{
public:
	friend AuList< Type >;

protected :
	Type				m_Data	;
	AuNode<Type> *	m_pNext	;
	AuNode<Type> *	m_pPrev	;

public :
	AuNode	( Type Data );
	~AuNode	(			);

	// Managing this node links
	void LinkPrev		( AuNode<Type> * pNode	);
	void UnlinkPrev		(							);
	void LinkNext		( AuNode<Type> * pNode	);
	void UnlinkNext		(							);

	// Get prev or next node;
	AuNode<Type> *	GetPrevNode() const;
	AuNode<Type> *	GetNextNode() const;

	// get data , this node hold
	Type & GetData();
	//----> kday
	
	const Type & GetData()const;
	//<---- kday
	void SetData( Type Data );
};

// Main List Class.
template< class Type >
class AuList
{
protected :
	AuNode<Type> *	m_pHead		;
	AuNode<Type> *	m_pTail		;
	int					m_nMaxCount	;
	int					m_nCount	;

	ApMemoryPool		m_csNodePool	;

public :
	AuList( int nMaxNode  = 0);
	// If the element is an allocated pointer and you want it to be destroied 
	// with AuList , you may set bDataSelDestruct TRUE to order self derstruction
	// if bDataSelfDestruct is TRUE , NULL cannot be inserted!.

	AuList( const AuList & param ); // Copy constructor
										// it will generate exception , when m_bDataSelfDestruct flag is on
	~AuList();

	BOOL			AddHead(Type Data); // Store the data in the top of the list
	BOOL			AddTail(Type Data); // Insert the data in the bottom

	AuNode<Type>*	InsertBefore	( AuNode<Type> *pos , Type Data );
	AuNode<Type>*	InsertAfter		( AuNode<Type> *pos , Type Data );

	BOOL			RemoveHead	(						); // Remove Head node
	BOOL			RemoveTail	(						); // Remove Tail node
	BOOL			RemoveNode	( AuNode<Type>* node	); // Remove Specific node
	BOOL			RemoveData	( Type Data				); // Remove Specific data
	void			RemoveAll	(						); // Clear all the contents

	AuNode<Type>*	GetHeadNode() const;
	AuNode<Type>*	GetTailNode() const;

	AuNode<Type>*	GetNode( Type Data ) const;// Find a specific node with certain data ,
		// if impossible to find that , it will return NULL;
	Type &			GetHead		() const;// Get Head value;
	Type &			GetTail		() const;// Get Tail value;

	Type &			GetPrev		( AuNode<Type>*& node );// Get CURRENT node value and move to prev node. not a prev one
	Type &			GetNext		( AuNode<Type>*& node );// Get CURRENT node value and move to next node. not a next one

	int				GetCount	() const;// Get how many element this class hold.
	BOOL			IsEmpty		() const;// Determine this class have nothing.

	// Overloaded operator with AuList;
	// this will generate exception , when m_bDataSelfDestruct flag is on
	AuList &		operator=	( const AuList & param	);
	AuList &		operator+=	( AuList & param			);

	// For useing this list like standard array..
	Type &			operator[]	( int offset								);
	BOOL			Alloc		( int max_array_size , Type initialvalue	); // 최대치 지정. 

	friend AuList	operator+	( AuList & param1 , AuList & param2 );

	void			MoveToTail	( Type	node);

	// ----> kday
	// 20040518
	const AuNode<Type>*	Find	( const AuNode<Type>* pNode )const;
	const Type*			Find	( const Type*		  pData )const;
	// <---- kday
};

template< class Type >
class AuQueue  
{
private:
	AuList<Type>	list;
public:
	Type &	PeekQueue	(			) const;
	BOOL	IsEmpty		(			) const;
	int		GetCount	(			) const;
	Type &	Dequeue		(			);
	BOOL	Enqueue		( Type Data	);
	void	RemoveAll	(			);

	AuQueue();
	virtual ~AuQueue();
};

template< class Type >
class AuStack  
{
private:
	AuList<Type>	list;
public:
	Type &	Peek		(			) const;
	BOOL	IsEmpty		(			) const;
	int		GetCount	(			) const;
	Type &	Pop			(			);
	BOOL	Push		( Type Data	);
	void	RemoveAll();

	AuStack();
	virtual ~AuStack();
};

template< class Type >
AuNode<Type>::AuNode( Type Data )
{
	m_Data	= Data;
	m_pPrev	= NULL;
	m_pNext	= NULL;
}

template< class Type >
AuNode<Type>::~AuNode()
{
	// do no op
}
	  
template< class Type >
void AuNode<Type>::LinkPrev( AuNode<Type> * pNode )
{
	m_pPrev = pNode;
}

template< class Type >
void AuNode<Type>::LinkNext( AuNode<Type> * pNode )
{
	m_pNext = pNode;
}

template< class Type >
void AuNode<Type>::UnlinkPrev()
{
	m_pPrev = NULL;
}

template< class Type >
void AuNode<Type>::UnlinkNext()
{
	m_pNext = NULL;
}

template< class Type >
AuNode<Type> * AuNode<Type>::GetPrevNode() const
{
	return m_pPrev;
}

template< class Type >
AuNode<Type> * AuNode<Type>::GetNextNode() const
{
	return m_pNext;
}

template< class Type >
Type & AuNode<Type>::GetData()
{
	return m_Data;
}

//----> kday
template< class Type >
const Type & AuNode<Type>::GetData() const
{
	return m_Data;
}
//<---- kday

template< class Type >
void AuNode<Type>::SetData( Type Data )
{
	m_Data = Data;
}


template< class Type >
AuList<Type>::AuList( int nMaxNode )
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;
	m_nMaxCount = nMaxNode;

	if ( m_nMaxCount )
	{
#ifdef _CPPRTTI
		m_csNodePool.Initialize( sizeof( AuNode<Type> ), m_nMaxCount, "AuList");
#else
		m_csNodePool.Initialize( sizeof( AuNode<Type> ), m_nMaxCount );
#endif
	}
}

template< class Type >
AuList<Type>::AuList( const AuList<Type> & param ) // Copy constructor
{
	// 리스트 먼저 초기화 시키고
	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;

	m_nMaxCount = param.m_nMaxCount;
	if ( m_nMaxCount )
	{
		m_csNodePool.Initialize( sizeof( AuNode<Type> ), m_nMaxCount );
	}

	AuNode<Type> *pNode = param.m_pHead;//.GetHeadNode();

	while( pNode )
	{
		AddTail( pNode->GetData() );
		pNode = pNode->GetNextNode();
	}
}

template< class Type >
AuList<Type>::~AuList()
{
	RemoveAll();
}

template< class Type >
BOOL AuList<Type>::AddHead( Type Data )
{
	AuNode<Type> * pNode;
	
	if ( m_nMaxCount )
	{
		pNode = (AuNode<Type> *) m_csNodePool.Alloc();
		if ( pNode )
		{
			memset( pNode, 0, sizeof(AuNode<Type>) );
			pNode->SetData(Data);
		}
	}
	else
		pNode = new AuNode<Type> ( Data );

	if (pNode == NULL)
	{
		return FALSE;	
	}

	if (m_pHead == NULL)	// Node가 하나도 없을때
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}
	else
	{
		pNode->LinkNext(m_pHead);
		m_pHead->LinkPrev(pNode);
		m_pHead = pNode;
	}

	++m_nCount;

	return TRUE;
}

template< class Type >
BOOL AuList<Type>::AddTail( Type Data )
{
	AuNode<Type> * pNode;
	
	if ( m_nMaxCount )
	{
		pNode = (AuNode<Type> *) m_csNodePool.Alloc();
		if ( pNode )
		{
			memset( pNode, 0, sizeof(AuNode<Type>) );
			pNode->SetData(Data);
		}
	}
	else
		pNode = new AuNode<Type> ( Data );

	if (pNode == NULL)
	{
		return FALSE;	
	}

	if (m_pTail == NULL)	// Node가 하나도 없을때 
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}
	else
	{
		pNode->LinkPrev(m_pTail);
		m_pTail->LinkNext(pNode);
		m_pTail = pNode;
	}

	++m_nCount;

	return TRUE;
}

template< class Type >
BOOL AuList<Type>::RemoveHead()
{
	if( m_pHead )
	{
		RemoveNode ( m_pHead );
		return TRUE;
	}
	else
		return FALSE;
}

template< class Type >
BOOL AuList<Type>::RemoveTail()
{
	if( m_pTail ) 
	{
		RemoveNode(m_pTail);
		return TRUE;
	}
	else
		return FALSE;
}

template< class Type >
void AuList<Type>::MoveToTail	( Type	data)
{
	AuNode<Type>*		pNode = GetNode(data);
	if(pNode && m_pHead)
	{
		// node가 있을때
		if (pNode == m_pHead)
		{
			// head node를 지울때.
			if (m_pHead == m_pTail)
			{
				// node가 하나 있을때.
			}
			else
			{
				// node가 여러개 있을때.
				m_pHead = pNode->GetNextNode();
				m_pHead->UnlinkPrev();

				m_pTail->LinkNext(pNode);
				pNode->LinkPrev(m_pTail);
				pNode->UnlinkNext();
				m_pTail = pNode;
			}
		}
		else if (pNode == m_pTail)
		{
		}
		else
		{
			AuNode<Type>*		pn = pNode->GetNextNode();
			(pNode->GetPrevNode())->LinkNext(pNode->GetNextNode());
			pn->LinkPrev(pNode->GetPrevNode());

			m_pTail->LinkNext(pNode);
			pNode->LinkPrev(m_pTail);
			pNode->UnlinkNext();
			m_pTail = pNode;
		}
	}
}

template< class Type >
BOOL AuList<Type>::RemoveNode( AuNode<Type> * pNode)
{
	// node가 있는지 없는지 check
	if (m_pHead)
	{
		// node가 있을때
		if (pNode == m_pHead)
		{
			// head node를 지울때.
			if (m_pHead == m_pTail)
			{
				// node가 하나 있을때.
				m_pHead = m_pTail = NULL;
				m_nCount = 0;

				if ( m_nMaxCount )
					m_csNodePool.Free( pNode );
				else
					delete pNode;

				pNode = NULL;

				return TRUE;
			}
			else
			{
				// node가 여러개 있을때.
				m_pHead = pNode->GetNextNode();
				m_pHead->UnlinkPrev();
				--m_nCount;

				if ( m_nMaxCount )
					m_csNodePool.Free( pNode );
				else
					delete pNode;

				pNode = NULL;
				return TRUE;
			}
		}
		else if (pNode == m_pTail)
		{
			m_pTail = pNode->GetPrevNode();
			m_pTail->UnlinkNext();
			--m_nCount;

			if ( m_nMaxCount )
				m_csNodePool.Free( pNode );
			else
				delete pNode;

			pNode = NULL;
			return TRUE;
		}
		else
		{
			AuNode<Type> * m_pCur = m_pHead;

			while ((m_pCur != NULL) && ((m_pCur->GetNextNode()) != NULL))
			{
				if ((m_pCur->GetNextNode()) == pNode)
				{
					m_pCur->LinkNext(pNode->GetNextNode());
					(pNode->GetNextNode())->LinkPrev(m_pCur);
					--m_nCount;

					if ( m_nMaxCount )
						m_csNodePool.Free( pNode );
					else
						delete pNode;

					pNode = NULL;
					return TRUE;
				}
				m_pCur = m_pCur->GetNextNode();
			}	
		}
	}

	return FALSE;
}

template< class Type >
BOOL AuList<Type>::RemoveData( Type Data )
{
	AuNode<Type> * pCur = m_pHead;

	while (pCur != NULL)
	{
		if ( ( pCur->GetData() ) == Data ) 
		{
			RemoveNode(pCur);
			return TRUE;			
		}

		GetNext(pCur);
	}

	return FALSE;
}

template< class Type >
void AuList<Type>::RemoveAll()
{
	while( RemoveHead() );

	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;
}

template< class Type >
AuNode<Type> * AuList<Type>::GetHeadNode() const
{
	return m_pHead;
}

template< class Type >
AuNode<Type> * AuList<Type>::GetTailNode() const
{
	return m_pTail;
}

template< class Type >
AuNode<Type> * AuList<Type>::GetNode( Type Data) const
{
	AuNode<Type>	*pNode = GetHeadNode();
	while( pNode )
	{
		if( pNode->GetData() == Data ) return pNode;
		pNode = pNode->GetNextNode();
	}
	return NULL;
}

template< class Type >
Type & AuList<Type>::GetHead() const
{
	if (m_pHead == NULL)
	{
		throw "Cannot Get Head , cause there is no data\n";
//		return NULL;
	}

	return ( m_pHead->GetData() );
}

template< class Type >
Type & AuList<Type>::GetTail() const
{
	if (m_pTail == NULL)
	{
		throw "AuList::GetTail - List is empty!!!\n";
	}

	return ( m_pTail->GetData() );
}

template< class Type >
Type & AuList<Type>::GetPrev( AuNode<Type> *& pNode )
{
	if (pNode == NULL)
	{
//		return NULL;
		throw "AuList::GetPrev - List is empty!!!\n";
	}

	AuNode<Type> * pCur = pNode;

	pNode = pNode->GetPrevNode();

	return (pCur->GetData());
}

template< class Type >
Type & AuList<Type>::GetNext( AuNode<Type> *& pNode )
{
	if (pNode == NULL)
	{
		// 없는데이타는 마지막 값을 리턴.
		return GetTail();
	}

	AuNode<Type> * pCur = pNode;

	pNode = pNode->GetNextNode();

	return (pCur->GetData());
}

template< class Type >
int AuList<Type>::GetCount() const
{
	return m_nCount;
}

template< class Type >
BOOL AuList<Type>::IsEmpty() const
{
	if (m_nCount == 0)
		return TRUE;
	
	return FALSE;
}

template< class Type >
AuNode<Type> * AuList<Type>::InsertBefore( AuNode<Type> *pos , Type Data )
{
	AuNode<Type> * pNode;
	
	if ( m_nMaxCount )
	{
		pNode = (AuNode<Type> *) m_csNodePool.Alloc();
		if ( pNode )
		{
			memset( pNode, 0, sizeof(AuNode<Type>) );
			pNode->SetData(Data);
		}
	}
	else
		pNode = new AuNode<Type> ( Data );

	if (pNode == NULL)
	{
		return NULL;
	}

	if (m_pTail == NULL)	// Node가 하나도 없을때 
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}
	else
	{
		AuNode<Type> *pPrevNode;
		if( pPrevNode = pos->GetPrevNode() )
		{
			// Prev가 있을경우
			pPrevNode->LinkNext( pNode );
			pNode->LinkPrev( pPrevNode );
			pNode->LinkNext( pos );
			pos->LinkPrev( pNode );
		}
		else
		{
			// Prev가 NULL 인경우.
			m_pHead = pNode;
			pNode->LinkNext( pos );
			pos->LinkPrev( pNode );
		}
	}

	++m_nCount;

	return pNode;
}

template< class Type >
AuNode<Type> * AuList<Type>::InsertAfter( AuNode<Type> *pos , Type Data )
{
	AuNode<Type> * pNode;
	
	if ( m_nMaxCount )
	{
		pNode = (AuNode<Type> *) m_csNodePool.Alloc();
		if ( pNode )
		{
			memset( pNode, 0, sizeof(AuNode<Type>) );
			pNode->SetData(Data);
		}
	}
	else
		pNode = new AuNode<Type> ( Data );

	if (pNode == NULL)
	{
		return NULL;
	}

	if (m_pTail == NULL)	// Node가 하나도 없을때 
	{
		m_pHead = pNode;
		m_pTail = pNode;
	}
	else
	{
		AuNode<Type> *pNextNode;
		if( pNextNode = pos->GetNextNode() )
		{
			// Next가 있을경우

			pos->LinkNext( pNode );
			pNode->LinkPrev( pos );
			pNode->LinkNext( pNextNode );
			pNextNode->LinkPrev( pNode );
		}
		else
		{
			// Next가 NULL 인경우.
			m_pTail = pNode;
			pNode->LinkPrev( pos );
			pos->LinkNext( pNode );
		}
	}

	++m_nCount;

	return pNode;
}

template< class Type >
AuList<Type> & AuList<Type>::operator= ( const AuList<Type> & param )
{
	// 우선 데이타를 다 날리고.
	RemoveAll();
	
	AuNode<Type> *pNode = param.m_pHead;

	while( pNode )
	{
		AddTail( pNode->GetData() );
		pNode = pNode->GetNextNode();
	}

	return *this;
}

template< class Type >
AuList<Type> & AuList<Type>::operator+= ( AuList<Type> & param )
{
	// 리스트를 더함.
	AuNode<Type> *pNode = param.GetHeadNode();

	while( pNode )
	{
		AddTail( param.GetNext( pNode ) );
	}

	return *this;
}

template< class Type >
AuList<Type> operator+ ( AuList<Type> & param1 , AuList<Type> & param2 )
{
	AuList<Type> list;

	AuNode<Type> *pNode = param1.GetHeadNode();

	while( pNode )
	{
		list.AddTail( param1.GetNext( pNode ) );
	}

	pNode = param2.GetHeadNode();

	while( pNode )
	{
		list.AddTail( param2.GetNext( pNode ) );
	}

	return list;
}


template< class Type >
Type & AuList<Type>::operator[] ( int offset )
{
	if( offset < 0 ) throw "AuList - Offset 이 0보다 작다.\n";
	if( offset >= m_nCount )
	{
#ifdef	_DEBUG
		OutputDebugString( "AuList<Type>::operator[] 범위초과\n" );
#endif
//		Type dummy;
//		while( offset != GetCount() - 1 )
//		{
//			// 더미데이타를 채워서 리스트를 작성한다.
//			// 여기서 워닝 하나 발생.
//			// 문제가 생길 가능성 다분 -_-;
//#pragma warning(disable:4700)
//			this->AddTail( dummy );
//#pragma warning(default:4700)
//		}
//
//		return this->GetTail();
		throw "AuList - Offset 이 범위를 초가했음.\n";
	}

	AuNode<Type> *pNode = this->GetHeadNode();
	int count = 0;

	while( pNode )
	{
		if( offset == count ) return pNode->GetData();
		pNode = pNode->GetNextNode();
		++count ;
	}

	throw "AuList::operator[] - Strange Value\n";
}

template< class Type >
BOOL AuList<Type>::Alloc ( int max_array_size , Type initialvalue	) // 최대치 지정.
{
	while( max_array_size != GetCount() - 1 )
	{
		// 더미데이타를 채워서 리스트를 작성한다.
		if( !this->AddTail( initialvalue ) ) return FALSE;
	}
	return TRUE;
}


// ----> kday
// 20040518
template< class Type >
const AuNode<Type>* AuList<Type>::Find( const AuNode<Type>* pNode )const
{
	ASSERT( pNode );

	if( !m_pHead ||
		!m_pTail )
		return NULL;

	const AuNode<Type>*	pCurr	= m_pHead;
	while( pCurr ){
		if( pNode == pCurr )
			return pCurr;	//find it
		pCurr	= pCurr->GetNextNode();
	}

	return NULL;
};

template< class Type >
const Type*	AuList<Type>::Find( const Type* pData )const
{
	ASSERT( pData );

	if( !m_pHead ||
		!m_pTail )
		return NULL;

	const AuNode<Type>*	pCurr	= m_pHead;
	while( pCurr ){
		if( pCurr->GetData() == *pData )
			return &pCurr->GetData();	//find it
		pCurr	= pCurr->GetNextNode();
	}

	return NULL;	
};
// <---- kday
///////////////////////////////////////
// AuQueue Implementation
///////////////////////////////////////
template< class Type >
AuQueue<Type>::AuQueue()
{
}

template< class Type >
AuQueue<Type>::~AuQueue()
{
	RemoveAll();
}

template< class Type >
void AuQueue<Type>::RemoveAll()
{
	while( !IsEmpty() )
	{
		delete list.GetHead();
		list.RemoveHead();
	}
}

template< class Type >
BOOL AuQueue<Type>::Enqueue( Type Data )
{
	return list.AddTail( Data );
}

template< class Type >
Type & AuQueue<Type>::Dequeue()
{
	Type head = list.GetHead();
	list.RemoveHead();
	return head;
}

template< class Type >
int AuQueue<Type>::GetCount() const
{
	return list.GetCount();
}

template< class Type >
BOOL AuQueue<Type>::IsEmpty() const
{
	return list.IsEmpty();
}

template< class Type >
Type & AuQueue<Type>::PeekQueue() const
{
	return list.GetHead();
}

/////////////////////////////////////////////
// AuStack Implementation
/////////////////////////////////////////////
template< class Type >
AuStack<Type>::AuStack():list()
{

}

template< class Type >
AuStack<Type>::~AuStack()
{
	RemoveAll();
}

template< class Type >
void AuStack<Type>::RemoveAll()
{
	while( !IsEmpty() )
	{
		delete list.GetHead();
		list.RemoveHead();
	}
}

template< class Type >
BOOL AuStack<Type>::Push( Type Data )
{
	return list.AddHead( Data );
}

template< class Type >
Type & AuStack<Type>::Pop()
{
	Type Data = list.GetHead();
	list.RemoveHead();
	return Data;
}

template< class Type >
int AuStack<Type>::GetCount() const
{
	return list.GetCount();
}

template< class Type >
BOOL AuStack<Type>::IsEmpty() const
{
	return list.IsEmpty();
}

template< class Type >
Type & AuStack<Type>::Peek() const
{
	return list.GetHead();
}

#endif // #ifndef _AULIST_H_
