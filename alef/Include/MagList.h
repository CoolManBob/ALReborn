///////////////////////////////////////////////////////////////////////////////////////////
// Double Linked List Template Class;
// 2000/06/14 Taken from Yong
// 2000/06/23 CMagQueue , CMagStack is made from the CMagList by Magoja
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
//	CMagList<int>	list;
//	for( int i = 0 ; i < 10 ; i ++ )
//		list.AddTail( i );
//
//	CMagNode<int> *pNode = list.GetHeadNode();
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
//	CMagList<int>	list;
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

#ifndef _MAG_LINKED_LIST_H_
#define	_MAG_LINKED_LIST_H_

#include <windows.h>

// Node structure for CMagList data store
// Do not use it directly
template< class Type >
class CMagNode
{
protected :
	Type				m_Data	;
	CMagNode<Type> *	m_pNext	;
	CMagNode<Type> *	m_pPrev	;
public :
	CMagNode	( Type Data );
	~CMagNode	(			);

	// Managing this node links
	void LinkPrev		( CMagNode<Type> * pNode	);
	void UnlinkPrev		(							);
	void LinkNext		( CMagNode<Type> * pNode	);
	void UnlinkNext		(							);

	// Get prev or next node;
	CMagNode<Type> *	GetPrevNode() const;
	CMagNode<Type> *	GetNextNode() const;

	// get data , this node hold
	Type & GetData();
};

// Main List Class.
template< class Type >
class CMagList
{
protected :
	CMagNode<Type> *	m_pHead		;
	CMagNode<Type> *	m_pTail		;
	int					m_nCount	;

public :
	CMagList();
	// If the element is an allocated pointer and you want it to be destroied 
	// with CMagList , you may set bDataSelDestruct TRUE to order self derstruction
	// if bDataSelfDestruct is TRUE , NULL cannot be inserted!.

	CMagList( const CMagList & param ); // Copy constructor
										// it will generate exception , when m_bDataSelfDestruct flag is on
	~CMagList();

	BOOL			AddHead(Type Data); // Store the data in the top of the list
	BOOL			AddTail(Type Data); // Insert the data in the bottom

	CMagNode<Type>*	InsertBefore	( CMagNode<Type> *pos , Type Data );
	CMagNode<Type>*	InsertAfter		( CMagNode<Type> *pos , Type Data );

	BOOL			RemoveHead	(						); // Remove Head node
	BOOL			RemoveTail	(						); // Remove Tail node
	BOOL			RemoveNode	( CMagNode<Type>* node	); // Remove Specific node
	BOOL			RemoveData	( Type Data				); // Remove Specific data
	void			RemoveAll	(						); // Clear all the contents

	CMagNode<Type>*	GetHeadNode() const;
	CMagNode<Type>*	GetTailNode() const;

	CMagNode<Type>*	GetNode( Type Data ) const;// Find a specific node with certain data ,
		// if impossible to find that , it will return NULL;
	Type &			GetHead		() const;// Get Head value;
	Type &			GetTail		() const;// Get Tail value;

	Type &			GetPrev		( CMagNode<Type>*& node );// Get CURRENT node value and move to prev node. not a prev one
	Type &			GetNext		( CMagNode<Type>*& node );// Get CURRENT node value and move to next node. not a next one

	int				GetCount	() const;// Get how many element this class hold.
	BOOL			IsEmpty		() const;// Determine this class have nothing.

	// Overloaded operator with CMagList;
	// this will generate exception , when m_bDataSelfDestruct flag is on
	CMagList &		operator=	( const CMagList & param	);
	CMagList &		operator+=	( CMagList & param			);

	// For useing this list like standard array..
	Type &			operator[]	( int offset								);
	BOOL			Alloc		( int max_array_size , Type initialvalue	); // 최대치 지정. 

	friend CMagList	operator+	( CMagList & param1 , CMagList & param2 );
};

template< class Type >
class CMagQueue  
{
private:
	CMagList<Type>	list;
public:
	Type &	PeekQueue	(			) const;
	BOOL	IsEmpty		(			) const;
	int		GetCount	(			) const;
	Type &	Dequeue		(			);
	BOOL	Enqueue		( Type Data	);
	void	RemoveAll	(			);

	CMagQueue();
	virtual ~CMagQueue();
};

template< class Type >
class CMagStack  
{
private:
	CMagList<Type>	list;
public:
	Type &	Peek		(			) const;
	BOOL	IsEmpty		(			) const;
	int		GetCount	(			) const;
	Type &	Pop			(			);
	BOOL	Push		( Type Data	);
	void	RemoveAll();

	CMagStack();
	virtual ~CMagStack();
};

template< class Type >
CMagNode<Type>::CMagNode( Type Data )
{
	m_Data	= Data;
	m_pPrev	= NULL;
	m_pNext	= NULL;
}

template< class Type >
CMagNode<Type>::~CMagNode()
{
	// do no op
}
	  
template< class Type >
void CMagNode<Type>::LinkPrev( CMagNode<Type> * pNode )
{
	m_pPrev = pNode;
}

