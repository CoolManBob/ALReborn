#ifndef __CLASS_UI_GRID_ITEM_H__
#define __CLASS_UI_GRID_ITEM_H__



#include "AcUIMessageDialog.h"



enum eUiGridSlotItemType
{
	UiGridItem_UnKnown = 0,
	UiGridItem_Item,
	UiGridItem_Skill,
};


class CUiGridSlot : public AcUIBase
{
private :
	int											m_nSlotIndex;
	eUiGridSlotItemType							m_eSlotItemType;

	int											m_nTID;
	int											m_nlID;

	void*										m_pSlotItemTexture;

public :
	CUiGridSlot( void );
	virtual ~CUiGridSlot( void );

public :
	virtual void		OnWindowRender			( void );
	virtual BOOL		OnDragDrop				( void* pParam1, void* pParam2 );

public :
	void				OnClear					( void );

public :
	BOOL				Initialize				( int nMsgBoxWidth, int nMsgBoxHeight );

	int					GetItemID				( void ) { return m_nlID; }
	int					GetItemTID				( void ) { return m_nTID; }
	void*				GetItemTexture			( void ) { return m_pSlotItemTexture; }
};


#endif