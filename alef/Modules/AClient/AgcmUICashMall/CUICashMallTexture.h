#ifndef __CLASS_UI_CASHMALL_TEXTURE_H__
#define __CLASS_UI_CASHMALL_TEXTURE_H__




#include "AcUIBase.h"
#include "AgpdItem.h"



class CUICashMallTexture : public AcUIBase
{
public :
	int													m_nItemID;
	int													m_nItemTID;
	int													m_nStackCount;

	AgpdItem*											m_ppdItem;
	BOOL												m_bIsEnableToolTip;

public :
	CUICashMallTexture( void );
	virtual ~CUICashMallTexture( void );

public :
	virtual void			OnSetFocus					( void );
	virtual void			OnKillFocus					( void );
	virtual void			OnWindowRender				( void );

public :
	void					SetItemTID					( int nTID, int nStackCount = 1 );
	void					SetTexture					( RwTexture* pTexture );
	void					SetTexture					( char* pFileName );
	void					IsEnableToolTip				( BOOL bIs ) { m_bIsEnableToolTip = bIs; }
};




#endif