#ifndef	__AGSMEVENTITEMCONVERT_H__
#define	__AGSMEVENTITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventItemConvertD" )
#else
#pragma comment ( lib , "AgsmEventItemConvert" )
#endif
#endif

#include "AgpmEventItemConvert.h"

#include "AgsmCharacter.h"

class AgsmEventItemConvert : public AgsModule {
private:
	AgpmEventItemConvert	*m_pcsAgpmEventItemConvert;
	AgsmCharacter			*m_pcsAgsmCharacter;

public:
	AgsmEventItemConvert();
	virtual ~AgsmEventItemConvert();

	BOOL					OnAddModule();

	static BOOL				CBItemConvertRequest(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGSMEVENTITEMCONVERT_H__