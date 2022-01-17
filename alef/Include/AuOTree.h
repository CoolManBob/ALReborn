#ifndef _AUOTREE_H_
#define	_AUOTREE_H_

#include "ApBase.h"

// Leaf Indexing Order
//

class AuOTree
{
protected:
	int				m_nType		;	// 타입 저장.
	AuOTree *		m_pParent	;

	union
	{
		BOOL		bBlocking	;
		AuOTree *	pLeaf		;	// 8개 포인터로 잡아 넣을것임..
	} m_Data;

	void			SetUpLeaf	( float x , float y , float z , float width , float unitsize , BOOL bBlock , AuOTree *pParent );

public:
	// Range..
	float			m_fStartX	;
	float			m_fStartY	;
	float			m_fStartZ	;
	float			m_fWidth	;
	float			m_fUnitSize	;

	enum	TYPE
	{
		LEAF	,
		BRANCH	,
		NOT_READY
	};

	// Creator / Destructor
	AuOTree();
	~AuOTree();

	// Parn 작업, Destroy가 필요하다. 다 지우고 NOT_READY 상태로
	void		Destroy		();
	// Parn 작업, Init() 된건가?
	BOOL		IsReady		() const { return ( GetType() != NOT_READY ); }

	// Get Functions...
	int			GetType		() const { return m_nType		; }

	BOOL		IsRoot		() const { if( m_pParent ) return FALSE; else return TRUE; }
	// 현재 녀석이 Root 인지 점검..
	AuOTree *	GetRoot		() const ;
	// Root 를 얻어냄..


	//////////////////////////////////////////////////////////////
	// ROOT/LEAF Operations...
	//////////////////////////////////////////////////////////////

		// 리프 생성 루틴..
		BOOL		CreateLeaf	(	BOOL bBlock1 = TRUE,
									BOOL bBlock2 = TRUE,
									BOOL bBlock3 = TRUE,
									BOOL bBlock4 = TRUE,
									BOOL bBlock5 = TRUE,
									BOOL bBlock6 = TRUE,
									BOOL bBlock7 = TRUE,
									BOOL bBlock8 = TRUE
								);

		// Parn 작업
		// unitsize 까지 Leaf를 모두 생성해버린다.
		BOOL		CreateLeafAll	();

		// 각 리프에 벨류..
		// 디폴트값으로 TRUE 설정했음.. 알아서 직접 바꾸어 주어야함.

		BOOL		DeleteLeaf	(	BOOL bBlock = TRUE );
		// 이 노드에 연결돼어있는 모든 리프 삭제.
		// 인제는 삭제하고 난 후의 노드의 블러킹 정보.
		// 하위 노드를 리컬시브하게 삭제함..

	//////////////////////////////////////////////////////////////
	// ROOT Operations...
	//////////////////////////////////////////////////////////////
		BOOL		Init		( float x , float y , float z , float width , float unitsize , BOOL bBlock = TRUE );
		// 기본 루트 생성하는 펑션..  x,y,z 좌표를 넣고 , width * width * width 만큼의 입방체..
		void		Optimize	();
		// 같은 정보를 가진 리프를 합쳐 버린다. 생성시 한번만 실행한다.;

	//////////////////////////////////////////////////////////////
	// LEAF Operations...
	//////////////////////////////////////////////////////////////
		BOOL		SetBlocking	( BOOL bBlock					);
		// Parn 작업, Blocking 요소를 넣어서 하위 LEAF 노드에 대한 계산을 한다.
		BOOL		AddBlocking	( AuBLOCKING *pstBlocking		);

		// TRUE / FALSE
		// LEAF 에서만 설정 가능..
		BOOL		GetBlocking	( float x , float y , float z	) const ;
		AuOTree *	GetParent	( void							) const { return m_pParent;}
		AuOTree *	GetLeaf		( int index = 0					) const ;
		// 0~7 까지..
		// 에러시 널 리턴..
		// 0 이면 어레이를 리턴한다고 보면 ok...

	//////////////////////////////////////////////////////////////
	// Tree traveling functinos... 
	// 충돌 체크 펑션들..
	//////////////////////////////////////////////////////////////
		inline BOOL		IsInThisBox	( float x , float y , float z	) const 
		{
			if(	m_fStartX <= x && m_fStartX + m_fWidth > x	&&
				m_fStartY <= y && m_fStartY + m_fWidth > y	&&
				m_fStartX <= z && m_fStartZ + m_fWidth > z	)
					return TRUE		;
			else	return FALSE	;
		}
		// 이 포인트가 상자 안에 있나?..

};


#endif // #ifndef _AUOTREE_H_
