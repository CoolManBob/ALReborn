#include "MagDebug.h"
#include "AuOTree.h"
#include "ApMemoryTracker.h"

AuOTree::AuOTree() : m_nType ( NOT_READY ) , m_pParent ( NULL )
{
	// 초기화지롱..
}

AuOTree::~AuOTree()
{
	// 
	DeleteLeaf();
}

// Parn 작업, Destroy가 필요하다. 다 지우고 NOT_READY 상태로
void		AuOTree::Destroy		()
{
	DeleteLeaf();

	m_nType = NOT_READY;
	m_pParent = NULL;
}

AuOTree *	AuOTree::GetRoot		() const
{
	ASSERT( GetType() == LEAF || GetType() == BRANCH );

	AuOTree * pParent = ( AuOTree * ) this;

	// 페어런트를 찾음..
	while( pParent->GetParent() )
	{
		pParent = GetParent();
	}

	return pParent;
}

BOOL		AuOTree::Init		( float x , float y , float z , float width , float unitsize , BOOL bBlock )
{
	ASSERT( GetType() == NOT_READY );
	if( GetType() != NOT_READY )
	{
		return FALSE;
	}

	m_fStartX			= x			;
	m_fStartY			= y			;
	m_fStartZ			= z			;
	m_fWidth			= width		;
	m_fUnitSize			= unitsize	;

	m_nType				= LEAF		;
	m_pParent			= NULL		;	// 페어런트는 없으니까.

	m_Data.bBlocking	= bBlock	;

	return TRUE;
}


void		AuOTree::Optimize	()
{
	// Do no op yet...
	ASSERT( GetType() == LEAF || GetType() == BRANCH );

	if( GetType() == LEAF )
	{
		// do no op..
		return;
	}
	else
	{
		// BRANCH
		ASSERT( NULL != m_Data.pLeaf );

		int		countLeaf = 0	;
		int		nBlocking = 0	;

		// Leaf 갯수를 
		for( int i = 0 ; i < 8 ; ++i )
		{
			ASSERT( LEAF == m_Data.pLeaf[ i ].GetType() || BRANCH == m_Data.pLeaf[ i ].GetType() );

			// BRANCH라면.. 옵티마이즈를 먼저함..
			if( BRANCH == m_Data.pLeaf[ i ].GetType() )
				m_Data.pLeaf[ i ].Optimize();

			// 옵티마이즈 한 결과를 점검..
			if( LEAF == m_Data.pLeaf[ i ].GetType() )
			{
				++countLeaf ;

				// Blocking 일경우 
				if( m_Data.pLeaf[ i ].m_Data.bBlocking )
					++nBlocking ;
			}
			else
			{
				// BRANCH
				// Do no op
			}
		}

		// 전부다 Leaf면..
		if( countLeaf == 8 )
		{
			if( nBlocking == 8 )
			{
				// 전부 TRUE
				DeleteLeaf( TRUE	);
			}
			else if( nBlocking == 0 )
			{
				// 전부 FALSE
				DeleteLeaf( FALSE	);
			}
		}

		// 전부 Leaf가 아니면 아무일 안함..
		return;
	}
}

BOOL		AuOTree::CreateLeaf	(	BOOL bBlock1,
										BOOL bBlock2,
										BOOL bBlock3,
										BOOL bBlock4,
										BOOL bBlock5,
										BOOL bBlock6,
										BOOL bBlock7,
										BOOL bBlock8
									)
{
	ASSERT( GetType() == LEAF		);
	ASSERT( m_fWidth > m_fUnitSize	);

	if( ( GetType() != LEAF			) ||
		( m_fWidth <= m_fUnitSize	)	)
	{
		// 머시라고 -_-;;
		return FALSE;
	}

	AuOTree *	pTree	= new AuOTree[ 8 ]	;
	float		fWidth	= m_fWidth / 2.0f		;

	ASSERT( NULL != pTree );

	// Set Up Leaves..

	pTree[ 0 ].SetUpLeaf(	m_fStartX			,	m_fStartY			,	m_fStartZ			, fWidth , m_fUnitSize , bBlock1 , this );
	pTree[ 1 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY			,	m_fStartZ			, fWidth , m_fUnitSize , bBlock2 , this );
	pTree[ 2 ].SetUpLeaf(	m_fStartX			,	m_fStartY			,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock3 , this );
	pTree[ 3 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY			,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock4 , this );
	pTree[ 4 ].SetUpLeaf(	m_fStartX			,	m_fStartY + fWidth	,	m_fStartZ			, fWidth , m_fUnitSize , bBlock5 , this );
	pTree[ 5 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY + fWidth	,	m_fStartZ			, fWidth , m_fUnitSize , bBlock6 , this );
	pTree[ 6 ].SetUpLeaf(	m_fStartX			,	m_fStartY + fWidth	,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock7 , this );
	pTree[ 7 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY + fWidth	,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock8 , this );

	// 브렌치로 변경됨..
	m_nType			= BRANCH	;
	m_Data.pLeaf	= pTree		;

	return TRUE;
}

