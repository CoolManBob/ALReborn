// AgcaUIManager2.h: interface for the AgcaUIManager2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCAUIMANAGER2_H__0EE56BEA_AA24_4FCA_B7AD_59CD1FCCFCAC__INCLUDED_)
#define AFX_AGCAUIMANAGER2_H__0EE56BEA_AA24_4FCA_B7AD_59CD1FCCFCAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgcdUIManager2.h"

class AgcaUIManager2  : public ApAdmin
{
public:
	AgcaUIManager2( void ) {	}
	virtual ~AgcaUIManager2( void ) {	}

	AgcdUI*					AddUI					( AgcdUI *pcsUI );
	AgcdUI*					GetUI					( INT32 lID );
	AgcdUI*					GetUI					( CHAR *szName );
	BOOL					RemoveUI				( INT32 lID, CHAR *szName );
};

#endif // !defined(AFX_AGCAUIMANAGER2_H__0EE56BEA_AA24_4FCA_B7AD_59CD1FCCFCAC__INCLUDED_)