template< class Type >
void CMagNode<Type>::LinkNext( CMagNode<Type> * pNode )
{
	m_pNext = pNode;
}

template< class Type >
void CMagNode<Type>::UnlinkPrev()
{
	m_pPrev = NULL;
}

template< class Type >
void CMagNode<Type>::UnlinkNext()
{
	m_pNext = NULL;
}

template< class Type >
CMagNode<Type> * CMagNode<Type>::GetPrevNode() const
{
	return m_pPrev;
}

template< class Type >
CMagNode<Type> * CMagNode<Type>::GetNextNode() const
{
	return m_pNext;
}

template< class Type >
Type & CMagNode<Type>::GetData()
{
	return m_Data;
}

template< class Type >
CMagList<Type>::CMagList()
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;
}

template< class Type >
CMagList<Type>::CMagList( const CMagList<Type> & param ) // Copy constructor
{
	// 리스트 먼저 초기화 시키고
	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;

	CMagNode<Type> *pNode = param.m_pHead;//.GetHeadNode();

	while( pNode )
	{
		AddTail( pNode->GetData() );
		pNode = pNode->GetNextNode();
	}
}

template< class Type >
CMagList<Type>::~CMagList()
{
	RemoveAll();
}

template< class Type >
BOOL CMagList<Type>::AddHead( Type Data )
{
	CMagNode<Type> * pNode = new CMagNode<Type> ( Data );

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

	m_nCount++;

	return TRUE;
}

template< class Type >
BOOL CMagList<Type>::AddTail( Type Data )
{
	CMagNode<Type>* pNode = new CMagNode<Type>( Data );

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

	m_nCount++;

	return TRUE;
}

