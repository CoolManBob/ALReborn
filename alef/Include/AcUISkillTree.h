#ifndef		_ACUISKILLTREE_H_
#define		_ACUISKILLTREE_H_

#include "AcUIGrid.h"

// AcUISkillTree의 주요 특징 
// - Window Size : Width , Height 자동 설정 된다
// - 

// Command Message
enum 
{
	UICM_SKILLTREE_MAX_MESSAGE							= UICM_GRID_MAX_MESSAGE,
};


typedef enum
{
	ACUI_SKILLTREE_TYPE_1_3								= 0, // 앞에꺼가 Row 뒤에꺼가 Column (곧, 1_3은 1 Row 3 Column)
	ACUI_SKILLTREE_TYPE_2_3,
	ACUI_SKILLTREE_TYPE_3_3,
	ACUI_SKILLTREE_TYPE_1_10,
	ACUI_SKILLTREE_TYPE_1_2,
	ACUI_SKILLTREE_TYPE_2_2,
	ACUI_SKILLTREE_MAX_TYPE,
} AcUISkillTreeType;


class AcUISkillTree : public AcUIGrid 
{
public:
	INT32												m_lSkillTreeMode;	
	INT32												m_lOneRowItemNum;		// Skill Tree내에서 한 Row당 최대 Item 수 - 초기는 3
	INT32												m_alSkillBackImageID[ ACUI_SKILLTREE_MAX_TYPE ];

	AcUISkillTree( void );
	virtual ~AcUISkillTree( void );

public:
	virtual void			MoveGridItemWindow			( void );

	BOOL					SetSkillBackImage			( AcUISkillTreeType eType, INT32 lImageID );
	INT32					GetSkillBackImage			( AcUISkillTreeType eType );

private :
	void					_MoveGridSkill				( void );
	void					_MoveGridSkillOneLineMode	( void );
	void					_UpdateSkillTreeSize		( BOOL bOneLineTreeMode );
};

#endif		// _ACUISKILLTREE_H_