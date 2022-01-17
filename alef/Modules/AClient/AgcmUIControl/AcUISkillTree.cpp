#include "AcUISkillTree.h"



AcUISkillTree::AcUISkillTree( void )
{
	m_lOneRowItemNum	= 3;
	m_nType				= TYPE_SKILL_TREE;
	m_bMoveItemCopy		= TRUE;		// Item 옮길때 Window를 Copy해서 둔다 
}

AcUISkillTree::~AcUISkillTree( void )
{
}

void AcUISkillTree::MoveGridItemWindow( void )
{
	if( !m_pAgpdGrid || !m_apGridItemWindow ) return;

	BOOL bOneLineTreeMode = FALSE;
	if( m_pAgpdGrid->m_nRow == 1 )
	{
		bOneLineTreeMode = TRUE;
	}

	// AcUIGridItem 배치 
	if( bOneLineTreeMode )	// One Line Skill Tree 구조인 경우 
	{
		_MoveGridSkillOneLineMode();
	}
	else	
	{
		_MoveGridSkill();
	}

	// AcUISkillTree 전체 크기 결정 
	_UpdateSkillTreeSize( bOneLineTreeMode );
}

BOOL AcUISkillTree::SetSkillBackImage( AcUISkillTreeType eType, INT32 lImageID )
{
	if( eType < 0 || eType >= ACUI_SKILLTREE_MAX_TYPE )	return FALSE;
	m_alSkillBackImageID[ eType ] = lImageID;
	return TRUE;
}

INT32 AcUISkillTree::GetSkillBackImage( AcUISkillTreeType eType )
{
	if( eType < 0 || eType >= ACUI_SKILLTREE_MAX_TYPE )	return -1;
	return m_alSkillBackImageID[ eType ];
}


void AcUISkillTree::_MoveGridSkill( void )
{
	for( INT32 nRowCount = 0 ; nRowCount < m_pAgpdGrid->m_nRow ; ++nRowCount )
	{
		for( INT32 nColCount = 0 ; nColCount < m_pAgpdGrid->m_nColumn ; ++nColCount )
		{
			INT32 lIndex = ( nRowCount * m_pAgpdGrid->m_nColumn ) + nColCount;
			INT32 nPosX = m_lGridItemStart_x + nColCount * ( m_lGridItemGap_x + m_lGridItemWidth );
			INT32 nPosY = m_lGridItemStart_y + nRowCount * ( m_lGridItemGap_y + m_lGridItemHeight );

			m_apGridItemWindow[ lIndex ]->MoveWindow( nPosX, nPosY, w, h );
			m_apGridItemWindow[ lIndex ]->m_Property.bMovable = TRUE;
		}
	}

	// 첫번째 Skill Window 위치 내려주기 
	if( m_pAgpdGrid->m_nRow == 2 && m_pAgpdGrid->m_nColumn == 3 )
	{
		INT32 nPosX = m_apGridItemWindow[ 0 ]->x;
		INT32 nPosY = m_apGridItemWindow[ 0 ]->y + ( m_lGridItemHeight + m_lGridItemGap_y ) / 2;

		m_apGridItemWindow[0]->MoveWindow( nPosX, nPosY, w, h );
		
		// RenderInfo - RenderID Setting 
		m_stRenderInfo.lRenderID = GetSkillBackImage( ACUI_SKILLTREE_TYPE_2_3 );
	}
	else if ( m_pAgpdGrid->m_nRow == 3 && m_pAgpdGrid->m_nColumn == 3 )
	{
		INT32 nPosX = m_apGridItemWindow[ 0 ]->x;
		INT32 nPosY = m_lGridItemStart_y + ( m_lGridItemGap_y + m_lGridItemHeight );

		m_apGridItemWindow[0]->MoveWindow( nPosX, nPosY, w, h );

		// RenderInfo - RenderID Setting 
		m_stRenderInfo.lRenderID = GetSkillBackImage( ACUI_SKILLTREE_TYPE_3_3 );
	}
	else if ( m_pAgpdGrid->m_nRow == 2 && m_pAgpdGrid->m_nColumn == 2 )
	{
		// RenderInfo = RenderIF Setting 
		m_stRenderInfo.lRenderID = GetSkillBackImage( ACUI_SKILLTREE_TYPE_2_2 );
	}
}