template< class Type >
BOOL CMagList<Type>::RemoveHead()
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
BOOL CMagList<Type>::RemoveTail()
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
BOOL CMagList<Type>::RemoveNode( CMagNode<Type> * pNode)
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
				delete pNode;
				pNode = NULL;
				return TRUE;
			}
			else
			{
				// node가 여러개 있을때.
				m_pHead = pNode->GetNextNode();
				m_pHead->UnlinkPrev();
				m_nCount--;
				delete pNode;
				pNode = NULL;
				return TRUE;
			}
		}
		else if (pNode == m_pTail)
		{
			m_pTail = pNode->GetPrevNode();
			m_pTail->UnlinkNext();
			m_nCount--;
			delete pNode;
			pNode = NULL;
			return TRUE;
		}
		else
		{
			CMagNode<Type> * m_pCur = m_pHead;

			while ((m_pCur != NULL) && ((m_pCur->GetNextNode()) != NULL))
			{
				if ((m_pCur->GetNextNode()) == pNode)
				{
					m_pCur->LinkNext(pNode->GetNextNode());
					(pNode->GetNextNode())->LinkPrev(m_pCur);
					m_nCount--;
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
BOOL CMagList<Type>::RemoveData( Type Data )
{
	CMagNode<Type> * pCur = m_pHead;

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
void CMagList<Type>::RemoveAll()
{
	while( RemoveHead() );

	m_pHead = NULL;
	m_pTail = NULL;
	m_nCount = 0;
}

template< class Type >
CMagNode<Type> * CMagList<Type>::GetHeadNode() const
{
	return m_pHead;
}

template< class Type >
CMagNode<Type> * CMagList<Type>::GetTailNode() const
{
	return m_pTail;
}

template< class Type >
CMagNode<Type> * CMagList<Type>::GetNode( Type Data) const
{
	CMagNode<Type>	*pNode = GetHeadNode();
	while( pNode )
	{
		if( pNode->GetData() == Data ) return pNode;
		pNode = pNode->GetNextNode();
	}
	return NULL;
}

template< class Type >
Type & CMagList<Type>::GetHead() const
{
	if (m_pHead == NULL)
	{
		//throw "Cannot Get Head , cause there is no data\n";
		return NULL;
	}

	return ( m_pHead->GetData() );
}

template< class Type >
Type & CMagList<Type>::GetTail() const
{
	if (m_pTail == NULL)
	{
		throw "CMagList::GetTail - List is empty!!!\n";
	}

	return ( m_pTail->GetData() );
}

template< class Type >
Type & CMagList<Type>::GetPrev( CMagNode<Type> *& pNode )
{
	if (pNode == NULL)
	{
//		return NULL;
		throw "CMagList::GetPrev - List is empty!!!\n";
	}

	CMagNode<Type> * pCur = pNode;

	pNode = pNode->GetPrevNode();

	return (pCur->GetData());
}

template< class Type >
Type & CMagList<Type>::GetNext( CMagNode<Type> *& pNode )
{
	if (pNode == NULL)
	{
		// 없는데이타는 마지막 값을 리턴.
		return GetTail();
	}

	CMagNode<Type> * pCur = pNode;

	pNode = pNode->GetNextNode();

	return (pCur->GetData());
}

template< class Type >
int CMagList<Type>::GetCount() const
{
	return m_nCount;
}

template< class Type >
BOOL CMagList<Type>::IsEmpty() const
{
	if (m_nCount == 0)
		return TRUE;
	
	return FALSE;
}

template< class Type >
CMagNode<Type> * CMagList<Type>::InsertBefore( CMagNode<Type> *pos , Type Data )
{
	CMagNode<Type> * pNode = new CMagNode<Type>( Data );

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
		CMagNode<Type> *pPrevNode;
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

	m_nCount++;

	return pNode;
}

template< class Type >
CMagNode<Type> * CMagList<Type>::InsertAfter( CMagNode<Type> *pos , Type Data )
{
	CMagNode<Type> * pNode = new CMagNode<Type> ( Data );

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
		CMagNode<Type> *pNextNode;
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

	m_nCount++;

	return pNode;
}

template< class Type >
CMagList<Type> & CMagList<Type>::operator= ( const CMagList<Type> & param )
{
	// 우선 데이타를 다 날리고.
	RemoveAll();
	
	CMagNode<Type> *pNode = param.m_pHead;

	while( pNode )
	{
		AddTail( pNode->GetData() );
		pNode = pNode->GetNextNode();
	}

	return *this;
}

template< class Type >
CMagList<Type> & CMagList<Type>::operator+= ( CMagList<Type> & param )
{
	// 리스트를 더함.
	CMagNode<Type> *pNode = param.GetHeadNode();

	while( pNode )
	{
		AddTail( param.GetNext( pNode ) );
	}

	return *this;
}

template< class Type >
CMagList<Type> operator+ ( CMagList<Type> & param1 , CMagList<Type> & param2 )
{
	CMagList<Type> list;

	CMagNode<Type> *pNode = param1.GetHeadNode();

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

#pragma warning( push )
#pragma warning( disable:4700 )

template< class Type >
Type & CMagList<Type>::operator[] ( int offset )
{
	if( offset < 0 ) throw "CMagList - Offset 이 0보다 작다.\n";
	if( offset >= m_nCount )
	{
#ifdef	_DEBUG
		OutputDebugString( "CMagList<Type>::operator[] 범위초과\n" );
#endif
		Type dummy;
		while( offset != GetCount() - 1 )
		{
			// 더미데이타를 채워서 리스트를 작성한다.
			// 여기서 워닝 하나 발생.
			// 문제가 생길 가능성 다분 -_-;
			this->AddTail( dummy );
		}

		return this->GetTail();
	}

	CMagNode<Type> *pNode = this->GetHeadNode();
	int count = 0;

	while( pNode )
	{
		if( offset == count ) return pNode->GetData();
		pNode = pNode->GetNextNode();
		count ++;
	}

	throw "CMagList::operator[] - Strange Value\n";
}
#pragma warning( pop )

template< class Type >
BOOL CMagList<Type>::Alloc ( int max_array_size , Type initialvalue	) // 최대치 지정.
{
	while( max_array_size != GetCount() - 1 )
	{
		// 더미데이타를 채워서 리스트를 작성한다.
		if( !this->AddTail( initialvalue ) ) return FALSE;
	}
	return TRUE;
}
///////////////////////////////////////
// CMagQueue Implementation
///////////////////////////////////////
template< class Type >
CMagQueue<Type>::CMagQueue()
{

}

template< class Type >
CMagQueue<Type>::~CMagQueue()
{
	RemoveAll();
}

template< class Type >
void CMagQueue<Type>::RemoveAll()
{
	while( !IsEmpty() )
	{
		delete list.GetHead();
		list.RemoveHead();
	}
}

template< class Type >
BOOL CMagQueue<Type>::Enqueue( Type Data )
{
	return list.AddTail( Data );
}

template< class Type >
Type & CMagQueue<Type>::Dequeue()
{
	Type head = list.GetHead();
	list.RemoveHead();
	return head;
}

template< class Type >
int CMagQueue<Type>::GetCount() const
{
	return list.GetCount();
}

template< class Type >
BOOL CMagQueue<Type>::IsEmpty() const
{
	return list.IsEmpty();
}

template< class Type >
Type & CMagQueue<Type>::PeekQueue() const
{
	return list.GetHead();
}

/////////////////////////////////////////////
// CMagStack Implementation
/////////////////////////////////////////////
template< class Type >
CMagStack<Type>::CMagStack():list()
{

}

template< class Type >
CMagStack<Type>::~CMagStack()
{
	RemoveAll();
}

template< class Type >
void CMagStack<Type>::RemoveAll()
{
	while( !IsEmpty() )
	{
		delete list.GetHead();
		list.RemoveHead();
	}
}

template< class Type >
BOOL CMagStack<Type>::Push( Type Data )
{
	return list.AddHead( Data );
}

template< class Type >
Type & CMagStack<Type>::Pop()
{
	Type Data = list.GetHead();
	list.RemoveHead();
	return Data;
}

template< class Type >
int CMagStack<Type>::GetCount() const
{
	return list.GetCount();
}

template< class Type >
BOOL CMagStack<Type>::IsEmpty() const
{
	return list.IsEmpty();
}

template< class Type >
Type & CMagStack<Type>::Peek() const
{
	return list.GetHead();
}

#endif // #ifndef _MAG_LINKED_LIST_H_