// Parn 작업한 CreateLeafAll()
BOOL		AuOTree::CreateLeafAll	()
{
	ASSERT( GetType() == LEAF		);
	ASSERT( m_fWidth > m_fUnitSize	);

	if( ( GetType() != LEAF			) ||
		( m_fWidth  <= m_fUnitSize	)	)
	{
		// 머시라고 -_-;;
		return FALSE;
	}

	if ( !CreateLeaf() )
		return FALSE;

	for( int i = 0 ; i < 8 ; ++i )
	{
		m_Data.pLeaf[ i ].CreateLeafAll();
	}

	return TRUE;
}

void		AuOTree::SetUpLeaf	( float x , float y , float z , float width , float unitsize , BOOL bBlock , AuOTree *pParent )
{
	ASSERT( GetType() == NOT_READY );

	m_fStartX			= x			;
	m_fStartY			= y			;
	m_fStartZ			= z			;
	m_fWidth			= width		;
	m_fUnitSize			= unitsize	;

	m_nType				= LEAF		;
	m_pParent			= pParent	;	// 페어런트는 없으니까.

	m_Data.bBlocking	= bBlock	;
}

BOOL		AuOTree::DeleteLeaf	(	BOOL bBlock  )
{
	// 초기화 돼어 있는지 점검..
	ASSERT( GetType() == LEAF || GetType() == BRANCH );
	if( GetType() != LEAF && GetType() != BRANCH ) return FALSE;

	if( GetType() == LEAF )
	{
		m_Data.bBlocking = bBlock;
		return TRUE;
	}
	else
	{
		// BRANCH
		ASSERT( NULL != m_Data.pLeaf );

		for( int i = 0 ; i < 8 ; ++i )
		{
			m_Data.pLeaf[ i ].DeleteLeaf( bBlock );
		}
		// 메모리 제거.
		delete [] m_Data.pLeaf;

		m_nType				= LEAF		;
		m_Data.bBlocking	= bBlock	;

		return TRUE;
	}

}

BOOL		AuOTree::SetBlocking	( BOOL bBlock					)
{
	// LEAF에서만 설정이 가능하도다.
	ASSERT( GetType() == LEAF );
	if( GetType() != LEAF )
	{
		// -_-;;;..

		return FALSE;
	}

	BOOL	bPrev		= m_Data.bBlocking	;
	m_Data.bBlocking	= bBlock			;
	return bPrev;
}

// Parn 작업, Blocking 요소를 넣어서 하위 LEAF 노드에 대한 계산을 한다.
BOOL		AuOTree::AddBlocking	( AuBLOCKING *pstBlocking		)
{
	ASSERT( NULL != pstBlocking );

	// Leaf이면, 현재 사각형의 중점이 Blocking인지 아닌지를 Test해서 Blocking정보를 만든다.
	if( GetType() == LEAF )
	{
		AuPOS	center;

		center.x = m_fStartX + (float) ( m_fWidth / 2.0 );
		center.y = m_fStartY + (float) ( m_fWidth / 2.0 );
		center.z = m_fStartZ + (float) ( m_fWidth / 2.0 );

		switch( pstBlocking->type )
		{
		case AUBLOCKING_TYPE_BOX:
			m_Data.bBlocking	= AUTEST_POS_IN_BOX(center, pstBlocking->data.box);
			break;
		case AUBLOCKING_TYPE_SPHERE:
			m_Data.bBlocking	= AUTEST_POS_IN_SPHERE(center, pstBlocking->data.sphere);
			break;
		case AUBLOCKING_TYPE_CYLINDER:
			m_Data.bBlocking	= AUTEST_POS_IN_CYLINDER(center, pstBlocking->data.cylinder);
			break;
		}

		return TRUE;
	}

	// Leaf가 아니면, recursive하게 계속 Call
	for( int i = 0 ; i < 8 ; ++i )
		if( m_Data.pLeaf )
			m_Data.pLeaf[ i ].AddBlocking( pstBlocking );

	return TRUE;
}

AuOTree *	AuOTree::GetLeaf		( int index 					) const
{
	// 브렌치여야 하고 , 인덱스의 범위는 8 밑으로여야한다.
	ASSERT( GetType() == BRANCH		);
	ASSERT( index >= 0 && index < 8	);
	if( GetType() != BRANCH || index < 0 || index >= 8 ) return NULL;

	return m_Data.pLeaf + index;
}

BOOL		AuOTree::GetBlocking	( float x , float y , float z	) const
{
	ASSERT( GetType() == LEAF || GetType() == BRANCH );
	if( GetType() != LEAF && GetType() != BRANCH ) return FALSE;

	AuOTree	* pLeaf			;

	pLeaf	= GetLeaf();

	ASSERT( NULL != pLeaf );

	for( int i = 0 ; i < 8 ; ++i )
	{
		if( pLeaf[ i ].IsInThisBox( x , y , z ) )
		{
			return pLeaf[ i ].GetBlocking( x , y , z );
		}
	}

	ASSERT( !"옥트리 정보가 이상합니다. 블러킹 정보를 찾을수가 없음." );

	return FALSE;
}
