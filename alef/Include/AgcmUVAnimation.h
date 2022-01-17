#ifndef __CLASS_UV_ANIMATION_H__
#define __CLASS_UV_ANIMATION_H__


#include <rwcore.h>
#include <rpworld.h>

#include "AgcModule.h"
#include "AcuRpUVAnimData.h"
#include <list>

class AgcmUVAnimation : public AgcModule
{
	struct stUVAnimationEntry
	{
		RpMaterial*								m_pMaterial;
		float									m_fCurrentTime;

		stUVAnimationEntry( void )
		{
			m_pMaterial = NULL;
			m_fCurrentTime = 0.0f;
		}
	};

	typedef std::list< stUVAnimationEntry >				ListUVAnimation;
	typedef std::list< stUVAnimationEntry >::iterator	ListUVAnimationIter;

private :
	ListUVAnimation						m_listUVAnimation;
	unsigned int						m_nPrevTickCount;

public :
	AgcmUVAnimation( void );
	virtual ~AgcmUVAnimation( void );

public :
	BOOL				OnAddModule				( void );
	BOOL				OnIdle					( unsigned int nCurrTickCount );
	BOOL				OnDestroy				( void );

	BOOL				UpdateUVAnimation		( float fSPF );
	BOOL				AddUVAnimation			( RpMaterial* pMaterial );
	BOOL				RemoveUVAnimation		( RpMaterial* pMaterial );
	BOOL				FindUVAnimation			( RpMaterial* pMaterial );
	BOOL				IsVaildUVAnimation		( RpMaterial* pMaterial );

	static VOID			Callback_UVConstructor	( void* pData , void* pClass , void* pCustData );
	static VOID			Callback_UVDestructor	( void* pData , void* pClass , void* pCustData );

	static BOOL			CallBack_AddAtomic		( void* pData, void* pClass, void* pCustData );
	static BOOL			CallBack_AddClump		( void* pData, void* pClass, void* pCustData );
	static BOOL			CallBack_DeleteAtomic	( void* pData, void* pClass, void* pCustData );
	static BOOL			CallBack_DeleteClump	( void* pData, void* pClass, void* pCustData );

	static RpAtomic*	CallBack_FindAtomic		( RpAtomic* pAtomic, void* pData );
	static RpGeometry*	CallBack_FindGeometry	( RpGeometry* pGeometry, void* pData );
	static RpMaterial*	CallBack_FindMaterial	( RpMaterial* pMaterial, void* pData );

	static RpAtomic*	CallBack_RemoveAtomic	( RpAtomic* pAtomic, void* pData );
	static RpGeometry*	CallBack_RemoveGeometry	( RpGeometry* pGeometry, void* pData );
	static RpMaterial*	CallBack_RemoveMaterial	( RpMaterial* pMaterial, void* pData );

private:
	static		AgcmUVAnimation*		m_pThis;

};

#endif
