#include "AgcdEffCtrl.h"
#include <list>

typedef	std::list< EFFCTRL_SET* >				ListUseEffect;
typedef std::list< EFFCTRL_SET* >::iterator		ListUseEffectIter;

class AgcmEventEffectList
{
public:
	AgcmEventEffectList			( VOID ) 	{	m_pListUseEffect=NULL;				}
	~AgcmEventEffectList		( VOID )	{	DEF_SAFEDELETE( m_pListUseEffect );	}

private:
	AgcmEventEffectList& operator = (const AgcmEventEffectList& cpy) { cpy; return *this; };

public:
	BOOL				CreateList				( VOID );
	BOOL				DestroyUseEffectList	( VOID );
	BOOL				AddUseEffectList		( EFFCTRL_SET* pNode );
	BOOL				CheckUseEffect			( EFFCTRL_SET* pNode );
	BOOL				RemoveUseEffectList		( EFFCTRL_SET* pNode );

public:
	ListUseEffect*		m_pListUseEffect;
};