void AcUISkillTree::_MoveGridSkillOneLineMode( void )
{
	INT32 nRowCount = 1;
	INT32 nRest	= 0;		

	for( INT32 nColCount = 0 ; nColCount < m_pAgpdGrid->m_nColumn ; ++nColCount )
	{
		// Line 결정 
		nRowCount = ( INT32 )( nColCount / m_lOneRowItemNum ) + 1;
		nRest = nColCount % m_lOneRowItemNum			;
		
		if( ( nRowCount % 2 ) == 1 )	// 홀수열 일때 
		{
			INT32 nPosX = m_lGridItemStart_x + nRest * ( m_lGridItemGap_x + m_lGridItemWidth );
			INT32 nPosY = m_lGridItemStart_y + ( nRowCount - 1 ) * ( 15 + m_lGridItemHeight );

			m_apGridItemWindow[ nColCount ]->MoveWindow( nPosX, nPosY,
				m_apGridItemWindow[ 0 ]->w, m_apGridItemWindow[ 0 ]->h );
		}
		else						// 짝수열 일때 
		{
			INT32 nPosX = m_lGridItemStart_x + ( m_lOneRowItemNum - nRest - 1 ) * ( m_lGridItemGap_x + m_lGridItemWidth );
			INT32 nPosY = m_lGridItemStart_y + ( nRowCount - 1 ) * ( 15 + m_lGridItemHeight );

			m_apGridItemWindow[ nColCount ]->MoveWindow( nPosX, nPosY,
				m_apGridItemWindow[ 0 ]->w, m_apGridItemWindow[ 0 ]->h );
		}
	}

	// RenderInfo - RenderID Setting 
	if( m_pAgpdGrid->m_nColumn <= 3 )
	{
		m_stRenderInfo.lRenderID = GetSkillBackImage( ACUI_SKILLTREE_TYPE_1_3 );
	}
	else if( m_pAgpdGrid->m_nColumn == 10 )
	{
		m_stRenderInfo.lRenderID = GetSkillBackImage( ACUI_SKILLTREE_TYPE_1_10 );
	}

	// 10개 짜리일때 마지막 아이템 중간 정렬
	if( m_pAgpdGrid->m_nColumn == 10 )
	{
		INT32 nPosX = m_lGridItemStart_x + ( m_lGridItemGap_x + m_lGridItemWidth );
		INT32 nPosY = m_apGridItemWindow[ 9 ]->y + 7;

		m_apGridItemWindow[ 9 ]->MoveWindow( nPosX, nPosY, w, h );
	}

	if( m_pAgpdGrid->m_nColumn == 0 ) return;
}

void AcUISkillTree::_UpdateSkillTreeSize( BOOL bOneLineTreeMode )
{
	w = m_lGridItemStart_x * 2 + m_lOneRowItemNum * m_lGridItemWidth + ( m_pAgpdGrid->m_nColumn - 1 ) * m_lGridItemGap_x;
	
	if( bOneLineTreeMode )	// One Line 인 경우 
	{
		h = m_lGridItemStart_y * 2 + ( ( INT32 )( m_pAgpdGrid->m_nColumn / m_lOneRowItemNum ) + 1 ) * m_lGridItemHeight + ( INT32 )( m_pAgpdGrid->m_nColumn / m_lOneRowItemNum ) * m_lGridItemGap_y;
	}
	else
	{
		h = m_lGridItemStart_y * 2 + m_pAgpdGrid->m_nRow * m_lGridItemHeight + ( m_pAgpdGrid->m_nRow - 1 ) * m_lGridItemGap_y;
	}
}
