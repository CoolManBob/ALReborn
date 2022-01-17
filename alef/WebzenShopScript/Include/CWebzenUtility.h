#ifndef __CLASS_WEBZEN_UTILITY_H__
#define __CLASS_WEBZEN_UTILITY_H__


/*
#include <Winsock2.h>
#include "Windows.h"
#include "ContainerUtil.h"



typedef void ( *fnMsgCallBack )( char* pMsg );

class CWebzenUtility
{
private :
	CRITICAL_SECTION							m_cs;
	fnMsgCallBack								m_fnMsgCallBack;

public :
	CWebzenUtility( void );
	virtual ~CWebzenUtility( void );

public :
	void			WriteFileLog				( char* pMsg, char* pFileName );
	void			WriteFormattedLog			( char* pFormat, ... );

public :
	BOOL			ConvertWideToMBCS			( WCHAR* pWideString, char* pBuffer, int nBufferSize );
	BOOL			ConvertMBCSToWide			( char* pMBCSString, WCHAR* pBuffer, int nBufferSize );

public :
	void			SetMsgCallBack				( fnMsgCallBack fnCallBack ) { m_fnMsgCallBack = fnCallBack; }
};


namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	CWebzenUtility & Loger();

	//-----------------------------------------------------------------------
}
*/


#endif